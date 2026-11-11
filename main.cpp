#include "mainwindow.h"
#include "connexion.h"

#include <QApplication>
#include <QMessageBox>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setStyle("Fusion");

    // Connexion directe automatique sans afficher de boîte de saisie
    if (!Connexion::instance().creerConnexion("SYSTEM", "admin")) {
        QMessageBox::critical(nullptr, "Base de données Oracle",
                              Connexion::instance().derniereErreur());
        return -1;
    }

    MainWindow w;
    w.showMaximized();
    return a.exec();
}