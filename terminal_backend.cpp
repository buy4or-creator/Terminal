#include "terminal_backend.h"
#include <QHash>
#include <QPointer>
#include <QSettings>
#include <QThread>
#include <atomic>
#include <vector>

#ifdef Q_OS_WIN
#include <windows.h>
#include <shellapi.h>

#ifndef PROC_THREAD_ATTRIBUTE_PSEUDOCONSOLE
#define PROC_THREAD_ATTRIBUTE_PSEUDOCONSOLE 0x00020016
#endif

typedef VOID* HPCON;
typedef HRESULT (WINAPI *PFN_CreatePseudoConsole)(COORD, HANDLE, HANDLE, DWORD, HPCON*);
typedef HRESULT (WINAPI *PFN_ResizePseudoConsole)(HPCON, COORD);
typedef VOID (WINAPI *PFN_ClosePseudoConsole)(HPCON);

namespace {
struct ConPtyApi { PFN_CreatePseudoConsole create = nullptr; PFN_ResizePseudoConsole resize = nullptr; PFN_ClosePseudoConsole close = nullptr; bool loaded = false; };
ConPtyApi &conPtyApi() {
    static ConPtyApi api;
    if (!api.loaded) {
        api.loaded = true;
        if (HMODULE k32 = GetModuleHandleW(L"kernel32.dll")) {
            api.create = reinterpret_cast<PFN_CreatePseudoConsole>(GetProcAddress(k32, "CreatePseudoConsole"));
            api.resize = reinterpret_cast<PFN_ResizePseudoConsole>(GetProcAddress(k32, "ResizePseudoConsole"));
            api.close = reinterpret_cast<PFN_ClosePseudoConsole>(GetProcAddress(k32, "ClosePseudoConsole"));
        }
    }
    return api;
}

#endif
namespace {
QString ansiColor(int idx) {
    static const char *pal[16] = {
        "#1e1e2e","#f38ba8","#a6e3a1","#f9e2af","#89b4fa","#f5c2e7","#94e2d5","#cdd6f4",
        "#585b70","#f38ba8","#a6e3a1","#f9e2af","#89b4fa","#f5c2e7","#94e2d5","#ffffff"
    };
    if (idx >= 0 && idx < 16) return QString::fromLatin1(pal[idx]);
    if (idx >= 16 && idx <= 231) {
        int v = idx - 16, r = (v / 36) ? (v / 36) * 40 + 55 : 0, g = ((v / 6) % 6) ? ((v / 6) % 6) * 40 + 55 : 0, b = (v % 6) ? (v % 6) * 40 + 55 : 0;
        return QStringLiteral("#%1%2%3").arg(r,2,16,QLatin1Char('0')).arg(g,2,16,QLatin1Char('0')).arg(b,2,16,QLatin1Char('0'));
    }
    if (idx >= 232 && idx <= 255) { int g = (idx - 232) * 10 + 8; return QStringLiteral("#%1%2%3").arg(g,2,16,QLatin1Char('0')).arg(g,2,16,QLatin1Char('0')).arg(g,2,16,QLatin1Char('0')); }
    return QString();
}

struct Cell { QChar ch = QLatin1Char(' '); QString fg; QString bg; bool bold = false; };

struct VtScreen {
    int rows = 32, cols = 120, r = 0, c = 0;
    QString curFg, curBg; bool curBold = false;
    QVector<QVector<Cell>> grid;
    QVector<QVector<Cell>> savedGrid;
    QStringList scrollbackHtml;
    int savedR = 0, savedC = 0;
    bool altScreen = false;

    VtScreen(int w = 120, int h = 32) : rows(qMax(15, h)), cols(qMax(40, w)) { grid.fill(QVector<Cell>(cols), rows); }

    QString rowToHtml(const QVector<Cell> &row) const {
        int last = row.size() - 1;
        while (last >= 0 && row[last].ch == QLatin1Char(' ') && row[last].bg.isEmpty()) --last;
        if (last < 0) return QString();
        QString html, openStyle;
        for (int x = 0; x <= last; ++x) {
            const Cell &cell = row[x];
            QString style;
            if (!cell.fg.isEmpty()) style += QStringLiteral("color:%1;").arg(cell.fg);
            if (!cell.bg.isEmpty()) style += QStringLiteral("background-color:%1;").arg(cell.bg);
            if (cell.bold) style += QStringLiteral("font-weight:700;");
            if (style != openStyle) {
                if (!openStyle.isEmpty()) html += QStringLiteral("</span>");
                if (!style.isEmpty()) html += QStringLiteral("<span style=\"%1\">").arg(style);
                openStyle = style;
            }
            QChar ch = cell.ch;
            if (ch == QLatin1Char('&')) html += QStringLiteral("&amp;");
            else if (ch == QLatin1Char('<')) html += QStringLiteral("&lt;");
            else if (ch == QLatin1Char('>')) html += QStringLiteral("&gt;");
            else html += ch;
        }
        if (!openStyle.isEmpty()) html += QStringLiteral("</span>");
        return html;
    }

    void resize(int w, int h) {
        rows = qMax(15, h); cols = qMax(40, w);
        while (grid.size() < rows) grid.append(QVector<Cell>(cols));
        while (grid.size() > rows) scrollbackHtml.append(rowToHtml(grid.takeFirst()));
        for (int i = 0; i < rows; ++i) grid[i].resize(cols);
        r = qBound(0, r, rows - 1); c = qBound(0, c, cols - 1);
    }

    void scrollUp() {
        scrollbackHtml.append(rowToHtml(grid.takeFirst()));
        if (scrollbackHtml.size() > 400) scrollbackHtml.removeFirst();
        grid.append(QVector<Cell>(cols));
    }

    void putChar(QChar ch) {
        if (ch == QLatin1Char('\r')) { c = 0; return; }
        if (ch == QLatin1Char('\n')) { if (++r >= rows) { r = rows - 1; scrollUp(); } return; }
        if (ch == QLatin1Char('\b')) { if (c > 0) --c; return; }
        if (ch == QLatin1Char('\t')) { c = qMin(cols - 1, (c + 8) & ~7); return; }
        if (ch.unicode() < 32) return;
        if (c >= cols) { c = 0; if (++r >= rows) { r = rows - 1; scrollUp(); } }
        grid[r][c] = {ch, curFg, curBg, curBold}; ++c;
    }
    void applySgr(const QStringList &parts) {
        if (parts.isEmpty() || (parts.size() == 1 && parts[0].isEmpty())) { curFg.clear(); curBg.clear(); curBold = false; return; }
        for (int k = 0; k < parts.size(); ++k) {
            int code = parts[k].toInt();
            if (code == 0) { curFg.clear(); curBg.clear(); curBold = false; }
            else if (code == 1) curBold = true;
            else if (code == 22) curBold = false;
            else if (code == 39) curFg.clear();
            else if (code == 49) curBg.clear();
            else if (code >= 30 && code <= 37) curFg = ansiColor(code - 30 + (curBold ? 8 : 0));
            else if (code >= 90 && code <= 97) curFg = ansiColor(code - 90 + 8);
            else if (code >= 40 && code <= 47) curBg = ansiColor(code - 40);
            else if (code >= 100 && code <= 107) curBg = ansiColor(code - 100 + 8);
            else if ((code == 38 || code == 48) && k + 2 < parts.size() && parts[k + 1].toInt() == 5) {
                QString col = ansiColor(parts[k + 2].toInt());
                if (code == 38) curFg = col; else curBg = col;
                k += 2;
            } else if ((code == 38 || code == 48) && k + 4 < parts.size() && parts[k + 1].toInt() == 2) {
                QString col = QStringLiteral("#%1%2%3").arg(parts[k+2].toInt(),2,16,QLatin1Char('0')).arg(parts[k+3].toInt(),2,16,QLatin1Char('0')).arg(parts[k+4].toInt(),2,16,QLatin1Char('0'));
                if (code == 38) curFg = col; else curBg = col;
                k += 4;
            }
        }
    }

    QString feed(const QString &raw) {
        int i = 0, n = raw.size();
        while (i < n) {
            QChar ch = raw.at(i);
            if (ch == QLatin1Char('\x1b')) {
                if (i + 1 < n && raw.at(i + 1) == QLatin1Char(']')) {
                    i += 2; while (i < n) { if (raw.at(i) == QLatin1Char('\x07')) { ++i; break; } if (raw.at(i) == QLatin1Char('\x1b') && i + 1 < n && raw.at(i + 1) == QLatin1Char('\\')) { i += 2; break; } ++i; } continue;
                }
                if (i + 1 < n && raw.at(i + 1) == QLatin1Char('[')) {
                    i += 2; QString params;
                    while (i < n && (raw.at(i).isDigit() || raw.at(i) == QLatin1Char(';') || raw.at(i) == QLatin1Char('?'))) { params.append(raw.at(i)); ++i; }
                    if (i < n) {
                        QChar cmd = raw.at(i++); QString cp = params; cp.remove(QLatin1Char('?'));
                        const QStringList parts = cp.split(QLatin1Char(';'));
                        int p1 = parts.value(0).toInt(), p2 = parts.value(1).toInt();
                                                if (cmd == QLatin1Char('h') && cp == QStringLiteral("1049")) {
                            if (!altScreen) { savedGrid = grid; savedR = r; savedC = c; altScreen = true; grid.fill(QVector<Cell>(cols), rows); r = 0; c = 0; }
                        } else if (cmd == QLatin1Char('l') && cp == QStringLiteral("1049")) {
                            if (altScreen) { grid = savedGrid; r = qBound(0, savedR, rows - 1); c = qBound(0, savedC, cols - 1); altScreen = false; }
                        } else if (cmd == QLatin1Char('m')) applySgr(parts);
                        else if (cmd == QLatin1Char('H') || cmd == QLatin1Char('f')) { r = qBound(0, (p1 > 0 ? p1 - 1 : 0), rows - 1); c = qBound(0, (p2 > 0 ? p2 - 1 : 0), cols - 1); }
                        else if (cmd == QLatin1Char('A')) r = qMax(0, r - qMax(1, p1));
                        else if (cmd == QLatin1Char('B')) r = qMin(rows - 1, r + qMax(1, p1));
                        else if (cmd == QLatin1Char('C')) c = qMin(cols - 1, c + qMax(1, p1));
                        else if (cmd == QLatin1Char('D')) c = qMax(0, c - qMax(1, p1));
                        else if (cmd == QLatin1Char('G')) c = qBound(0, (p1 > 0 ? p1 - 1 : 0), cols - 1);
                        else if (cmd == QLatin1Char('J')) {
                            if (p1 == 2 || p1 == 3) { scrollbackHtml.clear(); for (int k = 0; k < rows; ++k) grid[k] = QVector<Cell>(cols); r = 0; c = 0; }
                            else if (p1 == 0) { for (int x = c; x < cols; ++x) grid[r][x] = Cell{}; for (int y = r + 1; y < rows; ++y) grid[y] = QVector<Cell>(cols); }
                        }                         else if (cmd == QLatin1Char('@')) {
                            int cnt = qMax(1, p1);
                            for (int s = 0; s < cnt && c < cols; ++s) { grid[r].insert(c, Cell{}); grid[r].resize(cols); }
                        } else if (cmd == QLatin1Char('P')) {
                            int cnt = qMax(1, p1);
                            for (int s = 0; s < cnt && c < cols; ++s) { grid[r].remove(c); grid[r].append(Cell{}); }
                        } else if (cmd == QLatin1Char('K')) {
                            if (p1 == 2) grid[r] = QVector<Cell>(cols);
                            else if (p1 == 1) { for (int x = 0; x <= c && x < cols; ++x) grid[r][x] = Cell{}; }
                            else { for (int x = c; x < cols; ++x) grid[r][x] = Cell{}; }
                        }
                    }
                    continue;
                }
                i += 2; continue;
            }
            putChar(ch); ++i;
        }
        QStringList out = altScreen ? QStringList{} : scrollbackHtml;         int lastRow = r;
        for (int y = rows - 1; y >= 0; --y) {
            bool empty = true; for (const Cell &cell : grid[y]) if (cell.ch != QLatin1Char(' ') || !cell.bg.isEmpty()) { empty = false; break; }
            if (!empty) { lastRow = qMax(lastRow, y); break; }
        }
        for (int y = 0; y <= lastRow; ++y) {
            if (y == r && c >= 0 && c < cols) {
                                QSettings cs(QStringLiteral("ConsoleV1"), QStringLiteral("TerminalApp"));
                const QString cStyle = cs.value(QStringLiteral("cursorStyle"), QStringLiteral("block")).toString();
                const QString cColor = cs.value(QStringLiteral("cursorColor"), QStringLiteral("#a78bfa")).toString();
                QVector<Cell> rowWithCursor = grid[y];
                if (cStyle == QStringLiteral("underline")) {
                    rowWithCursor[c].fg = cColor;
                    rowWithCursor[c].bold = true;
                    if (rowWithCursor[c].ch == QLatin1Char(' ')) rowWithCursor[c].ch = QLatin1Char('_');
                } else if (cStyle == QStringLiteral("bar")) {
                    rowWithCursor[c].fg = cColor;
                    rowWithCursor[c].bold = true;
                    if (rowWithCursor[c].ch == QLatin1Char(' ')) rowWithCursor[c].ch = QChar(0x258F);
                } else {
                    rowWithCursor[c].bg = cColor;
                    rowWithCursor[c].fg = QStringLiteral("#0b0f17");
                    rowWithCursor[c].bold = true;
                }
                out << rowToHtml(rowWithCursor);
            } else {
                out << rowToHtml(grid[y]);
            }
        }
        return QStringLiteral("<pre style=\"margin:0;white-space:pre;\">") + out.join(QLatin1Char('\n')) + QStringLiteral("</pre>");
    }
};
#ifdef Q_OS_WIN
struct ConPtyState { HPCON hPC = nullptr; HANDLE hPipeInWrite = INVALID_HANDLE_VALUE, hPipeOutRead = INVALID_HANDLE_VALUE, hProcess = INVALID_HANDLE_VALUE; std::atomic_bool running{false}; VtScreen screen; };
static QHash<const TerminalBackend *, ConPtyState *> g_states;
#elif defined(Q_OS_LINUX)
#include <pty.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/wait.h>
#include <signal.h>
struct LinuxPtyState { int masterFd = -1; pid_t pid = -1; std::atomic_bool running{false}; VtScreen screen; };
static QHash<const TerminalBackend *, LinuxPtyState *> g_linuxStates;
#endif
}
TerminalBackend::TerminalBackend(QObject *parent) : QObject(parent) {}
TerminalBackend::~TerminalBackend() { stop(); }
bool TerminalBackend::usingFallback() const { return false; }
bool TerminalBackend::isRunning() const {
#ifdef Q_OS_WIN
    if (auto *st = g_states.value(this, nullptr); st && st->running.load()) return true;
#elif defined(Q_OS_LINUX)
    if (auto *st = g_linuxStates.value(this, nullptr); st && st->running.load()) return true;
#endif
    return process_ && process_->state() != QProcess::NotRunning;
}
QProcess *TerminalBackend::process() const { return process_; }
bool TerminalBackend::start(const QString &shell, const QSize &terminalSize, bool elevated) {
    stop();
#ifdef Q_OS_WIN
    const bool ps = shell.compare(QStringLiteral("powershell"), Qt::CaseInsensitive) == 0;
    const QString exe = ps ? QStringLiteral("powershell.exe") : QStringLiteral("cmd.exe");
    const QString args = ps ? QStringLiteral("-NoLogo -NoExit") : QStringLiteral("/K prompt %USERNAME%@%COMPUTERNAME%:$P$G ");
    if (elevated) { ShellExecuteW(nullptr, L"runas", reinterpret_cast<LPCWSTR>(exe.utf16()), reinterpret_cast<LPCWSTR>(args.utf16()), nullptr, SW_SHOWNORMAL); elevated_ = true; return true; }
    auto &api = conPtyApi();
    HANDLE hInR = INVALID_HANDLE_VALUE, hInW = INVALID_HANDLE_VALUE, hOutR = INVALID_HANDLE_VALUE, hOutW = INVALID_HANDLE_VALUE;
    if (api.create && CreatePipe(&hInR, &hInW, nullptr, 0) && CreatePipe(&hOutR, &hOutW, nullptr, 0)) {
        COORD size{static_cast<SHORT>(qMax(40, terminalSize.width())), static_cast<SHORT>(qMax(15, terminalSize.height()))};
        HPCON hPC = nullptr;
        if (SUCCEEDED(api.create(size, hInR, hOutW, 0, &hPC))) {
            SIZE_T attrSize = 0; InitializeProcThreadAttributeList(nullptr, 1, 0, &attrSize); std::vector<BYTE> attrBuf(attrSize);
            auto *attrList = reinterpret_cast<LPPROC_THREAD_ATTRIBUTE_LIST>(attrBuf.data()); InitializeProcThreadAttributeList(attrList, 1, 0, &attrSize);
            UpdateProcThreadAttribute(attrList, 0, PROC_THREAD_ATTRIBUTE_PSEUDOCONSOLE, hPC, sizeof(HPCON), nullptr, nullptr);
            STARTUPINFOEXW si{}; si.StartupInfo.cb = sizeof(STARTUPINFOEXW); si.StartupInfo.dwFlags = STARTF_USESTDHANDLES;
            si.StartupInfo.hStdInput = INVALID_HANDLE_VALUE; si.StartupInfo.hStdOutput = INVALID_HANDLE_VALUE; si.StartupInfo.hStdError = INVALID_HANDLE_VALUE; si.lpAttributeList = attrList;
            PROCESS_INFORMATION pi{}; QString fullCmd = ps ? QStringLiteral("powershell.exe -NoLogo -NoExit") : QStringLiteral("cmd.exe");
            std::vector<wchar_t> cmdBuf(fullCmd.size() + 1, 0); fullCmd.toWCharArray(cmdBuf.data());
            BOOL ok = CreateProcessW(nullptr, cmdBuf.data(), nullptr, nullptr, FALSE, EXTENDED_STARTUPINFO_PRESENT, nullptr, nullptr, &si.StartupInfo, &pi);
            DeleteProcThreadAttributeList(attrList); CloseHandle(hInR); CloseHandle(hOutW);
            if (ok) {
                CloseHandle(pi.hThread); auto *st = new ConPtyState{hPC, hInW, hOutR, pi.hProcess, {true}, VtScreen(size.X, size.Y)}; g_states.insert(this, st);
                QPointer<TerminalBackend> guard(this);
                QThread *reader = QThread::create([guard, st] {
                    QByteArray buf(4096, 0); DWORD readBytes = 0;
                    while (st->running.load() && ReadFile(st->hPipeOutRead, buf.data(), buf.size(), &readBytes, nullptr) && readBytes > 0) {
                        QByteArray raw = buf.left(static_cast<int>(readBytes));
                        QString html = st->screen.feed(QString::fromUtf8(raw));
                        if (guard) QMetaObject::invokeMethod(guard, [guard, raw, html] { if (guard) { emit guard->outputReady(raw); emit guard->outputText(html); } }, Qt::QueuedConnection);
                    }
                    if (guard) QMetaObject::invokeMethod(guard, [guard] { if (guard) emit guard->processFinished(0); }, Qt::QueuedConnection);
                });
                connect(reader, &QThread::finished, reader, &QObject::deleteLater); reader->start();
                if (!ps) { DWORD w=0; const char* init="prompt %USERNAME%@%COMPUTERNAME%:$P$G \r\n"; WriteFile(hInW, init, (DWORD)strlen(init), &w, nullptr); FlushFileBuffers(hInW); }
                return true;
            }
            if (api.close) api.close(hPC);
        }
        CloseHandle(hInW); CloseHandle(hOutR);
    }
#elif defined(Q_OS_LINUX)
    Q_UNUSED(shell); Q_UNUSED(elevated);
    struct winsize ws{};
    ws.ws_col = static_cast<unsigned short>(qMax(40, terminalSize.width()));
    ws.ws_row = static_cast<unsigned short>(qMax(15, terminalSize.height()));
    int masterFd = -1;
    pid_t pid = forkpty(&masterFd, nullptr, nullptr, &ws);
    if (pid < 0) return false;
    if (pid == 0) {
        setenv("TERM", "xterm-256color", 1);
        const char *sh = getenv("SHELL");
        if (!sh || !*sh) sh = "/bin/bash";
        execlp(sh, sh, "-l", nullptr);
        _exit(127);
    }
    auto *st = new LinuxPtyState{masterFd, pid, {true}, VtScreen(ws.ws_col, ws.ws_row)};
    g_linuxStates.insert(this, st);
    QPointer<TerminalBackend> guard(this);
    QThread *reader = QThread::create([guard, st] {
        QByteArray buf(4096, 0);
        while (st->running.load()) {
            ssize_t n = read(st->masterFd, buf.data(), buf.size());
            if (n <= 0) break;
            QByteArray raw = buf.left(static_cast<int>(n));
            QString html = st->screen.feed(QString::fromUtf8(raw));
            if (guard) QMetaObject::invokeMethod(guard, [guard, raw, html] {
                if (guard) { emit guard->outputReady(raw); emit guard->outputText(html); }
            }, Qt::QueuedConnection);
        }
        if (guard) QMetaObject::invokeMethod(guard, [guard] { if (guard) emit guard->processFinished(0); }, Qt::QueuedConnection);
    });
    connect(reader, &QThread::finished, reader, &QObject::deleteLater);
    reader->start();
    return true;
#endif
    return false;
}
void TerminalBackend::writeInput(const QByteArray &data) {
#ifdef Q_OS_LINUX
    if (auto *st = g_linuxStates.value(this, nullptr); st && st->running.load() && st->masterFd >= 0) {
        ssize_t ignored = write(st->masterFd, data.constData(), data.size()); Q_UNUSED(ignored); return;
    }
#endif
#ifdef Q_OS_WIN
    if (auto *st = g_states.value(this, nullptr); st && st->running.load() && st->hPipeInWrite != INVALID_HANDLE_VALUE) {
        DWORD written = 0; WriteFile(st->hPipeInWrite, data.constData(), static_cast<DWORD>(data.size()), &written, nullptr); FlushFileBuffers(st->hPipeInWrite); return;
    }
#endif
}
void TerminalBackend::sendCommand(const QString &command) { writeInput((command + QStringLiteral("\r")).toUtf8()); }
void TerminalBackend::resizeTerminal(const QSize &s) {
#ifdef Q_OS_LINUX
    if (auto *st = g_linuxStates.value(this, nullptr); st && st->masterFd >= 0) {
        st->screen.resize(s.width(), s.height());
        struct winsize ws{}; ws.ws_col = static_cast<unsigned short>(qMax(40, s.width())); ws.ws_row = static_cast<unsigned short>(qMax(15, s.height()));
        ioctl(st->masterFd, TIOCSWINSZ, &ws); return;
    }
#endif
#ifdef Q_OS_WIN
    if (auto *st = g_states.value(this, nullptr); st && st->hPC && conPtyApi().resize) { st->screen.resize(s.width(), s.height()); conPtyApi().resize(st->hPC, {static_cast<SHORT>(qMax(40, s.width())), static_cast<SHORT>(qMax(15, s.height()))}); }
#else
    Q_UNUSED(s);
#endif
}
void TerminalBackend::stop() {
#ifdef Q_OS_WIN
    if (auto *st = g_states.take(this)) {
        st->running.store(false); if (st->hPC && conPtyApi().close) conPtyApi().close(st->hPC);
        if (st->hPipeInWrite != INVALID_HANDLE_VALUE) CloseHandle(st->hPipeInWrite); if (st->hPipeOutRead != INVALID_HANDLE_VALUE) CloseHandle(st->hPipeOutRead);
        if (st->hProcess != INVALID_HANDLE_VALUE) { TerminateProcess(st->hProcess, 0); CloseHandle(st->hProcess); } delete st;
    }
#elif defined(Q_OS_LINUX)
    if (auto *st = g_linuxStates.take(this)) {
        st->running.store(false);
        if (st->masterFd >= 0) close(st->masterFd);
        if (st->pid > 0) { kill(st->pid, SIGTERM); waitpid(st->pid, nullptr, WNOHANG); }
        delete st;
    }
#endif
}