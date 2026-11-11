#include "connection.h"
#include <QMessageBox>
#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("HighFashionWorkshop");
    app.setOrganizationName("FashioNova");

    Connection &c = Connection::createInstance();
    bool test = c.createConnect();

    if (!test) {
        QMessageBox::critical(nullptr, "Base de données",
                              "Connexion échouée");
        return 1;
    }

    MainWindow w;   // créée APRÈS la connexion : le constructeur lit la table CLIENTS
    w.show();
    return app.exec();
}