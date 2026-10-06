/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.7.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDateEdit>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "statwidgets.h"
#include "widgets.h"

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QHBoxLayout *layoutCentral;
    QWidget *sidebar;
    QVBoxLayout *layoutSidebar;
    QLabel *labelLogo;
    QSpacerItem *spacerLogo;
    QPushButton *btnClients;
    QPushButton *btnEmployes;
    QPushButton *btnCommandes;
    QPushButton *btnMaquettes;
    QPushButton *btnMachines;
    QPushButton *btnArticles;
    QSpacerItem *spacerSidebar;
    QWidget *zoneDroite;
    QVBoxLayout *layoutZoneDroite;
    QWidget *topbar;
    QHBoxLayout *layoutTopbar;
    QToolButton *btnMenu;
    QLineEdit *editRechercheGlobale;
    QSpacerItem *spacerTop;
    QToolButton *btnCloche;
    QLabel *labelAvatar;
    QLabel *labelUser;
    QToolButton *btnCompte;
    QStackedWidget *stackedWidget;
    QWidget *pageClients;
    QVBoxLayout *layoutPageClients;
    QLabel *labelClients;
    QWidget *pageEmployes;
    QVBoxLayout *layoutPageEmployes;
    QLabel *labelEmployes;
    QWidget *pageCommandes;
    QVBoxLayout *layoutPageCommandes;
    QLabel *labelCommandes;
    QWidget *pageMaquettes;
    QVBoxLayout *layoutMaquettes;
    QScrollArea *scrollMaquettes;
    QWidget *scrollContents;
    QVBoxLayout *layoutPageMaquettes;
    Banniere *banniere;
    QHBoxLayout *layoutMilieu;
    QFrame *carteFormulaire;
    QVBoxLayout *layoutCarteFormulaire;
    QFrame *enteteFormulaire;
    QHBoxLayout *layoutEnteteFormulaire;
    QLabel *iconeFormulaire;
    QLabel *titreFormulaire;
    QSpacerItem *spacerEnteteFormulaire;
    QPushButton *btnReinitialiser;
    QWidget *corpsFormulaire;
    QVBoxLayout *layoutCorpsFormulaire;
    QHBoxLayout *layoutHautFormulaire;
    QGridLayout *gridChamps;
    QLabel *labelId;
    QFrame *champId;
    QHBoxLayout *layout_champId;
    QLabel *iconeId;
    QLineEdit *editId;
    QLabel *labelNom;
    QFrame *champNom;
    QHBoxLayout *layout_champNom;
    QLabel *iconeNom;
    QLineEdit *editNom;
    QLabel *labelType;
    QFrame *champType;
    QHBoxLayout *layout_champType;
    QLabel *iconeType;
    QComboBox *comboType;
    QLabel *labelDate;
    QFrame *champDate;
    QHBoxLayout *layout_champDate;
    QToolButton *btnCalendrier;
    QDateEdit *dateCreation;
    QLabel *labelStatut;
    QFrame *champStatut;
    QHBoxLayout *layout_champStatut;
    QFrame *fondPastille;
    QHBoxLayout *layoutPastille;
    QLabel *pastilleStatut;
    QComboBox *comboStatut;
    QLabel *labelDescription;
    QTextEdit *editDescription;
    QVBoxLayout *layoutApercu;
    ApercuMaquette *apercu;
    QHBoxLayout *layoutMiniatures;
    Miniature *mini1;
    Miniature *mini2;
    Miniature *mini3;
    QToolButton *btnPlus;
    QSpacerItem *spacerApercu;
    QGridLayout *gridBoutons;
    QPushButton *btnAjouter;
    QPushButton *btnModifier;
    QPushButton *btnSupprimer;
    QPushButton *btnAfficher;
    QFrame *carteListe;
    QVBoxLayout *layoutCarteListe;
    QFrame *enteteListe;
    QHBoxLayout *layoutEnteteListe;
    QLabel *iconeListe;
    QLabel *titreListe;
    QSpacerItem *spacerEnteteListe;
    QPushButton *btnExporterPdf;
    QWidget *corpsListe;
    QVBoxLayout *layoutCorpsListe;
    QHBoxLayout *layoutRechercheTri;
    QLineEdit *editRechercheListe;
    QLabel *labelTri;
    QComboBox *comboTri;
    QTableWidget *tableMaquettes;
    QHBoxLayout *layoutBasListe;
    QLabel *labelRetard;
    QSpacerItem *spacerPagination;
    QHBoxLayout *layoutPagination;
    QSpacerItem *spacerListe;
    QHBoxLayout *layoutBas;
    QFrame *carteStats;
    QVBoxLayout *layoutCarteStats;
    QFrame *enteteStats;
    QHBoxLayout *layoutEnteteStats;
    QLabel *iconeStats;
    QLabel *titreStats;
    QSpacerItem *spacerEnteteStats;
    QWidget *corpsStats;
    QHBoxLayout *layoutCorpsStats;
    QFrame *blocType;
    QVBoxLayout *layout_blocType;
    QLabel *titre_blocType;
    DonutChart *chartType;
    QFrame *blocEvolution;
    QVBoxLayout *layout_blocEvolution;
    QLabel *titre_blocEvolution;
    LineChart *chartEvolution;
    QFrame *blocTop;
    QVBoxLayout *layout_blocTop;
    QLabel *titre_blocTop;
    BarChart *chartTop;
    QSpacerItem *spacerPage;
    QWidget *pageMachines;
    QVBoxLayout *layoutPageMachines;
    QLabel *labelMachines;
    QWidget *pageArticles;
    QVBoxLayout *layoutPageArticles;
    QLabel *labelArticles;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(1540, 1030);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        layoutCentral = new QHBoxLayout(centralwidget);
        layoutCentral->setSpacing(0);
        layoutCentral->setObjectName("layoutCentral");
        layoutCentral->setContentsMargins(0, 0, 0, 0);
        sidebar = new QWidget(centralwidget);
        sidebar->setObjectName("sidebar");
        sidebar->setMinimumSize(QSize(200, 0));
        sidebar->setMaximumSize(QSize(200, 16777215));
        layoutSidebar = new QVBoxLayout(sidebar);
        layoutSidebar->setSpacing(6);
        layoutSidebar->setObjectName("layoutSidebar");
        layoutSidebar->setContentsMargins(6, 0, 6, 15);
        labelLogo = new QLabel(sidebar);
        labelLogo->setObjectName("labelLogo");
        labelLogo->setMinimumSize(QSize(0, 175));
        labelLogo->setMaximumSize(QSize(16777215, 175));
        labelLogo->setAlignment(Qt::AlignCenter);

        layoutSidebar->addWidget(labelLogo);

        spacerLogo = new QSpacerItem(20, 14, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Fixed);

        layoutSidebar->addItem(spacerLogo);

        btnClients = new QPushButton(sidebar);
        btnClients->setObjectName("btnClients");
        btnClients->setCheckable(true);
        btnClients->setProperty("nav", QVariant(true));
        btnClients->setMinimumSize(QSize(0, 50));
        btnClients->setMaximumSize(QSize(16777215, 50));
        btnClients->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));

        layoutSidebar->addWidget(btnClients);

        btnEmployes = new QPushButton(sidebar);
        btnEmployes->setObjectName("btnEmployes");
        btnEmployes->setCheckable(true);
        btnEmployes->setProperty("nav", QVariant(true));
        btnEmployes->setMinimumSize(QSize(0, 50));
        btnEmployes->setMaximumSize(QSize(16777215, 50));
        btnEmployes->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));

        layoutSidebar->addWidget(btnEmployes);

        btnCommandes = new QPushButton(sidebar);
        btnCommandes->setObjectName("btnCommandes");
        btnCommandes->setCheckable(true);
        btnCommandes->setProperty("nav", QVariant(true));
        btnCommandes->setMinimumSize(QSize(0, 50));
        btnCommandes->setMaximumSize(QSize(16777215, 50));
        btnCommandes->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));

        layoutSidebar->addWidget(btnCommandes);

        btnMaquettes = new QPushButton(sidebar);
        btnMaquettes->setObjectName("btnMaquettes");
        btnMaquettes->setCheckable(true);
        btnMaquettes->setProperty("nav", QVariant(true));
        btnMaquettes->setMinimumSize(QSize(0, 50));
        btnMaquettes->setMaximumSize(QSize(16777215, 50));
        btnMaquettes->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));

        layoutSidebar->addWidget(btnMaquettes);

        btnMachines = new QPushButton(sidebar);
        btnMachines->setObjectName("btnMachines");
        btnMachines->setCheckable(true);
        btnMachines->setProperty("nav", QVariant(true));
        btnMachines->setMinimumSize(QSize(0, 50));
        btnMachines->setMaximumSize(QSize(16777215, 50));
        btnMachines->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));

        layoutSidebar->addWidget(btnMachines);

        btnArticles = new QPushButton(sidebar);
        btnArticles->setObjectName("btnArticles");
        btnArticles->setCheckable(true);
        btnArticles->setProperty("nav", QVariant(true));
        btnArticles->setMinimumSize(QSize(0, 50));
        btnArticles->setMaximumSize(QSize(16777215, 50));
        btnArticles->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));

        layoutSidebar->addWidget(btnArticles);

        spacerSidebar = new QSpacerItem(20, 20, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        layoutSidebar->addItem(spacerSidebar);


        layoutCentral->addWidget(sidebar);

        zoneDroite = new QWidget(centralwidget);
        zoneDroite->setObjectName("zoneDroite");
        layoutZoneDroite = new QVBoxLayout(zoneDroite);
        layoutZoneDroite->setSpacing(0);
        layoutZoneDroite->setObjectName("layoutZoneDroite");
        layoutZoneDroite->setContentsMargins(0, 0, 0, 0);
        topbar = new QWidget(zoneDroite);
        topbar->setObjectName("topbar");
        topbar->setMinimumSize(QSize(0, 65));
        topbar->setMaximumSize(QSize(16777215, 65));
        layoutTopbar = new QHBoxLayout(topbar);
        layoutTopbar->setSpacing(18);
        layoutTopbar->setObjectName("layoutTopbar");
        layoutTopbar->setContentsMargins(24, 10, 28, 10);
        btnMenu = new QToolButton(topbar);
        btnMenu->setObjectName("btnMenu");
        btnMenu->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));

        layoutTopbar->addWidget(btnMenu);

        editRechercheGlobale = new QLineEdit(topbar);
        editRechercheGlobale->setObjectName("editRechercheGlobale");
        editRechercheGlobale->setMinimumSize(QSize(520, 38));
        editRechercheGlobale->setMaximumSize(QSize(520, 38));

        layoutTopbar->addWidget(editRechercheGlobale);

        spacerTop = new QSpacerItem(20, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        layoutTopbar->addItem(spacerTop);

        btnCloche = new QToolButton(topbar);
        btnCloche->setObjectName("btnCloche");
        btnCloche->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));
        btnCloche->setMinimumSize(QSize(40, 40));
        btnCloche->setMaximumSize(QSize(40, 40));

        layoutTopbar->addWidget(btnCloche);

        labelAvatar = new QLabel(topbar);
        labelAvatar->setObjectName("labelAvatar");

        layoutTopbar->addWidget(labelAvatar);

        labelUser = new QLabel(topbar);
        labelUser->setObjectName("labelUser");

        layoutTopbar->addWidget(labelUser);

        btnCompte = new QToolButton(topbar);
        btnCompte->setObjectName("btnCompte");
        btnCompte->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));

        layoutTopbar->addWidget(btnCompte);


        layoutZoneDroite->addWidget(topbar);

        stackedWidget = new QStackedWidget(zoneDroite);
        stackedWidget->setObjectName("stackedWidget");
        pageClients = new QWidget();
        pageClients->setObjectName("pageClients");
        layoutPageClients = new QVBoxLayout(pageClients);
        layoutPageClients->setObjectName("layoutPageClients");
        labelClients = new QLabel(pageClients);
        labelClients->setObjectName("labelClients");
        labelClients->setAlignment(Qt::AlignCenter);

        layoutPageClients->addWidget(labelClients);

        stackedWidget->addWidget(pageClients);
        pageEmployes = new QWidget();
        pageEmployes->setObjectName("pageEmployes");
        layoutPageEmployes = new QVBoxLayout(pageEmployes);
        layoutPageEmployes->setObjectName("layoutPageEmployes");
        labelEmployes = new QLabel(pageEmployes);
        labelEmployes->setObjectName("labelEmployes");
        labelEmployes->setAlignment(Qt::AlignCenter);

        layoutPageEmployes->addWidget(labelEmployes);

        stackedWidget->addWidget(pageEmployes);
        pageCommandes = new QWidget();
        pageCommandes->setObjectName("pageCommandes");
        layoutPageCommandes = new QVBoxLayout(pageCommandes);
        layoutPageCommandes->setObjectName("layoutPageCommandes");
        labelCommandes = new QLabel(pageCommandes);
        labelCommandes->setObjectName("labelCommandes");
        labelCommandes->setAlignment(Qt::AlignCenter);

        layoutPageCommandes->addWidget(labelCommandes);

        stackedWidget->addWidget(pageCommandes);
        pageMaquettes = new QWidget();
        pageMaquettes->setObjectName("pageMaquettes");
        layoutMaquettes = new QVBoxLayout(pageMaquettes);
        layoutMaquettes->setObjectName("layoutMaquettes");
        layoutMaquettes->setContentsMargins(0, 0, 0, 0);
        scrollMaquettes = new QScrollArea(pageMaquettes);
        scrollMaquettes->setObjectName("scrollMaquettes");
        scrollMaquettes->setFrameShape(QFrame::NoFrame);
        scrollMaquettes->setWidgetResizable(true);
        scrollContents = new QWidget();
        scrollContents->setObjectName("scrollContents");
        layoutPageMaquettes = new QVBoxLayout(scrollContents);
        layoutPageMaquettes->setSpacing(14);
        layoutPageMaquettes->setObjectName("layoutPageMaquettes");
        layoutPageMaquettes->setContentsMargins(0, 0, 0, 16);
        banniere = new Banniere(scrollContents);
        banniere->setObjectName("banniere");

        layoutPageMaquettes->addWidget(banniere);

        layoutMilieu = new QHBoxLayout();
        layoutMilieu->setSpacing(14);
        layoutMilieu->setObjectName("layoutMilieu");
        layoutMilieu->setContentsMargins(14, 0, 14, 0);
        carteFormulaire = new QFrame(scrollContents);
        carteFormulaire->setObjectName("carteFormulaire");
        layoutCarteFormulaire = new QVBoxLayout(carteFormulaire);
        layoutCarteFormulaire->setSpacing(0);
        layoutCarteFormulaire->setObjectName("layoutCarteFormulaire");
        layoutCarteFormulaire->setContentsMargins(0, 0, 0, 0);
        enteteFormulaire = new QFrame(carteFormulaire);
        enteteFormulaire->setObjectName("enteteFormulaire");
        enteteFormulaire->setMinimumSize(QSize(0, 46));
        enteteFormulaire->setMaximumSize(QSize(16777215, 46));
        layoutEnteteFormulaire = new QHBoxLayout(enteteFormulaire);
        layoutEnteteFormulaire->setSpacing(14);
        layoutEnteteFormulaire->setObjectName("layoutEnteteFormulaire");
        layoutEnteteFormulaire->setContentsMargins(20, 0, 14, 0);
        iconeFormulaire = new QLabel(enteteFormulaire);
        iconeFormulaire->setObjectName("iconeFormulaire");

        layoutEnteteFormulaire->addWidget(iconeFormulaire);

        titreFormulaire = new QLabel(enteteFormulaire);
        titreFormulaire->setObjectName("titreFormulaire");

        layoutEnteteFormulaire->addWidget(titreFormulaire);

        spacerEnteteFormulaire = new QSpacerItem(20, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        layoutEnteteFormulaire->addItem(spacerEnteteFormulaire);

        btnReinitialiser = new QPushButton(enteteFormulaire);
        btnReinitialiser->setObjectName("btnReinitialiser");
        btnReinitialiser->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));

        layoutEnteteFormulaire->addWidget(btnReinitialiser);


        layoutCarteFormulaire->addWidget(enteteFormulaire);

        corpsFormulaire = new QWidget(carteFormulaire);
        corpsFormulaire->setObjectName("corpsFormulaire");
        layoutCorpsFormulaire = new QVBoxLayout(corpsFormulaire);
        layoutCorpsFormulaire->setSpacing(14);
        layoutCorpsFormulaire->setObjectName("layoutCorpsFormulaire");
        layoutCorpsFormulaire->setContentsMargins(16, 14, 16, 14);
        layoutHautFormulaire = new QHBoxLayout();
        layoutHautFormulaire->setSpacing(16);
        layoutHautFormulaire->setObjectName("layoutHautFormulaire");
        gridChamps = new QGridLayout();
        gridChamps->setObjectName("gridChamps");
        gridChamps->setHorizontalSpacing(12);
        gridChamps->setVerticalSpacing(11);
        labelId = new QLabel(corpsFormulaire);
        labelId->setObjectName("labelId");

        gridChamps->addWidget(labelId, 0, 0, 1, 1);

        champId = new QFrame(corpsFormulaire);
        champId->setObjectName("champId");
        champId->setMinimumSize(QSize(0, 36));
        champId->setMaximumSize(QSize(16777215, 36));
        layout_champId = new QHBoxLayout(champId);
        layout_champId->setSpacing(10);
        layout_champId->setObjectName("layout_champId");
        layout_champId->setContentsMargins(10, 0, 4, 0);
        iconeId = new QLabel(champId);
        iconeId->setObjectName("iconeId");
        iconeId->setMinimumSize(QSize(22, 0));
        iconeId->setMaximumSize(QSize(22, 16777215));

        layout_champId->addWidget(iconeId);

        editId = new QLineEdit(champId);
        editId->setObjectName("editId");
        editId->setMaxLength(10);

        layout_champId->addWidget(editId);


        gridChamps->addWidget(champId, 0, 1, 1, 1);

        labelNom = new QLabel(corpsFormulaire);
        labelNom->setObjectName("labelNom");

        gridChamps->addWidget(labelNom, 1, 0, 1, 1);

        champNom = new QFrame(corpsFormulaire);
        champNom->setObjectName("champNom");
        champNom->setMinimumSize(QSize(0, 36));
        champNom->setMaximumSize(QSize(16777215, 36));
        layout_champNom = new QHBoxLayout(champNom);
        layout_champNom->setSpacing(10);
        layout_champNom->setObjectName("layout_champNom");
        layout_champNom->setContentsMargins(10, 0, 4, 0);
        iconeNom = new QLabel(champNom);
        iconeNom->setObjectName("iconeNom");
        iconeNom->setMinimumSize(QSize(22, 0));
        iconeNom->setMaximumSize(QSize(22, 16777215));

        layout_champNom->addWidget(iconeNom);

        editNom = new QLineEdit(champNom);
        editNom->setObjectName("editNom");
        editNom->setMaxLength(50);

        layout_champNom->addWidget(editNom);


        gridChamps->addWidget(champNom, 1, 1, 1, 1);

        labelType = new QLabel(corpsFormulaire);
        labelType->setObjectName("labelType");

        gridChamps->addWidget(labelType, 2, 0, 1, 1);

        champType = new QFrame(corpsFormulaire);
        champType->setObjectName("champType");
        champType->setMinimumSize(QSize(0, 36));
        champType->setMaximumSize(QSize(16777215, 36));
        layout_champType = new QHBoxLayout(champType);
        layout_champType->setSpacing(10);
        layout_champType->setObjectName("layout_champType");
        layout_champType->setContentsMargins(10, 0, 4, 0);
        iconeType = new QLabel(champType);
        iconeType->setObjectName("iconeType");
        iconeType->setMinimumSize(QSize(22, 0));
        iconeType->setMaximumSize(QSize(22, 16777215));

        layout_champType->addWidget(iconeType);

        comboType = new QComboBox(champType);
        comboType->addItem(QString());
        comboType->addItem(QString());
        comboType->addItem(QString());
        comboType->setObjectName("comboType");

        layout_champType->addWidget(comboType);


        gridChamps->addWidget(champType, 2, 1, 1, 1);

        labelDate = new QLabel(corpsFormulaire);
        labelDate->setObjectName("labelDate");

        gridChamps->addWidget(labelDate, 3, 0, 1, 1);

        champDate = new QFrame(corpsFormulaire);
        champDate->setObjectName("champDate");
        champDate->setMinimumSize(QSize(0, 36));
        champDate->setMaximumSize(QSize(16777215, 36));
        layout_champDate = new QHBoxLayout(champDate);
        layout_champDate->setSpacing(6);
        layout_champDate->setObjectName("layout_champDate");
        layout_champDate->setContentsMargins(6, 0, 4, 0);
        btnCalendrier = new QToolButton(champDate);
        btnCalendrier->setObjectName("btnCalendrier");
        btnCalendrier->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));

        layout_champDate->addWidget(btnCalendrier);

        dateCreation = new QDateEdit(champDate);
        dateCreation->setObjectName("dateCreation");
        dateCreation->setCalendarPopup(true);

        layout_champDate->addWidget(dateCreation);


        gridChamps->addWidget(champDate, 3, 1, 1, 1);

        labelStatut = new QLabel(corpsFormulaire);
        labelStatut->setObjectName("labelStatut");

        gridChamps->addWidget(labelStatut, 4, 0, 1, 1);

        champStatut = new QFrame(corpsFormulaire);
        champStatut->setObjectName("champStatut");
        champStatut->setMinimumSize(QSize(0, 36));
        champStatut->setMaximumSize(QSize(16777215, 36));
        layout_champStatut = new QHBoxLayout(champStatut);
        layout_champStatut->setSpacing(10);
        layout_champStatut->setObjectName("layout_champStatut");
        layout_champStatut->setContentsMargins(1, 1, 4, 1);
        fondPastille = new QFrame(champStatut);
        fondPastille->setObjectName("fondPastille");
        fondPastille->setMinimumSize(QSize(38, 34));
        fondPastille->setMaximumSize(QSize(38, 34));
        layoutPastille = new QHBoxLayout(fondPastille);
        layoutPastille->setObjectName("layoutPastille");
        layoutPastille->setContentsMargins(0, 0, 0, 0);
        pastilleStatut = new QLabel(fondPastille);
        pastilleStatut->setObjectName("pastilleStatut");
        pastilleStatut->setMinimumSize(QSize(14, 14));
        pastilleStatut->setMaximumSize(QSize(14, 14));

        layoutPastille->addWidget(pastilleStatut, 0, Qt::AlignCenter);


        layout_champStatut->addWidget(fondPastille);

        comboStatut = new QComboBox(champStatut);
        comboStatut->addItem(QString());
        comboStatut->addItem(QString());
        comboStatut->addItem(QString());
        comboStatut->setObjectName("comboStatut");

        layout_champStatut->addWidget(comboStatut);


        gridChamps->addWidget(champStatut, 4, 1, 1, 1);

        labelDescription = new QLabel(corpsFormulaire);
        labelDescription->setObjectName("labelDescription");

        gridChamps->addWidget(labelDescription, 5, 0, 1, 1, Qt::AlignTop);

        editDescription = new QTextEdit(corpsFormulaire);
        editDescription->setObjectName("editDescription");
        editDescription->setMinimumSize(QSize(0, 74));
        editDescription->setMaximumSize(QSize(16777215, 74));

        gridChamps->addWidget(editDescription, 5, 1, 1, 1);

        gridChamps->setColumnStretch(1, 1);
        gridChamps->setColumnMinimumWidth(0, 125);

        layoutHautFormulaire->addLayout(gridChamps);

        layoutApercu = new QVBoxLayout();
        layoutApercu->setSpacing(10);
        layoutApercu->setObjectName("layoutApercu");
        apercu = new ApercuMaquette(corpsFormulaire);
        apercu->setObjectName("apercu");
        apercu->setMinimumSize(QSize(168, 236));
        apercu->setMaximumSize(QSize(168, 236));

        layoutApercu->addWidget(apercu);

        layoutMiniatures = new QHBoxLayout();
        layoutMiniatures->setSpacing(5);
        layoutMiniatures->setObjectName("layoutMiniatures");
        mini1 = new Miniature(corpsFormulaire);
        mini1->setObjectName("mini1");
        mini1->setMinimumSize(QSize(37, 64));
        mini1->setMaximumSize(QSize(37, 64));

        layoutMiniatures->addWidget(mini1);

        mini2 = new Miniature(corpsFormulaire);
        mini2->setObjectName("mini2");
        mini2->setMinimumSize(QSize(37, 64));
        mini2->setMaximumSize(QSize(37, 64));

        layoutMiniatures->addWidget(mini2);

        mini3 = new Miniature(corpsFormulaire);
        mini3->setObjectName("mini3");
        mini3->setMinimumSize(QSize(37, 64));
        mini3->setMaximumSize(QSize(37, 64));

        layoutMiniatures->addWidget(mini3);

        btnPlus = new QToolButton(corpsFormulaire);
        btnPlus->setObjectName("btnPlus");
        btnPlus->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));
        btnPlus->setMinimumSize(QSize(43, 64));
        btnPlus->setMaximumSize(QSize(43, 64));

        layoutMiniatures->addWidget(btnPlus);


        layoutApercu->addLayout(layoutMiniatures);

        spacerApercu = new QSpacerItem(20, 20, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        layoutApercu->addItem(spacerApercu);


        layoutHautFormulaire->addLayout(layoutApercu);

        layoutHautFormulaire->setStretch(0, 1);

        layoutCorpsFormulaire->addLayout(layoutHautFormulaire);

        gridBoutons = new QGridLayout();
        gridBoutons->setObjectName("gridBoutons");
        gridBoutons->setHorizontalSpacing(12);
        gridBoutons->setVerticalSpacing(8);
        btnAjouter = new QPushButton(corpsFormulaire);
        btnAjouter->setObjectName("btnAjouter");
        btnAjouter->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));
        btnAjouter->setMinimumSize(QSize(0, 42));
        btnAjouter->setMaximumSize(QSize(16777215, 42));

        gridBoutons->addWidget(btnAjouter, 0, 0, 1, 1);

        btnModifier = new QPushButton(corpsFormulaire);
        btnModifier->setObjectName("btnModifier");
        btnModifier->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));
        btnModifier->setMinimumSize(QSize(0, 42));
        btnModifier->setMaximumSize(QSize(16777215, 42));

        gridBoutons->addWidget(btnModifier, 0, 1, 1, 1);

        btnSupprimer = new QPushButton(corpsFormulaire);
        btnSupprimer->setObjectName("btnSupprimer");
        btnSupprimer->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));
        btnSupprimer->setMinimumSize(QSize(0, 42));
        btnSupprimer->setMaximumSize(QSize(16777215, 42));

        gridBoutons->addWidget(btnSupprimer, 0, 2, 1, 1);

        btnAfficher = new QPushButton(corpsFormulaire);
        btnAfficher->setObjectName("btnAfficher");
        btnAfficher->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));
        btnAfficher->setMinimumSize(QSize(0, 42));
        btnAfficher->setMaximumSize(QSize(16777215, 42));

        gridBoutons->addWidget(btnAfficher, 1, 1, 1, 1);


        layoutCorpsFormulaire->addLayout(gridBoutons);


        layoutCarteFormulaire->addWidget(corpsFormulaire);


        layoutMilieu->addWidget(carteFormulaire);

        carteListe = new QFrame(scrollContents);
        carteListe->setObjectName("carteListe");
        layoutCarteListe = new QVBoxLayout(carteListe);
        layoutCarteListe->setSpacing(0);
        layoutCarteListe->setObjectName("layoutCarteListe");
        layoutCarteListe->setContentsMargins(0, 0, 0, 0);
        enteteListe = new QFrame(carteListe);
        enteteListe->setObjectName("enteteListe");
        enteteListe->setMinimumSize(QSize(0, 46));
        enteteListe->setMaximumSize(QSize(16777215, 46));
        layoutEnteteListe = new QHBoxLayout(enteteListe);
        layoutEnteteListe->setSpacing(14);
        layoutEnteteListe->setObjectName("layoutEnteteListe");
        layoutEnteteListe->setContentsMargins(20, 0, 14, 0);
        iconeListe = new QLabel(enteteListe);
        iconeListe->setObjectName("iconeListe");

        layoutEnteteListe->addWidget(iconeListe);

        titreListe = new QLabel(enteteListe);
        titreListe->setObjectName("titreListe");

        layoutEnteteListe->addWidget(titreListe);

        spacerEnteteListe = new QSpacerItem(20, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        layoutEnteteListe->addItem(spacerEnteteListe);

        btnExporterPdf = new QPushButton(enteteListe);
        btnExporterPdf->setObjectName("btnExporterPdf");
        btnExporterPdf->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));
        btnExporterPdf->setMinimumSize(QSize(0, 34));
        btnExporterPdf->setMaximumSize(QSize(16777215, 34));

        layoutEnteteListe->addWidget(btnExporterPdf);


        layoutCarteListe->addWidget(enteteListe);

        corpsListe = new QWidget(carteListe);
        corpsListe->setObjectName("corpsListe");
        layoutCorpsListe = new QVBoxLayout(corpsListe);
        layoutCorpsListe->setSpacing(14);
        layoutCorpsListe->setObjectName("layoutCorpsListe");
        layoutCorpsListe->setContentsMargins(16, 14, 16, 14);
        layoutRechercheTri = new QHBoxLayout();
        layoutRechercheTri->setSpacing(16);
        layoutRechercheTri->setObjectName("layoutRechercheTri");
        editRechercheListe = new QLineEdit(corpsListe);
        editRechercheListe->setObjectName("editRechercheListe");
        editRechercheListe->setMinimumSize(QSize(0, 36));
        editRechercheListe->setMaximumSize(QSize(16777215, 36));

        layoutRechercheTri->addWidget(editRechercheListe);

        labelTri = new QLabel(corpsListe);
        labelTri->setObjectName("labelTri");

        layoutRechercheTri->addWidget(labelTri);

        comboTri = new QComboBox(corpsListe);
        comboTri->addItem(QString());
        comboTri->addItem(QString());
        comboTri->addItem(QString());
        comboTri->addItem(QString());
        comboTri->setObjectName("comboTri");
        comboTri->setMinimumSize(QSize(215, 36));
        comboTri->setMaximumSize(QSize(215, 36));

        layoutRechercheTri->addWidget(comboTri);


        layoutCorpsListe->addLayout(layoutRechercheTri);

        tableMaquettes = new QTableWidget(corpsListe);
        if (tableMaquettes->columnCount() < 7)
            tableMaquettes->setColumnCount(7);
        QTableWidgetItem *__qtablewidgetitem = new QTableWidgetItem();
        tableMaquettes->setHorizontalHeaderItem(0, __qtablewidgetitem);
        QTableWidgetItem *__qtablewidgetitem1 = new QTableWidgetItem();
        tableMaquettes->setHorizontalHeaderItem(1, __qtablewidgetitem1);
        QTableWidgetItem *__qtablewidgetitem2 = new QTableWidgetItem();
        tableMaquettes->setHorizontalHeaderItem(2, __qtablewidgetitem2);
        QTableWidgetItem *__qtablewidgetitem3 = new QTableWidgetItem();
        tableMaquettes->setHorizontalHeaderItem(3, __qtablewidgetitem3);
        QTableWidgetItem *__qtablewidgetitem4 = new QTableWidgetItem();
        tableMaquettes->setHorizontalHeaderItem(4, __qtablewidgetitem4);
        QTableWidgetItem *__qtablewidgetitem5 = new QTableWidgetItem();
        tableMaquettes->setHorizontalHeaderItem(5, __qtablewidgetitem5);
        QTableWidgetItem *__qtablewidgetitem6 = new QTableWidgetItem();
        tableMaquettes->setHorizontalHeaderItem(6, __qtablewidgetitem6);
        tableMaquettes->setObjectName("tableMaquettes");
        tableMaquettes->setEditTriggers(QAbstractItemView::NoEditTriggers);
        tableMaquettes->setSelectionMode(QAbstractItemView::SingleSelection);
        tableMaquettes->setSelectionBehavior(QAbstractItemView::SelectRows);
        tableMaquettes->setShowGrid(false);
        tableMaquettes->setWordWrap(false);
        tableMaquettes->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        tableMaquettes->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        tableMaquettes->setFocusPolicy(Qt::NoFocus);
        tableMaquettes->horizontalHeader()->setHighlightSections(false);
        tableMaquettes->verticalHeader()->setVisible(false);
        tableMaquettes->verticalHeader()->setDefaultSectionSize(47);

        layoutCorpsListe->addWidget(tableMaquettes);

        layoutBasListe = new QHBoxLayout();
        layoutBasListe->setObjectName("layoutBasListe");
        labelRetard = new QLabel(corpsListe);
        labelRetard->setObjectName("labelRetard");

        layoutBasListe->addWidget(labelRetard);

        spacerPagination = new QSpacerItem(20, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        layoutBasListe->addItem(spacerPagination);

        layoutPagination = new QHBoxLayout();
        layoutPagination->setSpacing(6);
        layoutPagination->setObjectName("layoutPagination");

        layoutBasListe->addLayout(layoutPagination);


        layoutCorpsListe->addLayout(layoutBasListe);

        spacerListe = new QSpacerItem(20, 20, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        layoutCorpsListe->addItem(spacerListe);


        layoutCarteListe->addWidget(corpsListe);


        layoutMilieu->addWidget(carteListe);

        layoutMilieu->setStretch(0, 47);
        layoutMilieu->setStretch(1, 55);

        layoutPageMaquettes->addLayout(layoutMilieu);

        layoutBas = new QHBoxLayout();
        layoutBas->setObjectName("layoutBas");
        layoutBas->setContentsMargins(14, 0, 14, 0);
        carteStats = new QFrame(scrollContents);
        carteStats->setObjectName("carteStats");
        layoutCarteStats = new QVBoxLayout(carteStats);
        layoutCarteStats->setSpacing(0);
        layoutCarteStats->setObjectName("layoutCarteStats");
        layoutCarteStats->setContentsMargins(0, 0, 0, 0);
        enteteStats = new QFrame(carteStats);
        enteteStats->setObjectName("enteteStats");
        enteteStats->setMinimumSize(QSize(0, 46));
        enteteStats->setMaximumSize(QSize(16777215, 46));
        layoutEnteteStats = new QHBoxLayout(enteteStats);
        layoutEnteteStats->setSpacing(14);
        layoutEnteteStats->setObjectName("layoutEnteteStats");
        layoutEnteteStats->setContentsMargins(20, 0, 14, 0);
        iconeStats = new QLabel(enteteStats);
        iconeStats->setObjectName("iconeStats");

        layoutEnteteStats->addWidget(iconeStats);

        titreStats = new QLabel(enteteStats);
        titreStats->setObjectName("titreStats");

        layoutEnteteStats->addWidget(titreStats);

        spacerEnteteStats = new QSpacerItem(20, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        layoutEnteteStats->addItem(spacerEnteteStats);


        layoutCarteStats->addWidget(enteteStats);

        corpsStats = new QWidget(carteStats);
        corpsStats->setObjectName("corpsStats");
        layoutCorpsStats = new QHBoxLayout(corpsStats);
        layoutCorpsStats->setSpacing(18);
        layoutCorpsStats->setObjectName("layoutCorpsStats");
        layoutCorpsStats->setContentsMargins(16, 14, 16, 14);
        blocType = new QFrame(corpsStats);
        blocType->setObjectName("blocType");
        layout_blocType = new QVBoxLayout(blocType);
        layout_blocType->setObjectName("layout_blocType");
        layout_blocType->setContentsMargins(16, 12, 16, 10);
        titre_blocType = new QLabel(blocType);
        titre_blocType->setObjectName("titre_blocType");

        layout_blocType->addWidget(titre_blocType);

        chartType = new DonutChart(blocType);
        chartType->setObjectName("chartType");

        layout_blocType->addWidget(chartType);


        layoutCorpsStats->addWidget(blocType);

        blocEvolution = new QFrame(corpsStats);
        blocEvolution->setObjectName("blocEvolution");
        layout_blocEvolution = new QVBoxLayout(blocEvolution);
        layout_blocEvolution->setObjectName("layout_blocEvolution");
        layout_blocEvolution->setContentsMargins(16, 12, 16, 10);
        titre_blocEvolution = new QLabel(blocEvolution);
        titre_blocEvolution->setObjectName("titre_blocEvolution");

        layout_blocEvolution->addWidget(titre_blocEvolution);

        chartEvolution = new LineChart(blocEvolution);
        chartEvolution->setObjectName("chartEvolution");

        layout_blocEvolution->addWidget(chartEvolution);


        layoutCorpsStats->addWidget(blocEvolution);

        blocTop = new QFrame(corpsStats);
        blocTop->setObjectName("blocTop");
        layout_blocTop = new QVBoxLayout(blocTop);
        layout_blocTop->setObjectName("layout_blocTop");
        layout_blocTop->setContentsMargins(16, 12, 16, 10);
        titre_blocTop = new QLabel(blocTop);
        titre_blocTop->setObjectName("titre_blocTop");

        layout_blocTop->addWidget(titre_blocTop);

        chartTop = new BarChart(blocTop);
        chartTop->setObjectName("chartTop");

        layout_blocTop->addWidget(chartTop);


        layoutCorpsStats->addWidget(blocTop);

        layoutCorpsStats->setStretch(0, 37);
        layoutCorpsStats->setStretch(1, 41);
        layoutCorpsStats->setStretch(2, 38);

        layoutCarteStats->addWidget(corpsStats);


        layoutBas->addWidget(carteStats);


        layoutPageMaquettes->addLayout(layoutBas);

        spacerPage = new QSpacerItem(20, 20, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        layoutPageMaquettes->addItem(spacerPage);

        scrollMaquettes->setWidget(scrollContents);

        layoutMaquettes->addWidget(scrollMaquettes);

        stackedWidget->addWidget(pageMaquettes);
        pageMachines = new QWidget();
        pageMachines->setObjectName("pageMachines");
        layoutPageMachines = new QVBoxLayout(pageMachines);
        layoutPageMachines->setObjectName("layoutPageMachines");
        labelMachines = new QLabel(pageMachines);
        labelMachines->setObjectName("labelMachines");
        labelMachines->setAlignment(Qt::AlignCenter);

        layoutPageMachines->addWidget(labelMachines);

        stackedWidget->addWidget(pageMachines);
        pageArticles = new QWidget();
        pageArticles->setObjectName("pageArticles");
        layoutPageArticles = new QVBoxLayout(pageArticles);
        layoutPageArticles->setObjectName("layoutPageArticles");
        labelArticles = new QLabel(pageArticles);
        labelArticles->setObjectName("labelArticles");
        labelArticles->setAlignment(Qt::AlignCenter);

        layoutPageArticles->addWidget(labelArticles);

        stackedWidget->addWidget(pageArticles);

        layoutZoneDroite->addWidget(stackedWidget);


        layoutCentral->addWidget(zoneDroite);

        MainWindow->setCentralWidget(centralwidget);

        retranslateUi(MainWindow);

        stackedWidget->setCurrentIndex(3);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "FashioNova \342\200\224 Gestion des Maquettes", nullptr));
        labelLogo->setText(QString());
        btnClients->setText(QCoreApplication::translate("MainWindow", "  Clients", nullptr));
        btnEmployes->setText(QCoreApplication::translate("MainWindow", "  Employ\303\251s", nullptr));
        btnCommandes->setText(QCoreApplication::translate("MainWindow", "  Commandes", nullptr));
        btnMaquettes->setText(QCoreApplication::translate("MainWindow", "  Maquettes", nullptr));
        btnMachines->setText(QCoreApplication::translate("MainWindow", "  Machines", nullptr));
        btnArticles->setText(QCoreApplication::translate("MainWindow", "  Articles", nullptr));
        btnMenu->setText(QString());
        btnMenu->setProperty("role", QVariant(QCoreApplication::translate("MainWindow", "iconBtn", nullptr)));
        editRechercheGlobale->setPlaceholderText(QCoreApplication::translate("MainWindow", "Rechercher une maquette, un type, un statut ...", nullptr));
        btnCloche->setText(QString());
        btnCloche->setProperty("role", QVariant(QCoreApplication::translate("MainWindow", "iconBtn", nullptr)));
#if QT_CONFIG(tooltip)
        btnCloche->setToolTip(QCoreApplication::translate("MainWindow", "Maquettes en retard de validation", nullptr));
#endif // QT_CONFIG(tooltip)
        labelAvatar->setText(QString());
        labelUser->setText(QCoreApplication::translate("MainWindow", "Designer", nullptr));
        btnCompte->setText(QString());
        btnCompte->setProperty("role", QVariant(QCoreApplication::translate("MainWindow", "iconBtn", nullptr)));
        labelClients->setText(QCoreApplication::translate("MainWindow", "Module \302\253 Clients \302\273\n"
"Ce module est g\303\251r\303\251 par un autre membre de l'\303\251quipe.", nullptr));
        labelClients->setProperty("role", QVariant(QCoreApplication::translate("MainWindow", "placeholder", nullptr)));
        labelEmployes->setText(QCoreApplication::translate("MainWindow", "Module \302\253 Employ\303\251s \302\273\n"
"Ce module est g\303\251r\303\251 par un autre membre de l'\303\251quipe.", nullptr));
        labelEmployes->setProperty("role", QVariant(QCoreApplication::translate("MainWindow", "placeholder", nullptr)));
        labelCommandes->setText(QCoreApplication::translate("MainWindow", "Module \302\253 Commandes \302\273\n"
"Ce module est g\303\251r\303\251 par un autre membre de l'\303\251quipe.", nullptr));
        labelCommandes->setProperty("role", QVariant(QCoreApplication::translate("MainWindow", "placeholder", nullptr)));
        carteFormulaire->setProperty("role", QVariant(QCoreApplication::translate("MainWindow", "carte", nullptr)));
        enteteFormulaire->setProperty("role", QVariant(QCoreApplication::translate("MainWindow", "entete", nullptr)));
        iconeFormulaire->setText(QString());
        titreFormulaire->setText(QCoreApplication::translate("MainWindow", "Informations de la maquette", nullptr));
        titreFormulaire->setProperty("role", QVariant(QCoreApplication::translate("MainWindow", "titreCarte", nullptr)));
        btnReinitialiser->setText(QCoreApplication::translate("MainWindow", "R\303\251initialiser", nullptr));
        btnReinitialiser->setProperty("role", QVariant(QCoreApplication::translate("MainWindow", "btnBlanc", nullptr)));
        corpsFormulaire->setProperty("role", QVariant(QCoreApplication::translate("MainWindow", "corps", nullptr)));
        labelId->setText(QCoreApplication::translate("MainWindow", "ID Maquette", nullptr));
        labelId->setProperty("role", QVariant(QCoreApplication::translate("MainWindow", "labelChamp", nullptr)));
        champId->setProperty("role", QVariant(QCoreApplication::translate("MainWindow", "champ", nullptr)));
        iconeId->setText(QString());
        editId->setProperty("role", QVariant(QCoreApplication::translate("MainWindow", "interne", nullptr)));
        labelNom->setText(QCoreApplication::translate("MainWindow", "Nom", nullptr));
        labelNom->setProperty("role", QVariant(QCoreApplication::translate("MainWindow", "labelChamp", nullptr)));
        champNom->setProperty("role", QVariant(QCoreApplication::translate("MainWindow", "champ", nullptr)));
        iconeNom->setText(QString());
        editNom->setProperty("role", QVariant(QCoreApplication::translate("MainWindow", "interne", nullptr)));
        labelType->setText(QCoreApplication::translate("MainWindow", "Type", nullptr));
        labelType->setProperty("role", QVariant(QCoreApplication::translate("MainWindow", "labelChamp", nullptr)));
        champType->setProperty("role", QVariant(QCoreApplication::translate("MainWindow", "champ", nullptr)));
        iconeType->setText(QString());
        comboType->setItemText(0, QCoreApplication::translate("MainWindow", "Robe", nullptr));
        comboType->setItemText(1, QCoreApplication::translate("MainWindow", "Veste", nullptr));
        comboType->setItemText(2, QCoreApplication::translate("MainWindow", "Jupe", nullptr));

        comboType->setProperty("role", QVariant(QCoreApplication::translate("MainWindow", "interne", nullptr)));
        labelDate->setText(QCoreApplication::translate("MainWindow", "Date de cr\303\251ation", nullptr));
        labelDate->setProperty("role", QVariant(QCoreApplication::translate("MainWindow", "labelChamp", nullptr)));
        champDate->setProperty("role", QVariant(QCoreApplication::translate("MainWindow", "champ", nullptr)));
        btnCalendrier->setText(QString());
        btnCalendrier->setProperty("role", QVariant(QCoreApplication::translate("MainWindow", "iconBtn", nullptr)));
        dateCreation->setProperty("role", QVariant(QCoreApplication::translate("MainWindow", "interne", nullptr)));
        dateCreation->setDisplayFormat(QCoreApplication::translate("MainWindow", "dd/MM/yyyy", nullptr));
        labelStatut->setText(QCoreApplication::translate("MainWindow", "Statut", nullptr));
        labelStatut->setProperty("role", QVariant(QCoreApplication::translate("MainWindow", "labelChamp", nullptr)));
        champStatut->setProperty("role", QVariant(QCoreApplication::translate("MainWindow", "champ", nullptr)));
        pastilleStatut->setText(QString());
        comboStatut->setItemText(0, QCoreApplication::translate("MainWindow", "En cours", nullptr));
        comboStatut->setItemText(1, QCoreApplication::translate("MainWindow", "En attente", nullptr));
        comboStatut->setItemText(2, QCoreApplication::translate("MainWindow", "Valid\303\251e", nullptr));

        comboStatut->setProperty("role", QVariant(QCoreApplication::translate("MainWindow", "interneGras", nullptr)));
        labelDescription->setText(QCoreApplication::translate("MainWindow", "Description", nullptr));
        labelDescription->setProperty("role", QVariant(QCoreApplication::translate("MainWindow", "labelChamp", nullptr)));
        editDescription->setPlaceholderText(QCoreApplication::translate("MainWindow", "Description de la maquette ...", nullptr));
        btnPlus->setText(QString());
        btnPlus->setProperty("role", QVariant(QCoreApplication::translate("MainWindow", "btnPlus", nullptr)));
        btnAjouter->setText(QCoreApplication::translate("MainWindow", "Ajouter", nullptr));
        btnAjouter->setProperty("role", QVariant(QCoreApplication::translate("MainWindow", "btnPrimaire", nullptr)));
        btnModifier->setText(QCoreApplication::translate("MainWindow", "Modifier", nullptr));
        btnModifier->setProperty("role", QVariant(QCoreApplication::translate("MainWindow", "btnGris", nullptr)));
        btnSupprimer->setText(QCoreApplication::translate("MainWindow", "Supprimer", nullptr));
        btnSupprimer->setProperty("role", QVariant(QCoreApplication::translate("MainWindow", "btnBeige", nullptr)));
        btnAfficher->setText(QCoreApplication::translate("MainWindow", "Afficher", nullptr));
        btnAfficher->setProperty("role", QVariant(QCoreApplication::translate("MainWindow", "btnClair", nullptr)));
        carteListe->setProperty("role", QVariant(QCoreApplication::translate("MainWindow", "carte", nullptr)));
        enteteListe->setProperty("role", QVariant(QCoreApplication::translate("MainWindow", "entete", nullptr)));
        iconeListe->setText(QString());
        titreListe->setText(QCoreApplication::translate("MainWindow", "Liste des maquettes", nullptr));
        titreListe->setProperty("role", QVariant(QCoreApplication::translate("MainWindow", "titreCarte", nullptr)));
        btnExporterPdf->setText(QCoreApplication::translate("MainWindow", "Exporter PDF", nullptr));
        btnExporterPdf->setProperty("role", QVariant(QCoreApplication::translate("MainWindow", "btnPrimaire", nullptr)));
        corpsListe->setProperty("role", QVariant(QCoreApplication::translate("MainWindow", "corps", nullptr)));
        editRechercheListe->setPlaceholderText(QCoreApplication::translate("MainWindow", "Rechercher une maquette...", nullptr));
        labelTri->setText(QCoreApplication::translate("MainWindow", "Trier par :", nullptr));
        comboTri->setItemText(0, QCoreApplication::translate("MainWindow", "Choisir un crit\303\250re", nullptr));
        comboTri->setItemText(1, QCoreApplication::translate("MainWindow", "Date de cr\303\251ation", nullptr));
        comboTri->setItemText(2, QCoreApplication::translate("MainWindow", "Type", nullptr));
        comboTri->setItemText(3, QCoreApplication::translate("MainWindow", "Statut", nullptr));

        comboTri->setProperty("role", QVariant(QCoreApplication::translate("MainWindow", "tri", nullptr)));
        QTableWidgetItem *___qtablewidgetitem = tableMaquettes->horizontalHeaderItem(1);
        ___qtablewidgetitem->setText(QCoreApplication::translate("MainWindow", "ID", nullptr));
        QTableWidgetItem *___qtablewidgetitem1 = tableMaquettes->horizontalHeaderItem(2);
        ___qtablewidgetitem1->setText(QCoreApplication::translate("MainWindow", "Nom", nullptr));
        QTableWidgetItem *___qtablewidgetitem2 = tableMaquettes->horizontalHeaderItem(3);
        ___qtablewidgetitem2->setText(QCoreApplication::translate("MainWindow", "Type", nullptr));
        QTableWidgetItem *___qtablewidgetitem3 = tableMaquettes->horizontalHeaderItem(4);
        ___qtablewidgetitem3->setText(QCoreApplication::translate("MainWindow", "Date de cr\303\251ation", nullptr));
        QTableWidgetItem *___qtablewidgetitem4 = tableMaquettes->horizontalHeaderItem(5);
        ___qtablewidgetitem4->setText(QCoreApplication::translate("MainWindow", "Statut", nullptr));
        QTableWidgetItem *___qtablewidgetitem5 = tableMaquettes->horizontalHeaderItem(6);
        ___qtablewidgetitem5->setText(QCoreApplication::translate("MainWindow", "Aper\303\247u", nullptr));
        labelRetard->setText(QString());
        carteStats->setProperty("role", QVariant(QCoreApplication::translate("MainWindow", "carte", nullptr)));
        enteteStats->setProperty("role", QVariant(QCoreApplication::translate("MainWindow", "entete", nullptr)));
        iconeStats->setText(QString());
        titreStats->setText(QCoreApplication::translate("MainWindow", "Statistiques des maquettes", nullptr));
        titreStats->setProperty("role", QVariant(QCoreApplication::translate("MainWindow", "titreCarte", nullptr)));
        corpsStats->setProperty("role", QVariant(QCoreApplication::translate("MainWindow", "corps", nullptr)));
        blocType->setProperty("role", QVariant(QCoreApplication::translate("MainWindow", "blocStat", nullptr)));
        titre_blocType->setText(QCoreApplication::translate("MainWindow", "R\303\251partition par type", nullptr));
        titre_blocType->setProperty("role", QVariant(QCoreApplication::translate("MainWindow", "titreStat", nullptr)));
        blocEvolution->setProperty("role", QVariant(QCoreApplication::translate("MainWindow", "blocStat", nullptr)));
        titre_blocEvolution->setText(QCoreApplication::translate("MainWindow", "\303\211volution des maquettes", nullptr));
        titre_blocEvolution->setProperty("role", QVariant(QCoreApplication::translate("MainWindow", "titreStat", nullptr)));
        blocTop->setProperty("role", QVariant(QCoreApplication::translate("MainWindow", "blocStat", nullptr)));
        titre_blocTop->setText(QCoreApplication::translate("MainWindow", "Top 5 des maquettes", nullptr));
        titre_blocTop->setProperty("role", QVariant(QCoreApplication::translate("MainWindow", "titreStat", nullptr)));
        labelMachines->setText(QCoreApplication::translate("MainWindow", "Module \302\253 Machines \302\273\n"
"Ce module est g\303\251r\303\251 par un autre membre de l'\303\251quipe.", nullptr));
        labelMachines->setProperty("role", QVariant(QCoreApplication::translate("MainWindow", "placeholder", nullptr)));
        labelArticles->setText(QCoreApplication::translate("MainWindow", "Module \302\253 Articles \302\273\n"
"Ce module est g\303\251r\303\251 par un autre membre de l'\303\251quipe.", nullptr));
        labelArticles->setProperty("role", QVariant(QCoreApplication::translate("MainWindow", "placeholder", nullptr)));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
