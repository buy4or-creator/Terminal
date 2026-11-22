#include "mainwindow.h"

#include <QApplication>
#include <QClipboard>
#include <QCheckBox>
#include <QColorDialog>
#include <QComboBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QDir>
#include <QMimeData>
#include <QUrl>
#include <QFile>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsView>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QPainterPathStroker>
#include <QInputDialog>
#include <QIcon>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QKeyEvent>
#include <QLabel>
#include <QLineEdit>
#include <QFrame>
#include <QGraphicsDropShadowEffect>
#include <QLayout>
#include <QMessageBox>
#include <QMouseEvent>
#include <QPainter>
#include <QMenu>
#include <QGraphicsProxyWidget>
#include <QPlainTextEdit>
#include <QScrollBar>
#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QPointer>
#include <QPushButton>
#include <QRegularExpression>
#include <QResizeEvent>
#include <QScrollArea>
#include <QSettings>
#include <QSlider>
#include <QSpinBox>
#include <QStackedWidget>
#include <QStandardPaths>
#include <QTextCursor>
#include <QVBoxLayout>
#include <QPropertyAnimation>
#include <QEasingCurve>
#include <QScreen>
#include <QGuiApplication>
#include <QTimer>
#include <QRandomGenerator>
#include <QTime>
#include <QShowEvent>
#include <QFrame>

#ifdef Q_OS_WIN
#include <windows.h>
#include <windowsx.h>
#include <dwmapi.h>
#include <shellapi.h>
#endif

#include <algorithm>
#include <cmath>
#include <functional>

namespace {
QString rgba(const QString &hex, int alpha)
{
    QColor color(hex);
    return QStringLiteral("rgba(%1,%2,%3,%4)").arg(color.red()).arg(color.green()).arg(color.blue()).arg(alpha);
}

QString themeAccent(const QString &theme)
{
    if (theme == QStringLiteral("Graphite Pro") || theme == QStringLiteral("Graphite")) return QStringLiteral("#a9c4d8");
    if (theme == QStringLiteral("Obsidian")) return QStringLiteral("#e6edf3");
    if (theme == QStringLiteral("Emerald Matrix")) return QStringLiteral("#83e0b4");
    if (theme == QStringLiteral("Cyberpunk Neon")) return QStringLiteral("#ff4fa3");
    if (theme == QStringLiteral("Amber Sunset")) return QStringLiteral("#f3b45f");
    if (theme == QStringLiteral("Northern Lights")) return QStringLiteral("#72e1d2");
    if (theme == QStringLiteral("Red Mars")) return QStringLiteral("#ff786f");
    return QStringLiteral("#b78cff");
}

QString consoleBase(const QString &theme)
{
    if (theme == QStringLiteral("Graphite Pro") || theme == QStringLiteral("Graphite")) return QStringLiteral("#1c1e22");
    if (theme == QStringLiteral("Obsidian")) return QStringLiteral("#090a0d");
    if (theme == QStringLiteral("Emerald Matrix")) return QStringLiteral("#0a1b14");
    if (theme == QStringLiteral("Cyberpunk Neon")) return QStringLiteral("#160c20");
    if (theme == QStringLiteral("Amber Sunset")) return QStringLiteral("#21160f");
    if (theme == QStringLiteral("Northern Lights")) return QStringLiteral("#081b20");
    if (theme == QStringLiteral("Red Mars")) return QStringLiteral("#200f14");
    return QStringLiteral("#0e1022");
}

QString appStyle(const QString &accent, int alpha, int scale, const QString &theme)
{
    QString background = QStringLiteral("#0b0c10");
    QString panel = QStringLiteral("#111218");
    QString raised = QStringLiteral("#191b23");
    QString accentColor = accent;
    QString hover = QStringLiteral("#222530");
    QString pressed = rgba(accent, 50);
    QString border = QStringLiteral("rgba(255,255,255,0.08)");
    QString glass = rgba(QStringLiteral("#0e1016"), alpha);
    if (theme == QStringLiteral("Graphite Pro") || theme == QStringLiteral("Graphite")) {
        background = QStringLiteral("#1c1e22"); panel = QStringLiteral("#26292e"); raised = QStringLiteral("#34383f");
        accentColor = QStringLiteral("#a9c4d8"); border = QStringLiteral("#3a3e45"); glass = rgba(QStringLiteral("#1c1e22"), alpha);
    } else if (theme == QStringLiteral("Obsidian")) {
        background = QStringLiteral("#090a0d"); panel = QStringLiteral("#101116"); raised = QStringLiteral("#181a20");
        accentColor = QStringLiteral("#e6edf3"); border = QStringLiteral("#282a31"); glass = rgba(QStringLiteral("#090a0d"), alpha);
    } else if (theme == QStringLiteral("Emerald Matrix")) {
        background = QStringLiteral("#091914"); panel = QStringLiteral("#10231c"); raised = QStringLiteral("#18352a");
        accentColor = QStringLiteral("#83e0b4"); border = QStringLiteral("#28503e"); glass = rgba(QStringLiteral("#0a1b14"), alpha);
    } else if (theme == QStringLiteral("Cyberpunk Neon")) {
        background = QStringLiteral("#150b1c"); panel = QStringLiteral("#21112b"); raised = QStringLiteral("#32183b");
        accentColor = QStringLiteral("#ff4fa3"); border = QStringLiteral("#63305f"); hover = QStringLiteral("#3b2144"); glass = rgba(QStringLiteral("#160c20"), alpha);
    } else if (theme == QStringLiteral("Amber Sunset")) {
        background = QStringLiteral("#1c120d"); panel = QStringLiteral("#2a1d16"); raised = QStringLiteral("#3a281c");
        accentColor = QStringLiteral("#f3b45f"); border = QStringLiteral("#59402b"); hover = QStringLiteral("#45301f"); glass = rgba(QStringLiteral("#21160f"), alpha);
    } else if (theme == QStringLiteral("Northern Lights")) {
        background = QStringLiteral("#07191d"); panel = QStringLiteral("#0d252a"); raised = QStringLiteral("#14363a");
        accentColor = QStringLiteral("#72e1d2"); border = QStringLiteral("#28565a"); hover = QStringLiteral("#1b4146"); glass = rgba(QStringLiteral("#081b20"), alpha);
    } else if (theme == QStringLiteral("Red Mars")) {
        background = QStringLiteral("#1b0d11"); panel = QStringLiteral("#2a151a"); raised = QStringLiteral("#3b1e24");
        accentColor = QStringLiteral("#ff786f"); border = QStringLiteral("#61333a"); hover = QStringLiteral("#47252b"); glass = rgba(QStringLiteral("#200f14"), alpha);
    } else if (theme == QStringLiteral("Deep Space")) {
        background = QStringLiteral("#0b0c18"); panel = QStringLiteral("#111426"); raised = QStringLiteral("#191d34");
        accentColor = QStringLiteral("#b78cff"); border = QStringLiteral("rgba(155,165,255,0.18)"); glass = rgba(QStringLiteral("#0e1022"), alpha);
    }
    pressed = rgba(accentColor, 50);
    const QString topbar = theme == QStringLiteral("Graphite Pro") ? QStringLiteral("#202328") : panel;
    return QStringLiteral(
        "QWidget{color:#e7e8ee;font-family:'Segoe UI';font-size:%4px;}"
        "QWidget#root{background:%5;border-radius:16px;}"
        "QWidget#topBar{background:%10;border-bottom:1px solid %7;}"
        "QWidget#nav,QWidget#drawerContent{background:%6;border:1px solid %7;border-radius:16px;}"
        "QWidget#drawerPanel{background:transparent;border:0;}"
        "QFrame#consoleCard{background:%1;border:1px solid rgba(255,255,255,0.1);border-radius:16px;}"
        "QPlainTextEdit{background:transparent;border:0;color:#e6edf3;selection-background-color:%2;font-family:Consolas,'Cascadia Mono',monospace;}"
        "QLineEdit{background:%6;border:1px solid %7;border-radius:12px;padding:9px 12px;color:#f4f4f7;selection-background-color:%2;}"
        "QLineEdit:focus{border-color:%2;}"
        "QPushButton{background:%8;border:1px solid %7;border-radius:10px;padding:8px 12px;color:#dfe0e8;}"
        "QPushButton:hover{background:%9;border-color:%2;}"
        "QPushButton:pressed,QPushButton:checked{background:%3;border-color:%2;color:#fff;}"
        "QLabel#muted{color:#858896;}QScrollArea{border:0;background:transparent;}"
        "QSlider::groove:horizontal{height:4px;background:%8;border-radius:2px;}"
        "QSlider::handle:horizontal{width:14px;margin:-5px 0;background:%2;border-radius:7px;}"
        "QSpinBox,QComboBox{background:%6;border:1px solid %7;border-radius:8px;padding:7px;}"
    ).arg(glass, accentColor, pressed, QString::number(qMax(10, 13 * scale / 100)), background, panel, border, raised, hover, topbar);
}

}

class StarfieldWidget final : public QWidget {
public:
    explicit StarfieldWidget(QWidget *parent = nullptr) : QWidget(parent), timer_(new QTimer(this))
    {
        setAttribute(Qt::WA_TransparentForMouseEvents);
        setAttribute(Qt::WA_NoSystemBackground);
        setAttribute(Qt::WA_TranslucentBackground);
        setAutoFillBackground(false);
        for (int i = 0; i < 180; ++i) stars_.append({QRandomGenerator::global()->generateDouble(), QRandomGenerator::global()->generateDouble(), 0.25 + QRandomGenerator::global()->generateDouble() * 1.5, QRandomGenerator::global()->generateDouble() * 6.28});
        connect(timer_, &QTimer::timeout, this, [this] { tick(); });
        timer_->start(16);
    }
    void setTheme(const QString &theme) { theme_ = theme; update(); }
    void setAnimated(bool enabled) { animated_ = enabled; if (enabled) timer_->start(16); else timer_->stop(); update(); }
    void setStarsVisible(bool enabled) { visible_ = enabled; update(); }
protected:
    void paintEvent(QPaintEvent *) override
    {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);
        if (theme_ == QStringLiteral("Graphite Pro") || theme_ == QStringLiteral("Graphite")) p.fillRect(rect(), QColor("#1c1e22"));
        else if (theme_ == QStringLiteral("Obsidian")) p.fillRect(rect(), QColor("#090a0d"));
        else if (theme_ == QStringLiteral("Emerald Matrix")) p.fillRect(rect(), QColor("#091914"));
        else if (theme_ == QStringLiteral("Cyberpunk Neon")) p.fillRect(rect(), QColor("#150b1c"));
        else if (theme_ == QStringLiteral("Amber Sunset")) p.fillRect(rect(), QColor("#1c120d"));
        else if (theme_ == QStringLiteral("Northern Lights")) p.fillRect(rect(), QColor("#07191d"));
        else if (theme_ == QStringLiteral("Red Mars")) p.fillRect(rect(), QColor("#1b0d11"));
        else {
            p.fillRect(rect(), QColor("#080a17"));
            QRadialGradient nebula(width() * .78, height() * .20, qMax(width(), height()) * .7);
            nebula.setColorAt(0, QColor(70, 62, 150, 46)); nebula.setColorAt(.48, QColor(32, 50, 105, 21)); nebula.setColorAt(1, QColor(8, 10, 23, 0));
            p.fillRect(rect(), nebula);
            if (visible_) {
                for (const Star &star : stars_) {
                    const int alpha = qBound(30, int(120 + 100 * std::sin(star.phase_)), 220);
                    p.setPen(Qt::NoPen); p.setBrush(QColor(205, 220, 255, alpha));
                    p.drawEllipse(QPointF(star.x_ * width(), star.y_ * height()), star.radius_, star.radius_);
                }
                if (meteorActive_) {
                    QPen pen(QColor(190, 215, 255, 155), 1.5); p.setPen(pen);
                    p.drawLine(meteorPos_, meteorPos_ - QPointF(90, -46));
                }
            }
        }
    }
private:
    struct Star { double x_, y_, radius_, phase_; };
    void tick()
    {
        if (!animated_ || !visible_ || theme_ != QStringLiteral("Deep Space")) return;
        for (Star &star : stars_) { star.phase_ += .045; star.y_ += .00003 * star.radius_; if (star.y_ > 1) star.y_ = 0; }
        if (!meteorActive_ && QRandomGenerator::global()->bounded(1000) < 1) { meteorActive_ = true; meteorPos_ = QPointF(QRandomGenerator::global()->bounded(qMax(1, width())), QRandomGenerator::global()->bounded(qMax(1, height() / 2))); }
        if (meteorActive_) { meteorPos_ += QPointF(7, 4); if (meteorPos_.x() > width() || meteorPos_.y() > height()) meteorActive_ = false; }
        update();
    }
    QTimer *timer_;
    QVector<Star> stars_;
    QString theme_ = QStringLiteral("Deep Space");
    QPointF meteorPos_;
    bool animated_ = true;
    bool visible_ = true;
    bool meteorActive_ = false;
};

namespace {
QPushButton *button(const QString &text, QWidget *parent, const QString &objectName = QString())
{
    auto *result = new QPushButton(text, parent);
    result->setCursor(Qt::PointingHandCursor);
    if (!objectName.isEmpty()) result->setObjectName(objectName);
    return result;
}

QString shellLabel(const QString &shell)
{
    return shell == QStringLiteral("powershell") ? QStringLiteral("PowerShell") : QStringLiteral("CMD");
}


class TerminalHighlighter final : public QSyntaxHighlighter {
public:
    explicit TerminalHighlighter(QTextDocument *parent) : QSyntaxHighlighter(parent) {}
protected:
    void highlightBlock(const QString &text) override {
        QTextCharFormat promptFmt; promptFmt.setForeground(QColor("#4ade80")); promptFmt.setFontWeight(QFont::Bold);
        QTextCharFormat pathFmt; pathFmt.setForeground(QColor("#60a5fa")); pathFmt.setFontWeight(QFont::Bold);
        QTextCharFormat logoFmt; logoFmt.setForeground(QColor("#38bdf8"));
        QTextCharFormat keyFmt; keyFmt.setForeground(QColor("#22d3ee")); keyFmt.setFontWeight(QFont::Bold);
        QTextCharFormat valFmt; valFmt.setForeground(QColor("#e2e8f0"));
        QTextCharFormat numFmt; numFmt.setForeground(QColor("#c084fc"));
        QTextCharFormat dirFmt; dirFmt.setForeground(QColor("#818cf8"));
        QTextCharFormat errFmt; errFmt.setForeground(QColor("#f87171"));

        static const QRegularExpression promptRx(QStringLiteral("^([a-zA-Z0-9_.-]+@[a-zA-Z0-9_.-]+)(:[^$>]+[$>])(.*)$"));
        if (auto m = promptRx.match(text); m.hasMatch()) {
            setFormat(m.capturedStart(1), m.capturedLength(1), promptFmt);
            setFormat(m.capturedStart(2), m.capturedLength(2), pathFmt);
            setFormat(m.capturedStart(3), m.capturedLength(3), valFmt);
        }
        static const QRegularExpression logoRx(QStringLiteral("^[\\s.,:;o0cl'\"`\\\\/|-]{8,}"));
        if (auto m = logoRx.match(text); m.hasMatch()) {
            setFormat(m.capturedStart(0), m.capturedLength(0), logoFmt);
        }
        static const QRegularExpression keyRx(QStringLiteral("\\b(OS|Host|Kernel|Uptime|Packages|Shell|CPU|GPU|Memory|Swap|Disk|Local IP|Locale):(.*)$"));
        if (auto m = keyRx.match(text); m.hasMatch()) {
            setFormat(m.capturedStart(1), m.capturedLength(1) + 1, keyFmt);
            setFormat(m.capturedStart(2), m.capturedLength(2), valFmt);
        }
        static const QRegularExpression pathRx(QStringLiteral("(?:[A-Za-z]:\\\\[^\\s\"']+|/(?:home|etc|var|usr|opt|root|tmp|mnt|srv)[^\\s\"']*)"));
        auto pit = pathRx.globalMatch(text);
        while (pit.hasNext()) { auto m = pit.next(); setFormat(m.capturedStart(0), m.capturedLength(0), dirFmt); }

        static const QRegularExpression numRx(QStringLiteral("\\b\\d+(?:\\.\\d+)?(?:%|GiB|MiB|GB|MB|GHz|MHz)?\\b"));
        auto nit = numRx.globalMatch(text);
        while (nit.hasNext()) { auto m = nit.next(); setFormat(m.capturedStart(0), m.capturedLength(0), numFmt); }

        static const QRegularExpression errRx(QStringLiteral("(?i)\\b(error|failed|denied|not found|unreachable|fatal)\\b"));
        auto eit = errRx.globalMatch(text);
        while (eit.hasNext()) { auto m = eit.next(); setFormat(m.capturedStart(0), m.capturedLength(0), errFmt); }
    }
};
class TerminalCanvas final : public QPlainTextEdit {
public:
    explicit TerminalCanvas(QWidget *parent = nullptr) : QPlainTextEdit(parent)
    {
        setUndoRedoEnabled(true);
        setCursorWidth(8);
        setAcceptDrops(true);
        setTextInteractionFlags(Qt::TextEditorInteraction); 
    }
    std::function<void(const QString &)> commandSubmitted;
    std::function<void(const QByteArray &)> rawKeySent;
    int promptPos = 0;
    QString lastSentCmd;
    QString pendingHtml;
    bool renderScheduled = false;
    QStringList history;
    int historyIndex = 0;

    void appendSystemText(const QString &raw)
    {
        if (raw.isEmpty()) return;
        pendingHtml = raw;
        if (!renderScheduled) {
            renderScheduled = true;
            QTimer::singleShot(16, this, [this] {
                renderScheduled = false;
                document()->setDefaultFont(font());
                document()->setHtml(pendingHtml);
                QTextCursor fc(document());
                fc.select(QTextCursor::Document);
                QTextCharFormat fmt;
                fmt.setFontFamilies({QStringLiteral("JetBrains Mono"), QStringLiteral("Cascadia Code"), QStringLiteral("Consolas")});
                fmt.setFontPointSize(11);
                fc.mergeCharFormat(fmt);
                moveCursor(QTextCursor::End);
                this->verticalScrollBar()->setValue(this->verticalScrollBar()->maximum()); this->horizontalScrollBar()->setValue(0);
            });
        }
        moveCursor(QTextCursor::End);
        promptPos = textCursor().position();
        this->verticalScrollBar()->setValue(this->verticalScrollBar()->maximum()); this->horizontalScrollBar()->setValue(0);
    }

    void replaceCurrentInput(const QString &n)
    {
        QTextCursor c = textCursor();
        c.setPosition(promptPos);
        c.movePosition(QTextCursor::End, QTextCursor::KeepAnchor);
        c.removeSelectedText();
        c.insertText(n);
        setTextCursor(c);
    }

    QString currentInput() const
    {
        const QString f = toPlainText();
        return promptPos <= f.length() ? f.mid(promptPos) : QString();
    }

protected:
    bool event(QEvent *e) override {
        if (e->type() == QEvent::KeyPress) {
            auto *ke = static_cast<QKeyEvent *>(e);
            if (ke->key() == Qt::Key_Tab) {
                if (rawKeySent) rawKeySent(QByteArray("\t", 1));
                return true;
            }
        }
        return QPlainTextEdit::event(e);
    }
    void dragEnterEvent(QDragEnterEvent *e) override { if (e->mimeData()->hasUrls()) e->acceptProposedAction(); else QPlainTextEdit::dragEnterEvent(e); }
    void dragMoveEvent(QDragMoveEvent *e) override { if (e->mimeData()->hasUrls()) e->acceptProposedAction(); else QPlainTextEdit::dragMoveEvent(e); }
    void dropEvent(QDropEvent *e) override
    {
        if (e->mimeData()->hasUrls()) {
            QStringList paths;
            for (const QUrl &url : e->mimeData()->urls()) {
                if (!url.isLocalFile()) continue;
                QString local = QDir::toNativeSeparators(url.toLocalFile());
                if (local.contains(QLatin1Char(' '))) local = QLatin1Char('"') + local + QLatin1Char('"');
                paths << local;
            }
            if (!paths.isEmpty()) {
                if (textCursor().position() < promptPos) moveCursor(QTextCursor::End);
                insertPlainText(paths.join(QLatin1Char(' ')));
                setFocus();
            }
            e->acceptProposedAction();
            return;
        }
        QPlainTextEdit::dropEvent(e);
    }

        void keyPressEvent(QKeyEvent *e) override
    {
        const bool ctrl = e->modifiers() & Qt::ControlModifier;
        if (!ctrl && !(e->modifiers() & Qt::AltModifier)) {
            if (e->key() == Qt::Key_Return || e->key() == Qt::Key_Enter) { if (rawKeySent) rawKeySent(QByteArray("\r", 1)); e->accept(); return; }
            if (e->key() == Qt::Key_Backspace) { if (rawKeySent) rawKeySent(QByteArray("\x7f", 1)); e->accept(); return; }
            if (e->key() == Qt::Key_Up) { if (rawKeySent) rawKeySent(QByteArray("\x1b[A", 3)); e->accept(); return; }
            if (e->key() == Qt::Key_Down) { if (rawKeySent) rawKeySent(QByteArray("\x1b[B", 3)); e->accept(); return; }
            if (e->key() == Qt::Key_Right) { if (rawKeySent) rawKeySent(QByteArray("\x1b[C", 3)); e->accept(); return; }
            if (e->key() == Qt::Key_Left) { if (rawKeySent) rawKeySent(QByteArray("\x1b[D", 3)); e->accept(); return; }
            if (!e->text().isEmpty()) { if (rawKeySent) rawKeySent(e->text().toUtf8()); e->accept(); return; }
        }
        if (e->matches(QKeySequence::Copy)) { if (!e->text().isEmpty() && rawKeySent) { rawKeySent(e->text().toUtf8()); } e->accept(); return; }
                // Ctrl-комбинации для nano / bash / vim (кроме Ctrl+Shift+C / Ctrl+V)
        if (ctrl && !(e->modifiers() & Qt::ShiftModifier)) {
            if (e->key() == Qt::Key_C && textCursor().hasSelection()) { copy(); e->accept(); return; }
            if (e->key() == Qt::Key_V) { if (rawKeySent) rawKeySent(QApplication::clipboard()->text().toUtf8()); e->accept(); return; }
            if (e->key() >= Qt::Key_A && e->key() <= Qt::Key_Z) {
                char ctrlCode = static_cast<char>(e->key() - Qt::Key_A + 1);
                if (rawKeySent) rawKeySent(QByteArray(1, ctrlCode));
                e->accept();
                return;
            }
        }
        if (e->key() == Qt::Key_Delete) { if (rawKeySent) rawKeySent(QByteArray("\x1b[3~", 4)); e->accept(); return; }
        if (e->key() == Qt::Key_End) { if (rawKeySent) rawKeySent(QByteArray("\x1b[F", 3)); e->accept(); return; }
        if (e->key() == Qt::Key_PageUp) { if (rawKeySent) rawKeySent(QByteArray("\x1b[5~", 4)); e->accept(); return; }
        if (e->key() == Qt::Key_PageDown) { if (rawKeySent) rawKeySent(QByteArray("\x1b[6~", 4)); e->accept(); return; }
        if (textCursor().position() < promptPos) moveCursor(QTextCursor::End);
        
        if (e->key() == Qt::Key_Backspace) { if (rawKeySent) rawKeySent(QByteArray(1, char(127))); e->accept(); return; }
        if (e->key() == Qt::Key_Left) { if (rawKeySent) rawKeySent(QByteArray("\x1b[D", 3)); e->accept(); return; } if (e->key() == Qt::Key_Right) { if (rawKeySent) rawKeySent(QByteArray("\x1b[C", 3)); e->accept(); return; } if (e->key() == Qt::Key_Home) { if (rawKeySent) rawKeySent(QByteArray("\x1b[H", 3)); e->accept(); return; }
        if (e->key() == Qt::Key_Up) { if (rawKeySent) rawKeySent(QByteArray("\x1b[A", 3)); e->accept(); return; }
        if (e->key() == Qt::Key_Down) { if (rawKeySent) rawKeySent(QByteArray("\x1b[B", 3)); e->accept(); return; }
        if (e->key() == Qt::Key_Tab) { if (rawKeySent) rawKeySent(QByteArray("\t", 1)); e->accept(); return; }
        if (!e->text().isEmpty() && rawKeySent) { rawKeySent(e->text().toUtf8()); } e->accept();
    }
};

class NoteCard final : public QGraphicsObject {
public:
    NoteCard(QString heading, QString content, QString accent, QString id, QString theme)
        : heading_(std::move(heading)), content_(std::move(content)), accent_(std::move(accent)), id_(std::move(id)), theme_(std::move(theme))
    {
        setAcceptedMouseButtons(Qt::LeftButton);
        setZValue(2);
    }
    QRectF boundingRect() const override { return QRectF(0, 0, 260, 174); }
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) override
    {
        painter->setRenderHint(QPainter::Antialiasing);
        painter->setBrush(theme_ == QStringLiteral("Graphite") ? QColor("#26292e") : (theme_ == QStringLiteral("Obsidian") ? QColor("#101116") : QColor("#111426")));
        painter->setPen(QPen(QColor("#ffffff"), 1));
        painter->drawRoundedRect(boundingRect(), 12, 12);
        painter->setPen(QPen(QColor(accent_), 2));
        painter->drawLine(QPointF(13, 42), QPointF(247, 42));
        painter->setPen(QColor("#f3f3f6"));
        painter->drawText(QRectF(14, 10, 190, 25), Qt::AlignVCenter, heading_);
        painter->setPen(QColor("#b7b9c4"));
        painter->drawText(QRectF(208, 9, 20, 24), Qt::AlignCenter, QStringLiteral("↗"));
        painter->setPen(QColor("#ff929b"));
        painter->drawText(QRectF(232, 9, 20, 24), Qt::AlignCenter, QStringLiteral("×"));
        painter->setPen(QColor("#b6b8c2"));
        painter->drawText(QRectF(14, 52, 232, 105), Qt::TextWordWrap | Qt::AlignTop, content_);
        painter->setPen(QColor(accent_));
        painter->drawEllipse(QPointF(244, 21), 4, 4);
    }
    QString heading() const { return heading_; }
    QString content() const { return content_; }
    QString id() const { return id_; }
    void setAccent(const QString &accent) { accent_ = accent; update(); }
    void setTheme(const QString &theme) { theme_ = theme; update(); }
    void setHeading(const QString &heading) { heading_ = heading; update(); }
    void setContent(const QString &content) { content_ = content; update(); }
    std::function<void()> moved;
    std::function<void(NoteCard *)> linkRequested;
    std::function<void(NoteCard *)> editRequested;
    std::function<void(NoteCard *)> deleteRequested;
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override
    {
        if (event->button() == Qt::LeftButton && event->pos().y() < 42 && event->pos().x() >= 232) {
            if (deleteRequested) deleteRequested(this);
            event->accept();
            return;
        }
        if (event->button() == Qt::LeftButton && event->pos().y() < 42 && event->pos().x() >= 204) {
            if (linkRequested) linkRequested(this);
            event->accept();
            return;
        }
        dragging_ = event->button() == Qt::LeftButton;
        lastScenePos_ = event->scenePos();
        event->accept();
    }
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override
    {
        if (!dragging_) return;
        const QPointF delta = event->scenePos() - lastScenePos_;
        setPos(pos() + delta);
        lastScenePos_ = event->scenePos();
        if (moved) moved();
        event->accept();
    }
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override
    {
        dragging_ = false;
        event->accept();
    }
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override
    {
        if (event->pos().y() < 44 && editRequested) editRequested(this);
        event->accept();
    }
private:
    QString heading_;
    QString content_;
    QString accent_;
    QString id_;
    QString theme_;
    QPointF lastScenePos_;
    bool dragging_ = false;
};

class NoteEdge final : public QGraphicsPathItem {
public:
    NoteEdge(NoteCard *from, NoteCard *to) : from_(from), to_(to)
    {
        QPen edgePen(QColor(QStringLiteral("#ffffff")), 2.0, Qt::SolidLine, Qt::RoundCap); edgePen.setCosmetic(true);
        setPen(edgePen); setBrush(Qt::NoBrush); setZValue(1);
        setFlag(QGraphicsItem::ItemSendsGeometryChanges);
        setFlag(QGraphicsItem::ItemIsSelectable);
        setAcceptedMouseButtons(Qt::LeftButton | Qt::RightButton);
        setToolTip(QStringLiteral("Кликните, чтобы удалить ветку"));
        refresh();
    }
    std::function<void(NoteEdge *)> deleteRequested;
    void refresh()
    {
        if (!from_ || !to_) return;
        const QPointF a = from_->mapToScene(QPointF(244, 21));
        const QPointF b = to_->mapToScene(QPointF(14, 21));
        QPainterPath path(a);
        const qreal bend = (b.x() - a.x()) * 0.42;
        path.cubicTo(a + QPointF(bend, 0), b - QPointF(bend, 0), b);
        setPath(path);
    }
    bool touches(NoteCard *card) const { return from_ == card || to_ == card; }
    NoteCard *from() const { return from_; }
    NoteCard *to() const { return to_; }
    QRectF boundingRect() const override { return QGraphicsPathItem::boundingRect().adjusted(-8, -8, 8, 8); }
    QPainterPath shape() const override
    {
        QPainterPathStroker stroker; stroker.setWidth(14); return stroker.createStroke(path());
    }
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override
    {
        if (event->button() == Qt::RightButton || event->button() == Qt::LeftButton) {
            if (deleteRequested) deleteRequested(this);
            event->accept();
            return;
        }
        QGraphicsPathItem::mousePressEvent(event);
    }
private:
    NoteCard *from_;
    NoteCard *to_;
};

class NotesView final : public QGraphicsView {
public:
    explicit NotesView(QGraphicsScene *scene, QWidget *parent = nullptr) : QGraphicsView(scene, parent)
    {
        setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform | QPainter::TextAntialiasing);
        setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
        setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        setFrameShape(QFrame::NoFrame);
        setBackgroundBrush(Qt::NoBrush);
        setMouseTracking(true);
    }

    std::function<void(const QPointF &)> addAt;
    std::function<void(QGraphicsItem *)> linkAt;
    std::function<void(const QPointF &)> previewAt;
    std::function<void()> cancelLink;
    bool drawMode_ = false;
    bool eraseMode_ = false;
    QString drawColor_ = QStringLiteral("#a78bfa");
    qreal brushWidth_ = 3.0;
    QPointF lastDrawPt_;
    bool isDrawing_ = false;
    QList<QGraphicsPathItem *> drawnItems_;

    void clearStrokes()
    {
        for (auto *it : drawnItems_) { scene()->removeItem(it); delete it; }
        drawnItems_.clear();
    }

protected:
    void drawBackground(QPainter *painter, const QRectF &rect) override
    {
        painter->save();
        painter->fillRect(rect, QColor(10, 13, 20, 175));
        const qreal step = 36.0;
        const int alpha = qBound(14, int(zoom_ * 48.0), 115);
        const qreal radius = qBound(0.9, zoom_ * 1.35, 2.2);
        painter->setPen(Qt::NoPen);
        painter->setBrush(QColor(210, 225, 255, alpha));
        const qreal left = std::floor(rect.left() / step) * step;
        const qreal top = std::floor(rect.top() / step) * step;
        for (qreal x = left; x < rect.right(); x += step) {
            for (qreal y = top; y < rect.bottom(); y += step) {
                painter->drawEllipse(QPointF(x, y), radius, radius);
            }
        }
        painter->restore();
    }

        void keyPressEvent(QKeyEvent *event) override
    {
        if (event->key() == Qt::Key_Escape) {
            if (cancelLink) cancelLink();
            event->accept();
            return;
        }
        if ((event->modifiers() & Qt::ControlModifier) && event->key() == Qt::Key_Z) {
            if (!drawnItems_.isEmpty()) {
                auto *it = drawnItems_.takeLast();
                scene()->removeItem(it);
                delete it;
            }
            event->accept();
            return;
        }
        QGraphicsView::keyPressEvent(event);
    }

    void wheelEvent(QWheelEvent *event) override
    {
        if (event->modifiers() & Qt::ControlModifier) {
            const qreal factor = event->angleDelta().y() > 0 ? 1.12 : 0.89;
            const qreal next = qBound(0.45, zoom_ * factor, 2.2);
            const qreal delta = next / zoom_;
            zoom_ = next;
            scale(delta, delta);
            event->accept();
            return;
        }
        QGraphicsView::wheelEvent(event);
    }

    void mousePressEvent(QMouseEvent *event) override
    {
        if (eraseMode_ && event->button() == Qt::LeftButton) {
            const QPoint pt = event->position().toPoint();
            for (QGraphicsItem *it : items(QRect(pt.x() - 10, pt.y() - 10, 20, 20))) {
                if (auto *pi = dynamic_cast<QGraphicsPathItem *>(it); pi && drawnItems_.contains(pi)) {
                    drawnItems_.removeOne(pi); scene()->removeItem(pi); delete pi;
                }
            }
            event->accept(); return;
        }
        if (drawMode_ && event->button() == Qt::LeftButton) {
            lastDrawPt_ = mapToScene(event->position().toPoint());
            isDrawing_ = true;
            event->accept(); return;
        }
        if (event->button() == Qt::LeftButton) {
            if (linkAt) {
                for (QGraphicsItem *hit : scene()->items(mapToScene(event->position().toPoint()))) {
                    QGraphicsItem *root = hit;
                    while (root->parentItem()) root = root->parentItem();
                    if (!dynamic_cast<QGraphicsPathItem *>(root) && !dynamic_cast<QGraphicsEllipseItem *>(root)) { linkAt(root); break; }
                }
            }
        }
        if (event->button() == Qt::LeftButton && !itemAt(event->position().toPoint())) {
            panning_ = true;
            panStart_ = event->position().toPoint();
            setCursor(Qt::ClosedHandCursor);
            event->accept();
            return;
        }
        if (event->button() == Qt::RightButton && cancelLink) cancelLink();
        QGraphicsView::mousePressEvent(event);
    }

    void mouseMoveEvent(QMouseEvent *event) override
    {
        if (eraseMode_ && (event->buttons() & Qt::LeftButton)) {
            const QPoint pt = event->position().toPoint();
            for (QGraphicsItem *it : items(QRect(pt.x() - 10, pt.y() - 10, 20, 20))) {
                if (auto *pi = dynamic_cast<QGraphicsPathItem *>(it); pi && drawnItems_.contains(pi)) {
                    drawnItems_.removeOne(pi); scene()->removeItem(pi); delete pi;
                }
            }
            event->accept(); return;
        }
        if (drawMode_ && isDrawing_ && (event->buttons() & Qt::LeftButton)) {
            const QPointF curPt = mapToScene(event->position().toPoint());
            QPainterPath seg; seg.moveTo(lastDrawPt_); seg.lineTo(curPt);
            auto *item = scene()->addPath(seg, QPen(QColor(drawColor_), brushWidth_, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
            item->setZValue(5);
            drawnItems_.append(item);
            lastDrawPt_ = curPt;
            event->accept(); return;
        }
        if (panning_) {
            const QPoint delta = event->position().toPoint() - panStart_;
            panStart_ = event->position().toPoint();
            horizontalScrollBar()->setValue(horizontalScrollBar()->value() - delta.x());
            verticalScrollBar()->setValue(verticalScrollBar()->value() - delta.y());
            event->accept();
            return;
        }
        if (previewAt) previewAt(mapToScene(event->position().toPoint()));
        QGraphicsView::mouseMoveEvent(event);
    }

    void mouseReleaseEvent(QMouseEvent *event) override
    {
        if (drawMode_ && isDrawing_) { isDrawing_ = false; event->accept(); return; }
        if (panning_) {
            panning_ = false;
            setCursor(Qt::ArrowCursor);
            event->accept();
            return;
        }
        if (event->button() == Qt::LeftButton) {
            if (linkAt) {
                for (QGraphicsItem *hit : scene()->items(mapToScene(event->position().toPoint()))) {
                    QGraphicsItem *root = hit;
                    while (root->parentItem()) root = root->parentItem();
                    if (!dynamic_cast<QGraphicsPathItem *>(root) && !dynamic_cast<QGraphicsEllipseItem *>(root)) { linkAt(root); break; }
                }
            }
        }
        QGraphicsView::mouseReleaseEvent(event);
    }

private:
    bool panning_ = false;
    QPoint panStart_;
    qreal zoom_ = 1.0;
};

}

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint | Qt::WindowSystemMenuHint | Qt::WindowMinMaxButtonsHint);
    setWindowTitle(QStringLiteral("Terminal"));
    setMinimumSize(760, 480);
    resize(1150, 720);
    loadSettings();
    root_ = new QWidget(this);
    root_->setObjectName(QStringLiteral("root"));
    setCentralWidget(root_);
    auto *rootLayout = new QVBoxLayout(root_);
    starfield_ = new StarfieldWidget(root_);
    starfield_->setGeometry(root_->rect());
    starfield_->lower();
    rootLayout->setContentsMargins(12, 8, 12, 12);
    rootLayout->setSpacing(10);
    root_->setAttribute(Qt::WA_TranslucentBackground, theme_ == QStringLiteral("Deep Space"));

    auto *header = new QWidget(root_);
    header->setObjectName(QStringLiteral("topBar"));
    auto *headerLayout = new QHBoxLayout(header);
    headerLayout->setContentsMargins(8, 2, 2, 2);
    const QString iconPath = QStandardPaths::writableLocation(QStandardPaths::PicturesLocation) + QStringLiteral("/consol.png");
    const QIcon appIcon(iconPath);
    QApplication::setWindowIcon(appIcon);
    QFont uiFont;
    uiFont.setFamilies({QStringLiteral("Inter"), QStringLiteral("Segoe UI Variable Text"), QStringLiteral("Segoe UI"), QStringLiteral("Arial")});
    uiFont.setPointSize(10);
    uiFont.setStyleStrategy(QFont::PreferAntialias);
    QApplication::setFont(uiFont);
    setWindowIcon(appIcon);
    auto *brandIcon = new QLabel(header);
    brandIcon->setPixmap(appIcon.pixmap(20, 20));
    brandIcon->setFixedSize(20, 20);
    auto *brand = new QLabel(QStringLiteral("Terminal"), header);
    brand->setStyleSheet(QStringLiteral("font-weight:600;letter-spacing:1px;color:#dfe0e8;"));
    clockLabel_ = new QLabel(header);
    clockLabel_->setObjectName(QStringLiteral("muted"));
    auto *spacer = new QWidget(header);
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    auto *close = button(QString(), header, QStringLiteral("trafficRed"));
    auto *minimize = button(QString(), header, QStringLiteral("trafficYellow"));
    auto *maximize = button(QString(), header, QStringLiteral("trafficGreen"));
    for (QPushButton *control : {close, minimize, maximize}) {
        control->setFixedSize(14, 14);
        control->setStyleSheet(QStringLiteral("QPushButton{min-width:14px;max-width:14px;min-height:14px;max-height:14px;padding:0;border:0;border-radius:7px;color:#202124;font-size:11px;font-weight:700;background:%1;}QPushButton:hover{background:%2;}").arg(control->objectName() == QStringLiteral("trafficRed") ? QStringLiteral("#ff5f56") : control->objectName() == QStringLiteral("trafficYellow") ? QStringLiteral("#ffbd2e") : QStringLiteral("#27c93f"), control->objectName() == QStringLiteral("trafficRed") ? QStringLiteral("#ff776f") : control->objectName() == QStringLiteral("trafficYellow") ? QStringLiteral("#ffd45c") : QStringLiteral("#48db5f")));
        control->installEventFilter(this);
    }
    auto *trafficSpacer = new QWidget(header);
    trafficSpacer->setFixedWidth(8);
    headerLayout->addWidget(brandIcon);
    headerLayout->addWidget(brand);
    headerLayout->addSpacing(12);
    headerLayout->addWidget(clockLabel_);
    headerLayout->addWidget(spacer);
    headerLayout->addWidget(close);
    headerLayout->addWidget(minimize);
    headerLayout->addWidget(maximize);
    headerLayout->addWidget(trafficSpacer);
    rootLayout->addWidget(header);
    connect(minimize, &QPushButton::clicked, this, &QWidget::showMinimized);
    connect(maximize, &QPushButton::clicked, this, [this] { HWND h = reinterpret_cast<HWND>(winId()); ShowWindow(h, IsZoomed(h) ? SW_RESTORE : SW_MAXIMIZE); });
    connect(close, &QPushButton::clicked, this, &QWidget::close);
#ifdef Q_OS_WIN
    const HWND nativeWindow = reinterpret_cast<HWND>(winId());
    LONG_PTR nativeStyle = GetWindowLongPtrW(nativeWindow, GWL_STYLE);
    nativeStyle |= WS_THICKFRAME | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_CAPTION;
    SetWindowLongPtrW(nativeWindow, GWL_STYLE, nativeStyle);
    SetWindowPos(nativeWindow, nullptr, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
    const DWORD cornerPreference = 2;
    DwmSetWindowAttribute(nativeWindow, 33, &cornerPreference, sizeof(cornerPreference));
#endif

    auto *columns = new QHBoxLayout;
    columns->setSpacing(12);
    rootLayout->addLayout(columns, 1);
    nav_ = new QWidget(root_);
    nav_->setObjectName(QStringLiteral("nav"));
    nav_->setFixedWidth(220);
    auto *navLayout = new QVBoxLayout(nav_);
    navLayout->setContentsMargins(12, 12, 12, 12);
    navLayout->setSpacing(9);
    auto *newConsole = button(QStringLiteral("＋  Новая консоль"), nav_);
    tabHost_ = new QWidget(nav_);
    auto *tabLayout = new QVBoxLayout(tabHost_);
    tabLayout->setContentsMargins(0, 4, 0, 0);
    tabLayout->setSpacing(6);
    tabLayout->setAlignment(Qt::AlignTop);
    navLayout->addWidget(newConsole);
    navLayout->addWidget(tabHost_);
    navLayout->addStretch(1);
    notesNavButton_ = button(QStringLiteral("Заметки"), nav_);
    auto *settings = button(QStringLiteral("Настройки"), nav_);
    navLayout->addWidget(notesNavButton_);
    navLayout->addWidget(settings);
    columns->addWidget(nav_);

    contentStack_ = new QStackedWidget(root_);
    consolePage_ = new QWidget(contentStack_);
    auto *consoleLayout = new QHBoxLayout(consolePage_);
    consoleLayout->setContentsMargins(0, 0, 0, 0);
    consoleLayout->setSpacing(10);
    sessionStack_ = new QStackedWidget(consolePage_);
    consoleLayout->addWidget(sessionStack_, 1);

    drawer_ = new QWidget(consolePage_);
    drawer_->setObjectName(QStringLiteral("drawerPanel"));
    drawer_->setMinimumWidth(270);
    drawer_->setMaximumWidth(270);
    auto *drawerLayout = new QVBoxLayout(drawer_);
    drawerLayout->setContentsMargins(0, 0, 0, 0);
    drawerContent_ = new QWidget(drawer_);
    drawerContent_->setObjectName(QStringLiteral("drawerContent"));
    auto *contentLayout = new QVBoxLayout(drawerContent_);
    contentLayout->setContentsMargins(12, 12, 12, 12);
    contentLayout->setSpacing(10);
    auto *actualDrawerLayout = contentLayout;
    drawerLayout->addWidget(drawerContent_);
    auto *drawerTitle = new QLabel(QStringLiteral("Оболочки и бинды"), drawerContent_);
    drawerTitle->setStyleSheet(QStringLiteral("font-size:15px;font-weight:600;"));
    actualDrawerLayout->addWidget(drawerTitle);
    auto *shellGrid = new QGridLayout;
    shellGrid->setSpacing(6);
    const QStringList shellNames{QStringLiteral("CMD"), QStringLiteral("PowerShell"), QStringLiteral("CMD Admin"), QStringLiteral("PS Admin")};
    for (int i = 0; i < shellNames.size(); ++i) {
        auto *shellButton = button(shellNames.at(i), drawerContent_);
        shellGrid->addWidget(shellButton, i / 2, i % 2);
        const QString shell = i % 2 == 0 ? QStringLiteral("cmd") : QStringLiteral("powershell");
        const bool elevated = i >= 2;
        connect(shellButton, &QPushButton::clicked, this, [this, shell, elevated] { launchShell(shell, elevated); });
    }
    actualDrawerLayout->addLayout(shellGrid);
    auto *bindScroll = new QScrollArea(drawerContent_);
    bindScroll->setWidgetResizable(true);
    bindHost_ = new QWidget(bindScroll);
    auto *bindLayout = new QVBoxLayout(bindHost_);
    bindLayout->setContentsMargins(0, 0, 0, 0);
    bindLayout->setAlignment(Qt::AlignTop);
    bindScroll->setWidget(bindHost_);
    actualDrawerLayout->addWidget(bindScroll, 1);
    auto *newBind = button(QStringLiteral("＋  Создать бинд"), drawerContent_);
    actualDrawerLayout->addWidget(newBind);
    consoleLayout->addWidget(drawer_);
    contentStack_->addWidget(consolePage_);

    notesPage_ = new QWidget(contentStack_);
    auto *notesLayout = new QVBoxLayout(notesPage_);
    notesLayout->setContentsMargins(0, 0, 0, 0);
    auto *notesHeader = new QHBoxLayout; notesHeader->setSpacing(6); notesHeader->setContentsMargins(6, 4, 6, 4);
    auto *notesTitle = new QLabel(QStringLiteral("Заметки"), notesPage_);
    notesTitle->setStyleSheet(QStringLiteral("font-size:18px;font-weight:600;"));
    auto *addCard = button(QStringLiteral("＋  Карточка"), notesPage_);
    auto *drawBtn = button(QStringLiteral("✏  Рисовать"), notesPage_);
    auto *eraseBtn = button(QStringLiteral("🧽  Ластик"), notesPage_); eraseBtn->setCheckable(true); auto *clearDrawBtn = button(QStringLiteral("🧹  Очистить всё"), notesPage_);
    drawBtn->setCheckable(true);
    
    noteScene_ = new QGraphicsScene(this); noteScene_->setItemIndexMethod(QGraphicsScene::NoIndex); auto *view = new NotesView(noteScene_, notesPage_); noteView_ = view;
    view->setFocusPolicy(Qt::StrongFocus); auto *brushBtn = button(QStringLiteral("🎨  Кисть"), notesPage_);
    auto *brushMenu = new QMenu(brushBtn);
    brushMenu->setStyleSheet(QStringLiteral("QMenu{background:#161b26;color:#f0f6fc;border:1px solid rgba(255,255,255,0.14);border-radius:10px;padding:6px;}QMenu::item{padding:6px 14px;border-radius:6px;}QMenu::item:selected{background:rgba(255,255,255,0.12);}"));
    auto addColorAct = [&](const QString &label, const QString &hex) {
        connect(brushMenu->addAction(label), &QAction::triggered, view, [view, drawBtn, eraseBtn, hex] {
            view->drawColor_ = hex; view->drawMode_ = true; view->eraseMode_ = false;
            drawBtn->setChecked(true); eraseBtn->setChecked(false); view->setCursor(Qt::CrossCursor);
        });
    };
    addColorAct(QStringLiteral("🟣 Фиолетовый"), QStringLiteral("#a78bfa"));
    addColorAct(QStringLiteral("🔵 Голубой"), QStringLiteral("#38bdf8"));
    addColorAct(QStringLiteral("🟢 Зелёный"), QStringLiteral("#4ade80"));
    addColorAct(QStringLiteral("🟡 Жёлтый"), QStringLiteral("#facc15"));
    addColorAct(QStringLiteral("🔴 Красный"), QStringLiteral("#f87171"));
    addColorAct(QStringLiteral("⚪ Белый"), QStringLiteral("#f8fafc"));
    brushMenu->addSeparator();
    connect(brushMenu->addAction(QStringLiteral("— Тонкая (2 px)")), &QAction::triggered, view, [view]{ view->brushWidth_ = 2.0; });
    connect(brushMenu->addAction(QStringLiteral("━ Средняя (4 px)")), &QAction::triggered, view, [view]{ view->brushWidth_ = 4.0; });
    connect(brushMenu->addAction(QStringLiteral("▬ Маркер (8 px)")), &QAction::triggered, view, [view]{ view->brushWidth_ = 8.0; });
    brushBtn->setMenu(brushMenu);

    auto *pinBtn = button(QStringLiteral("🔒  PIN"), notesPage_);
    connect(pinBtn, &QPushButton::clicked, this, [this] {
        QSettings s(QStringLiteral("ConsoleV1"), QStringLiteral("TerminalApp"));
        const QString curPin = s.value(QStringLiteral("notesPin")).toString();
        bool ok = false;
        const QString pin = QInputDialog::getText(this, QStringLiteral("PIN-код заметок"),
            curPin.isEmpty() ? QStringLiteral("Задайте 4-значный PIN-код:") : QStringLiteral("Новый 4-значный PIN (пусто — убрать защиту):"),
            QLineEdit::Password, QString(), &ok);
        if (!ok) return;
        if (pin.isEmpty()) { s.remove(QStringLiteral("notesPin")); QMessageBox::information(this, QStringLiteral("Заметки"), QStringLiteral("Защита PIN-кодом снята.")); }
        else if (pin.length() == 4 && pin.toInt(&ok) && ok) { s.setValue(QStringLiteral("notesPin"), pin); QMessageBox::information(this, QStringLiteral("Заметки"), QStringLiteral("PIN-код установлен!")); }
        else { QMessageBox::warning(this, QStringLiteral("Ошибка"), QStringLiteral("PIN-код должен состоять ровно из 4 цифр.")); }
    });
    auto *closeNotesBtn = button(QStringLiteral("✕  Закрыть заметки"), notesPage_);
    connect(closeNotesBtn, &QPushButton::clicked, this, [this] { setNotesVisible(false); });
        notesHeader->addWidget(notesTitle);
    notesHeader->addStretch();
    notesHeader->addWidget(addCard); notesHeader->addWidget(drawBtn); notesHeader->addWidget(eraseBtn); notesHeader->addWidget(clearDrawBtn); notesHeader->addWidget(brushBtn); notesHeader->addWidget(pinBtn); notesHeader->addWidget(closeNotesBtn);
    notesLayout->addLayout(notesHeader);
    
    view->setSceneRect(-5000, -5000, 10000, 10000);
    view->setRenderHint(QPainter::Antialiasing);
    view->setFrameShape(QFrame::NoFrame);
    view->setDragMode(QGraphicsView::NoDrag);
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->addAt = [this](QPointF pos) { createNote(pos, QStringLiteral("Заметка"), QString()); };
    view->linkAt = [this](QGraphicsItem *item) { finishNoteLink(item); };
    view->previewAt = [this](QPointF pos) {
        if (!previewPath_ || !linkStart_) return;
        const QPointF from = linkStart_->sceneBoundingRect().center();
        QPainterPath path(from);
        const qreal bend = (pos.x() - from.x()) * 0.35;
        path.cubicTo(from + QPointF(bend, 0), pos - QPointF(bend, 0), pos);
        previewPath_->setPath(path);
        if (previewGlow_) previewGlow_->setPath(path);
    };
    view->drawColor_ = accent_; connect(drawBtn, &QPushButton::toggled, view, [view, eraseBtn](bool on){ view->drawMode_ = on; if (on) eraseBtn->setChecked(false); view->setCursor(on ? Qt::CrossCursor : Qt::ArrowCursor); }); connect(eraseBtn, &QPushButton::toggled, view, [view, drawBtn](bool on){ view->eraseMode_ = on; if (on) drawBtn->setChecked(false); view->setCursor(on ? Qt::PointingHandCursor : Qt::ArrowCursor); }); connect(clearDrawBtn, &QPushButton::clicked, view, [view]{ view->clearStrokes(); }); notesLayout->addWidget(view, 1);
    contentStack_->addWidget(notesPage_);
    columns->addWidget(contentStack_, 1);
    starfield_->lower();

    drawerToggle_ = button(QStringLiteral("‹"), root_);
    drawerToggle_->setFixedSize(34, 34);
    drawerToggle_->raise();
    connect(drawerToggle_, &QPushButton::clicked, this, &MainWindow::toggleDrawer);
    connect(newConsole, &QPushButton::clicked, this, &MainWindow::createConsole);
    connect(notesNavButton_, &QPushButton::clicked, this, [this] { setNotesVisible(!notesVisible_); });
    connect(settings, &QPushButton::clicked, this, &MainWindow::openSettings);
    connect(newBind, &QPushButton::clicked, this, &MainWindow::createBind);
    connect(addCard, &QPushButton::clicked, this, &MainWindow::addNote);

    accent_ = themeAccent(theme_);
    setStyleSheet(appStyle(accent_, consoleAlpha_, uiScale_, theme_));
    setWindowOpacity(windowOpacity_ / 100.0);
    for (QPushButton *control : findChildren<QPushButton *>()) control->installEventFilter(this);
    root_->setAttribute(Qt::WA_TranslucentBackground, theme_ == QStringLiteral("Deep Space"));
    starfield_->setTheme(theme_);
    starfield_->setAnimated(starsEnabled_);
    createConsole();
    loadNotes();
    refreshBinds();
    centerWindow();
    QTimer *clock = new QTimer(this);
    connect(clock, &QTimer::timeout, this, [this] { clockLabel_->setText(QTime::currentTime().toString(QStringLiteral("HH:mm"))); });
    clock->start(1000);
}

MainWindow::~MainWindow()
{
    saveSettings();
    saveNotes();
    const auto sessions = sessions_;
    for (ConsoleSession *session : sessions) stopProcess(session);
    qDeleteAll(sessions_);
}

void MainWindow::centerWindow()
{
    resize(1150, 720);
    if (QScreen *screen = QGuiApplication::primaryScreen())
        move(screen->availableGeometry().center() - rect().center());
}

ConsoleSession *MainWindow::currentSession() const
{
    QWidget *page = sessionStack_ ? sessionStack_->currentWidget() : nullptr;
    for (ConsoleSession *session : sessions_) if (session->page == page) return session;
    return nullptr;
}

void MainWindow::createConsole()
{
    auto *session = new ConsoleSession;
    session->page = new QWidget(sessionStack_);
    auto *pageLayout = new QVBoxLayout(session->page);
    pageLayout->setContentsMargins(8, 8, 8, 8);
    pageLayout->setSpacing(10);
    auto *card = new QFrame(session->page);
    card->setObjectName(QStringLiteral("consoleCard"));
    card->setAttribute(Qt::WA_Hover, true);
    auto *cardShadow = new QGraphicsDropShadowEffect(card);
    cardShadow->setBlurRadius(22);
    cardShadow->setOffset(0, 8);
    cardShadow->setColor(QColor(0, 0, 0, 90));
    card->setGraphicsEffect(cardShadow);

    card->setStyleSheet(QStringLiteral("QFrame#consoleCard{background:%1;border:1px solid rgba(255,255,255,0.1);border-radius:16px;}").arg(rgba(consoleBase(theme_), consoleAlpha_)));
    auto *cardLayout = new QVBoxLayout(card);
    cardLayout->setContentsMargins(16, 14, 16, 14);
    cardLayout->setSpacing(10);
    auto *toolbar = new QHBoxLayout;
    session->title = new QLineEdit(QStringLiteral("Terminal %1").arg(sessions_.size() + 1), card);
    session->title->setMaximumWidth(260);
    session->shellBadge = new QLabel(QStringLiteral("CMD"), card);
    session->shellBadge->setStyleSheet(QStringLiteral("background:rgba(255,255,255,0.08);border-radius:8px;padding:5px 9px;font-weight:600;"));
    session->status = new QLabel(QStringLiteral("Starting"), card);
    session->status->setObjectName(QStringLiteral("muted"));
    auto *spacer = new QWidget(card);
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    auto *copy = button(QStringLiteral("Копировать"), card);
    auto *paste = button(QStringLiteral("Вставить"), card);
    auto *copyAll = button(QStringLiteral("📄 Скопировать всё"), card);
    toolbar->addWidget(session->title);
    toolbar->addWidget(session->shellBadge);
    toolbar->addWidget(session->status);
    toolbar->addWidget(spacer);
    toolbar->addWidget(copy);
    toolbar->addWidget(paste); toolbar->addWidget(copyAll);
    cardLayout->addLayout(toolbar);
    auto *canvas = new TerminalCanvas(card); session->output = canvas;
    canvas->setReadOnly(true); canvas->setCursorWidth(0); canvas->setTextInteractionFlags(Qt::TextSelectableByMouse | Qt::TextSelectableByKeyboard);
    session->output->setFocusPolicy(Qt::StrongFocus);
    session->output->setLineWrapMode(QPlainTextEdit::NoWrap); session->output->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    session->output->setStyleSheet(QStringLiteral("QPlainTextEdit{background:transparent;border:0;padding:6px;color:#f0f6fc;selection-background-color:#6d5aa6;font-family:'JetBrains Mono','Cascadia Code','Consolas',monospace;font-size:14px;}"));
    
    QFont terminalFont(QStringLiteral("Consolas"), fontSize_); terminalFont.setStyleStrategy(QFont::PreferQuality);
    session->output->setFont(terminalFont);
    cardLayout->addWidget(session->output, 1);
    canvas->commandSubmitted=[this,session,canvas](const QString&cmd){if(cmd.trimmed().compare(QStringLiteral("cls"),Qt::CaseInsensitive)==0||cmd.trimmed().compare(QStringLiteral("clear"),Qt::CaseInsensitive)==0){canvas->clear();canvas->promptPos=0;writeCommand(session,QString());return;}writeCommand(session,cmd);};canvas->rawKeySent=[session](const QByteArray&b){if(session->backend)session->backend->writeInput(b);};
    pageLayout->addWidget(card, 1);
    session->tabRow = new QWidget(tabHost_);
    auto *tabLayout = new QHBoxLayout(session->tabRow);
    tabLayout->setContentsMargins(0, 0, 0, 0);
    tabLayout->setSpacing(3);
    session->tabButton = button(session->title->text(), session->tabRow);
    session->tabButton->setCheckable(true);
    auto *rename = button(QStringLiteral("✎"), session->tabRow);
    auto *close = button(QStringLiteral("×"), session->tabRow);
    rename->setFixedWidth(30);
    close->setFixedWidth(30);
    tabLayout->addWidget(session->tabButton, 1);
    tabLayout->addWidget(rename);
    tabLayout->addWidget(close);
    qobject_cast<QVBoxLayout *>(tabHost_->layout())->addWidget(session->tabRow);
    sessions_.append(session);
    sessionStack_->addWidget(session->page);
    connect(session->tabButton, &QPushButton::clicked, this, [this, session] { activateSession(session); QTimer::singleShot(120, this, [session] { if (session && session->backend && session->output) session->backend->resizeTerminal(QSize(qMax(40, int((session->output->viewport()->width() - 12) / 8.5)), qMax(15, int((session->output->viewport()->height() - 12) / 19.0)))); }); });
    connect(rename, &QPushButton::clicked, this, [this, session] { renameSession(session); });
    connect(close, &QPushButton::clicked, this, [this, session] { closeSession(session); });
    connect(session->title, &QLineEdit::textChanged, this, [session](const QString &text) { session->tabButton->setText(text); });
    connect(copy, &QPushButton::clicked, this, [session] {
        const QString selected = session->output->textCursor().selectedText();
        QApplication::clipboard()->setText(selected.isEmpty() ? session->output->toPlainText() : selected);
    });
    connect(copyAll, &QPushButton::clicked, this, [session] { if (session->output) QApplication::clipboard()->setText(session->output->toPlainText()); }); connect(paste, &QPushButton::clicked, this, [session] { session->output->moveCursor(QTextCursor::End);session->output->insertPlainText(QApplication::clipboard()->text());session->output->setFocus(); });
    activateSession(session); QTimer::singleShot(120, this, [session] { if (session && session->backend && session->output) session->backend->resizeTerminal(QSize(qMax(40, int((session->output->viewport()->width() - 12) / 8.5)), qMax(15, int((session->output->viewport()->height() - 12) / 19.0)))); });
        startProcess(session, QStringLiteral("cmd"));
    
}

void MainWindow::activateSession(ConsoleSession *session)
{
    if (!session) return;
    sessionStack_->setCurrentWidget(session->page);
    for (ConsoleSession *item : sessions_) item->tabButton->setChecked(item == session);
    if(session->output) session->output->setFocus();
}

void MainWindow::renameSession(ConsoleSession *session)
{
    if (!session) return;
    bool accepted = false;
    const QString name = QInputDialog::getText(this, QStringLiteral("Переименовать консоль"), QStringLiteral("Название"), QLineEdit::Normal, session->title->text(), &accepted);
    if (accepted && !name.trimmed().isEmpty()) session->title->setText(name.trimmed());
}

void MainWindow::closeSession(ConsoleSession *session)
{
    if (!session) return;
    if (sessions_.size() == 1) {
        stopProcess(session);
        session->output->clear();
            startProcess(session, QStringLiteral("cmd"));
    
        return;
    }
    const int index = sessions_.indexOf(session);
    const bool active = sessionStack_->currentWidget() == session->page;
    stopProcess(session);
    sessions_.removeAt(index);
    sessionStack_->removeWidget(session->page);
    session->tabRow->deleteLater();
    session->page->deleteLater();
    delete session;
    if (active) activateSession(sessions_.at(qMin(index, sessions_.size() - 1)));
}

void MainWindow::startProcess(ConsoleSession *session, const QString &shell, bool elevated)
{
    if (!session) return;
#ifdef Q_OS_WIN
    if (elevated) {
        const QString executable = shell == QStringLiteral("powershell") ? QStringLiteral("powershell.exe") : QStringLiteral("cmd.exe");
        const QString parameters = shell == QStringLiteral("powershell")
            ? QStringLiteral("-NoLogo -NoExit -Command \"[Console]::OutputEncoding = [System.Text.Encoding]::UTF8; [Console]::InputEncoding = [System.Text.Encoding]::UTF8;\"")
            : QStringLiteral("/Q /K \"chcp 65001 > nul\"");
        const HINSTANCE result = ShellExecuteW(reinterpret_cast<HWND>(winId()), L"runas", reinterpret_cast<LPCWSTR>(executable.utf16()), reinterpret_cast<LPCWSTR>(parameters.utf16()), nullptr, SW_SHOWNORMAL);
        session->status->setText(reinterpret_cast<INT_PTR>(result) > 32 ? QStringLiteral("Elevated console opened separately") : QStringLiteral("Elevation cancelled or failed"));
        return;
    }
#else
    Q_UNUSED(elevated);
#endif
    if (session->backend || session->process) stopProcess(session);
    session->shell = shell;
    session->process = nullptr;
    session->backend = new TerminalBackend(this);
    connect(session->backend, &TerminalBackend::outputText, this, [this, session](const QString &output) {
        QString text = output;
        text.remove(QRegularExpression(QString::fromLatin1(R"(\x1b(?:[@-Z\\-_]|\[[0-?]*[ -/]*[@-~]|\][^\x07\x1b]*(?:\x07|\x1b\\)))")));
        appendOutput(session, text);
    });
    connect(session->backend, &TerminalBackend::errorOccurred, this, [this, session](const QString &error) { appendOutput(session, error + QLatin1Char('\n')); });
    connect(session->backend, &TerminalBackend::processFinished, this, [session](int code) {
        if (session->status) session->status->setText(QStringLiteral("Exited (%1)").arg(code));
    });
    session->status->setText(QStringLiteral("Starting"));
    if (!session->backend->start(shell, QSize(160, 42), false)) session->status->setText(QStringLiteral("Unavailable"));
    else session->status->setText(QStringLiteral("QProcess"));
    updateSessionUi(session);
}

void MainWindow::stopProcess(ConsoleSession *session)
{
    if (!session) return;
    if (session->backend) {
        session->backend->stop();
        session->backend->deleteLater();
        session->backend = nullptr;
    }
    if (session->process) {
        session->process->terminate();
        if (!session->process->waitForFinished(250)) session->process->kill();
        session->process->deleteLater();
        session->process = nullptr;
    }
}

void MainWindow::updateSessionUi(ConsoleSession *session)
{
    if (!session) return;
    session->shellBadge->setText(shellLabel(session->shell));
}

void MainWindow::sendCommand() {}

void MainWindow::readOutput()
{
    auto *process = qobject_cast<QProcess *>(sender());
    if (!process) return;
    for (ConsoleSession *session : sessions_) {
        if (session->process != process) continue;
        const QByteArray bytes = process->readAllStandardOutput();
        QString text = QString::fromLocal8Bit(bytes);
        text.remove(QRegularExpression(QString::fromLatin1(R"(\x1b(?:[@-Z\\-_]|\[[0-?]*[ -/]*[@-~]|\][^\x07\x1b]*(?:\x07|\x1b\\)))")));
        appendOutput(session, text);
        return;
    }
}

void MainWindow::writeCommand(ConsoleSession *session, const QString &rawCommand) {
    if (!session || (!session->backend && !session->process)) return;
    QString command = rawCommand;
    
    const QByteArray input = (command + QStringLiteral("\r\n")).toUtf8();
    
    if (auto *cv = dynamic_cast<TerminalCanvas *>(session->output)) cv->lastSentCmd = command; if (session->backend) { session->backend->sendCommand(command); return; }
    else if (session->process) {
        session->process->write(input);
        session->process->waitForBytesWritten(50);
    }
}

void MainWindow::appendOutput(ConsoleSession *session, const QString &text)
{
    if (!session || !session->output) return;
    session->output->moveCursor(QTextCursor::End);
    if(auto*cv=dynamic_cast<TerminalCanvas*>(session->output))cv->appendSystemText(text);else session->output->insertPlainText(text);
    session->output->verticalScrollBar()->setValue(session->output->verticalScrollBar()->maximum());
    session->output->horizontalScrollBar()->setValue(0);
}

void MainWindow::launchShell(const QString &shell, bool elevated)
{
    if (ConsoleSession *session = currentSession()) startProcess(session, shell, elevated);
}

void MainWindow::runBind(const CommandBind &bind)
{
    ConsoleSession *session = currentSession();
    if (!session || (!session->backend && !session->process)) return;
    writeCommand(session, bind.command);
}

void MainWindow::createBind()
{
    bool accepted = false;
    const QString name = QInputDialog::getText(this, QStringLiteral("Новый бинд"), QStringLiteral("Название"), QLineEdit::Normal, QString(), &accepted);
    if (!accepted || name.trimmed().isEmpty()) return;
    const QString command = QInputDialog::getText(this, QStringLiteral("Новый бинд"), QStringLiteral("Команда"), QLineEdit::Normal, QString(), &accepted);
    if (!accepted || command.trimmed().isEmpty()) return;
    binds_.append({name.trimmed(), command});
    refreshBinds();
    saveSettings();
}

void MainWindow::refreshBinds()
{
    if (!bindHost_) return;
    auto *layout = qobject_cast<QVBoxLayout *>(bindHost_->layout());
    if (!layout) return;
    while (QLayoutItem *item = layout->takeAt(0)) {
        if (item->widget()) item->widget()->deleteLater();
        delete item;
    }
    for (int i = 0; i < binds_.size(); ++i) {
        auto *row = new QWidget(bindHost_);
        auto *rowLayout = new QHBoxLayout(row);
        rowLayout->setContentsMargins(0, 0, 0, 0);
        rowLayout->setSpacing(4);
        auto *run = button(binds_.at(i).name, row);
        run->setToolTip(binds_.at(i).command);
        auto *edit = button(QStringLiteral("✎"), row);
        auto *remove = button(QStringLiteral("×"), row);
        edit->setFixedWidth(30);
        remove->setFixedWidth(30);
        rowLayout->addWidget(run, 1);
        rowLayout->addWidget(edit);
        rowLayout->addWidget(remove);
        layout->addWidget(row);
        connect(run, &QPushButton::clicked, this, [this, i] { if (i < binds_.size()) runBind(binds_.at(i)); });
        connect(edit, &QPushButton::clicked, this, [this, i] {
            if (i >= binds_.size()) return;
            bool ok = false;
            const QString name = QInputDialog::getText(this, QStringLiteral("Изменить бинд"), QStringLiteral("Название"), QLineEdit::Normal, binds_.at(i).name, &ok);
            if (!ok || name.trimmed().isEmpty()) return;
            const QString command = QInputDialog::getText(this, QStringLiteral("Изменить бинд"), QStringLiteral("Команда"), QLineEdit::Normal, binds_.at(i).command, &ok);
            if (!ok || command.trimmed().isEmpty()) return;
            binds_[i] = {name.trimmed(), command};
            refreshBinds();
            saveSettings();
        });
        connect(remove, &QPushButton::clicked, this, [this, i] {
            if (i < binds_.size()) binds_.removeAt(i);
            refreshBinds();
            saveSettings();
        });
    }
    layout->addStretch(1);
}

void MainWindow::toggleDrawer()
{
    const int from = drawerOpen_ ? 270 : 0;
    const int to = drawerOpen_ ? 0 : 270;
    drawerOpen_ = !drawerOpen_;
    if (drawerOpen_) drawerContent_->setVisible(true);
    drawer_->setMinimumWidth(0);
    drawer_->setMaximumWidth(270);
    auto *animation = new QPropertyAnimation(drawer_, "maximumWidth", this);
    animation->setDuration(200);
    animation->setEasingCurve(QEasingCurve::OutCubic);
    animation->setStartValue(from);
    animation->setEndValue(to);
    connect(animation, &QPropertyAnimation::valueChanged, this, [this](const QVariant &) { placeDrawerToggle(); });
    connect(animation, &QPropertyAnimation::finished, this, [this] {
        if (!drawerOpen_) drawerContent_->setVisible(false);
        drawer_->setMinimumWidth(drawerOpen_ ? 270 : 0);
        drawer_->setMaximumWidth(drawerOpen_ ? 270 : 0);
        drawerToggle_->setText(drawerOpen_ ? QStringLiteral("‹") : QStringLiteral("›"));
        placeDrawerToggle();
    });
    animation->start(QAbstractAnimation::DeleteWhenStopped);
}

void MainWindow::placeDrawerToggle()
{
    if (!drawerToggle_ || !root_ || !drawer_) return;
    const QPoint edge = drawer_->mapTo(root_, QPoint(0, root_->height() / 2));
    drawerToggle_->move(edge.x() - drawerToggle_->width() / 2, edge.y() - drawerToggle_->height() / 2);
    drawerToggle_->raise();
}

void MainWindow::setNotesVisible(bool visible)
{
    if (notesVisible_ == visible) return;
    if (visible) {
        QSettings s(QStringLiteral("ConsoleV1"), QStringLiteral("TerminalApp"));
        const QString savedPin = s.value(QStringLiteral("notesPin")).toString();
        if (!savedPin.isEmpty()) {
            bool ok = false;
            const QString entered = QInputDialog::getText(this, QStringLiteral("Доступ к заметкам"), QStringLiteral("Введите 4-значный PIN-код:"), QLineEdit::Password, QString(), &ok);
            if (!ok || entered != savedPin) { notesNavButton_->setChecked(false); return; }
        }
    }
    notesVisible_ = visible;
    notesNavButton_->setChecked(visible);
    if (drawerToggle_) drawerToggle_->setVisible(!visible);
    contentStack_->setCurrentWidget(visible ? notesPage_ : consolePage_);
    if (!visible) {
        placeDrawerToggle();
        if (ConsoleSession *s = currentSession(); s && s->output) s->output->setFocus();
    }
}

void MainWindow::addNote()
{
    const QPointF center = noteView_->mapToScene(noteView_->viewport()->rect().center());
    createNote(center, QStringLiteral("Заметка"), QString());
}

QGraphicsObject *MainWindow::createNote(const QPointF &position, const QString &heading, const QString &body, const QString &existingId)
{
    QString id = existingId;
    if (id.isEmpty()) id = QString::number(nextNoteId_++);
    else nextNoteId_ = qMax(nextNoteId_, id.toInt() + 1);
    auto *card = new NoteCard(heading, body, accent_, id, theme_);
    noteScene_->addItem(card);
    card->setPos(position);
    noteIds_.insert(id, card);
    card->moved = [this, card] {
        for (QGraphicsPathItem *item : noteEdges_) if (auto *edge = dynamic_cast<NoteEdge *>(item); edge && edge->touches(card)) edge->refresh();
        saveNotes();
    };
    card->linkRequested = [this](NoteCard *source) {
        if (linkStart_ == source) {
            linkStart_ = nullptr;
            if (previewPath_) { noteScene_->removeItem(previewPath_); delete previewPath_; previewPath_ = nullptr; }
            if (previewGlow_) { noteScene_->removeItem(previewGlow_); delete previewGlow_; previewGlow_ = nullptr; }
            return;
        }
        linkStart_ = source;
        if (previewPath_) { noteScene_->removeItem(previewPath_); delete previewPath_; }
        previewGlow_ = noteScene_->addPath(QPainterPath(), QPen(QColor(255, 255, 255, 55), 8, Qt::SolidLine, Qt::RoundCap));
        previewGlow_->setZValue(2);
        previewPath_ = noteScene_->addPath(QPainterPath(), QPen(QColor(QStringLiteral("#ffffff")), 2.0, Qt::DashLine, Qt::RoundCap));
        previewPath_->setZValue(3);
    };
    card->deleteRequested = [this](NoteCard *note) { QTimer::singleShot(0, this, [this, note] { deleteNote(note); }); };
    card->editRequested = [this](NoteCard *note) {
        bool ok = false;
        const QString title = QInputDialog::getText(this, QStringLiteral("Заметка"), QStringLiteral("Заголовок"), QLineEdit::Normal, note->heading(), &ok);
        if (!ok) return;
        note->setHeading(title);
        const QString text = QInputDialog::getMultiLineText(this, QStringLiteral("Заметка"), QStringLiteral("Текст"), note->content(), &ok);
        if (ok) note->setContent(text);
        saveNotes();
    };
    saveNotes();
    return card;
}

void MainWindow::deleteNote(QGraphicsObject *object)
{
    auto *note = dynamic_cast<NoteCard *>(object);
    if (!note) return;
    const QList<QGraphicsItem *> items = noteScene_->items();
    const auto edges = noteEdges_;
    for (QGraphicsPathItem *item : edges) {
        auto *edge = dynamic_cast<NoteEdge *>(item);
        if (edge && edge->touches(note)) deleteEdge(edge);
    }
    noteIds_.remove(note->id());
    noteScene_->removeItem(note);
    delete note;
    if (linkStart_ == object) linkStart_ = nullptr;
    saveNotes();
}

void MainWindow::deleteEdge(QGraphicsPathItem *object)
{
    if (!object) return;
    noteEdges_.removeAll(object);
    noteScene_->removeItem(object);
    delete object;
    saveNotes();
}

void MainWindow::finishNoteLink(QGraphicsItem *target)
{
    auto *destination = dynamic_cast<NoteCard *>(target);
    auto *source = dynamic_cast<NoteCard *>(linkStart_);
    if (source && destination == source) return;
    if (source && destination) {
        const auto existingEdges = noteEdges_;
        for (QGraphicsPathItem *item : existingEdges) {
            auto *edge = dynamic_cast<NoteEdge *>(item);
            if (edge && ((edge->from() == source && edge->to() == destination) || (edge->from() == destination && edge->to() == source))) {
                linkStart_ = nullptr;
                if (previewPath_) { noteScene_->removeItem(previewPath_); delete previewPath_; previewPath_ = nullptr; }
                if (previewGlow_) { noteScene_->removeItem(previewGlow_); delete previewGlow_; previewGlow_ = nullptr; }
                return;
            }
        }
        auto *edge = new NoteEdge(source, destination);
        edge->deleteRequested = [this](NoteEdge *line) { QTimer::singleShot(0, this, [this, line] { deleteEdge(line); }); };
        noteEdges_.append(edge);
        noteScene_->addItem(edge);
    }
    linkStart_ = nullptr;
    if (previewPath_) { noteScene_->removeItem(previewPath_); delete previewPath_; previewPath_ = nullptr; }
    if (previewGlow_) { noteScene_->removeItem(previewGlow_); delete previewGlow_; previewGlow_ = nullptr; }
    saveNotes();
}

void MainWindow::saveNotes() const
{
    if (!noteScene_ || loadingNotes_) return;
    QJsonArray cards;
    QJsonArray edges;
    for (QGraphicsItem *item : noteScene_->items()) {
        if (const auto *card = dynamic_cast<const NoteCard *>(item)) {
            cards.append(QJsonObject{{QStringLiteral("id"), card->id()}, {QStringLiteral("title"), card->heading()}, {QStringLiteral("body"), card->content()}, {QStringLiteral("x"), card->pos().x()}, {QStringLiteral("y"), card->pos().y()}});
        } else if (const auto *edge = dynamic_cast<const NoteEdge *>(item)) {
            edges.append(QJsonObject{{QStringLiteral("from"), edge->from()->id()}, {QStringLiteral("to"), edge->to()->id()}});
        }
    }
    const QString directory = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(directory);
    QFile file(directory + QStringLiteral("/notes.json"));
    if (file.open(QIODevice::WriteOnly | QIODevice::Truncate)) file.write(QJsonDocument(QJsonObject{{QStringLiteral("cards"), cards}, {QStringLiteral("edges"), edges}}).toJson());
}

void MainWindow::loadNotes()
{
    QFile file(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + QStringLiteral("/notes.json"));
    if (!file.open(QIODevice::ReadOnly)) return;
    const QJsonObject document = QJsonDocument::fromJson(file.readAll()).object();
    loadingNotes_ = true;
    QHash<QString, NoteCard *> cards;
    for (const QJsonValue &value : document.value(QStringLiteral("cards")).toArray()) {
        const QJsonObject object = value.toObject();
        const QString id = object.value(QStringLiteral("id")).toString();
        auto *card = dynamic_cast<NoteCard *>(createNote(QPointF(object.value(QStringLiteral("x")).toDouble(), object.value(QStringLiteral("y")).toDouble()), object.value(QStringLiteral("title")).toString(), object.value(QStringLiteral("body")).toString(), id));
        cards.insert(id, card);
    }
    for (const QJsonValue &value : document.value(QStringLiteral("edges")).toArray()) {
        const QJsonObject edge = value.toObject();
        if (cards.contains(edge.value(QStringLiteral("from")).toString()) && cards.contains(edge.value(QStringLiteral("to")).toString()))
        {
            NoteCard *from = cards.value(edge.value(QStringLiteral("from")).toString());
            NoteCard *to = cards.value(edge.value(QStringLiteral("to")).toString());
            bool duplicate = false;
            for (QGraphicsPathItem *existing : noteEdges_) {
                auto *prior = dynamic_cast<NoteEdge *>(existing);
                if (prior && ((prior->from() == from && prior->to() == to) || (prior->from() == to && prior->to() == from))) { duplicate = true; break; }
            }
            if (!duplicate) {
                auto *line = new NoteEdge(from, to);
                line->deleteRequested = [this](NoteEdge *item) { QTimer::singleShot(0, this, [this, item] { deleteEdge(item); }); };
                noteEdges_.append(line);
                noteScene_->addItem(line);
            }
        }
    }
    loadingNotes_ = false;
}

void MainWindow::openSettings()
{
    QDialog dialog(this);
    dialog.setWindowTitle(QStringLiteral("Настройки"));
    dialog.setMinimumWidth(460);
    auto *layout = new QVBoxLayout(&dialog);
    auto *search = new QLineEdit(&dialog);
    search->setPlaceholderText(QStringLiteral("🔍 Поиск по настройкам..."));
    layout->addWidget(search);
    auto *cursorCard = new QFrame(&dialog);
    cursorCard->setProperty("searchText", QStringLiteral("курсор терминала стиль цвет блок подчёркивание черта cursor"));
    auto *cursorLayout = new QVBoxLayout(cursorCard);
    cursorLayout->addWidget(new QLabel(QStringLiteral("Курсор терминала"), cursorCard));
    auto *cursorRow = new QHBoxLayout;
    auto *cursorStyleCombo = new QComboBox(cursorCard);
    cursorStyleCombo->addItem(QStringLiteral("Блок  █"), QStringLiteral("block"));
    cursorStyleCombo->addItem(QStringLiteral("Подчёркивание  ▁"), QStringLiteral("underline"));
    cursorStyleCombo->addItem(QStringLiteral("Черта  ▏"), QStringLiteral("bar"));
    auto *cursorColorCombo = new QComboBox(cursorCard);
    cursorColorCombo->addItem(QStringLiteral("Фиолетовый"), QStringLiteral("#a78bfa"));
    cursorColorCombo->addItem(QStringLiteral("Белый"), QStringLiteral("#f8fafc"));
    cursorColorCombo->addItem(QStringLiteral("Зелёный"), QStringLiteral("#4ade80"));
    cursorColorCombo->addItem(QStringLiteral("Голубой"), QStringLiteral("#38bdf8"));
    cursorColorCombo->addItem(QStringLiteral("Янтарный"), QStringLiteral("#facc15"));
    cursorColorCombo->addItem(QStringLiteral("Розовый"), QStringLiteral("#f472b6"));
    {
        QSettings cs(QStringLiteral("ConsoleV1"), QStringLiteral("TerminalApp"));
        cursorStyleCombo->setCurrentIndex(qMax(0, cursorStyleCombo->findData(cs.value(QStringLiteral("cursorStyle"), QStringLiteral("block")).toString())));
        cursorColorCombo->setCurrentIndex(qMax(0, cursorColorCombo->findData(cs.value(QStringLiteral("cursorColor"), QStringLiteral("#a78bfa")).toString())));
    }
    auto saveCursorPrefs = [cursorStyleCombo, cursorColorCombo] {
        QSettings cs(QStringLiteral("ConsoleV1"), QStringLiteral("TerminalApp"));
        cs.setValue(QStringLiteral("cursorStyle"), cursorStyleCombo->currentData().toString());
        cs.setValue(QStringLiteral("cursorColor"), cursorColorCombo->currentData().toString());
    };
    connect(cursorStyleCombo, qOverload<int>(&QComboBox::currentIndexChanged), &dialog, [saveCursorPrefs](int){ saveCursorPrefs(); });
    connect(cursorColorCombo, qOverload<int>(&QComboBox::currentIndexChanged), &dialog, [saveCursorPrefs](int){ saveCursorPrefs(); });
    cursorRow->addWidget(cursorStyleCombo, 1);
    cursorRow->addWidget(cursorColorCombo, 1);
    cursorLayout->addLayout(cursorRow);
    layout->addWidget(cursorCard);

    auto *themeCard = new QFrame(&dialog);
    themeCard->setProperty("searchText", QStringLiteral("тема интерфейса глубокий космос deep space графит graphite обсидиан obsidian"));
    auto *themeLayout = new QVBoxLayout(themeCard);
    auto *themeLabel = new QLabel(QStringLiteral("Тема интерфейса"), themeCard);
    auto *themeCombo = new QComboBox(themeCard);
    const QStringList paletteNames{QStringLiteral("Глубокий Космос"), QStringLiteral("Графит Pro"), QStringLiteral("Обсидиан"), QStringLiteral("Изумрудная матрица"), QStringLiteral("Киберпанк Неон"), QStringLiteral("Янтарный Закат"), QStringLiteral("Северное Сияние"), QStringLiteral("Красный Марс")};
    const QStringList paletteKeys{QStringLiteral("Deep Space"), QStringLiteral("Graphite Pro"), QStringLiteral("Obsidian"), QStringLiteral("Emerald Matrix"), QStringLiteral("Cyberpunk Neon"), QStringLiteral("Amber Sunset"), QStringLiteral("Northern Lights"), QStringLiteral("Red Mars")};
    const QStringList swatches{QStringLiteral("#b78cff"),QStringLiteral("#a9c4d8"),QStringLiteral("#e6edf3"),QStringLiteral("#83e0b4"),QStringLiteral("#ff4fa3"),QStringLiteral("#f3b45f"),QStringLiteral("#72e1d2"),QStringLiteral("#ff786f")};
    for (int i = 0; i < paletteNames.size(); ++i) themeCombo->addItem(paletteNames.at(i), paletteKeys.at(i));
    themeCombo->setCurrentIndex(qMax(0, themeCombo->findData(theme_)));
    themeLayout->addWidget(themeLabel);
    themeLayout->addWidget(themeCombo);
    auto *paletteGrid = new QGridLayout;
    paletteGrid->setSpacing(6);
    for (int i = 0; i < paletteNames.size(); ++i) {
        auto *tile = button(paletteNames.at(i), themeCard);
        tile->setCheckable(true);
        tile->setStyleSheet(QStringLiteral("QPushButton{border:1px solid %1;text-align:left;}QPushButton:checked{background:%2;border:1px solid %1;}").arg(swatches.at(i), rgba(swatches.at(i), 35)));
        tile->setChecked(themeCombo->currentData().toString() == paletteKeys.at(i));
        paletteGrid->addWidget(tile, i / 2, i % 2);
        connect(tile, &QPushButton::clicked, &dialog, [this, themeCombo, tile, paletteGrid, i] {
            if (themeCombo->currentIndex() != i) themeCombo->setCurrentIndex(i);
            else applySettings(themeAccent(themeCombo->itemData(i).toString()), consoleAlpha_, fontSize_, uiScale_, themeCombo->itemData(i).toString(), starsEnabled_, windowOpacity_);
            for (int j = 0; j < paletteGrid->count(); ++j) if (auto *other = qobject_cast<QPushButton *>(paletteGrid->itemAt(j)->widget())) other->setChecked(other == tile);
        });
    }
    themeLayout->addLayout(paletteGrid);
    layout->addWidget(themeCard);
    auto *starsToggle = new QCheckBox(QStringLiteral("Анимация звёзд и метеоров"), &dialog);
    starsToggle->setProperty("searchText", QStringLiteral("анимация звёзд stars метеоры meteors фон background"));
    starsToggle->setChecked(starsEnabled_);
    layout->addWidget(starsToggle);
    auto *accentCard = new QFrame(&dialog);
    accentCard->setProperty("searchText", QStringLiteral("акцентный цвет палитра accent color"));
    auto *accentLayout = new QVBoxLayout(accentCard);
    auto *accentRow = new QHBoxLayout;
    auto *accentLabel = new QLabel(QStringLiteral("Акцентный цвет"), accentCard);
    auto *colorButton = button(QStringLiteral("Выбрать цвет"), accentCard);
    accentRow->addWidget(accentLabel);
    accentRow->addStretch();
    accentRow->addWidget(colorButton);
    accentLayout->addLayout(accentRow);
    layout->addWidget(accentCard);
    QString selectedAccent = accent_;
    connect(colorButton, &QPushButton::clicked, &dialog, [&dialog, &selectedAccent] {
        const QColor selected = QColorDialog::getColor(QColor(selectedAccent), &dialog, QStringLiteral("Акцентный цвет"));
        if (selected.isValid()) selectedAccent = selected.name();
    });
    auto *windowOpacityCard = new QFrame(&dialog);
    windowOpacityCard->setProperty("searchText", QStringLiteral("общая прозрачность окна opacity window"));
    auto *windowOpacityLayout = new QVBoxLayout(windowOpacityCard);
    auto *windowOpacityLabel = new QLabel(QStringLiteral("Общая прозрачность окна: %1%").arg(windowOpacity_), windowOpacityCard);
    auto *windowOpacitySlider = new QSlider(Qt::Horizontal, windowOpacityCard);
    windowOpacitySlider->setRange(40, 100);
    windowOpacitySlider->setValue(windowOpacity_);
    windowOpacityLayout->addWidget(windowOpacityLabel);
    windowOpacityLayout->addWidget(windowOpacitySlider);
    layout->addWidget(windowOpacityCard);
    connect(windowOpacitySlider, &QSlider::valueChanged, this, [&dialog, this, windowOpacityLabel](int value) { windowOpacity_ = value; setWindowOpacity(value / 100.0); dialog.setWindowOpacity(value / 100.0); windowOpacityLabel->setText(QStringLiteral("Общая прозрачность окна: %1%").arg(value)); saveSettings(); });
    auto *alphaCard = new QFrame(&dialog);
    alphaCard->setProperty("searchText", QStringLiteral("прозрачность подложки консоли альфа transparency opacity alpha"));
    auto *alphaLayout = new QVBoxLayout(alphaCard);
    auto *alphaLabel = new QLabel(QStringLiteral("Прозрачность подложки консоли: %1").arg(consoleAlpha_), alphaCard);
    auto *alpha = new QSlider(Qt::Horizontal, alphaCard);
    alpha->setRange(120, 245);
    alpha->setValue(consoleAlpha_);
    alphaLayout->addWidget(alphaLabel);
    alphaLayout->addWidget(alpha);
    connect(alpha, &QSlider::valueChanged, &dialog, [this, alphaLabel](int value) { consoleAlpha_ = value; for (ConsoleSession *session : sessions_) { if (auto *card = session->page->findChild<QFrame *>(QStringLiteral("consoleCard"))) card->setStyleSheet(QStringLiteral("QFrame#consoleCard{background:%1;border:1px solid rgba(255,255,255,0.1);border-radius:16px;}").arg(rgba(consoleBase(theme_), value))); } alphaLabel->setText(QStringLiteral("Прозрачность подложки консоли: %1").arg(value)); saveSettings(); });
    layout->addWidget(alphaCard);
    auto *fontCard = new QFrame(&dialog);
    fontCard->setProperty("searchText", QStringLiteral("размер шрифта текст font size"));
    auto *fontLayout = new QVBoxLayout(fontCard);
    auto *fontRow = new QHBoxLayout;
    fontRow->addWidget(new QLabel(QStringLiteral("Размер шрифта"), fontCard));
    auto *fontSize = new QSpinBox(fontCard);
    fontSize->setRange(10, 24);
    fontSize->setValue(fontSize_);
    fontRow->addWidget(fontSize);
    fontLayout->addLayout(fontRow);
    layout->addWidget(fontCard);
    auto *scaleCard = new QFrame(&dialog);
    scaleCard->setProperty("searchText", QStringLiteral("масштаб интерфейса размер scale ui"));
    auto *scaleLayout = new QVBoxLayout(scaleCard);
    auto *scaleLabel = new QLabel(QStringLiteral("Масштаб интерфейса"), scaleCard);
    auto *scale = new QSlider(Qt::Horizontal, scaleCard);
    scale->setRange(80, 125);
    scale->setValue(uiScale_);
    scaleLayout->addWidget(scaleLabel);
    scaleLayout->addWidget(scale);
    layout->addWidget(scaleCard);
    connect(themeCombo, qOverload<int>(&QComboBox::currentIndexChanged), &dialog, [this, &selectedAccent, themeCombo, alpha, fontSize, scale, starsToggle, windowOpacitySlider, paletteGrid](int index) {
        if (index < 0) return;
        const QString key = themeCombo->itemData(index).toString();
        const QString selected = themeAccent(key);
        selectedAccent = selected;
        applySettings(selected, alpha->value(), fontSize->value(), scale->value(), key, starsToggle->isChecked(), windowOpacitySlider->value());
        for (int i = 0; i < paletteGrid->count(); ++i) if (auto *tile = qobject_cast<QPushButton *>(paletteGrid->itemAt(i)->widget())) tile->setChecked(i == index);
    });
    auto *buttons = new QDialogButtonBox(QDialogButtonBox::Save | QDialogButtonBox::Cancel, &dialog);
    layout->addWidget(buttons);
    const QString dialogStyle = appStyle(accent_, consoleAlpha_, uiScale_, theme_);
    search->setObjectName(QStringLiteral("settingsSearch"));
    dialog.setStyleSheet(dialogStyle + QStringLiteral("QDialog{background:%1;}QFrame{background:rgba(255,255,255,0.035);border:1px solid rgba(255,255,255,0.08);border-radius:12px;}QLineEdit#settingsSearch{background:rgba(255,255,255,0.06);}").arg(theme_ == QStringLiteral("Graphite") ? QStringLiteral("#1e2024") : QStringLiteral("#0d0f18")));
    const QList<QWidget *> searchable{themeCard, starsToggle, accentCard, windowOpacityCard, alphaCard, fontCard, scaleCard};
    connect(search, &QLineEdit::textChanged, &dialog, [searchable, search, accent = accent_](const QString &query) {
        const QString needle = query.trimmed();
        for (QWidget *widget : searchable) {
            const bool match = needle.isEmpty() || widget->property("searchText").toString().contains(needle, Qt::CaseInsensitive);
            widget->setVisible(match);
            if (auto *frame = qobject_cast<QFrame *>(widget)) frame->setStyleSheet(match && !needle.isEmpty() ? QStringLiteral("QFrame{border:1px solid %1;}").arg(accent) : QString());
        }
    });
    connect(buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    if (dialog.exec() == QDialog::Accepted) applySettings(selectedAccent, alpha->value(), fontSize->value(), scale->value(), themeCombo->currentData().toString(), starsToggle->isChecked(), windowOpacitySlider->value());
}

void MainWindow::applySettings(const QString &accent, int alpha, int fontSize, int scale, const QString &theme, bool stars, int windowOpacity)
{
    accent_ = accent;
    consoleAlpha_ = alpha;
    fontSize_ = fontSize;
    uiScale_ = scale;
    theme_ = theme;
    starsEnabled_ = stars;
    windowOpacity_ = windowOpacity;
    accent_ = themeAccent(theme_);
    setWindowOpacity(windowOpacity_ / 100.0);
    setStyleSheet(appStyle(accent_, consoleAlpha_, uiScale_, theme_));
    starfield_->setTheme(theme_);
    starfield_->setAnimated(starsEnabled_);
    for (ConsoleSession *session : sessions_) {
        QFont font = session->output->font();
        font.setPointSize(fontSize_);
        session->output->setFont(font);
        if (auto *card = session->page->findChild<QFrame *>(QStringLiteral("consoleCard"))) card->setStyleSheet(QStringLiteral("QFrame#consoleCard{background:%1;border:1px solid rgba(255,255,255,0.1);border-radius:16px;}").arg(rgba(consoleBase(theme_), consoleAlpha_)));
    }
    for (QGraphicsItem *item : noteScene_->items()) {
        if (auto *card = dynamic_cast<NoteCard *>(item)) { card->setAccent(accent_); card->setTheme(theme_); }
    }
    saveSettings();
    saveNotes();
}

void MainWindow::loadSettings()
{
    QSettings settings(QStringLiteral("Console-v1"), QStringLiteral("Console-v1"));
    accent_ = settings.value(QStringLiteral("accent"), accent_).toString();
    consoleAlpha_ = settings.value(QStringLiteral("alpha"), consoleAlpha_).toInt();
    fontSize_ = settings.value(QStringLiteral("fontSize"), fontSize_).toInt();
    uiScale_ = settings.value(QStringLiteral("uiScale"), uiScale_).toInt();
    windowOpacity_ = settings.value(QStringLiteral("windowOpacity"), windowOpacity_).toInt();
    theme_ = settings.value(QStringLiteral("theme"), theme_).toString();
    starsEnabled_ = settings.value(QStringLiteral("starsEnabled"), starsEnabled_).toBool();
    const QVariantList stored = settings.value(QStringLiteral("binds")).toList();
    for (const QVariant &value : stored) {
        const QVariantMap map = value.toMap();
        binds_.append({map.value(QStringLiteral("name")).toString(), map.value(QStringLiteral("command")).toString()});
    }
}

void MainWindow::saveSettings() const
{
    QSettings settings(QStringLiteral("Console-v1"), QStringLiteral("Console-v1"));
    settings.setValue(QStringLiteral("accent"), accent_);
    settings.setValue(QStringLiteral("alpha"), consoleAlpha_);
    settings.setValue(QStringLiteral("fontSize"), fontSize_);
    settings.setValue(QStringLiteral("uiScale"), uiScale_);
    settings.setValue(QStringLiteral("windowOpacity"), windowOpacity_);
    settings.setValue(QStringLiteral("theme"), theme_);
    settings.setValue(QStringLiteral("starsEnabled"), starsEnabled_);
    QVariantList values;
    for (const CommandBind &bind : binds_) values.append(QVariantMap{{QStringLiteral("name"), bind.name}, {QStringLiteral("command"), bind.command}});
    settings.setValue(QStringLiteral("binds"), values);
}

bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() == QEvent::Enter || event->type() == QEvent::Leave) {
        auto *effect = qobject_cast<QGraphicsOpacityEffect *>(qobject_cast<QWidget *>(watched) ? qobject_cast<QWidget *>(watched)->graphicsEffect() : nullptr);
        if (effect) {
            auto *animation = new QPropertyAnimation(effect, "opacity", effect);
            animation->setDuration(200);
            animation->setEasingCurve(QEasingCurve::OutCubic);
            animation->setStartValue(effect->opacity());
            animation->setEndValue(event->type() == QEvent::Enter ? 1.0 : 0.94);
            animation->start(QAbstractAnimation::DeleteWhenStopped);
        }
    }
    if (event->type() == QEvent::Enter || event->type() == QEvent::Leave) {
        if (auto *control = qobject_cast<QPushButton *>(qobject_cast<QWidget *>(watched))) {
            const bool hovered = event->type() == QEvent::Enter;
            if (control->objectName() == QStringLiteral("trafficRed")) control->setText(hovered ? QStringLiteral("×") : QString());
            else if (control->objectName() == QStringLiteral("trafficYellow")) control->setText(hovered ? QStringLiteral("−") : QString());
            else if (control->objectName() == QStringLiteral("trafficGreen")) control->setText(hovered ? QStringLiteral("+") : QString());
        }
        if (auto *widget = qobject_cast<QWidget *>(watched)) {
            if (auto *effect = qobject_cast<QGraphicsOpacityEffect *>(widget->graphicsEffect())) {
                auto *animation = new QPropertyAnimation(effect, "opacity", effect);
                animation->setDuration(200);
                animation->setEasingCurve(QEasingCurve::OutCubic);
                animation->setStartValue(effect->opacity());
                animation->setEndValue(event->type() == QEvent::Enter ? 1.0 : 0.96);
                animation->start(QAbstractAnimation::DeleteWhenStopped);
            }
        }
    }
    
    if (event->type() == QEvent::Resize) {
        for (ConsoleSession *session : sessions_) {
            if (session->output != watched || !session->backend) continue;
            const QSize pixels = session->output->size();
            session->backend->resizeTerminal(QSize(qMax(40, int((pixels.width() - 24) / qMax(1.0, QFontMetricsF(session->output->font()).horizontalAdvance(QLatin1Char('M'))))), qMax(15, int((session->output->viewport()->height() - 12) / 19.0))));
            break;
        }
    }
    return QMainWindow::eventFilter(watched, event);
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
    if (starfield_ && root_) starfield_->setGeometry(root_->rect());
    for (ConsoleSession *session : sessions_) {
        if (!session->backend || session->page != sessionStack_->currentWidget()) continue;
        const QSize pixels = session->output ? session->output->size() : QSize(1080, 540);
        session->backend->resizeTerminal(QSize(qMax(40, int((pixels.width() - 24) / qMax(1.0, QFontMetricsF(session->output->font()).horizontalAdvance(QLatin1Char('M'))))), qMax(15, int((session->output->viewport()->height() - 12) / 19.0))));
    }
    placeDrawerToggle();
}

void MainWindow::showEvent(QShowEvent *event)
{
    QMainWindow::showEvent(event);
    placeDrawerToggle();
}

bool MainWindow::nativeEvent(const QByteArray &eventType, void *message, qintptr *result)
{
#ifdef Q_OS_WIN
    auto *msg = static_cast<MSG *>(message);
    if (msg && msg->message == WM_NCCALCSIZE && msg->wParam == TRUE) {
        *result = 0;
        return true;
    }
    if (msg && msg->message == WM_NCHITTEST) {
        const qreal dpr = devicePixelRatioF();
        const POINT point{GET_X_LPARAM(msg->lParam), GET_Y_LPARAM(msg->lParam)};
        RECT rect{};
        const HWND hwnd = reinterpret_cast<HWND>(winId());
        if (!GetWindowRect(hwnd, &rect)) return false;
        const int border = qMax(1, int(8 * dpr));
        const int corner = qMax(border, int(12 * dpr));

        if (!IsZoomed(hwnd)) {
            const bool left = point.x >= rect.left && point.x < rect.left + border;
            const bool right = point.x < rect.right && point.x >= rect.right - border;
            const bool top = point.y >= rect.top && point.y < rect.top + border;
            const bool bottom = point.y < rect.bottom && point.y >= rect.bottom - border;
            const bool cornerLeft = point.x >= rect.left && point.x < rect.left + corner;
            const bool cornerRight = point.x < rect.right && point.x >= rect.right - corner;
            const bool cornerTop = point.y >= rect.top && point.y < rect.top + corner;
            const bool cornerBottom = point.y < rect.bottom && point.y >= rect.bottom - corner;
            if (cornerTop && cornerLeft) { *result = HTTOPLEFT; return true; }
            if (cornerTop && cornerRight) { *result = HTTOPRIGHT; return true; }
            if (cornerBottom && cornerLeft) { *result = HTBOTTOMLEFT; return true; }
            if (cornerBottom && cornerRight) { *result = HTBOTTOMRIGHT; return true; }
            if (left) { *result = HTLEFT; return true; }
            if (right) { *result = HTRIGHT; return true; }
            if (top) { *result = HTTOP; return true; }
            if (bottom) { *result = HTBOTTOM; return true; }
        }

        const QPoint localPos = mapFromGlobal(QPoint(qRound(point.x / dpr), qRound(point.y / dpr)));
        if (localPos.y() >= 0 && localPos.y() <= 40) {
            QWidget *child = childAt(localPos);
            if (!child || (!qobject_cast<QAbstractButton *>(child) && !qobject_cast<QLineEdit *>(child))) {
                *result = HTCAPTION;
                return true;
            }
        }
        return false;
    }
#endif
    return QMainWindow::nativeEvent(eventType, message, result);
}
