#include <QApplication>
#include <QFile>
#include <QMessageBox>

#include "database.h"
#include "mainwindow.h"

int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
#endif
    QApplication app(argc, argv);
    QApplication::setOrganizationName(QStringLiteral("FashioNova"));
    QApplication::setApplicationName(QStringLiteral("FashioNova Machines"));

    QFile qss(QStringLiteral(":/style.qss"));
    if (qss.open(QIODevice::ReadOnly))
        app.setStyleSheet(QString::fromUtf8(qss.readAll()));

    QString error;
    if (!Database::open(&error)) {
        QMessageBox::critical(nullptr, QStringLiteral("Base de données"),
                              QStringLiteral("Impossible de se connecter à Oracle :\n%1\n\n"
                                             "Vérifiez le pilote Oracle ODBC 64 bits, le DSN FASHIONOVA_XE, "
                                             "le service Oracle et le compte FASHIONOVA.").arg(error));
        return 1;
    }

    MainWindow w;
    w.show();
    return app.exec();
}
