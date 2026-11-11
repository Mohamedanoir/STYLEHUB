#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "icones.h"
#include "statwidgets.h"
#include "widgets.h"
#include "modulepage.h"
#include "employepage.h"
#include "modules/fashionova-v2/clientsv2window.h"

#include <QAbstractItemView>
#include <QApplication>
#include <QButtonGroup>
#include <QComboBox>
#include <QDateEdit>
#include <QDesktopServices>
#include <QFileDialog>
#include <QFileInfo>
#include <QFrame>
#include <QGraphicsDropShadowEffect>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QImageReader>
#include <QIntValidator>
#include <QRegularExpressionValidator>
#include <QToolTip>
#include <QEvent>
#include <QKeyEvent>
#include <QPainter>
#include <QScrollBar>
#include <QLabel>
#include <QLineEdit>
#include <QMenu>
#include <QMessageBox>
#include <QMovie>
#include <QPageLayout>
#include <QPageSize>
#include <QPdfWriter>
#include <QPushButton>
#include <QScrollArea>
#include <QSet>
#include <QStackedWidget>
#include <QStyle>
#include <QTableWidget>
#include <QTextDocument>
#include <QTextEdit>
#include <QTimer>
#include <QToolButton>
#include <QUrl>
#include <QVBoxLayout>

#include <algorithm>

// ============================================================================
//  Charte graphique FashioNova
// ============================================================================
static const QColor NAVY("#0B2A45");
static const QColor TEXTE("#1B2333");
static const QColor OR("#C59A62");

// Date "vide" : le champ Date de création affiche un blanc tant que l'utilisateur
// n'a pas choisi de date (QDateEdit ne peut pas être réellement vide).
static const QDate DATE_VIDE(1900, 1, 1);

// Lit l'image n° "index" d'un GIF (pour les miniatures)
static QPixmap imageDuGif(const QString &chemin, int index)
{
    QImageReader r(chemin);
    if (index > 0) r.jumpToImage(qMin(index, qMax(0, r.imageCount() - 1)));
    return QPixmap::fromImage(r.read());
}

static void erreurSaisie(QLineEdit *edit, const QString &msg)
{
    if (edit)
        QToolTip::showText(edit->mapToGlobal(QPoint(0, edit->height())), msg, edit, {}, 2500);
}

static QString nomFichierType(const QString &type)
{
    const QString t = type.trimmed().toLower();
    if (t.contains("robe"))     return "robe";
    if (t.contains("veste"))    return "veste";
    if (t.contains("jupe"))     return "jupe";
    if (t.contains("pantalon")) return "pantalon";
    if (t.contains("shirt"))    return "t_shirt";
    if (t.contains("chapeau"))  return "chapeau";
    if (t.contains("chemise"))  return "chemise_homme";
    return "";
}

static QString valeurChoisie(const QComboBox *combo)
{
    return (combo && combo->currentIndex() > 0) ? combo->currentText() : QString();
}

// ============================================================================
//  Constructeur / destructeur
// ============================================================================
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    for (const QString &type : Maquette::types()) {
        const QString n = nomFichierType(type);
        if (n.isEmpty()) continue;

        QString avant;
        if      (QFileInfo::exists(":/images/" + n + "_avant.png")) avant = ":/images/" + n + "_avant.png";
        else if (QFileInfo::exists(":/images/" + n + "_avant.gif")) avant = ":/images/" + n + "_avant.gif";
        if (avant.isEmpty()) continue;
        m_imgAvant[n] = QPixmap(avant);

        QString validee;
        if      (QFileInfo::exists(":/images/" + n + "_validee.gif")) validee = ":/images/" + n + "_validee.gif";
        else if (QFileInfo::exists(":/images/" + n + "_validee.png")) validee = ":/images/" + n + "_validee.png";
        if (!validee.isEmpty()) {
            m_imgValidee[n]  = imageDuGif(validee, 0);
            m_imgValidee2[n] = imageDuGif(validee, QImageReader(validee).imageCount() / 2);
        }
    }

    configurerInterface();
    appliquerStyle();

    rafraichirListe();
    onReinitialiser();
    afficherPage(nbPages());

    m_timerRetard = new QTimer(this);
    connect(m_timerRetard, &QTimer::timeout, this, &MainWindow::rafraichirListe);
    m_timerRetard->start(60 * 1000);
    QTimer::singleShot(900, this, &MainWindow::verifierRetards);
}

MainWindow::~MainWindow()
{
    delete ui;
}

static void ajouterOmbre(QWidget *w, int flou, int alpha)
{
    if (!w) return;
    auto *ombre = new QGraphicsDropShadowEffect(w);
    ombre->setBlurRadius(flou);
    ombre->setOffset(0, 2);
    ombre->setColor(QColor(120, 90, 50, alpha));
    w->setGraphicsEffect(ombre);
}

static void iconeBouton(QAbstractButton *b, const QString &icone, const QColor &c, int taille)
{
    if (!b) return;
    b->setIcon(Icones::icone(icone, c, taille));
    b->setIconSize(QSize(taille, taille));
}

void MainWindow::configurerInterface()
{
    m_sidebar          = nullptr; // La barre latérale est gérée par ui->layoutSidebar
    m_pages            = ui->stackedWidget;
    m_rechercheGlobale = ui->editRechercheGlobale;

    // Protection contre l'ambiguïté des champs ID
    m_editId           = ui->editIdMaquette ? ui->editIdMaquette : ui->editId;
    m_editNom          = ui->editNom;
    m_comboType        = ui->comboType;
    m_dateCreation     = ui->dateCreation;
    m_comboStatut      = ui->comboStatutMaquette ? ui->comboStatutMaquette : ui->comboStatut;
    m_pastilleStatut   = ui->pastilleStatut;
    m_fondPastille     = ui->fondPastille;
    m_comboTaille      = ui->comboTaille;
    m_editLargeur      = ui->editLargeur;
    m_editLongueur     = ui->editLongueur;
    m_editDescription  = ui->editDescription;
    m_editVideo        = ui->editVideo;
    m_apercu           = ui->apercu;
    m_mini[0]          = ui->mini1;
    m_mini[1]          = ui->mini2;
    m_mini[2]          = ui->mini3;
    m_boutonPlus       = ui->btnPlus;
    m_rechercheListe   = ui->editRechercheListe;
    m_comboTri         = ui->comboTri;
    m_table            = ui->tableMaquettes;
    m_paginationLayout = ui->layoutPagination;
    m_labelRetard      = ui->labelRetard;
    m_donut            = ui->chartType;
    m_courbe           = ui->chartEvolution;
    m_barres           = ui->chartTop;

    // Menu latéral
    QPixmap logo(":/images/logo.png");
    logo.setDevicePixelRatio(2.0);
    if (ui->labelLogo) ui->labelLogo->clear();

    if (ui->layoutSidebar) ui->layoutSidebar->setSpacing(6);

    m_navGroupe = new QButtonGroup(this);
    const QList<QPushButton *> nav = {ui->btnClients, ui->btnEmployes, ui->btnCommandes,
                                      ui->btnMaquettes, ui->btnMachines, ui->btnArticles};
    const QStringList icons = {"users", "employes", "clipboard", "mannequin", "gear", "tag"};
    for (int i = 0; i < nav.size(); ++i) {
        if (!nav[i]) continue;
        nav[i]->setIcon(Icones::iconeDouble(icons[i], Qt::white, NAVY, 22));
        nav[i]->setIconSize(QSize(22, 22));
        nav[i]->setFixedHeight(40);
        m_navGroupe->addButton(nav[i], i);
    }

    connect(m_navGroupe, &QButtonGroup::idClicked, this, [this](int id) {
        if (m_pages) m_pages->setCurrentIndex(id);
        if (id == 0 && m_clientsV2) m_clientsV2->reloadClients();
        if (id == 1 && ui->employeWidget) {
            ui->employeWidget->loadEmployees();
            ui->employeWidget->refresh();
            ui->employeWidget->updateStats();
        }
        if (id == 3) {
            rafraichirListe();
            onReinitialiser();
            afficherPage(nbPages());
        }
    });

    if (ui->btnMaquettes) ui->btnMaquettes->setChecked(true);
    if (m_pages) m_pages->setCurrentIndex(3);

    // Module Clients
    m_clientsV2 = new FashionNovaV2Window(this);
    if (ui->pageClients && ui->pageClients->layout()) {
        QLayout *layoutClients = ui->pageClients->layout();
        while (QLayoutItem *item = layoutClients->takeAt(0)) {
            delete item->widget();
            delete item;
        }
        layoutClients->setContentsMargins(0, 0, 0, 0);
        QWidget *clientsPage = m_clientsV2->takeClientsPage();
        layoutClients->addWidget(clientsPage);
        clientsPage->show();
    }

    // Module Machines
    {
        using T = ChampForm;
        auto remplacer = [this](QWidget *ancien, ModulePage *page) {
            if (!ancien) return;
            QLayout *lay = ancien->layout();
            if (!lay) return;
            while (QLayoutItem *it = lay->takeAt(0)) {
                delete it->widget();
                delete it;
            }
            lay->setContentsMargins(0, 0, 0, 0);
            lay->addWidget(page);
        };

        remplacer(ui->pageMachines, new ModulePage("Machines",
                                                   { {"Référence", T::Texte, {}}, {"Nom", T::Texte, {}},
                                                    {"Type", T::Choix, {"Couture", "Coupe", "Broderie", "Repassage"}},
                                                    {"Date d'achat", T::Date, {}},
                                                    {"État", T::Choix, {"Fonctionnelle", "En panne", "En maintenance"}},
                                                    {"Notes", T::Description, {}} },
                                                   { {"MCH-01", "Surjeteuse Juki", "Couture",   "12/01/2024", "Fonctionnelle",  "Atelier 1"},
                                                    {"MCH-02", "Table de coupe",  "Coupe",     "03/03/2023", "Fonctionnelle",  "Atelier 1"},
                                                    {"MCH-03", "Brodeuse Brother", "Broderie", "21/06/2025", "En maintenance", "Atelier 2"},
                                                    {"MCH-04", "Fer industriel",  "Repassage", "15/09/2022", "En panne",        "Atelier 2"} },
                                                   ui->pageMachines));
    }

    // Barre supérieure
    iconeBouton(ui->btnMenu, "menu", TEXTE, 26);
    if (m_rechercheGlobale)
        m_rechercheGlobale->addAction(Icones::icone("search", QColor("#8A94A3"), 18), QLineEdit::LeadingPosition);

    iconeBouton(ui->btnCloche, "bell", TEXTE, 28);
    connect(ui->btnCloche, &QToolButton::clicked, this, &MainWindow::onCloche);
    m_badgeCloche = new QLabel("0", ui->btnCloche);
    m_badgeCloche->setObjectName("badgeCloche");
    m_badgeCloche->setAlignment(Qt::AlignCenter);
    m_badgeCloche->setFixedSize(17, 17);
    m_badgeCloche->move(22, 2);

    if (ui->labelAvatar) ui->labelAvatar->setPixmap(Icones::pixmap("avatar", NAVY, 40));
    iconeBouton(ui->btnCompte, "chevron", TEXTE, 18);
    if (ui->btnCompte) {
        ui->btnCompte->setPopupMode(QToolButton::InstantPopup);
        QMenu *menuCompte = new QMenu(ui->btnCompte);
        menuCompte->addAction("Designer / Responsable Création")->setEnabled(false);
        menuCompte->addSeparator();
        menuCompte->addAction("Quitter", qApp, &QApplication::quit);
        ui->btnCompte->setMenu(menuCompte);
        ui->btnCompte->setStyleSheet("QToolButton::menu-indicator { image: none; }");
    }

    // Bannière et cartes
    if (ui->banniere) ui->banniere->setImage(QPixmap(":/images/banniere.png"));
    if (ui->iconeFormulaire) ui->iconeFormulaire->setPixmap(Icones::pixmap("mannequin", NAVY, 26));
    if (ui->iconeListe) ui->iconeListe->setPixmap(Icones::pixmap("list", NAVY, 26));
    if (ui->iconeStats) ui->iconeStats->setPixmap(Icones::pixmap("stats", NAVY, 26));

    for (QWidget *c : {static_cast<QWidget *>(ui->carteFormulaire), static_cast<QWidget *>(ui->carteListe),
                       static_cast<QWidget *>(ui->carteStats)})
        ajouterOmbre(c, 18, 28);
    for (QWidget *b : {static_cast<QWidget *>(ui->blocType), static_cast<QWidget *>(ui->blocEvolution),
                       static_cast<QWidget *>(ui->blocTop)})
        ajouterOmbre(b, 14, 22);

    // Icônes du formulaire
    if (ui->iconeId) ui->iconeId->setPixmap(Icones::pixmap("idcard", TEXTE, 20));
    if (ui->iconeNom) ui->iconeNom->setPixmap(Icones::pixmap("namecard", TEXTE, 20));
    if (ui->iconeType) ui->iconeType->setPixmap(Icones::pixmap("type", TEXTE, 20));
    if (ui->iconeTaille) ui->iconeTaille->setPixmap(Icones::pixmap("taille", TEXTE, 20));
    if (ui->iconeLargeur) ui->iconeLargeur->setPixmap(Icones::pixmap("largeur", TEXTE, 20));
    if (ui->iconeLongueur) ui->iconeLongueur->setPixmap(Icones::pixmap("longueur", TEXTE, 20));

    // ID_MAQUETTE
    if (m_editId) {
        m_editId->setValidator(new QIntValidator(1, 999999999, m_editId));
        m_editId->setReadOnly(true);
        m_editId->setCursor(Qt::PointingHandCursor);
        m_editId->setPlaceholderText("Cliquez ici : ID automatique");
        m_editId->setToolTip("Cliquez pour obtenir automatiquement le prochain ID (le dernier ID + 1)");
        m_editId->installEventFilter(this);
    }

    // NOM_MAQUETTE
    if (m_editNom) {
        m_editNom->setValidator(new QRegularExpressionValidator(QRegularExpression("^[\\p{L} ]*$"), m_editNom));
        connect(m_editNom, &QLineEdit::inputRejected, this, [this]() {
            erreurSaisie(m_editNom, "⚠ Le nom accepte uniquement des lettres — pas de chiffres !");
        });
    }

    // LARGEUR et LONGUEUR
    QRegularExpression reMesure("^\\d{1,3}([.,]\\d{1,2})?$");
    if (m_editLargeur) {
        m_editLargeur->setValidator(new QRegularExpressionValidator(reMesure, m_editLargeur));
        connect(m_editLargeur, &QLineEdit::inputRejected, this, [this]() {
            erreurSaisie(m_editLargeur, "⚠ Largeur : saisissez un nombre réel (ex : 45.5) — pas de texte !");
        });
    }
    if (m_editLongueur) {
        m_editLongueur->setValidator(new QRegularExpressionValidator(reMesure, m_editLongueur));
        connect(m_editLongueur, &QLineEdit::inputRejected, this, [this]() {
            erreurSaisie(m_editLongueur, "⚠ Longueur : saisissez un nombre réel (ex : 120.0) — pas de texte !");
        });
    }

    // Mesures dynamiques
    for (const MesureType &mesure : Maquette::mesuresPossibles()) {
        QString nom;
        for (const QString &mot : mesure.colonne.split('_'))
            nom += mot.left(1).toUpper() + mot.mid(1).toLower();

        QLineEdit *edit = findChild<QLineEdit *>("edit" + nom);
        QWidget *label  = findChild<QWidget *>("label" + nom);
        QWidget *champ  = findChild<QWidget *>("champ" + nom);
        if (!edit || !label || !champ) continue;
        if (QLabel *icone = findChild<QLabel *>("icone" + nom))
            icone->setPixmap(Icones::pixmap("mesure", TEXTE, 20));

        edit->setPlaceholderText("en cm, ex : " + mesure.exemple);
        edit->setValidator(new QRegularExpressionValidator(QRegularExpression("^\\d{1,3}([.,]\\d{1,2})?$"), edit));
        const QString libelle = mesure.libelle;
        const QString exemple = mesure.exemple;
        connect(edit, &QLineEdit::inputRejected, this, [edit, libelle, exemple]() {
            erreurSaisie(edit, "⚠ " + libelle + " : saisissez un nombre réel (ex : " + exemple + ") — pas de texte !");
        });
        if (!Maquette::colonnePresente(mesure.colonne)) {
            edit->setEnabled(false);
            edit->setPlaceholderText("colonne absente (voir maquettes_oracle.sql)");
        }
        m_editMesures[mesure.colonne]   = edit;
        m_lignesMesures[mesure.colonne] = {label, champ};
    }

    // Date de création
    if (m_dateCreation) {
        m_dateCreation->setMinimumDate(DATE_VIDE);
        m_dateCreation->setSpecialValueText(" ");
        m_dateCreation->setMaximumDate(QDate::currentDate());
        m_dateCreation->setDate(DATE_VIDE);
        m_dateCreation->installEventFilter(this);
        if (QLineEdit *zoneDate = m_dateCreation->findChild<QLineEdit *>())
            zoneDate->installEventFilter(this);
    }
    iconeBouton(ui->btnCalendrier, "calendar", TEXTE, 20);
    if (ui->btnCalendrier) {
        connect(ui->btnCalendrier, &QToolButton::clicked, this, [this]() {
            if (m_dateCreation && m_dateCreation->date() == DATE_VIDE) m_dateCreation->setDate(QDate::currentDate());
            if (m_dateCreation) {
                m_dateCreation->setFocus();
                QKeyEvent ev(QEvent::KeyPress, Qt::Key_F4, Qt::NoModifier);
                QApplication::sendEvent(m_dateCreation, &ev);
            }
        });
    }

    if (ui->labelDescription) ui->labelDescription->setContentsMargins(0, 14, 0, 0);
    if (ui->btnParcourirVideo) {
        connect(ui->btnParcourirVideo, &QPushButton::clicked, this, [this]() {
            const QString chemin = QFileDialog::getOpenFileName(this, "Choisir une vidéo", "", "Vidéos (*.mp4 *.avi *.mov *.mkv *.wmv *.gif)");
            if (!chemin.isEmpty() && m_editVideo) m_editVideo->setText(chemin);
        });
    }

    iconeBouton(ui->btnReinitialiser, "reset", TEXTE, 18);
    iconeBouton(ui->btnAjouterMaquette ? ui->btnAjouterMaquette : ui->btnAjouter, "plus", Qt::white, 20);
    iconeBouton(ui->btnModifierMaquette ? ui->btnModifierMaquette : ui->btnModifier, "pencil", Qt::white, 20);
    iconeBouton(ui->btnSupprimerMaquette ? ui->btnSupprimerMaquette : ui->btnSupprimer, "trash", TEXTE, 20);
    if (m_boutonPlus) m_boutonPlus->setIconSize(QSize(26, 26));

    if (m_apercu && m_apercu->boutonCrayon()) {
        m_apercu->boutonCrayon()->setToolTip("Modifier la vidéo de présentation");
        connect(m_apercu->boutonCrayon(), &QToolButton::clicked, this, &MainWindow::onChoisirVideo);
    }
    for (int i = 0; i < 3; ++i) {
        if (m_mini[i]) {
            connect(m_mini[i], &Miniature::clique, this, [this, i]() {
                if (m_apercu && !m_mini[i]->image().isNull()) m_apercu->afficherImage(m_mini[i]->image());
            });
        }
    }

    if (ui->btnReinitialiser) connect(ui->btnReinitialiser, &QPushButton::clicked, this, &MainWindow::onReinitialiser);
    QPushButton *btnAjout = ui->btnAjouterMaquette ? ui->btnAjouterMaquette : ui->btnAjouter;
    QPushButton *btnModif = ui->btnModifierMaquette ? ui->btnModifierMaquette : ui->btnModifier;
    QPushButton *btnSuppr = ui->btnSupprimerMaquette ? ui->btnSupprimerMaquette : ui->btnSupprimer;

    if (btnAjout) connect(btnAjout, &QPushButton::clicked, this, &MainWindow::onAjouter);
    if (btnModif) connect(btnModif, &QPushButton::clicked, this, &MainWindow::onModifier);
    if (btnSuppr) connect(btnSuppr, &QPushButton::clicked, this, &MainWindow::onSupprimer);
    if (m_boutonPlus) connect(m_boutonPlus, &QToolButton::clicked, this, &MainWindow::onBoutonPlus);

    if (m_comboType) {
        connect(m_comboType, &QComboBox::currentTextChanged, this, &MainWindow::mettreAJourImageMaquette);
        connect(m_comboType, &QComboBox::currentTextChanged, this, &MainWindow::mettreAJourMesures);
    }
    if (m_comboStatut) connect(m_comboStatut, &QComboBox::currentTextChanged, this, &MainWindow::mettreAJourImageMaquette);

    // Liste
    if (m_rechercheListe) {
        m_rechercheListe->addAction(Icones::icone("search", TEXTE, 16), QLineEdit::LeadingPosition);
        connect(m_rechercheListe, &QLineEdit::textChanged, this, [this]() { m_pageCourante = 1; rafraichirListe(); });
    }
    if (m_comboTri) {
        const QStringList cles = {"", "date", "type", "statut"};
        for (int i = 0; i < cles.size(); ++i) m_comboTri->setItemData(i, cles[i]);
        connect(m_comboTri, &QComboBox::currentIndexChanged, this, [this]() { m_pageCourante = 1; rafraichirListe(); });
    }
    iconeBouton(ui->btnExporterPdf, "pdf", Qt::white, 20);

    if (m_table) {
        if (m_table->horizontalHeaderItem(0))
            m_table->horizontalHeaderItem(0)->setIcon(Icones::icone("box", QColor("#5B6472"), 18));
        m_table->setIconSize(QSize(14, 14));
        m_table->setTextElideMode(Qt::ElideRight);
        QHeaderView *hh = m_table->horizontalHeader();
        if (hh) {
            hh->setFixedHeight(44);
            hh->setDefaultAlignment(Qt::AlignLeft | Qt::AlignVCenter);
            hh->setSectionResizeMode(QHeaderView::Fixed);
            hh->setSectionResizeMode(2, QHeaderView::Stretch);
            connect(hh, &QHeaderView::sectionClicked, this, [this](int col) {
                if (col != 0) return;
                m_toutCoche = !m_toutCoche;
                if (m_table->horizontalHeaderItem(0)) {
                    m_table->horizontalHeaderItem(0)->setIcon(
                        Icones::icone(m_toutCoche ? "boxcheck" : "box", m_toutCoche ? NAVY : QColor("#5B6472"), 18));
                }
                for (int r = 0; r < m_table->rowCount(); ++r)
                    if (m_table->item(r, 0))
                        m_table->item(r, 0)->setCheckState(m_toutCoche ? Qt::Checked : Qt::Unchecked);
            });
        }
        m_table->setColumnWidth(0, 38);
        m_table->setColumnWidth(1, 62);
        m_table->setColumnWidth(3, 74);
        m_table->setColumnWidth(4, 128);
        m_table->setColumnWidth(5, 96);
        m_table->setColumnWidth(6, 80);
        m_table->setFixedHeight(44 + LIGNES_PAR_PAGE * 47 + 2);
        connect(m_table, &QTableWidget::cellClicked, this, &MainWindow::onLigneCliquee);
    }

    if (m_editDescription) {
        connect(m_editDescription, &QTextEdit::textChanged, this, [this]() {
            QString texte = m_editDescription->toPlainText();
            QString filtre = texte;
            filtre.remove(QRegularExpression("\\d"));
            if (filtre != texte) {
                int pos = m_editDescription->textCursor().position() - (texte.length() - filtre.length());
                m_editDescription->blockSignals(true);
                m_editDescription->setPlainText(filtre);
                QTextCursor c = m_editDescription->textCursor();
                c.setPosition(qMax(0, pos));
                m_editDescription->setTextCursor(c);
                m_editDescription->blockSignals(false);
                QToolTip::showText(
                    m_editDescription->mapToGlobal(QPoint(0, m_editDescription->height())),
                    "⚠ La description accepte uniquement des lettres — pas de chiffres !",
                    m_editDescription, {}, 2500);
            }
        });
    }

    if (m_rechercheGlobale && m_rechercheListe)
        connect(m_rechercheGlobale, &QLineEdit::textChanged, m_rechercheListe, &QLineEdit::setText);
    if (ui->btnExporterPdf)
        connect(ui->btnExporterPdf, &QPushButton::clicked, this, &MainWindow::onExporterPdf);
}

void MainWindow::appliquerStyle()
{
    qApp->setStyleSheet(QString(R"(
        * { font-family: "Segoe UI", "Arial"; }
        QWidget { font-size: 14px; color: #1B2333; }
        QMainWindow, #zoneDroite, #scrollContents, #scrollMaquettes, QStackedWidget, #pageMaquettes { background: #F7F1EA; }

        QPushButton[nav="true"] {
            background: transparent; color: #FFFFFF; border: none; border-radius: 6px;
            text-align: left; padding-left: 20px; font-size: 14px; font-weight: 500;
        }
        QPushButton[nav="true"]:hover   { background: rgba(255,255,255,0.12); }
        QPushButton[nav="true"]:checked {
            background: qlineargradient(x1:0,y1:0,x2:1,y2:0, stop:0 #E5C378, stop:1 #C89538);
            color: #08213B; font-weight: bold;
        }

        #topbar { background: #FBF8F4; border-bottom: 1px solid #EFE7DC; }
        *[role="iconBtn"] { background: transparent; border: none; }
        #editRechercheGlobale { border: 1px solid #E6DED3; border-radius: 19px; padding: 0 14px 0 6px; background: #FFFFFF; font-size: 14px; }
        #badgeCloche { background: #E8853D; color: white; border-radius: 8px; font-size: 10px; font-weight: 700; }
        #labelUser { font-size: 17px; font-weight: 600; }

        *[role="carte"] { background: #FFFFFF; border: 1px solid #EFE5D8; border-radius: 10px; }
        *[role="entete"] { background: #F2E7D8; border: none; border-top-left-radius: 10px; border-top-right-radius: 10px; }
        *[role="corps"] { background: transparent; }
        *[role="titreCarte"] { font-size: 18px; font-weight: 700; color: #1B2333; }
        *[role="labelChamp"] { font-size: 15px; font-weight: 700; }
        #labelTri   { font-size: 16px; font-weight: 700; }

        *[role="champ"] { background: #FFFFFF; border: 1px solid #E3DCD2; border-radius: 6px; }
        *[role="interne"], *[role="interneGras"] { border: none; background: transparent; padding: 0; font-size: 15px; }
        *[role="interneGras"] { font-weight: 700; }
        QComboBox[role="interne"]::drop-down, QComboBox[role="interneGras"]::drop-down, #comboTri::drop-down { border: none; width: 26px; }
        QComboBox::down-arrow { image: url(:/images/chevron.png); width: 14px; height: 14px; }
        QDateEdit[role="interne"]::drop-down { width: 0px; border: none; }

        #editVideo { background-color: #FFFFFF; color: #1B2333; border: 1px solid #E3DCD2;
                     border-radius: 6px; padding: 5px 10px; selection-background-color: #DCE8F4;
                     selection-color: #0B2D52; }
        #editVideo:disabled { background-color: #F6F1EB; color: #536173; }
        #btnParcourirVideo { background-color: #F1E8DD; color: #0B2D52;
                            border: 1px solid #E3DCD2; border-radius: 7px;
                            padding: 7px 14px; font-weight: 600; }
        #btnParcourirVideo:hover { background-color: #E8D8C5; }

        QMessageBox { background-color: #FFFFFF; color: #1B2333; }
        QMessageBox QLabel { background: transparent; color: #1B2333; font-size: 14px; }
        QMessageBox QTextEdit, QMessageBox QTextBrowser { background: #FFFFFF; color: #1B2333; }
        QMessageBox QPushButton { background-color: #F1E8DD; color: #102C48;
                                  border: 1px solid #D9C9B5; border-radius: 5px;
                                  min-width: 85px; min-height: 30px; padding: 4px 12px; }
        QMessageBox QPushButton:hover { background-color: #E4D3BF; }

        #editDescription { border: 1px solid #E3DCD2; border-radius: 6px; padding: 6px 8px; font-size: 14px; background: #FFFFFF; }
        #editRechercheListe { border: 1px solid #E3DCD2; border-radius: 6px; padding-left: 4px; font-size: 14px; background: #FFFFFF; }
        #comboTri { border: 1px solid #E3DCD2; border-radius: 6px; padding-left: 12px; font-size: 15px; background: #FFFFFF; }
        QComboBox QAbstractItemView { background: white; border: 1px solid #E3DCD2; selection-background-color: #F2E7D8; selection-color: #1B2333; outline: 0; }

        #btnPlus { background: #EFE3D2; border: none; border-radius: 6px; }
        #btnPlus:hover { background: #E7D6BF; }

        QPushButton { border-radius: 6px; padding: 0 18px; font-size: 15px; font-weight: 600; }
        *[role="btnPrimaire"] { background: #0B2D52; color: white; border: none; }
        *[role="btnPrimaire"]:hover { background: #163F6E; }
        *[role="btnGris"] { background: #708090; color: white; border: none; }
        *[role="btnGris"]:hover { background: #5F6E7D; }
        *[role="btnBeige"] { background: #EFE3D2; color: #1B2333; border: none; }
        *[role="btnBeige"]:hover { background: #E6D5BE; }
        *[role="btnClair"] { background: #E7E7E7; color: #1B2333; border: none; }
        *[role="btnClair"]:hover { background: #DCDCDC; }
        *[role="btnBlanc"] { background: #FFFFFF; color: #1B2333; border: none; padding: 6px 14px; font-size: 14px; }
        *[role="btnBlanc"]:hover { background: #FAF6F1; }

        QTableWidget { border: 1px solid #EEE6DB; border-radius: 6px; background: #FFFFFF;
                       selection-background-color: #F6EEE3; selection-color: #1B2333; font-size: 13px; }
        QTableWidget::item { border-bottom: 1px solid #F1EBE3; padding-left: 6px; }
        QTableWidget::item:selected { background: #F6EEE3; color: #1B2333; }
        QHeaderView::section { background: #F8F1E8; border: none; border-bottom: 1px solid #EEE6DB;
                               padding-left: 6px; font-weight: 700; font-size: 14px; color: #1B2333; }
        #pageBtn { background: #FFFFFF; border: 1px solid #E6E0D8; border-radius: 4px; padding: 0; font-size: 14px; font-weight: 500; }
        #pageBtn:hover { background: #F6F0E8; }
        #pageBtnActif { background: #0B2A45; color: white; border: none; border-radius: 4px; padding: 0; font-size: 14px; font-weight: 700; }
        #labelRetard { color: #B42318; font-size: 12px; font-weight: 600; }

        *[role="blocStat"] { background: #FFFFFF; border: 1px solid #F1EAE0; border-radius: 8px; }
        *[role="titreStat"] { font-weight: 700; font-size: 15px; }
        *[role="placeholder"] { font-size: 20px; color: #6B7280; }

        QScrollBar:vertical { background: transparent; width: 10px; }
        QScrollBar::handle:vertical { background: #D9CDBE; border-radius: 5px; min-height: 40px; }
        QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0; }
    )"));
}

QString MainWindow::nomImage(const QString &type) const
{
    const QString n = nomFichierType(type);
    return m_imgAvant.contains(n) ? n : QString();
}

void MainWindow::mettreAJourImageMaquette()
{
    if (!m_comboType || !m_comboStatut) return;
    const QString type = valeurChoisie(m_comboType);
    const QString statut = valeurChoisie(m_comboStatut);
    const bool estValidee = statut.startsWith("Valid");

    QString point = "#B0B7C3", fond = "#F1F2F4";
    if (statut == "En cours") { point = "#10A84E"; fond = "#E6F4EA"; }
    else if (estValidee)      { point = "#2F6FB3"; fond = "#E4EEF8"; }
    if (m_pastilleStatut) m_pastilleStatut->setStyleSheet("background:" + point + "; border-radius:7px;");
    if (m_fondPastille) m_fondPastille->setStyleSheet("background:" + fond + "; border-top-left-radius:5px; border-bottom-left-radius:5px;");

    const QString nom = nomImage(type);
    if (nom.isEmpty()) {
        if (m_apercu) m_apercu->afficherVide(type.isEmpty() ? "Choisissez un type" : "Aperçu non disponible pour ce type");
        for (Miniature *mi : m_mini) if (mi) mi->setImage(QPixmap());
        return;
    }

    QString fichierValidee;
    if      (QFileInfo::exists(":/images/" + nom + "_validee.gif")) fichierValidee = ":/images/" + nom + "_validee.gif";
    else if (QFileInfo::exists(":/images/" + nom + "_validee.png")) fichierValidee = ":/images/" + nom + "_validee.png";

    if (m_apercu) {
        if (estValidee && !fichierValidee.isEmpty())
            m_apercu->afficherAnimation(fichierValidee);
        else
            m_apercu->afficherImage(m_imgAvant[nom]);
    }

    if (m_mini[0]) m_mini[0]->setImage(m_imgAvant.value(nom));
    if (m_mini[1]) m_mini[1]->setImage(m_imgValidee.value(nom));
    if (m_mini[2]) m_mini[2]->setImage(m_imgValidee2.value(nom));
}

QPixmap MainWindow::apercuPour(const Maquette &m) const
{
    const QString n = nomImage(m.getType());
    if (!n.isEmpty()) {
        const QPixmap validee = m_imgValidee.value(n);
        return m.getStatut().startsWith("Valid") && !validee.isNull() ? validee : m_imgAvant.value(n);
    }
    return QPixmap();
}

static void choisirTexte(QComboBox *combo, const QString &valeur)
{
    if (!combo) return;
    if (valeur.trimmed().isEmpty()) {
        combo->setCurrentIndex(0);
        return;
    }
    int i = combo->findText(valeur.trimmed(), Qt::MatchFixedString);
    if (i < 0) {
        combo->addItem(valeur.trimmed());
        i = combo->count() - 1;
    }
    combo->setCurrentIndex(i);
}

static void choisirStatut(QComboBox *combo, const QString &valeur)
{
    if (!combo) return;
    const QString v = valeur.trimmed().toLower();
    int i = 0;
    for (int j = 0; j < combo->count(); ++j) {
        const QString item = combo->itemText(j).trimmed().toLower();
        if (v.startsWith("valid") && item.startsWith("valid")) { i = j; break; }
        if (v == "en cours"      && item == "en cours")        { i = j; break; }
    }
    combo->setCurrentIndex(qMax(0, i));
}

Maquette MainWindow::lireFormulaire() const
{
    const QDate date = (!m_dateCreation || m_dateCreation->date() == DATE_VIDE) ? QDate() : m_dateCreation->date();
    const QString id = m_editId ? m_editId->text() : "";
    const QString nom = m_editNom ? m_editNom->text() : "";
    const QString desc = m_editDescription ? m_editDescription->toPlainText() : "";
    const QString video = m_editVideo ? m_editVideo->text() : "";
    const QString taille = valeurChoisie(m_comboTaille);
    const double larg = m_editLargeur ? Maquette::lireMesure(m_editLargeur->text()) : 0.0;
    const double longu = m_editLongueur ? Maquette::lireMesure(m_editLongueur->text()) : 0.0;

    return Maquette(id, nom, valeurChoisie(m_comboType), date,
                    valeurChoisie(m_comboStatut), desc, video, 0,
                    taille, larg, longu, lireMesuresDuType());
}

QMap<QString, double> MainWindow::lireMesuresDuType() const
{
    QMap<QString, double> mesures;
    for (const QString &colonne : Maquette::mesuresDuType(valeurChoisie(m_comboType)))
        if (QLineEdit *e = m_editMesures.value(colonne))
            mesures[colonne] = Maquette::lireMesure(e->text());
    return mesures;
}

void MainWindow::mettreAJourMesures()
{
    const QStringList duType = Maquette::mesuresDuType(valeurChoisie(m_comboType));
    for (auto it = m_lignesMesures.constBegin(); it != m_lignesMesures.constEnd(); ++it)
        for (QWidget *w : it.value())
            if (w) w->setVisible(duType.contains(it.key()));
}

void MainWindow::genererId()
{
    if (m_table) m_table->clearSelection();
    if (m_editId) m_editId->setText(QString::number(Maquette::prochainId()));
    mettreAJourBoutonVideo("");
}

void MainWindow::onAjouter()
{
    if (m_editId && m_editId->text().trimmed().isEmpty())
        genererId();

    Maquette m = lireFormulaire();
    QString err;
    if (m.ajouter(err)) {
        QMessageBox::information(this, "Ajout", "Maquette " + m.getId() + " ajoutée avec succès.");
        rafraichirListe();
        onReinitialiser();
        afficherMaquetteDansTableau(m.getId());
    } else {
        QMessageBox::warning(this, "Ajout impossible", err);
    }
}

void MainWindow::onModifier()
{
    if (!m_editId || m_editId->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Modification impossible",
                             "Sélectionnez d'abord une maquette dans la liste (clic sur une ligne).");
        return;
    }

    Maquette m = lireFormulaire();
    QString err;
    if (m.modifier(err)) {
        QMessageBox::information(this, "Modification", "Maquette " + m.getId() + " modifiée avec succès.");
        rafraichirListe();
        remplirFormulaire(m.getId());
        afficherMaquetteDansTableau(m.getId());
    } else {
        QMessageBox::warning(this, "Modification impossible", err);
    }
}

void MainWindow::onSupprimer()
{
    QStringList ids;
    if (m_table) {
        for (int r = 0; r < m_table->rowCount(); ++r)
            if (m_table->item(r, 0) && m_table->item(r, 0)->checkState() == Qt::Checked)
                if (m_table->item(r, 1)) ids << m_table->item(r, 1)->text();
    }

    if (ids.isEmpty() && m_editId) {
        const QString id = m_editId->text().trimmed();
        if (id.isEmpty() || !Maquette::existe(id)) {
            QMessageBox::warning(this, "Suppression", "Cochez des maquettes dans la liste ou sélectionnez une maquette (clic sur une ligne).");
            return;
        }
        ids << id;
    }

    if (QMessageBox::question(this, "Suppression",
                              "Voulez-vous vraiment supprimer : " + ids.join(", ") + " ?") != QMessageBox::Yes)
        return;

    int n = 0;
    QStringList erreurs;
    for (const QString &id : ids) {
        QString err;
        if (Maquette::supprimer(id, &err)) ++n;
        else erreurs << id + " : " + err;
    }

    QString msg = QString("%1 maquette(s) supprimée(s) avec succès.").arg(n);
    if (!erreurs.isEmpty()) {
        msg += "\n\nNon supprimée(s) :\n" + erreurs.join("\n");
        QMessageBox::warning(this, "Suppression", msg);
    } else {
        QMessageBox::information(this, "Suppression", msg);
    }

    m_toutCoche = false;
    if (m_table && m_table->horizontalHeaderItem(0))
        m_table->horizontalHeaderItem(0)->setIcon(Icones::icone("box", QColor("#5B6472"), 18));
    rafraichirListe();
    onReinitialiser();
    afficherPage(nbPages());
}

void MainWindow::onReinitialiser()
{
    if (m_table) m_table->clearSelection();
    if (m_editId) m_editId->clear();
    if (m_editNom) {
        m_editNom->clear();
        m_editNom->setPlaceholderText("Nom de la maquette");
    }
    if (m_comboType) m_comboType->setCurrentIndex(0);
    if (m_dateCreation) {
        m_dateCreation->setMaximumDate(QDate::currentDate());
        m_dateCreation->setDate(DATE_VIDE);
    }
    if (m_comboStatut) m_comboStatut->setCurrentIndex(0);
    if (m_comboTaille) m_comboTaille->setCurrentIndex(0);
    if (m_editLargeur) m_editLargeur->clear();
    if (m_editLongueur) m_editLongueur->clear();
    for (QLineEdit *e : m_editMesures) if (e) e->clear();
    if (m_editDescription) {
        m_editDescription->clear();
        m_editDescription->setPlaceholderText("Description de la maquette ...");
    }
    if (m_editVideo) m_editVideo->clear();
    mettreAJourBoutonVideo("");
    mettreAJourImageMaquette();
    mettreAJourMesures();
}

void MainWindow::remplirFormulaire(const QString &id)
{
    Maquette m;
    if (!Maquette::charger(id, m)) return;

    if (m_comboType) m_comboType->blockSignals(true);
    if (m_comboStatut) m_comboStatut->blockSignals(true);
    if (m_editId) m_editId->setText(m.getId());
    if (m_editNom) m_editNom->setText(m.getNom());
    choisirTexte(m_comboType, m.getType());
    if (m_dateCreation) {
        m_dateCreation->setMaximumDate(QDate::currentDate());
        m_dateCreation->setDate(m.getDateCreation().isValid() ? m.getDateCreation() : DATE_VIDE);
    }
    choisirStatut(m_comboStatut, m.getStatut());
    choisirTexte(m_comboTaille, m.getTaille());
    if (m_editLargeur) m_editLargeur->setText(Maquette::texteMesure(m.getLargeur()));
    if (m_editLongueur) m_editLongueur->setText(Maquette::texteMesure(m.getLongueur()));
    for (auto it = m_editMesures.constBegin(); it != m_editMesures.constEnd(); ++it)
        if (it.value()) it.value()->setText(Maquette::texteMesure(m.getMesure(it.key())));
    if (m_editDescription) m_editDescription->setPlainText(m.getDescription());
    if (m_editVideo) m_editVideo->setText(m.getVideo());

    if (m_comboType) m_comboType->blockSignals(false);
    if (m_comboStatut) m_comboStatut->blockSignals(false);

    mettreAJourBoutonVideo(m.getVideo());
    mettreAJourImageMaquette();
    mettreAJourMesures();
}

void MainWindow::rafraichirListe()
{
    const QString recherche = m_rechercheListe ? m_rechercheListe->text() : "";
    const QString tri = m_comboTri ? m_comboTri->currentData().toString() : "";
    m_liste = Maquette::afficher(recherche, tri);
    m_pageCourante = qBound(1, m_pageCourante, nbPages());
    afficherPage(m_pageCourante);
    rafraichirStatistiques();
    verifierRetards();
}

int MainWindow::nbPages() const
{
    return qMax(1, int((m_liste.size() + LIGNES_PAR_PAGE - 1) / LIGNES_PAR_PAGE));
}

void MainWindow::afficherMaquetteDansTableau(const QString &id)
{
    for (int i = 0; i < m_liste.size(); ++i) {
        if (m_liste[i].getId() == id) {
            afficherPage(i / LIGNES_PAR_PAGE + 1);
            if (m_table) m_table->selectRow(i % LIGNES_PAR_PAGE);
            return;
        }
    }
    afficherPage(nbPages());
}

void MainWindow::afficherPage(int page)
{
    if (!m_table) return;
    m_pageCourante = page;
    const int debut = (page - 1) * LIGNES_PAR_PAGE;
    const int fin = qMin(int(m_liste.size()), debut + LIGNES_PAR_PAGE);

    QSet<QString> coches;
    for (int r = 0; r < m_table->rowCount(); ++r)
        if (m_table->item(r, 0) && m_table->item(r, 1) && m_table->item(r, 0)->checkState() == Qt::Checked)
            coches.insert(m_table->item(r, 1)->text());

    QString idSurligne;
    if (m_table->selectionModel()) {
        const QModelIndexList lignesChoisies = m_table->selectionModel()->selectedRows(1);
        if (!lignesChoisies.isEmpty()) idSurligne = lignesChoisies.first().data().toString();
    }

    m_table->setRowCount(0);
    for (int i = debut; i < fin; ++i) {
        const Maquette &m = m_liste[i];
        const int r = m_table->rowCount();
        m_table->insertRow(r);
        const bool retard = m.estEnRetard(JOURS_MAX_VALIDATION);

        QTableWidgetItem *coche = new QTableWidgetItem;
        coche->setFlags(Qt::ItemIsEnabled | Qt::ItemIsUserCheckable | Qt::ItemIsSelectable);
        coche->setCheckState(m_toutCoche || coches.contains(m.getId()) ? Qt::Checked : Qt::Unchecked);
        m_table->setItem(r, 0, coche);
        m_table->setItem(r, 1, new QTableWidgetItem(m.getId()));
        m_table->setItem(r, 2, new QTableWidgetItem(m.getNom()));
        m_table->setItem(r, 3, new QTableWidgetItem(m.getType()));
        m_table->setItem(r, 4, new QTableWidgetItem(m.getDateCreation().toString("dd/MM/yyyy")));
        m_table->setItem(r, 5, new QTableWidgetItem);
        m_table->setItem(r, 6, new QTableWidgetItem);

        QString fond = "#E3EDF7", texte = "#2C4A6B";
        if (m.getStatut().startsWith("Valid"))  { fond = "#E2F1E3"; texte = "#2E7D32"; }
        else if (m.getStatut() == "En attente") { fond = "#FAE6CF"; texte = "#A0622A"; }
        QWidget *cBadge = new QWidget;
        QHBoxLayout *hb = new QHBoxLayout(cBadge);
        hb->setContentsMargins(0, 0, 8, 0);
        QLabel *badge = new QLabel(m.getStatut());
        badge->setAlignment(Qt::AlignCenter);
        badge->setFixedSize(82, 30);
        badge->setStyleSheet(QString("background:%1; color:%2; border-radius:4px; font-size:13px; font-weight:500;").arg(fond, texte));
        hb->addWidget(badge);
        cBadge->setAttribute(Qt::WA_TransparentForMouseEvents);
        m_table->setCellWidget(r, 5, cBadge);

        QWidget *cAp = new QWidget;
        QHBoxLayout *ha = new QHBoxLayout(cAp);
        ha->setContentsMargins(6, 3, 10, 3);
        Miniature *mini = new Miniature;
        mini->setFixedSize(64, 40);
        mini->setImage(apercuPour(m));
        ha->addWidget(mini);
        cAp->setAttribute(Qt::WA_TransparentForMouseEvents);
        m_table->setCellWidget(r, 6, cAp);

        if (retard) {
            const QString tip = QString("⚠ En retard de validation : %1 jours").arg(m.getDateCreation().daysTo(QDate::currentDate()));
            for (int c = 0; c < 7; ++c) {
                if (m_table->item(r, c)) {
                    m_table->item(r, c)->setForeground(QColor("#B42318"));
                    m_table->item(r, c)->setToolTip(tip);
                }
            }
        }
    }
    for (int r = 0; r < m_table->rowCount(); ++r)
        if (!idSurligne.isEmpty() && m_table->item(r, 1) && m_table->item(r, 1)->text() == idSurligne)
            m_table->selectRow(r);
    construirePagination(nbPages());
}

void MainWindow::construirePagination(int nbPages)
{
    if (!m_paginationLayout) return;
    while (QLayoutItem *it = m_paginationLayout->takeAt(0)) {
        delete it->widget();
        delete it;
    }

    auto ajouterBouton = [this](const QString &texte, const QString &icone, int page, bool actif, bool enabled) {
        QPushButton *b = new QPushButton(texte);
        b->setObjectName(actif ? "pageBtnActif" : "pageBtn");
        b->setFixedSize(34, 34);
        b->setEnabled(enabled);
        b->setCursor(Qt::PointingHandCursor);
        if (!icone.isEmpty()) {
            b->setIcon(Icones::icone(icone, enabled ? QColor("#1B2333") : QColor("#C0C6CE"), 16));
            b->setIconSize(QSize(16, 16));
        }
        if (page > 0) connect(b, &QPushButton::clicked, this, [this, page]() { afficherPage(page); });
        m_paginationLayout->addWidget(b);
    };

    ajouterBouton("", "left", m_pageCourante - 1, false, m_pageCourante > 1);

    const QSet<int> visibles = {1, 2, 3, nbPages, m_pageCourante - 1, m_pageCourante, m_pageCourante + 1};
    QList<int> pages;
    for (int p : visibles) if (p >= 1 && p <= nbPages) pages << p;
    std::sort(pages.begin(), pages.end());
    int precedente = 0;
    for (int p : pages) {
        if (p - precedente > 1) {
            QLabel *l = new QLabel("...");
            l->setFixedWidth(28);
            l->setAlignment(Qt::AlignCenter);
            m_paginationLayout->addWidget(l);
        }
        ajouterBouton(QString::number(p), "", p, p == m_pageCourante, true);
        precedente = p;
    }

    ajouterBouton("", "right", m_pageCourante + 1, false, m_pageCourante < nbPages);
}

void MainWindow::onLigneCliquee(int ligne, int colonne)
{
    if (colonne == 0 || !m_table) return;
    if (QTableWidgetItem *it = m_table->item(ligne, 1)) {
        remplirFormulaire(it->text());
        Maquette::incrementerVues(it->text());
        rafraichirStatistiques();
    }
}

void MainWindow::onExporterPdf()
{
    QString chemin = QFileDialog::getSaveFileName(this, "Exporter la liste en PDF",
                                                  "Liste_Maquettes.pdf", "PDF (*.pdf)");
    if (chemin.isEmpty()) return;
    if (!chemin.endsWith(".pdf", Qt::CaseInsensitive)) chemin += ".pdf";

    QString html =
        "<html><body style='font-family:Arial'>"
        "<h1 style='color:#0B2A45; text-align:center'>FASHIONOVA</h1>"
        "<h2 style='text-align:center; color:#A07A3F'>Liste des maquettes</h2>"
        "<p style='text-align:center'>Exporté le " + QDate::currentDate().toString("dd/MM/yyyy") + "</p>"
                                                        "<table width='100%' border='1' cellspacing='0' cellpadding='6' style='border-collapse:collapse'>"
                                                        "<tr style='background:#0B2A45; color:white'>"
                                                        "<th>ID</th><th>Nom</th><th>Type</th><th>Taille</th><th>Largeur (cm)</th><th>Longueur (cm)</th>"
                                                        "<th>Mesures selon le type (cm)</th>"
                                                        "<th>Date de création</th><th>Statut</th><th>Description</th></tr>";
    for (const Maquette &m : m_liste) {
        const bool retard = m.estEnRetard(JOURS_MAX_VALIDATION);
        html += QString("<tr%1><td>%2</td><td>%3</td><td>%4</td><td>%5</td><td>%6</td><td>%7</td>")
                    .arg(retard ? " style='background:#FDEDEC'" : "",
                         m.getId(), m.getNom().toHtmlEscaped(), m.getType(), m.getTaille(),
                         Maquette::texteMesure(m.getLargeur()), Maquette::texteMesure(m.getLongueur()));
        QStringList mesures;
        for (const QString &colonne : Maquette::mesuresDuType(m.getType()))
            if (m.getMesure(colonne) >= 0)
                mesures << Maquette::mesure(colonne).libelle + " : " + Maquette::texteMesure(m.getMesure(colonne));
        html += "<td>" + mesures.join("<br>").toHtmlEscaped().replace("&lt;br&gt;", "<br>") + "</td>";
        html += QString("<td>%1</td><td>%2</td><td>%3</td></tr>")
                    .arg(m.getDateCreation().toString("dd/MM/yyyy"),
                         m.getStatut() + (retard ? " (en retard)" : ""),
                         m.getDescription().toHtmlEscaped());
    }
    html += "</table><p>Total : " + QString::number(m_liste.size()) + " maquette(s)</p></body></html>";

    QPdfWriter writer(chemin);
    writer.setPageSize(QPageSize(QPageSize::A4));
    writer.setPageOrientation(QPageLayout::Landscape);
    writer.setResolution(96);
    writer.setPageMargins(QMarginsF(12, 12, 12, 12), QPageLayout::Millimeter);

    QTextDocument doc;
    doc.setHtml(html);
    doc.setPageSize(writer.pageLayout().paintRectPixels(writer.resolution()).size());
    doc.print(&writer);

    QMessageBox::information(this, "Export PDF", "PDF créé avec succès :\n" + chemin);
    QDesktopServices::openUrl(QUrl::fromLocalFile(chemin));
}

void MainWindow::rafraichirStatistiques()
{
    static const QMap<QString, QString> pluriel = {
                                                    {"Robe", "Robes"}, {"Veste", "Vestes"}, {"Jupe", "Jupes"}, {"Pantalon", "Pantalons"},
                                                    {"T-shirt", "T-shirts"}, {"Chapeau", "Chapeaux"}, {"Chemise homme", "Chemises homme"}};
    QList<QPair<QString, int>> parType;
    const QMap<QString, int> t = Maquette::statsParType();
    for (const QString &nom : Maquette::types())
        if (t.value(nom) > 0) parType.append({pluriel.value(nom, nom), t.value(nom)});
    for (auto it = t.constBegin(); it != t.constEnd(); ++it)
        if (!it.key().trimmed().isEmpty() && !Maquette::types().contains(it.key()) && it.value() > 0)
            parType.append({it.key(), it.value()});
    if (m_donut) m_donut->setDonnees(parType);
    if (m_courbe) m_courbe->setDonnees(Maquette::statsParMois(6));
    if (m_barres) m_barres->setDonnees(Maquette::top5Consultees());
}

void MainWindow::mettreAJourBoutonVideo(const QString &video)
{
    if (!m_boutonPlus) return;
    const bool aVideo = !video.isEmpty();
    m_boutonPlus->setIcon(Icones::icone(aVideo ? "play" : "plus", OR, 26));
    m_boutonPlus->setToolTip(aVideo ? "Lire la vidéo de présentation : " + QFileInfo(video).fileName()
                                    : "Ajouter une vidéo de présentation");
}

void MainWindow::onChoisirVideo()
{
    if (!m_editId) return;
    const QString id = m_editId->text().trimmed();
    if (!Maquette::existe(id)) {
        QMessageBox::warning(this, "Vidéo", "Enregistrez d'abord la maquette (bouton Ajouter), puis associez une vidéo.");
        return;
    }
    const QString f = QFileDialog::getOpenFileName(this, "Choisir la vidéo de présentation", "",
                                                   "Vidéos (*.mp4 *.avi *.mov *.mkv *.wmv *.gif)");
    if (f.isEmpty()) return;
    if (Maquette::enregistrerVideo(id, f)) {
        if (m_editVideo) m_editVideo->setText(f);
        mettreAJourBoutonVideo(f);
        QMessageBox::information(this, "Vidéo", "Vidéo associée à la maquette " + id + " avec succès.");
    } else {
        QMessageBox::warning(this, "Vidéo", "Impossible d'enregistrer la vidéo.");
    }
}

void MainWindow::onBoutonPlus()
{
    if (!m_editId) return;
    Maquette m;
    if (!Maquette::charger(m_editId->text().trimmed(), m) || m.getVideo().isEmpty()) {
        onChoisirVideo();
        return;
    }
    if (!QFileInfo::exists(m.getVideo())) {
        QMessageBox::warning(this, "Vidéo", "Le fichier vidéo est introuvable :\n" + m.getVideo());
        return;
    }
    QDesktopServices::openUrl(QUrl::fromLocalFile(m.getVideo()));
}

void MainWindow::verifierRetards()
{
    if (!m_badgeCloche || !m_labelRetard) return;
    const QList<Maquette> retards = Maquette::enRetard(JOURS_MAX_VALIDATION);
    m_badgeCloche->setText(QString::number(retards.size()));
    m_labelRetard->setText(retards.isEmpty() ? ""
                                             : QString("⚠ %1 maquette(s) en retard").arg(retards.size()));

    if (!retards.isEmpty() && !m_alerteDejaAffichee && isVisible()) {
        m_alerteDejaAffichee = true;
        onCloche();
    }
}

void MainWindow::onCloche()
{
    const QList<Maquette> retards = Maquette::enRetard(JOURS_MAX_VALIDATION);
    if (retards.isEmpty()) {
        QMessageBox::information(this, "Alertes", "Aucune maquette en retard de validation.");
        return;
    }
    QString txt = QString("<b>%1 maquette(s) non validée(s) depuis plus de %2 jours :</b><ul>")
                      .arg(retards.size()).arg(JOURS_MAX_VALIDATION);
    for (const Maquette &m : retards)
        txt += QString("<li><b>%1</b> – %2 (%3) : <span style='color:#B42318'>%4 jours</span></li>")
                   .arg(m.getId(), m.getNom().toHtmlEscaped(), m.getStatut())
                   .arg(m.getDateCreation().daysTo(QDate::currentDate()));
    txt += "</ul>";

    QMessageBox box(this);
    box.setIcon(QMessageBox::Warning);
    box.setWindowTitle("Maquettes en retard de validation");
    box.setTextFormat(Qt::RichText);
    box.setText(txt);
    box.exec();
}

void MainWindow::placerImageSidebar()
{
    if (!m_imageSidebar || !m_sidebar) return;
    static const QPixmap full(":/images/sidebar.png");
    static const QPixmap bas(":/images/sidebar_bas.png");
    if (full.isNull() || bas.isNull()) return;

    const int W = m_sidebar->width();
    const int H = m_sidebar->height();
    if (W < 10 || H < 10) return;
    const qreal dpr = m_sidebar->devicePixelRatioF();
    const double k  = double(W) / full.width();

    const int hautSrc  = 262;
    const int basSrc   = 1100;
    const int hautDest = int(hautSrc * k);

    const int limite   = ui->btnArticles ? ui->btnArticles->geometry().bottom() + 8 : 400;
    const double ratio = double(bas.height()) / bas.width();
    int gh = int(W * ratio);
    int gw = W;
    if (gh > H - limite) { gh = qMax(0, H - limite); gw = int(gh / ratio); }

    QPixmap img(QSize(W, H) * dpr);
    img.setDevicePixelRatio(dpr);
    img.fill(QColor("#071E35"));
    QPainter p(&img);
    p.setRenderHint(QPainter::SmoothPixmapTransform, true);
    p.drawPixmap(QRect(0, hautDest - 1, W, H - hautDest + 1), full, QRect(0, hautSrc, full.width(), basSrc - hautSrc));
    p.drawPixmap(QRect(0, 0, W, hautDest), full, QRect(0, 0, full.width(), hautSrc));
    if (gh > 40) p.drawPixmap(QRect((W - gw) / 2, H - gh, gw, gh), bas);
    p.end();

    m_imageSidebar->setPixmap(img);
    m_imageSidebar->setGeometry(0, 0, W, H);
    m_imageSidebar->lower();
    m_imageSidebar->show();
}

void MainWindow::changeEvent(QEvent *ev)
{
    QMainWindow::changeEvent(ev);
    if (ev->type() == QEvent::ActivationChange && isActiveWindow() && m_table)
        rafraichirListe();
}

bool MainWindow::eventFilter(QObject *obj, QEvent *ev)
{
    if (m_editId && obj == m_editId && ev->type() == QEvent::MouseButtonPress)
        genererId();

    if (m_dateCreation && m_dateCreation->date() == DATE_VIDE) {
        const bool clicDate = ev->type() == QEvent::MouseButtonPress
                              && obj->parent() == m_dateCreation && qobject_cast<QLineEdit *>(obj);
        bool tabDate = false;
        if (obj == m_dateCreation && ev->type() == QEvent::FocusIn) {
            const Qt::FocusReason r = static_cast<QFocusEvent *>(ev)->reason();
            tabDate = r == Qt::MouseFocusReason || r == Qt::TabFocusReason || r == Qt::BacktabFocusReason;
        }
        if (clicDate || tabDate)
            m_dateCreation->setDate(QDate::currentDate());
    }

    if (m_sidebar && obj == m_sidebar && (ev->type() == QEvent::Resize || ev->type() == QEvent::Show
                                          || ev->type() == QEvent::LayoutRequest))
        placerImageSidebar();

    return QMainWindow::eventFilter(obj, ev);
}