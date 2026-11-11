#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMap>
#include <QPixmap>

#include "maquette.h"

class QButtonGroup;
class QComboBox;
class QDateEdit;
class QFrame;
class QHBoxLayout;
class QLabel;
class QLineEdit;
class QPushButton;
class QStackedWidget;
class QTableWidget;
class QTextEdit;
class QTimer;
class QToolButton;
class QWidget;
class ApercuMaquette;
class Miniature;
class DonutChart;
class LineChart;
class BarChart;
class FashionNovaV2Window;
class MachinesPage;

namespace Ui { class MainWindow; }

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    bool eventFilter(QObject *obj, QEvent *ev) override;
    void changeEvent(QEvent *ev) override;   // retour sur la fenêtre -> recharger depuis Oracle

private slots:
    // CRUD
    void onAjouter();
    void onModifier();
    void onSupprimer();
    void onReinitialiser();

    // Liste
    void rafraichirListe();
    void onLigneCliquee(int ligne, int colonne);
    void onExporterPdf();

    // Image qui change selon Type + Statut
    void mettreAJourImageMaquette();
    // Attributs (mesures) qui changent selon le Type
    void mettreAJourMesures();

    // Métiers innovants
    void onChoisirVideo();
    void onBoutonPlus();
    void verifierRetards();
    void onCloche();

private:
    void placerImageSidebar();          // image du mannequin en bas du menu
    QLabel *m_imageSidebar = nullptr;
    int     m_navShift = 0;             // décalage vers le haut des boutons pendant le défilement

    // Interface (mainwindow.ui) : réglages faits en code (icônes, ombres, connexions)
    void configurerInterface();
    void appliquerStyle();

    void afficherPage(int page);
    int  nbPages() const;                       // nombre de pages du tableau
    void afficherMaquetteDansTableau(const QString &id);   // page de la maquette + ligne surlignée
    void construirePagination(int nbPages);
    void rafraichirStatistiques();
    void remplirFormulaire(const QString &id);
    Maquette lireFormulaire() const;    // lit tous les champs du formulaire
    QMap<QString, double> lireMesuresDuType() const;   // mesures affichées pour le type choisi
    void genererId();                   // champ ID = prochain ID libre (dernier ID + 1)
    void mettreAJourBoutonVideo(const QString &video);
    QString nomImage(const QString &type) const;   // "robe", "veste", "jupe"... ou "" (pas d'image)
    QPixmap apercuPour(const Maquette &m) const;

    Ui::MainWindow *ui;

    // Raccourcis vers les widgets de mainwindow.ui
    QWidget        *m_sidebar = nullptr;
    QStackedWidget *m_pages = nullptr;
    QButtonGroup   *m_navGroupe = nullptr;
    FashionNovaV2Window *m_clientsV2 = nullptr;
    MachinesPage *m_pageMachines = nullptr;

    QLineEdit   *m_rechercheGlobale = nullptr;
    QLabel      *m_badgeCloche = nullptr;

    QLineEdit   *m_editId = nullptr;
    QLineEdit   *m_editNom = nullptr;
    QComboBox   *m_comboType = nullptr;
    QDateEdit   *m_dateCreation = nullptr;
    QComboBox   *m_comboStatut = nullptr;
    QLabel      *m_pastilleStatut = nullptr;
    QFrame      *m_fondPastille = nullptr;
    QComboBox   *m_comboTaille = nullptr;
    QLineEdit   *m_editLargeur = nullptr;        // mesures en cm (nombre réel), tous les types
    QLineEdit   *m_editLongueur = nullptr;
    // Mesures qui changent selon le type : colonne Oracle -> champ / ligne (label + champ)
    QMap<QString, QLineEdit *>      m_editMesures;
    QMap<QString, QList<QWidget *>> m_lignesMesures;
    QTextEdit   *m_editDescription = nullptr;
    QLineEdit   *m_editVideo = nullptr;

    ApercuMaquette *m_apercu = nullptr;
    Miniature      *m_mini[3] = {nullptr, nullptr, nullptr};
    QToolButton    *m_boutonPlus = nullptr;

    QLineEdit    *m_rechercheListe = nullptr;
    QComboBox    *m_comboTri = nullptr;
    QTableWidget *m_table = nullptr;
    QHBoxLayout  *m_paginationLayout = nullptr;
    QLabel       *m_labelRetard = nullptr;
    QList<Maquette> m_liste;
    int  m_pageCourante = 1;
    bool m_toutCoche = false;

    DonutChart *m_donut = nullptr;
    LineChart  *m_courbe = nullptr;
    BarChart   *m_barres = nullptr;

    QTimer *m_timerRetard = nullptr;
    bool    m_alerteDejaAffichee = false;

    QMap<QString, QPixmap> m_imgAvant;     // type (robe, veste...) -> toile
    QMap<QString, QPixmap> m_imgValidee;   // type -> 1re image du GIF
    QMap<QString, QPixmap> m_imgValidee2;  // type -> image du milieu du GIF

    static constexpr int JOURS_MAX_VALIDATION = 7;
    static constexpr int LIGNES_PAR_PAGE = 5;
};

#endif // MAINWINDOW_H
