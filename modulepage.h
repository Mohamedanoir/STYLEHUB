#ifndef MODULEPAGE_H
#define MODULEPAGE_H

#include <QDialog>
#include <QList>
#include <QStringList>
#include <QWidget>

class QTableWidget;
class QPushButton;
class QLineEdit;

// Un champ du formulaire : libellé + type de saisie
struct ChampForm
{
    enum Type { Texte, Nombre, Date, Choix, Description };
    QString     libelle;
    Type        type = Texte;
    QStringList choix;     // utilisé si type == Choix
};

// ============================================================================
//  Formulaire (fenêtre) : Ajouter / Modifier / Afficher
//  -> uniquement visuel : "Enregistrer" ferme la fenêtre sans rien sauvegarder
// ============================================================================
class FormulaireDialog : public QDialog
{
    Q_OBJECT
public:
    enum Mode { Ajout, Modification, Affichage };

    FormulaireDialog(const QString &titreModule, const QList<ChampForm> &champs,
                     Mode mode, const QStringList &valeurs, QWidget *parent = nullptr);
};

// ============================================================================
//  Page d'un module (Commandes, Machines, ...) avec 4 boutons NON fonctionnels :
//  Ajouter / Modifier / Supprimer / Afficher  -> chacun ouvre seulement un formulaire
//  ou une confirmation. Aucune base de données.
//  Quand le vrai module est prêt : remplacer cette page par la vraie interface.
// ============================================================================
class ModulePage : public QWidget
{
    Q_OBJECT
public:
    ModulePage(const QString &titre, const QList<ChampForm> &champs,
               const QList<QStringList> &exemples, QWidget *parent = nullptr);

private slots:
    void onAjouter();
    void onModifier();
    void onSupprimer();
    void onAfficher();

private:
    QStringList ligneSelectionnee() const;   // valeurs de la ligne choisie (ou vide)

    QString             m_titre;
    QList<ChampForm>    m_champs;
    QTableWidget       *m_table = nullptr;
};

#endif // MODULEPAGE_H
