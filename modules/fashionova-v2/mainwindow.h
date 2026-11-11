#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPixmap>
#include <QPointer>
#include <QString>
#include <QVector>

#include "client.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class DonutCard;
class EvolutionCard;
class TopCard;

struct Commande {
    QString numero, clientId, date, designation, statut;
    double montant = 0.0;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;

private slots:
    void ajouter();
    void modifier();
    void supprimer();
    void reinitialiser();
    void exporterPdf();
    void remplirFormulaire();
    void onFilterChanged();

private:
    QVector<Client> sortedFiltered() const;
    void refreshTable(const QString &keepId);
    Client formData() const;
    // Contrôles de saisie communs à l'ajout et à la modification.
    // ancienId : vide pour un ajout, ID d'origine pour une modification.
    bool valider(const Client &c, const QString &ancienId = QString());
    bool confirmerModification(const Client &ancien, const Client &nouveau);
    QString nextId() const;
    int selectedIndex() const;
    QString selectedId() const;
    int indexOfId(const QString &id) const;
    void status(const QString &text);
    void updateBanner();
    void updateStats();        // recalcule les 3 statistiques à partir des données
    void updateStatsLayout();  // hauteur / proportions de la zone stats
    void refreshHistorique();

    Ui::MainWindow *ui;
    QPixmap bannerPix;
    DonutCard *statDonut = nullptr;
    EvolutionCard *statEvolution = nullptr;
    TopCard *statTop = nullptr;
    QVector<Client> clients; // toutes les données
    QVector<Commande> commandes; // historique de toutes les commandes
    QVector<Client> view;    // lignes actuellement affichées
};

#endif // MAINWINDOW_H
