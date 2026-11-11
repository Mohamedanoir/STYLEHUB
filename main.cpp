#include "mainwindow.h"
#include "connexion.h"

#include <QApplication>
#include <QMessageBox>
#include <QInputDialog>
#include <QLineEdit>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setStyle("Fusion");

    // Demander les identifiants Oracle au lancement; ne pas enregistrer le mot de passe.
    bool ok = false;
    const QString utilisateur = QInputDialog::getText(
        nullptr, "Connexion Oracle", "Utilisateur Oracle :",
        QLineEdit::Normal, "fashionova", &ok).trimmed();
    if (!ok) return 0;
    if (utilisateur.isEmpty()) {
        QMessageBox::warning(nullptr, "Connexion Oracle", "Saisissez le nom d'utilisateur Oracle.");
        return -1;
    }

    const QString motDePasse = QInputDialog::getText(
        nullptr, "Connexion Oracle", "Mot de passe Oracle :",
        QLineEdit::Password, QString(), &ok);
    if (!ok) return 0;
    if (motDePasse.isEmpty()) {
        QMessageBox::warning(nullptr, "Connexion Oracle", "Le mot de passe ne peut pas être vide.");
        return -1;
    }

    if (!Connexion::instance().creerConnexion(utilisateur, motDePasse)) {
        QMessageBox::critical(nullptr, "Base de données Oracle",
                              Connexion::instance().derniereErreur());
        return -1;
    }

    MainWindow w;
    w.showMaximized();
    return a.exec();
}
