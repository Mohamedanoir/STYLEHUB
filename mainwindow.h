#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPixmap>
#include <QString>
#include <QVector>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

struct Client {
    QString id, nom, prenom, tel, email, adresse, type, date, remarques;
};

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
    void afficher();
    void reinitialiser();
    void exporterPdf();
    void remplirFormulaire();
    void onFilterChanged();

private:
    QVector<Client> sortedFiltered() const;
    void refreshTable(const QString &keepId);
    Client formData() const;
    bool valider(const Client &c);
    QString nextId() const;
    int selectedIndex() const;
    QString selectedId() const;
    int indexOfId(const QString &id) const;
    void status(const QString &text);
    void updateBanner();
    void updateStats();
    void refreshHistorique();

    Ui::MainWindow *ui;
    QPixmap bannerPix;
    QPixmap pixRepartition, pixEvolution, pixTop5;
    QVector<Client> clients; // toutes les données
    QVector<Commande> commandes; // historique de toutes les commandes
    QVector<Client> view;    // lignes actuellement affichées
};

#endif // MAINWINDOW_H
