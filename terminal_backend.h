#ifndef TERMINAL_BACKEND_H
#define TERMINAL_BACKEND_H

#include <QObject>
#include <QProcess>
#include <QSize>
#include <QString>
#include <QStringList>
#include <atomic>
#include <memory>
#include <thread>

class TerminalBackend final : public QObject
{
    Q_OBJECT
public:
    explicit TerminalBackend(QObject *parent = nullptr);
    ~TerminalBackend() override;
    bool start(const QString &shell, const QSize &terminalSize = QSize(120, 32), bool elevated = false);
    bool startCommand(const QString &program, const QStringList &arguments,
                      const QSize &terminalSize = QSize(120, 32), bool elevated = false);
    void writeInput(const QByteArray &data);
    void writeData(const QByteArray &data) { writeInput(data); }
    void sendCommand(const QString &command);
    void resizeTerminal(const QSize &terminalSize);
    void stop();
    bool isRunning() const;
    bool usingFallback() const;
    QProcess *process() const;

signals:
    void outputReady(const QByteArray &data);
    void outputText(const QString &text);
    void errorOccurred(const QString &message);
    void processFinished(int exitCode);

private:
    struct Impl;
    bool startFallback(const QString &program, const QStringList &arguments, const QString &reason);
    std::unique_ptr<Impl> impl_;
};

#endif
