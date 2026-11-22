#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QGraphicsObject>
#include <QGraphicsPathItem>
#include <QHash>
#include <QMainWindow>
#include <QProcess>
#include "terminal_backend.h"
#include <QStringList>
#include <QVector>
#include <functional>

class QGraphicsScene;
class QGraphicsView;
class QLabel;
class QLineEdit;
class QPlainTextEdit;
class QPushButton;
class QResizeEvent;
class QShowEvent;
class QStackedWidget;
class QWidget;
class QEvent;
class StarfieldWidget;

struct CommandBind {
    QString name;
    QString command;
};

struct ConsoleSession {
    QProcess *process = nullptr;
    TerminalBackend *backend = nullptr;
    QWidget *page = nullptr;
    QWidget *tabRow = nullptr;
    QLineEdit *title = nullptr;
    QLineEdit *input = nullptr;
    QPlainTextEdit *output = nullptr;
    QLabel *shellBadge = nullptr;
    QLabel *status = nullptr;
    QPushButton *tabButton = nullptr;
    QString shell = QStringLiteral("cmd");
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
    bool nativeEvent(const QByteArray &eventType, void *message, qintptr *result) override;

private slots:
    void createConsole();
    void sendCommand();
    void readOutput();
    void createBind();
    void openSettings();
    void toggleDrawer();
    void addNote();

private:
    ConsoleSession *currentSession() const;
    void activateSession(ConsoleSession *session);
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
    void applySettings(const QString &accent, int alpha, int fontSize, int scale, const QString &theme, bool stars, int windowOpacity);
    void loadSettings();
    void saveSettings() const;
    void loadNotes();
    void saveNotes() const;
    QGraphicsObject *createNote(const QPointF &position, const QString &title, const QString &body, const QString &id = QString());
    void finishNoteLink(QGraphicsItem *target);
    void deleteNote(QGraphicsObject *note);
    void deleteEdge(QGraphicsPathItem *edge);
    void placeDrawerToggle();
    void setNotesVisible(bool visible);
    void centerWindow();

    QWidget *root_ = nullptr;
    StarfieldWidget *starfield_ = nullptr;
    QWidget *nav_ = nullptr;
    QWidget *consolePage_ = nullptr;
    QWidget *notesPage_ = nullptr;
    QWidget *drawer_ = nullptr;
    QWidget *drawerContent_ = nullptr;
    QWidget *bindHost_ = nullptr;
    QWidget *tabHost_ = nullptr;
    QStackedWidget *contentStack_ = nullptr;
    QStackedWidget *sessionStack_ = nullptr;
    QGraphicsScene *noteScene_ = nullptr;
    QGraphicsView *noteView_ = nullptr;
    QPushButton *drawerToggle_ = nullptr;
    QPushButton *notesNavButton_ = nullptr;
    QLabel *clockLabel_ = nullptr;
    QVector<ConsoleSession *> sessions_;
    QVector<CommandBind> binds_;
    QHash<QString, QGraphicsObject *> noteIds_;
    QString accent_ = QStringLiteral("#a78bfa");
    QString theme_ = QStringLiteral("Deep Space");
    int consoleAlpha_ = 215;
    int windowOpacity_ = 100;
    int fontSize_ = 14;
    int uiScale_ = 100;
    int nextNoteId_ = 1;
    bool drawerOpen_ = true;
    bool notesVisible_ = false;
    bool starsEnabled_ = true;
    bool loadingNotes_ = false;
    bool windowDragging_ = false;
    bool draggingWindow_ = false;
    QPoint dragOffset_;
    QGraphicsObject *linkStart_ = nullptr;
    QGraphicsPathItem *previewPath_ = nullptr;
    QGraphicsPathItem *previewGlow_ = nullptr;
    QVector<QGraphicsPathItem *> noteEdges_;
};

#endif
