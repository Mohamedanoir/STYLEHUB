#include "smartmarket.h"
#include "connexion.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("Fashionova SmartMarket");
    app.setStyle("Fusion");

    Connexion maquetteDb;
    maquetteDb.creerConnexion();

    // Un seul UI (smartmarket.ui) : la page de connexion est la page 0 du stackedWidget principal.
    SmartMarket w;
    w.show();
    return app.exec();
}
