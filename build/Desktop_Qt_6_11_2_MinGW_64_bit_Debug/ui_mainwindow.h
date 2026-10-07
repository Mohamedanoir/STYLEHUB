/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.11.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QDate>
#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDateEdit>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "articlespage.h"

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QHBoxLayout *mainLayout;
    QFrame *sidebar;
    QVBoxLayout *sidebarLayout;
    QPushButton *btnNavClients;
    QPushButton *btnNavEmployes;
    QPushButton *btnNavCommandes;
    QPushButton *btnNavMaquettes;
    QPushButton *btnNavMachines;
    QPushButton *btnNavArticles;
    QSpacerItem *sidebarSpacer;
    QWidget *content;
    QVBoxLayout *contentLayout;
    QHBoxLayout *topBar;
    QPushButton *btnMenu;
    QLineEdit *searchGlobal;
    QSpacerItem *topSpacer;
    QPushButton *btnNotif;
    QPushButton *btnUser;
    QStackedWidget *stackPages;
    QScrollArea *pageClients;
    QWidget *pageClientsContents;
    QVBoxLayout *pageClientsLayout;
    QLabel *bannerImage;
    QHBoxLayout *middleLayout;
    QGroupBox *groupInfos;
    QVBoxLayout *infosLayout;
    QHBoxLayout *infosHeader;
    QSpacerItem *infosHeaderSpacer;
    QPushButton *btnReinit;
    QFormLayout *formLayout;
    QLabel *lblId;
    QLineEdit *editId;
    QLabel *lblNom;
    QLineEdit *editNom;
    QLabel *lblPrenom;
    QLineEdit *editPrenom;
    QLabel *lblTel;
    QLineEdit *editTel;
    QLabel *lblEmail;
    QLineEdit *editEmail;
    QLabel *lblAdresse;
    QLineEdit *editAdresse;
    QLabel *lblType;
    QComboBox *comboType;
    QLabel *lblDate;
    QDateEdit *dateInscription;
    QLabel *lblRemarques;
    QPlainTextEdit *editRemarques;
    QGridLayout *buttonsLayout;
    QPushButton *btnAjouter;
    QPushButton *btnModifier;
    QPushButton *btnSupprimer;
    QPushButton *btnAfficher;
    QSpacerItem *infosSpacer;
    QGroupBox *groupListe;
    QVBoxLayout *listeLayout;
    QHBoxLayout *listeHeader;
    QLineEdit *searchListe;
    QLabel *lblTrier;
    QComboBox *comboTrier;
    QPushButton *btnExport;
    QTableWidget *tableClients;
    QHBoxLayout *paginationLayout;
    QSpacerItem *pagSpacer;
    QPushButton *btnPagePrev;
    QPushButton *btnPage1;
    QPushButton *btnPage2;
    QPushButton *btnPage3;
    QPushButton *btnPageDots;
    QPushButton *btnPage10;
    QPushButton *btnPageNext;
    QGroupBox *groupHistorique;
    QVBoxLayout *historiqueLayout;
    QHBoxLayout *historiqueHeader;
    QLabel *lblNbCommandes;
    QLabel *lblTotalDepense;
    QLabel *lblDerniereCommande;
    QSpacerItem *historiqueSpacer;
    QComboBox *comboStatutCmd;
    QPushButton *btnVoirCommandes;
    QTableWidget *tableCommandes;
    QWidget *statsPanel;
    QHBoxLayout *statsLayout;
    QLabel *statsRepartition;
    QLabel *statsEvolution;
    QLabel *statsTop5;
    QFrame *pageEmployes;
    QFrame *pageCommandes;
    QFrame *pageMaquettes;
    QFrame *pageMachines;
    ArticlesPage *pageArticles;
    QButtonGroup *navGroup;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(1540, 960);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/images/logo.png"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        MainWindow->setWindowIcon(icon);
        MainWindow->setStyleSheet(QString::fromUtf8("\n"
"QMainWindow, QWidget#centralwidget { background-color: #f6f1e9; }\n"
"QPushButton[nav=\"true\"] { color: #FFFFFF; background: transparent; border: none; text-align: left; padding-left: 20px; margin: 0px 6px; border-radius: 6px; font-family: \"Segoe UI\"; font-size: 13px; font-weight: 500; }\n"
"QPushButton[nav=\"true\"]:hover { background-color: rgba(255, 255, 255, 0.12); }\n"
"QPushButton[nav=\"true\"]:checked { background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #E5C378, stop:1 #C89538); color: #08213B; font-weight: bold; }\n"
"QFrame#banner { background-color: #efe3cf; border: none; }\n"
"QLabel#bannerTitle { font-size: 38px; font-style: italic; font-weight: bold; color: #0b2a4a; }\n"
"QLabel#bannerSub { font-size: 15px; font-style: italic; color: #b08d57; }\n"
"QLabel#bannerQuote { font-size: 16px; font-style: italic; color: #b08d57; }\n"
"QGroupBox { background-color: white; border: 1px solid #e6dccb; border-radius: 8px; margin-top: 28px; font-weight: bold; font-size: 15px; color: #0b2a4a; }\n"
""
                        "QGroupBox::title { subcontrol-origin: margin; subcontrol-position: top left; left: 0px; padding: 6px 14px; background-color: #efe3cf; border-radius: 6px; }\n"
"QLineEdit, QComboBox, QDateEdit, QPlainTextEdit { background: white; border: 1px solid #ddd3c2; border-radius: 6px; padding: 6px; font-weight: normal; color: #222; }\n"
"QLabel[field=\"true\"] { font-weight: bold; color: #0b2a4a; font-size: 13px; }\n"
"QPushButton#btnAjouter { background-color: #0b2a4a; color: white; font-weight: bold; padding: 10px; border-radius: 6px; }\n"
"QPushButton#btnModifier, QPushButton#btnAfficher { background-color: #d9dde2; color: #333; font-weight: bold; padding: 10px; border-radius: 6px; }\n"
"QPushButton#btnSupprimer { background-color: #efe3cf; color: #0b2a4a; font-weight: bold; padding: 10px; border-radius: 6px; }\n"
"QPushButton#btnReinit { background-color: white; border: 1px solid #e6dccb; padding: 6px 12px; border-radius: 6px; color: #0b2a4a; }\n"
"QPushButton#btnExport { background-color: #0b2a4a; color: white; fon"
                        "t-weight: bold; padding: 8px 18px; border-radius: 6px; }\n"
"QTableWidget { background: white; border: 1px solid #e6dccb; gridline-color: #eee6d8; selection-background-color: #efe3cf; selection-color: #0b2a4a; }\n"
"QHeaderView::section { background-color: #f6f1e9; color: #0b2a4a; font-weight: bold; padding: 6px; border: none; border-bottom: 1px solid #e6dccb; }\n"
"QPushButton[page=\"true\"] { background: white; border: 1px solid #e6dccb; border-radius: 6px; min-width: 30px; padding: 6px; color: #0b2a4a; }\n"
"QPushButton#btnPage1 { background-color: #0b2a4a; color: white; }\n"
"QProgressBar { border: none; background-color: #ebeef2; border-radius: 5px; max-height: 10px; }\n"
"QProgressBar::chunk { background-color: #0b2a4a; border-radius: 5px; }\n"
"QFrame[card=\"true\"] { background: white; border: 1px solid #eee6d8; border-radius: 6px; }\n"
"   "));
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        mainLayout = new QHBoxLayout(centralwidget);
        mainLayout->setSpacing(0);
        mainLayout->setObjectName("mainLayout");
        mainLayout->setContentsMargins(0, 0, 0, 0);
        sidebar = new QFrame(centralwidget);
        sidebar->setObjectName("sidebar");
        sidebar->setMinimumSize(QSize(200, 0));
        sidebar->setMaximumSize(QSize(200, 16777215));
        sidebar->setStyleSheet(QString::fromUtf8("QFrame#sidebar { border-image: url(:/images/sidebar.png) 0 0 0 0 stretch stretch; }"));
        sidebarLayout = new QVBoxLayout(sidebar);
        sidebarLayout->setSpacing(6);
        sidebarLayout->setObjectName("sidebarLayout");
        sidebarLayout->setContentsMargins(0, 175, 0, 15);
        btnNavClients = new QPushButton(sidebar);
        navGroup = new QButtonGroup(MainWindow);
        navGroup->setObjectName("navGroup");
        navGroup->addButton(btnNavClients);
        btnNavClients->setObjectName("btnNavClients");
        btnNavClients->setMinimumSize(QSize(0, 40));
        btnNavClients->setMaximumSize(QSize(16777215, 40));
        btnNavClients->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/icons/clients_off.png"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        icon1.addFile(QString::fromUtf8(":/icons/clients_on.png"), QSize(), QIcon::Mode::Normal, QIcon::State::On);
        btnNavClients->setIcon(icon1);
        btnNavClients->setIconSize(QSize(20, 20));
        btnNavClients->setCheckable(true);
        btnNavClients->setChecked(true);
        btnNavClients->setProperty("nav", QVariant(QString::fromUtf8("true")));

        sidebarLayout->addWidget(btnNavClients);

        btnNavEmployes = new QPushButton(sidebar);
        navGroup->addButton(btnNavEmployes);
        btnNavEmployes->setObjectName("btnNavEmployes");
        btnNavEmployes->setMinimumSize(QSize(0, 40));
        btnNavEmployes->setMaximumSize(QSize(16777215, 40));
        btnNavEmployes->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));
        QIcon icon2;
        icon2.addFile(QString::fromUtf8(":/icons/employes_off.png"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        icon2.addFile(QString::fromUtf8(":/icons/employes_on.png"), QSize(), QIcon::Mode::Normal, QIcon::State::On);
        btnNavEmployes->setIcon(icon2);
        btnNavEmployes->setIconSize(QSize(20, 20));
        btnNavEmployes->setCheckable(true);
        btnNavEmployes->setProperty("nav", QVariant(QString::fromUtf8("true")));

        sidebarLayout->addWidget(btnNavEmployes);

        btnNavCommandes = new QPushButton(sidebar);
        navGroup->addButton(btnNavCommandes);
        btnNavCommandes->setObjectName("btnNavCommandes");
        btnNavCommandes->setMinimumSize(QSize(0, 40));
        btnNavCommandes->setMaximumSize(QSize(16777215, 40));
        btnNavCommandes->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));
        QIcon icon3;
        icon3.addFile(QString::fromUtf8(":/icons/commandes_off.png"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        icon3.addFile(QString::fromUtf8(":/icons/commandes_on.png"), QSize(), QIcon::Mode::Normal, QIcon::State::On);
        btnNavCommandes->setIcon(icon3);
        btnNavCommandes->setIconSize(QSize(20, 20));
        btnNavCommandes->setCheckable(true);
        btnNavCommandes->setProperty("nav", QVariant(QString::fromUtf8("true")));

        sidebarLayout->addWidget(btnNavCommandes);

        btnNavMaquettes = new QPushButton(sidebar);
        navGroup->addButton(btnNavMaquettes);
        btnNavMaquettes->setObjectName("btnNavMaquettes");
        btnNavMaquettes->setMinimumSize(QSize(0, 40));
        btnNavMaquettes->setMaximumSize(QSize(16777215, 40));
        btnNavMaquettes->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));
        QIcon icon4;
        icon4.addFile(QString::fromUtf8(":/icons/maquettes_off.png"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        icon4.addFile(QString::fromUtf8(":/icons/maquettes_on.png"), QSize(), QIcon::Mode::Normal, QIcon::State::On);
        btnNavMaquettes->setIcon(icon4);
        btnNavMaquettes->setIconSize(QSize(20, 20));
        btnNavMaquettes->setCheckable(true);
        btnNavMaquettes->setProperty("nav", QVariant(QString::fromUtf8("true")));

        sidebarLayout->addWidget(btnNavMaquettes);

        btnNavMachines = new QPushButton(sidebar);
        navGroup->addButton(btnNavMachines);
        btnNavMachines->setObjectName("btnNavMachines");
        btnNavMachines->setMinimumSize(QSize(0, 40));
        btnNavMachines->setMaximumSize(QSize(16777215, 40));
        btnNavMachines->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));
        QIcon icon5;
        icon5.addFile(QString::fromUtf8(":/icons/machines_off.png"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        icon5.addFile(QString::fromUtf8(":/icons/machines_on.png"), QSize(), QIcon::Mode::Normal, QIcon::State::On);
        btnNavMachines->setIcon(icon5);
        btnNavMachines->setIconSize(QSize(20, 20));
        btnNavMachines->setCheckable(true);
        btnNavMachines->setProperty("nav", QVariant(QString::fromUtf8("true")));

        sidebarLayout->addWidget(btnNavMachines);

        btnNavArticles = new QPushButton(sidebar);
        navGroup->addButton(btnNavArticles);
        btnNavArticles->setObjectName("btnNavArticles");
        btnNavArticles->setMinimumSize(QSize(0, 40));
        btnNavArticles->setMaximumSize(QSize(16777215, 40));
        btnNavArticles->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));
        QIcon icon6;
        icon6.addFile(QString::fromUtf8(":/icons/articles_off.png"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        icon6.addFile(QString::fromUtf8(":/icons/articles_on.png"), QSize(), QIcon::Mode::Normal, QIcon::State::On);
        btnNavArticles->setIcon(icon6);
        btnNavArticles->setIconSize(QSize(20, 20));
        btnNavArticles->setCheckable(true);
        btnNavArticles->setProperty("nav", QVariant(QString::fromUtf8("true")));

        sidebarLayout->addWidget(btnNavArticles);

        sidebarSpacer = new QSpacerItem(20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        sidebarLayout->addItem(sidebarSpacer);


        mainLayout->addWidget(sidebar);

        content = new QWidget(centralwidget);
        content->setObjectName("content");
        contentLayout = new QVBoxLayout(content);
        contentLayout->setSpacing(10);
        contentLayout->setObjectName("contentLayout");
        contentLayout->setContentsMargins(0, 8, 10, 10);
        topBar = new QHBoxLayout();
        topBar->setObjectName("topBar");
        btnMenu = new QPushButton(content);
        btnMenu->setObjectName("btnMenu");
        btnMenu->setFlat(true);

        topBar->addWidget(btnMenu);

        searchGlobal = new QLineEdit(content);
        searchGlobal->setObjectName("searchGlobal");

        topBar->addWidget(searchGlobal);

        topSpacer = new QSpacerItem(200, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        topBar->addItem(topSpacer);

        btnNotif = new QPushButton(content);
        btnNotif->setObjectName("btnNotif");
        btnNotif->setFlat(true);

        topBar->addWidget(btnNotif);

        btnUser = new QPushButton(content);
        btnUser->setObjectName("btnUser");
        btnUser->setFlat(true);

        topBar->addWidget(btnUser);


        contentLayout->addLayout(topBar);

        stackPages = new QStackedWidget(content);
        stackPages->setObjectName("stackPages");
        pageClients = new QScrollArea();
        pageClients->setObjectName("pageClients");
        pageClients->setFrameShape(QFrame::NoFrame);
        pageClients->setWidgetResizable(true);
        pageClients->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        pageClients->setStyleSheet(QString::fromUtf8("QScrollArea#pageClients { background: transparent; border: none; }\n"
"QScrollArea#pageClients > QWidget { background: transparent; }\n"
"QWidget#pageClientsContents { background: transparent; }"));
        pageClientsContents = new QWidget();
        pageClientsContents->setObjectName("pageClientsContents");
        pageClientsLayout = new QVBoxLayout(pageClientsContents);
        pageClientsLayout->setSpacing(10);
        pageClientsLayout->setObjectName("pageClientsLayout");
        pageClientsLayout->setContentsMargins(0, 0, 0, 0);
        bannerImage = new QLabel(pageClientsContents);
        bannerImage->setObjectName("bannerImage");
        QSizePolicy sizePolicy(QSizePolicy::Policy::Ignored, QSizePolicy::Policy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(bannerImage->sizePolicy().hasHeightForWidth());
        bannerImage->setSizePolicy(sizePolicy);
        bannerImage->setMinimumSize(QSize(0, 135));
        bannerImage->setMaximumSize(QSize(16777215, 135));
        bannerImage->setPixmap(QPixmap(QString::fromUtf8(":/images/banner.png")));
        bannerImage->setAlignment(Qt::AlignCenter);

        pageClientsLayout->addWidget(bannerImage);

        middleLayout = new QHBoxLayout();
        middleLayout->setSpacing(12);
        middleLayout->setObjectName("middleLayout");
        groupInfos = new QGroupBox(pageClientsContents);
        groupInfos->setObjectName("groupInfos");
        infosLayout = new QVBoxLayout(groupInfos);
        infosLayout->setObjectName("infosLayout");
        infosHeader = new QHBoxLayout();
        infosHeader->setObjectName("infosHeader");
        infosHeaderSpacer = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        infosHeader->addItem(infosHeaderSpacer);

        btnReinit = new QPushButton(groupInfos);
        btnReinit->setObjectName("btnReinit");

        infosHeader->addWidget(btnReinit);


        infosLayout->addLayout(infosHeader);

        formLayout = new QFormLayout();
        formLayout->setObjectName("formLayout");
        formLayout->setVerticalSpacing(12);
        formLayout->setHorizontalSpacing(20);
        lblId = new QLabel(groupInfos);
        lblId->setObjectName("lblId");
        lblId->setProperty("field", QVariant(QString::fromUtf8("true")));

        formLayout->setWidget(0, QFormLayout::ItemRole::LabelRole, lblId);

        editId = new QLineEdit(groupInfos);
        editId->setObjectName("editId");

        formLayout->setWidget(0, QFormLayout::ItemRole::FieldRole, editId);

        lblNom = new QLabel(groupInfos);
        lblNom->setObjectName("lblNom");
        lblNom->setProperty("field", QVariant(QString::fromUtf8("true")));

        formLayout->setWidget(1, QFormLayout::ItemRole::LabelRole, lblNom);

        editNom = new QLineEdit(groupInfos);
        editNom->setObjectName("editNom");

        formLayout->setWidget(1, QFormLayout::ItemRole::FieldRole, editNom);

        lblPrenom = new QLabel(groupInfos);
        lblPrenom->setObjectName("lblPrenom");
        lblPrenom->setProperty("field", QVariant(QString::fromUtf8("true")));

        formLayout->setWidget(2, QFormLayout::ItemRole::LabelRole, lblPrenom);

        editPrenom = new QLineEdit(groupInfos);
        editPrenom->setObjectName("editPrenom");

        formLayout->setWidget(2, QFormLayout::ItemRole::FieldRole, editPrenom);

        lblTel = new QLabel(groupInfos);
        lblTel->setObjectName("lblTel");
        lblTel->setProperty("field", QVariant(QString::fromUtf8("true")));

        formLayout->setWidget(3, QFormLayout::ItemRole::LabelRole, lblTel);

        editTel = new QLineEdit(groupInfos);
        editTel->setObjectName("editTel");

        formLayout->setWidget(3, QFormLayout::ItemRole::FieldRole, editTel);

        lblEmail = new QLabel(groupInfos);
        lblEmail->setObjectName("lblEmail");
        lblEmail->setProperty("field", QVariant(QString::fromUtf8("true")));

        formLayout->setWidget(4, QFormLayout::ItemRole::LabelRole, lblEmail);

        editEmail = new QLineEdit(groupInfos);
        editEmail->setObjectName("editEmail");

        formLayout->setWidget(4, QFormLayout::ItemRole::FieldRole, editEmail);

        lblAdresse = new QLabel(groupInfos);
        lblAdresse->setObjectName("lblAdresse");
        lblAdresse->setProperty("field", QVariant(QString::fromUtf8("true")));

        formLayout->setWidget(5, QFormLayout::ItemRole::LabelRole, lblAdresse);

        editAdresse = new QLineEdit(groupInfos);
        editAdresse->setObjectName("editAdresse");

        formLayout->setWidget(5, QFormLayout::ItemRole::FieldRole, editAdresse);

        lblType = new QLabel(groupInfos);
        lblType->setObjectName("lblType");
        lblType->setProperty("field", QVariant(QString::fromUtf8("true")));

        formLayout->setWidget(6, QFormLayout::ItemRole::LabelRole, lblType);

        comboType = new QComboBox(groupInfos);
        comboType->addItem(QString());
        comboType->addItem(QString());
        comboType->addItem(QString());
        comboType->setObjectName("comboType");

        formLayout->setWidget(6, QFormLayout::ItemRole::FieldRole, comboType);

        lblDate = new QLabel(groupInfos);
        lblDate->setObjectName("lblDate");
        lblDate->setProperty("field", QVariant(QString::fromUtf8("true")));

        formLayout->setWidget(7, QFormLayout::ItemRole::LabelRole, lblDate);

        dateInscription = new QDateEdit(groupInfos);
        dateInscription->setObjectName("dateInscription");
        dateInscription->setCalendarPopup(true);
        dateInscription->setDate(QDate(2024, 3, 15));

        formLayout->setWidget(7, QFormLayout::ItemRole::FieldRole, dateInscription);

        lblRemarques = new QLabel(groupInfos);
        lblRemarques->setObjectName("lblRemarques");
        lblRemarques->setProperty("field", QVariant(QString::fromUtf8("true")));

        formLayout->setWidget(8, QFormLayout::ItemRole::LabelRole, lblRemarques);

        editRemarques = new QPlainTextEdit(groupInfos);
        editRemarques->setObjectName("editRemarques");
        editRemarques->setMaximumSize(QSize(16777215, 80));

        formLayout->setWidget(8, QFormLayout::ItemRole::FieldRole, editRemarques);


        infosLayout->addLayout(formLayout);

        buttonsLayout = new QGridLayout();
        buttonsLayout->setObjectName("buttonsLayout");
        btnAjouter = new QPushButton(groupInfos);
        btnAjouter->setObjectName("btnAjouter");

        buttonsLayout->addWidget(btnAjouter, 0, 0, 1, 1);

        btnModifier = new QPushButton(groupInfos);
        btnModifier->setObjectName("btnModifier");

        buttonsLayout->addWidget(btnModifier, 0, 1, 1, 1);

        btnSupprimer = new QPushButton(groupInfos);
        btnSupprimer->setObjectName("btnSupprimer");

        buttonsLayout->addWidget(btnSupprimer, 0, 2, 1, 1);

        btnAfficher = new QPushButton(groupInfos);
        btnAfficher->setObjectName("btnAfficher");

        buttonsLayout->addWidget(btnAfficher, 1, 1, 1, 1);


        infosLayout->addLayout(buttonsLayout);

        infosSpacer = new QSpacerItem(20, 20, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        infosLayout->addItem(infosSpacer);


        middleLayout->addWidget(groupInfos);

        groupListe = new QGroupBox(pageClientsContents);
        groupListe->setObjectName("groupListe");
        listeLayout = new QVBoxLayout(groupListe);
        listeLayout->setObjectName("listeLayout");
        listeHeader = new QHBoxLayout();
        listeHeader->setObjectName("listeHeader");
        searchListe = new QLineEdit(groupListe);
        searchListe->setObjectName("searchListe");

        listeHeader->addWidget(searchListe);

        lblTrier = new QLabel(groupListe);
        lblTrier->setObjectName("lblTrier");
        lblTrier->setProperty("field", QVariant(QString::fromUtf8("true")));

        listeHeader->addWidget(lblTrier);

        comboTrier = new QComboBox(groupListe);
        comboTrier->addItem(QString());
        comboTrier->addItem(QString());
        comboTrier->addItem(QString());
        comboTrier->setObjectName("comboTrier");

        listeHeader->addWidget(comboTrier);

        btnExport = new QPushButton(groupListe);
        btnExport->setObjectName("btnExport");

        listeHeader->addWidget(btnExport);


        listeLayout->addLayout(listeHeader);

        tableClients = new QTableWidget(groupListe);
        if (tableClients->columnCount() < 7)
            tableClients->setColumnCount(7);
        QTableWidgetItem *__qtablewidgetitem = new QTableWidgetItem();
        tableClients->setHorizontalHeaderItem(0, __qtablewidgetitem);
        QTableWidgetItem *__qtablewidgetitem1 = new QTableWidgetItem();
        tableClients->setHorizontalHeaderItem(1, __qtablewidgetitem1);
        QTableWidgetItem *__qtablewidgetitem2 = new QTableWidgetItem();
        tableClients->setHorizontalHeaderItem(2, __qtablewidgetitem2);
        QTableWidgetItem *__qtablewidgetitem3 = new QTableWidgetItem();
        tableClients->setHorizontalHeaderItem(3, __qtablewidgetitem3);
        QTableWidgetItem *__qtablewidgetitem4 = new QTableWidgetItem();
        tableClients->setHorizontalHeaderItem(4, __qtablewidgetitem4);
        QTableWidgetItem *__qtablewidgetitem5 = new QTableWidgetItem();
        tableClients->setHorizontalHeaderItem(5, __qtablewidgetitem5);
        QTableWidgetItem *__qtablewidgetitem6 = new QTableWidgetItem();
        tableClients->setHorizontalHeaderItem(6, __qtablewidgetitem6);
        if (tableClients->rowCount() < 6)
            tableClients->setRowCount(6);
        QTableWidgetItem *__qtablewidgetitem7 = new QTableWidgetItem();
        tableClients->setItem(0, 0, __qtablewidgetitem7);
        QTableWidgetItem *__qtablewidgetitem8 = new QTableWidgetItem();
        tableClients->setItem(0, 1, __qtablewidgetitem8);
        QTableWidgetItem *__qtablewidgetitem9 = new QTableWidgetItem();
        tableClients->setItem(0, 2, __qtablewidgetitem9);
        QTableWidgetItem *__qtablewidgetitem10 = new QTableWidgetItem();
        tableClients->setItem(0, 3, __qtablewidgetitem10);
        QTableWidgetItem *__qtablewidgetitem11 = new QTableWidgetItem();
        tableClients->setItem(0, 4, __qtablewidgetitem11);
        QTableWidgetItem *__qtablewidgetitem12 = new QTableWidgetItem();
        tableClients->setItem(0, 5, __qtablewidgetitem12);
        QTableWidgetItem *__qtablewidgetitem13 = new QTableWidgetItem();
        tableClients->setItem(0, 6, __qtablewidgetitem13);
        QTableWidgetItem *__qtablewidgetitem14 = new QTableWidgetItem();
        tableClients->setItem(1, 0, __qtablewidgetitem14);
        QTableWidgetItem *__qtablewidgetitem15 = new QTableWidgetItem();
        tableClients->setItem(1, 1, __qtablewidgetitem15);
        QTableWidgetItem *__qtablewidgetitem16 = new QTableWidgetItem();
        tableClients->setItem(1, 2, __qtablewidgetitem16);
        QTableWidgetItem *__qtablewidgetitem17 = new QTableWidgetItem();
        tableClients->setItem(1, 3, __qtablewidgetitem17);
        QTableWidgetItem *__qtablewidgetitem18 = new QTableWidgetItem();
        tableClients->setItem(1, 4, __qtablewidgetitem18);
        QTableWidgetItem *__qtablewidgetitem19 = new QTableWidgetItem();
        tableClients->setItem(1, 5, __qtablewidgetitem19);
        QTableWidgetItem *__qtablewidgetitem20 = new QTableWidgetItem();
        tableClients->setItem(1, 6, __qtablewidgetitem20);
        QTableWidgetItem *__qtablewidgetitem21 = new QTableWidgetItem();
        tableClients->setItem(2, 0, __qtablewidgetitem21);
        QTableWidgetItem *__qtablewidgetitem22 = new QTableWidgetItem();
        tableClients->setItem(2, 1, __qtablewidgetitem22);
        QTableWidgetItem *__qtablewidgetitem23 = new QTableWidgetItem();
        tableClients->setItem(2, 2, __qtablewidgetitem23);
        QTableWidgetItem *__qtablewidgetitem24 = new QTableWidgetItem();
        tableClients->setItem(2, 3, __qtablewidgetitem24);
        QTableWidgetItem *__qtablewidgetitem25 = new QTableWidgetItem();
        tableClients->setItem(2, 4, __qtablewidgetitem25);
        QTableWidgetItem *__qtablewidgetitem26 = new QTableWidgetItem();
        tableClients->setItem(2, 5, __qtablewidgetitem26);
        QTableWidgetItem *__qtablewidgetitem27 = new QTableWidgetItem();
        tableClients->setItem(2, 6, __qtablewidgetitem27);
        QTableWidgetItem *__qtablewidgetitem28 = new QTableWidgetItem();
        tableClients->setItem(3, 0, __qtablewidgetitem28);
        QTableWidgetItem *__qtablewidgetitem29 = new QTableWidgetItem();
        tableClients->setItem(3, 1, __qtablewidgetitem29);
        QTableWidgetItem *__qtablewidgetitem30 = new QTableWidgetItem();
        tableClients->setItem(3, 2, __qtablewidgetitem30);
        QTableWidgetItem *__qtablewidgetitem31 = new QTableWidgetItem();
        tableClients->setItem(3, 3, __qtablewidgetitem31);
        QTableWidgetItem *__qtablewidgetitem32 = new QTableWidgetItem();
        tableClients->setItem(3, 4, __qtablewidgetitem32);
        QTableWidgetItem *__qtablewidgetitem33 = new QTableWidgetItem();
        tableClients->setItem(3, 5, __qtablewidgetitem33);
        QTableWidgetItem *__qtablewidgetitem34 = new QTableWidgetItem();
        tableClients->setItem(3, 6, __qtablewidgetitem34);
        QTableWidgetItem *__qtablewidgetitem35 = new QTableWidgetItem();
        tableClients->setItem(4, 0, __qtablewidgetitem35);
        QTableWidgetItem *__qtablewidgetitem36 = new QTableWidgetItem();
        tableClients->setItem(4, 1, __qtablewidgetitem36);
        QTableWidgetItem *__qtablewidgetitem37 = new QTableWidgetItem();
        tableClients->setItem(4, 2, __qtablewidgetitem37);
        QTableWidgetItem *__qtablewidgetitem38 = new QTableWidgetItem();
        tableClients->setItem(4, 3, __qtablewidgetitem38);
        QTableWidgetItem *__qtablewidgetitem39 = new QTableWidgetItem();
        tableClients->setItem(4, 4, __qtablewidgetitem39);
        QTableWidgetItem *__qtablewidgetitem40 = new QTableWidgetItem();
        tableClients->setItem(4, 5, __qtablewidgetitem40);
        QTableWidgetItem *__qtablewidgetitem41 = new QTableWidgetItem();
        tableClients->setItem(4, 6, __qtablewidgetitem41);
        QTableWidgetItem *__qtablewidgetitem42 = new QTableWidgetItem();
        tableClients->setItem(5, 0, __qtablewidgetitem42);
        QTableWidgetItem *__qtablewidgetitem43 = new QTableWidgetItem();
        tableClients->setItem(5, 1, __qtablewidgetitem43);
        QTableWidgetItem *__qtablewidgetitem44 = new QTableWidgetItem();
        tableClients->setItem(5, 2, __qtablewidgetitem44);
        QTableWidgetItem *__qtablewidgetitem45 = new QTableWidgetItem();
        tableClients->setItem(5, 3, __qtablewidgetitem45);
        QTableWidgetItem *__qtablewidgetitem46 = new QTableWidgetItem();
        tableClients->setItem(5, 4, __qtablewidgetitem46);
        QTableWidgetItem *__qtablewidgetitem47 = new QTableWidgetItem();
        tableClients->setItem(5, 5, __qtablewidgetitem47);
        QTableWidgetItem *__qtablewidgetitem48 = new QTableWidgetItem();
        tableClients->setItem(5, 6, __qtablewidgetitem48);
        tableClients->setObjectName("tableClients");
        tableClients->setSelectionBehavior(QAbstractItemView::SelectRows);
        tableClients->setEditTriggers(QAbstractItemView::NoEditTriggers);
        tableClients->setAlternatingRowColors(false);
        tableClients->horizontalHeader()->setStretchLastSection(true);
        tableClients->verticalHeader()->setVisible(false);

        listeLayout->addWidget(tableClients);

        paginationLayout = new QHBoxLayout();
        paginationLayout->setObjectName("paginationLayout");
        pagSpacer = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        paginationLayout->addItem(pagSpacer);

        btnPagePrev = new QPushButton(groupListe);
        btnPagePrev->setObjectName("btnPagePrev");
        btnPagePrev->setProperty("page", QVariant(QString::fromUtf8("true")));

        paginationLayout->addWidget(btnPagePrev);

        btnPage1 = new QPushButton(groupListe);
        btnPage1->setObjectName("btnPage1");
        btnPage1->setProperty("page", QVariant(QString::fromUtf8("true")));

        paginationLayout->addWidget(btnPage1);

        btnPage2 = new QPushButton(groupListe);
        btnPage2->setObjectName("btnPage2");
        btnPage2->setProperty("page", QVariant(QString::fromUtf8("true")));

        paginationLayout->addWidget(btnPage2);

        btnPage3 = new QPushButton(groupListe);
        btnPage3->setObjectName("btnPage3");
        btnPage3->setProperty("page", QVariant(QString::fromUtf8("true")));

        paginationLayout->addWidget(btnPage3);

        btnPageDots = new QPushButton(groupListe);
        btnPageDots->setObjectName("btnPageDots");
        btnPageDots->setProperty("page", QVariant(QString::fromUtf8("true")));

        paginationLayout->addWidget(btnPageDots);

        btnPage10 = new QPushButton(groupListe);
        btnPage10->setObjectName("btnPage10");
        btnPage10->setProperty("page", QVariant(QString::fromUtf8("true")));

        paginationLayout->addWidget(btnPage10);

        btnPageNext = new QPushButton(groupListe);
        btnPageNext->setObjectName("btnPageNext");
        btnPageNext->setProperty("page", QVariant(QString::fromUtf8("true")));

        paginationLayout->addWidget(btnPageNext);


        listeLayout->addLayout(paginationLayout);


        middleLayout->addWidget(groupListe);


        pageClientsLayout->addLayout(middleLayout);

        groupHistorique = new QGroupBox(pageClientsContents);
        groupHistorique->setObjectName("groupHistorique");
        groupHistorique->setMinimumSize(QSize(0, 270));
        historiqueLayout = new QVBoxLayout(groupHistorique);
        historiqueLayout->setObjectName("historiqueLayout");
        historiqueHeader = new QHBoxLayout();
        historiqueHeader->setObjectName("historiqueHeader");
        lblNbCommandes = new QLabel(groupHistorique);
        lblNbCommandes->setObjectName("lblNbCommandes");
        lblNbCommandes->setStyleSheet(QString::fromUtf8("background-color:#f6f1e9; border:1px solid #e6dccb; border-radius:6px; padding:6px 14px; color:#0b2a4a; font-weight:bold;"));

        historiqueHeader->addWidget(lblNbCommandes);

        lblTotalDepense = new QLabel(groupHistorique);
        lblTotalDepense->setObjectName("lblTotalDepense");
        lblTotalDepense->setStyleSheet(QString::fromUtf8("background-color:#f6f1e9; border:1px solid #e6dccb; border-radius:6px; padding:6px 14px; color:#0b2a4a; font-weight:bold;"));

        historiqueHeader->addWidget(lblTotalDepense);

        lblDerniereCommande = new QLabel(groupHistorique);
        lblDerniereCommande->setObjectName("lblDerniereCommande");
        lblDerniereCommande->setStyleSheet(QString::fromUtf8("background-color:#f6f1e9; border:1px solid #e6dccb; border-radius:6px; padding:6px 14px; color:#0b2a4a; font-weight:bold;"));

        historiqueHeader->addWidget(lblDerniereCommande);

        historiqueSpacer = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        historiqueHeader->addItem(historiqueSpacer);

        comboStatutCmd = new QComboBox(groupHistorique);
        comboStatutCmd->addItem(QString());
        comboStatutCmd->addItem(QString());
        comboStatutCmd->addItem(QString());
        comboStatutCmd->addItem(QString());
        comboStatutCmd->addItem(QString());
        comboStatutCmd->setObjectName("comboStatutCmd");

        historiqueHeader->addWidget(comboStatutCmd);

        btnVoirCommandes = new QPushButton(groupHistorique);
        btnVoirCommandes->setObjectName("btnVoirCommandes");
        btnVoirCommandes->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));
        btnVoirCommandes->setStyleSheet(QString::fromUtf8("background-color:#0b2a4a; color:white; font-weight:bold; padding:8px 16px; border-radius:6px;"));

        historiqueHeader->addWidget(btnVoirCommandes);


        historiqueLayout->addLayout(historiqueHeader);

        tableCommandes = new QTableWidget(groupHistorique);
        if (tableCommandes->columnCount() < 5)
            tableCommandes->setColumnCount(5);
        QTableWidgetItem *__qtablewidgetitem49 = new QTableWidgetItem();
        tableCommandes->setHorizontalHeaderItem(0, __qtablewidgetitem49);
        QTableWidgetItem *__qtablewidgetitem50 = new QTableWidgetItem();
        tableCommandes->setHorizontalHeaderItem(1, __qtablewidgetitem50);
        QTableWidgetItem *__qtablewidgetitem51 = new QTableWidgetItem();
        tableCommandes->setHorizontalHeaderItem(2, __qtablewidgetitem51);
        QTableWidgetItem *__qtablewidgetitem52 = new QTableWidgetItem();
        tableCommandes->setHorizontalHeaderItem(3, __qtablewidgetitem52);
        QTableWidgetItem *__qtablewidgetitem53 = new QTableWidgetItem();
        tableCommandes->setHorizontalHeaderItem(4, __qtablewidgetitem53);
        tableCommandes->setObjectName("tableCommandes");
        tableCommandes->setMinimumSize(QSize(0, 160));
        tableCommandes->setSelectionBehavior(QAbstractItemView::SelectRows);
        tableCommandes->setEditTriggers(QAbstractItemView::NoEditTriggers);
        tableCommandes->horizontalHeader()->setStretchLastSection(true);
        tableCommandes->verticalHeader()->setVisible(false);

        historiqueLayout->addWidget(tableCommandes);


        pageClientsLayout->addWidget(groupHistorique);

        statsPanel = new QWidget(pageClientsContents);
        statsPanel->setObjectName("statsPanel");
        sizePolicy.setHeightForWidth(statsPanel->sizePolicy().hasHeightForWidth());
        statsPanel->setSizePolicy(sizePolicy);
        statsPanel->setMinimumSize(QSize(0, 150));
        statsLayout = new QHBoxLayout(statsPanel);
        statsLayout->setSpacing(12);
        statsLayout->setObjectName("statsLayout");
        statsLayout->setContentsMargins(0, 0, 0, 0);
        statsRepartition = new QLabel(statsPanel);
        statsRepartition->setObjectName("statsRepartition");
        QSizePolicy sizePolicy1(QSizePolicy::Policy::Ignored, QSizePolicy::Policy::Ignored);
        sizePolicy1.setHorizontalStretch(378);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(statsRepartition->sizePolicy().hasHeightForWidth());
        statsRepartition->setSizePolicy(sizePolicy1);
        statsRepartition->setPixmap(QPixmap(QString::fromUtf8(":/images/stats_repartition.png")));
        statsRepartition->setAlignment(Qt::AlignCenter);

        statsLayout->addWidget(statsRepartition);

        statsEvolution = new QLabel(statsPanel);
        statsEvolution->setObjectName("statsEvolution");
        QSizePolicy sizePolicy2(QSizePolicy::Policy::Ignored, QSizePolicy::Policy::Ignored);
        sizePolicy2.setHorizontalStretch(417);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(statsEvolution->sizePolicy().hasHeightForWidth());
        statsEvolution->setSizePolicy(sizePolicy2);
        statsEvolution->setPixmap(QPixmap(QString::fromUtf8(":/images/stats_evolution.png")));
        statsEvolution->setAlignment(Qt::AlignCenter);

        statsLayout->addWidget(statsEvolution);

        statsTop5 = new QLabel(statsPanel);
        statsTop5->setObjectName("statsTop5");
        QSizePolicy sizePolicy3(QSizePolicy::Policy::Ignored, QSizePolicy::Policy::Ignored);
        sizePolicy3.setHorizontalStretch(384);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(statsTop5->sizePolicy().hasHeightForWidth());
        statsTop5->setSizePolicy(sizePolicy3);
        statsTop5->setPixmap(QPixmap(QString::fromUtf8(":/images/stats_top5.png")));
        statsTop5->setAlignment(Qt::AlignCenter);

        statsLayout->addWidget(statsTop5);


        pageClientsLayout->addWidget(statsPanel);

        pageClients->setWidget(pageClientsContents);
        stackPages->addWidget(pageClients);
        pageEmployes = new QFrame();
        pageEmployes->setObjectName("pageEmployes");
        pageEmployes->setFrameShape(QFrame::NoFrame);
        pageEmployes->setStyleSheet(QString::fromUtf8("QFrame#pageEmployes { background-color: white; border-radius: 8px; }"));
        stackPages->addWidget(pageEmployes);
        pageCommandes = new QFrame();
        pageCommandes->setObjectName("pageCommandes");
        pageCommandes->setFrameShape(QFrame::NoFrame);
        pageCommandes->setStyleSheet(QString::fromUtf8("QFrame#pageCommandes { background-color: white; border-radius: 8px; }"));
        stackPages->addWidget(pageCommandes);
        pageMaquettes = new QFrame();
        pageMaquettes->setObjectName("pageMaquettes");
        pageMaquettes->setFrameShape(QFrame::NoFrame);
        pageMaquettes->setStyleSheet(QString::fromUtf8("QFrame#pageMaquettes { background-color: white; border-radius: 8px; }"));
        stackPages->addWidget(pageMaquettes);
        pageMachines = new QFrame();
        pageMachines->setObjectName("pageMachines");
        pageMachines->setFrameShape(QFrame::NoFrame);
        pageMachines->setStyleSheet(QString::fromUtf8("QFrame#pageMachines { background-color: white; border-radius: 8px; }"));
        stackPages->addWidget(pageMachines);
        pageArticles = new ArticlesPage();
        pageArticles->setObjectName("pageArticles");
        stackPages->addWidget(pageArticles);

        contentLayout->addWidget(stackPages);


        mainLayout->addWidget(content);

        MainWindow->setCentralWidget(centralwidget);

        retranslateUi(MainWindow);

        stackPages->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "FashioNova - Gestion des Clients (v9)", nullptr));
        btnNavClients->setText(QCoreApplication::translate("MainWindow", "  Clients", nullptr));
        btnNavEmployes->setText(QCoreApplication::translate("MainWindow", "  Employ\303\251s", nullptr));
        btnNavCommandes->setText(QCoreApplication::translate("MainWindow", "  Commandes", nullptr));
        btnNavMaquettes->setText(QCoreApplication::translate("MainWindow", "  Maquettes", nullptr));
        btnNavMachines->setText(QCoreApplication::translate("MainWindow", "  Machines", nullptr));
        btnNavArticles->setText(QCoreApplication::translate("MainWindow", "  Articles", nullptr));
        btnMenu->setText(QCoreApplication::translate("MainWindow", "\342\230\260", nullptr));
        searchGlobal->setPlaceholderText(QCoreApplication::translate("MainWindow", "Rechercher un client, un nom, un num\303\251ro de t\303\251l\303\251phone...", nullptr));
        btnNotif->setText(QCoreApplication::translate("MainWindow", "\360\237\224\224 3", nullptr));
        btnUser->setText(QCoreApplication::translate("MainWindow", "\360\237\221\244 Designer \342\226\276", nullptr));
        bannerImage->setText(QString());
        groupInfos->setTitle(QCoreApplication::translate("MainWindow", "\360\237\221\244  Informations du client", nullptr));
        btnReinit->setText(QCoreApplication::translate("MainWindow", "\342\237\263  R\303\251initialiser", nullptr));
        lblId->setText(QCoreApplication::translate("MainWindow", "ID Client", nullptr));
        editId->setText(QCoreApplication::translate("MainWindow", "CL001", nullptr));
        lblNom->setText(QCoreApplication::translate("MainWindow", "Nom", nullptr));
        editNom->setText(QCoreApplication::translate("MainWindow", "Ben Ali", nullptr));
        lblPrenom->setText(QCoreApplication::translate("MainWindow", "Pr\303\251nom", nullptr));
        editPrenom->setText(QCoreApplication::translate("MainWindow", "Sofia", nullptr));
        lblTel->setText(QCoreApplication::translate("MainWindow", "T\303\251l\303\251phone", nullptr));
        editTel->setText(QCoreApplication::translate("MainWindow", "+216 98 765 432", nullptr));
        lblEmail->setText(QCoreApplication::translate("MainWindow", "Email", nullptr));
        editEmail->setText(QCoreApplication::translate("MainWindow", "sofia.benali@email.com", nullptr));
        lblAdresse->setText(QCoreApplication::translate("MainWindow", "Adresse", nullptr));
        editAdresse->setText(QCoreApplication::translate("MainWindow", "12 Rue de la Libert\303\251, Tunis", nullptr));
        lblType->setText(QCoreApplication::translate("MainWindow", "Type de client", nullptr));
        comboType->setItemText(0, QCoreApplication::translate("MainWindow", "VIP", nullptr));
        comboType->setItemText(1, QCoreApplication::translate("MainWindow", "Premium", nullptr));
        comboType->setItemText(2, QCoreApplication::translate("MainWindow", "Standard", nullptr));

        lblDate->setText(QCoreApplication::translate("MainWindow", "Date d'inscription", nullptr));
        dateInscription->setDisplayFormat(QCoreApplication::translate("MainWindow", "dd/MM/yyyy", nullptr));
        lblRemarques->setText(QCoreApplication::translate("MainWindow", "Remarques", nullptr));
        editRemarques->setPlainText(QCoreApplication::translate("MainWindow", "Cliente fid\303\250le, pr\303\251f\303\250re les robes de soir\303\251e et les cr\303\251ations sur mesure.", nullptr));
        btnAjouter->setText(QCoreApplication::translate("MainWindow", "\357\274\213  Ajouter", nullptr));
        btnModifier->setText(QCoreApplication::translate("MainWindow", "\342\234\216  Modifier", nullptr));
        btnSupprimer->setText(QCoreApplication::translate("MainWindow", "\360\237\227\221  Supprimer", nullptr));
        btnAfficher->setText(QCoreApplication::translate("MainWindow", "\360\237\221\201  Afficher", nullptr));
        groupListe->setTitle(QCoreApplication::translate("MainWindow", "\360\237\227\202  Liste des clients", nullptr));
        searchListe->setPlaceholderText(QCoreApplication::translate("MainWindow", "Rechercher un client...", nullptr));
        lblTrier->setText(QCoreApplication::translate("MainWindow", "Trier par :", nullptr));
        comboTrier->setItemText(0, QCoreApplication::translate("MainWindow", "Date d'inscription", nullptr));
        comboTrier->setItemText(1, QCoreApplication::translate("MainWindow", "Nom", nullptr));
        comboTrier->setItemText(2, QCoreApplication::translate("MainWindow", "Type de client", nullptr));

        btnExport->setText(QCoreApplication::translate("MainWindow", "\360\237\223\204  Exporter PDF", nullptr));
        QTableWidgetItem *___qtablewidgetitem = tableClients->horizontalHeaderItem(0);
        ___qtablewidgetitem->setText(QCoreApplication::translate("MainWindow", "ID", nullptr));
        QTableWidgetItem *___qtablewidgetitem1 = tableClients->horizontalHeaderItem(1);
        ___qtablewidgetitem1->setText(QCoreApplication::translate("MainWindow", "Nom", nullptr));
        QTableWidgetItem *___qtablewidgetitem2 = tableClients->horizontalHeaderItem(2);
        ___qtablewidgetitem2->setText(QCoreApplication::translate("MainWindow", "Pr\303\251nom", nullptr));
        QTableWidgetItem *___qtablewidgetitem3 = tableClients->horizontalHeaderItem(3);
        ___qtablewidgetitem3->setText(QCoreApplication::translate("MainWindow", "T\303\251l\303\251phone", nullptr));
        QTableWidgetItem *___qtablewidgetitem4 = tableClients->horizontalHeaderItem(4);
        ___qtablewidgetitem4->setText(QCoreApplication::translate("MainWindow", "Date\n"
"d'inscription", nullptr));
        QTableWidgetItem *___qtablewidgetitem5 = tableClients->horizontalHeaderItem(5);
        ___qtablewidgetitem5->setText(QCoreApplication::translate("MainWindow", "Statut", nullptr));
        QTableWidgetItem *___qtablewidgetitem6 = tableClients->horizontalHeaderItem(6);
        ___qtablewidgetitem6->setText(QCoreApplication::translate("MainWindow", "Actions", nullptr));

        const bool __sortingEnabled = tableClients->isSortingEnabled();
        tableClients->setSortingEnabled(false);
        QTableWidgetItem *___qtablewidgetitem7 = tableClients->item(0, 0);
        ___qtablewidgetitem7->setText(QCoreApplication::translate("MainWindow", "CL001", nullptr));
        QTableWidgetItem *___qtablewidgetitem8 = tableClients->item(0, 1);
        ___qtablewidgetitem8->setText(QCoreApplication::translate("MainWindow", "Ben Ali", nullptr));
        QTableWidgetItem *___qtablewidgetitem9 = tableClients->item(0, 2);
        ___qtablewidgetitem9->setText(QCoreApplication::translate("MainWindow", "Sofia", nullptr));
        QTableWidgetItem *___qtablewidgetitem10 = tableClients->item(0, 3);
        ___qtablewidgetitem10->setText(QCoreApplication::translate("MainWindow", "+216 98 765 432", nullptr));
        QTableWidgetItem *___qtablewidgetitem11 = tableClients->item(0, 4);
        ___qtablewidgetitem11->setText(QCoreApplication::translate("MainWindow", "15/03/2024", nullptr));
        QTableWidgetItem *___qtablewidgetitem12 = tableClients->item(0, 5);
        ___qtablewidgetitem12->setText(QCoreApplication::translate("MainWindow", "VIP", nullptr));
        QTableWidgetItem *___qtablewidgetitem13 = tableClients->item(0, 6);
        ___qtablewidgetitem13->setText(QCoreApplication::translate("MainWindow", "\360\237\221\201  \342\234\216", nullptr));
        QTableWidgetItem *___qtablewidgetitem14 = tableClients->item(1, 0);
        ___qtablewidgetitem14->setText(QCoreApplication::translate("MainWindow", "CL002", nullptr));
        QTableWidgetItem *___qtablewidgetitem15 = tableClients->item(1, 1);
        ___qtablewidgetitem15->setText(QCoreApplication::translate("MainWindow", "Trabelsi", nullptr));
        QTableWidgetItem *___qtablewidgetitem16 = tableClients->item(1, 2);
        ___qtablewidgetitem16->setText(QCoreApplication::translate("MainWindow", "Amine", nullptr));
        QTableWidgetItem *___qtablewidgetitem17 = tableClients->item(1, 3);
        ___qtablewidgetitem17->setText(QCoreApplication::translate("MainWindow", "+216 92 123 456", nullptr));
        QTableWidgetItem *___qtablewidgetitem18 = tableClients->item(1, 4);
        ___qtablewidgetitem18->setText(QCoreApplication::translate("MainWindow", "22/04/2024", nullptr));
        QTableWidgetItem *___qtablewidgetitem19 = tableClients->item(1, 5);
        ___qtablewidgetitem19->setText(QCoreApplication::translate("MainWindow", "Standard", nullptr));
        QTableWidgetItem *___qtablewidgetitem20 = tableClients->item(1, 6);
        ___qtablewidgetitem20->setText(QCoreApplication::translate("MainWindow", "\360\237\221\201  \342\234\216", nullptr));
        QTableWidgetItem *___qtablewidgetitem21 = tableClients->item(2, 0);
        ___qtablewidgetitem21->setText(QCoreApplication::translate("MainWindow", "CL003", nullptr));
        QTableWidgetItem *___qtablewidgetitem22 = tableClients->item(2, 1);
        ___qtablewidgetitem22->setText(QCoreApplication::translate("MainWindow", "Hmidi", nullptr));
        QTableWidgetItem *___qtablewidgetitem23 = tableClients->item(2, 2);
        ___qtablewidgetitem23->setText(QCoreApplication::translate("MainWindow", "Salma", nullptr));
        QTableWidgetItem *___qtablewidgetitem24 = tableClients->item(2, 3);
        ___qtablewidgetitem24->setText(QCoreApplication::translate("MainWindow", "+216 95 987 654", nullptr));
        QTableWidgetItem *___qtablewidgetitem25 = tableClients->item(2, 4);
        ___qtablewidgetitem25->setText(QCoreApplication::translate("MainWindow", "10/05/2024", nullptr));
        QTableWidgetItem *___qtablewidgetitem26 = tableClients->item(2, 5);
        ___qtablewidgetitem26->setText(QCoreApplication::translate("MainWindow", "VIP", nullptr));
        QTableWidgetItem *___qtablewidgetitem27 = tableClients->item(2, 6);
        ___qtablewidgetitem27->setText(QCoreApplication::translate("MainWindow", "\360\237\221\201  \342\234\216", nullptr));
        QTableWidgetItem *___qtablewidgetitem28 = tableClients->item(3, 0);
        ___qtablewidgetitem28->setText(QCoreApplication::translate("MainWindow", "CL004", nullptr));
        QTableWidgetItem *___qtablewidgetitem29 = tableClients->item(3, 1);
        ___qtablewidgetitem29->setText(QCoreApplication::translate("MainWindow", "Saidi", nullptr));
        QTableWidgetItem *___qtablewidgetitem30 = tableClients->item(3, 2);
        ___qtablewidgetitem30->setText(QCoreApplication::translate("MainWindow", "Yassine", nullptr));
        QTableWidgetItem *___qtablewidgetitem31 = tableClients->item(3, 3);
        ___qtablewidgetitem31->setText(QCoreApplication::translate("MainWindow", "+216 93 456 789", nullptr));
        QTableWidgetItem *___qtablewidgetitem32 = tableClients->item(3, 4);
        ___qtablewidgetitem32->setText(QCoreApplication::translate("MainWindow", "18/06/2024", nullptr));
        QTableWidgetItem *___qtablewidgetitem33 = tableClients->item(3, 5);
        ___qtablewidgetitem33->setText(QCoreApplication::translate("MainWindow", "Premium", nullptr));
        QTableWidgetItem *___qtablewidgetitem34 = tableClients->item(3, 6);
        ___qtablewidgetitem34->setText(QCoreApplication::translate("MainWindow", "\360\237\221\201  \342\234\216", nullptr));
        QTableWidgetItem *___qtablewidgetitem35 = tableClients->item(4, 0);
        ___qtablewidgetitem35->setText(QCoreApplication::translate("MainWindow", "CL005", nullptr));
        QTableWidgetItem *___qtablewidgetitem36 = tableClients->item(4, 1);
        ___qtablewidgetitem36->setText(QCoreApplication::translate("MainWindow", "Mansouri", nullptr));
        QTableWidgetItem *___qtablewidgetitem37 = tableClients->item(4, 2);
        ___qtablewidgetitem37->setText(QCoreApplication::translate("MainWindow", "Rania", nullptr));
        QTableWidgetItem *___qtablewidgetitem38 = tableClients->item(4, 3);
        ___qtablewidgetitem38->setText(QCoreApplication::translate("MainWindow", "+216 22 334 455", nullptr));
        QTableWidgetItem *___qtablewidgetitem39 = tableClients->item(4, 4);
        ___qtablewidgetitem39->setText(QCoreApplication::translate("MainWindow", "27/07/2024", nullptr));
        QTableWidgetItem *___qtablewidgetitem40 = tableClients->item(4, 5);
        ___qtablewidgetitem40->setText(QCoreApplication::translate("MainWindow", "Standard", nullptr));
        QTableWidgetItem *___qtablewidgetitem41 = tableClients->item(4, 6);
        ___qtablewidgetitem41->setText(QCoreApplication::translate("MainWindow", "\360\237\221\201  \342\234\216", nullptr));
        QTableWidgetItem *___qtablewidgetitem42 = tableClients->item(5, 0);
        ___qtablewidgetitem42->setText(QCoreApplication::translate("MainWindow", "CL006", nullptr));
        QTableWidgetItem *___qtablewidgetitem43 = tableClients->item(5, 1);
        ___qtablewidgetitem43->setText(QCoreApplication::translate("MainWindow", "Mejri", nullptr));
        QTableWidgetItem *___qtablewidgetitem44 = tableClients->item(5, 2);
        ___qtablewidgetitem44->setText(QCoreApplication::translate("MainWindow", "Lilia", nullptr));
        QTableWidgetItem *___qtablewidgetitem45 = tableClients->item(5, 3);
        ___qtablewidgetitem45->setText(QCoreApplication::translate("MainWindow", "+216 95 667 788", nullptr));
        QTableWidgetItem *___qtablewidgetitem46 = tableClients->item(5, 4);
        ___qtablewidgetitem46->setText(QCoreApplication::translate("MainWindow", "03/08/2024", nullptr));
        QTableWidgetItem *___qtablewidgetitem47 = tableClients->item(5, 5);
        ___qtablewidgetitem47->setText(QCoreApplication::translate("MainWindow", "Premium", nullptr));
        QTableWidgetItem *___qtablewidgetitem48 = tableClients->item(5, 6);
        ___qtablewidgetitem48->setText(QCoreApplication::translate("MainWindow", "\360\237\221\201  \342\234\216", nullptr));
        tableClients->setSortingEnabled(__sortingEnabled);

        btnPagePrev->setText(QCoreApplication::translate("MainWindow", "\342\200\271", nullptr));
        btnPage1->setText(QCoreApplication::translate("MainWindow", "1", nullptr));
        btnPage2->setText(QCoreApplication::translate("MainWindow", "2", nullptr));
        btnPage3->setText(QCoreApplication::translate("MainWindow", "3", nullptr));
        btnPageDots->setText(QCoreApplication::translate("MainWindow", "...", nullptr));
        btnPage10->setText(QCoreApplication::translate("MainWindow", "10", nullptr));
        btnPageNext->setText(QCoreApplication::translate("MainWindow", "\342\200\272", nullptr));
        groupHistorique->setTitle(QCoreApplication::translate("MainWindow", "\360\237\247\276  Historique des commandes", nullptr));
        lblNbCommandes->setText(QCoreApplication::translate("MainWindow", "Commandes : -", nullptr));
        lblTotalDepense->setText(QCoreApplication::translate("MainWindow", "Total d\303\251pens\303\251 : -", nullptr));
        lblDerniereCommande->setText(QCoreApplication::translate("MainWindow", "Derni\303\250re commande : -", nullptr));
        comboStatutCmd->setItemText(0, QCoreApplication::translate("MainWindow", "Tous les statuts", nullptr));
        comboStatutCmd->setItemText(1, QCoreApplication::translate("MainWindow", "Livr\303\251e", nullptr));
        comboStatutCmd->setItemText(2, QCoreApplication::translate("MainWindow", "En cours", nullptr));
        comboStatutCmd->setItemText(3, QCoreApplication::translate("MainWindow", "En attente", nullptr));
        comboStatutCmd->setItemText(4, QCoreApplication::translate("MainWindow", "Annul\303\251e", nullptr));

        btnVoirCommandes->setText(QCoreApplication::translate("MainWindow", "Voir dans Commandes  \342\200\272", nullptr));
        QTableWidgetItem *___qtablewidgetitem49 = tableCommandes->horizontalHeaderItem(0);
        ___qtablewidgetitem49->setText(QCoreApplication::translate("MainWindow", "N\302\260 commande", nullptr));
        QTableWidgetItem *___qtablewidgetitem50 = tableCommandes->horizontalHeaderItem(1);
        ___qtablewidgetitem50->setText(QCoreApplication::translate("MainWindow", "Date", nullptr));
        QTableWidgetItem *___qtablewidgetitem51 = tableCommandes->horizontalHeaderItem(2);
        ___qtablewidgetitem51->setText(QCoreApplication::translate("MainWindow", "D\303\251signation", nullptr));
        QTableWidgetItem *___qtablewidgetitem52 = tableCommandes->horizontalHeaderItem(3);
        ___qtablewidgetitem52->setText(QCoreApplication::translate("MainWindow", "Montant", nullptr));
        QTableWidgetItem *___qtablewidgetitem53 = tableCommandes->horizontalHeaderItem(4);
        ___qtablewidgetitem53->setText(QCoreApplication::translate("MainWindow", "Statut", nullptr));
        statsRepartition->setText(QString());
        statsEvolution->setText(QString());
        statsTop5->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
