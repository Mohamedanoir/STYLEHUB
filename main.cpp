#include "mainwindow.h"
#include "connexion.h"
#include "login/loginwindow.h"

#include <QApplication>
#include <QPointer>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("Fashionova Orders");
    app.setStyle("Fusion");

    Connexion maquetteDb;
    maquetteDb.creerConnexion();

    // 1) Écran de connexion
    auto *login = new LoginWindow;
    login->setAttribute(Qt::WA_DeleteOnClose);

    // 2) Après « Se connecter » : ouverture de l'application (6 modules) puis fermeture du login
    QPointer<MainWindow> principale;
    QObject::connect(login, &LoginWindow::connecte, login, [&principale, login](const QString &, const QString &) {
        if (!principale) {
            principale = new MainWindow;
            principale->setAttribute(Qt::WA_DeleteOnClose);
        }
        principale->show();
        login->close();
    });

    login->show();
    return app.exec();
}
