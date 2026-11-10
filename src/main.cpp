#include <QApplication>
#include <QInputDialog>
#include <QLineEdit>
#include <QMessageBox>
#include "connexion.h"
#include "employe.h"
#include "mainwindow.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    a.setStyle("Fusion");

    bool ok = false;
    const QString utilisateur = QInputDialog::getText(
        nullptr, "Connexion Oracle", "Utilisateur du schéma Oracle :",
        QLineEdit::Normal, QString(), &ok);
    if (!ok || utilisateur.trimmed().isEmpty())
        return 0;

    const QString motDePasse = QInputDialog::getText(
        nullptr, "Connexion Oracle", "Mot de passe Oracle :",
        QLineEdit::Password, QString(), &ok);
    if (!ok)
        return 0;

    Connexion &connexion = Connexion::instance();
    QString erreur;
    if (!connexion.ouvrir(utilisateur, motDePasse, &erreur)) {
        QMessageBox::critical(nullptr, "Connexion Oracle",
                              "Connexion impossible à Oracle XE.\n\n" + erreur +
                              "\n\nVérifiez que le service Oracle XE et le listener sont démarrés.");
        return 1;
    }

    QString schemaError;
    Employe::afficher(&schemaError);
    if (!schemaError.isEmpty()) {
        QMessageBox::critical(nullptr, "Schéma EMPLOYES introuvable",
                              "La connexion Oracle fonctionne, mais la table EMPLOYES "
                              "n'est pas accessible pour cet utilisateur.\n\n" +
                              schemaError +
                              "\n\nLe script SQL fourni supprime d'abord les tables existantes. "
                              "Ne l'exécutez que dans un schéma vide ou après sauvegarde.");
        connexion.fermer();
        return 1;
    }

    MainWindow w;
    w.show();
    const int codeRetour = a.exec();
    connexion.fermer();
    return codeRetour;
}
