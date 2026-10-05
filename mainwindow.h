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

namespace Ui { class MainWindow; }

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    bool eventFilter(QObject *obj, QEvent *ev) override;

private slots:
    // CRUD
    void onAjouter();
    void onModifier();
    void onSupprimer();
    void onAfficher();
    void onReinitialiser();

    // Liste
    void rafraichirListe();
    void onLigneCliquee(int ligne, int colonne);
    void onExporterPdf();

    // Image qui change selon Type + Statut
    void mettreAJourImageMaquette();

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
    void construirePagination(int nbPages);
    void rafraichirStatistiques();
    void remplirFormulaire(const QString &id);
    void mettreAJourBoutonVideo(const QString &video);
    QString nomImage(const QString &type) const;   // "robe", "veste", "jupe" ou ""
    QPixmap apercuPour(const Maquette &m) const;

    Ui::MainWindow *ui;

    // Raccourcis vers les widgets de mainwindow.ui
    QWidget        *m_sidebar = nullptr;
    QStackedWidget *m_pages = nullptr;
    QButtonGroup   *m_navGroupe = nullptr;

    QLineEdit   *m_rechercheGlobale = nullptr;
    QLabel      *m_badgeCloche = nullptr;

    QLineEdit   *m_editId = nullptr;
    QLineEdit   *m_editNom = nullptr;
    QComboBox   *m_comboType = nullptr;
    QDateEdit   *m_dateCreation = nullptr;
    QComboBox   *m_comboStatut = nullptr;
    QLabel      *m_pastilleStatut = nullptr;
    QFrame      *m_fondPastille = nullptr;
    QTextEdit   *m_editDescription = nullptr;

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

    QMap<QString, QPixmap> m_imgAvant;     // robe/veste/jupe -> toile
    QMap<QString, QPixmap> m_imgValidee;   // robe/veste/jupe -> 1re image du GIF
    QMap<QString, QPixmap> m_imgValidee2;  // robe/veste/jupe -> image du milieu du GIF

    static constexpr int JOURS_MAX_VALIDATION = 7;
    static constexpr int LIGNES_PAR_PAGE = 5;
};

#endif // MAINWINDOW_H
