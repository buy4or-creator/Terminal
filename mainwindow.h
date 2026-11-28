#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QGraphicsObject>
#include <QGraphicsPathItem>
#include <QByteArray>
#include <QHash>
#include <QMainWindow>
#include <QProcess>
#include <QPointer>
#include "terminal_backend.h"
#include <QStringList>
#include <QVector>
#include <functional>

class QGraphicsScene;
class QGraphicsView;
class QLabel;
class QLineEdit;
class QTextEdit;
class QPushButton;
class QResizeEvent;
class QShowEvent;
class QStackedWidget;
class QTreeWidget;
class QSpinBox;
class QWidget;
class QEvent;
class StarfieldWidget;

struct CommandBind {
    QString name;
    QString command;
    quint64 id = 0;
};

struct ConsoleSession {
    quint64 id = 0;
    bool closing = false;
    QProcess *process = nullptr;
    TerminalBackend *backend = nullptr;
    QWidget *page = nullptr;
    QWidget *tabRow = nullptr;
    QLineEdit *title = nullptr;
    QLineEdit *input = nullptr;
    QWidget *output = nullptr;
    QLabel *status = nullptr;
    QPushButton *tabButton = nullptr;
    QPushButton *sshExplorerButton = nullptr;
    QString shell;
    QString sshTarget;
    QString sshHost;
    QString sshLogin;
    QString sshPassword;
    int sshPort = 22;
    QStringList sshSftpOptions;
    QByteArray sshOutputTail;
    QString sshInputLine;
    QString remoteDirectory = QStringLiteral(".");
    int sshInputCursor = 0;
    bool sshActive = false;
    bool sshCandidate = false;
    QStringList history;
    int historyIndex = 0;
    QString historyDraft;
};

class MainWindow final : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void showEvent(QShowEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    bool nativeEvent(const QByteArray &eventType, void *message, qintptr *result) override;

private slots:
    void createConsole();
    void sendCommand();
    void readOutput();
    void createBind();
    void openSettings();
    void buildSettingsPage();
    void toggleDrawer();
    void addNote();

private:
    enum class SftpPendingAction { AwaitPrompt, Pwd, List, ChangeDirectory, Transfer };
    ConsoleSession *currentSession() const;
    ConsoleSession *findSession(quint64 id) const;
    void activateSession(ConsoleSession *session);
    void reorderSession(int from, int to);
    void closeSession(ConsoleSession *session);
    void renameSession(ConsoleSession *session);
    void startProcess(ConsoleSession *session, const QString &shell, bool elevated = false);
    void stopProcess(ConsoleSession *session);
    void appendOutput(ConsoleSession *session, const QString &text);
    void writeCommand(ConsoleSession *session, const QString &command);
    void launchShell(const QString &shell, bool elevated = false);
    void runBind(const CommandBind &bind);
    void refreshBinds();
    void updateSessionUi(ConsoleSession *session);
    void captureTerminalInput(ConsoleSession *session, const QByteArray &bytes);
    void inspectTerminalOutput(ConsoleSession *session, const QByteArray &bytes);
    void openSshExplorer(ConsoleSession *session);
    void refreshSshExplorer();
    void startSftpSession(ConsoleSession *session);
    void stopSftpSession(bool sendBye);
    void closeSshExplorer();
    void handleSftpOutput(TerminalBackend *backend, const QByteArray &bytes);
    void sendSftpCommand(const QString &command, SftpPendingAction action);
    void startSftpTransfer(const QStringList &commands, const QString &successStatus);
    int populateSftpListing(const QString &output, bool namesOnly = false);
    void handleSftpPrompt(const QString &response);
    void navigateSftpParent();
    QString selectedRemotePath() const;
    void insertRemotePath(const QString &path);
    void applySettings(const QString &accent, int alpha, int fontSize, int scale, const QString &theme, bool stars, int windowOpacity);
    void loadSettings();
    void saveSettings() const;
    void loadNotes();
    void saveNotes() const;
    QGraphicsObject *createNote(const QPointF &position, const QString &title, const QString &body, const QString &id = QString());
    void finishNoteLink(QGraphicsItem *target);
    void deleteNote(QGraphicsObject *note);
    void deleteEdge(QGraphicsPathItem *edge);
    void setNotesVisible(bool visible);
    void animateOverlay(QWidget *overlay, bool open);
    void updateNavigationIndicators();
    void centerWindow();

    QWidget *root_ = nullptr;
    StarfieldWidget *starfield_ = nullptr;
    QWidget *nav_ = nullptr;
    QWidget *consolePage_ = nullptr;
    QWidget *notesPage_ = nullptr;
    QWidget *settingsPage_ = nullptr;
    QWidget *settingsOverlay_ = nullptr;
    QWidget *sshExplorerOverlay_ = nullptr;
    QWidget *sshExplorerCard_ = nullptr;
    QWidget *sshExplorerResizeGrip_ = nullptr;
    QWidget *drawer_ = nullptr;
    QWidget *drawerContent_ = nullptr;
    QWidget *bindHost_ = nullptr;
    QWidget *tabHost_ = nullptr;
    QStackedWidget *contentStack_ = nullptr;
    QStackedWidget *sessionStack_ = nullptr;
    QGraphicsScene *noteScene_ = nullptr;
    QGraphicsView *noteView_ = nullptr;
    QTreeWidget *sshExplorerTree_ = nullptr;
    QLineEdit *sshExplorerPath_ = nullptr;
    QLineEdit *sshExplorerHost_ = nullptr;
    QSpinBox *sshExplorerPort_ = nullptr;
    QLineEdit *sshExplorerLogin_ = nullptr;
    QLineEdit *sshExplorerPassword_ = nullptr;
    QPushButton *sshExplorerPasswordReveal_ = nullptr;
    QLabel *sshExplorerStatus_ = nullptr;
    ConsoleSession *sshExplorerSession_ = nullptr;
    ConsoleSession *sftpOwnerSession_ = nullptr;
    TerminalBackend *sftpBackend_ = nullptr;
    QPushButton *notesNavButton_ = nullptr;
    QPushButton *settingsNavButton_ = nullptr;
    QPushButton *settingsCloseButton_ = nullptr;
    QPushButton *drawerCollapseButton_ = nullptr;
    QLabel *clockLabel_ = nullptr;
    QVector<ConsoleSession *> sessions_;
    QVector<CommandBind> binds_;
    quint64 nextSessionId_ = 1;
    quint64 nextBindId_ = 1;
    SftpPendingAction sftpPendingAction_ = SftpPendingAction::AwaitPrompt;
    QByteArray sftpBuffer_;
    QString sftpResponseError_;
    QStringList sftpTransferCommands_;
    QString sftpTransferSuccessStatus_;
    bool sftpReady_ = false;
    bool sftpConnected_ = false;
    bool sftpPasswordSent_ = false;
    bool sftpListingFallbackTried_ = false;
    bool sshExplorerResizing_ = false;
    QPoint sshExplorerResizeStartGlobal_;
    QSize sshExplorerResizeStartSize_;
    QHash<QString, QGraphicsObject *> noteIds_;
    QString accent_ = QStringLiteral("#7eb8da");
    QString theme_ = QStringLiteral("Graphite Pro");
    QString cursorStyle_ = QStringLiteral("block");
    QString cursorColor_ = QStringLiteral("#7eb8da");
    int consoleAlpha_ = 230;
    int windowOpacity_ = 100;
    int fontSize_ = 14;
    int uiScale_ = 100;
    int nextNoteId_ = 1;
    int notesZoomOutPercent_ = 18;
    int noteCardSizePreset_ = 1;
    bool drawerOpen_ = true;
    bool notesVisible_ = false;
    bool starsEnabled_ = false;
    bool loadingNotes_ = false;
    bool windowDragging_ = false;
    QPoint dragOffset_;
    QPointer<QPushButton> draggingTabButton_;
    QPoint tabPressPosition_;
    bool tabReorderActive_ = false;
    QGraphicsObject *linkStart_ = nullptr;
    QGraphicsPathItem *previewPath_ = nullptr;
    QGraphicsPathItem *previewGlow_ = nullptr;
    QVector<QGraphicsPathItem *> noteEdges_;
};

#endif
