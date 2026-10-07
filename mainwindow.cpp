#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "articlespage.h"

#include <QColor>
#include <QDate>
#include <QFile>
#include <QGuiApplication>
#include <QLocale>
#include <QScreen>
#include <QTimer>
#include <QEvent>
#include <QFileDialog>
#include <QHeaderView>
#include <QMessageBox>
#include <QPair>
#include <QPdfWriter>
#include <QRegularExpression>
#include <QSignalBlocker>
#include <QStatusBar>
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

} // namespace

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    auto S = [](const char *s) { return QString::fromUtf8(s); };
    clients = {
        { S("CL001"), S("Ben Ali"),  S("Sofia"),   S("+216 98 765 432"), S("sofia.benali@email.com"),
          S("12 Rue de la Liberté, Tunis"), S("VIP"), S("15/03/2024"),
          S("Cliente fidèle, préfère les robes de soirée et les créations sur mesure.") },
        { S("CL002"), S("Trabelsi"), S("Amine"),   S("+216 92 123 456"), "", "", S("Standard"), S("22/04/2024"), "" },
        { S("CL003"), S("Hmidi"),    S("Salma"),   S("+216 95 987 654"), "", "", S("VIP"),      S("10/05/2024"), "" },
        { S("CL004"), S("Saidi"),    S("Yassine"), S("+216 93 456 789"), "", "", S("Premium"),  S("18/06/2024"), "" },
        { S("CL005"), S("Mansouri"), S("Rania"),   S("+216 22 334 455"), "", "", S("Standard"), S("27/07/2024"), "" },
        { S("CL006"), S("Mejri"),    S("Lilia"),   S("+216 95 667 788"), "", "", S("Premium"),  S("03/08/2024"), "" },
    };

    // --- historique des commandes (données d'exemple)
    const QString livree  = S("Livrée");
    const QString annulee = S("Annulée");
    commandes = {
        { "CMD-1042", "CL001", "12/09/2024", S("Robe de soirée sur mesure (satin bleu nuit)"), "En cours", 1850.0 },
        { "CMD-1019", "CL001", "02/07/2024", S("Tailleur : veston + pantalon"), livree, 980.0 },
        { "CMD-0987", "CL001", "21/05/2024", S("Retouches robe de cocktail"), livree, 120.0 },
        { "CMD-0954", "CL001", "30/03/2024", S("Robe de mariée (essayage)"), annulee, 2400.0 },
        { "CMD-1051", "CL002", "20/09/2024", S("Chemise sur mesure"), "En attente", 180.0 },
        { "CMD-1030", "CL002", "15/08/2024", S("Costume 2 pièces"), livree, 1200.0 },
        { "CMD-1038", "CL003", "03/09/2024", S("Robe de soirée"), "En cours", 1450.0 },
        { "CMD-1025", "CL003", "28/07/2024", S("Abaya brodée"), livree, 650.0 },
        { "CMD-1002", "CL003", "12/06/2024", S("Jupe plissée"), livree, 240.0 },
        { "CMD-1044", "CL004", "14/09/2024", S("Costume de mariage"), "En cours", 2100.0 },
        { "CMD-1033", "CL005", "25/08/2024", S("Ensemble casual"), livree, 520.0 },
        { "CMD-1047", "CL006", "16/09/2024", S("Retouches robe"), "En attente", 90.0 },
        { "CMD-1011", "CL006", "12/08/2024", S("Robe cocktail"), livree, 780.0 },
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
            ":/images/sidebar.png", ":/images/banner.png", ":/images/logo.png",
            ":/images/stats_repartition.png",
            ":/images/stats_evolution.png", ":/images/stats_top5.png",
            ":/icons/clients_off.png", ":/icons/clients_on.png"
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

    // --- boutons du formulaire
    connect(ui->btnAjouter,   &QPushButton::clicked, this, &MainWindow::ajouter);
    connect(ui->btnModifier,  &QPushButton::clicked, this, &MainWindow::modifier);
    connect(ui->btnSupprimer, &QPushButton::clicked, this, &MainWindow::supprimer);
    connect(ui->btnAfficher,  &QPushButton::clicked, this, &MainWindow::afficher);
    connect(ui->btnReinit,    &QPushButton::clicked, this, &MainWindow::reinitialiser);
    connect(ui->btnExport,    &QPushButton::clicked, this, &MainWindow::exporterPdf);

    // --- recherche / tri / sélection
    connect(ui->searchListe,  &QLineEdit::textChanged, this, &MainWindow::onFilterChanged);
    connect(ui->searchGlobal, &QLineEdit::textChanged, this, &MainWindow::onFilterChanged);
    connect(ui->comboTrier, qOverload<int>(&QComboBox::currentIndexChanged),
            this, &MainWindow::onFilterChanged);
    connect(ui->tableClients, &QTableWidget::itemSelectionChanged,
            this, &MainWindow::remplirFormulaire);
    connect(ui->tableClients, &QTableWidget::cellDoubleClicked,
            this, [this](int, int) { afficher(); });

    // --- barre du haut
    connect(ui->btnMenu, &QPushButton::clicked, this,
            [this] { ui->sidebar->setVisible(!ui->sidebar->isVisible()); });
    connect(ui->btnNotif, &QPushButton::clicked, this, [this] {
        QMessageBox::information(this, "Notifications", "Vous avez 3 nouvelles notifications.");
    });

    // --- bannière : l'image remplit la largeur sans déformation
    bannerPix = QPixmap(":/images/banner.png");
    ui->bannerImage->installEventFilter(this);
    updateBanner();

    // --- statistiques : image qui s'adapte à la largeur (sans déformation)
    pixRepartition = QPixmap(":/images/stats_repartition.png");
    pixEvolution  = QPixmap(":/images/stats_evolution.png");
    pixTop5       = QPixmap(":/images/stats_top5.png");
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
            ui->searchGlobal->clear();          // la recherche globale est propre à chaque module
            ui->stackPages->setCurrentIndex(i);
        });
    }
    ui->stackPages->setCurrentIndex(0);
    ui->btnNavClients->setChecked(true);

    // --- module Articles (articlespage.h) : page 5 du QStackedWidget
    //     la recherche de la barre du haut est transmise au module affiché
    connect(ui->searchGlobal, &QLineEdit::textChanged, this, [this](const QString &t) {
        if (ui->stackPages->currentWidget() == ui->pageArticles)
            ui->pageArticles->setSearchText(t);
    });
    connect(ui->pageArticles, &ArticlesPage::statusMessage, this,
            [this](const QString &text, int ms) { statusBar()->showMessage(text, ms); });

    refreshTable(QStringLiteral("CL001"));
}

MainWindow::~MainWindow()
{
    delete ui;
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::Resize) {
        if (obj == ui->bannerImage)     updateBanner();
        else if (obj == ui->statsPanel) updateStats();
    }
    return QMainWindow::eventFilter(obj, event);
}

void MainWindow::updateBanner()
{
    const QSize s = ui->bannerImage->size();
    if (bannerPix.isNull() || s.isEmpty()) return;
    // KeepAspectRatioByExpanding : toute la largeur, léger recadrage haut/bas
    ui->bannerImage->setPixmap(
        bannerPix.scaled(s, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation));
}

void MainWindow::updateStats()
{
    // Les 3 photos (déclarées dans mainwindow.ui) gardent les proportions d'origine
    // (378 : 417 : 384 px, hauteur 186 px) et s'adaptent à la largeur de la page.
    const int gap = ui->statsLayout->spacing();
    const int w = ui->statsPanel->width();
    const double k = double(w - 2 * gap) / (378 + 417 + 384);
    if (k <= 0.1) return;
    const int h = qRound(186 * k);
    if (ui->statsPanel->height() != h)
        ui->statsPanel->setFixedHeight(h);

    auto place = [k, h](QLabel *lab, const QPixmap &pix, int nativeW) {
        const QSize sz(qRound(nativeW * k), h);
        lab->setFixedSize(sz);
        lab->setPixmap(pix.scaled(sz, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    };
    place(ui->statsRepartition, pixRepartition, 378);
    place(ui->statsEvolution,   pixEvolution,   417);
    place(ui->statsTop5,        pixTop5,        384);
}

// ------------------------------------------------------------------ utilitaires

void MainWindow::status(const QString &text)
{
    statusBar()->showMessage(text, 4000);
}

int MainWindow::selectedIndex() const
{
    const QModelIndexList rows = ui->tableClients->selectionModel()->selectedRows();
    return rows.isEmpty() ? -1 : rows.first().row();
}

QString MainWindow::selectedId() const
{
    const int i = selectedIndex();
    return (i >= 0 && i < view.size()) ? view[i].id : QString();
}

int MainWindow::indexOfId(const QString &id) const
{
    for (int i = 0; i < clients.size(); ++i)
        if (clients[i].id == id) return i;
    return -1;
}

QString MainWindow::nextId() const
{
    static const QRegularExpression re("^CL(\\d+)$");
    int maxNum = 0;
    for (const Client &c : clients) {
        const QRegularExpressionMatch m = re.match(c.id);
        if (m.hasMatch()) maxNum = qMax(maxNum, m.captured(1).toInt());
    }
    return QString("CL%1").arg(maxNum + 1, 3, 10, QChar('0'));
}

// ------------------------------------------------------------------ tableau

QVector<Client> MainWindow::sortedFiltered() const
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

void MainWindow::refreshTable(const QString &keepId)
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
}

void MainWindow::onFilterChanged()
{
    refreshTable(selectedId());
}

// ------------------------------------------------------------------ historique

void MainWindow::refreshHistorique()
{
    QTableWidget *t = ui->tableCommandes;
    t->setRowCount(0);

    const int idx = indexOfId(selectedId());
    if (idx < 0) {
        ui->groupHistorique->setTitle(QString::fromUtf8("🧾  Historique des commandes"));
        ui->lblNbCommandes->setText("Commandes : -");
        ui->lblTotalDepense->setText(QString::fromUtf8("Total dépensé : -"));
        ui->lblDerniereCommande->setText(QString::fromUtf8("Dernière commande : -"));
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

Client MainWindow::formData() const
{
    Client c;
    c.id        = ui->editId->text().trimmed();
    c.nom       = ui->editNom->text().trimmed();
    c.prenom    = ui->editPrenom->text().trimmed();
    c.tel       = ui->editTel->text().trimmed();
    c.email     = ui->editEmail->text().trimmed();
    c.adresse   = ui->editAdresse->text().trimmed();
    c.type      = ui->comboType->currentText();
    c.date      = ui->dateInscription->date().toString(kDateFmt);
    c.remarques = ui->editRemarques->toPlainText().trimmed();
    return c;
}

void MainWindow::remplirFormulaire()
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

bool MainWindow::valider(const Client &c)
{
    if (c.nom.isEmpty() || c.prenom.isEmpty()) {
        QMessageBox::warning(this, "Champs manquants", "Le nom et le prénom sont obligatoires.");
        return false;
    }
    if (c.tel.isEmpty()) {
        QMessageBox::warning(this, "Champs manquants", "Le numéro de téléphone est obligatoire.");
        return false;
    }
    return true;
}

// ------------------------------------------------------------------ actions

void MainWindow::reinitialiser()
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

void MainWindow::ajouter()
{
    Client c = formData();
    if (!valider(c)) return;
    if (c.id.isEmpty() || indexOfId(c.id) >= 0)
        c.id = nextId();
    clients.append(c);
    refreshTable(c.id);
    status("Client " + c.id + " ajouté.");
}

void MainWindow::modifier()
{
    const QString oldId = selectedId();
    if (oldId.isEmpty()) {
        QMessageBox::warning(this, "Aucune sélection", "Sélectionnez d'abord un client dans la liste.");
        return;
    }
    Client c = formData();
    if (!valider(c)) return;
    if (c.id != oldId && indexOfId(c.id) >= 0) {
        QMessageBox::warning(this, "ID déjà utilisé", "L'ID " + c.id + " existe déjà.");
        return;
    }
    const int idx = indexOfId(oldId);
    if (idx >= 0) clients[idx] = c;
    refreshTable(c.id);
    status("Client " + c.id + " modifié.");
}

void MainWindow::supprimer()
{
    const QString id = selectedId();
    const int idx = indexOfId(id);
    if (idx < 0) {
        QMessageBox::warning(this, "Aucune sélection", "Sélectionnez d'abord un client dans la liste.");
        return;
    }
    const Client &c = clients[idx];
    const auto rep = QMessageBox::question(
        this, "Confirmation",
        QString("Supprimer %1 %2 (%3) ?").arg(c.prenom, c.nom, c.id),
        QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    if (rep != QMessageBox::Yes) return;
    clients.remove(idx);
    refreshTable(QString());
    status("Client supprimé.");
}

void MainWindow::afficher()
{
    const int idx = indexOfId(selectedId());
    if (idx < 0) {
        QMessageBox::warning(this, "Aucune sélection", "Sélectionnez d'abord un client dans la liste.");
        return;
    }
    const Client &c = clients[idx];
    const QString dash = "-";
    QMessageBox::information(
        this, "Client " + c.id,
        QString("%1 %2\n\nTéléphone : %3\nEmail : %4\nAdresse : %5\nType : %6\n"
                "Inscrit le : %7\n\nRemarques : %8")
            .arg(c.prenom, c.nom, c.tel,
                 c.email.isEmpty() ? dash : c.email,
                 c.adresse.isEmpty() ? dash : c.adresse,
                 c.type, c.date,
                 c.remarques.isEmpty() ? dash : c.remarques));
}

void MainWindow::exporterPdf()
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
