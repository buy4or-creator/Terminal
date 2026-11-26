#include "mainwindow.h"

#include <QApplication>
#include <QFileInfo>
#include <cstdio>
#include <cstdlib>
#include <cstring>

int main(int argc, char *argv[])
{
    const bool askpassMode = QFileInfo(QString::fromLocal8Bit(argv[0])).completeBaseName() == QStringLiteral("terminal-ssh-askpass")
        || (argc > 1 && QByteArray(argv[1]) == QByteArrayLiteral("--ssh-askpass"));
    if (askpassMode) {
        const char *pass = std::getenv("TERMINAL_SFTP_PASS");
        if (!pass) pass = std::getenv("TERMINAL_SFTP_PASSWORD");
        std::printf("%s\n", pass ? pass : "");
        std::fflush(stdout);
        return 0;
    }
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return QApplication::exec();
}
