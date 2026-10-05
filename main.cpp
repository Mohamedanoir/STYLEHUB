#include "mainwindow.h"
#include "connexion.h"

#include <QApplication>
#include <QMessageBox>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setStyle("Fusion");

    Connexion c;
    if (!c.creerConnexion()) {
        QMessageBox::critical(nullptr, "Base de données",
                              "Impossible d'ouvrir la base de données.\n" + c.derniereErreur());
        return -1;
    }

    MainWindow w;
    w.showMaximized();
    return a.exec();
}
