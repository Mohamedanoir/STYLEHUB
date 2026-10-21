#include "mainwindow.h"
#include "connection.h"
#include <QApplication>
#include <QMessageBox>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("Fashionova Orders");
    app.setStyle("Fusion");

    // Instanciation du Singleton et test de la connexion ODBC/Oracle
    Connection &c = Connection::createInstance();
    bool testConn = c.createConnection();

    MainWindow w;
    w.show();

    if (testConn) {
        QMessageBox::information(&w, "Base de données",
                                 "Connexion à la base de données établie avec succès !");
    } else {
        QMessageBox::warning(&w, "Base de données",
                             "Échec de connexion à la base de données.\n"
                             "Vérifiez que le service Oracle est démarré et que le DSN ODBC est bien configuré.");
    }

    return app.exec();
}