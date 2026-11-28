#include <QButtonGroup>
#include <QMovie>
#include <QMimeData>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QImageReader>
#ifdef Q_OS_WIN
#include <windows.h>
#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>
#include <mferror.h>
#include <mmsystem.h>
#endif
#include <QShortcut>
#include <QDesktopServices>
#include <QUrl>
#include <QHash>
static QHash<QString, QString> s_sftpSessionPasswords;
#include "mainwindow.h"

static QStringList s_sftpBackHistory;
static QStringList s_sftpForwardHistory;
static QStringList s_sftpCutRemotePaths;


#include <QApplication>
#include <QCoreApplication>
#include <QClipboard>
#include <QContextMenuEvent>
#include <QCheckBox>
#include <QColorDialog>
#include <QComboBox>
#include <QDir>
#include <QMimeData>
#include <QUrl>
#include <QFile>
#include <QFileDialog>
#include <QDialog>
#include <QFileIconProvider>
#include <QFileInfo>
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
#include <QFontMetricsF>
#include <QGraphicsDropShadowEffect>
#include <QLayout>
#include <QMessageBox>
#include <QMouseEvent>
#include <QPainter>
#include <QMenu>
#include <QGraphicsProxyWidget>
#include <QTextEdit>
#include <QAbstractButton>
#include <QAbstractTextDocumentLayout>
#include <QScrollBar>
#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QPointer>
#include <QPushButton>
#include <QPixmap>
#include <QRegularExpression>
#include <QResizeEvent>
#include <QScrollArea>
#include <QSizeGrip>
#include <QTreeWidget>
#include <QHeaderView>
#include <QSettings>
#include <QSlider>
#include <QSizePolicy>
#include <QSpinBox>
#include <QStackedWidget>
#include <QStandardPaths>
#include <QTextCursor>
#include <QStringDecoder>
#include <QVBoxLayout>
#include <QPropertyAnimation>
#include <QEasingCurve>
#include <QScreen>
#include <QGuiApplication>
#include <QTimer>
#include <QTemporaryDir>
#include <QTemporaryFile>
#include <QRandomGenerator>
#include <QTime>
#include <QShowEvent>
#include <QFrame>
#include <QWindow>

#ifdef Q_OS_WIN
#include <windows.h>
#include <windowsx.h>
#include <dwmapi.h>
#include <shellapi.h>
#endif

#include <algorithm>
#include <cmath>
#include <functional>
#include <memory>

static int s_activeNoteBoard = 0;
static QString s_noteSelBorderColor = QStringLiteral("#ffffff");
static QString s_noteSelTextColor = QStringLiteral("#ffffff");
static int s_selectedAudioDeviceId = -1;
static QStringList s_noteBoardNames = {QStringLiteral("Основное поле")};
static QString activeNotesFilePath() {
    const QString dir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(dir);
    return (s_activeNoteBoard == 0)
        ? (dir + QStringLiteral("/notes.json"))
        : QStringLiteral("%1/notes_board_%2.json").arg(dir).arg(s_activeNoteBoard);
}
namespace {
constexpr auto kSettingsOrg = "ConsoleV1";
constexpr auto kSettingsApp = "TerminalApp";

QString rgba(const QString &hex, int alpha)
{
    QColor color(hex);
    return QStringLiteral("rgba(%1,%2,%3,%4)").arg(color.red()).arg(color.green()).arg(color.blue()).arg(alpha);
}

QString themeAccent(const QString &theme)
{
    if (theme == QStringLiteral("Graphite Pro") || theme == QStringLiteral("Graphite")) return QStringLiteral("#7eb8da");
    if (theme == QStringLiteral("Obsidian")) return QStringLiteral("#d7dde5");
    if (theme == QStringLiteral("Emerald Matrix")) return QStringLiteral("#5ecf9a");
    if (theme == QStringLiteral("Cyberpunk Neon")) return QStringLiteral("#39c5cf");
    if (theme == QStringLiteral("Amber Sunset")) return QStringLiteral("#e0a45a");
    if (theme == QStringLiteral("Northern Lights")) return QStringLiteral("#5bc4b8");
    if (theme == QStringLiteral("Red Mars")) return QStringLiteral("#e06b63");
    if (theme == QStringLiteral("Deep Space")) return QStringLiteral("#8aa4c8");
    return QStringLiteral("#7eb8da");
}

QString consoleBase(const QString &theme)
{
    if (theme == QStringLiteral("Graphite Pro") || theme == QStringLiteral("Graphite")) return QStringLiteral("#14161a");
    if (theme == QStringLiteral("Obsidian")) return QStringLiteral("#0a0b0e");
    if (theme == QStringLiteral("Emerald Matrix")) return QStringLiteral("#0a1611");
    if (theme == QStringLiteral("Cyberpunk Neon")) return QStringLiteral("#0d1418");
    if (theme == QStringLiteral("Amber Sunset")) return QStringLiteral("#17110c");
    if (theme == QStringLiteral("Northern Lights")) return QStringLiteral("#0a1518");
    if (theme == QStringLiteral("Red Mars")) return QStringLiteral("#160e10");
    return QStringLiteral("#10131a");
}

QString appStyle(const QString &accent, int alpha, int scale, const QString &theme)
{
    QString background = QStringLiteral("#12141a");
    QString panel = QStringLiteral("#1a1d24");
    QString raised = QStringLiteral("#242830");
    QString accentColor = accent.isEmpty() ? themeAccent(theme) : accent;
    QString hover = QStringLiteral("#2c313b");
    QString border = QStringLiteral("rgba(255,255,255,0.09)");
    QString glass = rgba(consoleBase(theme), alpha);
    QString text = QStringLiteral("#e8eaef");
    QString muted = QStringLiteral("#8b909c");

    if (theme == QStringLiteral("Graphite Pro") || theme == QStringLiteral("Graphite")) {
        background = QStringLiteral("#17191d"); panel = QStringLiteral("#21242a"); raised = QStringLiteral("#2c3038");
        accentColor = QStringLiteral("#7eb8da"); border = QStringLiteral("#363b44"); hover = QStringLiteral("#353a44");
        glass = rgba(QStringLiteral("#14161a"), alpha);
    } else if (theme == QStringLiteral("Obsidian")) {
        background = QStringLiteral("#08090c"); panel = QStringLiteral("#101115"); raised = QStringLiteral("#181a20");
        accentColor = QStringLiteral("#d7dde5"); border = QStringLiteral("#262830");
    } else if (theme == QStringLiteral("Emerald Matrix")) {
        background = QStringLiteral("#0a1410"); panel = QStringLiteral("#12201a"); raised = QStringLiteral("#1a2e25");
        accentColor = QStringLiteral("#5ecf9a"); border = QStringLiteral("#254536"); hover = QStringLiteral("#1f3830");
    } else if (theme == QStringLiteral("Cyberpunk Neon")) {
        background = QStringLiteral("#0b1216"); panel = QStringLiteral("#132026"); raised = QStringLiteral("#1c2d35");
        accentColor = QStringLiteral("#39c5cf"); border = QStringLiteral("#2a4550"); hover = QStringLiteral("#243840");
    } else if (theme == QStringLiteral("Amber Sunset")) {
        background = QStringLiteral("#15110d"); panel = QStringLiteral("#211a14"); raised = QStringLiteral("#2e241b");
        accentColor = QStringLiteral("#e0a45a"); border = QStringLiteral("#4a3a2a"); hover = QStringLiteral("#3a2e22");
    } else if (theme == QStringLiteral("Northern Lights")) {
        background = QStringLiteral("#0a1417"); panel = QStringLiteral("#122126"); raised = QStringLiteral("#1a3138");
        accentColor = QStringLiteral("#5bc4b8"); border = QStringLiteral("#2a4a50"); hover = QStringLiteral("#1f3a40");
    } else if (theme == QStringLiteral("Red Mars")) {
        background = QStringLiteral("#140e10"); panel = QStringLiteral("#1f1518"); raised = QStringLiteral("#2c1e22");
        accentColor = QStringLiteral("#e06b63"); border = QStringLiteral("#4a3036"); hover = QStringLiteral("#382428");
    } else if (theme == QStringLiteral("Deep Space")) {
        background = QStringLiteral("#0e1218"); panel = QStringLiteral("#161c26"); raised = QStringLiteral("#1f2734");
        accentColor = QStringLiteral("#8aa4c8"); border = QStringLiteral("#2a3545");
    }

    const QString pressed = rgba(accentColor, 55);
    const QString topbar = raised;
    const int fontPx = qMax(11, 13 * scale / 100);
    return QStringLiteral(
        "QWidget{color:%11;font-family:'Segoe UI Variable Text','Segoe UI','Noto Sans',sans-serif;font-size:%4px;}"
        "QWidget#root{background:%5;border-radius:14px;}"
        "QWidget#topBar{background:%10;border-bottom:1px solid %7;}"
        "QWidget#nav,QWidget#drawerContent{background:%6;border:1px solid %7;border-radius:12px;}"
        "QWidget#drawerPanel{background:transparent;border:0;}"
        "QFrame#consoleCard{background:%1;border:1px solid %7;border-radius:12px;}"
        "QTextEdit{background:transparent;border:0;color:%11;selection-background-color:%2;"
        "font-family:'Cascadia Code','JetBrains Mono','Consolas','DejaVu Sans Mono',monospace;}"
        "QLineEdit{background:%8;border:1px solid %7;border-radius:8px;padding:8px 11px;color:%11;selection-background-color:%2;}"
        "QLineEdit:focus{border-color:%2;}"
        "QPushButton{min-height:36px;background:rgba(255,255,255,0.07);border:1px solid rgba(255,255,255,0.10);border-radius:10px;padding:6px 14px;color:%11;}"
        "QPushButton:hover{background:rgba(255,255,255,0.14);border-color:%2;}"
        "QPushButton:pressed,QPushButton:checked{background:%3;border-color:%2;color:#fff;}"
        "QLabel#muted{color:%12;}QScrollArea{border:0;background:transparent;}"
        "QSlider::groove:horizontal{height:4px;background:%8;border-radius:2px;}"
        "QSlider::handle:horizontal{width:14px;margin:-5px 0;background:%2;border-radius:7px;}"
        "QSpinBox,QComboBox{background:%8;border:1px solid %7;border-radius:8px;padding:6px 8px;}"
        "QComboBox::drop-down{border:0;width:22px;}"
        "QMenu{background:%6;color:%11;border:1px solid %7;border-radius:8px;padding:4px;}"
        "QMenu::item{padding:6px 12px;border-radius:4px;}QMenu::item:selected{background:%9;}"
    ).arg(glass, accentColor, pressed, QString::number(fontPx), background, panel, border, raised, hover, topbar, text, muted);
}

bool parseSshInvocation(const QString &command, QString *target, QStringList *sftpOptions)
{
    const QStringList args=QProcess::splitCommand(command);
    if(args.isEmpty())return false;
    if(QFileInfo(args.first()).completeBaseName().compare(QStringLiteral("ssh"),Qt::CaseInsensitive)!=0)return false;
    QString host,user;
    QStringList options;
    auto takeValue=[&](int &i,const QString &option,const QString &sftpOption){
        if(i+1>=args.size())return false;
        options<<sftpOption<<args.at(++i);
        Q_UNUSED(option);
        return true;
    };
    for(int i=1;i<args.size();++i){
        const QString arg=args.at(i);
        if(arg==QStringLiteral("--")){if(i+1<args.size())host=args.at(i+1);break;}
        if(arg==QStringLiteral("-p")){if(!takeValue(i,arg,QStringLiteral("-P")))return false;continue;}
        if(arg==QStringLiteral("-i")||arg==QStringLiteral("-F")||arg==QStringLiteral("-J")||arg==QStringLiteral("-o")){
            if(!takeValue(i,arg,arg))return false;
            continue;
        }
        if(arg==QStringLiteral("-l")){if(i+1>=args.size())return false;user=args.at(++i);continue;}
        if(arg.startsWith(QStringLiteral("-p"))&&arg.size()>2){options<<QStringLiteral("-P")<<arg.mid(2);continue;}
        if((arg.startsWith(QStringLiteral("-i"))||arg.startsWith(QStringLiteral("-F"))||arg.startsWith(QStringLiteral("-J"))||arg.startsWith(QStringLiteral("-o")))&&arg.size()>2){options<<arg.left(2)<<arg.mid(2);continue;}
        if(arg.startsWith(QLatin1Char('-')))continue;
        host=arg;
        break;
    }
    if(host.isEmpty())return false;
    if(!user.isEmpty()&&!host.contains(QLatin1Char('@')))host=user+QLatin1Char('@')+host;
    *target=host;
    *sftpOptions=options;
    return true;
}

QString sftpQuote(const QString &value)
{
    QString escaped=value;
    escaped.replace(QLatin1Char('\\'),QStringLiteral("\\\\"));
    escaped.replace(QLatin1Char('"'),QStringLiteral("\\\""));
    escaped.replace(QLatin1Char('\n'),QStringLiteral("\\n"));
    escaped.replace(QLatin1Char('\r'),QStringLiteral("\\r"));
    return QLatin1Char('"')+escaped+QLatin1Char('"');
}

QString stripTerminalEscapes(const QByteArray &bytes)
{
    QString text = QString::fromUtf8(bytes);
    // ConPTY may encode line boundaries as cursor movement rather than CR/LF.
    // Preserve those boundaries before stripping the remaining CSI controls.
    static const QRegularExpression cursorLineBreak(QString::fromLatin1(R"(\x1B\[\d*(?:;\d*)*[HfGdE])"));
    static const QRegularExpression eraseLine(QString::fromLatin1(R"(\x1B\[\d*K)"));
    static const QRegularExpression ansi(QString::fromLatin1(R"(\x1B(?:\[[0-?]*[ -/]*[@-~]|\][^\x07]*(?:\x07|\x1B\\)))"));
    text.replace(cursorLineBreak, QStringLiteral("\n"));
    text.replace(eraseLine, QStringLiteral("\n"));
    text.remove(ansi);
    text.replace(QStringLiteral("\r\n"), QStringLiteral("\n"));
    text.replace(QLatin1Char('\r'), QLatin1Char('\n'));
    text.remove(QLatin1Char('\b'));
    return text;
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
        if (theme_ == QStringLiteral("Graphite Pro") || theme_ == QStringLiteral("Graphite")) p.fillRect(rect(), QColor("#17191d"));
        else if (theme_ == QStringLiteral("Obsidian")) p.fillRect(rect(), QColor("#08090c"));
        else if (theme_ == QStringLiteral("Emerald Matrix")) p.fillRect(rect(), QColor("#0a1410"));
        else if (theme_ == QStringLiteral("Cyberpunk Neon")) p.fillRect(rect(), QColor("#0b1216"));
        else if (theme_ == QStringLiteral("Amber Sunset")) p.fillRect(rect(), QColor("#15110d"));
        else if (theme_ == QStringLiteral("Northern Lights")) p.fillRect(rect(), QColor("#0a1417"));
        else if (theme_ == QStringLiteral("Red Mars")) p.fillRect(rect(), QColor("#140e10"));
        else {
            p.fillRect(rect(), QColor("#0e1218"));
            QRadialGradient nebula(width() * .72, height() * .18, qMax(width(), height()) * .65);
            nebula.setColorAt(0, QColor(80, 110, 150, 36));
            nebula.setColorAt(.5, QColor(40, 60, 90, 18));
            nebula.setColorAt(1, QColor(14, 18, 24, 0));
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
    QString theme_ = QStringLiteral("Graphite Pro");
    QPointF meteorPos_;
    bool animated_ = false;
    bool visible_ = true;
    bool meteorActive_ = false;
};


namespace {
QPushButton *button(const QString &text, QWidget *parent, const QString &objectName = QString())
{
    auto *result = new QPushButton(text, parent);
    result->setCursor(Qt::PointingHandCursor);
    result->setMinimumHeight(36);
    result->setToolTip(text);
    if (!objectName.isEmpty()) result->setObjectName(objectName);
    return result;
}

QIcon navigationDotIcon(bool active)
{
    QPixmap pixmap(8, 8);
    pixmap.fill(Qt::transparent);
    if (active) {
        QPainter painter(&pixmap);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setPen(Qt::NoPen);
        painter.setBrush(QColor(QStringLiteral("#FFFFFF")));
        painter.drawEllipse(QRectF(1, 1, 6, 6));
    }
    return QIcon(pixmap);
}

QIcon vectorActionIcon(const QString &name)
{
    QPixmap pixmap(18, 18);
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing);
    QPen pen(QColor(QStringLiteral("#dce5ef")), 1.6, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    painter.setPen(pen);
    if (name == QStringLiteral("edit")) {
        QPainterPath pencil;
        pencil.moveTo(4.0, 13.7);
        pencil.lineTo(12.6, 5.1);
        pencil.lineTo(15.0, 7.5);
        pencil.lineTo(6.4, 16.1);
        pencil.lineTo(3.2, 16.8);
        pencil.closeSubpath();
        painter.drawPath(pencil);
        painter.drawLine(QPointF(11.5, 6.2), QPointF(13.9, 8.6));
    } else if (name == QStringLiteral("select")) {
        pen.setStyle(Qt::DotLine);
        painter.setPen(pen);
        painter.setBrush(Qt::NoBrush);
        painter.drawRoundedRect(QRectF(3.5, 3.5, 11, 11), 2, 2);
    } else {
        painter.drawLine(QPointF(5.2, 5.2), QPointF(12.8, 12.8));
        painter.drawLine(QPointF(12.8, 5.2), QPointF(5.2, 12.8));
    }
    return QIcon(pixmap);
}

class AppleToggleSwitch final : public QAbstractButton {
    Q_OBJECT
    Q_PROPERTY(qreal knobPosition READ knobPosition WRITE setKnobPosition)
public:
    explicit AppleToggleSwitch(QWidget *parent = nullptr) : QAbstractButton(parent) {
        setCheckable(true); setFixedSize(42, 24); setCursor(Qt::PointingHandCursor);
        connect(this, &QAbstractButton::toggled, this, [this](bool on) {
            auto *animation = new QPropertyAnimation(this, "knobPosition", this);
            animation->setDuration(160); animation->setEasingCurve(QEasingCurve::OutCubic);
            animation->setStartValue(knobPosition_); animation->setEndValue(on ? 20.0 : 2.0);
            animation->start(QAbstractAnimation::DeleteWhenStopped);
        });
    }
    qreal knobPosition() const { return knobPosition_; }
    void setKnobPosition(qreal p) { knobPosition_ = p; update(); }
protected:
    void paintEvent(QPaintEvent *) override {
        QPainter p(this); p.setRenderHint(QPainter::Antialiasing);
        p.setPen(Qt::NoPen); p.setBrush(isChecked() ? QColor("#34C759") : QColor(255,255,255,45));
        p.drawRoundedRect(rect(), 12, 12);
        p.setBrush(Qt::white); p.drawEllipse(QRectF(knobPosition_, 2, 20, 20));
    }
private: qreal knobPosition_ = 2;
};

class AppleSlider final : public QSlider {
public:
    explicit AppleSlider(Qt::Orientation orientation, QWidget *parent = nullptr) : QSlider(orientation, parent) {
        setStyleSheet(QStringLiteral("QSlider::groove:horizontal{height:4px;background:rgba(255,255,255,0.16);border-radius:2px;}QSlider::sub-page:horizontal{background:#7eb8da;border-radius:2px;}QSlider::handle:horizontal{width:16px;height:16px;margin:-6px 0;background:#fff;border:1px solid rgba(0,0,0,0.12);border-radius:8px;}"));
    }
};

class AppleSegmentedControl final : public QWidget {
    Q_OBJECT
public:
    explicit AppleSegmentedControl(const QStringList &labels, int selected, QWidget *parent = nullptr) : QWidget(parent) {
        auto *row = new QHBoxLayout(this); row->setContentsMargins(3,3,3,3); row->setSpacing(2);
        for (int i=0;i<labels.size();++i) {
            auto *b = new QPushButton(labels.at(i), this); b->setCheckable(true); b->setMinimumHeight(32); b->setChecked(i==selected);
            b->setStyleSheet(QStringLiteral("QPushButton{background:transparent;border:0;border-radius:9px;padding:5px 10px;}QPushButton:checked{background:rgba(255,255,255,0.18);border:1px solid rgba(255,255,255,0.17);}"));
            row->addWidget(b,1); buttons_.append(b);
            connect(b,&QPushButton::clicked,this,[this,i]{setCurrentIndex(i);emit currentIndexChanged(i);});
        }
        setStyleSheet(QStringLiteral("AppleSegmentedControl{background:rgba(255,255,255,0.08);border:1px solid rgba(255,255,255,0.10);border-radius:12px;}"));
    }
    void setCurrentIndex(int i){for(int n=0;n<buttons_.size();++n) buttons_[n]->setChecked(n==i);index_=i;}
    int currentIndex() const{return index_;}
    QString currentData() const{return index_==0?QStringLiteral("block"):index_==1?QStringLiteral("underline"):QStringLiteral("bar");}
    void setCurrentData(const QString &v){setCurrentIndex(v==QStringLiteral("underline")?1:v==QStringLiteral("bar")?2:0);}
signals: void currentIndexChanged(int);
private: QVector<QPushButton*> buttons_; int index_=0;
};

class AppleStepper final : public QWidget {
    Q_OBJECT
public:
    AppleStepper(int value, int min, int max, QWidget *parent=nullptr):QWidget(parent),value_(value),min_(min),max_(max){
        auto *row=new QHBoxLayout(this);row->setContentsMargins(4,2,4,2);row->setSpacing(8);
        auto *minus=new QPushButton(QStringLiteral("−"),this);auto *plus=new QPushButton(QStringLiteral("+"),this);label_=new QLabel(QString::number(value_),this);label_->setAlignment(Qt::AlignCenter);
        for(auto *b:{minus,plus}){b->setFixedSize(28,28);b->setStyleSheet(QStringLiteral("QPushButton{border:0;border-radius:14px;background:rgba(255,255,255,0.10);padding:0;}QPushButton:hover{background:rgba(255,255,255,0.2);}"));}
        row->addWidget(minus);row->addWidget(label_);row->addWidget(plus);setStyleSheet(QStringLiteral("AppleStepper{background:rgba(255,255,255,0.07);border:1px solid rgba(255,255,255,0.12);border-radius:17px;}"));
        connect(minus,&QPushButton::clicked,this,[this]{setValue(value_-1);});connect(plus,&QPushButton::clicked,this,[this]{setValue(value_+1);});
    }
    int value()const{return value_;} void setValue(int v){value_=qBound(min_,v,max_);label_->setText(QString::number(value_));emit valueChanged(value_);}
signals:void valueChanged(int);
private:int value_,min_,max_;QLabel*label_=nullptr;
};

class LivePreviewWidget final : public QWidget {
public:
    enum class Kind { Appearance, Terminal, Glass, Notes };
    LivePreviewWidget(Kind kind, QWidget *parent = nullptr) : QWidget(parent), kind_(kind), blinkTimer_(new QTimer(this))
    {
        setMinimumHeight(118);
        setMaximumHeight(138);
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        setAttribute(Qt::WA_TranslucentBackground);
        connect(blinkTimer_, &QTimer::timeout, this, [this] { cursorVisible_ = !cursorVisible_; update(); });
        blinkTimer_->start(530);
    }
    void setValues(const QString &theme, const QString &accent, const QString &cursorStyle,
                   const QString &cursorColor, int fontSize, int alpha, int windowOpacity)
    {
        theme_ = theme;
        accent_ = accent;
        cursorStyle_ = cursorStyle;
        cursorColor_ = cursorColor;
        fontSize_ = fontSize;
        alpha_ = alpha;
        windowOpacity_ = windowOpacity;
        update();
    }
    void setHoverHint(const QString &hint)
    {
        hoverHint_ = hint;
        update();
    }
    void setNotesPreview(int preset, int zoom) { notePreset_ = qBound(0,preset,2); notesZoom_ = qBound(10,zoom,50); update(); }
protected:
    void paintEvent(QPaintEvent *) override
    {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);
        const QRectF card = QRectF(rect()).adjusted(1, 1, -1, -1);
        const QColor accent(accent_);
        const int cardAlpha = kind_ == Kind::Glass ? qBound(90, alpha_, 245) : 218;
        const QColor themeBase(consoleBase(theme_));
        QLinearGradient bg(card.topLeft(), card.bottomRight());
        bg.setColorAt(0, QColor(themeBase.red()+qMin(22,255-themeBase.red()), themeBase.green()+qMin(26,255-themeBase.green()), themeBase.blue()+qMin(36,255-themeBase.blue()), cardAlpha));
        bg.setColorAt(1, QColor(themeBase.red()/2, themeBase.green()/2, themeBase.blue()/2, cardAlpha));
        p.setPen(QPen(QColor(255, 255, 255, hoverHint_.isEmpty() ? 38 : 95), 1));
        p.setBrush(bg);
        p.drawRoundedRect(card, 14, 14);
        p.setPen(QPen(QColor(255, 255, 255, 65), 1));
        p.drawLine(QPointF(card.left()+14, card.top()+1), QPointF(card.right()-14, card.top()+1));

        p.setPen(QColor(245, 247, 250, 190));
        QFont labelFont = font();
        labelFont.setPixelSize(10);
        labelFont.setWeight(QFont::DemiBold);
        p.setFont(labelFont);
        const QString category = kind_ == Kind::Appearance ? QStringLiteral("ВНЕШНИЙ ВИД")
            : kind_ == Kind::Terminal ? QStringLiteral("ТЕРМИНАЛ")
            : kind_ == Kind::Notes ? QStringLiteral("ЗАМЕТКИ") : QStringLiteral("СТЕКЛО И ЭФФЕКТЫ");
        p.drawText(QRectF(14, 8, width()-28, 18), Qt::AlignLeft | Qt::AlignVCenter,
                   hoverHint_.isEmpty() ? QStringLiteral("ЖИВОЕ ПРЕВЬЮ · %1").arg(category)
                                        : QStringLiteral("ПРЕДПРОСМОТР · %1").arg(hoverHint_));

        const QRectF terminal = QRectF(14, 31, width()-28, height()-43);
        const int paneAlpha = kind_ == Kind::Glass ? qBound(80, alpha_, 245) : 225;
        p.setPen(QPen(QColor(255, 255, 255, 24), 1));
        p.setBrush(QColor(themeBase.darker(210).red(), themeBase.darker(210).green(), themeBase.darker(210).blue(), paneAlpha));
        p.drawRoundedRect(terminal, 9, 9);

        if (kind_ == Kind::Notes) {
            p.setClipRect(terminal);
            p.setPen(Qt::NoPen);
            p.setBrush(QColor(180,195,215,35));
            for (int x=terminal.left()+8; x<terminal.right(); x+=12)
                for (int y=terminal.top()+8; y<terminal.bottom(); y+=12)
                    p.drawEllipse(QPointF(x,y),0.8,0.8);
            const QSizeF sizes[] = {QSizeF(54,30),QSizeF(78,42),QSizeF(102,52)};
            QSizeF sample=sizes[notePreset_];
            const qreal fit=qMin((terminal.width()-16)/sample.width(),(terminal.height()-14)/sample.height());
            sample*=qMin(1.0,fit);
            sample*=qBound(0.55,notesZoom_/18.0,1.5);
            const QColor selB(s_noteSelBorderColor);
            const QColor selT(s_noteSelTextColor);
            QRectF note(QPointF(terminal.center().x()-sample.width()/2,terminal.center().y()-sample.height()/2),sample);
            p.setPen(QPen(selB, 1.8));
            p.setBrush(QColor(selB.red(), selB.green(), selB.blue(), 42));
            p.drawRoundedRect(note, 6, 6);
            p.setPen(QPen(selB, 1.2));
            p.drawLine(note.left()+5, note.top()+11, note.right()-5, note.top()+11);
            p.setPen(selT);
            QFont sampleFont=font(); sampleFont.setPixelSize(8); sampleFont.setWeight(QFont::DemiBold); p.setFont(sampleFont);
            p.drawText(note.adjusted(5, 2, -5, -2), Qt::AlignLeft|Qt::AlignTop, QStringLiteral("Выделено"));
            p.setPen(QColor(225,232,242,120));
            p.drawText(QRectF(terminal.left()+5,terminal.bottom()-15,terminal.width()-10,12),Qt::AlignRight|Qt::AlignVCenter,QStringLiteral("%1% · %2").arg(notesZoom_).arg(notePreset_==0?QStringLiteral("Компактный"):notePreset_==1?QStringLiteral("Стандартный"):QStringLiteral("Крупный")));
            return;
        }

        if (kind_ == Kind::Glass) {
            QLinearGradient shine(terminal.topLeft(), terminal.bottomRight());
            shine.setColorAt(0, QColor(255, 255, 255, qBound(10, windowOpacity_ / 3, 85)));
            shine.setColorAt(0.45, QColor(accent.red(), accent.green(), accent.blue(), 16));
            shine.setColorAt(1, QColor(255, 255, 255, 4));
            p.setPen(Qt::NoPen);
            p.setBrush(shine);
            p.drawRoundedRect(terminal.adjusted(1, 1, -1, -1), 8, 8);
            QRectF sample = terminal.adjusted(terminal.width()*0.16, 12, -terminal.width()*0.16, -12);
            p.setPen(QPen(QColor(255,255,255, qBound(22, windowOpacity_, 100)), 1));
            p.setBrush(QColor(255,255,255, qBound(10, windowOpacity_/2, 55)));
            p.drawRoundedRect(sample, 8, 8);
            p.setPen(QColor(244,247,252,220));
            p.drawText(sample.adjusted(12, 0, -12, 0), Qt::AlignVCenter | Qt::AlignLeft,
                       QStringLiteral("Матовая панель  ·  %1%") .arg(windowOpacity_));
            p.setPen(QColor(accent.red(),accent.green(),accent.blue(),180));
            p.drawLine(sample.left()+12, sample.top()+1, sample.right()-12, sample.top()+1);
            return;
        }

        const int px = kind_ == Kind::Terminal ? qBound(10, fontSize_, 24) : 12;
        QSettings stPrev(QString::fromLatin1(kSettingsOrg), QString::fromLatin1(kSettingsApp)); QFont mono(stPrev.value(QStringLiteral("terminalFontFamily"), QStringLiteral("Consolas")).toString());
        mono.setStyleHint(QFont::Monospace);
        mono.setPixelSize(px);
        p.setFont(mono);
        const qreal left = terminal.left()+12;
        const qreal top = terminal.top()+10;
        const qreal line = qMax(16, px+5);
        const QColor prompt = kind_ == Kind::Appearance ? accent : QColor(QStringLiteral("#71d6a3"));
        p.setPen(prompt);
        p.drawText(QPointF(left, top+line), QStringLiteral("user@studio:~$"));
        p.setPen(QColor(226, 232, 240, 220));
        p.drawText(QPointF(left, top+line*2), QStringLiteral("echo Liquid Glass"));
        p.setPen(accent);
        const qreal cursorX = left + QFontMetricsF(mono).horizontalAdvance(QStringLiteral("echo Liquid Glass")) + 2;
        const qreal cursorY = top + line*2 - px*0.78;
        if (kind_ == Kind::Appearance) {
            p.setPen(Qt::NoPen);
            p.setBrush(QColor(accent.red(),accent.green(),accent.blue(),55));
            p.drawRoundedRect(QRectF(terminal.right()-82,terminal.top()+10,68,22),7,7);
            p.setPen(accent);
            p.drawText(QRectF(terminal.right()-80,terminal.top()+10,64,22),Qt::AlignCenter,QStringLiteral("Акцент"));
        } else if (cursorVisible_) {
            p.setPen(Qt::NoPen);
            p.setBrush(QColor(cursorColor_));
            if (cursorStyle_ == QStringLiteral("underline")) p.drawRoundedRect(QRectF(cursorX,cursorY+px*0.72,px*0.8,2),1,1);
            else if (cursorStyle_ == QStringLiteral("bar")) p.drawRoundedRect(QRectF(cursorX,cursorY,2,px),1,1);
            else p.drawRoundedRect(QRectF(cursorX,cursorY,px*0.62,px),2,2);
        }
    }
private:
    Kind kind_;
    QTimer *blinkTimer_ = nullptr;
    QString theme_ = QStringLiteral("Graphite Pro");
    QString accent_ = QStringLiteral("#7eb8da");
    QString cursorStyle_ = QStringLiteral("block");
    QString cursorColor_ = QStringLiteral("#7eb8da");
    QString hoverHint_;
    int fontSize_ = 14;
    int alpha_ = 210;
    int windowOpacity_ = 100;
    int notePreset_ = 1;
    int notesZoom_ = 18;
    bool cursorVisible_ = true;
};

QString defaultShell()
{
#ifdef Q_OS_WIN
    return QStringLiteral("cmd");
#else
    return QStringLiteral("default");
#endif
}
class TerminalCanvas final : public QWidget {
    struct Cell { QChar ch = QLatin1Char(' '); QColor fg = QColor("#d7dae0"); QColor bg; bool bold=false; bool inverse=false; bool underline=false; };
public:
    explicit TerminalCanvas(QWidget *parent=nullptr):QWidget(parent){
        setFocusPolicy(Qt::StrongFocus);setAcceptDrops(true);setMouseTracking(true);setAutoFillBackground(false);
        blinkTimer_=new QTimer(this);blinkTimer_->setInterval(530);connect(blinkTimer_,&QTimer::timeout,this,[this]{cursorVisible_=!cursorVisible_;update();});blinkTimer_->start();
        resizeBuffer(80,24);
    }
    std::function<void(const QByteArray&)> rawKeySent;
    std::function<void(const QSize&)> terminalResized;
    TerminalBackend *terminalBackend=nullptr;
    QString lastSentCmd;
    int columns()const{return cols_;} int rows()const{return rows_;}
    QSize terminalSize()const{return QSize(cols_,rows_);}
    void refreshMetrics(){const QFontMetricsF fm(font());const int cw=qMax(1,qCeil(fm.horizontalAdvance(QLatin1Char('M'))));const int lh=qMax(1,qCeil(fm.lineSpacing()));const int c=qMax(2,(width()-16)/cw),r=qMax(2,(height()-16)/lh);if(c!=cols_||r!=rows_){resizeBuffer(c,r);if(terminalResized)terminalResized(QSize(cols_,rows_));}}
    QString currentInput()const{return QString();}
    QString toPlainText()const{QString out;const auto &buffer=activeScreen();for(int r=0;r<rows_;++r){QString line;for(const Cell &c:buffer[r])line+=c.ch;out+=line.trimmed();if(r+1<rows_)out+=QLatin1Char('\n');}return out;}
    QString selectedText()const{if(!selecting_||selectionAnchor_==selectionEnd_)return {};const auto &buffer=activeScreen();auto a=selectionAnchor_,b=selectionEnd_;if(a.y()>b.y()||(a.y()==b.y()&&a.x()>b.x()))qSwap(a,b);QString out;for(int r=a.y();r<=b.y();++r){int from=r==a.y()?a.x():0,to=r==b.y()?b.x():cols_-1;for(int c=from;c<=to;++c)out+=buffer[r][c].ch;if(r<b.y())out+=QLatin1Char('\n');}return out;}
    void clearSelection(){selecting_=false;update();}
    void pasteText(const QString &text){send((bracketedPaste_?QByteArray("\x1b[200~"):QByteArray())+text.toUtf8()+(bracketedPaste_?QByteArray("\x1b[201~"):QByteArray()));}
    void pasteFromClipboard(){pasteText(QApplication::clipboard()->text());}
    void setTerminalCursor(const QString &style,const QColor &color){cursorStyle_=style;cursorColor_=color;update();}
    void appendSystemText(const QString &raw){for(QChar ch:raw)consume(ch);update();}
    void appendSystemBytes(const QByteArray &bytes){const QString decoded=decoder_(bytes);appendSystemText(decoded);}
    void clearTerminal(){clearScreen(screen_);row_=col_=0;update();}
protected:
    void resizeEvent(QResizeEvent *e)override{QWidget::resizeEvent(e);refreshMetrics();}
    void paintEvent(QPaintEvent*)override{
        QPainter p(this);p.setRenderHint(QPainter::TextAntialiasing);p.fillRect(rect(),QColor(12,15,22,190));const QFontMetricsF fm(font());const qreal cw=fm.horizontalAdvance(QLatin1Char('M')),lh=fm.lineSpacing();const qreal ox=8,oy=8;const auto &buf=activeScreen();
        for(int r=0;r<rows_;++r)for(int c=0;c<cols_;++c){const Cell &cell=buf[r][c];const qreal x=ox+c*cw,y=oy+r*lh;QColor fg=cell.fg,bg=cell.bg;if(cell.inverse){if(!bg.isValid())bg=QColor(12,15,22);qSwap(fg,bg);}if(selecting_&&isSelected(c,r))bg=QColor(126,184,218,100);if(bg.isValid())p.fillRect(QRectF(x,y,cw,lh),bg);if(!cell.ch.isSpace()){p.setPen(fg);QFont f=font();f.setBold(cell.bold);p.setFont(f);p.drawText(QPointF(x,y+fm.ascent()),cell.ch);}if(cell.underline){p.setPen(fg);p.drawLine(QPointF(x,y+lh-1),QPointF(x+cw,y+lh-1));}}
        if(cursorEnabled_&&cursorVisible_&&hasFocus()){const qreal x=ox+col_*cw,y=oy+row_*lh;p.setPen(Qt::NoPen);p.setBrush(cursorColor_);if(cursorStyle_=="underline")p.drawRect(QRectF(x,y+lh-2,cw,2));else if(cursorStyle_=="bar")p.drawRect(QRectF(x,y,2,lh));else p.drawRect(QRectF(x,y,cw,lh));}
    }
    void mousePressEvent(QMouseEvent *e)override{setFocus(Qt::MouseFocusReason);if(e->button()==Qt::RightButton){const QString selection=selectedText();if(!selection.isEmpty()){QApplication::clipboard()->setText(selection);clearSelection();}else pasteFromClipboard();e->accept();return;}if(e->button()==Qt::LeftButton){selecting_=false;dragSelecting_=true;selectionAnchor_=selectionEnd_=cellAt(e->position());update();}QWidget::mousePressEvent(e);}
    void contextMenuEvent(QContextMenuEvent *e)override{e->accept();}
    void mouseMoveEvent(QMouseEvent *e)override{if(dragSelecting_&&(e->buttons()&Qt::LeftButton)){selectionEnd_=cellAt(e->position());selecting_=selectionAnchor_!=selectionEnd_;update();}QWidget::mouseMoveEvent(e);}
    void mouseReleaseEvent(QMouseEvent *e)override{if(e->button()==Qt::LeftButton)dragSelecting_=false;QWidget::mouseReleaseEvent(e);}
    bool event(QEvent *e)override{if(e->type()==QEvent::KeyPress&&static_cast<QKeyEvent*>(e)->key()==Qt::Key_Tab){send(QByteArray(1,'\t'));return true;}return QWidget::event(e);}
    bool focusNextPrevChild(bool)override{return false;}
    void keyPressEvent(QKeyEvent *e)override{
        cursorVisible_=true;blinkTimer_->start(530);update();const bool ctrl=e->modifiers()&Qt::ControlModifier,shift=e->modifiers()&Qt::ShiftModifier;
        if((ctrl&&shift&&e->key()==Qt::Key_C)||(ctrl&&e->key()==Qt::Key_C&&!selectedText().isEmpty())){QApplication::clipboard()->setText(selectedText());selecting_=false;update();return;}
        if((ctrl&&e->key()==Qt::Key_V)||(ctrl&&shift&&e->key()==Qt::Key_V)||(shift&&e->key()==Qt::Key_Insert)){pasteFromClipboard();return;}
        if((ctrl||e->modifiers()&Qt::AltModifier)&&e->key()==Qt::Key_Backspace){send(QByteArray(1,'\x17'));return;}
        QByteArray b;
        if(ctrl&&e->key()>=Qt::Key_A&&e->key()<=Qt::Key_Z){b=QByteArray(1,char(e->key()-Qt::Key_A+1));}
        if(b.isEmpty())switch(e->key()){
        case Qt::Key_Backspace:
#ifdef Q_OS_WIN
            b="\b";
#else
            b="\x7f";
#endif
            break;
        case Qt::Key_Return:case Qt::Key_Enter:
#ifdef Q_OS_WIN
            b="\r";
#else
            b="\n";
#endif
            break;
        case Qt::Key_Tab:b="\t";break;case Qt::Key_Up:b=applicationCursor_?"\x1bOA":"\x1b[A";break;case Qt::Key_Down:b=applicationCursor_?"\x1bOB":"\x1b[B";break;case Qt::Key_Right:b=applicationCursor_?"\x1bOC":"\x1b[C";break;case Qt::Key_Left:b=applicationCursor_?"\x1bOD":"\x1b[D";break;
        case Qt::Key_Home:b="\x1b[H";break;case Qt::Key_End:b="\x1b[F";break;case Qt::Key_Delete:b="\x1b[3~";break;case Qt::Key_PageUp:b="\x1b[5~";break;case Qt::Key_PageDown:b="\x1b[6~";break;default:if(!ctrl&&!e->text().isEmpty())b=e->text().toUtf8();break;}
        if(!b.isEmpty())send(b);
        e->accept();
    }
    void dragEnterEvent(QDragEnterEvent*e)override{if(e->mimeData()->hasUrls())e->acceptProposedAction();else QWidget::dragEnterEvent(e);}
    void dragMoveEvent(QDragMoveEvent*e)override{if(e->mimeData()->hasUrls())e->acceptProposedAction();else QWidget::dragMoveEvent(e);}
    void dropEvent(QDropEvent*e)override{if(!e->mimeData()->hasUrls()){e->ignore();return;}QStringList paths;for(const QUrl&u:e->mimeData()->urls())if(u.isLocalFile()){QString path=QDir::toNativeSeparators(u.toLocalFile());if(path.contains(' '))path='"'+path+'"';paths<<path;}if(terminalBackend&&terminalBackend->isRunning()&&!paths.isEmpty())terminalBackend->writeData(paths.join(' ').toUtf8());setFocus(Qt::MouseFocusReason);e->acceptProposedAction();}
private:
    void send(const QByteArray&b){cursorVisible_=true;blinkTimer_->start(530);update();if(rawKeySent)rawKeySent(b);}
    QPoint cellAt(const QPointF&p)const{const QFontMetricsF fm(font());return QPoint(qBound(0,int((p.x()-8)/qMax(1.0,fm.horizontalAdvance(QLatin1Char('M')))),cols_-1),qBound(0,int((p.y()-8)/qMax(1.0,fm.lineSpacing())),rows_-1));}
    bool isSelected(int c,int r)const{if(!selecting_)return false;auto a=selectionAnchor_,b=selectionEnd_;if(a.y()>b.y()||(a.y()==b.y()&&a.x()>b.x()))qSwap(a,b);return r>=a.y()&&r<=b.y()&&c>=(r==a.y()?a.x():0)&&c<=(r==b.y()?b.x():cols_-1);}
    QVector<QVector<Cell>>&activeScreen(){return alternate_?alternateScreen_:screen_;}const QVector<QVector<Cell>>&activeScreen()const{return alternate_?alternateScreen_:screen_;}
    Cell blankCell()const{Cell cell;cell.fg=fg_;cell.bg=bg_;cell.bold=bold_;cell.inverse=inverse_;cell.underline=underline_;return cell;}
    void resizeBuffer(int c,int r){const int oldRows=rows_;const bool fullScroll=scrollBottom_==oldRows-1;cols_=qBound(2,c,300);rows_=qBound(2,r,120);if(fullScroll)scrollBottom_=rows_-1;else scrollBottom_=qBound(scrollTop_,scrollBottom_,rows_-1);scrollTop_=qBound(0,scrollTop_,scrollBottom_);auto resize=[this](QVector<QVector<Cell>>&b){b.resize(rows_);for(auto&line:b)line.resize(cols_);};resize(screen_);resize(alternateScreen_);row_=qBound(0,row_,rows_-1);col_=qBound(0,col_,cols_-1);savedRow_=qBound(0,savedRow_,rows_-1);savedAltRow_=qBound(0,savedAltRow_,rows_-1);update();}
    void clearScreen(QVector<QVector<Cell>>&b){b=QVector<QVector<Cell>>(rows_,QVector<Cell>(cols_,blankCell()));}
    void lineFeed(){if(row_==scrollBottom_){auto&b=activeScreen();for(int r=scrollTop_;r<scrollBottom_;++r)b[r]=b[r+1];b[scrollBottom_]=QVector<Cell>(cols_,blankCell());}else if(row_<rows_-1)++row_;}
    void reverseIndex(){if(row_==scrollTop_){auto&b=activeScreen();for(int r=scrollBottom_;r>scrollTop_;--r)b[r]=b[r-1];b[scrollTop_]=QVector<Cell>(cols_,blankCell());}else if(row_>0)--row_;}
    static QColor ansi(int n){static const char*base[]={"#000000","#cd3131","#0dbc79","#e5e510","#2472c8","#bc3fbc","#11a8cd","#e5e5e5","#666666","#f14c4c","#23d18b","#f5f543","#3b8eea","#d670d6","#29b8db","#ffffff"};return QColor(base[qBound(0,n,15)]);}
    static QColor indexed(int n){n=qBound(0,n,255);if(n<16)return ansi(n);if(n<232){int v=n-16;auto level=[](int x){return x==0?0:55+x*40;};return QColor(level(v/36),level((v/6)%6),level(v%6));}int g=8+(n-232)*10;return QColor(g,g,g);}
    void eraseLine(int mode){auto&line=activeScreen()[row_];const int first=mode==1?0:col_,last=mode==1?col_:cols_-1;for(int c=first;c<=last&&c<cols_;++c)line[c]=blankCell();}
    void sgr(const QStringList&a){if(a.isEmpty()){fg_=QColor("#d7dae0");bg_=QColor();bold_=inverse_=underline_=false;return;}for(int i=0;i<a.size();++i){int n=a[i].isEmpty()?0:a[i].toInt();if(n==0){fg_=QColor("#d7dae0");bg_=QColor();bold_=inverse_=underline_=false;}else if(n==1)bold_=true;else if(n==4)underline_=true;else if(n==7)inverse_=true;else if(n==27)inverse_=false;else if(n==22)bold_=false;else if(n==24)underline_=false;else if(n==39)fg_=QColor("#d7dae0");else if(n==49)bg_=QColor();else if(n>=30&&n<=37)fg_=ansi(n-30);else if(n>=90&&n<=97)fg_=ansi(n-90+8);else if(n>=40&&n<=47)bg_=ansi(n-40);else if(n>=100&&n<=107)bg_=ansi(n-100+8);else if((n==38||n==48)&&i+2<a.size()&&a[i+1]=="5"){QColor x=indexed(a[i+2].toInt());if(n==38)fg_=x;else bg_=x;i+=2;}else if((n==38||n==48)&&i+4<a.size()&&a[i+1]=="2"){QColor x(qBound(0,a[i+2].toInt(),255),qBound(0,a[i+3].toInt(),255),qBound(0,a[i+4].toInt(),255));if(n==38)fg_=x;else bg_=x;i+=4;}}}
    void csi(const QString&s,QChar final){QString t=s;bool priv=t.startsWith('?');if(priv)t.remove(0,1);QStringList a=t.split(';');auto arg=[&](int i,int d){return i<a.size()&&!a[i].isEmpty()?a[i].toInt():d;};auto&b=activeScreen();switch(final.unicode()){
        case 'A':row_=qMax(priv?scrollTop_:0,row_-arg(0,1));break;case 'B':row_=qMin(priv?scrollBottom_:rows_-1,row_+arg(0,1));break;case 'C':case 'a':col_=qMin(cols_-1,col_+arg(0,1));break;case 'D':col_=qMax(0,col_-arg(0,1));break;case 'E':row_=qMin(rows_-1,row_+arg(0,1));col_=0;break;case 'F':row_=qMax(0,row_-arg(0,1));col_=0;break;case 'G':case '`':col_=qBound(0,arg(0,1)-1,cols_-1);break;case 'H':case 'f':row_=qBound(0,arg(0,1)-1,rows_-1);col_=qBound(0,arg(1,1)-1,cols_-1);break;
        case 'J':if(arg(0,0)==2||arg(0,0)==3){clearScreen(b);if(arg(0,0)==2)row_=col_=0;}else if(arg(0,0)==0){for(int r=row_;r<rows_;++r)for(int c=(r==row_?col_:0);c<cols_;++c)b[r][c]=blankCell();}break;
        case 'K':{int mode=arg(0,0);if(mode==2){for(int c=0;c<cols_;++c)b[row_][c]=blankCell();}else eraseLine(mode);break;}
        case 'd':row_=qBound(0,arg(0,1)-1,rows_-1);break;
        case 'X':for(int c=col_;c<qMin(cols_,col_+arg(0,1));++c)b[row_][c]=blankCell();break;
        case 'P':{int n=qBound(1,arg(0,1),cols_-col_);for(int c=col_;c<cols_-n;++c)b[row_][c]=b[row_][c+n];for(int c=cols_-n;c<cols_;++c)b[row_][c]=blankCell();break;}
        case '@':{int n=qBound(1,arg(0,1),cols_-col_);for(int c=cols_-1;c>=col_+n;--c)b[row_][c]=b[row_][c-n];for(int c=col_;c<col_+n;++c)b[row_][c]=blankCell();break;}
        case 'S':for(int n=0;n<arg(0,1);++n){for(int r=scrollTop_;r<scrollBottom_;++r)b[r]=b[r+1];b[scrollBottom_]=QVector<Cell>(cols_,blankCell());}break;
        case 'T':for(int n=0;n<arg(0,1);++n){for(int r=scrollBottom_;r>scrollTop_;--r)b[r]=b[r-1];b[scrollTop_]=QVector<Cell>(cols_,blankCell());}break;
        case 'r':scrollTop_=qBound(0,arg(0,1)-1,rows_-1);scrollBottom_=qBound(scrollTop_,arg(1,rows_)-1,rows_-1);row_=scrollTop_;col_=0;break;
        case 'm':sgr(a);break;case 'h':case 'l':if(priv){for(const QString&modeText:a){const int mode=modeText.toInt();if(mode==1049||mode==1047){if(final=='h'){savedAltRow_=row_;savedAltCol_=col_;clearScreen(alternateScreen_);alternate_=true;row_=col_=0;}else{alternate_=false;row_=qBound(0,savedAltRow_,rows_-1);col_=qBound(0,savedAltCol_,cols_-1);}}else if(mode==1)applicationCursor_=final=='h';else if(mode==25)cursorEnabled_=final=='h';else if(mode==2004)bracketedPaste_=final=='h';}}break;case 's':savedRow_=row_;savedCol_=col_;break;case 'u':row_=savedRow_;col_=savedCol_;break;default:break;}if(final!='m'&&final!='h'&&final!='l'&&final!='s'&&final!='u')wrapPending_=false;}
    void consume(QChar ch){if(!escape_.isEmpty()){escape_+=ch;if(escape_==QChar(0x1b))return;if(escape_.startsWith(QChar(0x1b)+QLatin1Char(']'))){if(ch==QChar(7)||escape_.endsWith(QStringLiteral("\x1b\\")))escape_.clear();return;}if(escape_.size()==2&&escape_[1]=='[')return;if(escape_.size()>2&&escape_[1]=='['){ushort u=ch.unicode();if(u>=0x40&&u<=0x7e){csi(escape_.mid(2,escape_.size()-3),ch);escape_.clear();}return;}if(escape_.size()==2){if(ch=='7'){savedRow_=row_;savedCol_=col_;}else if(ch=='8'){row_=savedRow_;col_=savedCol_;}else if(ch=='D')lineFeed();else if(ch=='M')reverseIndex();escape_.clear();}return;}
        if(ch==QChar(0x1b)){escape_=ch;return;}if(ch=='\r'){col_=0;wrapPending_=false;return;}if(ch=='\n'){lineFeed();wrapPending_=false;return;}if(ch=='\b'){col_=qMax(0,col_-1);wrapPending_=false;return;}if(ch=='\t'){col_=qMin(cols_-1,(col_+8)&~7);wrapPending_=false;return;}if(ch==QChar(0x7f)||ch.unicode()<32)return;if(wrapPending_){col_=0;lineFeed();wrapPending_=false;}Cell&cell=activeScreen()[row_][col_];cell.ch=ch;cell.fg=fg_;cell.bg=bg_;cell.bold=bold_;cell.inverse=inverse_;cell.underline=underline_;if(col_==cols_-1)wrapPending_=true;else ++col_;}
    QVector<QVector<Cell>>screen_,alternateScreen_;int rows_=24,cols_=80,row_=0,col_=0,savedRow_=0,savedCol_=0,savedAltRow_=0,savedAltCol_=0,scrollTop_=0,scrollBottom_=23;bool alternate_=false,applicationCursor_=false,bracketedPaste_=false,bold_=false,inverse_=false,underline_=false,cursorVisible_=true,cursorEnabled_=true,wrapPending_=false,selecting_=false,dragSelecting_=false;QPoint selectionAnchor_,selectionEnd_;QString escape_,cursorStyle_="block";QColor fg_=QColor("#d7dae0"),bg_,cursorColor_=QColor("#7eb8da");QTimer*blinkTimer_=nullptr;QStringDecoder decoder_{QStringDecoder::Utf8};
};

class NoteCard final : public QGraphicsObject {
public:
    NoteCard(QString heading, QString content, QString accent, QString id, QString theme)
        : heading_(std::move(heading)), content_(std::move(content)), accent_(std::move(accent)), id_(std::move(id)), theme_(std::move(theme))
    {
        setAcceptedMouseButtons(Qt::LeftButton);
        setFlag(QGraphicsItem::ItemIsSelectable, true);
        setFlag(QGraphicsItem::ItemIsMovable, true);
        setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
        setZValue(2);
        editor_ = new QTextEdit;
        editor_->setPlainText(content_);
        editor_->setFrameShape(QFrame::NoFrame);
        editor_->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        editor_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        editor_->setContentsMargins(0, 0, 0, 0);
        editor_->document()->setDocumentMargin(0);
        editor_->setStyleSheet(QStringLiteral("QTextEdit{background:transparent;border:0;padding:0;color:rgba(245,247,250,0.90);selection-background-color:rgba(126,184,218,0.35);}"));
        proxy_ = new QGraphicsProxyWidget(this);
        proxy_->setWidget(editor_);
        proxy_->setPos(14,50);
        headingEditor_ = new QLineEdit(heading_);
        headingEditor_->setFrame(false);
        headingEditor_->setStyleSheet(QStringLiteral("QLineEdit{background:transparent;border:0;padding:0;color:#f3f3f6;font-weight:600;font-size:14px;}"));
        headingProxy_ = new QGraphicsProxyWidget(this);
        headingProxy_->setWidget(headingEditor_);
        headingProxy_->setPos(14, 8);
        headingProxy_->hide();
        editor_->setFixedWidth(qMax(158,int(width_-42)));
        editor_->setFixedHeight(height_-68);
        connect(editor_, &QTextEdit::textChanged, this, [this] { content_ = editor_->toPlainText(); resizeToContent(); if (contentChanged) contentChanged(); });
        connect(headingEditor_, &QLineEdit::textChanged, this, [this](const QString &text) { heading_ = text; update(); if (contentChanged) contentChanged(); });
        connect(headingEditor_, &QLineEdit::editingFinished, this, [this] { headingProxy_->hide(); update(); });
        QTimer::singleShot(0,this,[this]{resizeToContent();});
    }
    QRectF boundingRect() const override { return QRectF(0, 0, width_, height_); }
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) override
    {
        painter->setRenderHint(QPainter::Antialiasing);
        painter->setBrush(theme_ == QStringLiteral("Graphite") ? QColor("#26292e") : (theme_ == QStringLiteral("Obsidian") ? QColor("#101116") : QColor("#111426")));
        painter->setPen(QPen(QColor("#ffffff"), 1));
        painter->drawRoundedRect(boundingRect(), 14, 14);
        const bool sel = isSelected();
        const QColor selBorder(s_noteSelBorderColor);
        const QColor selText(s_noteSelTextColor);
        if (editor_) {
            const QString tc = sel ? s_noteSelTextColor : QStringLiteral("rgba(245,247,250,0.90)");
            if (editor_->property("lastTextColor").toString() != tc) {
                editor_->setProperty("lastTextColor", tc);
                editor_->setStyleSheet(QStringLiteral("QTextEdit{background:transparent;border:0;padding:0;color:%1;selection-background-color:rgba(126,184,218,0.35);}").arg(tc));
            }
        }
        if (sel) {
            painter->setPen(QPen(selBorder, 2.6));
            painter->setBrush(QColor(selBorder.red(), selBorder.green(), selBorder.blue(), 38));
            painter->drawRoundedRect(boundingRect().adjusted(1, 1, -1, -1), 14, 14);
        }
        painter->setPen(QPen(sel ? selBorder : QColor(accent_), sel ? 2.6 : 2.0));
        painter->drawLine(QPointF(13, 42), QPointF(width_-13, 42));
        if(!headingProxy_->isVisible()){
            painter->setPen(QColor("#f3f3f6"));
            painter->drawText(QRectF(14, 10, width_-62, 25), Qt::AlignVCenter, heading_);
        }
        painter->setPen(QColor("#ff929b"));
        painter->drawText(QRectF(width_-29, 8, 20, 24), Qt::AlignCenter, QStringLiteral("×"));
                if (!mediaPath_.isEmpty()) {
            const QRectF mediaRect(14, 50, qMax(40.0, width_ - 28.0), qMax(40.0, height_ - 66.0));
            painter->save();
            QPainterPath clip; clip.addRoundedRect(mediaRect, 8, 8); painter->setClipPath(clip);
            if (isGif_ && movie_ && movie_->isValid()) {
                const QPixmap frame = movie_->currentPixmap();
                if (!frame.isNull())
                    painter->drawPixmap(mediaRect.toRect(), frame.scaled(mediaRect.size().toSize(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
            } else if (isAudio_) {
                painter->fillRect(mediaRect, QColor(18, 22, 32, 195));
                const QRectF playBtn(mediaRect.left() + 8, mediaRect.center().y() - 18, 36, 36);
                painter->setBrush(QColor(accent_.isEmpty() ? QStringLiteral("#7eb8da") : accent_));
                painter->setPen(Qt::NoPen);
                painter->drawEllipse(playBtn);
                painter->setBrush(QColor("#0f131a"));
                if (videoPlaying_) {
                    painter->drawRoundedRect(QRectF(playBtn.center().x() - 5, playBtn.center().y() - 6, 3.5, 12), 1, 1);
                    painter->drawRoundedRect(QRectF(playBtn.center().x() + 1.5, playBtn.center().y() - 6, 3.5, 12), 1, 1);
                } else {
                    QPolygonF tri; tri << QPointF(playBtn.center().x() - 4, playBtn.center().y() - 7) << QPointF(playBtn.center().x() + 8, playBtn.center().y()) << QPointF(playBtn.center().x() - 4, playBtn.center().y() + 7);
                    painter->drawPolygon(tri);
                }
                const qreal waveLeft = playBtn.right() + 12;
                const qreal waveRight = mediaRect.right() - 10;
                const qreal waveW = qMax(40.0, waveRight - waveLeft);
                const qreal waveTop = mediaRect.top() + 8;
                const qreal waveH = qMax(18.0, mediaRect.height() - 34.0);
                const qreal progress = (audioDuration100Ns_ > 0) ? qBound(0.0, qreal(audioPos100Ns_) / qreal(audioDuration100Ns_), 1.0) : 0.0;
                const int bars = qMax(16, int(waveW / 5.0));
                for (int b = 0; b < bars; ++b) {
                    const qreal t = qreal(b) / qMax(1, bars - 1);
                    const qreal hFactor = 0.25 + 0.75 * qAbs(std::sin(b * 1.35 + 0.7) * std::cos(b * 0.45));
                    const qreal bh = qMax(4.0, waveH * hFactor);
                    const qreal bx = waveLeft + t * (waveW - 3.0);
                    const qreal by = waveTop + (waveH - bh) / 2.0;
                    painter->setBrush((t <= progress) ? QColor(accent_.isEmpty() ? QStringLiteral("#7eb8da") : accent_) : QColor(255, 255, 255, 60));
                    painter->drawRoundedRect(QRectF(bx, by, 3.0, bh), 1.5, 1.5);
                }
                const int curSec = int(audioPos100Ns_ / 10000000LL);
                const int totSec = int(audioDuration100Ns_ / 10000000LL);
                const QString timeStr = QStringLiteral("%1:%2 / %3:%4")
                    .arg(curSec / 60, 2, 10, QLatin1Char('0')).arg(curSec % 60, 2, 10, QLatin1Char('0'))
                    .arg(totSec / 60, 2, 10, QLatin1Char('0')).arg(totSec % 60, 2, 10, QLatin1Char('0'));
                painter->setPen(QColor(235, 240, 248, 195));
                painter->drawText(QRectF(waveLeft, mediaRect.bottom() - 21, 95, 18), Qt::AlignLeft | Qt::AlignVCenter, timeStr);

                const qreal volLeft = qMax(waveLeft + 96, waveRight - 86);
                painter->drawText(QRectF(volLeft, mediaRect.bottom() - 22, 22, 20), Qt::AlignCenter, videoMuted_ ? QStringLiteral("🔇") : QStringLiteral("🔊"));
                const QRectF vTrack(volLeft + 24, mediaRect.bottom() - 13, qMax(28.0, waveRight - (volLeft + 26)), 4);
                painter->setBrush(QColor(255, 255, 255, 45));
                painter->drawRoundedRect(vTrack, 2, 2);
                const qreal vFill = videoMuted_ ? 0.0 : qBound(0.0, qreal(videoVolume_), 1.0);
                painter->setBrush(QColor(accent_.isEmpty() ? QStringLiteral("#7eb8da") : accent_));
                painter->drawRoundedRect(QRectF(vTrack.left(), vTrack.top(), vTrack.width() * vFill, 4), 2, 2);
            } else if (isVideo_) {
                if (!videoFrame_.isNull()) {
                    painter->drawPixmap(mediaRect.toRect(), videoFrame_.scaled(mediaRect.size().toSize(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
                } else {
                    painter->fillRect(mediaRect, QColor(14, 18, 28, 210));
                }
                if (!videoPlaying_) {
                    const QPointF c = mediaRect.center();
                    painter->setBrush(QColor(18, 24, 36, 195)); painter->setPen(QPen(QColor(126, 184, 218, 220), 1.5)); painter->drawEllipse(c, 22, 22);
                    QPolygonF tri; tri << QPointF(c.x() - 5, c.y() - 9) << QPointF(c.x() + 10, c.y()) << QPointF(c.x() - 5, c.y() + 9);
                    painter->setBrush(QColor("#f3f5f8")); painter->setPen(Qt::NoPen); painter->drawPolygon(tri);
                }
                const QRectF barRect(mediaRect.left() + 8, mediaRect.bottom() - 32, qMin(150.0, mediaRect.width() - 16), 24);
                painter->setBrush(QColor(12, 16, 24, 195));
                painter->setPen(QPen(QColor(255, 255, 255, 45), 1));
                painter->drawRoundedRect(barRect, 7, 7);
                painter->setPen(QColor("#f3f5f8"));
                painter->drawText(QRectF(barRect.left() + 2, barRect.top(), 24, 24), Qt::AlignCenter, videoMuted_ ? QStringLiteral("🔇") : QStringLiteral("🔊"));
                const QRectF sliderTrack(barRect.left() + 28, barRect.center().y() - 2, barRect.width() - 38, 4);
                painter->setBrush(QColor(255, 255, 255, 45)); painter->setPen(Qt::NoPen);
                painter->drawRoundedRect(sliderTrack, 2, 2);
                const qreal fillRatio = videoMuted_ ? 0.0 : qBound(0.0, qreal(videoVolume_), 1.0);
                painter->setBrush(QColor(accent_.isEmpty() ? QStringLiteral("#7eb8da") : accent_));
                painter->drawRoundedRect(QRectF(sliderTrack.left(), sliderTrack.top(), sliderTrack.width() * fillRatio, 4), 2, 2);
                painter->setBrush(QColor("#ffffff"));
                painter->drawEllipse(QPointF(sliderTrack.left() + sliderTrack.width() * fillRatio, sliderTrack.center().y()), 5, 5);
            } else if (!pixmap_.isNull()) {
                painter->drawPixmap(mediaRect.toRect(), pixmap_.scaled(mediaRect.size().toSize(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
            }
            painter->restore();
        }
        painter->setPen(QPen(QColor(220,230,245,150),1,Qt::DashLine));
        painter->drawLine(QPointF(width_-20,height_-8),QPointF(width_-8,height_-20));
        painter->drawLine(QPointF(width_-14,height_-8),QPointF(width_-8,height_-14));
    }
    QString id() const { return id_; }
    QString heading() const { return heading_; }
    QString content() const { return content_; }
    qreal cardWidth() const { return width_; }
    qreal cardHeight() const { return height_; }
    void focusEditor() { if (editor_) editor_->setFocus(Qt::MouseFocusReason); }
    void editHeading() { if (headingEditor_ && headingProxy_) { headingEditor_->setText(heading_); headingProxy_->show(); headingEditor_->setFocus(Qt::MouseFocusReason); headingEditor_->selectAll(); } }
    void setSelectionMode(bool active) { selectionMode_ = active; setFlag(QGraphicsItem::ItemIsMovable, !active); }
    void setStoredSize(qreal w, qreal h) { manualWidth_ = qMax(180.0, w); manualHeight_ = qMax(isAudio_ ? 116.0 : 140.0, h); resizeToContent(); }
    void setBaseSize(qreal w, qreal h) { baseWidth_ = w; baseHeight_ = h; if (manualWidth_ < w) manualWidth_ = w; if (manualHeight_ < h) manualHeight_ = h; resizeToContent(); }
    std::function<void()> moved;
    std::function<void()> geometryChanged;
    std::function<void()> contentChanged;
    std::function<void(NoteCard *)> linkRequested;
    std::function<void(NoteCard *)> deleteRequested;
    QLineEdit *headingEditor() const { return headingEditor_; }
    QTextEdit *contentEditor() const { return editor_; }
    void setAccent(const QString &a) { accent_ = a; update(); }
    void setTheme(const QString &t) { theme_ = t; update(); }
    void setBasePreset(int preset) {
        if (preset == 0) setBaseSize(200, 120);
        else if (preset == 2) setBaseSize(340, 220);
        else setBaseSize(260, 160);
    }
    void setMediaFile(const QString &storedPath) {
        mediaPath_ = storedPath;
        if (mediaPath_.isEmpty()) return;
        const QString ext = QFileInfo(mediaPath_).suffix().toLower();
        isGif_ = (ext == QStringLiteral("gif"));
        isAudio_ = QStringList{QStringLiteral("mp3"),QStringLiteral("wav"),QStringLiteral("ogg"),QStringLiteral("flac"),QStringLiteral("m4a"),QStringLiteral("aac"),QStringLiteral("opus"),QStringLiteral("wma")}.contains(ext); isVideo_ = !isAudio_ && QStringList{QStringLiteral("mp4"),QStringLiteral("webm"),QStringLiteral("mkv"),QStringLiteral("avi"),QStringLiteral("mov"),QStringLiteral("m4v"),QStringLiteral("wmv")}.contains(ext);
        isAudio_ = QStringList{QStringLiteral("mp3"), QStringLiteral("wav"), QStringLiteral("ogg"), QStringLiteral("flac"), QStringLiteral("m4a"), QStringLiteral("aac"), QStringLiteral("opus"), QStringLiteral("wma")}.contains(ext); if (isAudio_) isVideo_ = false;
        if (editor_) editor_->hide();
        if (proxy_) proxy_->hide();
        if (movie_) { movie_->stop(); delete movie_; movie_ = nullptr; }
        if (videoTimer_) { videoTimer_->stop(); delete videoTimer_; videoTimer_ = nullptr; } if (!audioAlias_.isEmpty()) { mciSendStringW(reinterpret_cast<LPCWSTR>(QStringLiteral("close %1").arg(audioAlias_).utf16()), nullptr, 0, nullptr); audioAlias_.clear(); }
#ifdef Q_OS_WIN
        if (mfReader_) { mfReader_->Release(); mfReader_ = nullptr; }
#endif
        if (isGif_) {
            movie_ = new QMovie(mediaPath_, QByteArray(), this);
            movie_->setCacheMode(QMovie::CacheAll);
            connect(movie_, &QMovie::frameChanged, this, [this] { update(); });
            connect(movie_, &QMovie::finished, this, [this] { if (movie_) movie_->start(); });
            movie_->start();
        } else if (isVideo_ || isAudio_) {
#ifdef Q_OS_WIN
            CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED); MFStartup(MF_VERSION);
            const std::wstring wpath = QDir::toNativeSeparators(mediaPath_).toStdWString();
            videoPlaying_ = true; videoMuted_ = false;
            if (isVideo_) {
            IMFAttributes *attrs = nullptr;
            MFCreateAttributes(&attrs, 1);
            if (attrs) {
                attrs->SetUINT32(MF_SOURCE_READER_ENABLE_VIDEO_PROCESSING, TRUE);
                IMFSourceReader *reader = nullptr;
                if (SUCCEEDED(MFCreateSourceReaderFromURL(wpath.c_str(), attrs, &reader)) && reader) {
                    IMFMediaType *type = nullptr;
                    MFCreateMediaType(&type);
                    type->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video);
                    type->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_RGB32);
                    reader->SetCurrentMediaType(DWORD(MF_SOURCE_READER_FIRST_VIDEO_STREAM), nullptr, type);
                    type->Release();
                    IMFMediaType *curType = nullptr;
                    if (SUCCEEDED(reader->GetCurrentMediaType(DWORD(MF_SOURCE_READER_FIRST_VIDEO_STREAM), &curType)) && curType) {
                        UINT32 w = 0, h = 0;
                        MFGetAttributeSize(curType, MF_MT_FRAME_SIZE, &w, &h);
                        videoW_ = int(w); videoH_ = int(h);
                        curType->Release();
                    }
                    mfReader_ = reader;
                    videoPlaying_ = true; videoMuted_ = false; audioAlias_ = QStringLiteral("vid_%1").arg(quintptr(this)); mciSendStringW(reinterpret_cast<LPCWSTR>(QStringLiteral("open \"%1\" type mpegvideo alias %2").arg(QDir::toNativeSeparators(mediaPath_), audioAlias_).utf16()), nullptr, 0, nullptr); mciSendStringW(reinterpret_cast<LPCWSTR>(QStringLiteral("play %1 repeat").arg(audioAlias_).utf16()), nullptr, 0, nullptr);
                    videoTimer_ = new QTimer(this);
                    connect(videoTimer_, &QTimer::timeout, this, [this] { decodeNextVideoFrame(); });
                    videoTimer_->start(33);
                    decodeNextVideoFrame();
                }
                attrs->Release();
            }
            }
            if (audioTimer_) { audioTimer_->stop(); delete audioTimer_; audioTimer_ = nullptr; }
            if (mfAudioReader_) { mfAudioReader_->Release(); mfAudioReader_ = nullptr; }
            if (waveOut_) { waveOutReset(waveOut_); waveOutClose(waveOut_); waveOut_ = nullptr; }
            IMFSourceReader *aReader = nullptr;
            if (SUCCEEDED(MFCreateSourceReaderFromURL(wpath.c_str(), nullptr, &aReader)) && aReader) {
                aReader->SetStreamSelection(DWORD(MF_SOURCE_READER_ALL_STREAMS), FALSE);
                aReader->SetStreamSelection(DWORD(MF_SOURCE_READER_FIRST_AUDIO_STREAM), TRUE);
                IMFMediaType *aType = nullptr;
                MFCreateMediaType(&aType);
                aType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio);
                aType->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_PCM);
                if (SUCCEEDED(aReader->SetCurrentMediaType(DWORD(MF_SOURCE_READER_FIRST_AUDIO_STREAM), nullptr, aType))) {
                    IMFMediaType *actualType = nullptr;
                    if (SUCCEEDED(aReader->GetCurrentMediaType(DWORD(MF_SOURCE_READER_FIRST_AUDIO_STREAM), &actualType)) && actualType) {
                        WAVEFORMATEX *pWfx = nullptr; UINT32 cbSize = 0;
                        if (SUCCEEDED(MFCreateWaveFormatExFromMFMediaType(actualType, &pWfx, &cbSize)) && pWfx) {
                            const UINT devId = (s_selectedAudioDeviceId < 0) ? WAVE_MAPPER : UINT(s_selectedAudioDeviceId);
                            if (waveOutOpen(&waveOut_, devId, pWfx, 0, 0, CALLBACK_NULL) == MMSYSERR_NOERROR) {
                                waveOutSetVolume(waveOut_, 0xFFFFFFFF);
                                PROPVARIANT durVar; PropVariantInit(&durVar);
                                if (SUCCEEDED(aReader->GetPresentationAttribute(DWORD(MF_SOURCE_READER_MEDIASOURCE), MF_PD_DURATION, &durVar))) {
                                    audioDuration100Ns_ = durVar.uhVal.QuadPart;
                                    PropVariantClear(&durVar);
                                }
                                audioPos100Ns_ = 0;
                                mfAudioReader_ = aReader;
                                audioTimer_ = new QTimer(this);
                                connect(audioTimer_, &QTimer::timeout, this, [this] { pumpAudioSamples(); });
                                if (isAudio_) { videoPlaying_ = false; audioTimer_->stop(); } else { audioTimer_->start(20); }
                            } else { aReader->Release(); }
                            CoTaskMemFree(pWfx);
                        } else { aReader->Release(); }
                        actualType->Release();
                    } else { aReader->Release(); }
                } else { aReader->Release(); }
                aType->Release();
            }
            if (isAudio_ && !mfAudioReader_) {
                audioAlias_ = QStringLiteral("gs_%1").arg(quintptr(this));
                const QString nativeFile = QDir::toNativeSeparators(mediaPath_);
                mciSendStringW(reinterpret_cast<LPCWSTR>(QStringLiteral("open \"%1\" alias %2").arg(nativeFile, audioAlias_).utf16()), nullptr, 0, nullptr);
                mciSendStringW(reinterpret_cast<LPCWSTR>(QStringLiteral("set %1 time format milliseconds").arg(audioAlias_).utf16()), nullptr, 0, nullptr);
                wchar_t lenBuf[64]{};
                if (mciSendStringW(reinterpret_cast<LPCWSTR>(QStringLiteral("status %1 length").arg(audioAlias_).utf16()), lenBuf, 64, nullptr) == 0) {
                    audioDuration100Ns_ = QString::fromWCharArray(lenBuf).toLongLong() * 10000LL;
                }
                audioTimer_ = new QTimer(this);
                connect(audioTimer_, &QTimer::timeout, this, [this] {
                    if (!videoPlaying_ || audioAlias_.isEmpty()) return;
                    wchar_t posBuf[64]{};
                    if (mciSendStringW(reinterpret_cast<LPCWSTR>(QStringLiteral("status %1 position").arg(audioAlias_).utf16()), posBuf, 64, nullptr) == 0) {
                        audioPos100Ns_ = QString::fromWCharArray(posBuf).toLongLong() * 10000LL;
                        if (audioDuration100Ns_ > 0 && audioPos100Ns_ >= audioDuration100Ns_) {
                            audioPos100Ns_ = 0; videoPlaying_ = false; audioTimer_->stop();
                            mciSendStringW(reinterpret_cast<LPCWSTR>(QStringLiteral("seek %1 to start").arg(audioAlias_).utf16()), nullptr, 0, nullptr);
                        }
                        update();
                    }
                });
            }
#endif
        } else {
            QImageReader reader(mediaPath_);
            reader.setAutoTransform(true);
            const QImage img = reader.read();
            if (!img.isNull()) pixmap_ = QPixmap::fromImage(img);
            else pixmap_.load(mediaPath_);
        }
        resizeToContent();
        update();
    }
    void decodeNextVideoFrame() {
#ifdef Q_OS_WIN
        if (!mfReader_ || !videoPlaying_) return;
        DWORD streamIdx = 0, flags = 0; LONGLONG ts = 0; IMFSample *sample = nullptr;
        HRESULT hr = mfReader_->ReadSample(DWORD(MF_SOURCE_READER_FIRST_VIDEO_STREAM), 0, &streamIdx, &flags, &ts, &sample);
        if (FAILED(hr) || (flags & MF_SOURCE_READERF_ENDOFSTREAM)) {
            if (sample) sample->Release();
            PROPVARIANT var; PropVariantInit(&var); var.vt = VT_I8; var.hVal.QuadPart = 0;
            mfReader_->SetCurrentPosition(GUID_NULL, var); PropVariantClear(&var); return;
        }
        if (sample) {
            IMFMediaBuffer *buf = nullptr;
            if (SUCCEEDED(sample->ConvertToContiguousBuffer(&buf)) && buf) {
                BYTE *data = nullptr; DWORD maxLen = 0, curLen = 0;
                if (SUCCEEDED(buf->Lock(&data, &maxLen, &curLen)) && data && curLen > 0) {
                    UINT32 strideVal = 0; IMFMediaType *mt = nullptr;
                    if (SUCCEEDED(mfReader_->GetCurrentMediaType(DWORD(MF_SOURCE_READER_FIRST_VIDEO_STREAM), &mt)) && mt) {
                        UINT32 w = 0, h = 0;
                        if (SUCCEEDED(MFGetAttributeSize(mt, MF_MT_FRAME_SIZE, &w, &h)) && w > 0 && h > 0) { videoW_ = int(w); videoH_ = int(h); }
                        mt->GetUINT32(MF_MT_DEFAULT_STRIDE, &strideVal); mt->Release();
                    }
                    if (videoW_ > 0 && videoH_ > 0) {
                        const int stride = (strideVal > 0) ? int(strideVal) : int(curLen / DWORD(videoH_));
                        QImage img(data, videoW_, videoH_, stride > 0 ? stride : videoW_ * 4, QImage::Format_RGB32);
                        videoFrame_ = QPixmap::fromImage(img.copy()); update();
                    }
                    buf->Unlock();
                }
                buf->Release();
            }
            sample->Release();
        }
#endif
    }
    void flushAudioBuffer() {
#ifdef Q_OS_WIN
        if (!audioAlias_.isEmpty()) { mciSendStringW(reinterpret_cast<LPCWSTR>(QStringLiteral("setaudio %1 volume to %2").arg(audioAlias_).arg(videoMuted_ ? 0 : int(videoVolume_ * 1000)).utf16()), nullptr, 0, nullptr); if (videoPlaying_) mciSendStringW(reinterpret_cast<LPCWSTR>(QStringLiteral("play %1 from %2").arg(audioAlias_).arg(audioPos100Ns_ / 10000LL).utf16()), nullptr, 0, nullptr); else mciSendStringW(reinterpret_cast<LPCWSTR>(QStringLiteral("seek %1 to %2").arg(audioAlias_).arg(audioPos100Ns_ / 10000LL).utf16()), nullptr, 0, nullptr); } if (waveOut_ && mfAudioReader_) {
            waveOutReset(waveOut_);
            PROPVARIANT var; PropVariantInit(&var); var.vt = VT_I8; var.hVal.QuadPart = audioPos100Ns_;
            mfAudioReader_->SetCurrentPosition(GUID_NULL, var);
            PropVariantClear(&var);
        }
#endif
    }
    void pumpAudioSamples() {
#ifdef Q_OS_WIN
        if (!mfAudioReader_ || !waveOut_ || !videoPlaying_) return;
        for (int i = activeWaveHdrs_.size() - 1; i >= 0; --i) {
            WAVEHDR *wh = activeWaveHdrs_.at(i);
            if (wh->dwFlags & WHDR_DONE) {
                waveOutUnprepareHeader(waveOut_, wh, sizeof(WAVEHDR));
                delete[] wh->lpData;
                delete wh;
                activeWaveHdrs_.removeAt(i);
            }
        }
        while (activeWaveHdrs_.size() < 8) {
            DWORD streamIdx = 0, flags = 0; LONGLONG ts = 0; IMFSample *sample = nullptr;
            HRESULT hr = mfAudioReader_->ReadSample(DWORD(MF_SOURCE_READER_FIRST_AUDIO_STREAM), 0, &streamIdx, &flags, &ts, &sample);
            if (FAILED(hr) || (flags & MF_SOURCE_READERF_ENDOFSTREAM)) {
                if (sample) sample->Release();
                PROPVARIANT var; PropVariantInit(&var); var.vt = VT_I8; var.hVal.QuadPart = 0;
                mfAudioReader_->SetCurrentPosition(GUID_NULL, var); PropVariantClear(&var);
                if (isAudio_) {
                    audioPos100Ns_ = 0;
                    videoPlaying_ = false;
                    if (audioTimer_) audioTimer_->stop();
                    update();
                }
                break;
            }
            if (!sample) break;
            IMFMediaBuffer *buf = nullptr;
            if (SUCCEEDED(sample->ConvertToContiguousBuffer(&buf)) && buf) {
                BYTE *data = nullptr; DWORD maxLen = 0, curLen = 0;
                if (SUCCEEDED(buf->Lock(&data, &maxLen, &curLen)) && data && curLen > 0) {
                    audioPos100Ns_ = ts;
                    auto *wh = new WAVEHDR();
                    wh->lpData = new char[curLen];
                    const float gain = videoMuted_ ? 0.0f : videoVolume_;
                    const int16_t *srcSamples = reinterpret_cast<const int16_t *>(data);
                    int16_t *dstSamples = reinterpret_cast<int16_t *>(wh->lpData);
                    const DWORD sampleCount = curLen / 2;
                    for (DWORD s = 0; s < sampleCount; ++s) {
                        dstSamples[s] = int16_t(qBound(-32768, int(srcSamples[s] * gain), 32767));
                    }
                    update();
                    wh->dwBufferLength = curLen;
                    if (waveOutPrepareHeader(waveOut_, wh, sizeof(WAVEHDR)) == MMSYSERR_NOERROR) {
                        waveOutWrite(waveOut_, wh, sizeof(WAVEHDR));
                        activeWaveHdrs_.append(wh);
                    } else { delete[] wh->lpData; delete wh; }
                    buf->Unlock();
                }
                buf->Release();
            }
            sample->Release();
        }
#endif
    }
    void pauseMedia() {
        videoPlaying_ = false;
        if (videoTimer_) videoTimer_->stop();
        if (audioTimer_) audioTimer_->stop();
#ifdef Q_OS_WIN
        if (waveOut_) waveOutReset(waveOut_);
#endif
        update();
    }
    QString mediaPath() const { return mediaPath_; } int videoWidth() const { return videoW_; } int videoHeight() const { return videoH_; }
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override {
        if (selectionMode_ && event->button() == Qt::LeftButton) { event->ignore(); return; }
        if (event->button() == Qt::LeftButton && event->pos().x() >= width_-20 && event->pos().y() >= height_-20) {
            resizing_ = true; resizeStart_ = event->scenePos(); resizeStartWidth_ = width_; resizeStartHeight_ = height_; event->accept(); return;
        }
        if (event->button() == Qt::LeftButton && event->pos().y() < 42 && event->pos().x() >= width_-36) {
            if (deleteRequested) deleteRequested(this);
            event->accept();
            return;
        }
        if ((isVideo_ || isAudio_) && event->button() == Qt::LeftButton && event->pos().y() >= 44 && event->pos().x() >= 8 && event->pos().x() < width_ - 14 && event->pos().y() < height_ - 14) {
            const QRectF mediaRect(14, 50, qMax(40.0, width_ - 28.0), qMax(40.0, height_ - 66.0));
            if (isVideo_) {
                const QRectF barRect(mediaRect.left() + 8, mediaRect.bottom() - 32, qMin(150.0, mediaRect.width() - 16), 24);
                const QRectF muteRect(barRect.left(), barRect.top(), 26, 24);
                const QRectF sliderRect(barRect.left() + 26, barRect.top(), barRect.width() - 32, 24);
                if (muteRect.contains(event->pos())) {
                    videoMuted_ = !videoMuted_; flushAudioBuffer();
                    update(); event->accept(); return;
                } else if (sliderRect.contains(event->pos())) {
                    videoMuted_ = false;
                    videoVolume_ = float(qBound(0.0, (event->pos().x() - sliderRect.left()) / qMax(1.0, sliderRect.width()), 1.0));
                    draggingVolume_ = true; flushAudioBuffer();
                    update(); event->accept(); return;
                }
            } else if (isAudio_) {
                const QRectF playHitZone(8, 44, 56, qMax(40.0, height_ - 54.0));
                if (playHitZone.contains(event->pos())) {
                    videoPlaying_ = !videoPlaying_;
                    if (videoPlaying_) {
                        if (mfAudioReader_ && waveOut_) {
                            waveOutRestart(waveOut_);
                            if (audioTimer_) audioTimer_->start(20);
                            pumpAudioSamples();
                        } else if (!audioAlias_.isEmpty()) {
                            mciSendStringW(reinterpret_cast<LPCWSTR>(QStringLiteral("play %1 from %2").arg(audioAlias_).arg(audioPos100Ns_ / 10000LL).utf16()), nullptr, 0, nullptr);
                            if (audioTimer_) audioTimer_->start(80);
                        }
                    } else {
                        if (audioTimer_) audioTimer_->stop();
                        if (waveOut_) waveOutPause(waveOut_);
                        if (!audioAlias_.isEmpty()) mciSendStringW(reinterpret_cast<LPCWSTR>(QStringLiteral("pause %1").arg(audioAlias_).utf16()), nullptr, 0, nullptr);
                    }
                    update(); event->accept(); return;
                }
                const QRectF playBtn(mediaRect.left() + 10, mediaRect.center().y() - 18, 36, 36);
                const qreal waveLeft = playBtn.right() + 12;
                const qreal waveRight = mediaRect.right() - 12;
                const qreal volLeft = qMax(waveLeft + 96, waveRight - 88);
                const QRectF muteRect(volLeft, mediaRect.bottom() - 26, 22, 22);
                const QRectF vTrack(volLeft + 24, mediaRect.bottom() - 24, qMax(30.0, waveRight - (volLeft + 26)), 20);
                const QRectF waveRect(waveLeft, mediaRect.top() + 8, qMax(40.0, waveRight - waveLeft), qMax(22.0, mediaRect.height() - 36.0));
                if (muteRect.contains(event->pos())) {
                    videoMuted_ = !videoMuted_; flushAudioBuffer();
                    update(); event->accept(); return;
                } else if (vTrack.contains(event->pos())) {
                    videoMuted_ = false;
                    videoVolume_ = float(qBound(0.0, (event->pos().x() - vTrack.left()) / qMax(1.0, vTrack.width()), 1.0));
                    draggingVolume_ = true; flushAudioBuffer();
                    update(); event->accept(); return;
                } else if (waveRect.contains(event->pos()) && mfAudioReader_ && audioDuration100Ns_ > 0) {
                    const qreal ratio = qBound(0.0, (event->pos().x() - waveRect.left()) / qMax(1.0, waveRect.width()), 1.0);
                    audioPos100Ns_ = LONGLONG(ratio * audioDuration100Ns_);
                    flushAudioBuffer();
                    draggingSeek_ = true;
                    update(); event->accept(); return;
                }
            }
            videoPlaying_ = !videoPlaying_;
            if (videoTimer_) { if (videoPlaying_) videoTimer_->start(33); else videoTimer_->stop(); }
            if (audioTimer_) { if (videoPlaying_) if (isAudio_) { videoPlaying_ = false; audioTimer_->stop(); } else { audioTimer_->start(20); } else audioTimer_->stop(); }
            update(); event->accept(); return;
        }
        dragging_ = event->button() == Qt::LeftButton; lastScenePos_ = event->scenePos(); event->accept();
    }
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override {
        if (event->button() == Qt::LeftButton && event->pos().y() < 42 && event->pos().x() < width_-48) { editHeading(); event->accept(); return; }
        QGraphicsObject::mouseDoubleClickEvent(event);
    }
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override {
        if (selectionMode_) { QGraphicsObject::mouseMoveEvent(event); return; }
        if (draggingVolume_) {
            const QRectF mediaRect(14, 50, qMax(40.0, width_ - 28.0), qMax(40.0, height_ - 66.0));
            if (isVideo_) {
                const QRectF barRect(mediaRect.left() + 8, mediaRect.bottom() - 32, qMin(150.0, mediaRect.width() - 16), 24);
                videoVolume_ = float(qBound(0.0, (event->pos().x() - (barRect.left() + 26)) / qMax(1.0, barRect.width() - 32), 1.0));
            } else if (isAudio_) {
                const qreal waveLeft = mediaRect.left() + 58;
                const qreal waveRight = mediaRect.right() - 12;
                const qreal volLeft = qMax(waveLeft + 96, waveRight - 88);
                videoVolume_ = float(qBound(0.0, (event->pos().x() - (volLeft + 24)) / qMax(1.0, waveRight - (volLeft + 26)), 1.0));
            }
            update(); event->accept(); return;
        }
        if (draggingSeek_ && isAudio_ && mfAudioReader_ && audioDuration100Ns_ > 0) {
            const qreal waveLeft = 14 + 58;
            const qreal waveRight = width_ - 26;
            const qreal ratio = qBound(0.0, (event->pos().x() - waveLeft) / qMax(1.0, waveRight - waveLeft), 1.0);
            audioPos100Ns_ = LONGLONG(ratio * audioDuration100Ns_);
            PROPVARIANT var; PropVariantInit(&var); var.vt = VT_I8; var.hVal.QuadPart = audioPos100Ns_;
            mfAudioReader_->SetCurrentPosition(GUID_NULL, var); PropVariantClear(&var);
            update(); event->accept(); return;
        }
        if (resizing_) {
            if (!mediaPath_.isEmpty() && (event->modifiers() & Qt::ShiftModifier)) {
                qreal natW = pixmap_.width(), natH = pixmap_.height();
                if (isGif_ && movie_ && movie_->isValid()) { natW = movie_->currentPixmap().width(); natH = movie_->currentPixmap().height(); }
                else if (isVideo_ && videoW_ > 0 && videoH_ > 0) { natW = videoW_; natH = videoH_; }
                if (natW > 0 && natH > 0) {
                    const qreal newW = qMax(180.0, resizeStartWidth_ + (event->scenePos().x() - resizeStart_.x()));
                    manualWidth_ = newW; manualHeight_ = qMax(140.0, (newW - 28.0) * (natH / natW) + 66.0);
                    resizeToContent(); event->accept(); return;
                }
            }
            const QPointF delta = event->scenePos()-resizeStart_;
            manualWidth_=qMax(baseWidth_,resizeStartWidth_+delta.x());
            manualHeight_=qMax(baseHeight_,resizeStartHeight_+delta.y());
            resizeToContent(); event->accept(); return;
        }
        if (!dragging_) return;
        setPos(pos() + (event->scenePos() - lastScenePos_)); lastScenePos_ = event->scenePos(); event->accept();
    }
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override {
        if (selectionMode_) { QGraphicsObject::mouseReleaseEvent(event); return; } if (draggingVolume_ || draggingSeek_) { draggingVolume_ = false; draggingSeek_ = false; event->accept(); return; }
        if (resizing_) { resizing_=false; if(contentChanged)contentChanged(); event->accept(); return; }
        dragging_ = false; event->accept();
    }
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override {
        const QVariant result=QGraphicsObject::itemChange(change,value);
        if(change==QGraphicsItem::ItemPositionHasChanged&&moved)moved();
        return result;
    }
private:
    void resizeToContent(){
        if (!mediaPath_.isEmpty()) {
            const qreal w = qMax(180.0, manualWidth_), h = qMax(isAudio_ ? 116.0 : 140.0, manualHeight_);
            const bool ch = (qAbs(w - width_) > 1.0 || qAbs(h - height_) > 1.0);
            if (ch) { prepareGeometryChange(); width_ = w; height_ = h; }
            headingEditor_->setFixedWidth(qMax(100, int(width_ - 62))); update();
            if (ch && geometryChanged) geometryChanged();
            return;
        }
        const QFontMetricsF metrics(editor_->font()); qreal longest=0;
        for(const QString &line:content_.split(QLatin1Char('\n'))) longest=qMax(longest,metrics.horizontalAdvance(line));
        const qreal wantedWidth=qMax(baseWidth_,qMax(manualWidth_,longest+42.0)); bool changed=false;
        if(qAbs(wantedWidth-width_)>1.0){prepareGeometryChange();width_=wantedWidth;changed=true;}
        editor_->setFixedWidth(qMax(158,int(width_-42))); headingEditor_->setFixedWidth(qMax(100,int(width_-62))); proxy_->resize(width_-42,height_-68);
        editor_->document()->setTextWidth(qMax(100.0,width_-48.0));
        const qreal documentHeight=editor_->document()->documentLayout()->documentSize().height();
        const qreal wantedHeight=qMax(baseHeight_,qMax(manualHeight_,documentHeight+68.0));
        if(qAbs(wantedHeight-height_)>1.0){prepareGeometryChange();height_=wantedHeight;changed=true;}
        editor_->setFixedHeight(qMax(52,int(height_-68))); proxy_->resize(width_-42,height_-68); update();
        if(changed&&geometryChanged)geometryChanged();
    }
    QString heading_, content_, accent_, id_, theme_;
    qreal width_=260, height_=160, manualWidth_=260, manualHeight_=160, baseWidth_=260, baseHeight_=160;
    QTextEdit *editor_=nullptr; QLineEdit *headingEditor_=nullptr;
    QGraphicsProxyWidget *proxy_=nullptr; QGraphicsProxyWidget *headingProxy_=nullptr;
    QPointF lastScenePos_, resizeStart_; qreal resizeStartWidth_=260, resizeStartHeight_=174;
    bool dragging_=false, resizing_=false, selectionMode_=false;
    QString mediaPath_; QPixmap pixmap_; QMovie *movie_ = nullptr; bool isGif_ = false, isVideo_ = false, isAudio_ = false; float videoVolume_ = 0.85f; LONGLONG audioDuration100Ns_ = 0; LONGLONG audioPos100Ns_ = 0; bool draggingVolume_ = false; bool draggingSeek_ = false;
    QTimer *videoTimer_ = nullptr; bool videoPlaying_ = false; bool videoMuted_ = false; QString audioAlias_; int videoW_ = 0, videoH_ = 0; QPixmap videoFrame_;
#ifdef Q_OS_WIN
    IMFSourceReader *mfReader_ = nullptr;
    IMFSourceReader *mfAudioReader_ = nullptr;
    HWAVEOUT waveOut_ = nullptr;
    QTimer *audioTimer_ = nullptr;
    QList<WAVEHDR *> activeWaveHdrs_;
#endif
public:
    ~NoteCard() override {
        if (movie_) { movie_->stop(); delete movie_; }
        if (videoTimer_) { videoTimer_->stop(); delete videoTimer_; }
        if (audioTimer_) { audioTimer_->stop(); delete audioTimer_; }
#ifdef Q_OS_WIN
        if (waveOut_) { waveOutReset(waveOut_); waveOutClose(waveOut_); waveOut_ = nullptr; }
        if (mfReader_) { mfReader_->Release(); mfReader_ = nullptr; }
        if (mfAudioReader_) { mfAudioReader_->Release(); mfAudioReader_ = nullptr; }
#endif
    }
};

class NoteEdge final : public QGraphicsPathItem {
public:
    NoteEdge(NoteCard *from, NoteCard *to) : from_(from), to_(to) {
        QPen edgePen(QColor(QStringLiteral("#ffffff")), 2.0, Qt::SolidLine, Qt::RoundCap); edgePen.setCosmetic(true);
        setPen(edgePen); setBrush(Qt::NoBrush); setZValue(1);
        setFlag(QGraphicsItem::ItemSendsGeometryChanges); setFlag(QGraphicsItem::ItemIsSelectable);
        setAcceptedMouseButtons(Qt::LeftButton | Qt::RightButton); refresh();
    }
    std::function<void(NoteEdge *)> deleteRequested;
    NoteCard *from() const { return from_; } NoteCard *to() const { return to_; }
    bool touches(const NoteCard *card) const { return from_ == card || to_ == card; }
    void refresh() {
        if (!from_ || !to_) return;
        const QRectF fromRect = from_->sceneBoundingRect(), toRect = to_->sceneBoundingRect();
        const QPointF fromCenter = fromRect.center(), toCenter = toRect.center(), direction = toCenter - fromCenter;
        auto borderPoint = [](const QRectF &rect, const QPointF &toward) {
            const QPointF center = rect.center(), delta = toward - center;
            const qreal dx = qAbs(delta.x()) < 0.001 ? 1e9 : (rect.width() / 2.0) / qAbs(delta.x());
            const qreal dy = qAbs(delta.y()) < 0.001 ? 1e9 : (rect.height() / 2.0) / qAbs(delta.y());
            return center + delta * qMin(dx, dy);
        };
        const QPointF start = borderPoint(fromRect, toCenter), end = borderPoint(toRect, fromCenter);
        const qreal span = qMax(54.0, qAbs(direction.x()) * 0.42), sign = direction.x() >= 0 ? 1.0 : -1.0;
        QPainterPath path(start); path.cubicTo(start + QPointF(span * sign, 0), end - QPointF(span * sign, 0), end); setPath(path);
    }
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override {
        if (deleteRequested && (event->button() == Qt::LeftButton || event->button() == Qt::RightButton)) { deleteRequested(this); event->accept(); return; }
        QGraphicsPathItem::mousePressEvent(event);
    }
private:
    NoteCard *from_; NoteCard *to_;
};
class NotesView final : public QGraphicsView {
public:
    explicit NotesView(QGraphicsScene *scene, QWidget *parent = nullptr) : QGraphicsView(scene, parent) {
        setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform | QPainter::TextAntialiasing);
        setViewportUpdateMode(QGraphicsView::MinimalViewportUpdate);
        setOptimizationFlag(QGraphicsView::DontAdjustForAntialiasing, true);
        setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff); setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        setFrameShape(QFrame::NoFrame); setBackgroundBrush(Qt::NoBrush); setMouseTracking(true);
        setAcceptDrops(true); if (viewport()) viewport()->setAcceptDrops(true);
    }
    std::function<void(const QPointF &)> addAt; std::function<void(QGraphicsItem *)> linkAt;
    std::function<void(NoteCard *)> startLinkAt; std::function<bool()> hasLinkStart;
    std::function<bool(NoteCard *)> isLinkStart; std::function<void(const QPointF &)> previewAt;
    std::function<void()> cancelLink; std::function<void()> leaveNotes;
    std::function<void(const QStringList &, const QPointF &)> filesDroppedAt;
    bool drawMode_ = false, eraseMode_ = false, linkMode_ = false, selectMode_ = false;
    void resetInteractions() {
        panning_ = false; movingSelection_ = false; isDrawing_ = false;
        linkGestureActive_ = false; linkGestureSource_ = nullptr;
        selectionStartPositions_.clear();
        setDragMode(QGraphicsView::NoDrag);
        if (viewport()) viewport()->unsetCursor();
        unsetCursor();
    }
    void setZoomOutLimit(int percent) { minimumZoom_ = qBound(0.10, percent / 100.0, 0.50); if (zoom_ < minimumZoom_) { const qreal f = minimumZoom_ / zoom_; zoom_ = minimumZoom_; scale(f, f); } }
    QString drawColor_ = QStringLiteral("#a78bfa"); qreal brushWidth_ = 3.0; QPointF lastDrawPt_; bool isDrawing_ = false;
    QList<QGraphicsPathItem *> drawnItems_;
    void clearStrokes() { for (auto *it : drawnItems_) { scene()->removeItem(it); delete it; } drawnItems_.clear(); }
    void undoStroke() { if (drawnItems_.isEmpty()) return; auto *it = drawnItems_.takeLast(); scene()->removeItem(it); delete it; }
protected:
    NoteCard *cardAt(const QPoint &position) const {
        for (QGraphicsItem *hit : scene()->items(mapToScene(position))) {
            QGraphicsItem *root = hit; while (root->parentItem()) root = root->parentItem();
            if (auto *card = dynamic_cast<NoteCard *>(root)) return card;
        }
        return nullptr;
    }
    void dragEnterEvent(QDragEnterEvent *e) override { if (e->mimeData() && e->mimeData()->hasUrls()) e->acceptProposedAction(); else QGraphicsView::dragEnterEvent(e); }
    void dragMoveEvent(QDragMoveEvent *e) override { if (e->mimeData() && e->mimeData()->hasUrls()) e->acceptProposedAction(); else QGraphicsView::dragMoveEvent(e); }
    void dropEvent(QDropEvent *e) override {
        if (e->mimeData() && e->mimeData()->hasUrls()) {
            QStringList paths; for (const QUrl &u : e->mimeData()->urls()) if (u.isLocalFile()) paths.append(u.toLocalFile());
            if (!paths.isEmpty() && filesDroppedAt) filesDroppedAt(paths, mapToScene(e->position().toPoint()));
            e->acceptProposedAction(); return;
        }
        QGraphicsView::dropEvent(e);
    }
    void drawBackground(QPainter *painter, const QRectF &rect) override {
        painter->save();
        painter->fillRect(rect, QColor(14, 17, 23, 240));
        qreal step = 28.0; while (step * zoom_ < 14.0) step *= 2.0;
        if (step <= 224.0) {
            painter->setPen(Qt::NoPen); painter->setBrush(QColor(180, 195, 215, 26));
            const qreal left = std::floor(rect.left() / step) * step, top = std::floor(rect.top() / step) * step;
            for (qreal x = left; x < rect.right(); x += step)
                for (qreal y = top; y < rect.bottom(); y += step)
                    painter->drawEllipse(QPointF(x, y), 1.15, 1.15);
        }
        painter->restore();
    }
    void keyPressEvent(QKeyEvent *event) override
    {
        if (event->key() == Qt::Key_Escape) {
            if (cancelLink) cancelLink();
            if (leaveNotes) leaveNotes();
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
            const qreal next = qBound(minimumZoom_, zoom_ * factor, 2.2);
            const qreal delta = next / zoom_;
            zoom_ = next;
            scale(delta, delta);
            event->accept();
            return;
        }
        QGraphicsView::wheelEvent(event);
    }

    QSet<QGraphicsItem *> prevSelected_;
    QPoint rubberOrigin_;
    void mousePressEvent(QMouseEvent *event) override
    {
        if (selectMode_ && event->button() == Qt::LeftButton) {
            movingSelection_ = false;
            panning_ = false;
            prevSelected_.clear();
            for (QGraphicsItem *it : scene()->selectedItems()) prevSelected_.insert(it);
            rubberOrigin_ = event->pos();
            setDragMode(QGraphicsView::RubberBandDrag);
            QGraphicsView::mousePressEvent(event);
            for (QGraphicsItem *it : prevSelected_) it->setSelected(true);
            event->accept();
            return;
        }
        mousePressEventInner(event);
    }
    void mousePressEventInner(QMouseEvent *event)
    {
        if (linkMode_ && (event->button() == Qt::RightButton || event->button() == Qt::MiddleButton)) {
            if (cancelLink) cancelLink();
            linkGestureActive_ = false;
            linkGestureSource_ = nullptr;
            event->accept();
            return;
        }
        if (linkMode_ && event->button() == Qt::LeftButton) {
            if (NoteCard *card = cardAt(event->position().toPoint())) {
                if (hasLinkStart && hasLinkStart()) {
                    if (isLinkStart && !isLinkStart(card) && linkAt) {
                        linkAt(card);
                    } else if (cancelLink) {
                        cancelLink();
                    }
                    linkGestureActive_ = false;
                    linkGestureSource_ = nullptr;
                } else {
                    if (startLinkAt) startLinkAt(card);
                    linkGestureActive_ = true;
                    linkGestureSource_ = card;
                }
                event->accept();
                return;
            } else if (cancelLink) {
                cancelLink();
                linkGestureActive_ = false;
                linkGestureSource_ = nullptr;
                event->accept();
                return;
            }
        }
        if(selectMode_&&event->button()==Qt::LeftButton){
            QGraphicsItem *hit=itemAt(event->position().toPoint());
            while(hit&&hit->parentItem())hit=hit->parentItem();
            if(hit){
                if(!(event->modifiers()&Qt::ControlModifier)&&!hit->isSelected())scene()->clearSelection();
                hit->setSelected(true);
                selectionStartPositions_.clear();
                for(QGraphicsItem *selected:scene()->selectedItems())if(selected->flags()&QGraphicsItem::ItemIsMovable)selectionStartPositions_.insert(selected,selected->pos());
                selectionMoveStart_=mapToScene(event->position().toPoint());
                movingSelection_=true;
                event->accept();
                return;
            }
        }
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
        if (event->button() == Qt::LeftButton && !selectMode_ && !itemAt(event->position().toPoint())) {
            panning_ = true;
            panStart_ = event->position().toPoint();
            setCursor(Qt::ClosedHandCursor);
            event->accept();
            return;
        }
        if (selectMode_ && event->button() == Qt::LeftButton) {
            prevSelected_.clear();
            for (QGraphicsItem *it : scene()->selectedItems()) prevSelected_.insert(it);
            rubberOrigin_ = event->pos();
            if (!cardAt(event->pos())) {
                setDragMode(QGraphicsView::RubberBandDrag);
            } else {
                setDragMode(QGraphicsView::NoDrag);
            }
        }
        QGraphicsView::mousePressEvent(event);
        if (selectMode_ && event->button() == Qt::LeftButton) {
            for (QGraphicsItem *it : prevSelected_) it->setSelected(true);
        }
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
        if(linkGestureActive_){if(previewAt)previewAt(mapToScene(event->position().toPoint()));event->accept();return;}
        if(movingSelection_){const QPointF delta=mapToScene(event->position().toPoint())-selectionMoveStart_;for(auto it=selectionStartPositions_.cbegin();it!=selectionStartPositions_.cend();++it)it.key()->setPos(it.value()+delta);event->accept();return;}
        if (drawMode_ && isDrawing_ && (event->buttons() & Qt::LeftButton)) {
            const QPointF curPt = mapToScene(event->position().toPoint());
            QPainterPath seg; seg.moveTo(lastDrawPt_); seg.lineTo(curPt);
            auto *item = scene()->addPath(seg, QPen(QColor(drawColor_), brushWidth_, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
            item->setZValue(5);
            item->setFlag(QGraphicsItem::ItemIsSelectable,true);
            item->setFlag(QGraphicsItem::ItemIsMovable,true);
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
        if (selectMode_ && event->button() == Qt::LeftButton) {
            const QRect band = QRect(rubberOrigin_, event->pos()).normalized();
            QGraphicsView::mouseReleaseEvent(event);
            setDragMode(QGraphicsView::NoDrag);
            setCursor(Qt::ArrowCursor);
            QSet<QGraphicsItem *> hitSet;
            if (band.width() > 4 || band.height() > 4) {
                for (QGraphicsItem *hit : scene()->items(mapToScene(band))) {
                    QGraphicsItem *root = hit; while (root->parentItem()) root = root->parentItem();
                    if (dynamic_cast<NoteCard *>(root)) hitSet.insert(root);
                }
            } else if (NoteCard *clickedCard = cardAt(event->pos())) {
                hitSet.insert(clickedCard);
            }
            for (QGraphicsItem *it : scene()->items()) {
                if (dynamic_cast<NoteCard *>(it)) {
                    const bool wasSel = prevSelected_.contains(it);
                    const bool inBand = hitSet.contains(it);
                    it->setSelected(inBand ? !wasSel : wasSel);
                }
            }
            scene()->update();
            return;
        }
        panning_ = false; if (viewport()) viewport()->unsetCursor(); if (dragMode() == QGraphicsView::ScrollHandDrag) setDragMode(QGraphicsView::NoDrag);
        unsetCursor();
        QGraphicsView::mouseReleaseEvent(event);
    }
private:
    bool panning_ = false;
    bool linkGestureActive_ = false;
    NoteCard *linkGestureSource_ = nullptr;
    bool movingSelection_ = false;
    QPointF selectionMoveStart_;
    QHash<QGraphicsItem *, QPointF> selectionStartPositions_;
    QPoint panStart_;
    qreal zoom_ = 1.0;
    qreal minimumZoom_ = 0.18;
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
    rootLayout->setContentsMargins(10, 6, 10, 10);
    rootLayout->setSpacing(8);
    root_->setAttribute(Qt::WA_TranslucentBackground, true);

    auto *header = new QWidget(root_);
    header->setObjectName(QStringLiteral("topBar"));
    auto *headerLayout = new QHBoxLayout(header);
    headerLayout->setContentsMargins(8, 2, 2, 2);
    const QIcon appIcon(QStringLiteral(":/app.ico"));
    QIcon windowIcon = appIcon;
    if (windowIcon.isNull())
        windowIcon = QIcon(QCoreApplication::applicationDirPath() + QStringLiteral("/app.ico"));
    if (windowIcon.isNull())
        windowIcon = QIcon(QStringLiteral("app.ico"));
    QApplication::setWindowIcon(windowIcon);
    setWindowIcon(windowIcon);
    QFont uiFont;
    uiFont.setFamilies({QStringLiteral("Segoe UI Variable Text"), QStringLiteral("Segoe UI"), QStringLiteral("Noto Sans"), QStringLiteral("Ubuntu")});
    uiFont.setPointSize(10);
    uiFont.setStyleStrategy(QFont::PreferAntialias);
    QApplication::setFont(uiFont);
    auto *brandIcon = new QLabel(header);
    brandIcon->setPixmap(windowIcon.pixmap(18, 18));
    brandIcon->setFixedSize(18, 18);
    auto *brand = new QLabel(QStringLiteral("Terminal"), header);
    brand->setStyleSheet(QStringLiteral("font-weight:600;letter-spacing:0.4px;color:#e8eaef;"));
    clockLabel_ = new QLabel(header);
    clockLabel_->setObjectName(QStringLiteral("muted"));
    auto *spacer = new QWidget(header);
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    auto *minimize = button(QString(), header, QStringLiteral("trafficYellow"));
    auto *maximize = button(QString(), header, QStringLiteral("trafficGreen"));
    auto *close = button(QString(), header, QStringLiteral("trafficRed"));
    for (QPushButton *control : {close, minimize, maximize}) {
        control->setFixedSize(12, 12);
        control->setStyleSheet(QStringLiteral(
            "QPushButton{min-width:12px;max-width:12px;min-height:12px;max-height:12px;padding:0;border:0;border-radius:6px;"
            "color:#1a1b1e;font-size:10px;font-weight:700;background:%1;}QPushButton:hover{background:%2;}")
            .arg(control->objectName() == QStringLiteral("trafficRed") ? QStringLiteral("#ff5f56")
                 : control->objectName() == QStringLiteral("trafficYellow") ? QStringLiteral("#ffbd2e")
                                                                           : QStringLiteral("#27c93f"),
                 control->objectName() == QStringLiteral("trafficRed") ? QStringLiteral("#ff776f")
                 : control->objectName() == QStringLiteral("trafficYellow") ? QStringLiteral("#ffd45c")
                                                                           : QStringLiteral("#48db5f")));
        control->installEventFilter(this);
    }
    header->setCursor(Qt::OpenHandCursor);
    header->installEventFilter(this);
    brandIcon->installEventFilter(this);
    brand->installEventFilter(this);
    clockLabel_->installEventFilter(this);
    spacer->installEventFilter(this);
    headerLayout->addWidget(brandIcon);
    headerLayout->addWidget(brand);
    headerLayout->addSpacing(12);
    headerLayout->addWidget(clockLabel_);
    headerLayout->addWidget(spacer);
    headerLayout->addWidget(minimize);
    headerLayout->addWidget(maximize);
    headerLayout->addWidget(close);
    rootLayout->addWidget(header);
    connect(minimize, &QPushButton::clicked, this, &QWidget::showMinimized);
    connect(maximize, &QPushButton::clicked, this, [this] { if (isMaximized()) showNormal(); else showMaximized(); });
    connect(close, &QPushButton::clicked, this, &QWidget::close);
#ifdef Q_OS_WIN
    const HWND nativeWindow = reinterpret_cast<HWND>(winId());
    LONG_PTR nativeStyle = GetWindowLongPtrW(nativeWindow, GWL_STYLE);
    nativeStyle |= WS_THICKFRAME | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_CAPTION;
    SetWindowLongPtrW(nativeWindow, GWL_STYLE, nativeStyle);
    SetWindowPos(nativeWindow, nullptr, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
    const DWORD cornerPreference = 2;
    DwmSetWindowAttribute(nativeWindow, 33, &cornerPreference, sizeof(cornerPreference));
    const DWORD backdropType = 3;
    DwmSetWindowAttribute(nativeWindow, 38, &backdropType, sizeof(backdropType));
#endif

    auto *columns = new QHBoxLayout;
    columns->setSpacing(12);
    rootLayout->addLayout(columns, 1);
    nav_ = new QWidget(root_);
    nav_->setObjectName(QStringLiteral("nav"));
    nav_->setFixedWidth(190);
    nav_->setCursor(Qt::OpenHandCursor);
    nav_->installEventFilter(this);
    auto *navLayout = new QVBoxLayout(nav_);
    navLayout->setContentsMargins(8, 8, 8, 8);
    navLayout->setSpacing(6);
    class NavDragStrip final : public QWidget {
    public:
        explicit NavDragStrip(QWidget *parent) : QWidget(parent) { setFixedHeight(22); setCursor(Qt::OpenHandCursor); }
    protected:
        void paintEvent(QPaintEvent *) override {
            QPainter p(this); p.setRenderHint(QPainter::Antialiasing);
            p.setPen(QPen(QColor(255,255,255,75), 1.4));
            const qreal cx = width() / 2.0;
            p.drawLine(QPointF(cx-16, height()/2.0), QPointF(cx+16, height()/2.0));
        }
    };
    auto *dragStrip = new NavDragStrip(nav_);
    dragStrip->setObjectName(QStringLiteral("windowDragHandle"));
    dragStrip->setToolTip(QStringLiteral("Перетащите, чтобы переместить окно"));
    dragStrip->installEventFilter(this);
    navLayout->addWidget(dragStrip);
    auto *newConsole = button(QStringLiteral("＋  Новая консоль"), nav_);
    newConsole->setToolTip(QStringLiteral("Новая вкладка терминала"));
    tabHost_ = new QWidget(nav_);
    auto *tabLayout = new QVBoxLayout(tabHost_);
    tabLayout->setContentsMargins(0, 4, 0, 0);
    tabLayout->setSpacing(6);
    tabLayout->setAlignment(Qt::AlignTop);
    navLayout->addWidget(newConsole);
    navLayout->addWidget(tabHost_);
    auto *navDragSpace = new QWidget(nav_);
    navDragSpace->setObjectName(QStringLiteral("navDragSpace"));
    navDragSpace->setCursor(Qt::OpenHandCursor);
    navDragSpace->setStyleSheet(QStringLiteral("background:transparent;border:0;"));
    navDragSpace->installEventFilter(this);
    navLayout->addWidget(navDragSpace, 1);
    notesNavButton_ = button(QStringLiteral("Заметки"), nav_);
    settingsNavButton_ = button(QStringLiteral("Настройки"), nav_);
    settingsNavButton_->setToolTip(QStringLiteral("Настройки (Esc — закрыть)"));
    for (QPushButton *navButton : {notesNavButton_, settingsNavButton_}) {
        navButton->setIcon(navigationDotIcon(false));
        navButton->setIconSize(QSize(8, 8));
        navButton->setStyleSheet(QStringLiteral("QPushButton{text-align:left;padding:6px 10px;}"));
    }
    QSettings boardStore(QString::fromLatin1(kSettingsOrg), QString::fromLatin1(kSettingsApp));
    const QStringList storedBoards = boardStore.value(QStringLiteral("noteBoardNames")).toStringList();
    if (!storedBoards.isEmpty()) s_noteBoardNames = storedBoards;

    auto *boardsDrawer = new QFrame(nav_);
    boardsDrawer->setVisible(false);
    boardsDrawer->setStyleSheet(QStringLiteral("QFrame{background:rgba(255,255,255,0.055);border:1px solid rgba(255,255,255,0.12);border-radius:12px;}"));
    auto *boardsLay = new QVBoxLayout(boardsDrawer);
    boardsLay->setContentsMargins(6, 5, 6, 6);
    boardsLay->setSpacing(4);


    auto *boardsListBox = new QWidget(boardsDrawer);
    auto *boardsListLay = new QVBoxLayout(boardsListBox);
    boardsListLay->setContentsMargins(0, 0, 0, 0);
    boardsListLay->setSpacing(3);
    boardsLay->addWidget(boardsListBox);

    auto *newBoardBtn = new QPushButton(QStringLiteral("+ Новое поле"), boardsDrawer);
    newBoardBtn->setCursor(Qt::PointingHandCursor);
    newBoardBtn->setStyleSheet(QStringLiteral("QPushButton{background:rgba(126,184,218,0.20);border:1px solid rgba(126,184,218,0.38);border-radius:8px;padding:6px 10px;color:#e3f2fa;font-size:12px;font-weight:600;text-align:left;}QPushButton:hover{background:rgba(126,184,218,0.32);}"));
    boardsLay->addWidget(newBoardBtn);

    auto rebuildBoardsMenu = std::make_shared<std::function<void()>>();
    *rebuildBoardsMenu = [this, boardsListBox, boardsListLay, rebuildBoardsMenu]() {
        while (QLayoutItem *it = boardsListLay->takeAt(0)) {
            if (it->widget()) it->widget()->deleteLater();
            delete it;
        }
        for (int i = 0; i < s_noteBoardNames.size(); ++i) {
            auto *row = new QHBoxLayout;
            row->setContentsMargins(0, 0, 0, 0);
            row->setSpacing(3);
            const bool active = (i == s_activeNoteBoard);
            auto *bBtn = new QPushButton(s_noteBoardNames.at(i), boardsListBox);
            bBtn->setIcon(navigationDotIcon(active));
            bBtn->setIconSize(QSize(7, 7));
            bBtn->setCursor(Qt::PointingHandCursor);
            bBtn->setStyleSheet(active
                ? QStringLiteral("QPushButton{background:rgba(255,255,255,0.16);border:0;border-radius:7px;padding:6px 8px;color:#fff;font-weight:600;text-align:left;font-size:12.5px;}")
                : QStringLiteral("QPushButton{background:transparent;border:0;border-radius:7px;padding:6px 8px;color:rgba(235,240,248,0.75);text-align:left;font-size:12.5px;}QPushButton:hover{background:rgba(255,255,255,0.09);color:#fff;}"));
            connect(bBtn, &QPushButton::clicked, this, [this, i, rebuildBoardsMenu]() {
                if (i != s_activeNoteBoard) {
                    saveNotes();
                    s_activeNoteBoard = i;
                    if (noteScene_) { noteScene_->clear(); noteEdges_.clear(); linkStart_ = nullptr; previewPath_ = nullptr; previewGlow_ = nullptr; }
                    if (noteView_) static_cast<NotesView *>(noteView_)->clearStrokes();
                    loadNotes();
                }
                setNotesVisible(true);
                QTimer::singleShot(0, this, [rebuildBoardsMenu] { if (*rebuildBoardsMenu) (*rebuildBoardsMenu)(); });
            });
            row->addWidget(bBtn, 1);
            if (i > 0) {
                auto *delB = new QPushButton(QStringLiteral("×"), boardsListBox);
                delB->setFixedSize(20, 20); delB->setCursor(Qt::PointingHandCursor); delB->setToolTip(QStringLiteral("Удалить поле"));
                delB->setStyleSheet(QStringLiteral("QPushButton{background:rgba(255,255,255,0.04);border:1px solid rgba(255,255,255,0.08);border-radius:10px;color:rgba(235,240,248,0.6);font-size:12px;font-weight:600;padding:0;min-height:20px;max-width:20px;}QPushButton:hover{background:rgba(255,95,86,0.28);border-color:rgba(255,110,100,0.65);color:#ffd6d3;}"));
                connect(delB, &QPushButton::clicked, this, [this, i, rebuildBoardsMenu]() {
                    QFile::remove(QStringLiteral("%1/notes_board_%2.json").arg(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)).arg(i));
                    s_noteBoardNames.removeAt(i);
                    QSettings st(QString::fromLatin1(kSettingsOrg), QString::fromLatin1(kSettingsApp));
                    st.setValue(QStringLiteral("noteBoardNames"), s_noteBoardNames);
                    if (s_activeNoteBoard >= s_noteBoardNames.size()) s_activeNoteBoard = 0;
                    if (noteScene_) { noteScene_->clear(); noteEdges_.clear(); }
                    loadNotes();
                    QTimer::singleShot(0, this, [rebuildBoardsMenu] { if (*rebuildBoardsMenu) (*rebuildBoardsMenu)(); });
                });
                row->addWidget(delB);
            }
            auto *wrap = new QWidget(boardsListBox);
            wrap->setLayout(row);
            boardsListLay->addWidget(wrap);
        }
    };
    (*rebuildBoardsMenu)();

    connect(newBoardBtn, &QPushButton::clicked, this, [this, rebuildBoardsMenu]() {
        bool ok = false;
        const QString name = QInputDialog::getText(this, QStringLiteral("Новое поле заметок"), QStringLiteral("Название страницы:"), QLineEdit::Normal, QStringLiteral("Поле %1").arg(s_noteBoardNames.size() + 1), &ok).trimmed();
        if (!ok || name.isEmpty()) return;
        saveNotes();
        s_noteBoardNames.append(name);
        QSettings st(QString::fromLatin1(kSettingsOrg), QString::fromLatin1(kSettingsApp));
        st.setValue(QStringLiteral("noteBoardNames"), s_noteBoardNames);
        s_activeNoteBoard = s_noteBoardNames.size() - 1;
        if (noteScene_) { noteScene_->clear(); noteEdges_.clear(); linkStart_ = nullptr; previewPath_ = nullptr; previewGlow_ = nullptr; }
        if (noteView_) static_cast<NotesView *>(noteView_)->clearStrokes();
        saveNotes();
        setNotesVisible(true);
        QTimer::singleShot(0, this, [rebuildBoardsMenu] { if (*rebuildBoardsMenu) (*rebuildBoardsMenu)(); });
    });

    auto *notesRowWidget = new QWidget(nav_);
    auto *notesRowLayout = new QHBoxLayout(notesRowWidget);
    notesRowLayout->setContentsMargins(0, 0, 0, 0);
    notesRowLayout->setSpacing(0);

    notesNavButton_->setStyleSheet(QStringLiteral("QPushButton{text-align:left;padding:6px 10px;border-top-right-radius:0px;border-bottom-right-radius:0px;border-right:1px solid rgba(255,255,255,0.15);}"));

    auto *boardsArrowBtn = new QPushButton(QStringLiteral("▴"), notesRowWidget);
    boardsArrowBtn->setFixedWidth(28);
    boardsArrowBtn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    boardsArrowBtn->setCursor(Qt::PointingHandCursor);
    boardsArrowBtn->setToolTip(QStringLiteral("Показать / скрыть список полей заметок"));
    boardsArrowBtn->setStyleSheet(QStringLiteral("QPushButton{border-top-left-radius:0px;border-bottom-left-radius:0px;border-left:0px;padding:0;min-width:28px;max-width:28px;color:rgba(235,240,248,0.85);font-size:12px;}QPushButton:hover{color:#fff;}"));

    connect(boardsArrowBtn, &QPushButton::clicked, this, [boardsDrawer, boardsArrowBtn]() {
        const bool nextOpen = !boardsDrawer->isVisible();
        boardsDrawer->setVisible(nextOpen);
        boardsArrowBtn->setText(nextOpen ? QStringLiteral("▾") : QStringLiteral("▴"));
    });

    notesRowLayout->addWidget(notesNavButton_, 1);
    notesRowLayout->addWidget(boardsArrowBtn);

    navLayout->addWidget(boardsDrawer);
    navLayout->addWidget(notesRowWidget);
    navLayout->addWidget(settingsNavButton_);
    columns->addWidget(nav_);

    contentStack_ = new QStackedWidget(root_);
    consolePage_ = new QWidget(contentStack_);
    auto *consoleLayout = new QHBoxLayout(consolePage_);
    consoleLayout->setContentsMargins(0, 0, 0, 0);
    consoleLayout->setSpacing(10);
    sessionStack_ = new QStackedWidget(consolePage_);
    sessionStack_->setMinimumWidth(420);
    sessionStack_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    consoleLayout->addWidget(sessionStack_, 1);

    drawer_ = new QWidget(consolePage_);
    drawer_->setObjectName(QStringLiteral("drawerPanel"));
    drawer_->setMinimumWidth(0);
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
    auto *drawerTitle = new QLabel(QStringLiteral("Бинды"), drawerContent_);
    drawerTitle->setToolTip(QStringLiteral("Пользовательские бинды"));
    drawerTitle->setStyleSheet(QStringLiteral("font-size:15px;font-weight:600;"));
    auto *drawerHeader = new QHBoxLayout;
    drawerHeader->addWidget(drawerTitle, 1);
    drawerCollapseButton_ = button(QStringLiteral("Свернуть"), drawerContent_);
    drawerCollapseButton_->setObjectName(QStringLiteral("drawerCollapse"));
    drawerCollapseButton_->setToolTip(QStringLiteral("Свернуть панель биндов"));
    drawerHeader->addWidget(drawerCollapseButton_);
    actualDrawerLayout->addLayout(drawerHeader);
    connect(drawerCollapseButton_, &QPushButton::clicked, this, &MainWindow::toggleDrawer);
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
    auto *notesHeader = new QVBoxLayout; notesHeader->setSpacing(6); notesHeader->setContentsMargins(8, 10, 8, 10); notesHeader->setAlignment(Qt::AlignTop);
    auto *notesTitle = new QLabel(QStringLiteral("Заметки"), notesPage_);
    notesTitle->setStyleSheet(QStringLiteral("font-size:18px;font-weight:600;"));
    auto *addCard = button(QStringLiteral("＋  Карточка"), notesPage_);
    auto *drawBtn = button(QStringLiteral("Кисть"), notesPage_);
    auto *eraseBtn = button(QStringLiteral("Ластик"), notesPage_); eraseBtn->setCheckable(true); auto *clearDrawBtn = button(QStringLiteral("Очистить"), notesPage_);
    auto *linkBtn = button(QStringLiteral("Связь"), notesPage_); linkBtn->setCheckable(true);
    auto *selectBtn = button(QStringLiteral("Выделить"), notesPage_); selectBtn->setCheckable(true);
    selectBtn->setIcon(vectorActionIcon(QStringLiteral("select")));
    selectBtn->setIconSize(QSize(16, 16));
    selectBtn->setToolTip(QStringLiteral("Выделить карточки и штрихи рамкой, затем переместить их вместе"));
    auto *undoBtn = button(QStringLiteral("Отменить"), notesPage_); undoBtn->setToolTip(QStringLiteral("Отменить штрих (Ctrl+Z)"));
    drawBtn->setCheckable(true);
    
    noteScene_ = new QGraphicsScene(this); noteScene_->setItemIndexMethod(QGraphicsScene::NoIndex); auto *view = new NotesView(noteScene_, notesPage_); noteView_ = view;
    view->filesDroppedAt = [this](const QStringList &files, const QPointF &scenePos) {
        const QString mediaDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + QStringLiteral("/notes_media");
        QDir().mkpath(mediaDir); QPointF offset(0, 0);
        for (const QString &src : files) {
            const QFileInfo fi(src); const QString ext = fi.suffix().toLower();
            const QStringList ok = {QStringLiteral("png"),QStringLiteral("jpg"),QStringLiteral("jpeg"),QStringLiteral("webp"),QStringLiteral("bmp"),QStringLiteral("tiff"),QStringLiteral("tif"),QStringLiteral("ico"),QStringLiteral("svg"),QStringLiteral("gif"),QStringLiteral("mp4"),QStringLiteral("webm"),QStringLiteral("mkv"),QStringLiteral("avi"),QStringLiteral("mov"),QStringLiteral("m4v"),QStringLiteral("wmv"),QStringLiteral("mp3"),QStringLiteral("wav"),QStringLiteral("ogg"),QStringLiteral("flac"),QStringLiteral("m4a"),QStringLiteral("aac"),QStringLiteral("opus"),QStringLiteral("wma")};
            if (!ok.contains(ext)) continue;
            const QString dst = QDir(mediaDir).filePath(QStringLiteral("%1_%2").arg(QDateTime::currentMSecsSinceEpoch()).arg(fi.fileName()));
            if (QFile::copy(src, dst)) {
                if (auto *card = dynamic_cast<NoteCard *>(createNote(scenePos + offset, fi.completeBaseName(), QString()))) {
                    card->setMediaFile(dst); QImageReader r(dst); const QSize sz = r.size();
                    if (sz.isValid() && sz.width() > 0) {
                        const qreal w = qBound(240.0, qreal(sz.width()), 480.0);
                        card->setStoredSize(w, qBound(180.0, w * qreal(sz.height()) / qreal(sz.width()) + 66.0, 560.0));
                    } else if (card->videoWidth() > 0 && card->videoHeight() > 0) {
                        const qreal w = qBound(280.0, qreal(card->videoWidth()), 520.0);
                        card->setStoredSize(w, qBound(180.0, w * qreal(card->videoHeight()) / qreal(card->videoWidth()) + 66.0, 580.0));
                    } else if (QStringList{QStringLiteral("mp3"),QStringLiteral("wav"),QStringLiteral("ogg"),QStringLiteral("flac"),QStringLiteral("m4a"),QStringLiteral("aac"),QStringLiteral("opus"),QStringLiteral("wma")}.contains(ext)) {
                        card->setStoredSize(340.0, 126.0);
                    } else {
                        card->setStoredSize(340.0, 250.0);
                    }
                    saveNotes();
                }
                offset += QPointF(28, 28);
            }
        }
    };
    view->setZoomOutLimit(notesZoomOutPercent_);
    view->setFocusPolicy(Qt::StrongFocus); auto *brushBtn = button(QStringLiteral("Палитра"), notesPage_);
    auto *brushSize = new AppleStepper(3,1,16,notesPage_);
    connect(brushSize,&AppleStepper::valueChanged,view,[view](int value){view->brushWidth_=value;});
    auto *brushMenu = new QMenu(brushBtn);
    brushMenu->setStyleSheet(QStringLiteral("QMenu{background:#161b26;color:#f0f6fc;border:1px solid rgba(255,255,255,0.14);border-radius:10px;padding:6px;}QMenu::item{padding:6px 14px;border-radius:6px;}QMenu::item:selected{background:rgba(255,255,255,0.12);}"));
    auto addColorAct = [&](const QString &label, const QString &hex) {
        connect(brushMenu->addAction(label), &QAction::triggered, view, [view, drawBtn, eraseBtn, linkBtn, selectBtn, hex] {
            view->drawColor_ = hex; view->drawMode_ = true; view->eraseMode_ = false;
            drawBtn->setChecked(true); eraseBtn->setChecked(false);linkBtn->setChecked(false);selectBtn->setChecked(false); view->setCursor(Qt::CrossCursor);
        });
    };
    addColorAct(QStringLiteral("Сталь"), QStringLiteral("#7eb8da"));
    addColorAct(QStringLiteral("Голубой"), QStringLiteral("#39c5cf"));
    addColorAct(QStringLiteral("Зелёный"), QStringLiteral("#5ecf9a"));
    addColorAct(QStringLiteral("Янтарный"), QStringLiteral("#e0a45a"));
    addColorAct(QStringLiteral("Коралл"), QStringLiteral("#e06b63"));
    addColorAct(QStringLiteral("Белый"), QStringLiteral("#f8fafc"));
    brushMenu->addSeparator();
    connect(brushMenu->addAction(QStringLiteral("— Тонкая (2 px)")), &QAction::triggered, view, [view]{ view->brushWidth_ = 2.0; });
    connect(brushMenu->addAction(QStringLiteral("━ Средняя (4 px)")), &QAction::triggered, view, [view]{ view->brushWidth_ = 4.0; });
    connect(brushMenu->addAction(QStringLiteral("▬ Маркер (8 px)")), &QAction::triggered, view, [view]{ view->brushWidth_ = 8.0; });
    brushBtn->setMenu(brushMenu);

    auto *closeNotesBtn = button(QStringLiteral("Назад в терминал"), notesPage_);
    connect(closeNotesBtn, &QPushButton::clicked, this, [this, boardsDrawer] { setNotesVisible(false); });
    notesLayout->addWidget(notesTitle);
    auto *addMediaBtn = button(QStringLiteral("+ Медиа"), notesPage_); connect(addMediaBtn, &QPushButton::clicked, this, [this, view] { const QStringList f = QFileDialog::getOpenFileNames(this, QStringLiteral("Добавить медиа"), QString(), QStringLiteral("Медиа (*.png *.jpg *.jpeg *.webp *.bmp *.tiff *.tif *.ico *.svg *.gif *.mp4 *.webm *.mkv *.avi *.mov *.m4v *.wmv)")); if (!f.isEmpty() && view && view->filesDroppedAt) view->filesDroppedAt(f, view->mapToScene(view->viewport()->rect().center())); }); notesHeader->addWidget(addCard); notesHeader->addWidget(addMediaBtn);
    notesHeader->addWidget(drawBtn);
    notesHeader->addWidget(linkBtn);
    notesHeader->addWidget(selectBtn);
    notesHeader->addWidget(eraseBtn);
    notesHeader->addWidget(brushBtn);
    notesHeader->addWidget(brushSize);
    notesHeader->addWidget(undoBtn);
    notesHeader->addWidget(clearDrawBtn);
    notesHeader->addStretch(1);
    notesHeader->addWidget(closeNotesBtn);
    auto *toolPill = new QWidget(notesPage_);
    toolPill->setObjectName(QStringLiteral("freeformToolbar"));
    toolPill->setLayout(notesHeader);
    toolPill->setFixedWidth(160);
    toolPill->setStyleSheet(QStringLiteral("QWidget#freeformToolbar{background:rgba(32,36,46,0.88);border:1px solid rgba(255,255,255,0.16);border-top:1px solid rgba(255,255,255,0.35);border-radius:18px;}"));
    
    view->setSceneRect(-1000000, -1000000, 2000000, 2000000);
    view->setRenderHint(QPainter::Antialiasing);
    view->setFrameShape(QFrame::NoFrame);
    view->setDragMode(QGraphicsView::NoDrag);
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->addAt = [this](QPointF pos) { createNote(pos, QStringLiteral("Заметка"), QString()); };
    view->linkAt = [this](QGraphicsItem *item) { finishNoteLink(item); };
    view->startLinkAt = [](NoteCard *source) { if(source&&source->linkRequested)source->linkRequested(source); };
    view->hasLinkStart = [this] { return linkStart_ != nullptr; };
    view->isLinkStart = [this](NoteCard *card) { return linkStart_ == card; };
    view->leaveNotes = [this] { setNotesVisible(false); };
    view->cancelLink = [this] {
        linkStart_ = nullptr;
        if (previewPath_) { noteScene_->removeItem(previewPath_); delete previewPath_; previewPath_ = nullptr; }
        if (previewGlow_) { noteScene_->removeItem(previewGlow_); delete previewGlow_; previewGlow_ = nullptr; }
    };
    view->previewAt = [this](QPointF pos) {
        if (!previewPath_ || !linkStart_) return;
        const QRectF bounds=linkStart_->sceneBoundingRect();
        const QPointF center=bounds.center(),delta=pos-center;
        const qreal sx=qAbs(delta.x())<0.001?1e9:(bounds.width()/2.0)/qAbs(delta.x());
        const qreal sy=qAbs(delta.y())<0.001?1e9:(bounds.height()/2.0)/qAbs(delta.y());
        const QPointF from=center+delta*qMin(sx,sy);
        QPainterPath path(from);
        const qreal bend = (pos.x() - from.x()) * 0.35;
        path.cubicTo(from + QPointF(bend, 0), pos - QPointF(bend, 0), pos);
        previewPath_->setPath(path);
        if (previewGlow_) previewGlow_->setPath(path);
    };
    view->drawColor_ = accent_;
    connect(drawBtn,&QPushButton::toggled,view,[view,eraseBtn,linkBtn,selectBtn](bool on){view->resetInteractions();view->drawMode_=on;if(on){eraseBtn->setChecked(false);linkBtn->setChecked(false);selectBtn->setChecked(false);view->eraseMode_=false;view->linkMode_=false;view->selectMode_=false;}view->setCursor(on?Qt::CrossCursor:Qt::ArrowCursor);});
    connect(eraseBtn,&QPushButton::toggled,view,[view,drawBtn,linkBtn,selectBtn](bool on){view->resetInteractions();view->eraseMode_=on;if(on){drawBtn->setChecked(false);linkBtn->setChecked(false);selectBtn->setChecked(false);view->drawMode_=false;view->linkMode_=false;view->selectMode_=false;}view->setCursor(on?Qt::PointingHandCursor:Qt::ArrowCursor);});
    connect(linkBtn,&QPushButton::toggled,view,[view,drawBtn,eraseBtn,selectBtn](bool on){view->resetInteractions();view->linkMode_=on;if(on){drawBtn->setChecked(false);eraseBtn->setChecked(false);selectBtn->setChecked(false);view->drawMode_=false;view->eraseMode_=false;view->selectMode_=false;}view->setCursor(on?Qt::CrossCursor:Qt::ArrowCursor);});
    connect(selectBtn,&QPushButton::toggled,view,[this,view,drawBtn,linkBtn,eraseBtn](bool on){view->resetInteractions();view->selectMode_=on;if(on){drawBtn->setChecked(false);linkBtn->setChecked(false);eraseBtn->setChecked(false);view->drawMode_=view->linkMode_=view->eraseMode_=false;view->setDragMode(QGraphicsView::RubberBandDrag);view->setCursor(Qt::ArrowCursor);}for(QGraphicsItem *item:noteScene_->items())if(auto *card=dynamic_cast<NoteCard *>(item))card->setSelectionMode(on); noteScene_->update(); saveNotes();});
    connect(clearDrawBtn, &QPushButton::clicked, view, [view]{ view->clearStrokes(); });
    connect(undoBtn,&QPushButton::clicked,view,[view]{view->undoStroke();});
    auto *boardRow = new QHBoxLayout;
    boardRow->setContentsMargins(0, 0, 0, 0);
    boardRow->setSpacing(10);
    boardRow->addWidget(view, 1);
    boardRow->addWidget(toolPill, 0, Qt::AlignRight | Qt::AlignVCenter);
    notesLayout->addLayout(boardRow, 1);
    contentStack_->addWidget(notesPage_);
    buildSettingsPage();
    columns->addWidget(contentStack_, 1);
    starfield_->lower();

    connect(newConsole, &QPushButton::clicked, this, &MainWindow::createConsole);
    connect(notesNavButton_, &QPushButton::clicked, this, [this] { setNotesVisible(!notesVisible_); });
    connect(settingsNavButton_, &QPushButton::clicked, this, &MainWindow::openSettings);
    connect(newBind, &QPushButton::clicked, this, &MainWindow::createBind);
    connect(addCard, &QPushButton::clicked, this, &MainWindow::addNote);

    accent_ = themeAccent(theme_);
    setStyleSheet(appStyle(accent_, consoleAlpha_, uiScale_, theme_));
    setWindowOpacity(windowOpacity_ / 100.0);
    for (QPushButton *control : findChildren<QPushButton *>()) control->installEventFilter(this);
    root_->setAttribute(Qt::WA_TranslucentBackground, true);
    starfield_->setTheme(theme_);
    starfield_->setAnimated(starsEnabled_);
    starfield_->setStarsVisible(theme_ == QStringLiteral("Deep Space"));
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
    stopSftpSession(false);
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
    for (ConsoleSession *session : sessions_)
        if (session && !session->closing && session->page == page) return session;
    return nullptr;
}

ConsoleSession *MainWindow::findSession(quint64 id) const
{
    for (ConsoleSession *session : sessions_)
        if (session && session->id == id) return session;
    return nullptr;
}

void MainWindow::createConsole()
{
    auto *session = new ConsoleSession;
    session->id = nextSessionId_++;
    session->shell = defaultShell();
    session->page = new QWidget(sessionStack_);
    auto *pageLayout = new QVBoxLayout(session->page);
    pageLayout->setContentsMargins(6, 6, 6, 6);
    pageLayout->setSpacing(8);
    auto *card = new QFrame(session->page);
    card->setObjectName(QStringLiteral("consoleCard"));
    card->setAttribute(Qt::WA_Hover, true);
    card->setStyleSheet(QStringLiteral("QFrame#consoleCard{background:%1;border:1px solid rgba(255,255,255,0.08);border-radius:12px;}")
                            .arg(rgba(consoleBase(theme_), consoleAlpha_)));
    auto *cardLayout = new QVBoxLayout(card);
    cardLayout->setContentsMargins(12, 10, 12, 10);
    cardLayout->setSpacing(8);

    auto *toolbar = new QHBoxLayout;
    toolbar->setSpacing(6);
    session->title = new QLineEdit(QStringLiteral("Terminal %1").arg(sessions_.size() + 1), card);
    session->title->setMaximumWidth(220);
    session->status = new QLabel(QStringLiteral("Starting"), card);
    session->status->setObjectName(QStringLiteral("muted"));
    auto *spacer = new QWidget(card);
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    auto *copy = button(QStringLiteral("Копировать"), card);
    auto *paste = button(QStringLiteral("Вставить"), card);
    auto *clear = button(QStringLiteral("Очистить"), card);
    session->sshExplorerButton = button(QStringLiteral("Проводник SSH"), card);
    session->sshExplorerButton->setToolTip(QStringLiteral("Открыть файлы подключённого SSH-сервера"));
    session->sshExplorerButton->setVisible(false);
    auto *binds = button(QStringLiteral("Бинды"), card);
    binds->setObjectName(QStringLiteral("bindsToggle"));
    binds->setToolTip(QStringLiteral("Показать или скрыть панель пользовательских биндов"));
    copy->setToolTip(QStringLiteral("Копировать вывод (Ctrl+Shift+C)"));
    paste->setToolTip(QStringLiteral("Вставить из буфера (Ctrl+Shift+V)"));

    toolbar->addWidget(session->title);
    toolbar->addWidget(session->status);
    toolbar->addWidget(spacer);
    toolbar->addWidget(copy);
    toolbar->addWidget(paste);
    toolbar->addWidget(clear);
    toolbar->addWidget(session->sshExplorerButton);
    toolbar->addWidget(binds);
    binds->installEventFilter(this);
    connect(binds, &QPushButton::clicked, this, &MainWindow::toggleDrawer);
    connect(session->sshExplorerButton,&QPushButton::clicked,this,[this,session]{openSshExplorer(session);});
    cardLayout->addLayout(toolbar);

    auto *canvas = new TerminalCanvas(card);
    session->output = canvas;
    session->output->setFocusPolicy(Qt::StrongFocus);
    session->output->setStyleSheet(QStringLiteral(
        "QWidget{background:transparent;border:0;color:#d7dae0;"
        "font-family:'Cascadia Code','JetBrains Mono','Consolas','DejaVu Sans Mono',monospace;font-size:%1px;}")
        .arg(QString::number(fontSize_)));
    if (auto *terminal = dynamic_cast<TerminalCanvas *>(session->output))
        terminal->setTerminalCursor(cursorStyle_, QColor(cursorColor_));
    QFont terminalFont(QStringLiteral("Cascadia Code"), fontSize_);
    terminalFont.setFamilies({QStringLiteral("Cascadia Code"), QStringLiteral("JetBrains Mono"), QStringLiteral("Consolas"), QStringLiteral("DejaVu Sans Mono")});
    terminalFont.setStyleStrategy(QFont::PreferQuality);
    session->output->setFont(terminalFont);
    cardLayout->addWidget(session->output, 1);

    canvas->rawKeySent = [this,session](const QByteArray &bytes) {
        captureTerminalInput(session,bytes);
        if (session->backend) session->backend->writeInput(bytes);
    };
    canvas->terminalResized = [session](const QSize &size) {
        if (session->backend) session->backend->resizeTerminal(size);
    };
    pageLayout->addWidget(card, 1);

    session->tabRow = new QWidget(tabHost_);
    session->tabRow->setObjectName(QStringLiteral("tabRow"));
    session->tabRow->setAttribute(Qt::WA_Hover, true);
    session->tabRow->installEventFilter(this);
    auto *tabLayout = new QHBoxLayout(session->tabRow);
    tabLayout->setContentsMargins(5, 2, 5, 2);
    tabLayout->setSpacing(2);
    session->tabButton = button(session->title->text(), session->tabRow);
    session->tabButton->setObjectName(QStringLiteral("tabTitle"));
    session->tabButton->setCheckable(true);
    session->tabButton->setIcon(navigationDotIcon(false));
    session->tabButton->setIconSize(QSize(8, 8));
    auto *rename = button(QString(), session->tabRow);
    auto *close = button(QString(), session->tabRow);
    rename->setIcon(vectorActionIcon(QStringLiteral("edit")));
    close->setIcon(vectorActionIcon(QStringLiteral("close")));
    rename->setIconSize(QSize(15, 15));
    close->setIconSize(QSize(15, 15));
    rename->setObjectName(QStringLiteral("tabRename"));
    close->setObjectName(QStringLiteral("tabClose"));
    rename->setFixedSize(24,24); close->setFixedSize(24,24);
    rename->setVisible(false); close->setVisible(false);
    rename->setToolTip(QStringLiteral("Переименовать вкладку")); close->setToolTip(QStringLiteral("Закрыть вкладку"));
    session->tabButton->setMinimumHeight(36);
    session->tabButton->setStyleSheet(QStringLiteral("QPushButton{background:transparent;border:0;border-radius:8px;text-align:left;padding:4px 8px;}QPushButton:checked{background:rgba(255,255,255,0.12);border:0;}"));
    rename->setStyleSheet(QStringLiteral("QPushButton,QPushButton:hover{min-height:0;background:rgba(255,255,255,0.08);border:0;border-radius:7px;padding:0;}"));
    close->setStyleSheet(QStringLiteral("QPushButton,QPushButton:hover{min-height:0;background:rgba(255,255,255,0.08);border:0;border-radius:7px;padding:0;}QPushButton:hover{background:rgba(255,95,86,0.35);}"));
    session->tabRow->setStyleSheet(QStringLiteral("QWidget#tabRow{background:rgba(255,255,255,0.035);border:1px solid rgba(255,255,255,0.06);border-radius:10px;}QWidget#tabRow:hover{background:rgba(255,255,255,0.075);border-color:rgba(255,255,255,0.12);}"));
    tabLayout->addWidget(session->tabButton,1);
    tabLayout->addWidget(rename);
    tabLayout->addWidget(close);
    qobject_cast<QVBoxLayout *>(tabHost_->layout())->addWidget(session->tabRow);
    sessions_.append(session);
    sessionStack_->addWidget(session->page);

    const quint64 sessionId = session->id;
    auto resizeBackend = [this, sessionId] {
        ConsoleSession *session = findSession(sessionId);
        if (!session || !session->backend || !session->output) return;
        if (auto *terminal = dynamic_cast<TerminalCanvas *>(session->output))
            session->backend->resizeTerminal(terminal->terminalSize());
    };

    connect(session->tabButton, &QPushButton::clicked, this, [this, session, resizeBackend] {
        activateSession(session);
        QTimer::singleShot(120, this, resizeBackend);
    });
    connect(rename, &QPushButton::clicked, this, [this, session] { renameSession(session); });
    connect(close, &QPushButton::clicked, this, [this, session] { closeSession(session); });
    connect(session->title, &QLineEdit::textChanged, this, [session](const QString &text) { session->tabButton->setText(text); });
    connect(copy, &QPushButton::clicked, this, [session] {
        if (auto *terminal = dynamic_cast<TerminalCanvas *>(session->output)) {
            const QString selection = terminal->selectedText();
            QApplication::clipboard()->setText(selection.isEmpty() ? terminal->toPlainText() : selection);
            terminal->clearSelection();
        }
    });
    connect(paste, &QPushButton::clicked, this, [session] {
        if (auto *terminal = dynamic_cast<TerminalCanvas *>(session->output))
            terminal->pasteText(QApplication::clipboard()->text());
        else if (session->backend && session->backend->isRunning())
            session->backend->writeInput(QApplication::clipboard()->text().toUtf8());
        if (session->output) session->output->setFocus();
    });
    connect(clear, &QPushButton::clicked, this, [session] {
        if (auto *canvas = dynamic_cast<TerminalCanvas *>(session->output)) canvas->clearTerminal();
        else if (auto *terminal=dynamic_cast<TerminalCanvas *>(session->output)) terminal->clearTerminal();
        if (session->output) session->output->setFocus();
    });

    activateSession(session);
    startProcess(session, session->shell);
    QTimer::singleShot(120, this, resizeBackend);
}

void MainWindow::activateSession(ConsoleSession *session)
{
    if (!session || session->closing || !sessions_.contains(session)) return;
    if (sshExplorerOverlay_ && sshExplorerOverlay_->isVisible() && sshExplorerSession_ != session)
        closeSshExplorer();
    if (notesVisible_) setNotesVisible(false);
    sessionStack_->setCurrentWidget(session->page);
    for (ConsoleSession *item : sessions_) item->tabButton->setChecked(item == session);
    updateNavigationIndicators();
    if (session->output) {
        session->output->setFocus(Qt::OtherFocusReason);
        QTimer::singleShot(0, session->output, [output = session->output] {
            if (output && output->isVisible()) output->setFocus(Qt::OtherFocusReason);
        });
    }
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
    if (!session || session->closing) return;
    const int index = sessions_.indexOf(session);
    if (index < 0 || index >= sessions_.size()) return;
    session->closing = true;
    if (session->tabRow) {
        session->tabRow->setProperty("closing", true);
        session->tabRow->removeEventFilter(this);
        session->tabRow->setProperty("closing", true); session->tabRow->removeEventFilter(this); session->tabRow->setEnabled(false);
    }
    if (session->tabButton) session->tabButton->setEnabled(false);
    const quint64 tabId = session->id;
    QTimer::singleShot(0, this, [this, tabId] {
        ConsoleSession *target = findSession(tabId);
        if (!target) return;
        const int tabIndex = sessions_.indexOf(target);
        if (tabIndex < 0 || tabIndex >= sessions_.size()) return;
        const bool wasActive = sessionStack_ && sessionStack_->currentWidget() == target->page;
        if (sshExplorerSession_ == target) closeSshExplorer();
        stopProcess(target);
        sessions_.removeAt(tabIndex);
        if (sessionStack_ && target->page) sessionStack_->removeWidget(target->page);
        if (target->tabRow) target->tabRow->deleteLater();
        if (target->page) {
            connect(target->page, &QObject::destroyed, this, [target] { delete target; });
            target->page->deleteLater();
        } else {
            QTimer::singleShot(0, this, [target] { delete target; });
        }
        if (sessions_.isEmpty()) {
            createConsole();
        } else if (wasActive) {
            const int nextIndex = qBound(0, tabIndex, sessions_.size() - 1);
            if (nextIndex >= 0 && nextIndex < sessions_.size()) activateSession(sessions_.at(nextIndex));
        }
        updateNavigationIndicators();
    });
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
    session->sshActive=false;
    session->sshCandidate=false;
    session->sshTarget.clear();
    session->sshSftpOptions.clear();
    session->sshOutputTail.clear();
    session->sshInputLine.clear();
    session->sshInputCursor=0;
    updateSessionUi(session);
    session->backend = new TerminalBackend(this);
    if (auto *canvas = dynamic_cast<TerminalCanvas *>(session->output)) canvas->terminalBackend = session->backend;
    const quint64 sessionId = session->id;
    connect(session->backend, &TerminalBackend::outputReady, this, [this, sessionId](const QByteArray &bytes) {
        ConsoleSession *session = findSession(sessionId);
        if (!session || session->closing) return;
        inspectTerminalOutput(session,bytes);
        if (auto *canvas = dynamic_cast<TerminalCanvas *>(session->output)) canvas->appendSystemBytes(bytes);
        else appendOutput(session, QString::fromUtf8(bytes));
    });
    connect(session->backend, &TerminalBackend::errorOccurred, this, [this, sessionId](const QString &error) {
        ConsoleSession *session = findSession(sessionId);
        if (!session || session->closing) return;
        appendOutput(session, QStringLiteral("\r\n[terminal] %1\r\n").arg(error));
        if (session->status) session->status->setText(QStringLiteral("Error"));
    });
    connect(session->backend, &TerminalBackend::processFinished, this, [this, sessionId](int code) {
        ConsoleSession *session = findSession(sessionId);
        if (!session || session->closing) return;
        if (session->status) session->status->setText(QStringLiteral("Exited (%1)").arg(code));
    });
    session->status->setText(QStringLiteral("Starting"));
    const QSize initialSize = session->output
        ? (dynamic_cast<TerminalCanvas *>(session->output) ? dynamic_cast<TerminalCanvas *>(session->output)->terminalSize() : QSize(80,24))
        : QSize(80,24);
    if (!session->backend->start(shell, initialSize, false)) {
        session->status->setText(QStringLiteral("Не запущен"));
        appendOutput(session, QStringLiteral("\r\n[terminal] Не удалось запустить оболочку. Проверьте её наличие и путь.\r\n"));
    } else if (session->backend->usingFallback()) {
        session->status->setText(QStringLiteral("Fallback"));
    } else {
#ifdef Q_OS_WIN
        session->status->setText(QStringLiteral("ConPTY"));
#else
        session->status->setText(QStringLiteral("PTY"));
#endif
    }
    updateSessionUi(session);
}

void MainWindow::stopProcess(ConsoleSession *session)
{
    if (!session) return;
    if (session->backend) {
        TerminalBackend *backend = session->backend;
        session->backend = nullptr;
        if (auto *canvas = dynamic_cast<TerminalCanvas *>(session->output))
            if (canvas->terminalBackend == backend) canvas->terminalBackend = nullptr;
        disconnect(backend, nullptr, this, nullptr);
        backend->stop();
        backend->deleteLater();
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
    if(session->sshExplorerButton)session->sshExplorerButton->setVisible(session->sshActive);
    if(!session->sshActive&&sshExplorerSession_==session&&sshExplorerOverlay_)
        closeSshExplorer();
}

void MainWindow::captureTerminalInput(ConsoleSession *session,const QByteArray &bytes)
{
    if(!session)return;
    auto insertText=[session](const QString &text){session->sshInputLine.insert(session->sshInputCursor,text);session->sshInputCursor+=text.size();};
    auto submitLine=[this,session]{
        const QString line=session->sshInputLine.trimmed();
        QString target;QStringList options;
        if(parseSshInvocation(line,&target,&options)){
            session->sshTarget=target;
            session->sshSftpOptions=options;
            session->sshCandidate=true;
            session->sshActive=false;
            session->remoteDirectory=QStringLiteral(".");
            session->sshOutputTail.clear();
            updateSessionUi(session);
        }else if(line.compare(QStringLiteral("exit"),Qt::CaseInsensitive)==0&&session->sshActive){
            session->sshActive=false;session->sshCandidate=false;session->sshTarget.clear();session->sshSftpOptions.clear();updateSessionUi(session);
        }
        session->sshInputLine.clear();session->sshInputCursor=0;
    };
    for(qsizetype i=0;i<bytes.size();){
        if(bytes.mid(i,6)==QByteArray("\x1b[200~",6)){
            const qsizetype end=bytes.indexOf(QByteArray("\x1b[201~",6),i+6);
            const QByteArray text=end<0?bytes.mid(i+6):bytes.mid(i+6,end-i-6);
            insertText(QString::fromUtf8(text));
            i=end<0?bytes.size():end+6;
            continue;
        }
        const uchar ch=static_cast<uchar>(bytes.at(i));
        if(ch==0x1b){
            qsizetype end=i+1;
            if(end<bytes.size()&&bytes.at(end)=='['){++end;while(end<bytes.size()&&!(static_cast<uchar>(bytes.at(end))>=0x40&&static_cast<uchar>(bytes.at(end))<=0x7e))++end;if(end<bytes.size()){const char final=bytes.at(end);if(final=='D')session->sshInputCursor=qMax(0,session->sshInputCursor-1);else if(final=='C')session->sshInputCursor=qMin(session->sshInputLine.size(),session->sshInputCursor+1);else if(final=='A'||final=='B'){session->sshInputLine.clear();session->sshInputCursor=0;}++end;}}
            else if(end<bytes.size())++end;
            i=end;continue;
        }
        if(ch=='\r'||ch=='\n'){submitLine();++i;continue;}
        if(ch==0x08||ch==0x7f){if(session->sshInputCursor>0){session->sshInputLine.remove(session->sshInputCursor-1,1);--session->sshInputCursor;}++i;continue;}
        if(ch<0x20){
            if(ch==0x01)session->sshInputCursor=0;
            else if(ch==0x05)session->sshInputCursor=session->sshInputLine.size();
            else if(ch==0x15){session->sshInputLine.clear();session->sshInputCursor=0;}
            else if(ch==0x17){while(session->sshInputCursor>0&&session->sshInputLine.at(session->sshInputCursor-1).isSpace()){session->sshInputLine.remove(session->sshInputCursor-1,1);--session->sshInputCursor;}while(session->sshInputCursor>0&&!session->sshInputLine.at(session->sshInputCursor-1).isSpace()){session->sshInputLine.remove(session->sshInputCursor-1,1);--session->sshInputCursor;}}
            else if(ch==0x03&&!session->sshActive){session->sshCandidate=false;session->sshTarget.clear();session->sshSftpOptions.clear();}
            ++i;continue;
        }
        qsizetype end=i+1;
        while(end<bytes.size()&&static_cast<uchar>(bytes.at(end))>=0x20&&static_cast<uchar>(bytes.at(end))!=0x7f)++end;
        insertText(QString::fromUtf8(bytes.constData()+i,end-i));
        i=end;
    }
}

void MainWindow::inspectTerminalOutput(ConsoleSession *session,const QByteArray &bytes)
{
    if(!session)return;
    session->sshOutputTail+=bytes;
    if(session->sshOutputTail.size()>8192)session->sshOutputTail=session->sshOutputTail.right(8192);
    QString output=QString::fromUtf8(session->sshOutputTail);
    output.remove(QRegularExpression(QStringLiteral("\\x1B\\[[0-?]*[ -/]*[@-~]")));
    output.remove(QRegularExpression(QStringLiteral("\\x1B\\][^\\x07]*(?:\\x07|\\x1B\\\\)")));
    output.replace(QStringLiteral("\r\n"),QStringLiteral("\n"));
    output.replace(QLatin1Char('\r'), QLatin1Char('\n'));
    const QString lower=output.toLower();
    const bool failed=lower.contains(QStringLiteral("permission denied"))
        ||lower.contains(QStringLiteral("could not resolve hostname"))
        ||lower.contains(QStringLiteral("connection refused"))
        ||lower.contains(QStringLiteral("connection timed out"))
        ||lower.contains(QStringLiteral("no route to host"))
        ||lower.contains(QStringLiteral("host key verification failed"));
    if(session->sshCandidate&&failed){session->sshCandidate=false;session->sshTarget.clear();session->sshSftpOptions.clear();updateSessionUi(session);}
    if(session->sshCandidate){
        static const QRegularExpression remotePrompt(QStringLiteral("(?:^|\\n)(?:[\\w.-]+@[\\w.:-]+(?::[^\\r\\n]*)?|PS [A-Za-z]:\\\\[^\\r\\n]*|[A-Za-z]:\\\\[^\\r\\n]*)[$#%>]\\s*$"));
        if(remotePrompt.match(output).hasMatch()||lower.contains(QStringLiteral("last login:"))||lower.contains(QStringLiteral("welcome to"))){session->sshCandidate=false;session->sshActive=true;session->remoteDirectory=QStringLiteral(".");updateSessionUi(session);}
    }
    if (session->sshActive && (lower.contains(QStringLiteral("connection closed"))
        || (lower.contains(QStringLiteral("connection to ")) && lower.contains(QStringLiteral(" closed"))))) {
        session->sshActive=false;session->sshCandidate=false;session->sshTarget.clear();session->sshSftpOptions.clear();updateSessionUi(session);
    }
}

void MainWindow::startSftpSession(ConsoleSession *session)
{
    if (!session || !session->sshActive || session->closing) return;
    stopSftpSession(true);
    const QString executable = QStandardPaths::findExecutable(QStringLiteral("sftp"));
    if (executable.isEmpty()) {
        if (sshExplorerStatus_) sshExplorerStatus_->setText(QStringLiteral("Не найден OpenSSH sftp в PATH"));
        return;
    }

    QStringList arguments;
    for (int i = 0; i < session->sshSftpOptions.size(); ++i) {
        const QString option = session->sshSftpOptions.at(i);
        if (option == QStringLiteral("-P")) { ++i; continue; }
        arguments.append(option);
        if ((option == QStringLiteral("-i") || option == QStringLiteral("-F")
             || option == QStringLiteral("-J") || option == QStringLiteral("-o"))
            && i + 1 < session->sshSftpOptions.size())
            arguments.append(session->sshSftpOptions.at(++i));
    }
    arguments << QStringLiteral("-o") << QStringLiteral("StrictHostKeyChecking=accept-new")
              << QStringLiteral("-o") << QStringLiteral("ConnectTimeout=15");
    if (!session->sshPassword.isEmpty()) {
        arguments << QStringLiteral("-o") << QStringLiteral("PubkeyAuthentication=no")
                  << QStringLiteral("-o") << QStringLiteral("PasswordAuthentication=yes")
                  << QStringLiteral("-o") << QStringLiteral("PreferredAuthentications=password,keyboard-interactive")
                  << QStringLiteral("-o") << QStringLiteral("NumberOfPasswordPrompts=1");
    }
    arguments << QStringLiteral("-P") << QString::number(session->sshPort) << session->sshTarget;

    sftpReady_ = false;
    sftpConnected_ = false;
    sftpPasswordSent_ = false;
    sftpPendingAction_ = SftpPendingAction::AwaitPrompt;
    sftpBuffer_.clear();
    sftpListingFallbackTried_ = false;
    sftpResponseError_.clear();
    sftpTransferCommands_.clear();
    sftpTransferSuccessStatus_.clear();
    auto *backend = new TerminalBackend(this);
    sftpBackend_ = backend;
    sftpOwnerSession_ = session;
    connect(backend, &TerminalBackend::outputReady, this, [this, backend](const QByteArray &bytes) {
        handleSftpOutput(backend, bytes);
    });
    connect(backend, &TerminalBackend::errorOccurred, this, [this, backend](const QString &error) {
        if (backend == sftpBackend_ && sshExplorerStatus_)
            sshExplorerStatus_->setText(error);
    });
    connect(backend, &TerminalBackend::processFinished, this, [this, backend](int code) {
        if (backend != sftpBackend_) return;
        sftpReady_ = false;
        sftpConnected_ = false;
        sftpBackend_ = nullptr;
        sftpOwnerSession_ = nullptr;
        if (sshExplorerStatus_)
            sshExplorerStatus_->setText(code == 0 ? QStringLiteral("SFTP-сессия завершена")
                                                   : (sftpResponseError_.isEmpty() ? QStringLiteral("Соединение SFTP закрыто") : sftpResponseError_));
        backend->deleteLater();
    });
    if (sshExplorerStatus_) sshExplorerStatus_->setText(QStringLiteral("Подключение по SFTP…"));
    const QSize sftpTerminalSize(240, 80);
    if (!backend->startCommand(executable, arguments, sftpTerminalSize)) {
        if (backend == sftpBackend_) {
            sftpBackend_ = nullptr;
            sftpOwnerSession_ = nullptr;
            sftpReady_ = false;
            sftpConnected_ = false;
            if (sshExplorerStatus_) sshExplorerStatus_->setText(QStringLiteral("Не удалось запустить интерактивный SFTP"));
            backend->deleteLater();
        }
        return;
    }
    backend->resizeTerminal(sftpTerminalSize);
}

void MainWindow::stopSftpSession(bool sendBye)
{
    TerminalBackend *backend = sftpBackend_;
    if (!backend) return;
    const bool canSendBye = sftpReady_;
    sftpBackend_ = nullptr;
    sftpOwnerSession_ = nullptr;
    sftpReady_ = false;
    sftpConnected_ = false;
    sftpPasswordSent_ = false;
    sftpBuffer_.clear();
    sftpResponseError_.clear();
    sftpTransferCommands_.clear();
    sftpTransferSuccessStatus_.clear();
    disconnect(backend, nullptr, this, nullptr);
    if (sendBye && canSendBye && backend->isRunning()) backend->writeData(QByteArrayLiteral("bye\r\n"));
    QTimer::singleShot(sendBye ? 100 : 0, this, [backend] {
        backend->stop();
        backend->deleteLater();
    });
}

void MainWindow::closeSshExplorer()
{
    if (sshExplorerOverlay_ && sshExplorerOverlay_->isVisible())
        animateOverlay(sshExplorerOverlay_, false);
    stopSftpSession(true);
    sshExplorerSession_ = nullptr;
}

void MainWindow::sendSftpCommand(const QString &command, SftpPendingAction action)
{
    if (!sftpBackend_ || !sftpBackend_->isRunning()) return;
    sftpPendingAction_ = action;
    sftpBuffer_.clear();
    sftpResponseError_.clear();
    sftpBackend_->writeData(command.toUtf8() + QByteArrayLiteral("\r\n"));
}

void MainWindow::startSftpTransfer(const QStringList &commands, const QString &successStatus)
{
    if (!sftpBackend_ || !sftpConnected_) {
        if (sshExplorerStatus_) sshExplorerStatus_->setText(QStringLiteral("Сначала подключитесь к SFTP"));
        return;
    }
    if (commands.isEmpty()) return;
    sftpTransferCommands_ = commands;
    sftpTransferSuccessStatus_ = successStatus;
    if (sshExplorerStatus_) sshExplorerStatus_->setText(QStringLiteral("Передача файлов…"));
    const QString command = sftpTransferCommands_.takeFirst();
    sendSftpCommand(command, SftpPendingAction::Transfer);
}

void MainWindow::handleSftpOutput(TerminalBackend *backend, const QByteArray &bytes)
{
    if (!backend || backend != sftpBackend_) return;
    // Keep the raw bytes for the full command response. ConPTY can split ANSI
    // sequences and file rows across signals; sanitize and parse only at prompt.
    sftpBuffer_.append(bytes);
    constexpr qsizetype maxBufferedOutput = 4 * 1024 * 1024;
    if (sftpBuffer_.size() > maxBufferedOutput)
        sftpBuffer_.remove(0, sftpBuffer_.size() - maxBufferedOutput);
    const QString responseText = stripTerminalEscapes(sftpBuffer_);

    const QString lower = responseText.toLower();
    static const QRegularExpression passwordPrompt(QStringLiteral("password:\\s*$"), QRegularExpression::CaseInsensitiveOption);
    if (passwordPrompt.match(responseText).hasMatch() && !sftpPasswordSent_) {
        if (sshExplorerSession_ && !sshExplorerSession_->sshPassword.isEmpty()) {
            sftpPasswordSent_ = true;
            backend->writeData(sshExplorerSession_->sshPassword.toUtf8() + QByteArrayLiteral("\r\n"));
            sftpBuffer_.clear();
            if (sshExplorerStatus_) sshExplorerStatus_->setText(QStringLiteral("Проверка пароля…"));
            return;
        }
        if (sshExplorerStatus_) sshExplorerStatus_->setText(QStringLiteral("Сервер запросил пароль. Введите его и подключитесь снова."));
        return;
    }

    static const QRegularExpression failure(QStringLiteral("(?:permission denied|host key verification failed|connection timed out|connection refused|no such file|couldn't|failure|connection closed|could not resolve)"), QRegularExpression::CaseInsensitiveOption);
    const auto failureMatch = failure.match(responseText);
    if (failureMatch.hasMatch()) {
        const QString line = responseText.mid(responseText.lastIndexOf(QLatin1Char('\n'), failureMatch.capturedStart()) + 1).trimmed();
        QString errText = line.isEmpty() ? failureMatch.captured() : line; sftpResponseError_ = errText.replace(QStringLiteral("sftp>"), QString()).trimmed();
        if (sshExplorerStatus_) sshExplorerStatus_->setText(sftpResponseError_);
    }

    static const QRegularExpression prompt(QStringLiteral("(?:^|\\n)\\s*sftp>\\s*$"), QRegularExpression::CaseInsensitiveOption);
    if (prompt.match(responseText).hasMatch()) {
        sftpBuffer_.clear();
        handleSftpPrompt(responseText);
    }
}

void MainWindow::handleSftpPrompt(const QString &response)
{
    if (!sftpBackend_) return;
    switch (sftpPendingAction_) {
    case SftpPendingAction::AwaitPrompt:
        sftpReady_ = true;
        sftpConnected_ = true;
        if (sshExplorerCard_) {
            if (auto *p = sshExplorerCard_->findChild<QWidget *>(QStringLiteral("sftpAuthPanel"))) p->setVisible(false);
            if (auto *b = sshExplorerCard_->findChild<QWidget *>(QStringLiteral("sftpExplorerBody"))) b->setVisible(true);
            if (auto *s = sshExplorerCard_->findChild<QPushButton *>(QStringLiteral("sftpSwitchAccount"))) s->setVisible(true);
        }
        if (sshExplorerStatus_) sshExplorerStatus_->setText(QStringLiteral("Авторизовано · получаю каталог…"));
        sendSftpCommand(QStringLiteral("pwd"), SftpPendingAction::Pwd);
        return;
    case SftpPendingAction::Pwd: {
        static const QRegularExpression pwdPattern(QStringLiteral("(?:^|\\n)[ \\t]*Remote working directory:[ \\t]*([^\\r\\n]+)"), QRegularExpression::CaseInsensitiveOption);
        const auto match = pwdPattern.match(response);
        if (match.hasMatch()) {
            const QString homePath = match.captured(1).trimmed();
            if (sshExplorerSession_ && !homePath.isEmpty())
                sshExplorerSession_->remoteDirectory = QDir::cleanPath(homePath);
            if (!homePath.isEmpty()) sftpConnected_ = true;
            if (sshExplorerPath_ && sshExplorerSession_) sshExplorerPath_->setText(sshExplorerSession_->remoteDirectory);
        }
        sftpListingFallbackTried_ = false;
        sendSftpCommand(QStringLiteral("ls -la"), SftpPendingAction::List);
        return;
    }
    case SftpPendingAction::ChangeDirectory:
        if (!sftpResponseError_.isEmpty()) {
            sftpTransferSuccessStatus_ = sftpResponseError_;
        }
        sendSftpCommand(QStringLiteral("pwd"), SftpPendingAction::Pwd);
        return;
    case SftpPendingAction::List:
        if (!sftpResponseError_.isEmpty()) {
            if (sshExplorerStatus_) sshExplorerStatus_->setText(sftpResponseError_);
            return;
        }
        sftpConnected_ = true;
        if (populateSftpListing(response, sftpListingFallbackTried_) == 0 && !sftpListingFallbackTried_) {
            sftpListingFallbackTried_ = true;
            sendSftpCommand(QStringLiteral("ls -1"), SftpPendingAction::List);
            return;
        }
        if (!sftpTransferSuccessStatus_.isEmpty()) {
            if (sshExplorerStatus_) sshExplorerStatus_->setText(sftpTransferSuccessStatus_);
            sftpTransferSuccessStatus_.clear();
        }
        return;
    case SftpPendingAction::Transfer:
        if (!sftpResponseError_.isEmpty()) {
            sftpTransferSuccessStatus_ = sftpResponseError_;
            sftpTransferCommands_.clear();
            sendSftpCommand(QStringLiteral("pwd"), SftpPendingAction::Pwd);
            return;
        }
        if (!sftpTransferCommands_.isEmpty()) {
            const QString command = sftpTransferCommands_.takeFirst();
            sendSftpCommand(command, SftpPendingAction::Transfer);
            return;
        }
        sendSftpCommand(QStringLiteral("pwd"), SftpPendingAction::Pwd);
        return;
    }
}

int MainWindow::populateSftpListing(const QString &output, bool namesOnly)
{
    if (!sshExplorerTree_ || !sshExplorerSession_) return 0;
    sshExplorerTree_->clear();
    const QString directory = sshExplorerSession_->remoteDirectory;
    static const QRegularExpression permissionsPattern(QStringLiteral("^[dlcbps-][rwxStTs-]{9}"));
    static const QRegularExpression separator(QStringLiteral("\\s+"));
    QFileIconProvider icons;
    int count = 0;
    for (const QString &rawLine : output.split(QLatin1Char('\n'))) {
        const QString line = rawLine.trimmed();
        if (line.isEmpty() || line.startsWith(QStringLiteral("sftp>"), Qt::CaseInsensitive)) continue;

        QString permissions;
        QString size;
        QString name;
        if (namesOnly) {
            name = line;
        } else {
            QStringList parts;
            qsizetype position = 0;
            bool complete = true;
            // Capture eight separators so the ninth field retains the full
            // filename, including embedded spaces.
            for (int field = 0; field < 8; ++field) {
                const auto match = separator.match(line, position);
                if (!match.hasMatch()) { complete = false; break; }
                parts.append(line.mid(position, match.capturedStart() - position));
                position = match.capturedEnd();
            }
            if (!complete) continue;
            parts.append(line.mid(position).trimmed());
            if (parts.size() != 9 || !permissionsPattern.match(parts.at(0)).hasMatch()) continue;
            permissions = parts.at(0);
            size = parts.at(4);
            name = parts.at(8);
        }
        if (name == QStringLiteral(".") || name == QStringLiteral("..")) continue;
        if (name.isEmpty() || name == QStringLiteral("ls -1") || name == QStringLiteral("ls -la") || name.startsWith(QStringLiteral("sftp>"), Qt::CaseInsensitive)) continue;
        if (permissions.startsWith(QLatin1Char('l')) && name.contains(QStringLiteral(" -> ")))
            name = name.left(name.indexOf(QStringLiteral(" -> ")));
        const bool isDirectory = permissions.startsWith(QLatin1Char('d')) || (namesOnly && (name.endsWith(QLatin1Char('/')) || !name.contains(QLatin1Char('.'))));
        if (namesOnly && name.endsWith(QLatin1Char('/'))) name.chop(1);
        const QString full = QDir::cleanPath(QDir(directory).filePath(name));
        auto *item = new QTreeWidgetItem(sshExplorerTree_);
        item->setText(0, name);
        item->setText(1, size);
        item->setText(2, permissions);
        item->setIcon(0, icons.icon(isDirectory ? QFileIconProvider::Folder : QFileIconProvider::File));
        item->setData(0, Qt::UserRole, full);
        item->setData(0, Qt::UserRole + 1, isDirectory);
        ++count;
    }
    if (sshExplorerStatus_)
        sshExplorerStatus_->setText(QStringLiteral("%1 · %2 элементов").arg(sshExplorerSession_->sshTarget).arg(count));
    return count;
}

void MainWindow::openSshExplorer(ConsoleSession *session)
{
    if (!session || !session->sshActive) return;
    if (sftpBackend_ && sftpOwnerSession_ != session) stopSftpSession(true);

    if (!sshExplorerOverlay_) {
        sshExplorerOverlay_ = new QWidget(root_);
        sshExplorerOverlay_->setObjectName(QStringLiteral("sshExplorerOverlay"));
        sshExplorerOverlay_->setStyleSheet(QStringLiteral("QWidget#sshExplorerOverlay{background:rgba(8,12,18,0.58);}"));
        sshExplorerOverlay_->installEventFilter(this);

        sshExplorerCard_ = new QFrame(sshExplorerOverlay_);
        sshExplorerCard_->setObjectName(QStringLiteral("sshExplorerCard"));
        sshExplorerCard_->setMinimumSize(640, 460);
        sshExplorerCard_->resize(860, 560);

        auto *rootLayout = new QVBoxLayout(sshExplorerCard_);
        rootLayout->setContentsMargins(0, 6, 0, 6); rootLayout->setSpacing(0);
        rootLayout->setSpacing(10);

        class DraggableTitle : public QLabel {
        public:
            DraggableTitle(const QString &t, QWidget *c) : QLabel(t, c), card_(c) { setCursor(Qt::OpenHandCursor); }
        protected:
            void mousePressEvent(QMouseEvent *e) override {
                if (e->button() == Qt::LeftButton && card_) { dragging_ = true; off_ = e->globalPosition().toPoint() - card_->pos(); setCursor(Qt::ClosedHandCursor); }
            }
            void mouseMoveEvent(QMouseEvent *e) override {
                if (dragging_ && card_ && card_->parentWidget()) {
                    QPoint p = e->globalPosition().toPoint() - off_;
                    p.setX(qBound(0, p.x(), qMax(0, card_->parentWidget()->width() - card_->width())));
                    p.setY(qBound(0, p.y(), qMax(0, card_->parentWidget()->height() - card_->height())));
                    card_->move(p);
                }
            }
            void mouseReleaseEvent(QMouseEvent *) override { dragging_ = false; setCursor(Qt::OpenHandCursor); }
        private:
            QWidget *card_ = nullptr; bool dragging_ = false; QPoint off_;
        };

                auto *header = new QHBoxLayout;
        header->setContentsMargins(16, 6, 8, 6);
        header->setSpacing(8);
        auto *title = new DraggableTitle(QStringLiteral("Проводник SSH"), sshExplorerCard_);
        title->setStyleSheet(QStringLiteral("font-size:14px;font-weight:600;color:#f3f5f8;border:0;background:transparent;"));
        auto *targetLabel = new QLabel(sshExplorerCard_);
        targetLabel->setObjectName(QStringLiteral("sshExplorerTarget"));
        targetLabel->setStyleSheet(QStringLiteral("color:rgba(235,240,248,0.55);font-size:12px;border:0;background:transparent;"));
        header->addWidget(title);
        targetLabel->hide();
        header->addStretch(1);

        auto *minBtn = button(QStringLiteral("−"), sshExplorerCard_);
        minBtn->setFixedSize(28, 28); minBtn->setToolTip(QStringLiteral("Свернуть"));
        auto *maxBtn = button(QStringLiteral("□"), sshExplorerCard_);
        maxBtn->setFixedSize(28, 28); maxBtn->setToolTip(QStringLiteral("Развернуть / Восстановить"));
        auto *closeBtn = button(QStringLiteral("×"), sshExplorerCard_);
        closeBtn->setFixedSize(28, 28); closeBtn->setToolTip(QStringLiteral("Закрыть"));
        const QString winBtnCss = QStringLiteral("QPushButton{min-height:28px;max-width:28px;min-width:28px;background:transparent;border:0;border-radius:6px;color:#e6ebf2;font-size:14px;padding:0;}QPushButton:hover{background:rgba(255,255,255,0.10);}");
        minBtn->setStyleSheet(winBtnCss); maxBtn->setStyleSheet(winBtnCss);
        closeBtn->setStyleSheet(QStringLiteral("QPushButton{min-height:28px;max-width:28px;min-width:28px;background:transparent;border:0;border-radius:6px;color:#e6ebf2;font-size:15px;padding:0;}QPushButton:hover{background:#e81123;color:#fff;}"));
        header->addWidget(minBtn); header->addWidget(maxBtn); header->addWidget(closeBtn);
        rootLayout->addLayout(header);


                auto *authContainer = new QWidget(sshExplorerCard_);
        authContainer->setObjectName(QStringLiteral("sftpAuthPanel"));
        auto *authOuter = new QVBoxLayout(authContainer);
        authOuter->addStretch(1);
        auto *authCard = new QFrame(authContainer);
        authCard->setMinimumWidth(420);
        authCard->setMaximumWidth(460);
        authCard->setStyleSheet(QStringLiteral("QFrame{background:rgba(255,255,255,0.05);border:1px solid rgba(255,255,255,0.12);border-radius:16px;}"));
        auto *authLayout = new QVBoxLayout(authCard);
        authLayout->setContentsMargins(22, 20, 22, 20);
        authLayout->setSpacing(10);

        auto *authHead = new QLabel(QStringLiteral("Подключение к серверу SFTP"), authCard);
        authHead->setStyleSheet(QStringLiteral("font-size:15px;font-weight:600;border:0;background:transparent;"));
        authLayout->addWidget(authHead);

        auto *userHostRow = new QHBoxLayout; userHostRow->setSpacing(8);
        sshExplorerLogin_ = new QLineEdit(authCard); sshExplorerLogin_->setPlaceholderText(QStringLiteral("Логин"));
        sshExplorerHost_ = new QLineEdit(authCard); sshExplorerHost_->setPlaceholderText(QStringLiteral("IP или хост"));
        userHostRow->addWidget(sshExplorerLogin_, 1); userHostRow->addWidget(sshExplorerHost_, 2);
        authLayout->addLayout(userHostRow);

        auto *passRow = new QHBoxLayout; passRow->setSpacing(8);
        sshExplorerPassword_ = new QLineEdit(authCard);
        sshExplorerPassword_->setPlaceholderText(QStringLiteral("Пароль (пусто = по ключу)"));
        sshExplorerPassword_->setEchoMode(QLineEdit::Password);
        sshExplorerPasswordReveal_ = button(QStringLiteral("Показать"), authCard);
        sshExplorerPasswordReveal_->setCheckable(true); sshExplorerPasswordReveal_->setFixedWidth(88);
        passRow->addWidget(sshExplorerPassword_, 1); passRow->addWidget(sshExplorerPasswordReveal_);
        authLayout->addLayout(passRow);

        auto *portToggle = new QPushButton(QStringLiteral("▸ Параметры порта (22)"), authCard);
        portToggle->setCheckable(true); portToggle->setCursor(Qt::PointingHandCursor);
        portToggle->setStyleSheet(QStringLiteral("QPushButton{background:transparent;border:0;color:rgba(235,240,248,0.65);text-align:left;padding:2px 0;font-size:12px;}QPushButton:hover{color:#fff;}"));
        authLayout->addWidget(portToggle);

        auto *portBox = new QWidget(authCard); portBox->setVisible(false);
        auto *portRow = new QHBoxLayout(portBox); portRow->setContentsMargins(0, 0, 0, 0);
        sshExplorerPort_ = new QSpinBox(portBox);
        sshExplorerPort_->setRange(1, 65535); sshExplorerPort_->setValue(22);
        sshExplorerPort_->setPrefix(QStringLiteral("Порт: ")); sshExplorerPort_->setFixedWidth(110);
        sshExplorerPort_->setButtonSymbols(QAbstractSpinBox::NoButtons);
        portRow->addWidget(sshExplorerPort_); portRow->addStretch(1);
        authLayout->addWidget(portBox);

        auto *connectBtn = button(QStringLiteral("Подключиться"), authCard);
        connectBtn->setStyleSheet(QStringLiteral("QPushButton{background:rgba(126,184,218,0.28);border:1px solid rgba(126,184,218,0.5);border-radius:10px;padding:10px;font-weight:600;color:#fff;}QPushButton:hover{background:rgba(126,184,218,0.4);}"));
        authLayout->addWidget(connectBtn);

        auto *sepLine = new QFrame(authCard);
        sepLine->setFrameShape(QFrame::HLine);
        sepLine->setFixedHeight(1);
        sepLine->setStyleSheet(QStringLiteral("background:rgba(255,255,255,0.14);border:0;margin:6px 0;"));
        authLayout->addWidget(sepLine);

        auto *savedTitle = new QLabel(QStringLiteral("Недавние серверы"), authCard);
        savedTitle->setStyleSheet(QStringLiteral("font-size:12px;font-weight:600;color:rgba(235,240,248,0.6);border:0;background:transparent;"));
        authLayout->addWidget(savedTitle);

        auto *savedListBox = new QWidget(authCard);
        auto *savedListLay = new QVBoxLayout(savedListBox);
        savedListLay->setContentsMargins(0, 0, 0, 0);
        savedListLay->setSpacing(6);

        QSettings sftpStore(QString::fromLatin1(kSettingsOrg), QString::fromLatin1(kSettingsApp));
        const QVariantList savedList = sftpStore.value(QStringLiteral("sftpServers")).toList();
        if (savedList.isEmpty()) {
            sepLine->hide();
            savedTitle->hide();
        } else {
            for (const QVariant &v : savedList) {
                const QVariantMap m = v.toMap();
                const QString uLogin = m.value(QStringLiteral("login")).toString();
                const QString uHost = m.value(QStringLiteral("host")).toString();
                const int uPort = m.value(QStringLiteral("port"), 22).toInt();
                const QString uPass = QString::fromUtf8(QByteArray::fromBase64(m.value(QStringLiteral("pass")).toByteArray()));

                auto *srvBtn = new QPushButton(QStringLiteral("🖥   %1@%2 : %3").arg(uLogin, uHost).arg(uPort), savedListBox);
                srvBtn->setCursor(Qt::PointingHandCursor);
                srvBtn->setMinimumHeight(42);
                srvBtn->setStyleSheet(QStringLiteral("QPushButton{background:rgba(255,255,255,0.06);border:1px solid rgba(255,255,255,0.10);border-radius:10px;padding:8px 14px;text-align:left;font-size:13.5px;font-weight:500;color:#f3f5f8;}QPushButton:hover{background:rgba(126,184,218,0.22);border-color:rgba(126,184,218,0.48);}"));
                connect(srvBtn, &QPushButton::clicked, this, [this, uLogin, uHost, uPort, uPass, connectBtn] {
                    sshExplorerLogin_->setText(uLogin);
                    sshExplorerHost_->setText(uHost);
                    sshExplorerPort_->setValue(uPort);
                    sshExplorerPassword_->setText(uPass);
                    connectBtn->click();
                });
                savedListLay->addWidget(srvBtn);
            }
        }
        authLayout->addWidget(savedListBox);

        auto *centerRow = new QHBoxLayout;
        centerRow->addStretch(1); centerRow->addWidget(authCard); centerRow->addStretch(1);
        authOuter->addLayout(centerRow); authOuter->addStretch(1);
        rootLayout->addWidget(authContainer, 1);

                auto *explorerBody = new QWidget(sshExplorerCard_);
        explorerBody->setObjectName(QStringLiteral("sftpExplorerBody"));
        explorerBody->setVisible(false);
        auto *expLayout = new QVBoxLayout(explorerBody);
        expLayout->setContentsMargins(0, 0, 0, 0); expLayout->setSpacing(0);

        auto *navStrip = new QFrame(explorerBody);
        navStrip->setStyleSheet(QStringLiteral("QFrame{background:rgba(255,255,255,0.04);border-top:1px solid rgba(255,255,255,0.08);border-bottom:1px solid rgba(255,255,255,0.07);}QPushButton{background:transparent;border:0;border-radius:6px;padding:5px 9px;color:#e6ebf2;font-size:12.5px;}QPushButton:hover{background:rgba(255,255,255,0.09);}"));
        auto *topNav = new QHBoxLayout(navStrip); topNav->setContentsMargins(10, 6, 10, 6); topNav->setSpacing(3);
        const QString navBtnCss = QStringLiteral("QPushButton{min-height:28px;max-width:28px;min-width:28px;background:transparent;border:0;border-radius:6px;color:#dce3ec;font-size:14px;padding:0;}QPushButton:hover{background:rgba(255,255,255,0.09);}");
        auto *backBtn = button(QStringLiteral("←"), navStrip); backBtn->setStyleSheet(navBtnCss); backBtn->setToolTip(QStringLiteral("Назад"));
        auto *fwdBtn = button(QStringLiteral("→"), navStrip); fwdBtn->setStyleSheet(navBtnCss); fwdBtn->setToolTip(QStringLiteral("Вперёд"));
        auto *upBtn = button(QStringLiteral("↑"), navStrip); upBtn->setStyleSheet(navBtnCss); upBtn->setToolTip(QStringLiteral("Вверх"));
        auto *refBtn = button(QStringLiteral("↻"), navStrip); refBtn->setStyleSheet(navBtnCss); refBtn->setToolTip(QStringLiteral("Обновить"));

        auto *uploadBtn = button(QStringLiteral("↑ Загрузить"), navStrip);
        auto *downloadBtn = button(QStringLiteral("↓ Скачать"), navStrip);
        auto *cutBtn = button(QStringLiteral("✂ Вырезать"), navStrip);
        auto *pasteHereBtn = button(QStringLiteral("Вставить сюда"), navStrip); pasteHereBtn->setVisible(false);
        auto *switchBtn = button(QStringLiteral("⇄ Сменить"), navStrip);

        sshExplorerPath_ = new QLineEdit(navStrip);
        sshExplorerPath_->setStyleSheet(QStringLiteral("QLineEdit{background:rgba(12,15,22,0.55);border:1px solid rgba(255,255,255,0.10);border-radius:6px;padding:5px 10px;color:#f3f5f8;font-size:13px;}QLineEdit:focus{border-color:rgba(126,184,218,0.65);}"));
        auto *searchEdit = new QLineEdit(navStrip);
        searchEdit->setPlaceholderText(QStringLiteral("Search…")); searchEdit->setFixedWidth(160);
        searchEdit->setStyleSheet(QStringLiteral("QLineEdit{background:rgba(12,15,22,0.55);border:1px solid rgba(255,255,255,0.10);border-radius:6px;padding:5px 10px;color:#f3f5f8;font-size:13px;}"));

        topNav->addWidget(backBtn); topNav->addWidget(fwdBtn); topNav->addWidget(upBtn); topNav->addWidget(refBtn);
        topNav->addSpacing(6);
        topNav->addWidget(uploadBtn); topNav->addWidget(downloadBtn); topNav->addWidget(cutBtn); topNav->addWidget(pasteHereBtn);
        topNav->addSpacing(6);
        topNav->addWidget(sshExplorerPath_, 1);
        topNav->addWidget(searchEdit);
        topNav->addWidget(switchBtn);
        expLayout->addWidget(navStrip);

        auto *splitRow = new QHBoxLayout; splitRow->setContentsMargins(0, 0, 0, 0); splitRow->setSpacing(0);
        auto *sideNav = new QFrame(explorerBody); sideNav->setFixedWidth(185);
        sideNav->setStyleSheet(QStringLiteral("QFrame{background:rgba(18,22,30,0.45);border-right:1px solid rgba(255,255,255,0.07);}QPushButton{background:transparent;border:0;border-radius:6px;text-align:left;padding:7px 12px;color:#dce3ec;font-size:13px;}QPushButton:hover{background:rgba(255,255,255,0.08);color:#fff;}"));
        auto *sideLayout = new QVBoxLayout(sideNav); sideLayout->setContentsMargins(8, 10, 8, 10); sideLayout->setSpacing(2);
        const QList<QPair<QString, QString>> places = {
            {QStringLiteral("🏠  Домашняя"), QStringLiteral("~")},
            {QStringLiteral("💻  Корень (/)"), QStringLiteral("/")},
            {QStringLiteral("🌐  Сайты (/var/www)"), QStringLiteral("/var/www")},
            {QStringLiteral("⚙  Конфиги (/etc)"), QStringLiteral("/etc")},
            {QStringLiteral("📁  Временные (/tmp)"), QStringLiteral("/tmp")}
        };
        for (const auto &place : places) {
            auto *pBtn = new QPushButton(place.first, sideNav); pBtn->setCursor(Qt::PointingHandCursor);
            const QString targetDir = place.second;
            connect(pBtn, &QPushButton::clicked, this, [this, targetDir] {
                if (!sshExplorerSession_) return;
                if (!sshExplorerPath_->text().trimmed().isEmpty()) s_sftpBackHistory.append(sshExplorerPath_->text().trimmed());
                sshExplorerSession_->remoteDirectory = targetDir; sshExplorerPath_->setText(targetDir); refreshSshExplorer();
            });
            sideLayout->addWidget(pBtn);
        }
        sideLayout->addStretch(1); splitRow->addWidget(sideNav);

        sshExplorerTree_ = new QTreeWidget(explorerBody);
        sshExplorerTree_->setColumnCount(3);
        sshExplorerTree_->setHeaderLabels({QStringLiteral("Name"), QStringLiteral("Size"), QStringLiteral("Permissions")});
        sshExplorerTree_->setRootIsDecorated(false); sshExplorerTree_->setAlternatingRowColors(false); sshExplorerTree_->setSelectionMode(QAbstractItemView::ExtendedSelection);
        sshExplorerTree_->setStyleSheet(QStringLiteral("QTreeWidget{background:rgba(14,17,24,0.55);border:0;padding:4px;font-size:13px;}QTreeWidget::item{height:32px;border-radius:6px;padding-left:4px;}QTreeWidget::item:hover{background:rgba(255,255,255,0.06);}QTreeWidget::item:selected{background:rgba(126,184,218,0.22);color:#fff;}QHeaderView::section{background:transparent;border:0;border-bottom:1px solid rgba(255,255,255,0.07);padding:7px 10px;color:rgba(235,240,248,0.6);font-weight:500;}"));
        sshExplorerTree_->header()->setSectionResizeMode(0, QHeaderView::Stretch);
        sshExplorerTree_->header()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
        sshExplorerTree_->header()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
        splitRow->addWidget(sshExplorerTree_, 1);
        auto addExpKey = [this](const QKeySequence &seq, auto fn) {
            auto *sc = new QShortcut(seq, sshExplorerTree_);
            sc->setContext(Qt::WidgetWithChildrenShortcut);
            connect(sc, &QShortcut::activated, this, fn);
        };
        addExpKey(QKeySequence::SelectAll, [this] { sshExplorerTree_->selectAll(); });
        addExpKey(QKeySequence(Qt::Key_F5), [this] { refreshSshExplorer(); });
        addExpKey(QKeySequence(Qt::Key_Backspace), [this] { navigateSftpParent(); });
        addExpKey(QKeySequence(Qt::ALT | Qt::Key_Left), [backBtn] { backBtn->click(); });
        addExpKey(QKeySequence(Qt::ALT | Qt::Key_Right), [fwdBtn] { fwdBtn->click(); });
        addExpKey(QKeySequence::Cut, [cutBtn] { cutBtn->click(); });
        addExpKey(QKeySequence::Paste, [pasteHereBtn] { if (pasteHereBtn->isVisible()) pasteHereBtn->click(); });
        addExpKey(QKeySequence(Qt::Key_F2), [this] {
            QTreeWidgetItem *it = sshExplorerTree_->currentItem();
            if (!it) return;
            const QString remote = it->data(0, Qt::UserRole).toString();
            const QString base = QFileInfo(remote).fileName();
            bool ok = false;
            const QString next = QInputDialog::getText(this, QStringLiteral("Переименовать"), QStringLiteral("Новое имя:"), QLineEdit::Normal, base, &ok).trimmed();
            if (ok && !next.isEmpty() && next != base) {
                const QString dst = QDir::cleanPath(QDir(QFileInfo(remote).path()).filePath(next));
                startSftpTransfer({QStringLiteral("rename %1 %2").arg(sftpQuote(remote), sftpQuote(dst))}, QStringLiteral("Переименовано в %1").arg(next));
            }
        });
        addExpKey(QKeySequence::Delete, [this] {
            const QList<QTreeWidgetItem *> items = sshExplorerTree_->selectedItems();
            if (items.isEmpty()) return;
            const QString label = items.size() == 1 ? QStringLiteral("«%1»").arg(items.first()->text(0)) : QStringLiteral("%1 элементов").arg(items.size());
            if (QMessageBox::question(this, QStringLiteral("Удаление"), QStringLiteral("Удалить %1?").arg(label)) != QMessageBox::Yes) return;
            QStringList sftpRm;
            QStringList rmDirs;
            for (QTreeWidgetItem *it : items) {
                const QString rem = it->data(0, Qt::UserRole).toString();
                const bool isDir = it->data(0, Qt::UserRole + 1).toBool() || it->text(2).startsWith(QLatin1Char('d'));
                if (isDir) rmDirs.append(sftpQuote(rem));
                else sftpRm.append(QStringLiteral("rm %1").arg(sftpQuote(rem)));
            }
            if (!rmDirs.isEmpty() && sshExplorerSession_ && sshExplorerSession_->backend) {
                sshExplorerSession_->backend->writeInput(QStringLiteral("rm -rf %1\r").arg(rmDirs.join(QLatin1Char(' '))).toUtf8());
            }
            if (!sftpRm.isEmpty()) startSftpTransfer(sftpRm, QStringLiteral("Удалено: %1").arg(label));
            else QTimer::singleShot(800, this, [this] { refreshSshExplorer(); });
        });
        sshExplorerTree_->setContextMenuPolicy(Qt::CustomContextMenu);
        connect(sshExplorerTree_, &QTreeWidget::customContextMenuRequested, this, [this, pasteHereBtn](const QPoint &pos) {
            if (!sshExplorerSession_) return;
            QTreeWidgetItem *item = sshExplorerTree_->itemAt(pos);
            QMenu menu(sshExplorerTree_);
            menu.setStyleSheet(QStringLiteral("QMenu{background:rgba(24,29,40,0.97);border:1px solid rgba(255,255,255,0.16);border-radius:10px;padding:6px;color:#eef2f8;}QMenu::item{padding:7px 16px;border-radius:6px;}QMenu::item:selected{background:rgba(126,184,218,0.25);}QMenu::separator{height:1px;background:rgba(255,255,255,0.10);margin:4px 8px;}"));

            if (item) {
                const QString remote = item->data(0, Qt::UserRole).toString();
                const bool isDir = item->data(0, Qt::UserRole + 1).toBool() || item->text(2).startsWith(QLatin1Char('d'));
                const QString baseName = QFileInfo(remote).fileName();

                if (!isDir) {
                    QAction *actNano = menu.addAction(QStringLiteral("📝  Открыть в nano (консоль)"));
                    connect(actNano, &QAction::triggered, this, [this, remote] {
                        ConsoleSession *s = sshExplorerSession_;
                        if (!s || !s->backend) return;
                        if (sshExplorerOverlay_) sshExplorerOverlay_->hide();
                        s->backend->writeInput(QStringLiteral("nano %1\r").arg(sftpQuote(remote)).toUtf8());
                        if (s->output) s->output->setFocus(Qt::OtherFocusReason);
                    });

                    QAction *actLocalEdit = menu.addAction(QStringLiteral("📄  Открыть на ПК"));
                    connect(actLocalEdit, &QAction::triggered, this, [this, remote, baseName] {
                        const QString tmp = QDir::temp().filePath(baseName);
                        const QString lq = QStringLiteral("\"%1\"").arg(QDir::toNativeSeparators(tmp));
                        startSftpTransfer({QStringLiteral("get %1 %2").arg(sftpQuote(remote), lq)}, QStringLiteral("Открываю: %1").arg(baseName));
                        QTimer::singleShot(1200, this, [tmp] { QDesktopServices::openUrl(QUrl::fromLocalFile(tmp)); });
                    });
                    menu.addSeparator();
                }

                QAction *actDownload = menu.addAction(QStringLiteral("↓  Скачать"));
                connect(actDownload, &QAction::triggered, this, [this, remote, isDir, baseName] {
                    QString local = isDir
                        ? QFileDialog::getExistingDirectory(this, QStringLiteral("Скачать папку"), QDir::homePath())
                        : QFileDialog::getSaveFileName(this, QStringLiteral("Скачать файл"), QDir::home().filePath(baseName));
                    if (local.isEmpty()) return;
                    if (isDir) local = QDir(local).filePath(baseName);
                    const QString lq = QStringLiteral("\"%1\"").arg(QDir::toNativeSeparators(local));
                    startSftpTransfer({QStringLiteral("get%1 %2 %3").arg(isDir ? QStringLiteral(" -r") : QString(), sftpQuote(remote), lq)}, QStringLiteral("Сохранено: %1").arg(QFileInfo(local).fileName()));
                });

                QAction *actRename = menu.addAction(QStringLiteral("✎  Переименовать"));
                connect(actRename, &QAction::triggered, this, [this, remote, baseName] {
                    bool ok = false;
                    const QString nextName = QInputDialog::getText(this, QStringLiteral("Переименовать"), QStringLiteral("Новое имя:"), QLineEdit::Normal, baseName, &ok).trimmed();
                    if (!ok || nextName.isEmpty() || nextName == baseName) return;
                    const QString dst = QDir::cleanPath(QDir(QFileInfo(remote).path()).filePath(nextName));
                    startSftpTransfer({QStringLiteral("rename %1 %2").arg(sftpQuote(remote), sftpQuote(dst))}, QStringLiteral("Переименовано в %1").arg(nextName));
                });

                QAction *actMove = menu.addAction(QStringLiteral("➜  Переместить в…"));
                connect(actMove, &QAction::triggered, this, [this, remote, baseName] {
                    bool ok = false;
                    const QString targetDir = QInputDialog::getText(this, QStringLiteral("Переместить"), QStringLiteral("Целевая папка на сервере:"), QLineEdit::Normal, QFileInfo(remote).path(), &ok).trimmed();
                    if (!ok || targetDir.isEmpty()) return;
                    const QString dst = QDir::cleanPath(QDir(targetDir).filePath(baseName));
                    startSftpTransfer({QStringLiteral("rename %1 %2").arg(sftpQuote(remote), sftpQuote(dst))}, QStringLiteral("Перемещено в %1").arg(targetDir));
                });

                QAction *actCut = menu.addAction(QStringLiteral("✂  Вырезать"));
                connect(actCut, &QAction::triggered, this, [this, remote, baseName, pasteHereBtn] {
                    s_sftpCutRemotePaths = {remote};
                    pasteHereBtn->setVisible(true);
                    if (sshExplorerStatus_) sshExplorerStatus_->setText(QStringLiteral("Вырезано: %1").arg(baseName));
                });

                QAction *actArchive = menu.addAction(QStringLiteral("📦  Добавить в архив (.tar.gz)"));
                connect(actArchive, &QAction::triggered, this, [this, remote, baseName] {
                    ConsoleSession *s = sshExplorerSession_;
                    if (!s || !s->backend) return;
                    const QString parentDir = QFileInfo(remote).path();
                    const QString archName = baseName + QStringLiteral(".tar.gz");
                    s->backend->writeInput(QStringLiteral("tar -czf %1 -C %2 %3\r").arg(sftpQuote(parentDir + QLatin1Char('/') + archName), sftpQuote(parentDir), sftpQuote(baseName)).toUtf8());
                    if (sshExplorerStatus_) sshExplorerStatus_->setText(QStringLiteral("Архивация: %1").arg(archName));
                    QTimer::singleShot(1400, this, [this] { refreshSshExplorer(); });
                });

                menu.addSeparator();
                QAction *actDelete = menu.addAction(QStringLiteral("🗑  Удалить"));
                connect(actDelete, &QAction::triggered, this, [this, remote, isDir, baseName] {
                    if (QMessageBox::question(this, QStringLiteral("Удаление"), QStringLiteral("Удалить «%1»?").arg(baseName)) != QMessageBox::Yes) return;
                    if (isDir) {
                        if (sshExplorerSession_ && sshExplorerSession_->backend)
                            sshExplorerSession_->backend->writeInput(QStringLiteral("rm -rf %1\r").arg(sftpQuote(remote)).toUtf8());
                        QTimer::singleShot(800, this, [this] { refreshSshExplorer(); });
                    } else {
                        startSftpTransfer({QStringLiteral("rm %1").arg(sftpQuote(remote))}, QStringLiteral("Удалено: %1").arg(baseName));
                    }
                });
            }

            if (!s_sftpCutRemotePaths.isEmpty()) {
                if (item) menu.addSeparator();
                QAction *actPaste = menu.addAction(QStringLiteral("📋  Вставить сюда (%1)").arg(QString::number(s_sftpCutRemotePaths.size())));
                connect(actPaste, &QAction::triggered, this, [this, pasteHereBtn] {
                    if (!sshExplorerSession_ || s_sftpCutRemotePaths.isEmpty()) return;
                    QString dir = sshExplorerPath_->text().trimmed();
                    if (dir.isEmpty()) dir = sshExplorerSession_->remoteDirectory;
                    const QString src = s_sftpCutRemotePaths.first();
                    const QString dst = QDir::cleanPath(QDir(dir).filePath(QFileInfo(src).fileName()));
                    s_sftpCutRemotePaths.clear();
                    pasteHereBtn->setVisible(false);
                    startSftpTransfer({QStringLiteral("rename %1 %2").arg(sftpQuote(src), sftpQuote(dst))}, QStringLiteral("Перемещено в %1").arg(dir));
                });
            }

            menu.addSeparator();
            QAction *actMkdir = menu.addAction(QStringLiteral("📁  Создать папку"));
            connect(actMkdir, &QAction::triggered, this, [this] {
                if (!sshExplorerSession_) return;
                bool ok = false;
                const QString folder = QInputDialog::getText(this, QStringLiteral("Новая папка"), QStringLiteral("Имя папки:"), QLineEdit::Normal, QString(), &ok).trimmed();
                if (!ok || folder.isEmpty()) return;
                QString dir = sshExplorerPath_->text().trimmed();
                if (dir.isEmpty()) dir = sshExplorerSession_->remoteDirectory;
                const QString full = QDir::cleanPath(QDir(dir).filePath(folder));
                startSftpTransfer({QStringLiteral("mkdir %1").arg(sftpQuote(full))}, QStringLiteral("Папка создана: %1").arg(folder));
            });

            menu.exec(sshExplorerTree_->viewport()->mapToGlobal(pos));
        });

        expLayout->addLayout(splitRow, 1);
        rootLayout->addWidget(explorerBody, 1);


        auto *footer = new QHBoxLayout;
        sshExplorerStatus_ = new QLabel(QStringLiteral("Готово"), sshExplorerCard_);
        sshExplorerStatus_->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Preferred);
        sshExplorerStatus_->setStyleSheet(QStringLiteral("color:rgba(235,240,248,0.6);font-size:12px;"));
        class CardGrip : public QWidget {
        public:
            explicit CardGrip(QWidget *c) : QWidget(c), card_(c) { setFixedSize(16, 16); setCursor(Qt::SizeFDiagCursor); }
        protected:
            void paintEvent(QPaintEvent *) override {
                QPainter p(this); p.setRenderHint(QPainter::Antialiasing); p.setPen(QPen(QColor(220, 230, 245, 150), 1.5));
                p.drawLine(5, 14, 14, 5); p.drawLine(9, 14, 14, 9);
            }
            void mousePressEvent(QMouseEvent *e) override { if (e->button() == Qt::LeftButton && card_) { drag_ = true; pos_ = e->globalPosition().toPoint(); sz_ = card_->size(); } }
            void mouseMoveEvent(QMouseEvent *e) override {
                if (drag_ && card_ && card_->parentWidget()) {
                    const QPoint d = e->globalPosition().toPoint() - pos_;
                    const int w = qBound(640, sz_.width() + d.x() * 2, qMax(640, card_->parentWidget()->width() - 32));
                    const int h = qBound(460, sz_.height() + d.y() * 2, qMax(460, card_->parentWidget()->height() - 32));
                    card_->resize(w, h); card_->move((card_->parentWidget()->width() - w) / 2, (card_->parentWidget()->height() - h) / 2);
                }
            }
            void mouseReleaseEvent(QMouseEvent *) override { drag_ = false; }
        private:
            QWidget *card_ = nullptr; bool drag_ = false; QPoint pos_; QSize sz_;
        };
        auto *grip = new CardGrip(sshExplorerCard_); sshExplorerResizeGrip_ = grip;
        footer->setContentsMargins(14, 4, 10, 2); footer->addWidget(sshExplorerStatus_, 1); footer->addWidget(grip, 0, Qt::AlignRight | Qt::AlignBottom);
        rootLayout->addLayout(footer);
        connect(minBtn, &QPushButton::clicked, this, [this] { if (sshExplorerOverlay_) sshExplorerOverlay_->hide(); }); connect(maxBtn, &QPushButton::clicked, this, [this] { if (!sshExplorerCard_ || !sshExplorerOverlay_) return; if (sshExplorerCard_->width() >= sshExplorerOverlay_->width() - 60) { sshExplorerCard_->resize(920, 600); } else { sshExplorerCard_->resize(sshExplorerOverlay_->width() - 48, sshExplorerOverlay_->height() - 48); } sshExplorerCard_->move((sshExplorerOverlay_->width() - sshExplorerCard_->width()) / 2, (sshExplorerOverlay_->height() - sshExplorerCard_->height()) / 2); }); connect(closeBtn, &QPushButton::clicked, this, [this] { closeSshExplorer(); });
        connect(portToggle, &QPushButton::toggled, portBox, [portToggle, portBox](bool o) {
            portBox->setVisible(o); portToggle->setText(o ? QStringLiteral("▾ Параметры порта") : QStringLiteral("▸ Параметры порта (22)"));
        });
        connect(sshExplorerPasswordReveal_, &QPushButton::toggled, sshExplorerPassword_, [this](bool v) {
            sshExplorerPassword_->setEchoMode(v ? QLineEdit::Normal : QLineEdit::Password);
            sshExplorerPasswordReveal_->setText(v ? QStringLiteral("Скрыть") : QStringLiteral("Показать"));
        });
        connect(switchBtn, &QPushButton::clicked, this, [this, authContainer, explorerBody] {
            stopSftpSession(true); sftpConnected_ = false; explorerBody->setVisible(false); authContainer->setVisible(true);
            if (sshExplorerStatus_) sshExplorerStatus_->setText(QStringLiteral("Введите данные сервера"));
        });
        auto doConnect = [this] {
            ConsoleSession *s = sshExplorerSession_; if (!s) return;
            QString login = sshExplorerLogin_->text().trimmed(), host = sshExplorerHost_->text().trimmed();
            if (host.contains(QLatin1Char('@')) && login.isEmpty()) { const int at = host.indexOf(QLatin1Char('@')); login = host.left(at).trimmed(); host = host.mid(at + 1).trimmed(); }
            if (login.isEmpty() || host.isEmpty()) { sshExplorerStatus_->setText(QStringLiteral("Укажите логин и хост")); return; }
            s->sshLogin = login; s->sshHost = host; s->sshPort = sshExplorerPort_->value();
            s->sshTarget = login + QLatin1Char('@') + host; s->sshPassword = sshExplorerPassword_->text();
            s->sshSftpOptions = QStringList{QStringLiteral("-P"), QString::number(s->sshPort)};
            s->sshActive = true; s->remoteDirectory = QStringLiteral(".");
            
            sshExplorerPath_->setText(s->remoteDirectory); QSettings st(QString::fromLatin1(kSettingsOrg), QString::fromLatin1(kSettingsApp));
            QVariantList list = st.value(QStringLiteral("sftpServers")).toList();
            for (int i = list.size() - 1; i >= 0; --i) {
                const QVariantMap m = list.at(i).toMap();
                if (m.value("login").toString() == s->sshLogin && m.value("host").toString() == s->sshHost) list.removeAt(i);
            }
            list.prepend(QVariantMap{
                {"login", s->sshLogin}, {"host", s->sshHost}, {"port", s->sshPort},
                {"pass", QString::fromUtf8(s->sshPassword.toUtf8().toBase64())}
            });
            st.setValue(QStringLiteral("sftpServers"), list);
            startSftpSession(s);
        };
        connect(connectBtn, &QPushButton::clicked, this, doConnect);
        connect(sshExplorerPassword_, &QLineEdit::returnPressed, this, doConnect);
        connect(refBtn, &QPushButton::clicked, this, [this] { refreshSshExplorer(); });
        connect(sshExplorerPath_, &QLineEdit::returnPressed, this, [this] { refreshSshExplorer(); });
        connect(upBtn, &QPushButton::clicked, this, [this] { navigateSftpParent(); });
        connect(backBtn, &QPushButton::clicked, this, [this] {
            if (!sshExplorerSession_ || s_sftpBackHistory.isEmpty()) return;
            const QString cur = sshExplorerPath_->text().trimmed(); if (!cur.isEmpty()) s_sftpForwardHistory.append(cur);
            const QString prev = s_sftpBackHistory.takeLast(); sshExplorerSession_->remoteDirectory = prev; sshExplorerPath_->setText(prev); refreshSshExplorer();
        });
        connect(fwdBtn, &QPushButton::clicked, this, [this] {
            if (!sshExplorerSession_ || s_sftpForwardHistory.isEmpty()) return;
            const QString cur = sshExplorerPath_->text().trimmed(); if (!cur.isEmpty()) s_sftpBackHistory.append(cur);
            const QString next = s_sftpForwardHistory.takeLast(); sshExplorerSession_->remoteDirectory = next; sshExplorerPath_->setText(next); refreshSshExplorer();
        });
        connect(searchEdit, &QLineEdit::textChanged, this, [this](const QString &q) {
            if (!sshExplorerTree_) return;
            for (int i = 0; i < sshExplorerTree_->topLevelItemCount(); ++i) {
                QTreeWidgetItem *it = sshExplorerTree_->topLevelItem(i);
                it->setHidden(!q.isEmpty() && !it->text(0).contains(q, Qt::CaseInsensitive));
            }
        });
        connect(cutBtn, &QPushButton::clicked, this, [this, pasteHereBtn] {
            const QList<QTreeWidgetItem *> items = sshExplorerTree_->selectedItems();
            if (items.isEmpty()) { sshExplorerStatus_->setText(QStringLiteral("Выберите элементы")); return; }
            s_sftpCutRemotePaths.clear();
            for (QTreeWidgetItem *it : items) s_sftpCutRemotePaths.append(it->data(0, Qt::UserRole).toString());
            pasteHereBtn->setVisible(true);
            sshExplorerStatus_->setText(QStringLiteral("Вырезано элементов: %1").arg(s_sftpCutRemotePaths.size()));
        });
        connect(pasteHereBtn, &QPushButton::clicked, this, [this, pasteHereBtn] {
            if (!sshExplorerSession_ || s_sftpCutRemotePaths.isEmpty()) return;
            QString dir = sshExplorerPath_->text().trimmed(); if (dir.isEmpty()) dir = sshExplorerSession_->remoteDirectory;
            QStringList cmds;
            for (const QString &src : s_sftpCutRemotePaths) {
                const QString dst = QDir::cleanPath(QDir(dir).filePath(QFileInfo(src).fileName()));
                cmds.append(QStringLiteral("rename %1 %2").arg(sftpQuote(src), sftpQuote(dst)));
            }
            s_sftpCutRemotePaths.clear(); pasteHereBtn->setVisible(false);
            startSftpTransfer(cmds, QStringLiteral("Перемещено в %1").arg(dir));
        });
        
        connect(sshExplorerTree_, &QTreeWidget::itemActivated, this, [this](QTreeWidgetItem *item, int) {
            if (!sshExplorerSession_ || !item) return;
            const QString path = item->data(0, Qt::UserRole).toString();
            const bool isDir = item->data(0, Qt::UserRole + 1).toBool() || item->text(2).startsWith(QLatin1Char('d'));
            if (isDir) {
                if (!sshExplorerPath_->text().trimmed().isEmpty()) { s_sftpBackHistory.append(sshExplorerPath_->text().trimmed()); s_sftpForwardHistory.clear(); }
                sshExplorerSession_->remoteDirectory = path; sshExplorerPath_->setText(path); refreshSshExplorer();
            } else if (!path.isEmpty()) insertRemotePath(path);
        });
        connect(downloadBtn, &QPushButton::clicked, this, [this] {
            if (!sshExplorerSession_) return;
            QTreeWidgetItem *item = sshExplorerTree_ ? sshExplorerTree_->currentItem() : nullptr;
            const QString remote = selectedRemotePath(); if (remote.isEmpty() || !item) { sshExplorerStatus_->setText(QStringLiteral("Выберите файл или папку")); return; }
            const bool isDir = item->data(0, Qt::UserRole + 1).toBool();
            QString local = isDir ? QFileDialog::getExistingDirectory(this, QStringLiteral("Скачать папку"), QDir::homePath()) : QFileDialog::getSaveFileName(this, QStringLiteral("Скачать файл"), QDir::home().filePath(QFileInfo(remote).fileName()));
            if (local.isEmpty()) return;
            if (isDir) local = QDir(local).filePath(QFileInfo(remote).fileName());
            const QString lq = QStringLiteral("\"%1\"").arg(QDir::toNativeSeparators(local));
            startSftpTransfer({QStringLiteral("get%1 %2 %3").arg(isDir ? QStringLiteral(" -r") : QString(), sftpQuote(remote), lq)}, QStringLiteral("Сохранено: %1").arg(QFileInfo(local).fileName()));
        });
        connect(uploadBtn, &QPushButton::clicked, this, [this] {
            if (!sshExplorerSession_) return;
            const QStringList locals = QFileDialog::getOpenFileNames(this, QStringLiteral("Загрузить файлы на сервер")); if (locals.isEmpty()) return;
            QString dir = sshExplorerPath_->text().trimmed(); if (dir.isEmpty()) dir = sshExplorerSession_->remoteDirectory;
            dir = QDir::fromNativeSeparators(dir);
            QStringList cmds;
            cmds.append(QStringLiteral("cd %1").arg(sftpQuote(dir)));
            for (const QString &l : locals) {
                QString localPath = QDir::fromNativeSeparators(QFileInfo(l).absoluteFilePath());
                localPath.replace(QLatin1Char('"'), QStringLiteral("\\\""));
                QString dst = dir;
                if (!dst.isEmpty() && dst != QStringLiteral("." ) && !dst.endsWith(QLatin1Char('/'))) dst += QLatin1Char('/');
                if (dst == QStringLiteral(".")) dst.clear();
                dst += QFileInfo(l).fileName();
                cmds.append(QStringLiteral("put \"%1\" \"%2\"").arg(localPath, dst));
            }
            cmds.append(QStringLiteral("ls -la"));
            const QString fileLabel = locals.size() == 1 ? QFileInfo(locals.first()).fileName() : QStringLiteral("%1 файлов").arg(locals.size());
            startSftpTransfer(cmds, QStringLiteral("Загружено: %1").arg(fileLabel));
        });

        sshExplorerCard_->setStyleSheet(QStringLiteral("QFrame#sshExplorerCard{background:qlineargradient(x1:0,y1:0,x2:1,y2:1,stop:0 rgba(28,34,46,0.97),stop:1 rgba(16,20,28,0.97));border:1px solid rgba(255,255,255,0.16);border-radius:18px;}QTreeWidget{background:rgba(7,10,15,0.38);border:1px solid rgba(255,255,255,0.08);border-radius:12px;alternate-background-color:rgba(255,255,255,0.02);}QTreeWidget::item{height:30px;border-bottom:1px solid rgba(255,255,255,0.04);}QTreeWidget::item:selected{background:rgba(126,184,218,0.22);}QHeaderView::section{background:rgba(255,255,255,0.05);border:0;padding:7px;color:rgba(245,247,250,0.7);}"));
        for (QWidget *child : sshExplorerOverlay_->findChildren<QWidget *>()) child->installEventFilter(this);
    }

    sshExplorerSession_ = session;
    if (sshExplorerLogin_) sshExplorerLogin_->setText(session->sshLogin);
    if (sshExplorerHost_) sshExplorerHost_->setText(session->sshHost);
    if (sshExplorerPort_) sshExplorerPort_->setValue(session->sshPort > 0 ? session->sshPort : 22);
    if (sshExplorerPassword_) sshExplorerPassword_->clear();
    

    const bool connected = (sftpConnected_ && sftpOwnerSession_ == session && sftpBackend_ != nullptr);
    if (auto *auth = sshExplorerCard_->findChild<QWidget *>(QStringLiteral("sftpAuthPanel"))) auth->setVisible(!connected);
    if (auto *body = sshExplorerCard_->findChild<QWidget *>(QStringLiteral("sftpExplorerBody"))) body->setVisible(connected);

    sshExplorerOverlay_->setGeometry(root_->rect());
    sshExplorerCard_->move((sshExplorerOverlay_->width() - sshExplorerCard_->width()) / 2,
                           (sshExplorerOverlay_->height() - sshExplorerCard_->height()) / 2);
    sshExplorerOverlay_->show();
    sshExplorerOverlay_->raise();
}


void MainWindow::refreshSshExplorer()
{
    ConsoleSession *session=sshExplorerSession_;
    if(!session||!session->sshActive||!sshExplorerTree_||!sshExplorerPath_)return;
    if(!sftpBackend_||!sftpReady_){if(sshExplorerStatus_)sshExplorerStatus_->setText(QStringLiteral("Сначала подключитесь к SFTP"));return;}
    const QString requested=sshExplorerPath_->text().trimmed().isEmpty()?QStringLiteral("."):sshExplorerPath_->text().trimmed();
    sshExplorerTree_->clear();
    if(sshExplorerStatus_)sshExplorerStatus_->setText(QStringLiteral("Открываю каталог…"));
    sendSftpCommand(QStringLiteral("cd %1").arg(sftpQuote(requested)), SftpPendingAction::ChangeDirectory);
}

QString MainWindow::selectedRemotePath() const
{
    if(!sshExplorerTree_)return {};
    QTreeWidgetItem *item=sshExplorerTree_->currentItem();
    if(!item)return {};
    return item->data(0,Qt::UserRole).toString();
}

void MainWindow::insertRemotePath(const QString &path)
{
    if(!sshExplorerSession_||!sshExplorerSession_->sshActive)return;
    QString value=path;
    if(value.contains(QLatin1Char(' '))||value.contains(QLatin1Char('\''))){value=QLatin1Char('\'')+value.replace(QLatin1Char('\''),QStringLiteral("'\\''"))+QLatin1Char('\'');}
    ConsoleSession *session = sshExplorerSession_;
    if(session->backend)session->backend->writeInput(value.toUtf8());
    closeSshExplorer();
    if(session->output)session->output->setFocus(Qt::OtherFocusReason);
}

void MainWindow::sendCommand()
{
}

void MainWindow::readOutput()
{
    auto *process = qobject_cast<QProcess *>(sender());
    if (!process) return;
    for (ConsoleSession *session : sessions_) {
        if (session->process != process) continue;
        const QByteArray bytes = process->readAllStandardOutput();
        appendOutput(session, QString::fromUtf8(bytes));
        return;
    }
}

void MainWindow::writeCommand(ConsoleSession *session, const QString &rawCommand)
{
    if (!session || (!session->backend && !session->process)) return;
    const QString command = rawCommand;
    captureTerminalInput(session,command.toUtf8()+QByteArray(1,'\r'));
    if (auto *cv = dynamic_cast<TerminalCanvas *>(session->output))
        cv->lastSentCmd = command;
    if (session->backend) {
        session->backend->sendCommand(command);
        return;
    }
    if (session->process) {
#ifdef Q_OS_WIN
        const QByteArray input = (command + QStringLiteral("\r\n")).toUtf8();
#else
        const QByteArray input = (command + QStringLiteral("\n")).toUtf8();
#endif
        session->process->write(input);
        session->process->waitForBytesWritten(50);
    }
}

void MainWindow::appendOutput(ConsoleSession *session, const QString &text)
{
    if (!session || !session->output) return;
    if (auto *terminal=dynamic_cast<TerminalCanvas *>(session->output)) terminal->appendSystemText(text);
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
    binds_.append({name.trimmed(), command, nextBindId_++});
    refreshBinds();
    saveSettings();
}

void MainWindow::refreshBinds()
{
    if (!bindHost_) return;
    auto *layout = qobject_cast<QVBoxLayout *>(bindHost_->layout());
    if (!layout) return;
    while (QLayoutItem *item = layout->takeAt(0)) {
        if (QWidget *w = item->widget()) {
            w->setProperty("closing", true);
            w->removeEventFilter(this);
            w->hide();
            w->deleteLater();
        }
        delete item;
    }
    for (int i = 0; i < binds_.size(); ++i) {
        const int bindIndex = i;
        const quint64 bindId = binds_.at(i).id;
        auto *row = new QWidget(bindHost_);
        row->setObjectName(QStringLiteral("bindRow"));
        row->setAttribute(Qt::WA_Hover, true);
        row->installEventFilter(this);
        auto *rowLayout = new QHBoxLayout(row);
        rowLayout->setContentsMargins(5, 2, 5, 2);
        rowLayout->setSpacing(3);
        auto *run = button(binds_.at(i).name, row);
        run->setObjectName(QStringLiteral("bindRun"));
        run->setToolTip(binds_.at(i).command);
        run->setStyleSheet(QStringLiteral("QPushButton{background:transparent;border:0;text-align:left;padding:5px 8px;}QPushButton:hover{background:rgba(255,255,255,0.08);}"));
        auto *edit = button(QString(), row);
        edit->setIcon(vectorActionIcon(QStringLiteral("edit")));
        edit->setIconSize(QSize(15, 15));
        edit->setObjectName(QStringLiteral("bindEdit"));
        auto *remove = button(QString(), row);
        remove->setIcon(vectorActionIcon(QStringLiteral("close")));
        remove->setIconSize(QSize(15, 15));
        remove->setObjectName(QStringLiteral("bindRemove"));
        edit->setToolTip(QStringLiteral("Изменить команду"));
        remove->setToolTip(QStringLiteral("Удалить команду"));
        edit->setFixedSize(26,26); remove->setFixedSize(26,26);
        edit->setVisible(false); remove->setVisible(false);
        for(QPushButton *action:{edit,remove})action->setStyleSheet(QStringLiteral("QPushButton{min-height:0;background:rgba(255,255,255,0.08);border:0;border-radius:8px;padding:0;}QPushButton:hover{background:rgba(255,255,255,0.16);}"));
        row->setStyleSheet(QStringLiteral("QWidget#bindRow{background:rgba(255,255,255,0.055);border:1px solid rgba(255,255,255,0.08);border-radius:11px;}QWidget#bindRow:hover{background:rgba(255,255,255,0.10);}"));
        rowLayout->addWidget(run, 1);
        rowLayout->addWidget(edit);
        rowLayout->addWidget(remove);
        layout->addWidget(row);
        connect(run, &QPushButton::clicked, this, [this, bindId] {
            for (const CommandBind &bind : binds_)
                if (bind.id == bindId) { runBind(bind); return; }
        });
        connect(edit, &QPushButton::clicked, this, [this, bindId] {
            auto findIndex = [this, bindId] {
                for (int index = 0; index < binds_.size(); ++index)
                    if (binds_.at(index).id == bindId) return index;
                return -1;
            };
            int index = findIndex();
            if (index < 0 || index >= binds_.size()) return;
            bool ok = false;
            const QString name = QInputDialog::getText(this, QStringLiteral("Изменить бинд"), QStringLiteral("Название"), QLineEdit::Normal, binds_.at(index).name, &ok);
            if (!ok || name.trimmed().isEmpty()) return;
            index = findIndex();
            if (index < 0 || index >= binds_.size()) return;
            const QString command = QInputDialog::getText(this, QStringLiteral("Изменить бинд"), QStringLiteral("Команда"), QLineEdit::Normal, binds_.at(index).command, &ok);
            if (!ok || command.trimmed().isEmpty()) return;
            index = findIndex();
            if (index < 0 || index >= binds_.size()) return;
            binds_[index] = {name.trimmed(), command, bindId};
            refreshBinds();
            saveSettings();
        });
        connect(remove, &QPushButton::clicked, this, [this, bindIndex, bindId, row] {
            if (row) {
                row->setProperty("closing", true);
                row->removeEventFilter(this);
                row->setProperty("closing", true); row->removeEventFilter(this); row->setEnabled(false);
            }
            QTimer::singleShot(0, this, [this, bindIndex, bindId] {
                int index = -1;
                if (bindIndex >= 0 && bindIndex < binds_.size() && binds_.at(bindIndex).id == bindId)
                    index = bindIndex;
                else {
                    for (int candidate = 0; candidate < binds_.size(); ++candidate)
                        if (binds_.at(candidate).id == bindId) { index = candidate; break; }
                }
                if (index < 0 || index >= binds_.size()) return;
                binds_.removeAt(index);
                refreshBinds();
                saveSettings();
            });
        });
    }
    layout->addStretch(1);
}

void MainWindow::toggleDrawer()
{
    const int from = drawerOpen_ ? 270 : 0;
    const int to = drawerOpen_ ? 0 : 270;
    drawerOpen_ = !drawerOpen_;
    drawerContent_->setGraphicsEffect(nullptr);
    if (drawerOpen_) drawerContent_->setVisible(true);
    drawer_->setMinimumWidth(0);
    drawer_->setMaximumWidth(270);
    auto *animation = new QPropertyAnimation(drawer_, "maximumWidth", this);
    animation->setDuration(200);
    animation->setEasingCurve(QEasingCurve::OutCubic);
    animation->setStartValue(from);
    animation->setEndValue(to);
    connect(animation, &QPropertyAnimation::finished, this, [this] {
        if (!drawerOpen_) drawerContent_->setVisible(false);
        drawer_->setMinimumWidth(drawerOpen_ ? 270 : 0);
        drawer_->setMaximumWidth(drawerOpen_ ? 270 : 0);
        for (QPushButton *toggle : findChildren<QPushButton *>(QStringLiteral("bindsToggle")))
            toggle->setText(drawerOpen_ ? QStringLiteral("Скрыть бинды") : QStringLiteral("Бинды"));
        if (drawerCollapseButton_) drawerCollapseButton_->setText(drawerOpen_ ? QStringLiteral("Свернуть") : QStringLiteral("Развернуть"));
    });
    animation->start(QAbstractAnimation::DeleteWhenStopped);
}

void MainWindow::setNotesVisible(bool visible)
{
    if (notesVisible_ == visible) return;
    notesVisible_ = visible;
    notesNavButton_->setChecked(visible);
    contentStack_->setCurrentWidget(visible ? notesPage_ : consolePage_);
    updateNavigationIndicators();
    if (!visible) {
        if (ConsoleSession *s = currentSession(); s && s->output) s->output->setFocus();
    }
}

void MainWindow::updateNavigationIndicators()
{
    const bool settingsActive = settingsOverlay_ && settingsOverlay_->isVisible();
    const bool notesActive = !settingsActive && notesVisible_;
    if (notesNavButton_) notesNavButton_->setIcon(navigationDotIcon(notesActive));
    if (settingsNavButton_) settingsNavButton_->setIcon(navigationDotIcon(settingsActive));
    ConsoleSession *activeSession = (!settingsActive && !notesVisible_) ? currentSession() : nullptr;
    for (ConsoleSession *session : sessions_)
        if (session && session->tabButton)
            session->tabButton->setIcon(navigationDotIcon(session == activeSession));
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
    card->setBasePreset(noteCardSizePreset_);
    if(auto *view=dynamic_cast<NotesView *>(noteView_))card->setSelectionMode(view->selectMode_);
    card->headingEditor()->installEventFilter(this);
    card->contentEditor()->installEventFilter(this);
    noteScene_->addItem(card);
    card->setPos(position);
    noteIds_.insert(id, card);
    card->moved = [this, card] {
        for (QGraphicsPathItem *item : noteEdges_) if (auto *edge = dynamic_cast<NoteEdge *>(item); edge && edge->touches(card)) edge->refresh();
        saveNotes();
    };
    card->geometryChanged = [this, card] {
        for (QGraphicsPathItem *item : noteEdges_)
            if (auto *edge = dynamic_cast<NoteEdge *>(item); edge && edge->touches(card)) edge->refresh();
    };
    card->contentChanged = [this] { saveNotes(); };
    if (existingId.isEmpty()) QTimer::singleShot(0, card, [card]{card->focusEditor();});
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
    saveNotes();
    return card;
}

void MainWindow::deleteNote(QGraphicsObject *object)
{
    auto *note = dynamic_cast<NoteCard *>(object);
    if (!note) return;
    if (linkStart_ == note) linkStart_ = nullptr;
    if (previewPath_) { noteScene_->removeItem(previewPath_); delete previewPath_; previewPath_ = nullptr; }
    if (previewGlow_) { noteScene_->removeItem(previewGlow_); delete previewGlow_; previewGlow_ = nullptr; }
    const QList<QGraphicsItem *> items = noteScene_->items();
    const auto edges = noteEdges_;
    for (QGraphicsPathItem *item : edges) {
        auto *edge = dynamic_cast<NoteEdge *>(item);
        if (edge && edge->touches(note)) deleteEdge(edge);
    }
    noteIds_.remove(note->id());
    noteScene_->removeItem(note);
    note->deleteLater();
    saveNotes();
}

void MainWindow::deleteEdge(QGraphicsPathItem *object)
{
    if (!object) return;
    noteEdges_.removeAll(object);
    noteScene_->removeItem(object);
    QTimer::singleShot(0, this, [object] { delete object; });
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
            cards.append(QJsonObject{{QStringLiteral("id"), card->id()}, {QStringLiteral("title"), card->heading()}, {QStringLiteral("body"), card->content()}, {QStringLiteral("x"), card->pos().x()}, {QStringLiteral("y"), card->pos().y()}, {QStringLiteral("width"), card->cardWidth()}, {QStringLiteral("height"), card->cardHeight()}, {QStringLiteral("media"), card->mediaPath()}, {QStringLiteral("selected"), card->isSelected()}});
        } else if (const auto *edge = dynamic_cast<const NoteEdge *>(item)) {
            edges.append(QJsonObject{{QStringLiteral("from"), edge->from()->id()}, {QStringLiteral("to"), edge->to()->id()}});
        }
    }
    const QString directory = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(directory);
    QFile file(activeNotesFilePath());
    if (file.open(QIODevice::WriteOnly | QIODevice::Truncate)) file.write(QJsonDocument(QJsonObject{{QStringLiteral("cards"), cards}, {QStringLiteral("edges"), edges}}).toJson());
}

void MainWindow::loadNotes()
{
    QFile file(activeNotesFilePath());
    if (!file.open(QIODevice::ReadOnly)) return;
    const QJsonObject document = QJsonDocument::fromJson(file.readAll()).object();
    loadingNotes_ = true;
    QHash<QString, NoteCard *> cards;
    for (const QJsonValue &value : document.value(QStringLiteral("cards")).toArray()) {
        const QJsonObject object = value.toObject();
        const QString id = object.value(QStringLiteral("id")).toString();
        auto *card = dynamic_cast<NoteCard *>(createNote(QPointF(object.value(QStringLiteral("x")).toDouble(), object.value(QStringLiteral("y")).toDouble()), object.value(QStringLiteral("title")).toString(), object.value(QStringLiteral("body")).toString(), id));
        const QString m = object.value(QStringLiteral("media")).toString(); if (!m.isEmpty() && QFileInfo::exists(m)) { card->setMediaFile(m); card->pauseMedia(); } card->setStoredSize(object.value(QStringLiteral("width")).toDouble(260.0),object.value(QStringLiteral("height")).toDouble(174.0)); card->setSelected(object.value(QStringLiteral("selected")).toBool(false));
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
    if (!settingsPage_) buildSettingsPage();
    animateOverlay(settingsOverlay_, true);
    updateNavigationIndicators();
    settingsPage_->setFocus(Qt::OtherFocusReason);
}

void MainWindow::animateOverlay(QWidget *overlay, bool open)
{
    if (!overlay) return;
    overlay->setGraphicsEffect(nullptr);
    QWidget *card = overlay == settingsOverlay_ ? settingsPage_
        : overlay == sshExplorerOverlay_ ? sshExplorerCard_ : nullptr;
    if (card) card->setGraphicsEffect(nullptr);

    overlay->setGeometry(root_->rect());
    const QPoint targetPosition = card
        ? QPoint((overlay->width() - card->width()) / 2, (overlay->height() - card->height()) / 2)
        : overlay->pos();
    if (open) {
        overlay->show();
        overlay->raise();
        if (card) {
            card->move(targetPosition + QPoint(0, 9));
            card->raise();
        }
    } else if (!overlay->isVisible()) {
        return;
    }

    QWidget *animatedWidget = card ? card : overlay;
    auto *animation = new QPropertyAnimation(animatedWidget, "pos", overlay);
    animation->setDuration(180);
    animation->setEasingCurve(QEasingCurve::OutCubic);
    if (open) {
        animation->setStartValue(animatedWidget->pos());
        animation->setEndValue(targetPosition);
    } else {
        animation->setStartValue(animatedWidget->pos());
        animation->setEndValue(targetPosition + QPoint(0, 7));
    }
    connect(animation, &QPropertyAnimation::finished, overlay,
            [this, overlay, card, animatedWidget, targetPosition, open] {
        animatedWidget->move(targetPosition);
        if (!open) overlay->hide();
        overlay->update();
        for (QWidget *child : overlay->findChildren<QWidget *>()) child->update();
        if (card) card->update();
        if (overlay == settingsOverlay_) updateNavigationIndicators();
    });
    animation->start(QAbstractAnimation::DeleteWhenStopped);
}

void MainWindow::buildSettingsPage()
{
    settingsOverlay_ = new QWidget(root_);
    settingsOverlay_->setObjectName(QStringLiteral("glassSettingsOverlay"));
    settingsOverlay_->setStyleSheet(QStringLiteral("QWidget#glassSettingsOverlay{background:rgba(8,12,18,0.55);}"));
    auto *overlayLayout = new QVBoxLayout(settingsOverlay_);
    overlayLayout->setContentsMargins(20, 20, 20, 20);
    overlayLayout->addStretch(1);
    auto *settingsRow = new QHBoxLayout;
    settingsRow->addStretch(1);
    settingsPage_ = new QWidget(settingsOverlay_);
    settingsPage_->setObjectName(QStringLiteral("settingsPage"));
    settingsPage_->setFixedSize(740, 520);
    settingsRow->addWidget(settingsPage_, 1);
    settingsRow->addStretch(1);
    overlayLayout->addLayout(settingsRow, 1);
    overlayLayout->addStretch(1);
    settingsOverlay_->installEventFilter(this);
    auto *pageLayout = new QHBoxLayout(settingsPage_);
    pageLayout->setContentsMargins(8, 8, 8, 8);
    pageLayout->setSpacing(0);
    auto *settingsSidebar = new QWidget(settingsPage_);
    settingsSidebar->setObjectName(QStringLiteral("settingsSidebar"));
    settingsSidebar->setFixedWidth(190);
    auto *sidebarLayout = new QVBoxLayout(settingsSidebar);
    sidebarLayout->setContentsMargins(10, 12, 10, 12);
    sidebarLayout->setSpacing(8);
    auto *search = new QLineEdit(settingsSidebar);
    search->setObjectName(QStringLiteral("settingsSearch"));
    search->setPlaceholderText(QStringLiteral("Поиск настроек"));
    auto *appearanceNav = button(QStringLiteral("Внешний вид"), settingsSidebar);
    auto *terminalNav = button(QStringLiteral("Терминал"), settingsSidebar);
    auto *effectsNav = button(QStringLiteral("Стекло и эффекты"), settingsSidebar);
    auto *notesNav = button(QStringLiteral("Заметки"), settingsSidebar);
    settingsCloseButton_ = button(QString(), settingsPage_);
    settingsCloseButton_->setObjectName(QStringLiteral("settingsClose"));
    settingsCloseButton_->setFixedSize(13, 13);
    settingsCloseButton_->setToolTip(QStringLiteral("Закрыть настройки"));
    settingsCloseButton_->setStyleSheet(QStringLiteral("QPushButton#settingsClose{min-height:13px;max-width:13px;min-width:13px;background:#FF5F57;color:transparent;border:0;border-radius:6px;padding:0;}QPushButton#settingsClose:hover{color:#4D0000;font-weight:700;}"));
    settingsCloseButton_->move(settingsPage_->width() - 16 - settingsCloseButton_->width(), 16);
    settingsCloseButton_->installEventFilter(this);
    auto *sidebarTop = new QHBoxLayout;
    sidebarTop->addWidget(new QLabel(QStringLiteral("Настройки"), settingsSidebar));
    sidebarTop->addStretch(1);
    sidebarLayout->addLayout(sidebarTop);
    for (QPushButton *item : {appearanceNav, terminalNav, effectsNav, notesNav}) {
        item->setCheckable(true);
        item->setMinimumHeight(36);
        item->setStyleSheet(QStringLiteral("QPushButton{text-align:left;background:transparent;border:0;padding:6px 10px;outline:none;}QPushButton:focus{outline:none;}QPushButton:hover{background:rgba(255,255,255,0.08);}QPushButton:checked{background:rgba(255,255,255,0.13);border:1px solid rgba(255,255,255,0.08);}"));
        sidebarLayout->addWidget(item);
    }
    sidebarLayout->insertWidget(0, search);
    sidebarLayout->addStretch(1);
    auto *scroll = new QScrollArea(settingsPage_);
    scroll->setWidgetResizable(true);
    scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    auto *content = new QWidget(scroll);
    auto *layout = new QVBoxLayout(content);
    layout->setContentsMargins(12, 8, 12, 8);
    layout->setSpacing(6);
    scroll->setWidget(content);
    pageLayout->addWidget(settingsSidebar);
    pageLayout->addWidget(scroll, 1);
    auto makeSection = [content, layout](const QString &title) {
        auto *section = new QFrame(content);
        section->setObjectName(QStringLiteral("settingsSection"));
        auto *sectionLayout = new QVBoxLayout(section);
        sectionLayout->setContentsMargins(2, 2, 2, 2);
        sectionLayout->setSpacing(5);
        auto *heading = new QLabel(title, section);
        heading->setStyleSheet(QStringLiteral("font-weight:600;"));
        sectionLayout->addWidget(heading);
        layout->addWidget(section);
        return sectionLayout;
    };
    auto *appearanceLayout = makeSection(QStringLiteral("Внешний вид"));
    auto *fontLayoutSection = makeSection(QStringLiteral("Шрифт и курсор"));
    auto *scaleLayoutSection = makeSection(QStringLiteral("Прозрачность и масштаб"));
    auto *notesLayoutSection = makeSection(QString());
    auto *appearanceSection = appearanceLayout->parentWidget();
    auto *terminalSection = fontLayoutSection->parentWidget();
    auto *effectsSection = scaleLayoutSection->parentWidget();
    auto *notesSection = notesLayoutSection->parentWidget();
    auto *appearancePreview = new LivePreviewWidget(LivePreviewWidget::Kind::Appearance, appearanceSection);
    appearancePreview->setObjectName(QStringLiteral("appearanceLivePreview"));
    auto *terminalPreview = new LivePreviewWidget(LivePreviewWidget::Kind::Terminal, terminalSection);
    terminalPreview->setObjectName(QStringLiteral("terminalLivePreview"));
    auto *effectsPreview = new LivePreviewWidget(LivePreviewWidget::Kind::Glass, effectsSection);
    effectsPreview->setObjectName(QStringLiteral("effectsLivePreview"));
    auto *notesPreview = new LivePreviewWidget(LivePreviewWidget::Kind::Notes, notesSection);
    notesPreview->setObjectName(QStringLiteral("notesLivePreview"));
    notesPreview->setValues(theme_, accent_, cursorStyle_, cursorColor_, fontSize_, consoleAlpha_, windowOpacity_);
    notesPreview->setNotesPreview(noteCardSizePreset_, notesZoomOutPercent_);
    appearanceLayout->insertWidget(0, appearancePreview);
    fontLayoutSection->insertWidget(0, terminalPreview);
    scaleLayoutSection->insertWidget(0, effectsPreview);
    notesLayoutSection->insertWidget(0, notesPreview);
    auto *zoomRow = new QWidget(notesSection);
    auto *zoomLayout = new QVBoxLayout(zoomRow);
    zoomLayout->setContentsMargins(8, 5, 8, 5);
    auto *zoomLabel = new QLabel(QStringLiteral("Максимальное отдаление холста (Zoom Out): %1%").arg(notesZoomOutPercent_), zoomRow);
    auto *zoomSlider = new AppleSlider(Qt::Horizontal, zoomRow);
    zoomSlider->setRange(10, 50);
    zoomSlider->setValue(notesZoomOutPercent_);
    zoomLayout->addWidget(zoomLabel);
    zoomLayout->addWidget(zoomSlider);
    notesLayoutSection->addWidget(zoomRow);
    auto *sizeRow = new QWidget(notesSection);
    auto *sizeLayout = new QHBoxLayout(sizeRow);
    sizeLayout->setContentsMargins(8, 5, 8, 5);
    sizeLayout->addWidget(new QLabel(QStringLiteral("Базовый размер плашек заметок"), sizeRow));
    auto *sizeCombo = new QComboBox(sizeRow);
    sizeCombo->addItems({QStringLiteral("Компактный"), QStringLiteral("Стандартный"), QStringLiteral("Крупный")});
    sizeCombo->setCurrentIndex(qBound(0, noteCardSizePreset_, 2));
    sizeLayout->addWidget(sizeCombo);
    notesLayoutSection->addWidget(sizeRow);

    QSettings selStore(QString::fromLatin1(kSettingsOrg), QString::fromLatin1(kSettingsApp));
    s_noteSelBorderColor = selStore.value(QStringLiteral("noteSelBorderColor"), QStringLiteral("#ffffff")).toString();
    s_noteSelTextColor = selStore.value(QStringLiteral("noteSelTextColor"), QStringLiteral("#ffffff")).toString();

    auto *selColorsCard = new QFrame(content);
    selColorsCard->setProperty("searchText", QStringLiteral("выделение цвет рамки текста заметки selection border text color"));
    auto *selColorsLay = new QVBoxLayout(selColorsCard);
    selColorsLay->setSpacing(10);

    auto makeColorPickerRow = [&](const QString &labelText, const QString &settingKey, QString *targetVar) {
        auto *row = new QHBoxLayout;
        row->addWidget(new QLabel(labelText, selColorsCard));
        row->addStretch(1);
        const QList<QPair<QString, QString>> swatches = {
            {QStringLiteral("Белый"), QStringLiteral("#ffffff")},
            {QStringLiteral("Голубой"), QStringLiteral("#7eb8da")},
            {QStringLiteral("Мятный"), QStringLiteral("#71d6a3")},
            {QStringLiteral("Янтарный"), QStringLiteral("#f5b971")},
            {QStringLiteral("Коралл"), QStringLiteral("#f28b82")},
            {QStringLiteral("Сирень"), QStringLiteral("#c4b5fd")}
        };
        auto *grp = new QButtonGroup(selColorsCard);
        grp->setExclusive(true);
        for (const auto &sw : swatches) {
            auto *btn = new QPushButton(selColorsCard);
            btn->setCheckable(true);
            btn->setFixedSize(22, 22);
            btn->setCursor(Qt::PointingHandCursor);
            btn->setToolTip(sw.first);
            btn->setChecked(*targetVar == sw.second);
            btn->setStyleSheet(QStringLiteral("QPushButton{background:%1;border:2px solid rgba(255,255,255,0.22);border-radius:11px;padding:0;min-height:22px;max-width:22px;}QPushButton:checked{border:2.5px solid #ffffff;}").arg(sw.second));
            const QString hex = sw.second;
            connect(btn, &QPushButton::clicked, this, [this, hex, settingKey, targetVar, notesPreview] {
                *targetVar = hex;
                QSettings st(QString::fromLatin1(kSettingsOrg), QString::fromLatin1(kSettingsApp));
                st.setValue(settingKey, hex);
                if (noteScene_) noteScene_->update();
                if (notesPreview) notesPreview->update();
            });
            grp->addButton(btn);
            row->addWidget(btn);
        }
        selColorsLay->addLayout(row);
    };

    makeColorPickerRow(QStringLiteral("Цвет рамки при выделении"), QStringLiteral("noteSelBorderColor"), &s_noteSelBorderColor);
    makeColorPickerRow(QStringLiteral("Цвет текста при выделении"), QStringLiteral("noteSelTextColor"), &s_noteSelTextColor);
    notesLayoutSection->addWidget(selColorsCard);
    connect(zoomSlider, &QSlider::valueChanged, this, [this, zoomLabel, notesPreview](int value) {
        notesZoomOutPercent_ = value;
        zoomLabel->setText(QStringLiteral("Максимальное отдаление холста (Zoom Out): %1%").arg(value));
        if (auto *view = dynamic_cast<NotesView *>(noteView_)) view->setZoomOutLimit(value);
        notesPreview->setNotesPreview(noteCardSizePreset_, value);
        saveSettings();
    });
    connect(sizeCombo, qOverload<int>(&QComboBox::currentIndexChanged), this, [this, notesPreview](int value) {
        noteCardSizePreset_ = value;
        notesPreview->setNotesPreview(value, notesZoomOutPercent_);
        for (QGraphicsItem *item : noteScene_->items())
            if (auto *card = dynamic_cast<NoteCard *>(item)) card->setBasePreset(value);
        saveNotes(); saveSettings();
    });
    auto updatePreviewFunction = std::make_shared<std::function<void()>>();
    settingsPage_->setProperty("settingsCategory", 0);
    appearanceNav->setChecked(true);
    terminalSection->hide();
    effectsSection->hide();
    notesSection->hide();
    auto selectCategory = [this, appearanceNav, terminalNav, effectsNav, notesNav, appearanceSection, terminalSection, effectsSection, notesSection](int category) {
        settingsPage_->setProperty("settingsCategory", category);
        appearanceNav->setChecked(category == 0);
        terminalNav->setChecked(category == 1);
        effectsNav->setChecked(category == 2);
        notesNav->setChecked(category == 3);
        appearanceSection->setVisible(category == 0);
        terminalSection->setVisible(category == 1);
        effectsSection->setVisible(category == 2);
        notesSection->setVisible(category == 3);
    };
    connect(appearanceNav, &QPushButton::clicked, this, [selectCategory] { selectCategory(0); });
    connect(terminalNav, &QPushButton::clicked, this, [selectCategory] { selectCategory(1); });
    connect(effectsNav, &QPushButton::clicked, this, [selectCategory] { selectCategory(2); });
    connect(notesNav, &QPushButton::clicked, this, [selectCategory] { selectCategory(3); });
    connect(settingsCloseButton_, &QPushButton::clicked, this, [this] { animateOverlay(settingsOverlay_, false); });
    auto *cursorCard = new QFrame(content);
    cursorCard->setProperty("searchText", QStringLiteral("курсор терминала стиль цвет блок подчёркивание черта cursor"));
    auto *cursorLayout = new QVBoxLayout(cursorCard);
    cursorLayout->addWidget(new QLabel(QStringLiteral("Курсор терминала"), cursorCard));
    auto *cursorRow = new QHBoxLayout;
    auto *cursorStyleCombo = new AppleSegmentedControl({QStringLiteral("Блок"),QStringLiteral("Подчёркивание"),QStringLiteral("Черта")},0,cursorCard);
    auto *cursorColorCombo = new QComboBox(cursorCard);
    cursorColorCombo->addItem(QStringLiteral("Сталь"), QStringLiteral("#7eb8da"));
    cursorColorCombo->addItem(QStringLiteral("Белый"), QStringLiteral("#f8fafc"));
    cursorColorCombo->addItem(QStringLiteral("Зелёный"), QStringLiteral("#5ecf9a"));
    cursorColorCombo->addItem(QStringLiteral("Голубой"), QStringLiteral("#39c5cf"));
    cursorColorCombo->addItem(QStringLiteral("Янтарный"), QStringLiteral("#e0a45a"));
    cursorColorCombo->addItem(QStringLiteral("Коралл"), QStringLiteral("#e06b63"));
    cursorStyleCombo->setCurrentData(cursorStyle_);
    cursorColorCombo->setCurrentIndex(qMax(0, cursorColorCombo->findData(cursorColor_)));
    auto saveCursorPrefs = [this, cursorStyleCombo, cursorColorCombo] {
        cursorStyle_ = cursorStyleCombo->currentData();
        cursorColor_ = cursorColorCombo->currentData().toString();
        saveSettings();
    };
    connect(cursorStyleCombo, &AppleSegmentedControl::currentIndexChanged, settingsPage_, [saveCursorPrefs, updatePreviewFunction](int){ saveCursorPrefs(); if (*updatePreviewFunction) (*updatePreviewFunction)(); });
    connect(cursorColorCombo, qOverload<int>(&QComboBox::currentIndexChanged), settingsPage_, [saveCursorPrefs, updatePreviewFunction](int){ saveCursorPrefs(); if (*updatePreviewFunction) (*updatePreviewFunction)(); });
    cursorRow->addWidget(cursorStyleCombo, 1);
    cursorRow->addWidget(cursorColorCombo, 1);
    cursorLayout->addLayout(cursorRow);
    fontLayoutSection->addWidget(cursorCard);

    auto *themeCard = new QFrame(content);
    themeCard->setProperty("searchText", QStringLiteral("тема интерфейса глубокий космос deep space графит graphite обсидиан obsidian"));
    auto *themeLayout = new QVBoxLayout(themeCard);
    auto *themeLabel = new QLabel(QStringLiteral("Тема интерфейса"), themeCard);
    auto *themeCombo = new QComboBox(themeCard);
    const QStringList paletteNames{
        QStringLiteral("Графит Pro"),
        QStringLiteral("Обсидиан"),
        QStringLiteral("Изумруд"),
        QStringLiteral("Морской Неон"),
        QStringLiteral("Янтарный"),
        QStringLiteral("Север"),
        QStringLiteral("Марс"),
        QStringLiteral("Глубокий Космос")};
    const QStringList paletteKeys{
        QStringLiteral("Graphite Pro"),
        QStringLiteral("Obsidian"),
        QStringLiteral("Emerald Matrix"),
        QStringLiteral("Cyberpunk Neon"),
        QStringLiteral("Amber Sunset"),
        QStringLiteral("Northern Lights"),
        QStringLiteral("Red Mars"),
        QStringLiteral("Deep Space")};
    const QStringList swatches{
        QStringLiteral("#7eb8da"),
        QStringLiteral("#d7dde5"),
        QStringLiteral("#5ecf9a"),
        QStringLiteral("#39c5cf"),
        QStringLiteral("#e0a45a"),
        QStringLiteral("#5bc4b8"),
        QStringLiteral("#e06b63"),
        QStringLiteral("#8aa4c8")};
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
        connect(tile, &QPushButton::clicked, settingsPage_, [this, themeCombo, tile, paletteGrid, i] {
            if (themeCombo->currentIndex() != i) themeCombo->setCurrentIndex(i);
            else applySettings(themeAccent(themeCombo->itemData(i).toString()), consoleAlpha_, fontSize_, uiScale_, themeCombo->itemData(i).toString(), starsEnabled_, windowOpacity_);
            for (int j = 0; j < paletteGrid->count(); ++j) if (auto *other = qobject_cast<QPushButton *>(paletteGrid->itemAt(j)->widget())) other->setChecked(other == tile);
        });
    }
    themeLayout->addLayout(paletteGrid);
    appearanceLayout->addWidget(themeCard);
    auto *starsRow = new QWidget(content);
    auto *starsLayout = new QHBoxLayout(starsRow); starsLayout->setContentsMargins(10,4,10,4);
    starsLayout->addWidget(new QLabel(QStringLiteral("Анимация звёзд и метеоров"),starsRow)); starsLayout->addStretch(1);
    auto *starsToggle = new AppleToggleSwitch(starsRow); starsLayout->addWidget(starsToggle);
    starsRow->setProperty("searchText", QStringLiteral("анимация звёзд stars метеоры meteors фон background"));
    starsToggle->setChecked(starsEnabled_);
    appearanceLayout->addWidget(starsRow);
    auto *accentCard = new QFrame(content);
    accentCard->setProperty("searchText", QStringLiteral("акцентный цвет палитра accent color"));
    auto *accentLayout = new QVBoxLayout(accentCard);
    auto *accentRow = new QHBoxLayout;
    auto *accentLabel = new QLabel(QStringLiteral("Акцентный цвет"), accentCard);
    auto *colorButton = button(QStringLiteral("Выбрать цвет"), accentCard);
    accentRow->addWidget(accentLabel);
    accentRow->addStretch();
    accentRow->addWidget(colorButton);
    accentLayout->addLayout(accentRow);
    appearanceLayout->addWidget(accentCard);
    auto selectedAccent = std::make_shared<QString>(accent_);
    connect(colorButton, &QPushButton::clicked, settingsPage_, [this, selectedAccent, updatePreviewFunction] {
        const QColor selected = QColorDialog::getColor(QColor(*selectedAccent), settingsPage_, QStringLiteral("Акцентный цвет"));
        if (selected.isValid()) { *selectedAccent = selected.name(); if (*updatePreviewFunction) (*updatePreviewFunction)(); }
    });
    auto *windowOpacityCard = new QFrame(content);
    windowOpacityCard->setProperty("searchText", QStringLiteral("общая прозрачность окна opacity window"));
    auto *windowOpacityLayout = new QVBoxLayout(windowOpacityCard);
    auto *windowOpacityLabel = new QLabel(QStringLiteral("Общая прозрачность окна: %1%").arg(windowOpacity_), windowOpacityCard);
    auto *windowOpacitySlider = new AppleSlider(Qt::Horizontal, windowOpacityCard);
    windowOpacitySlider->setRange(40, 100);
    windowOpacitySlider->setValue(windowOpacity_);
    windowOpacityLayout->addWidget(windowOpacityLabel);
    windowOpacityLayout->addWidget(windowOpacitySlider);
    scaleLayoutSection->addWidget(windowOpacityCard);
    connect(windowOpacitySlider, &QSlider::valueChanged, this, [this, windowOpacityLabel, updatePreviewFunction](int value) { windowOpacity_ = value; setWindowOpacity(value / 100.0); windowOpacityLabel->setText(QStringLiteral("Общая прозрачность окна: %1%").arg(value)); saveSettings(); if (*updatePreviewFunction) (*updatePreviewFunction)(); });
    auto *alphaCard = new QFrame(content);
    alphaCard->setProperty("searchText", QStringLiteral("прозрачность подложки консоли альфа transparency opacity alpha"));
    auto *alphaLayout = new QVBoxLayout(alphaCard);
    auto *alphaLabel = new QLabel(QStringLiteral("Прозрачность подложки консоли: %1").arg(consoleAlpha_), alphaCard);
    auto *alpha = new AppleSlider(Qt::Horizontal, alphaCard);
    alpha->setRange(120, 245);
    alpha->setValue(consoleAlpha_);
    alphaLayout->addWidget(alphaLabel);
    alphaLayout->addWidget(alpha);
    connect(alpha, &QSlider::valueChanged, settingsPage_, [this, alphaLabel, updatePreviewFunction](int value) { consoleAlpha_ = value; for (ConsoleSession *session : sessions_) { if (auto *card = session->page->findChild<QFrame *>(QStringLiteral("consoleCard"))) card->setStyleSheet(QStringLiteral("QFrame#consoleCard{background:%1;border:1px solid rgba(255,255,255,0.1);border-radius:16px;}").arg(rgba(consoleBase(theme_), value))); } alphaLabel->setText(QStringLiteral("Прозрачность подложки консоли: %1").arg(value)); saveSettings(); if (*updatePreviewFunction) (*updatePreviewFunction)(); });
    scaleLayoutSection->addWidget(alphaCard);
    auto *fontCard = new QFrame(content);
    fontCard->setProperty("searchText", QStringLiteral("размер шрифта текст font size"));
    auto *fontLayout = new QVBoxLayout(fontCard);
    auto *fontRow = new QHBoxLayout;
    fontRow->addWidget(new QLabel(QStringLiteral("Размер шрифта"), fontCard));
    auto *fontSize = new AppleStepper(fontSize_,10,24,fontCard);
    fontRow->addWidget(fontSize);
    fontLayout->addLayout(fontRow);
        auto *fontFamilyHeader = new QHBoxLayout;
    fontFamilyHeader->addWidget(new QLabel(QStringLiteral("Шрифт терминала"), fontCard));
    auto *currentFontBadge = new QLabel(fontCard);
    currentFontBadge->setStyleSheet(QStringLiteral("color:rgba(235,240,248,0.65);font-size:12px;"));
    fontFamilyHeader->addStretch(1);
    fontFamilyHeader->addWidget(currentFontBadge);
    fontLayout->addLayout(fontFamilyHeader);

    auto *fontListBox = new QWidget(fontCard);
    auto *fontListLayout = new QVBoxLayout(fontListBox);
    fontListLayout->setContentsMargins(0, 4, 0, 0);
    fontListLayout->setSpacing(4);
    fontLayout->addWidget(fontListBox);

    auto rebuildFontPicker = std::make_shared<std::function<void()>>();
    *rebuildFontPicker = [this, fontListBox, fontListLayout, currentFontBadge, updatePreviewFunction, rebuildFontPicker]() {
        while (QLayoutItem *it = fontListLayout->takeAt(0)) {
            if (it->widget()) it->widget()->deleteLater();
            delete it;
        }
        QSettings st(QString::fromLatin1(kSettingsOrg), QString::fromLatin1(kSettingsApp));
        const QString baseFont = QStringLiteral("Consolas");
        QString activeFont = st.value(QStringLiteral("terminalFontFamily"), baseFont).toString();
        if (activeFont.isEmpty()) activeFont = baseFont;
        QStringList pinned = st.value(QStringLiteral("pinnedTerminalFonts")).toStringList();
        currentFontBadge->setText(activeFont);

        const QStringList all15 = {
            QStringLiteral("JetBrains Mono"), QStringLiteral("Fira Code"), QStringLiteral("Cascadia Code"),
            QStringLiteral("SF Mono"), QStringLiteral("Menlo"), QStringLiteral("Source Code Pro"),
            QStringLiteral("IBM Plex Mono"), QStringLiteral("Ubuntu Mono"), QStringLiteral("Hack"),
            QStringLiteral("Inconsolata"), QStringLiteral("DejaVu Sans Mono"), QStringLiteral("Roboto Mono"),
            QStringLiteral("Courier New"), QStringLiteral("Lucida Console"), QStringLiteral("Monaco")
        };

        auto applyFontEverywhere = [this, updatePreviewFunction, rebuildFontPicker](const QString &fam) {
            QSettings s(QString::fromLatin1(kSettingsOrg), QString::fromLatin1(kSettingsApp));
            s.setValue(QStringLiteral("terminalFontFamily"), fam);
            for (ConsoleSession *sess : sessions_) {
                if (sess && sess->output) {
                    QFont f(fam);
                    f.setStyleHint(QFont::Monospace);
                    f.setPixelSize(fontSize_);
                    sess->output->setFont(f);
                    sess->output->update();
                }
            }
            if (*updatePreviewFunction) (*updatePreviewFunction)();
            QTimer::singleShot(0, settingsPage_, [rebuildFontPicker] { if (*rebuildFontPicker) (*rebuildFontPicker)(); });
        };

        auto addFontRow = [&](const QString &fam, bool isBase, bool isPinned) {
            auto *row = new QFrame(fontListBox);
            const bool selected = (fam == activeFont);
            row->setStyleSheet(selected
                ? QStringLiteral("QFrame{background:rgba(126,184,218,0.18);border:1px solid rgba(126,184,218,0.45);border-radius:9px;}")
                : QStringLiteral("QFrame{background:rgba(255,255,255,0.035);border:1px solid rgba(255,255,255,0.07);border-radius:9px;}QFrame:hover{background:rgba(255,255,255,0.08);}"));
            auto *rl = new QHBoxLayout(row);
            rl->setContentsMargins(10, 5, 8, 5);
            rl->setSpacing(8);

            auto *pickBtn = new QPushButton(isBase ? QStringLiteral("Базовый (%1)").arg(fam) : fam, row);
            pickBtn->setCursor(Qt::PointingHandCursor);
            QFont sampleFont(fam);
            sampleFont.setStyleHint(QFont::Monospace);
            sampleFont.setPixelSize(13);
            pickBtn->setFont(sampleFont);
            pickBtn->setStyleSheet(QStringLiteral("QPushButton{background:transparent;border:0;text-align:left;color:#f3f5f8;padding:2px 0;}"));
            rl->addWidget(pickBtn, 1);

            if (!isBase) {
                auto *pinBtn = new QPushButton(isPinned ? QStringLiteral("📌 Закреплено") : QStringLiteral("📌"), row);
                pinBtn->setCursor(Qt::PointingHandCursor);
                pinBtn->setToolTip(isPinned ? QStringLiteral("Открепить шрифт") : QStringLiteral("Закрепить шрифт наверху"));
                pinBtn->setStyleSheet(isPinned
                    ? QStringLiteral("QPushButton{background:rgba(126,184,218,0.22);border:0;border-radius:6px;padding:3px 8px;color:#bfe3f7;font-size:11.5px;}")
                    : QStringLiteral("QPushButton{background:rgba(255,255,255,0.06);border:0;border-radius:6px;padding:3px 8px;color:rgba(235,240,248,0.65);font-size:11.5px;}QPushButton:hover{background:rgba(255,255,255,0.14);color:#fff;}"));
                connect(pinBtn, &QPushButton::clicked, this, [this, fam, rebuildFontPicker]() {
                    QSettings s(QString::fromLatin1(kSettingsOrg), QString::fromLatin1(kSettingsApp));
                    QStringList p = s.value(QStringLiteral("pinnedTerminalFonts")).toStringList();
                    if (p.contains(fam)) p.removeAll(fam); else p.prepend(fam);
                    s.setValue(QStringLiteral("pinnedTerminalFonts"), p);
                    QTimer::singleShot(0, settingsPage_, [rebuildFontPicker] { if (*rebuildFontPicker) (*rebuildFontPicker)(); });
                });
                rl->addWidget(pinBtn);
            }
            fontListLayout->addWidget(row);
            return pickBtn;
        };

        auto *baseBtn = addFontRow(baseFont, true, false);
        connect(baseBtn, &QPushButton::clicked, this, [applyFontEverywhere, baseFont] { applyFontEverywhere(baseFont); });

        auto *divider = new QFrame(fontListBox);
        divider->setFrameShape(QFrame::HLine);
        divider->setFixedHeight(1);
        divider->setStyleSheet(QStringLiteral("background:rgba(255,255,255,0.16);border:0;margin:4px 2px;"));
        fontListLayout->addWidget(divider);

        QStringList ordered;
        for (const QString &p : pinned) if (all15.contains(p)) ordered.append(p);
        for (const QString &f : all15) if (!ordered.contains(f)) ordered.append(f);
        for (const QString &fam : ordered) {
            auto *btn = addFontRow(fam, false, pinned.contains(fam));
            connect(btn, &QPushButton::clicked, this, [applyFontEverywhere, fam] { applyFontEverywhere(fam); });
        }
    };
    (*rebuildFontPicker)();

    fontLayoutSection->addWidget(fontCard);
    connect(fontSize, &AppleStepper::valueChanged, settingsPage_, [updatePreviewFunction](int){ if (*updatePreviewFunction) (*updatePreviewFunction)(); });
    auto *scaleCard = new QFrame(content);
    scaleCard->setProperty("searchText", QStringLiteral("масштаб интерфейса размер scale ui"));
    auto *scaleLayout = new QVBoxLayout(scaleCard);
    auto *scaleLabel = new QLabel(QStringLiteral("Масштаб интерфейса"), scaleCard);
    auto *scale = new AppleSlider(Qt::Horizontal, scaleCard);
    scale->setRange(80, 125);
    scale->setValue(uiScale_);
    scaleLayout->addWidget(scaleLabel);
    scaleLayout->addWidget(scale);
    scaleLayoutSection->addWidget(scaleCard);
    auto *audioCard = new QFrame(content);
    audioCard->setProperty("searchText", QStringLiteral("звук динамик аудио устройство вывод speaker audio output"));
    auto *audioLayout = new QVBoxLayout(audioCard);
    audioLayout->addWidget(new QLabel(QStringLiteral("Устройство вывода звука (динамик)"), audioCard));
    auto *speakerCombo = new QComboBox(audioCard);
    speakerCombo->addItem(QStringLiteral("Системное устройство по умолчанию"), -1);
#ifdef Q_OS_WIN
    const UINT numDevs = waveOutGetNumDevs();
    for (UINT d = 0; d < numDevs; ++d) {
        WAVEOUTCAPSW caps{};
        if (waveOutGetDevCapsW(d, &caps, sizeof(caps)) == MMSYSERR_NOERROR)
            speakerCombo->addItem(QString::fromWCharArray(caps.szPname), int(d));
    }
#endif
    QSettings audioSt(QString::fromLatin1(kSettingsOrg), QString::fromLatin1(kSettingsApp));
    s_selectedAudioDeviceId = audioSt.value(QStringLiteral("audioOutputDevice"), -1).toInt();
    const int savedDevIdx = speakerCombo->findData(s_selectedAudioDeviceId);
    if (savedDevIdx >= 0) speakerCombo->setCurrentIndex(savedDevIdx);
    connect(speakerCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [speakerCombo](int idx) {
        s_selectedAudioDeviceId = speakerCombo->itemData(idx).toInt();
        QSettings st(QString::fromLatin1(kSettingsOrg), QString::fromLatin1(kSettingsApp));
        st.setValue(QStringLiteral("audioOutputDevice"), s_selectedAudioDeviceId);
    });
    auto *speakerRow = new QHBoxLayout;
    auto *testSoundBtn = button(QStringLiteral("Проверить"), audioCard);
    testSoundBtn->setFixedWidth(95);
    connect(testSoundBtn, &QPushButton::clicked, this, [] { MessageBeep(MB_OK); });
    speakerRow->addWidget(speakerCombo, 1);
    speakerRow->addWidget(testSoundBtn);
    audioLayout->addLayout(speakerRow);
    scaleLayoutSection->addWidget(audioCard);
    connect(scale, &QSlider::valueChanged, settingsPage_, [updatePreviewFunction](int){ if (*updatePreviewFunction) (*updatePreviewFunction)(); });
    connect(starsToggle, &QAbstractButton::toggled, settingsPage_, [updatePreviewFunction](bool){ if (*updatePreviewFunction) (*updatePreviewFunction)(); });
    connect(themeCombo, qOverload<int>(&QComboBox::currentIndexChanged), settingsPage_, [this, selectedAccent, themeCombo, alpha, fontSize, scale, starsToggle, windowOpacitySlider, paletteGrid, updatePreviewFunction](int index) {
        if (index < 0) return;
        const QString key = themeCombo->itemData(index).toString();
        const QString selected = themeAccent(key);
        *selectedAccent = selected;
        applySettings(selected, alpha->value(), fontSize->value(), scale->value(), key, starsToggle->isChecked(), windowOpacitySlider->value());
        for (int i = 0; i < paletteGrid->count(); ++i) if (auto *tile = qobject_cast<QPushButton *>(paletteGrid->itemAt(i)->widget())) tile->setChecked(i == index);
        if (*updatePreviewFunction) (*updatePreviewFunction)();
    });
    *updatePreviewFunction = [appearancePreview, terminalPreview, effectsPreview, themeCombo, selectedAccent,
                              cursorStyleCombo, cursorColorCombo, fontSize, alpha, windowOpacitySlider] {
        const QString theme = themeCombo->currentData().toString();
        const QString accent = selectedAccent->isEmpty() ? themeAccent(theme) : *selectedAccent;
        const QString style = cursorStyleCombo->currentData();
        const QString cursor = cursorColorCombo->currentData().toString();
        const int size = fontSize->value();
        const int opacity = windowOpacitySlider->value();
        const int consoleAlpha = alpha->value();
        appearancePreview->setValues(theme, accent, style, cursor, size, consoleAlpha, opacity);
        terminalPreview->setValues(theme, accent, style, cursor, size, consoleAlpha, opacity);
        effectsPreview->setValues(theme, accent, style, cursor, size, consoleAlpha, opacity);
    };
    (*updatePreviewFunction)();
    auto *saveSettingsButton = new QPushButton(QStringLiteral("Сохранить"), content);
    auto *backToTerminal = new QPushButton(QStringLiteral("← Терминал"), content);
    auto *actionsRow = new QHBoxLayout;
    actionsRow->addWidget(backToTerminal);
    actionsRow->addStretch();
    actionsRow->addWidget(saveSettingsButton);
    layout->addLayout(actionsRow);
    connect(backToTerminal, &QPushButton::clicked, settingsOverlay_, [this] { animateOverlay(settingsOverlay_, false); });
    const QString pageStyle = appStyle(accent_, consoleAlpha_, uiScale_, theme_);
    settingsPage_->setStyleSheet(pageStyle + QStringLiteral("QWidget#settingsPage{background:qlineargradient(x1:0,y1:0,x2:1,y2:1,stop:0 rgba(36,45,61,0.96),stop:1 rgba(20,27,39,0.96));border:1px solid rgba(255,255,255,0.16);border-radius:20px;}QWidget#settingsSidebar{background:rgba(12,16,23,0.46);border-right:1px solid rgba(255,255,255,0.08);border-top-left-radius:14px;border-bottom-left-radius:14px;}QFrame#settingsSection{background:transparent;border:0;}QFrame[searchText]{background:rgba(255,255,255,0.045);border:0;border-bottom:1px solid rgba(255,255,255,0.08);border-radius:9px;}QSpinBox{min-height:36px;padding:0px 6px;}QSpinBox::up-button,QSpinBox::down-button{width:18px;}QLineEdit#settingsSearch{background:rgba(255,255,255,0.06);}"));
    const QList<QWidget *> appearanceItems{themeCard, starsRow, accentCard};
    const QList<QWidget *> terminalItems{cursorCard, fontCard};
    const QList<QWidget *> effectsItems{windowOpacityCard, alphaCard, scaleCard};
    auto matchesSearch = [search](QWidget *widget, const QString &needle) {
        return needle.isEmpty() || widget->property("searchText").toString().contains(needle, Qt::CaseInsensitive);
    };
    connect(search, &QLineEdit::textChanged, settingsPage_, [this, search, matchesSearch, appearanceItems, terminalItems, effectsItems, appearanceSection, terminalSection, effectsSection](const QString &query) {
        const QString needle = query.trimmed();
        bool appearanceMatch = false, terminalMatch = false, effectsMatch = false;
        for (QWidget *widget : appearanceItems) { const bool match = matchesSearch(widget, needle); widget->setVisible(match); appearanceMatch |= match; }
        for (QWidget *widget : terminalItems) { const bool match = matchesSearch(widget, needle); widget->setVisible(match); terminalMatch |= match; }
        for (QWidget *widget : effectsItems) { const bool match = matchesSearch(widget, needle); widget->setVisible(match); effectsMatch |= match; }
        if (needle.isEmpty()) {
            const int category = settingsPage_->property("settingsCategory").toInt();
            appearanceSection->setVisible(category == 0);
            terminalSection->setVisible(category == 1);
            effectsSection->setVisible(category == 2);
        } else {
            appearanceSection->setVisible(appearanceMatch);
            terminalSection->setVisible(terminalMatch);
            effectsSection->setVisible(effectsMatch);
        }
    });
    const auto registerPreview = [](QWidget *card, const QString &previewId, const QString &hint) {
        card->setProperty("previewId", previewId);
        card->setProperty("previewHint", hint);
        for (QWidget *child : card->findChildren<QWidget *>()) {
            child->setProperty("previewId", previewId);
            child->setProperty("previewHint", hint);
        }
    };
    registerPreview(themeCard, QStringLiteral("appearanceLivePreview"), QStringLiteral("Тема интерфейса"));
    registerPreview(starsRow, QStringLiteral("appearanceLivePreview"), QStringLiteral("Фон и анимация"));
    registerPreview(accentCard, QStringLiteral("appearanceLivePreview"), QStringLiteral("Акцентный цвет"));
    registerPreview(cursorCard, QStringLiteral("terminalLivePreview"), QStringLiteral("Курсор и его цвет"));
    registerPreview(fontCard, QStringLiteral("terminalLivePreview"), QStringLiteral("Размер шрифта"));
    registerPreview(windowOpacityCard, QStringLiteral("effectsLivePreview"), QStringLiteral("Прозрачность окна"));
    registerPreview(alphaCard, QStringLiteral("effectsLivePreview"), QStringLiteral("Прозрачность консоли"));
    registerPreview(scaleCard, QStringLiteral("effectsLivePreview"), QStringLiteral("Масштаб интерфейса"));
    for (QWidget *child : settingsOverlay_->findChildren<QWidget *>()) child->installEventFilter(this);
    settingsOverlay_->hide();
    settingsOverlay_->setGraphicsEffect(nullptr);
    settingsPage_->setGraphicsEffect(nullptr);
    settingsCloseButton_->raise();
    connect(saveSettingsButton, &QPushButton::clicked, this, [this, selectedAccent, alpha, fontSize, scale, themeCombo, starsToggle, windowOpacitySlider, saveCursorPrefs] { saveCursorPrefs(); applySettings(*selectedAccent, alpha->value(), fontSize->value(), scale->value(), themeCombo->currentData().toString(), starsToggle->isChecked(), windowOpacitySlider->value()); animateOverlay(settingsOverlay_, false); });
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
    accent_ = accent.isEmpty() ? themeAccent(theme_) : accent;
    setWindowOpacity(windowOpacity_ / 100.0);
    setStyleSheet(appStyle(accent_, consoleAlpha_, uiScale_, theme_));
    root_->setAttribute(Qt::WA_TranslucentBackground, false);
    starfield_->setTheme(theme_);
    starfield_->setAnimated(starsEnabled_);
    starfield_->setStarsVisible(theme_ == QStringLiteral("Deep Space"));
    for (ConsoleSession *session : sessions_) {
        QFont font = session->output->font();
        font.setPointSize(fontSize_);
        session->output->setFont(font);
        session->output->setStyleSheet(QStringLiteral(
            "QWidget{background:transparent;border:0;color:#d7dae0;"
            "font-family:'Cascadia Code','JetBrains Mono','Consolas','DejaVu Sans Mono',monospace;font-size:%1px;}")
                                           .arg(QString::number(fontSize_)));
        if (auto *terminal = dynamic_cast<TerminalCanvas *>(session->output))
            { terminal->setTerminalCursor(cursorStyle_, QColor(cursorColor_)); terminal->refreshMetrics(); }
        if (auto *card = session->page->findChild<QFrame *>(QStringLiteral("consoleCard"))) {
            card->setStyleSheet(QStringLiteral("QFrame#consoleCard{background:%1;border:1px solid rgba(255,255,255,0.08);border-radius:12px;}")
                                    .arg(rgba(consoleBase(theme_), consoleAlpha_)));
        }
    }
    for (QGraphicsItem *item : noteScene_->items()) {
        if (auto *card = dynamic_cast<NoteCard *>(item)) { card->setAccent(accent_); card->setTheme(theme_); }
    }
    saveSettings();
    saveNotes();
}

void MainWindow::loadSettings()
{
    QSettings settings(QString::fromLatin1(kSettingsOrg), QString::fromLatin1(kSettingsApp));
    accent_ = settings.value(QStringLiteral("accent"), accent_).toString();
    consoleAlpha_ = settings.value(QStringLiteral("alpha"), consoleAlpha_).toInt();
    fontSize_ = settings.value(QStringLiteral("fontSize"), fontSize_).toInt();
    uiScale_ = settings.value(QStringLiteral("uiScale"), uiScale_).toInt();
    windowOpacity_ = settings.value(QStringLiteral("windowOpacity"), windowOpacity_).toInt();
    theme_ = settings.value(QStringLiteral("theme"), theme_).toString();
    if (theme_ == QStringLiteral("Graphite")) theme_ = QStringLiteral("Graphite Pro");
    starsEnabled_ = settings.value(QStringLiteral("starsEnabled"), starsEnabled_).toBool();
    notesZoomOutPercent_ = qBound(10, settings.value(QStringLiteral("notesZoomOutPercent"), notesZoomOutPercent_).toInt(), 50);
    noteCardSizePreset_ = qBound(0, settings.value(QStringLiteral("noteCardSizePreset"), noteCardSizePreset_).toInt(), 2);
    cursorStyle_ = settings.value(QStringLiteral("cursorStyle"), cursorStyle_).toString();
    cursorColor_ = settings.value(QStringLiteral("cursorColor"), cursorColor_).toString();
    const QVariantList stored = settings.value(QStringLiteral("binds")).toList();
    for (const QVariant &value : stored) {
        const QVariantMap map = value.toMap();
        binds_.append({map.value(QStringLiteral("name")).toString(), map.value(QStringLiteral("command")).toString(), nextBindId_++});
    }
}

void MainWindow::saveSettings() const
{
    QSettings settings(QString::fromLatin1(kSettingsOrg), QString::fromLatin1(kSettingsApp));
    settings.setValue(QStringLiteral("accent"), accent_);
    settings.setValue(QStringLiteral("alpha"), consoleAlpha_);
    settings.setValue(QStringLiteral("fontSize"), fontSize_);
    settings.setValue(QStringLiteral("uiScale"), uiScale_);
    settings.setValue(QStringLiteral("windowOpacity"), windowOpacity_);
    settings.setValue(QStringLiteral("theme"), theme_);
    settings.setValue(QStringLiteral("starsEnabled"), starsEnabled_);
    settings.setValue(QStringLiteral("notesZoomOutPercent"), notesZoomOutPercent_);
    settings.setValue(QStringLiteral("noteCardSizePreset"), noteCardSizePreset_);
    settings.setValue(QStringLiteral("cursorStyle"), cursorStyle_);
    settings.setValue(QStringLiteral("cursorColor"), cursorColor_);
    QVariantList values;
    for (const CommandBind &bind : binds_)
        values.append(QVariantMap{{QStringLiteral("name"), bind.name}, {QStringLiteral("command"), bind.command}});
    settings.setValue(QStringLiteral("binds"), values);
}

bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{
    QWidget *dragWidget = qobject_cast<QWidget *>(watched);
    const bool headerLabel = dragWidget && dragWidget->parentWidget()
        && dragWidget->parentWidget()->objectName() == QStringLiteral("topBar")
        && qobject_cast<QLabel *>(dragWidget);
    const bool dragZone = dragWidget && (dragWidget == nav_
        || dragWidget->objectName() == QStringLiteral("topBar")
        || dragWidget->objectName() == QStringLiteral("windowDragHandle")
        || dragWidget->objectName() == QStringLiteral("navDragSpace") || headerLabel);
    if (dragZone && event->type() == QEvent::MouseButtonPress) {
        auto *mouse = static_cast<QMouseEvent *>(event);
        if (mouse->button() == Qt::LeftButton) {
            if (windowHandle() && windowHandle()->startSystemMove()) return true;
            windowDragging_ = true;
            dragOffset_ = mouse->globalPosition().toPoint() - frameGeometry().topLeft();
            dragWidget->setCursor(Qt::ClosedHandCursor);
            dragWidget->grabMouse();
            return true;
        }
    }
    if (windowDragging_ && event->type() == QEvent::MouseMove) {
        auto *mouse = static_cast<QMouseEvent *>(event);
        if (mouse->buttons() & Qt::LeftButton) {
            move(mouse->globalPosition().toPoint() - dragOffset_);
            return true;
        }
    }
    if (windowDragging_ && event->type() == QEvent::MouseButtonRelease
        && static_cast<QMouseEvent *>(event)->button() == Qt::LeftButton) {
        windowDragging_ = false;
        if (dragWidget) { dragWidget->releaseMouse(); dragWidget->setCursor(Qt::OpenHandCursor); }
        return true;
    }
    if (watched == settingsCloseButton_ && (event->type() == QEvent::Enter || event->type() == QEvent::Leave))
        settingsCloseButton_->setText(event->type() == QEvent::Enter ? QStringLiteral("×") : QString());
    if (auto *widget = qobject_cast<QWidget *>(watched)) {
        const QString previewId = widget->property("previewId").toString();
        if (!previewId.isEmpty() && (event->type() == QEvent::Enter || event->type() == QEvent::Leave)) {
            if (auto *previewWidget = settingsPage_ ? settingsPage_->findChild<QWidget *>(previewId) : nullptr) {
                if (auto *preview = dynamic_cast<LivePreviewWidget *>(previewWidget))
                    preview->setHoverHint(event->type() == QEvent::Enter ? widget->property("previewHint").toString() : QString());
            }
        }
    }
    if(notesVisible_&&event->type()==QEvent::KeyPress
        &&static_cast<QKeyEvent *>(event)->key()==Qt::Key_Escape){
        QWidget *focused=qobject_cast<QWidget *>(watched);
        if(qobject_cast<QTextEdit *>(focused)||qobject_cast<QLineEdit *>(focused)){
            focused->clearFocus();
            if(noteView_)noteView_->setFocus(Qt::OtherFocusReason);
            return true;
        }
    }
    if (sshExplorerOverlay_ && sshExplorerOverlay_->isVisible()) {
        QWidget *widget = qobject_cast<QWidget *>(watched);
        const bool belongsToOverlay = widget && (widget == sshExplorerOverlay_ || sshExplorerOverlay_->isAncestorOf(widget));
        if (belongsToOverlay && event->type() == QEvent::KeyPress
            && static_cast<QKeyEvent *>(event)->key() == Qt::Key_Escape) {
            ConsoleSession *session = sshExplorerSession_;
            closeSshExplorer();
            if (session && session->output) session->output->setFocus(Qt::OtherFocusReason);
            return true;
        }
        if (belongsToOverlay && event->type() == QEvent::MouseButtonPress) {
            const QPoint pos = sshExplorerOverlay_->mapFromGlobal(static_cast<QMouseEvent *>(event)->globalPosition().toPoint());
            if (!sshExplorerCard_->geometry().contains(pos)) {
                ConsoleSession *session = sshExplorerSession_;
                closeSshExplorer();
                if (session && session->output) session->output->setFocus(Qt::OtherFocusReason);
                return true;
            }
        }
    }
    if (settingsOverlay_ && settingsOverlay_->isVisible()) {
        QWidget *widget = qobject_cast<QWidget *>(watched);
        const bool belongsToOverlay = widget && (widget == settingsOverlay_ || settingsOverlay_->isAncestorOf(widget));
        if (belongsToOverlay && event->type() == QEvent::KeyPress
            && static_cast<QKeyEvent *>(event)->key() == Qt::Key_Escape) {
            animateOverlay(settingsOverlay_, false);
            return true;
        }
        if (belongsToOverlay && event->type() == QEvent::MouseButtonPress && widget != settingsPage_) {
            const QPoint pos = settingsOverlay_->mapFromGlobal(static_cast<QMouseEvent *>(event)->globalPosition().toPoint());
            if (!settingsPage_->geometry().contains(pos)) {
                animateOverlay(settingsOverlay_, false);
                return true;
            }
        }
    }
    if (watched && (watched->objectName() == QStringLiteral("tabRow") || watched->objectName() == QStringLiteral("bindRow"))
        && (event->type() == QEvent::Enter || event->type() == QEvent::Leave)) {
        auto *row = qobject_cast<QWidget *>(watched);
        if (row && row->isEnabled() && !row->property("closing").toBool()) {
            const bool showActions = (event->type() == QEvent::Enter);
            for (QObject *child : row->children()) {
                if (auto *action = qobject_cast<QPushButton *>(child)) {
                    const QString objName = action->objectName();
                    if (objName == QStringLiteral("tabRename")
                        || objName == QStringLiteral("tabClose")
                        || objName == QStringLiteral("bindEdit")
                        || objName == QStringLiteral("bindRemove")) {
                        action->setVisible(showActions);
                    }
                }
            }
        }
    }
    if (event->type() == QEvent::Enter || event->type() == QEvent::Leave) {
        if (auto *control = qobject_cast<QPushButton *>(qobject_cast<QWidget *>(watched))) {
            const bool hovered = event->type() == QEvent::Enter;
            if (control->objectName() == QStringLiteral("trafficRed")) control->setText(hovered ? QStringLiteral("×") : QString());
            else if (control->objectName() == QStringLiteral("trafficYellow")) control->setText(hovered ? QStringLiteral("−") : QString());
            else if (control->objectName() == QStringLiteral("trafficGreen")) control->setText(hovered ? QStringLiteral("+") : QString());
        }
    }
    
    return QMainWindow::eventFilter(watched, event);
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
    if (starfield_ && root_) { starfield_->setGeometry(root_->rect()); starfield_->update(); }
    if (root_) root_->update();
    if (event->size().width() < 1050 && drawerOpen_) toggleDrawer();
    if (settingsOverlay_) {
        settingsOverlay_->setGeometry(root_->rect());
    }
    if (sshExplorerOverlay_) {
        sshExplorerOverlay_->setGeometry(root_->rect());
    }
}

void MainWindow::showEvent(QShowEvent *event)
{
    QMainWindow::showEvent(event);
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && event->position().y() <= 42) {
        QWidget *child = childAt(event->position().toPoint());
        if (!child || (!qobject_cast<QAbstractButton *>(child) && !qobject_cast<QLineEdit *>(child))) {
            windowDragging_ = true;
            dragOffset_ = event->globalPosition().toPoint() - frameGeometry().topLeft();
            event->accept();
            return;
        }
    }
    QMainWindow::mousePressEvent(event);
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    if (windowDragging_ && (event->buttons() & Qt::LeftButton)) {
        if (!isMaximized())
            move(event->globalPosition().toPoint() - dragOffset_);
        event->accept();
        return;
    }
    QMainWindow::mouseMoveEvent(event);
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && windowDragging_) {
        windowDragging_ = false;
        event->accept();
        return;
    }
    QMainWindow::mouseReleaseEvent(event);
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

#include "mainwindow.moc"

void MainWindow::navigateSftpParent()
{
    if (!sshExplorerSession_ || !sshExplorerPath_) return;
    QString current = sshExplorerPath_->text().trimmed();
    if (current.isEmpty()) current = sshExplorerSession_->remoteDirectory;
    if (current.isEmpty()) current = QStringLiteral("/");
    while (current.size() > 1 && current.endsWith(QLatin1Char('/')))
        current.chop(1);
    const int slash = current.lastIndexOf(QLatin1Char('/'));
    const QString parent = (slash <= 0) ? QStringLiteral("/") : current.left(slash);
    sshExplorerSession_->remoteDirectory = parent;
    sshExplorerPath_->setText(parent);
    refreshSshExplorer();
}
