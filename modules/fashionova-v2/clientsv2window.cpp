#include "clientsv2window.h"
#include "ui_clientsv2window.h"
#include "statswidgets.h"

#include <QColor>
#include <QDate>
#include <QFile>
#include <QGuiApplication>
#include <QLocale>
#include <QMap>
#include <QScreen>
#include <QTimer>
#include <QEvent>
#include <QFileDialog>
#include <QHeaderView>
#include <QMessageBox>
#include <QPair>
#include <QPdfWriter>
#include <QPushButton>
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <QSignalBlocker>
#include <QStatusBar>
#include <QStringList>
#include <QTextDocument>
#include <algorithm>

namespace {

const QString kDateFmt = QStringLiteral("dd/MM/yyyy");

// couleur de fond / texte de la colonne « Statut »
QPair<QColor, QColor> statutColors(const QString &type)
{
    if (type == "VIP")     return qMakePair(QColor("#F3E3C3"), QColor("#A67C2E"));
    if (type == "Premium") return qMakePair(QColor("#C9E2F5"), QColor("#1A5A8A"));
    return qMakePair(QColor("#DBEAF7"), QColor("#2A6496"));
}

int typeRank(const QString &t)
{
    if (t == "VIP") return 0;
    if (t == "Premium") return 1;
    return 2;
}

// couleur de fond / texte du statut d'une commande
QPair<QColor, QColor> cmdColors(const QString &statut)
{
    if (statut == QString::fromUtf8("Livrée"))
        return qMakePair(QColor("#DDF1E4"), QColor("#1E7B3F"));
    if (statut == "En cours")
        return qMakePair(QColor("#DBEAF7"), QColor("#2A6496"));
    if (statut == "En attente")
        return qMakePair(QColor("#F3E3C3"), QColor("#A67C2E"));
    return qMakePair(QColor("#F8DDDD"), QColor("#B03A3A")); // Annulée
}

// catégorie de vêtement déduite de la désignation d'une commande
QString categorieCommande(const QString &designation)
{
    const QString d = designation.toLower();
    if (d.contains(QString::fromUtf8("retouche"))) return QString::fromUtf8("Retouches");
    if (d.contains("robe"))    return "Robes";
    if (d.contains("abaya"))   return "Abayas";
    if (d.contains("costume") || d.contains("tailleur") || d.contains("veston"))
        return "Costumes & tailleurs";
    if (d.contains("chemise")) return "Chemises";
    if (d.contains("jupe"))    return "Jupes";
    if (d.contains("ensemble")) return "Ensembles";
    return "Autres";
}

} // namespace

FashionNovaV2Window::FashionNovaV2Window(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::FashionNovaV2Window)
{
    ui->setupUi(this);

    auto S = [](const char *s) { return QString::fromUtf8(s); };
    // --- clients lus depuis la base Oracle (classe Client)
    clients = Client::lister();
    if (clients.isEmpty() && !Client::derniereErreur().isEmpty()) {
        QTimer::singleShot(0, this, [this] {
            QMessageBox::critical(this, QString::fromUtf8("Lecture de la base Oracle"),
                QString::fromUtf8("La connexion à Oracle est établie, mais la table CLIENTS "
                                   "n'a pas pu être lue. Vérifiez la migration du schéma clients "
                                   "et les colonnes ID_APP, NOM_CLIENT, PRENOM_CLIENT, TELEPHONE, "
                                   "EMAIL, ADRESSE, TYPE_CLIENT, DATE_INSCRIPTION et REMARQUES.\n\nDétail : ")
                    + Client::derniereErreur());
        });
    }

    // --- historique des commandes (données d'exemple)
    const QString livree  = S("Livrée");
    const QString annulee = S("Annulée");
    commandes = {
        { "CMD-1042", "CL-001", "12/09/2024", S("Robe de soirée sur mesure (satin bleu nuit)"), "En cours", 1850.0 },
        { "CMD-1019", "CL-001", "02/07/2024", S("Tailleur : veston + pantalon"), livree, 980.0 },
        { "CMD-0987", "CL-001", "21/05/2024", S("Retouches robe de cocktail"), livree, 120.0 },
        { "CMD-0954", "CL-001", "30/03/2024", S("Robe de mariée (essayage)"), annulee, 2400.0 },
        { "CMD-1051", "CL-002", "20/09/2024", S("Chemise sur mesure"), "En attente", 180.0 },
        { "CMD-1030", "CL-002", "15/08/2024", S("Costume 2 pièces"), livree, 1200.0 },
        { "CMD-1038", "CL-003", "03/09/2024", S("Robe de soirée"), "En cours", 1450.0 },
        { "CMD-1025", "CL-003", "28/07/2024", S("Abaya brodée"), livree, 650.0 },
        { "CMD-1002", "CL-003", "12/06/2024", S("Jupe plissée"), livree, 240.0 },
        { "CMD-1044", "CL-004", "14/09/2024", S("Costume de mariage"), "En cours", 2100.0 },
        { "CMD-1033", "CL-005", "25/08/2024", S("Ensemble casual"), livree, 520.0 },
        { "CMD-1047", "CL-006", "16/09/2024", S("Retouches robe"), "En attente", 90.0 },
        { "CMD-1011", "CL-006", "12/08/2024", S("Robe cocktail"), livree, 780.0 },
    };

    {
        QHeaderView *ch = ui->tableCommandes->horizontalHeader();
        ch->setSectionResizeMode(QHeaderView::ResizeToContents);
        ch->setSectionResizeMode(2, QHeaderView::Stretch);
        ch->setStretchLastSection(false);
        ui->tableCommandes->verticalHeader()->setDefaultSectionSize(34);
    }
    connect(ui->comboStatutCmd, qOverload<int>(&QComboBox::currentIndexChanged),
            this, [this](int) { refreshHistorique(); });
    connect(ui->btnVoirCommandes, &QPushButton::clicked, this, [this] {
        ui->stackPages->setCurrentIndex(2);
        ui->btnNavCommandes->setChecked(true);
    });

    // --- fenêtre adaptée à la taille de l'écran (évite de dépasser l'écran)
    if (QScreen *scr = QGuiApplication::primaryScreen()) {
        const QRect avail = scr->availableGeometry();
        resize(qMin(1540, avail.width() - 20), qMin(960, avail.height() - 20));
        move(avail.center() - rect().center());
    }

    // --- vérification des images intégrées (resources.qrc)
    QTimer::singleShot(0, this, [this] {
        const QStringList needed = {
            ":/fashionova-v2/images/sidebar.png", ":/fashionova-v2/images/banner.png", ":/fashionova-v2/images/logo.png",
            ":/fashionova-v2/icons/clients_off.png", ":/fashionova-v2/icons/clients_on.png"
        };
        QStringList missing;
        for (const QString &p : needed)
            if (!QFile::exists(p)) missing << p;
        if (!missing.isEmpty()) {
            QMessageBox::warning(this, "Images manquantes",
                "Les images ne sont pas intégrées au programme :\n" + missing.join("\n") +
                "\n\nVérifiez que les dossiers « images » et « icons » sont bien présents "
                "à côté de resources.qrc, puis :\nCompilation > Nettoyer tout, "
                "Exécuter qmake, puis Reconstruire.");
        }
    });

    QHeaderView *hh = ui->tableClients->horizontalHeader();
    hh->setSectionResizeMode(QHeaderView::Stretch);
    hh->setSectionResizeMode(0, QHeaderView::ResizeToContents); // ID
    hh->setSectionResizeMode(4, QHeaderView::ResizeToContents); // Date
    hh->setMinimumHeight(44);
    ui->tableClients->verticalHeader()->setDefaultSectionSize(40);

    // --- aides à la saisie (le contrôle complet reste fait par valider())
    ui->editId->setMaxLength(6);                       // CL-001
    ui->editId->setPlaceholderText("CL-001");
    ui->editTel->setMaxLength(16);                     // +216 12 345 678 (15) + marge
    ui->editTel->setPlaceholderText("+216 12 345 678");
    ui->editTel->setValidator(new QRegularExpressionValidator(
        QRegularExpression("[+0-9 ]*"), ui->editTel)); // pas de lettres ni de symboles
    ui->editAdresse->setMaxLength(200);
    ui->editAdresse->setPlaceholderText(
        QString::fromUtf8("N°, rue, quartier, ville, code postal (ex : 12 Rue de la Liberté, Tunis 1000)"));

    // --- boutons du formulaire
    connect(ui->btnAjouter,   &QPushButton::clicked, this, &FashionNovaV2Window::ajouter);
    connect(ui->btnModifier,  &QPushButton::clicked, this, &FashionNovaV2Window::modifier);
    connect(ui->btnSupprimer, &QPushButton::clicked, this, &FashionNovaV2Window::supprimer);
    connect(ui->btnReinit,    &QPushButton::clicked, this, &FashionNovaV2Window::reinitialiser);
    connect(ui->btnExport,    &QPushButton::clicked, this, &FashionNovaV2Window::exporterPdf);

    // --- recherche / tri / sélection
    connect(ui->searchListe,  &QLineEdit::textChanged, this, &FashionNovaV2Window::onFilterChanged);
    connect(ui->searchGlobal, &QLineEdit::textChanged, this, &FashionNovaV2Window::onFilterChanged);
    connect(ui->comboTrier, qOverload<int>(&QComboBox::currentIndexChanged),
            this, &FashionNovaV2Window::onFilterChanged);
    connect(ui->tableClients, &QTableWidget::itemSelectionChanged,
            this, &FashionNovaV2Window::remplirFormulaire);

    // --- barre du haut
    connect(ui->btnMenu, &QPushButton::clicked, this,
            [this] { ui->sidebar->setVisible(!ui->sidebar->isVisible()); });
    connect(ui->btnNotif, &QPushButton::clicked, this, [this] {
        QMessageBox::information(this, "Notifications", "Vous avez 3 nouvelles notifications.");
    });

    // --- bannière : l'image remplit la largeur sans déformation
    bannerPix = QPixmap(":/fashionova-v2/images/banner.png");
    ui->bannerImage->installEventFilter(this);
    updateBanner();

    // --- statistiques dynamiques (dessinées en code, calculées depuis clients + commandes)
    statDonut     = new DonutCard(ui->statsPanel);
    statEvolution = new EvolutionCard(ui->statsPanel);
    statTop       = new TopCard(ui->statsPanel);
    ui->statsLayout->addWidget(statDonut,     378);
    ui->statsLayout->addWidget(statEvolution, 417);
    ui->statsLayout->addWidget(statTop,       384);
    ui->statsPanel->installEventFilter(this);
    updateStats();

    // --- navigation : chaque bouton ouvre sa page dans le QStackedWidget
    //     0 = Clients (interface complète), 1..5 = pages blanches à intégrer
    const QList<QPushButton *> navs = {
        ui->btnNavClients, ui->btnNavEmployes, ui->btnNavCommandes,
        ui->btnNavMaquettes, ui->btnNavMachines, ui->btnNavArticles
    };
    for (int i = 0; i < navs.size(); ++i) {
        connect(navs[i], &QPushButton::clicked, this, [this, i] {
            ui->stackPages->setCurrentIndex(i);
        });
    }
    ui->stackPages->setCurrentIndex(0);
    ui->btnNavClients->setChecked(true);

    refreshTable(clients.isEmpty() ? QString() : clients.first().id);
}

FashionNovaV2Window::~FashionNovaV2Window()
{
    delete ui;
}

bool FashionNovaV2Window::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::Resize) {
        if (obj == ui->bannerImage)     updateBanner();
        else if (obj == ui->statsPanel) updateStatsLayout();
    }
    return QMainWindow::eventFilter(obj, event);
}

void FashionNovaV2Window::updateBanner()
{
    const QSize s = ui->bannerImage->size();
    if (bannerPix.isNull() || s.isEmpty()) return;
    // KeepAspectRatioByExpanding : toute la largeur, léger recadrage haut/bas
    ui->bannerImage->setPixmap(
        bannerPix.scaled(s, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation));
}

void FashionNovaV2Window::updateStatsLayout()
{
    // Les 3 cartes gardent les proportions d'origine (378 : 417 : 384, hauteur 186)
    // et s'adaptent à la largeur de la page.
    const int gap = ui->statsLayout->spacing();
    const int w = ui->statsPanel->width();
    const double k = double(w - 2 * gap) / (378 + 417 + 384);
    if (k <= 0.1) return;
    const int h = qRound(186 * k);
    if (ui->statsPanel->height() != h)
        ui->statsPanel->setFixedHeight(h);
}

void FashionNovaV2Window::updateStats()
{
    if (!statDonut) return;
    const QString annulee = QString::fromUtf8("Annulée");

    // --- 1) répartition par type de client
    int nStd = 0, nVip = 0, nPrem = 0;
    for (const Client &c : clients) {
        if (c.type == "VIP") ++nVip;
        else if (c.type == "Premium") ++nPrem;
        else ++nStd;
    }
    statDonut->setData(nStd, nVip, nPrem);

    // commandes valides = non annulées et appartenant à un client existant
    QMap<QString, int> nbCmd;       // clientId -> nombre de commandes
    QMap<QString, double> depense;  // clientId -> total dépensé
    QVector<const Commande *> valides;
    for (const Commande &cmd : commandes) {
        if (cmd.statut == annulee || indexOfId(cmd.clientId) < 0) continue;
        valides.append(&cmd);
        nbCmd[cmd.clientId] += 1;
        depense[cmd.clientId] += cmd.montant;
    }

    // --- 2) dépenses des 6 derniers mois (jusqu'au mois de la dernière commande)
    QDate last;
    for (const Commande *cmd : valides) {
        const QDate d = QDate::fromString(cmd->date, kDateFmt);
        if (d.isValid() && (!last.isValid() || d > last)) last = d;
    }
    if (!last.isValid()) last = QDate::currentDate();
    const QDate firstOfLast(last.year(), last.month(), 1);

    static const char *mois[] = { "Jan", "Fév", "Mar", "Avr", "Mai", "Juin",
                                  "Juil", "Aoû", "Sep", "Oct", "Nov", "Déc" };
    QStringList labels;
    QVector<double> values(6, 0.0);
    QVector<QDate> starts;
    for (int i = 5; i >= 0; --i) {
        const QDate m = firstOfLast.addMonths(-i);
        starts.append(m);
        labels << QString::fromUtf8(mois[m.month() - 1]);
    }
    for (const Commande *cmd : valides) {
        const QDate d = QDate::fromString(cmd->date, kDateFmt);
        for (int i = 0; i < starts.size(); ++i)
            if (d.isValid() && d.year() == starts[i].year() && d.month() == starts[i].month())
                values[i] += cmd->montant;
    }
    statEvolution->setData(labels, values);

    // --- 3) top 5 : clients avec le plus de commandes (égalité : plus gros total)
    QVector<QString> ids;
    for (auto it = nbCmd.cbegin(); it != nbCmd.cend(); ++it) ids.append(it.key());
    std::sort(ids.begin(), ids.end(), [&](const QString &a, const QString &b) {
        if (nbCmd[a] != nbCmd[b]) return nbCmd[a] > nbCmd[b];
        if (depense[a] != depense[b]) return depense[a] > depense[b];
        return a < b;
    });
    QVector<QPair<QString, int>> top;
    for (int i = 0; i < ids.size() && i < 5; ++i) {
        const Client &cl = clients[indexOfId(ids[i])];
        top.append(qMakePair(cl.prenom + " " + cl.nom, nbCmd[ids[i]]));
    }
    statTop->setData(top);

    updateStatsLayout();
}

// ------------------------------------------------------------------ utilitaires

void FashionNovaV2Window::status(const QString &text)
{
    statusBar()->showMessage(text, 4000);
}

int FashionNovaV2Window::selectedIndex() const
{
    const QModelIndexList rows = ui->tableClients->selectionModel()->selectedRows();
    return rows.isEmpty() ? -1 : rows.first().row();
}

QString FashionNovaV2Window::selectedId() const
{
    const int i = selectedIndex();
    return (i >= 0 && i < view.size()) ? view[i].id : QString();
}

int FashionNovaV2Window::indexOfId(const QString &id) const
{
    for (int i = 0; i < clients.size(); ++i)
        if (clients[i].id == id) return i;
    return -1;
}

QString FashionNovaV2Window::nextId() const
{
    // format imposé : CL-001 (« CL- » + 3 chiffres)
    static const QRegularExpression re("^CL-(\\d{3})$");
    int maxNum = 0;
    for (const Client &c : clients) {
        const QRegularExpressionMatch m = re.match(c.id);
        if (m.hasMatch()) maxNum = qMax(maxNum, m.captured(1).toInt());
    }
    if (maxNum >= 999) return QString();   // plus d'identifiant libre
    return QString("CL-%1").arg(maxNum + 1, 3, 10, QChar('0'));
}

// ------------------------------------------------------------------ tableau

QVector<Client> FashionNovaV2Window::sortedFiltered() const
{
    const QStringList terms = (ui->searchListe->text() + " " + ui->searchGlobal->text())
                                  .toLower().split(' ', Qt::SkipEmptyParts);
    QVector<Client> data;
    for (const Client &c : clients) {
        const QString hay = (c.id + " " + c.nom + " " + c.prenom + " " + c.tel + " "
                             + c.date + " " + c.type).toLower();
        bool ok = true;
        for (const QString &t : terms)
            if (!hay.contains(t)) { ok = false; break; }
        if (ok) data.append(c);
    }

    switch (ui->comboTrier->currentIndex()) {
    case 0: // date d'inscription
        std::sort(data.begin(), data.end(), [](const Client &a, const Client &b) {
            return QDate::fromString(a.date, kDateFmt) < QDate::fromString(b.date, kDateFmt);
        });
        break;
    case 1: // nom
        std::sort(data.begin(), data.end(), [](const Client &a, const Client &b) {
            return QString::localeAwareCompare(a.nom + a.prenom, b.nom + b.prenom) < 0;
        });
        break;
    default: // type de client
        std::sort(data.begin(), data.end(), [](const Client &a, const Client &b) {
            return typeRank(a.type) != typeRank(b.type) ? typeRank(a.type) < typeRank(b.type)
                                                         : a.nom < b.nom;
        });
    }
    return data;
}

void FashionNovaV2Window::refreshTable(const QString &keepId)
{
    view = sortedFiltered();

    {
        QSignalBlocker blocker(ui->tableClients);
        ui->tableClients->setRowCount(view.size());
        for (int r = 0; r < view.size(); ++r) {
            const Client &c = view[r];
            const QStringList cells = { c.id, c.nom, c.prenom, c.tel, c.date, c.type };
            for (int col = 0; col < cells.size(); ++col) {
                auto *item = new QTableWidgetItem(cells[col]);
                if (col == 5) {
                    const auto colors = statutColors(c.type);
                    item->setBackground(colors.first);
                    item->setForeground(colors.second);
                    item->setTextAlignment(Qt::AlignCenter);
                }
                ui->tableClients->setItem(r, col, item);
            }
            auto *actions = new QTableWidgetItem(QString::fromUtf8("👁   ✎"));
            actions->setTextAlignment(Qt::AlignCenter);
            ui->tableClients->setItem(r, 6, actions);
        }
    }

    bool found = false;
    for (int r = 0; r < view.size(); ++r) {
        if (view[r].id == keepId) {
            ui->tableClients->selectRow(r);
            found = true;
            break;
        }
    }
    if (!found) ui->tableClients->clearSelection();
    refreshHistorique();
    updateStats();
}

void FashionNovaV2Window::onFilterChanged()
{
    refreshTable(selectedId());
}

// ------------------------------------------------------------------ historique

void FashionNovaV2Window::refreshHistorique()
{
    QTableWidget *t = ui->tableCommandes;
    t->setRowCount(0);

    const int idx = indexOfId(selectedId());
    if (idx < 0) {
        ui->groupHistorique->setTitle(QString::fromUtf8("🧾  Historique des commandes"));
        ui->lblNbCommandes->setText("Commandes : -");
        ui->lblTotalDepense->setText(QString::fromUtf8("Total dépensé : -"));
        ui->lblDerniereCommande->setText(QString::fromUtf8("Dernière commande : -"));
        ui->lblPremierAchat->setText("Premier achat : -");
        ui->lblPanierMoyen->setText("Panier moyen : -");
        ui->lblCmdEnCours->setText(QString::fromUtf8("En cours / attente : -"));
        ui->lblPreferences->setText(QString::fromUtf8("Préférences : -"));
        return;
    }
    const Client &cl = clients[idx];
    ui->groupHistorique->setTitle(
        QString::fromUtf8("🧾  Historique des commandes — ") + cl.prenom + " " + cl.nom);

    // commandes du client (plus récentes d'abord)
    QVector<Commande> all;
    for (const Commande &cmd : commandes)
        if (cmd.clientId == cl.id) all.append(cmd);
    std::sort(all.begin(), all.end(), [](const Commande &a, const Commande &b) {
        return QDate::fromString(a.date, kDateFmt) > QDate::fromString(b.date, kDateFmt);
    });

    // résumé : toutes les commandes, total hors commandes annulées
    double total = 0.0;
    for (const Commande &cmd : all)
        if (cmd.statut != QString::fromUtf8("Annulée")) total += cmd.montant;
    const QLocale fr(QLocale::French);
    ui->lblNbCommandes->setText(QString("Commandes : %1").arg(all.size()));
    ui->lblTotalDepense->setText(QString::fromUtf8("Total dépensé : ") + fr.toString(total, 'f', 2) + " DT");
    ui->lblDerniereCommande->setText(QString::fromUtf8("Dernière commande : ")
                                     + (all.isEmpty() ? QString("-") : all.first().date));

    // --- infos complémentaires (hors commandes annulées = vrais achats)
    const QString sAnnulee = QString::fromUtf8("Annulée");
    QDate premier;
    int nbAchats = 0, nbOuvertes = 0;
    QMap<QString, int> parCat;       // catégorie -> nombre de commandes
    QMap<QString, double> parCatMt;  // catégorie -> montant (départage les égalités)
    for (const Commande &cmd : all) {
        if (cmd.statut == sAnnulee) continue;
        ++nbAchats;
        const QDate d = QDate::fromString(cmd.date, kDateFmt);
        if (d.isValid() && (!premier.isValid() || d < premier)) premier = d;
        if (cmd.statut == "En cours" || cmd.statut == "En attente") ++nbOuvertes;
        const QString cat = categorieCommande(cmd.designation);
        if (cat != QString::fromUtf8("Retouches")) {
            parCat[cat] += 1;
            parCatMt[cat] += cmd.montant;
        }
    }
    ui->lblPremierAchat->setText("Premier achat : "
                                 + (premier.isValid() ? premier.toString(kDateFmt) : QString("-")));
    ui->lblPanierMoyen->setText("Panier moyen : "
                                + (nbAchats ? fr.toString(total / nbAchats, 'f', 2) + " DT" : QString("-")));
    ui->lblCmdEnCours->setText(QString::fromUtf8("En cours / attente : ") + QString::number(nbOuvertes));

    QStringList cats = parCat.keys();
    std::sort(cats.begin(), cats.end(), [&](const QString &a, const QString &b) {
        if (parCat[a] != parCat[b]) return parCat[a] > parCat[b];
        return parCatMt[a] > parCatMt[b];
    });
    QStringList prefs;
    for (int i = 0; i < cats.size() && i < 2; ++i)
        prefs << QString("%1 (%2)").arg(cats[i]).arg(parCat[cats[i]]);
    ui->lblPreferences->setText(QString::fromUtf8("Préférences : ")
                                + (prefs.isEmpty() ? QString("-") : prefs.join(" · ")));

    // tableau (filtré par statut)
    const bool filtre = ui->comboStatutCmd->currentIndex() > 0;
    const QString statut = ui->comboStatutCmd->currentText();
    for (const Commande &cmd : all) {
        if (filtre && cmd.statut != statut) continue;
        const int r = t->rowCount();
        t->insertRow(r);
        t->setItem(r, 0, new QTableWidgetItem(cmd.numero));
        t->setItem(r, 1, new QTableWidgetItem(cmd.date));
        t->setItem(r, 2, new QTableWidgetItem(cmd.designation));
        auto *m = new QTableWidgetItem(fr.toString(cmd.montant, 'f', 2) + " DT");
        m->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        t->setItem(r, 3, m);
        auto *st = new QTableWidgetItem(cmd.statut);
        const auto colors = cmdColors(cmd.statut);
        st->setBackground(colors.first);
        st->setForeground(colors.second);
        st->setTextAlignment(Qt::AlignCenter);
        t->setItem(r, 4, st);
    }
}

// ------------------------------------------------------------------ formulaire

Client FashionNovaV2Window::formData() const
{
    Client c;
    // simplified() : supprime les espaces inutiles (début, fin, doublons)
    c.id        = ui->editId->text().trimmed().toUpper();
    c.nom       = ui->editNom->text().simplified();
    c.prenom    = ui->editPrenom->text().simplified();
    c.tel       = Client::normaliserTel(ui->editTel->text());   // +216 12 345 678
    c.email     = ui->editEmail->text().trimmed();
    c.adresse   = Client::normaliserAdresse(ui->editAdresse->text());
    c.type      = ui->comboType->currentText();
    c.date      = ui->dateInscription->date().toString(kDateFmt);
    c.remarques = ui->editRemarques->toPlainText().trimmed();
    return c;
}

void FashionNovaV2Window::remplirFormulaire()
{
    const int i = selectedIndex();
    if (i < 0 || i >= view.size()) { refreshHistorique(); return; }
    const Client &c = view[i];
    ui->editId->setText(c.id);
    ui->editNom->setText(c.nom);
    ui->editPrenom->setText(c.prenom);
    ui->editTel->setText(c.tel);
    ui->editEmail->setText(c.email);
    ui->editAdresse->setText(c.adresse);
    ui->comboType->setCurrentText(c.type);
    ui->dateInscription->setDate(QDate::fromString(c.date, kDateFmt));
    ui->editRemarques->setPlainText(c.remarques);
    refreshHistorique();
}

bool FashionNovaV2Window::valider(const Client &c, const QString &ancienId)
{
    // petit utilitaire : message d'erreur + focus sur le champ fautif
    auto erreur = [this](const QString &titre, const QString &msg, QWidget *champ) {
        QMessageBox::warning(this, titre, msg);
        if (champ) {
            champ->setFocus();
            if (auto *le = qobject_cast<QLineEdit *>(champ)) le->selectAll();
        }
        return false;
    };

    // --- 1) champs obligatoires : on liste précisément tous les champs manquants
    //     (coordonnées comprises : téléphone, email, adresse)
    struct Obligatoire { QString libelle; bool vide; QWidget *champ; };
    const QVector<Obligatoire> obligatoires = {
        { QString::fromUtf8("ID Client"),  c.id.isEmpty(),      ui->editId },
        { QString::fromUtf8("Nom"),        c.nom.isEmpty(),     ui->editNom },
        { QString::fromUtf8("Prénom"),     c.prenom.isEmpty(),  ui->editPrenom },
        { QString::fromUtf8("Téléphone"),  ui->editTel->text().trimmed().isEmpty(), ui->editTel },
        { QString::fromUtf8("Email"),      c.email.isEmpty(),   ui->editEmail },
        { QString::fromUtf8("Adresse"),    c.adresse.isEmpty(), ui->editAdresse },
    };
    QStringList manquants;
    QWidget *premier = nullptr;
    for (const Obligatoire &o : obligatoires) {
        if (!o.vide) continue;
        manquants << QString::fromUtf8("  • ") + o.libelle;
        if (!premier) premier = o.champ;
    }
    if (!manquants.isEmpty()) {
        const QString msg = manquants.size() == 1
            ? QString::fromUtf8("Le champ obligatoire suivant n'est pas renseigné :\n\n")
                  + manquants.join("\n")
                  + QString::fromUtf8("\n\nVeuillez le compléter avant de continuer.")
            : QString::fromUtf8("Les champs obligatoires suivants ne sont pas renseignés :\n\n")
                  + manquants.join("\n")
                  + QString::fromUtf8("\n\nVeuillez les compléter avant de continuer.");
        return erreur(QString::fromUtf8("Champ(s) manquant(s)"), msg, premier);
    }

    // --- 2) identifiant : format CL-001
    const QString eId = Client::erreurId(c.id);
    if (!eId.isEmpty())
        return erreur(QString::fromUtf8("Identifiant invalide"), eId, ui->editId);

    // --- 3) nom / prénom : lettres uniquement (accents autorisés), espaces, tiret, apostrophe
    static const QRegularExpression reNom(
        QString::fromUtf8("^\\p{L}+([ '\\-’]\\p{L}+)*$"));

    if (!reNom.match(c.nom).hasMatch())
        return erreur(QString::fromUtf8("Nom invalide"),
                      QString::fromUtf8("Le nom ne doit contenir que des lettres (pas de chiffres ni de symboles)."),
                      ui->editNom);
    if (c.nom.size() < 2)
        return erreur(QString::fromUtf8("Nom invalide"),
                      QString::fromUtf8("Le nom doit contenir au moins 2 lettres."), ui->editNom);
    if (c.nom.size() > 50)
        return erreur(QString::fromUtf8("Nom invalide"),
                      QString::fromUtf8("Le nom ne doit pas dépasser 50 caractères."), ui->editNom);

    if (!reNom.match(c.prenom).hasMatch())
        return erreur(QString::fromUtf8("Prénom invalide"),
                      QString::fromUtf8("Le prénom ne doit contenir que des lettres (pas de chiffres ni de symboles)."),
                      ui->editPrenom);
    if (c.prenom.size() < 2)
        return erreur(QString::fromUtf8("Prénom invalide"),
                      QString::fromUtf8("Le prénom doit contenir au moins 2 lettres."), ui->editPrenom);
    if (c.prenom.size() > 50)
        return erreur(QString::fromUtf8("Prénom invalide"),
                      QString::fromUtf8("Le prénom ne doit pas dépasser 50 caractères."), ui->editPrenom);

    // --- 4) téléphone tunisien : +216 suivi de 8 chiffres
    //     (on contrôle la saisie brute : formData() ne reformate que les numéros valides)
    const QString eTel = Client::erreurTel(ui->editTel->text());
    if (!eTel.isEmpty())
        return erreur(QString::fromUtf8("Téléphone invalide"), eTel, ui->editTel);

    // --- 5) email (obligatoire : coordonnée du client) : doit être valide
    static const QRegularExpression reMail(
        "^[A-Za-z0-9._%+\\-]+@[A-Za-z0-9.\\-]+\\.[A-Za-z]{2,}$");
    if (!reMail.match(c.email).hasMatch())
        return erreur(QString::fromUtf8("Email invalide"),
                      QString::fromUtf8("L'adresse email n'est pas valide (exemple : nom@domaine.com)."),
                      ui->editEmail);
    if (c.email.size() > 100)
        return erreur(QString::fromUtf8("Email invalide"),
                      QString::fromUtf8("L'adresse email ne doit pas dépasser 100 caractères."), ui->editEmail);

    // --- 6) adresse
    const QString eAdr = Client::erreurAdresse(c.adresse);
    if (!eAdr.isEmpty())
        return erreur(QString::fromUtf8("Adresse invalide"), eAdr, ui->editAdresse);

    // --- 7) unicité de l'identifiant (mémoire + base Oracle)
    //     ancienId vide = ajout ; sinon l'ID d'origine est exclu de la recherche.
    bool sqlOk = true;
    const bool dejaEnMemoire = (indexOfId(c.id) >= 0) && c.id != ancienId;
    const bool dejaEnBase = Client::idExiste(c.id, ancienId, &sqlOk);
    if (!sqlOk) {
        QMessageBox::critical(this, QString::fromUtf8("Base de données"),
            QString::fromUtf8("Impossible de vérifier l'unicité de l'identifiant.\n\nDétail : ")
                + Client::derniereErreur());
        return false;
    }
    if (dejaEnMemoire || dejaEnBase) {
        const QString prochain = nextId();
        return erreur(QString::fromUtf8("Identifiant déjà utilisé"),
            QString::fromUtf8("L'identifiant %1 est déjà attribué à un autre client.").arg(c.id)
            + (prochain.isEmpty() ? QString()
               : QString::fromUtf8("\n\nPremier identifiant disponible : %1.\n"
                                   "Le bouton « Réinitialiser » le propose automatiquement.").arg(prochain)),
            ui->editId);
    }
    return true;
}

// Boîte de dialogue de confirmation : tableau « Ancienne valeur / Nouvelle valeur »
// limité aux champs réellement modifiés. Renvoie true si l'utilisateur confirme.
bool FashionNovaV2Window::confirmerModification(const Client &ancien, const Client &nouveau)
{
    struct Ligne { QString champ, avant, apres; };
    QVector<Ligne> diff;
    auto cmp = [&diff](const QString &champ, const QString &a, const QString &b) {
        if (a != b) diff.append(Ligne{ champ, a, b });
    };
    cmp(QString::fromUtf8("ID Client"),         ancien.id,        nouveau.id);
    cmp(QString::fromUtf8("Nom"),               ancien.nom,       nouveau.nom);
    cmp(QString::fromUtf8("Prénom"),            ancien.prenom,    nouveau.prenom);
    cmp(QString::fromUtf8("Téléphone"),         ancien.tel,       nouveau.tel);
    cmp(QString::fromUtf8("Email"),             ancien.email,     nouveau.email);
    cmp(QString::fromUtf8("Adresse"),           ancien.adresse,   nouveau.adresse);
    cmp(QString::fromUtf8("Type de client"),    ancien.type,      nouveau.type);
    cmp(QString::fromUtf8("Date d'inscription"), ancien.date,     nouveau.date);
    cmp(QString::fromUtf8("Remarques"),         ancien.remarques, nouveau.remarques);

    auto cellule = [](const QString &v) {
        return v.isEmpty() ? QString::fromUtf8("<i style='color:#888'>(vide)</i>")
                           : v.toHtmlEscaped().replace("\n", "<br>");
    };

    QString html = QString::fromUtf8(
        "<p>Vous êtes sur le point de modifier le client <b>%1 %2</b> (%3).<br>"
        "Voici les modifications qui seront enregistrées :</p>"
        "<table border='1' cellspacing='0' cellpadding='6'>"
        "<tr style='background:#efe3cf'><th align='left'>Champ</th>"
        "<th align='left'>Ancienne valeur</th><th align='left'>Nouvelle valeur</th></tr>")
        .arg(ancien.prenom.toHtmlEscaped(), ancien.nom.toHtmlEscaped(), ancien.id.toHtmlEscaped());
    for (const Ligne &l : diff)
        html += "<tr><td><b>" + l.champ + "</b></td><td>" + cellule(l.avant)
              + "</td><td>" + cellule(l.apres) + "</td></tr>";
    html += "</table>";
    if (ancien.id != nouveau.id)
        html += QString::fromUtf8("<p style='color:#B03A3A'><b>Attention :</b> l'identifiant du client "
                                  "va changer de %1 à %2.</p>")
                    .arg(ancien.id.toHtmlEscaped(), nouveau.id.toHtmlEscaped());
    html += QString::fromUtf8("<p>Confirmer la modification ?</p>");

    QMessageBox box(this);
    box.setIcon(QMessageBox::Question);
    box.setWindowTitle(QString::fromUtf8("Confirmation de la modification"));
    box.setTextFormat(Qt::RichText);
    box.setText(html);
    QPushButton *btnConfirmer = box.addButton(QString::fromUtf8("Confirmer"), QMessageBox::AcceptRole);
    QPushButton *btnAnnuler   = box.addButton(QString::fromUtf8("Annuler"),   QMessageBox::RejectRole);
    box.setDefaultButton(btnAnnuler);
    box.setEscapeButton(btnAnnuler);
    box.exec();
    return box.clickedButton() == btnConfirmer;
}

// ------------------------------------------------------------------ actions

void FashionNovaV2Window::reinitialiser()
{
    ui->tableClients->clearSelection();
    ui->editId->setText(nextId());
    ui->editNom->clear();
    ui->editPrenom->clear();
    ui->editTel->clear();
    ui->editEmail->clear();
    ui->editAdresse->clear();
    ui->comboType->setCurrentText("Standard");
    ui->dateInscription->setDate(QDate::currentDate());
    ui->editRemarques->clear();
    ui->editNom->setFocus();
}

void FashionNovaV2Window::ajouter()
{
    Client c = formData();
    // aucune opération si une règle n'est pas respectée (champs, formats, doublon d'ID)
    if (!valider(c)) return;

    if (!c.ajouter()) {
        QMessageBox::critical(this, QString::fromUtf8("Base de données"),
            QString::fromUtf8("L'ajout du client a échoué. Aucune donnée n'a été enregistrée.\n\nDétail : ")
                + Client::derniereErreur());
        return;
    }
    clients = Client::lister();
    refreshTable(c.id);
    status("Client " + c.id + QString::fromUtf8(" ajouté."));
    QMessageBox::information(this, QString::fromUtf8("Ajout réussi"),
        QString::fromUtf8("Le client %1 %2 (%3) a été ajouté avec succès.")
            .arg(c.prenom, c.nom, c.id));
}

void FashionNovaV2Window::modifier()
{
    const QString oldId = selectedId();
    const int idx = indexOfId(oldId);
    if (oldId.isEmpty() || idx < 0) {
        QMessageBox::warning(this, QString::fromUtf8("Aucune sélection"),
            QString::fromUtf8("Sélectionnez d'abord un client dans la liste, puis modifiez les "
                              "informations souhaitées dans le formulaire."));
        return;
    }
    const Client ancien = clients[idx];   // valeurs actuelles (issues d'Oracle)

    Client c = formData();
    if (!valider(c, oldId)) return;

    // seules les informations réellement changées comptent
    if (c.id == ancien.id && c.nom == ancien.nom && c.prenom == ancien.prenom
        && c.tel == ancien.tel && c.email == ancien.email && c.adresse == ancien.adresse
        && c.type == ancien.type && c.date == ancien.date && c.remarques == ancien.remarques) {
        QMessageBox::information(this, QString::fromUtf8("Aucune modification"),
            QString::fromUtf8("Aucune information n'a été modifiée pour le client %1.").arg(ancien.id));
        return;
    }

    // confirmation : ancienne valeur / nouvelle valeur
    if (!confirmerModification(ancien, c)) {
        remplirFormulaire();              // annulation : on réaffiche les données d'origine
        status(QString::fromUtf8("Modification annulée : aucune donnée n'a été changée."));
        return;
    }

    if (!c.modifier(oldId)) {
        QMessageBox::critical(this, QString::fromUtf8("Base de données"),
            QString::fromUtf8("La modification a échoué. Aucune donnée n'a été changée.\n\nDétail : ")
                + Client::derniereErreur());
        return;
    }
    clients = Client::lister();
    refreshTable(c.id);                   // actualise le tableau et ré-affiche le client modifié
    status("Client " + c.id + QString::fromUtf8(" modifié."));
    QMessageBox::information(this, QString::fromUtf8("Modification réussie"),
        QString::fromUtf8("Les informations du client %1 %2 (%3) ont été mises à jour avec succès.")
            .arg(c.prenom, c.nom, c.id));
}

void FashionNovaV2Window::supprimer()
{
    const QString id = selectedId();
    const int idx = indexOfId(id);
    if (idx < 0) {
        QMessageBox::warning(this, QString::fromUtf8("Aucune sélection"),
            QString::fromUtf8("Sélectionnez d'abord un client dans la liste."));
        return;
    }
    const Client c = clients[idx];
    QMessageBox box(this);
    box.setIcon(QMessageBox::Warning);
    box.setWindowTitle(QString::fromUtf8("Confirmation de la suppression"));
    box.setText(QString::fromUtf8("Voulez-vous vraiment supprimer le client %1 %2 (%3) ?")
                    .arg(c.prenom, c.nom, c.id));
    box.setInformativeText(QString::fromUtf8("Cette action est définitive."));
    QPushButton *btnSupprimer = box.addButton(QString::fromUtf8("Supprimer"), QMessageBox::DestructiveRole);
    QPushButton *btnAnnuler   = box.addButton(QString::fromUtf8("Annuler"),   QMessageBox::RejectRole);
    box.setDefaultButton(btnAnnuler);
    box.setEscapeButton(btnAnnuler);
    box.exec();
    if (box.clickedButton() != btnSupprimer) return;

    if (!Client::supprimer(id)) {
        QMessageBox::critical(this, QString::fromUtf8("Base de données"),
            QString::fromUtf8("La suppression a échoué. Aucune donnée n'a été supprimée.\n\nDétail : ")
                + Client::derniereErreur());
        return;
    }
    clients = Client::lister();
    refreshTable(QString());
    status(QString::fromUtf8("Client %1 supprimé.").arg(id));
    QMessageBox::information(this, QString::fromUtf8("Suppression réussie"),
        QString::fromUtf8("Le client %1 %2 (%3) a été supprimé.").arg(c.prenom, c.nom, c.id));
}

void FashionNovaV2Window::exporterPdf()
{
    const QString path = QFileDialog::getSaveFileName(
        this, "Exporter en PDF", "liste_clients.pdf", "PDF (*.pdf)");
    if (path.isEmpty()) return;

    QString rows;
    for (const Client &c : view)
        rows += "<tr><td>" + c.id.toHtmlEscaped() + "</td><td>" + c.nom.toHtmlEscaped()
              + "</td><td>" + c.prenom.toHtmlEscaped() + "</td><td>" + c.tel.toHtmlEscaped()
              + "</td><td>" + c.date + "</td><td>" + c.type + "</td></tr>";

    const QString html =
        "<h2 style='color:#0b2a4a'>FashioNova - Liste des clients</h2>"
        "<table border='1' cellspacing='0' cellpadding='6' width='100%'>"
        "<tr style='background:#efe3cf'><th>ID</th><th>Nom</th><th>Prénom</th>"
        "<th>Téléphone</th><th>Date d'inscription</th><th>Statut</th></tr>" + rows + "</table>";

    QPdfWriter writer(path);
    QTextDocument doc;
    doc.setHtml(html);
    doc.print(&writer);
    status("PDF exporté : " + path);
}

QWidget *FashionNovaV2Window::takeClientsPage()
{
    QWidget *page = ui->pageClients;
    ui->stackPages->removeWidget(page);
    page->setParent(nullptr);
    return page;
}

void FashionNovaV2Window::reloadClients()
{
    clients = Client::lister();
    refreshTable(clients.isEmpty() ? QString() : clients.first().id);
    updateStats();
}
