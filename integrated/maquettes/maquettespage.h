#pragma once
#include <QMainWindow>
#include <QList>
#include <QPixmap>
#include <QString>

class QLabel;
class QTimer;
class Maquette;
namespace Ui { class MaquettesPage; }

// Module « Gestion des Maquettes » intégré dans STYLEHUB (page d'index 3 du menu).
// CRUD + recherche + tri + pagination (5 / page) + suppression multiple + stats
// + vidéo de présentation + alerte des maquettes en retard de validation + export PDF.
class MaquettesPage : public QMainWindow
{
    Q_OBJECT
public:
    explicit MaquettesPage(QWidget *parent = nullptr);
    ~MaquettesPage() override;

protected:
    void showEvent(QShowEvent *event) override;

private slots:
    void onAjouter();
    void onModifier();
    void onSupprimer();
    void onAfficher();
    void onReinitialiser();
    void onExporterPdf();
    void onChoisirVideo();
    void onLireVideo();
    void onLigneCliquee(int row, int column);
    void onStatutChange();
    void verifierRetards(bool popup);

private:
    void appliquerStyle();
    void installerIcones();
    void rafraichirTout();
    void rafraichirListe();
    void rafraichirPagination(int totalPages);
    void rafraichirStats();
    void remplirFormulaire(const Maquette &m);
    void afficherApercu(const QString &type, const QString &statut);
    void majBoutonVideo(const QString &video);
    QString prochainId() const;
    QString idCourant() const;
    Maquette depuisFormulaire() const;

    Ui::MaquettesPage *ui;
    QLabel *m_badgeCloche = nullptr;
    QTimer *m_timerRetard = nullptr;
    QList<QPixmap> m_miniatures;       // images affichables via les 3 miniatures
    QString m_videoCourante;
    int  m_page = 0;
    bool m_alerteAffichee = false;

    static constexpr int PAR_PAGE = 5;
    static constexpr int JOURS_RETARD = 7;
};
