#ifndef TERMINAL_BACKEND_H
#define TERMINAL_BACKEND_H

#include <QObject>
#include <QProcess>
#include <QSize>
#include <QString>
#include <memory>

class TerminalBackend final : public QObject
{
    Q_OBJECT
public:
    explicit TerminalBackend(QObject *parent = nullptr);
    ~TerminalBackend() override;
    bool start(const QString &shell, const QSize &terminalSize = QSize(120, 30), bool elevated = false);
    void writeInput(const QByteArray &data);
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
    QProcess *process_ = nullptr;
    bool elevated_ = false;
};

#endif
