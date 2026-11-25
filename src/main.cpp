#include <QApplication>
#include <QFontDatabase>
#include <QInputDialog>
#include <QLineEdit>
#include <QMessageBox>
#include "connexion.h"
#include "employe.h"
#include "mainwindow.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    a.setStyle("Fusion");

    const QStringList fontFiles = {
        QStringLiteral(":/fonts/Jost-Regular.ttf"),
        QStringLiteral(":/fonts/Jost-Medium.ttf"),
        QStringLiteral(":/fonts/Jost-SemiBold.ttf"),
        QStringLiteral(":/fonts/CormorantGaramond-Medium.ttf"),
        QStringLiteral(":/fonts/CormorantGaramond-MediumItalic.ttf")
    };
    for (const QString &fontFile : fontFiles)
        QFontDatabase::addApplicationFont(fontFile);


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
    if (!Employe::preparerSchemaPresence(&schemaError)) {
        QMessageBox::critical(nullptr, "Mise a jour de la presence impossible",
                              "Le compte Oracle doit posseder EMPLOYES pour ajouter la colonne de presence.\n\n" +
                              schemaError +
                              "\n\nVous pouvez aussi executer sql/03_add_employee_presence.sql avec le proprietaire de la table.");
        connexion.fermer();
        return 1;
    }

    schemaError.clear();
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

    // MainWindow schedules the employee login before showing the dashboard.
    MainWindow w;
    const int codeRetour = a.exec();
    connexion.fermer();
    return codeRetour;
}
