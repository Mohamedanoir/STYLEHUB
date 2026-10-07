#include "maquettespage.h"
#include "ui_maquettespage.h"

#include "maquette.h"
#include "icones.h"
#include "statwidgets.h"
#include "widgets.h"

#include <QApplication>
#include <QDate>
#include <QDesktopServices>
#include <QFileDialog>
#include <QHeaderView>
#include <QKeyEvent>
#include <QLabel>
#include <QMessageBox>
#include <QMovie>
#include <QPrinter>
#include <QPushButton>
#include <QRegularExpression>
#include <QTableWidget>
#include <QTextDocument>
#include <QTimer>
#include <QToolButton>
#include <QUrl>

namespace {
const QColor kNavy("#0B2A45");
const QColor kSable("#8C7B66");

QString dossierImages() { return QStringLiteral(":/maquettes/images/"); }

QString fichierAvant(const QString &type)   { return dossierImages() + type.toLower() + "_avant.png"; }
QString fichierValidee(const QString &type) { return dossierImages() + type.toLower() + "_validee.gif"; }

struct StyleStatut { QColor fond; QColor texte; QColor pastille; };
StyleStatut styleStatut(const QString &statut)
{
    if (statut.startsWith("Valid")) return { QColor("#DDF1E3"), QColor("#2E7D4F"), QColor("#2E9B5F") };
    if (statut == "En attente")     return { QColor("#E8ECF2"), QColor("#4A5568"), QColor("#7D8597") };
    return                                { QColor("#FBEBD3"), QColor("#B26B12"), QColor("#E59A2F") };   // En cours
}
} // namespace

// ============================================================================ ctor
MaquettesPage::MaquettesPage(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MaquettesPage)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Widget);

    // Le menu latéral et le bouton « hamburger » appartiennent à la fenêtre principale STYLEHUB
    ui->sidebar->hide();
    ui->btnMenu->hide();

    appliquerStyle();
    installerIcones();

    // --- champs du formulaire
    ui->dateCreation->setDisplayFormat("dd/MM/yyyy");
    ui->dateCreation->setCalendarPopup(true);
    ui->dateCreation->setMaximumDate(QDate::currentDate());
    ui->dateCreation->setDate(QDate::currentDate());
    ui->comboTri->setCurrentIndex(0);

    // --- tableau
    auto *t = ui->tableMaquettes;
    t->setColumnCount(6);
    t->setHorizontalHeaderLabels({ "ID", "Nom", "Type", "Date de création", "Statut", "Aperçu" });
    t->verticalHeader()->hide();
    t->verticalHeader()->setDefaultSectionSize(46);
    t->horizontalHeader()->setFixedHeight(38);
    t->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    t->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    t->horizontalHeader()->setHighlightSections(false);
    t->setFocusPolicy(Qt::NoFocus);
    t->setFixedHeight(38 + PAR_PAGE * 46 + 2);

    // --- signaux
    connect(ui->btnAjouter,      &QPushButton::clicked, this, &MaquettesPage::onAjouter);
    connect(ui->btnModifier,     &QPushButton::clicked, this, &MaquettesPage::onModifier);
    connect(ui->btnSupprimer,    &QPushButton::clicked, this, &MaquettesPage::onSupprimer);
    connect(ui->btnAfficher,     &QPushButton::clicked, this, &MaquettesPage::onAfficher);
    connect(ui->btnReinitialiser,&QPushButton::clicked, this, &MaquettesPage::onReinitialiser);
    connect(ui->btnExporterPdf,  &QPushButton::clicked, this, &MaquettesPage::onExporterPdf);
    connect(ui->btnPlus,         &QToolButton::clicked, this, [this] {
        if (m_videoCourante.isEmpty()) onChoisirVideo(); else onLireVideo();
    });
    connect(ui->apercu->boutonCrayon(), &QToolButton::clicked, this, &MaquettesPage::onChoisirVideo);
    connect(ui->btnCloche,       &QToolButton::clicked, this, [this] { verifierRetards(true); });
    connect(ui->btnCalendrier,   &QToolButton::clicked, this, [this] {
        ui->dateCreation->setFocus();
        QKeyEvent press(QEvent::KeyPress, Qt::Key_Down, Qt::AltModifier);
        QApplication::sendEvent(ui->dateCreation, &press);
    });

    connect(ui->tableMaquettes, &QTableWidget::cellClicked, this, &MaquettesPage::onLigneCliquee);
    connect(ui->editRechercheListe, &QLineEdit::textChanged, this, [this] { m_page = 0; rafraichirListe(); });
    connect(ui->editRechercheGlobale, &QLineEdit::textChanged, ui->editRechercheListe, &QLineEdit::setText);
    connect(ui->comboTri, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int) { m_page = 0; rafraichirListe(); });

    connect(ui->comboType,   &QComboBox::currentTextChanged, this, [this] { afficherApercu(ui->comboType->currentText(), ui->comboStatut->currentText()); });
    connect(ui->comboStatut, &QComboBox::currentTextChanged, this, [this] { onStatutChange(); });

    // Miniatures : image fixe / animation
    connect(ui->mini1, &Miniature::clique, this, [this] {
        ui->apercu->afficherImage(QPixmap(fichierAvant(ui->comboType->currentText())));
    });
    connect(ui->mini2, &Miniature::clique, this, [this] {
        ui->apercu->afficherAnimation(fichierValidee(ui->comboType->currentText()));
    });

    // Vérification automatique des retards toutes les minutes
    m_timerRetard = new QTimer(this);
    connect(m_timerRetard, &QTimer::timeout, this, [this] { rafraichirListe(); verifierRetards(false); });
    m_timerRetard->start(60 * 1000);

    onReinitialiser();
    rafraichirTout();
}

MaquettesPage::~MaquettesPage() { delete ui; }

void MaquettesPage::showEvent(QShowEvent *event)
{
    QMainWindow::showEvent(event);
    rafraichirTout();
    if (!m_alerteAffichee)                       // pop-up une seule fois, à la première ouverture du module
        QTimer::singleShot(350, this, [this] { verifierRetards(true); });
}

// ============================================================================ style
void MaquettesPage::appliquerStyle()
{
    setStyleSheet(QStringLiteral(R"(
QMainWindow, QWidget#centralwidget, QWidget#zoneDroite, QWidget#scrollContents, QWidget#pageMaquettes { background:#F7F3EC; }
QScrollArea#scrollMaquettes { background:#F7F3EC; border:none; }
QScrollBar:vertical { background:transparent; width:10px; margin:0; }
QScrollBar::handle:vertical { background:#D9CCB4; border-radius:5px; min-height:30px; }
QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height:0; }

QWidget#topbar { background:#F7F3EC; }
QLineEdit#editRechercheGlobale { background:#FFFFFF; border:1px solid #E7DCCB; border-radius:19px; padding:0 14px; color:#1F2A3D; }
QToolButton[role="iconBtn"] { background:transparent; border:none; }
QLabel#labelUser { font-size:15px; font-weight:600; color:#0B2A45; }

QFrame[role="carte"]   { background:#FFFFFF; border:1px solid #EADFCF; border-radius:12px; }
QFrame[role="entete"]  { background:#EFE3CF; border:none; border-top-left-radius:12px; border-top-right-radius:12px; }
QLabel[role="titreCarte"] { font-size:15px; font-weight:700; color:#0B2A45; background:transparent; }
QWidget[role="corps"]  { background:transparent; }
QLabel[role="labelChamp"] { font-weight:600; color:#0B2A45; background:transparent; }

QFrame[role="champ"]   { background:#FFFFFF; border:1px solid #E1D6C3; border-radius:8px; }
QFrame[role="champ"] QLabel { background:transparent; border:none; }
QLineEdit[role="interne"], QDateEdit[role="interne"], QComboBox[role="interne"], QComboBox[role="interneGras"]
    { border:none; background:transparent; padding:2px 4px; min-height:26px; color:#1F2A3D; }
QComboBox[role="interneGras"] { font-weight:600; }
QTextEdit { background:#FFFFFF; border:1px solid #E1D6C3; border-radius:8px; padding:4px 6px; color:#1F2A3D; }
QLineEdit:focus, QTextEdit:focus { border-color:#C9A36A; }
QComboBox::drop-down { border:none; width:22px; }
QComboBox::down-arrow { image:url(:/icons/chevron_dark.svg); width:12px; height:12px; }
QDateEdit::drop-down { border:none; width:0px; }
QComboBox QAbstractItemView { background:#FFFFFF; border:1px solid #E1D6C3; selection-background-color:#EFE3CF; selection-color:#0B2A45; outline:0; }
QFrame#fondPastille { background:#F3F0EA; border:none; border-radius:8px; }

QPushButton[role="btnPrimaire"], QPushButton[role="btnGris"], QPushButton[role="btnBeige"], QPushButton[role="btnClair"], QPushButton[role="btnBlanc"]
    { border:none; border-radius:8px; padding:8px 16px; font-weight:700; min-height:22px; }
QPushButton[role="btnPrimaire"] { background:#0B2A45; color:#FFFFFF; }
QPushButton[role="btnPrimaire"]:hover { background:#163D63; }
QPushButton[role="btnGris"]   { background:#D9DDE3; color:#3A4252; }
QPushButton[role="btnGris"]:hover { background:#C9CED6; }
QPushButton[role="btnBeige"]  { background:#EFE3CF; color:#6B4F2A; }
QPushButton[role="btnBeige"]:hover { background:#E6D5B8; }
QPushButton[role="btnClair"]  { background:#E6EAF0; color:#3A4252; }
QPushButton[role="btnClair"]:hover { background:#D6DCE6; }
QPushButton[role="btnBlanc"]  { background:#FFFFFF; color:#0B2A45; border:1px solid #E1D6C3; font-weight:600; }
QPushButton[role="btnBlanc"]:hover { background:#F7F0E3; }
QToolButton[role="btnPlus"] { background:transparent; border:none; border-radius:8px; }
QToolButton[role="btnPlus"]:hover { background:#F1E7D5; }

QComboBox[role="tri"] { background:#FFFFFF; border:1px solid #E1D6C3; border-radius:8px; padding:4px 10px; color:#1F2A3D; }
QLabel#labelTri { font-weight:700; color:#0B2A45; background:transparent; }
QLineEdit#editRechercheListe { background:#FFFFFF; border:1px solid #E1D6C3; border-radius:8px; padding:6px 10px; min-height:22px; color:#1F2A3D; }

QTableWidget { background:#FFFFFF; border:none; outline:0; color:#1F2A3D; alternate-background-color:#FBF8F2; }
QTableWidget::item { padding:4px 8px; border-bottom:1px solid #F0E8DA; }
QTableWidget::item:selected { background:#EFE3CF; color:#0B2A45; }
QHeaderView::section { background:#F7F0E3; color:#0B2A45; font-weight:700; border:none; padding:6px 8px; }

QPushButton[role="page"] { background:#FFFFFF; color:#0B2A45; border:1px solid #E1D6C3; border-radius:6px; min-width:30px; max-width:30px; min-height:30px; max-height:30px; font-weight:600; }
QPushButton[role="page"]:hover { background:#F7F0E3; }
QPushButton[role="page"][actif="true"] { background:#0B2A45; color:#FFFFFF; border-color:#0B2A45; }

QFrame[role="blocStat"] { background:#FFFFFF; border:1px solid #EADFCF; border-radius:10px; }
QLabel[role="titreStat"] { font-weight:700; color:#0B2A45; background:transparent; }
QLabel#labelRetard { background:transparent; font-weight:600; }
)"));
}

void MaquettesPage::installerIcones()
{
    const QColor encre("#2C3240");

    ui->banniere->setImage(QPixmap(dossierImages() + "banniere.png"));

    // en-têtes de cartes : pictogramme blanc dans un carré bleu nuit
    const struct { QLabel *l; const char *nom; } entetes[] = {
        { ui->iconeFormulaire, "clipboard" }, { ui->iconeListe, "list" }, { ui->iconeStats, "stats" } };
    for (const auto &e : entetes) {
        e.l->setPixmap(Icones::pixmap(e.nom, Qt::white, 18));
        e.l->setFixedSize(30, 30);
        e.l->setAlignment(Qt::AlignCenter);
        e.l->setStyleSheet("background:#0B2A45; border:none; border-radius:7px;");
    }

    // champs
    ui->iconeId->setPixmap(Icones::pixmap("idcard", kSable, 18));
    ui->iconeNom->setPixmap(Icones::pixmap("namecard", kSable, 18));
    ui->iconeType->setPixmap(Icones::pixmap("type", kSable, 18));
    ui->btnCalendrier->setIcon(Icones::icone("calendar", kSable, 18));

    // boutons
    ui->btnReinitialiser->setIcon(Icones::icone("reset", kNavy, 16));
    ui->btnExporterPdf->setIcon(Icones::icone("pdf", Qt::white, 16));
    ui->btnAjouter->setIcon(Icones::icone("plus", Qt::white, 16));
    ui->btnModifier->setIcon(Icones::icone("pencil", QColor("#3A4252"), 16));
    ui->btnSupprimer->setIcon(Icones::icone("trash", QColor("#6B4F2A"), 16));
    ui->btnAfficher->setIcon(Icones::icone("eye", QColor("#3A4252"), 16));
    ui->btnPlus->setIcon(Icones::icone("plus", QColor("#C59A62"), 22));
    ui->btnPlus->setIconSize(QSize(22, 22));
    ui->btnPlus->setToolTip("Ajouter une vidéo de présentation");
    ui->apercu->boutonCrayon()->setToolTip("Changer la vidéo de présentation");

    // barre du haut
    ui->editRechercheGlobale->addAction(Icones::icone("search", kSable, 18), QLineEdit::LeadingPosition);
    ui->editRechercheListe->addAction(Icones::icone("search", kSable, 16), QLineEdit::LeadingPosition);
    ui->btnCloche->setIcon(Icones::icone("bell", kNavy, 22));
    ui->btnCloche->setIconSize(QSize(22, 22));
    ui->labelAvatar->setPixmap(Icones::pixmap("avatar", kNavy, 30));
    ui->btnCompte->setIcon(Icones::icone("chevron", kNavy, 14));

    // pastille « nombre de maquettes en retard » sur la cloche
    m_badgeCloche = new QLabel(ui->btnCloche);
    m_badgeCloche->setAlignment(Qt::AlignCenter);
    m_badgeCloche->setFixedSize(17, 17);
    m_badgeCloche->move(ui->btnCloche->width() - 20, 2);
    m_badgeCloche->setStyleSheet("background:#D9534F; color:white; border-radius:8px; font-size:10px; font-weight:700;");
    m_badgeCloche->hide();
}

// ============================================================================ données
QString MaquettesPage::idCourant() const { return ui->editId->text().trimmed().toUpper(); }

QString MaquettesPage::prochainId() const
{
    int max = 0;
    static const QRegularExpression re("^MQ(\\d+)$");
    for (const Maquette &m : Maquette::afficher("", ""))
        if (auto mt = re.match(m.getId()); mt.hasMatch()) max = qMax(max, mt.captured(1).toInt());
    return QString("MQ%1").arg(max + 1, 3, 10, QChar('0'));
}

Maquette MaquettesPage::depuisFormulaire() const
{
    return Maquette(ui->editId->text(), ui->editNom->text(), ui->comboType->currentText(),
                    ui->dateCreation->date(), ui->comboStatut->currentText(),
                    ui->editDescription->toPlainText(), m_videoCourante);
}

void MaquettesPage::remplirFormulaire(const Maquette &m)
{
    ui->editId->setText(m.getId());
    ui->editNom->setText(m.getNom());
    ui->comboType->blockSignals(true);   ui->comboType->setCurrentText(m.getType());     ui->comboType->blockSignals(false);
    ui->comboStatut->blockSignals(true); ui->comboStatut->setCurrentText(m.getStatut()); ui->comboStatut->blockSignals(false);
    ui->dateCreation->setDate(m.getDateCreation());
    ui->editDescription->setPlainText(m.getDescription());
    majBoutonVideo(m.getVideo());
    onStatutChange();
    afficherApercu(m.getType(), m.getStatut());
}

void MaquettesPage::majBoutonVideo(const QString &video)
{
    m_videoCourante = video;
    const bool a = !video.isEmpty();
    ui->btnPlus->setIcon(Icones::icone(a ? "play" : "plus", QColor("#C59A62"), 22));
    ui->btnPlus->setToolTip(a ? "Lire la vidéo de présentation" : "Ajouter une vidéo de présentation");
}

void MaquettesPage::onStatutChange()
{
    const StyleStatut st = styleStatut(ui->comboStatut->currentText());
    ui->pastilleStatut->setStyleSheet(QString("background:%1; border:none; border-radius:7px;").arg(st.pastille.name()));
    afficherApercu(ui->comboType->currentText(), ui->comboStatut->currentText());
}

// Statut « Validée » -> GIF animé ; sinon image fixe (toile)
void MaquettesPage::afficherApercu(const QString &type, const QString &statut)
{
    const QPixmap avant(fichierAvant(type));
    if (statut.startsWith("Valid")) ui->apercu->afficherAnimation(fichierValidee(type));
    else if (!avant.isNull())       ui->apercu->afficherImage(avant);
    else                            ui->apercu->afficherVide("Aucune image");

    ui->mini1->setImage(avant);
    QMovie film(fichierValidee(type));
    film.jumpToFrame(0);
    ui->mini2->setImage(film.currentPixmap());
    ui->mini3->setImage(QPixmap());
}

// ============================================================================ liste / pagination
void MaquettesPage::rafraichirTout()
{
    rafraichirListe();
    rafraichirStats();
    verifierRetards(false);
}

void MaquettesPage::rafraichirListe()
{
    QString tri;
    switch (ui->comboTri->currentIndex()) { case 1: tri = "date"; break; case 2: tri = "type"; break; case 3: tri = "statut"; break; default: break; }
    const QList<Maquette> liste = Maquette::afficher(ui->editRechercheListe->text().trimmed(), tri);

    const int totalPages = qMax(1, (liste.size() + PAR_PAGE - 1) / PAR_PAGE);
    m_page = qBound(0, m_page, totalPages - 1);

    auto *t = ui->tableMaquettes;
    t->blockSignals(true);
    t->setRowCount(0);
    const int debut = m_page * PAR_PAGE;
    for (int i = debut; i < qMin(debut + PAR_PAGE, int(liste.size())); ++i) {
        const Maquette &m = liste.at(i);
        const int r = t->rowCount();
        t->insertRow(r);

        auto *id = new QTableWidgetItem(m.getId());
        id->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable);
        id->setCheckState(Qt::Unchecked);
        t->setItem(r, 0, id);
        t->setItem(r, 1, new QTableWidgetItem(m.getNom()));
        t->setItem(r, 2, new QTableWidgetItem(m.getType()));
        t->setItem(r, 3, new QTableWidgetItem(m.getDateCreation().toString("dd/MM/yyyy")));

        auto *st = new QTableWidgetItem(m.getStatut());
        const StyleStatut ss = styleStatut(m.getStatut());
        st->setBackground(ss.fond);
        st->setForeground(ss.texte);
        st->setTextAlignment(Qt::AlignCenter);
        t->setItem(r, 4, st);

        auto *ap = new QTableWidgetItem(m.getVideo().isEmpty() ? "Image" : "Vidéo");
        t->setItem(r, 5, ap);

        if (m.estEnRetard(JOURS_RETARD)) {            // métier : maquette non validée depuis > 7 jours
            for (int c : { 0, 1, 2, 3, 5 }) {
                t->item(r, c)->setForeground(QColor("#C0392B"));
                QFont f = t->item(r, c)->font(); f.setBold(true); t->item(r, c)->setFont(f);
            }
        }
    }
    t->blockSignals(false);
    rafraichirPagination(totalPages);
}

void MaquettesPage::rafraichirPagination(int totalPages)
{
    QLayout *lay = ui->layoutPagination;
    while (QLayoutItem *it = lay->takeAt(0)) {
        if (QWidget *w = it->widget()) w->deleteLater();
        delete it;
    }
    auto bouton = [this, lay](const QString &texte, bool actif, int cible) {
        auto *b = new QPushButton(texte);
        b->setProperty("role", "page");
        b->setProperty("actif", actif);
        b->setCursor(Qt::PointingHandCursor);
        connect(b, &QPushButton::clicked, this, [this, cible] { m_page = cible; rafraichirListe(); });
        lay->addWidget(b);
    };
    bouton("‹", false, qMax(0, m_page - 1));
    const int fenetre = 5;
    int a = qMax(0, m_page - fenetre / 2);
    int z = qMin(totalPages, a + fenetre);
    a = qMax(0, z - fenetre);
    for (int p = a; p < z; ++p) bouton(QString::number(p + 1), p == m_page, p);
    bouton("›", false, qMin(totalPages - 1, m_page + 1));
}

void MaquettesPage::onLigneCliquee(int row, int)
{
    auto *it = ui->tableMaquettes->item(row, 0);
    if (!it) return;
    Maquette m;
    if (Maquette::charger(it->text(), m)) remplirFormulaire(m);
}

// ============================================================================ CRUD
void MaquettesPage::onAjouter()
{
    QString err;
    if (!depuisFormulaire().ajouter(err)) { QMessageBox::warning(this, "Ajouter une maquette", err); return; }
    QMessageBox::information(this, "Ajouter une maquette", "Maquette ajoutée avec succès.");
    m_page = 1 << 20;                                  // se placer sur la dernière page (bornée dans rafraichirListe)
    rafraichirTout();
    onReinitialiser();
}

void MaquettesPage::onModifier()
{
    QString err;
    if (!depuisFormulaire().modifier(err)) { QMessageBox::warning(this, "Modifier une maquette", err); return; }
    QMessageBox::information(this, "Modifier une maquette", "Maquette modifiée avec succès.");
    rafraichirTout();
}

void MaquettesPage::onSupprimer()
{
    // 1) cases cochées (suppression multiple)   2) sinon la maquette du formulaire
    QStringList ids;
    for (int r = 0; r < ui->tableMaquettes->rowCount(); ++r)
        if (auto *it = ui->tableMaquettes->item(r, 0); it && it->checkState() == Qt::Checked) ids << it->text();
    if (ids.isEmpty() && !idCourant().isEmpty()) ids << idCourant();
    if (ids.isEmpty()) { QMessageBox::information(this, "Supprimer", "Sélectionnez ou cochez au moins une maquette."); return; }

    const QString q = ids.size() == 1 ? QString("Supprimer la maquette %1 ?").arg(ids.first())
                                      : QString("Supprimer %1 maquettes (%2) ?").arg(ids.size()).arg(ids.join(", "));
    if (QMessageBox::question(this, "Supprimer", q) != QMessageBox::Yes) return;

    int echecs = 0;
    for (const QString &id : ids) if (!Maquette::supprimer(id)) ++echecs;
    if (echecs) QMessageBox::warning(this, "Supprimer", QString("%1 suppression(s) impossible(s).").arg(echecs));
    onReinitialiser();
    rafraichirTout();
}

void MaquettesPage::onAfficher()
{
    Maquette m;
    if (idCourant().isEmpty() || !Maquette::charger(idCourant(), m)) {
        QMessageBox::information(this, "Afficher", "Maquette introuvable : saisissez un ID existant ou cliquez sur une ligne.");
        return;
    }
    Maquette::incrementerVues(m.getId());               // +1 consultation (Top 5)
    Maquette::charger(m.getId(), m);
    remplirFormulaire(m);
    rafraichirStats();
    QMessageBox::information(this, "Maquette " + m.getId(),
        QString("%1\nType : %2\nStatut : %3\nCréée le : %4\nConsultations : %5\nVidéo : %6\n\n%7")
            .arg(m.getNom(), m.getType(), m.getStatut(), m.getDateCreation().toString("dd/MM/yyyy"))
            .arg(m.getVues())
            .arg(m.getVideo().isEmpty() ? "aucune" : m.getVideo(), m.getDescription()));
}

void MaquettesPage::onReinitialiser()
{
    ui->editId->setText(prochainId());
    ui->editNom->clear();
    ui->editDescription->clear();
    ui->comboType->setCurrentIndex(0);
    ui->comboStatut->setCurrentIndex(0);
    ui->dateCreation->setDate(QDate::currentDate());
    ui->tableMaquettes->clearSelection();
    majBoutonVideo(QString());
    onStatutChange();
}

// ============================================================================ vidéo (métier 1)
void MaquettesPage::onChoisirVideo()
{
    const QString id = idCourant();
    if (id.isEmpty() || !Maquette::existe(id)) {
        QMessageBox::information(this, "Vidéo", "Enregistrez d'abord la maquette (bouton Ajouter), puis ajoutez sa vidéo.");
        return;
    }
    const QString f = QFileDialog::getOpenFileName(this, "Choisir une vidéo de présentation", QString(),
                                                   "Vidéos (*.mp4 *.avi *.mov *.mkv *.wmv);;Tous les fichiers (*)");
    if (f.isEmpty()) return;
    if (!Maquette::enregistrerVideo(id, f)) { QMessageBox::warning(this, "Vidéo", "Impossible d'enregistrer la vidéo."); return; }
    majBoutonVideo(f);
    rafraichirListe();
}

void MaquettesPage::onLireVideo()
{
    if (m_videoCourante.isEmpty()) return;
    if (!QDesktopServices::openUrl(QUrl::fromLocalFile(m_videoCourante)))
        QMessageBox::warning(this, "Vidéo", "Impossible d'ouvrir le fichier :\n" + m_videoCourante);
}

// ============================================================================ retards (métier 2)
void MaquettesPage::verifierRetards(bool popup)
{
    const QList<Maquette> retards = Maquette::enRetard(JOURS_RETARD);
    const int n = retards.size();

    if (n > 0) {
        ui->labelRetard->setText(QString("⚠  %1 maquette(s) non validée(s) depuis plus de %2 jours").arg(n).arg(JOURS_RETARD));
        ui->labelRetard->setStyleSheet("color:#C0392B; background:transparent; font-weight:600;");
    } else {
        ui->labelRetard->setText("✔  Aucune maquette en retard de validation");
        ui->labelRetard->setStyleSheet("color:#2E7D4F; background:transparent; font-weight:600;");
    }
    if (m_badgeCloche) { m_badgeCloche->setText(QString::number(n)); m_badgeCloche->setVisible(n > 0); }

    if (popup) {
        m_alerteAffichee = true;
        if (n == 0) { QMessageBox::information(this, "Maquettes en retard", "Aucune maquette en retard de validation."); return; }
        QStringList lignes;
        for (const Maquette &m : retards)
            lignes << QString("• %1 — %2 (%3 jours)").arg(m.getId(), m.getNom()).arg(m.getDateCreation().daysTo(QDate::currentDate()));
        QMessageBox::warning(this, "Maquettes en retard de validation",
            QString("%1 maquette(s) ne sont pas validées depuis plus de %2 jours :\n\n%3").arg(n).arg(JOURS_RETARD).arg(lignes.join("\n")));
    }
}

// ============================================================================ statistiques
void MaquettesPage::rafraichirStats()
{
    QList<QPair<QString, int>> parType;
    const QMap<QString, int> types = Maquette::statsParType();
    for (const QString &t : { "Robe", "Veste", "Jupe" })
        if (types.value(t) > 0) parType.append({ t, types.value(t) });
    ui->chartType->setDonnees(parType);
    ui->chartEvolution->setDonnees(Maquette::statsParMois(6));
    ui->chartTop->setDonnees(Maquette::top5Consultees());
}

// ============================================================================ export PDF
void MaquettesPage::onExporterPdf()
{
    const QString fichier = QFileDialog::getSaveFileName(this, "Exporter la liste des maquettes",
                                                         "maquettes.pdf", "PDF (*.pdf)");
    if (fichier.isEmpty()) return;

    QString html = "<h2 style='color:#0B2A45'>FashioNova — Liste des maquettes</h2>"
                   "<p>Édité le " + QDate::currentDate().toString("dd/MM/yyyy") + "</p>"
                   "<table border='1' cellspacing='0' cellpadding='5' width='100%'>"
                   "<tr style='background:#EFE3CF'><th>ID</th><th>Nom</th><th>Type</th><th>Date</th><th>Statut</th><th>Consultations</th></tr>";
    for (const Maquette &m : Maquette::afficher(ui->editRechercheListe->text().trimmed(), "")) {
        html += QString("<tr><td>%1</td><td>%2</td><td>%3</td><td>%4</td><td>%5</td><td align='center'>%6</td></tr>")
                    .arg(m.getId(), m.getNom().toHtmlEscaped(), m.getType(),
                         m.getDateCreation().toString("dd/MM/yyyy"), m.getStatut()).arg(m.getVues());
    }
    html += "</table>";

    QTextDocument doc;
    doc.setHtml(html);
    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setPageSize(QPageSize(QPageSize::A4));
    printer.setOutputFileName(fichier);
    doc.print(&printer);
    QMessageBox::information(this, "Export PDF", "Liste exportée :\n" + fichier);
}
