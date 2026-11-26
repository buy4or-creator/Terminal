#include "terminal_backend.h"

#include <QMetaObject>
#include <QPointer>
#include <QProcessEnvironment>
#include <cstring>
#include <vector>

#ifdef Q_OS_WIN
#include <windows.h>
#include <shellapi.h>
#ifndef PROC_THREAD_ATTRIBUTE_PSEUDOCONSOLE
#define PROC_THREAD_ATTRIBUTE_PSEUDOCONSOLE 0x00020016
#endif
#ifndef _HPCON_DEFINED
using HPCON = HANDLE;
#define _HPCON_DEFINED
#endif
#else
#include <cerrno>
#include <csignal>
#include <fcntl.h>
#include <pty.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#endif

struct TerminalBackend::Impl {
    QProcess *fallback = nullptr;
    QProcess *process_ = nullptr;
    std::atomic_bool running{false};
    bool fallbackMode = false;
    bool elevated = false;
#ifdef Q_OS_WIN
    using CreateFn = HRESULT (WINAPI *)(COORD, HANDLE, HANDLE, DWORD, HPCON *);
    using ResizeFn = HRESULT (WINAPI *)(HPCON, COORD);
    using CloseFn = void (WINAPI *)(HPCON);
    HPCON hpc = nullptr;
    CreateFn create = nullptr;
    ResizeFn resize = nullptr;
    CloseFn close = nullptr;
    HANDLE inputWrite = INVALID_HANDLE_VALUE;
    HANDLE outputRead = INVALID_HANDLE_VALUE;
    HANDLE processHandle = nullptr;
#else
    int master = -1;
    pid_t child = -1;
#endif
    std::thread reader;
    std::atomic_bool stopReader{false};
};

namespace {
#ifdef Q_OS_WIN
template <typename Function>
Function resolveFunction(HMODULE module, const char *name)
{
    Function result = nullptr;
    FARPROC address = GetProcAddress(module, name);
    static_assert(sizeof(result) == sizeof(address), "Function pointer size mismatch");
    std::memcpy(&result, &address, sizeof(result));
    return result;
}
void closeHandle(HANDLE &handle)
{
    if (handle != nullptr && handle != INVALID_HANDLE_VALUE)
        CloseHandle(handle);
    handle = INVALID_HANDLE_VALUE;
}
#endif

QString executableFor(const QString &shell)
{
#ifdef Q_OS_WIN
    return shell.compare(QStringLiteral("powershell"), Qt::CaseInsensitive) == 0
        ? QStringLiteral("powershell.exe")
        : QStringLiteral("cmd.exe");
#else
    if (shell.compare(QStringLiteral("powershell"), Qt::CaseInsensitive) == 0)
        return QStringLiteral("pwsh");
    if (shell.compare(QStringLiteral("bash"), Qt::CaseInsensitive) == 0)
        return QStringLiteral("/bin/bash");
    if (shell.compare(QStringLiteral("zsh"), Qt::CaseInsensitive) == 0)
        return QStringLiteral("/bin/zsh");
    return qEnvironmentVariable("SHELL", QStringLiteral("/bin/bash"));
#endif
}

QStringList argumentsFor(const QString &shell)
{
#ifdef Q_OS_WIN
    if (shell.compare(QStringLiteral("powershell"), Qt::CaseInsensitive) == 0) {
        return {
            QStringLiteral("-NoLogo"),
            QStringLiteral("-NoExit"),
            QStringLiteral("-Command"),
            QStringLiteral("[Console]::InputEncoding=[System.Text.Encoding]::UTF8;[Console]::OutputEncoding=[System.Text.Encoding]::UTF8;")
        };
    }
    return {QStringLiteral("/Q"), QStringLiteral("/K")};
#else
    Q_UNUSED(shell);
    return {QStringLiteral("-i")};
#endif
}

QByteArray lineEnding()
{
#ifdef Q_OS_WIN
    return QByteArray("\r\n");
#else
    return QByteArray("\n");
#endif
}

#ifndef Q_OS_WIN
void applyPtyEnvironment()
{
    setenv("TERM", "xterm-256color", 1);
    setenv("COLORTERM", "truecolor", 1);
    setenv("LANG", "C.UTF-8", 0);
}
#endif
} // namespace

TerminalBackend::TerminalBackend(QObject *parent)
    : QObject(parent)
    , impl_(std::make_unique<Impl>())
{
}

TerminalBackend::~TerminalBackend()
{
    stop();
}

bool TerminalBackend::usingFallback() const
{
    return impl_->fallbackMode;
}

bool TerminalBackend::isRunning() const
{
    return impl_->running.load();
}

QProcess *TerminalBackend::process() const
{
    return impl_->process_;
}

bool TerminalBackend::start(const QString &shell, const QSize &terminalSize, bool elevated)
{
    return startCommand(executableFor(shell), argumentsFor(shell), terminalSize, elevated);
}

bool TerminalBackend::startCommand(const QString &program, const QStringList &arguments,
                                   const QSize &terminalSize, bool elevated)
{
    stop();
#ifdef Q_OS_WIN
    if (elevated) {
        const QString parameters = arguments.join(QLatin1Char(' '));
        const HINSTANCE result = ShellExecuteW(
            nullptr,
            L"runas",
            reinterpret_cast<LPCWSTR>(program.utf16()),
            reinterpret_cast<LPCWSTR>(parameters.utf16()),
            nullptr,
            SW_SHOWNORMAL);
        const INT_PTR code = reinterpret_cast<INT_PTR>(result);
        if (code <= 32)
            emit errorOccurred(QStringLiteral("ShellExecute elevated failed (%1)").arg(code));
        return code > 32;
    }

    HMODULE kernel = GetModuleHandleW(L"kernel32.dll");
    impl_->create = resolveFunction<Impl::CreateFn>(kernel, "CreatePseudoConsole");
    impl_->resize = resolveFunction<Impl::ResizeFn>(kernel, "ResizePseudoConsole");
    impl_->close = resolveFunction<Impl::CloseFn>(kernel, "ClosePseudoConsole");
    if (!impl_->create || !impl_->resize || !impl_->close)
        return startFallback(program, arguments, QStringLiteral("ConPTY API unavailable"));

    HANDLE inRead = INVALID_HANDLE_VALUE, inWrite = INVALID_HANDLE_VALUE;
    HANDLE outRead = INVALID_HANDLE_VALUE, outWrite = INVALID_HANDLE_VALUE;
    if (!CreatePipe(&inRead, &inWrite, nullptr, 0))
        return startFallback(program, arguments, QStringLiteral("CreatePipe input error %1").arg(GetLastError()));
    if (!CreatePipe(&outRead, &outWrite, nullptr, 0)) {
        const DWORD error = GetLastError();
        closeHandle(inRead);
        closeHandle(inWrite);
        return startFallback(program, arguments, QStringLiteral("CreatePipe output error %1").arg(error));
    }

    COORD size{
        static_cast<SHORT>(qBound(2, terminalSize.width(), 300)),
        static_cast<SHORT>(qBound(2, terminalSize.height(), 120))};
    const HRESULT hr = impl_->create(size, inRead, outWrite, 0, &impl_->hpc);
    if (FAILED(hr)) {
        const DWORD error = GetLastError();
        closeHandle(inRead);
        closeHandle(inWrite);
        closeHandle(outRead);
        closeHandle(outWrite);
        return startFallback(
            program,
            arguments,
            QStringLiteral("CreatePseudoConsole HRESULT %1 / error %2")
                .arg(quint32(hr), 8, 16, QLatin1Char('0'))
                .arg(error));
    }

    SIZE_T attrSize = 0;
    InitializeProcThreadAttributeList(nullptr, 1, 0, &attrSize);
    std::vector<BYTE> attrBuffer(attrSize);
    auto *attrs = reinterpret_cast<LPPROC_THREAD_ATTRIBUTE_LIST>(attrBuffer.data());
    if (!InitializeProcThreadAttributeList(attrs, 1, 0, &attrSize)
        || !UpdateProcThreadAttribute(attrs, 0, PROC_THREAD_ATTRIBUTE_PSEUDOCONSOLE, impl_->hpc, sizeof(HPCON), nullptr, nullptr)) {
        const DWORD error = GetLastError();
        if (attrs)
            DeleteProcThreadAttributeList(attrs);
        closeHandle(inRead);
        closeHandle(inWrite);
        closeHandle(outRead);
        closeHandle(outWrite);
        impl_->close(impl_->hpc);
        impl_->hpc = nullptr;
        return startFallback(program, arguments, QStringLiteral("ConPTY attribute error %1").arg(error));
    }

    QString command = QLatin1Char('"') + program + QLatin1Char('"');
    for (const QString &arg : arguments)
        command += QStringLiteral(" \"") + arg + QLatin1Char('"');

    std::vector<wchar_t> mutableCommand(command.size() + 1, L'\0');
    command.toWCharArray(mutableCommand.data());

    STARTUPINFOEXW si{};
    si.StartupInfo.cb = sizeof(STARTUPINFOEXW);
    si.lpAttributeList = attrs;
    PROCESS_INFORMATION pi{};
    const BOOL ok = CreateProcessW(
        nullptr,
        mutableCommand.data(),
        nullptr,
        nullptr,
        FALSE,
        EXTENDED_STARTUPINFO_PRESENT,
        nullptr,
        nullptr,
        &si.StartupInfo,
        &pi);
    const DWORD createError = ok ? 0 : GetLastError();
    DeleteProcThreadAttributeList(attrs);
    if (!ok) {
        closeHandle(inRead);
        closeHandle(inWrite);
        closeHandle(outRead);
        closeHandle(outWrite);
        impl_->close(impl_->hpc);
        impl_->hpc = nullptr;
        return startFallback(program, arguments, QStringLiteral("CreateProcessW error %1").arg(createError));
    }

    closeHandle(inRead);
    closeHandle(outWrite);
    CloseHandle(pi.hThread);
    impl_->inputWrite = inWrite;
    impl_->outputRead = outRead;
    impl_->processHandle = pi.hProcess;
#else
    if (elevated) {
        emit errorOccurred(QStringLiteral("Elevated shell is Windows-only."));
        return false;
    }

    int master = -1;
    int execStatus[2] = {-1, -1};
    if (pipe(execStatus) != 0)
        return startFallback(program, arguments, QStringLiteral("exec status pipe error %1").arg(errno));
    if (fcntl(execStatus[1], F_SETFD, FD_CLOEXEC) == -1) {
        const int error = errno;
        close(execStatus[0]);
        close(execStatus[1]);
        return startFallback(program, arguments, QStringLiteral("exec status setup error %1").arg(error));
    }
    winsize ws{};
    ws.ws_col = static_cast<unsigned short>(qBound(2, terminalSize.width(), 300));
    ws.ws_row = static_cast<unsigned short>(qBound(2, terminalSize.height(), 120));
    const pid_t pid = forkpty(&master, nullptr, nullptr, &ws);
    if (pid < 0) {
        const int error = errno;
        close(execStatus[0]);
        close(execStatus[1]);
        return startFallback(program, arguments, QStringLiteral("forkpty error %1").arg(error));
    }

    if (pid == 0) {
        close(execStatus[0]);
        applyPtyEnvironment();
        std::vector<QByteArray> encodedArguments;
        encodedArguments.reserve(static_cast<size_t>(arguments.size()) + 1);
        encodedArguments.push_back(program.toLocal8Bit());
        for (const QString &argument : arguments)
            encodedArguments.push_back(argument.toLocal8Bit());
        std::vector<char *> argv;
        argv.reserve(encodedArguments.size() + 1);
        for (QByteArray &argument : encodedArguments)
            argv.push_back(argument.data());
        argv.push_back(nullptr);
        execvp(argv.front(), argv.data());
        const int error = errno;
        (void)::write(execStatus[1], &error, sizeof(error));
        _exit(127);
    }
    close(execStatus[1]);
    int execError = 0;
    ssize_t statusBytes;
    do {
        statusBytes = ::read(execStatus[0], &execError, sizeof(execError));
    } while (statusBytes < 0 && errno == EINTR);
    close(execStatus[0]);
    if (statusBytes > 0 || statusBytes < 0) {
        close(master);
        waitpid(pid, nullptr, 0);
        const int error = statusBytes > 0 ? execError : errno;
        return startFallback(program, arguments, QStringLiteral("could not execute %1 (error %2)").arg(program).arg(error));
    }
    impl_->master = master;
    impl_->child = pid;
#endif

    impl_->fallbackMode = false;
    impl_->stopReader.store(false);
    impl_->running.store(true);
    QPointer<TerminalBackend> guard(this);
    Impl *state = impl_.get();
    state->reader = std::thread([guard, state] {
        char buffer[4096];
#ifdef Q_OS_WIN
        while (!state->stopReader.load()) {
            DWORD count = 0;
            if (!ReadFile(state->outputRead, buffer, sizeof(buffer) - 1, &count, nullptr) || count == 0)
                break;
            const QByteArray bytes(buffer, static_cast<qsizetype>(count));
#else
        while (!state->stopReader.load()) {
            const ssize_t count = ::read(state->master, buffer, sizeof(buffer) - 1);
            if (count <= 0)
                break;
            const QByteArray bytes(buffer, static_cast<qsizetype>(count));
#endif
            if (guard) {
                QMetaObject::invokeMethod(
                    guard.data(),
                    [guard, bytes] {
                        if (!guard)
                            return;
                        emit guard->outputReady(bytes);
                        emit guard->outputText(QString::fromUtf8(bytes));
                    },
                    Qt::QueuedConnection);
            }
        }
        state->running.store(false);
        if (guard) {
            QMetaObject::invokeMethod(
                guard.data(),
                [guard, state] {
                    if (!guard)
                        return;
#ifdef Q_OS_WIN
                    DWORD exitCode = 0;
                    if (state->processHandle)
                        GetExitCodeProcess(state->processHandle, &exitCode);
                    emit guard->processFinished(static_cast<int>(exitCode));
#else
                    int status = 0;
                    if (state->child > 0)
                        waitpid(state->child, &status, WNOHANG);
                    emit guard->processFinished(WIFEXITED(status) ? WEXITSTATUS(status) : -1);
#endif
                },
                Qt::QueuedConnection);
        }
    });
    return true;
}

bool TerminalBackend::startFallback(const QString &program, const QStringList &arguments, const QString &reason)
{
    if (impl_->fallback) {
        impl_->fallback->kill();
        impl_->fallback->waitForFinished(500);
        impl_->fallback->deleteLater();
    }
    emit errorOccurred(QStringLiteral("PTY fallback: %1").arg(reason));

    impl_->fallback = new QProcess(this);
    impl_->process_ = impl_->fallback;
    impl_->fallbackMode = true;
    impl_->fallback->setProcessChannelMode(QProcess::MergedChannels);

    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    env.insert(QStringLiteral("TERM"), QStringLiteral("xterm-256color"));
    env.insert(QStringLiteral("COLORTERM"), QStringLiteral("truecolor"));
    impl_->fallback->setProcessEnvironment(env);
    impl_->fallback->setProgram(program);
    impl_->fallback->setArguments(arguments);

    connect(impl_->fallback, &QProcess::readyReadStandardOutput, this, [this] {
        const QByteArray bytes = impl_->fallback->readAllStandardOutput();
        emit outputReady(bytes);
        emit outputText(QString::fromUtf8(bytes));
    });
    connect(impl_->fallback, &QProcess::errorOccurred, this, [this](QProcess::ProcessError) {
        emit errorOccurred(impl_->fallback->errorString());
    });
    connect(
        impl_->fallback,
        qOverload<int, QProcess::ExitStatus>(&QProcess::finished),
        this,
        [this](int code, QProcess::ExitStatus) {
            impl_->running.store(false);
            emit processFinished(code);
        });

    impl_->fallback->start();
    const bool started = impl_->fallback->waitForStarted(2500);
    impl_->running.store(started);
    return started;
}

void TerminalBackend::writeInput(const QByteArray &data)
{
    if (impl_->fallbackMode) {
        if (impl_->fallback) {
            impl_->fallback->write(data);
            impl_->fallback->waitForBytesWritten(50);
        }
        return;
    }
#ifdef Q_OS_WIN
    if (impl_->inputWrite == INVALID_HANDLE_VALUE || impl_->inputWrite == nullptr) {
        emit errorOccurred(QStringLiteral("WriteFile error: invalid input handle"));
        return;
    }
    DWORD written = 0;
    if (!WriteFile(impl_->inputWrite, data.constData(), static_cast<DWORD>(data.size()), &written, nullptr))
        emit errorOccurred(QStringLiteral("WriteFile error: %1").arg(GetLastError()));
#else
    if (impl_->master < 0) {
        emit errorOccurred(QStringLiteral("PTY input descriptor invalid"));
        return;
    }
    if (::write(impl_->master, data.constData(), static_cast<size_t>(data.size())) < 0)
        emit errorOccurred(QStringLiteral("PTY write error: %1").arg(errno));
#endif
}

void TerminalBackend::sendCommand(const QString &command)
{
    writeInput(command.toUtf8() + lineEnding());
}

void TerminalBackend::resizeTerminal(const QSize &size)
{
#ifdef Q_OS_WIN
    if (impl_->hpc && impl_->resize) {
        COORD ws{
            static_cast<SHORT>(qBound(2, size.width(), 300)),
            static_cast<SHORT>(qBound(2, size.height(), 120))};
        impl_->resize(impl_->hpc, ws);
    }
#else
    if (impl_->master >= 0) {
        winsize ws{};
        ws.ws_col = static_cast<unsigned short>(qBound(2, size.width(), 300));
        ws.ws_row = static_cast<unsigned short>(qBound(2, size.height(), 120));
        ioctl(impl_->master, TIOCSWINSZ, &ws);
    }
#endif
}

void TerminalBackend::stop()
{
    impl_->stopReader.store(true);
#ifdef Q_OS_WIN
    if (impl_->inputWrite != INVALID_HANDLE_VALUE)
        CloseHandle(impl_->inputWrite);
    impl_->inputWrite = INVALID_HANDLE_VALUE;
    if (impl_->hpc && impl_->close)
        impl_->close(impl_->hpc);
    impl_->hpc = nullptr;
    if (impl_->reader.joinable())
        impl_->reader.join();
    if (impl_->outputRead != INVALID_HANDLE_VALUE)
        CloseHandle(impl_->outputRead);
    impl_->outputRead = INVALID_HANDLE_VALUE;
    if (impl_->processHandle) {
        TerminateProcess(impl_->processHandle, 0);
        CloseHandle(impl_->processHandle);
        impl_->processHandle = nullptr;
    }
#else
    if (impl_->master >= 0)
        close(impl_->master);
    impl_->master = -1;
    if (impl_->child > 0)
        kill(impl_->child, SIGHUP);
    if (impl_->reader.joinable())
        impl_->reader.join();
    if (impl_->child > 0)
        waitpid(impl_->child, nullptr, WNOHANG);
    impl_->child = -1;
#endif
    if (impl_->fallback) {
        impl_->fallback->kill();
        impl_->fallback->waitForFinished(500);
        impl_->fallback->deleteLater();
        impl_->fallback = nullptr;
    }
    impl_->process_ = nullptr;
    impl_->fallbackMode = false;
    impl_->running.store(false);
}
