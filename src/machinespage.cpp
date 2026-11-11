#include "machinespage.h"
#include "ui_machinespage.h"

#include <algorithm>
#include <utility>

#include <QAction>
#include <QComboBox>
#include <QDateEdit>
#include <QDesktopServices>
#include <QDoubleSpinBox>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QLocale>
#include <QMap>
#include <QMarginsF>
#include <QMenu>
#include <QMessageBox>
#include <QPageSize>
#include <QPdfWriter>
#include <QPushButton>
#include <QRegularExpressionValidator>
#include <QSignalBlocker>
#include <QSpinBox>
#include <QStyle>
#include <QTableWidget>
#include <QTextDocument>
#include <QTimer>
#include <QToolButton>
#include <QUrl>

#include "chartwidgets.h"
#include "database.h"
#include "machinevalidation.h"
#include "metiers.h"
#include "recorddialogs.h"
#include "uihelpers.h"

static const int kPageSize = 5;

static const bool kCrudActif = true;

static void repolish(QWidget *w)
{
    w->style()->unpolish(w);
    w->style()->polish(w);
    w->update();
}

static void clearLayout(QLayout *l)
{
    while (QLayoutItem *item = l->takeAt(0)) {
        if (QWidget *w = item->widget()) w->deleteLater();
        delete item;
    }
}

/* ============================ construction ============================ */

MachinesPage::MachinesPage(QWidget *parent)
    : QWidget(parent), ui(new Ui::MachinesPage)
{
    ui->setupUi(this);          // construit toute l'interface décrite dans machinespage.ui
    ui->lineNom->setMaxLength(200);
    ui->lineNom->setValidator(new QRegularExpressionValidator(
        MachineValidation::namePattern(), ui->lineNom));
    ui->lineNom->setToolTip(QStringLiteral("Lettres et chiffres acceptés, avec au moins une lettre. Exemple : Singer 200."));
    ui->dateMaintenance->setKeyboardTracking(false);
    m_pager = ui->pagerLayout;

    setupIcons();
    setupTable();
    connectSignals();

    reloadData();
    clearForm();

    // Re-vérification périodique (une machine peut devenir « en retard » sans action de l'utilisateur)
    auto *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MachinesPage::reloadData);
    timer->start(60 * 1000);
}

MachinesPage::~MachinesPage()
{
    delete ui;
}

// Les icônes sont des SVG recolorés à l'exécution : elles sont donc affectées ici et non dans le .ui.
void MachinesPage::setupIcons()
{
    const QColor ink("#2c3240");
    // cartes
    ui->iconCardForm->setPixmap(UI::svgPixmap(QStringLiteral("gear"),  Qt::white, 18));
    ui->iconCardList->setPixmap(UI::svgPixmap(QStringLiteral("clip"),  Qt::white, 18));
    ui->iconCardStats->setPixmap(UI::svgPixmap(QStringLiteral("chart"), Qt::white, 18));

    // champs du formulaire
    ui->iconId->setPixmap(UI::svgPixmap(QStringLiteral("idcard"),   ink, 18));
    ui->iconNom->setPixmap(UI::svgPixmap(QStringLiteral("gear"),    ink, 18));
    ui->iconType->setPixmap(UI::svgPixmap(QStringLiteral("box"),    ink, 18));
    ui->iconDate->setPixmap(UI::svgPixmap(QStringLiteral("calendar"), ink, 18));
    for (int i = 0; i < ui->comboEtat->count(); ++i)
        ui->comboEtat->setItemIcon(i, UI::dotIcon(UI::etatColor(ui->comboEtat->itemText(i))));

    // photo
    ui->btnPen->setIcon(UI::svgIcon(QStringLiteral("pen"), Qt::white, 16));
    ui->btnPlus->setIcon(UI::svgIcon(QStringLiteral("plus"), QColor("#c9a36a"), 22));
    ui->labelThumb1->setPixmap(UI::cover(QPixmap(QStringLiteral(":/machines/images/thumb1.jpg")), QSize(44, 56), 5));
    ui->labelThumb2->setPixmap(UI::cover(QPixmap(QStringLiteral(":/machines/images/thumb2.jpg")), QSize(44, 56), 5));

    // boutons
    ui->btnAdd->setIcon(UI::svgIcon(QStringLiteral("plus"),  Qt::white, 20));
    ui->btnMod->setIcon(UI::svgIcon(QStringLiteral("pen"),   Qt::white, 20));
    ui->btnDel->setIcon(UI::svgIcon(QStringLiteral("trash"), QColor("#4a3a26"), 20));
    ui->btnReset->setIcon(UI::svgIcon(QStringLiteral("refresh"), QColor("#23252e"), 16));
    ui->btnExport->setIcon(UI::svgIcon(QStringLiteral("doc"), Qt::white, 16));

    ui->lineSearch->addAction(UI::svgIcon(QStringLiteral("search"), QColor("#6b7a90"), 18),
                              QLineEdit::LeadingPosition);
}

void MachinesPage::setupBusinessActions()
{
    auto *layout = qobject_cast<QHBoxLayout *>(ui->headerStats->layout());
    if (!layout) return;

    auto addButton = [this, layout](const QString &text, const QString &name, const QString &tip,
                                    auto slot, int width) {
        auto *button = new QPushButton(text, ui->headerStats);
        button->setObjectName(name);
        button->setToolTip(tip);
        button->setFixedSize(width, 32);
        button->setCursor(Qt::PointingHandCursor);
        layout->addWidget(button);
        connect(button, &QPushButton::clicked, this, slot);
    };
    addButton(QStringLiteral("+ Relevé réel"), QStringLiteral("btnAddReading"),
              QStringLiteral("Saisir les heures et la production journalière d'une machine"),
              &MachinesPage::onAddReading, 118);
    addButton(QStringLiteral("Planifier entretien"), QStringLiteral("btnPlanMaintenance"),
              QStringLiteral("Enregistrer une intervention pour une machine"),
              &MachinesPage::onPlanMaintenance, 150);
    addButton(QStringLiteral("Appliquer le planning"), QStringLiteral("btnApplyPlan"),
              QStringLiteral("Enregistrer les propositions optimisées pour tout le parc"),
              &MachinesPage::onApplyOptimizedPlan, 155);
}

void MachinesPage::setupTable()
{
    QTableWidget *t = ui->tableMachines;
    t->setColumnCount(10);
    t->setHorizontalHeaderLabels({QString(), QStringLiteral("ID"), QStringLiteral("Nom"),
        QStringLiteral("Type"), QStringLiteral("État"), QStringLiteral("Heures fonct."),
        QStringLiteral("Pièces prod."), QStringLiteral("Heures arrêt"),
        QStringLiteral("Prochaine maint."), QStringLiteral("Aperçu")});
    t->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    t->verticalHeader()->setDefaultSectionSize(50);
    t->setFixedHeight(40 + 50 * kPageSize + 4);
    QHeaderView *hh = t->horizontalHeader();
    hh->setHighlightSections(false);
    hh->setFixedHeight(40);
    hh->setSectionsClickable(true);
    hh->setSectionResizeMode(QHeaderView::Fixed);
    hh->setSectionResizeMode(2, QHeaderView::Fixed);
    hh->setStretchLastSection(false);
    const int widths[] = { 30, 70, 132, 74, 110, 102, 88, 92, 116, 78 };
    for (int c = 0; c < 10; ++c) t->setColumnWidth(c, widths[c]);
    hh->setDefaultAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    // case « tout cocher » de l'en-tête
    connect(hh, &QHeaderView::sectionClicked, this, [this](int c) {
        if (c != 0) return;
        QTableWidget *tw = ui->tableMachines;
        bool all = true;
        for (int r = 0; r < tw->rowCount(); ++r)
            if (tw->item(r, 0)->checkState() != Qt::Checked) all = false;
        for (int r = 0; r < tw->rowCount(); ++r)
            tw->item(r, 0)->setCheckState(all ? Qt::Unchecked : Qt::Checked);
    });
}

void MachinesPage::connectSignals()
{
    connect(ui->lineNom, &QLineEdit::inputRejected, this, [this] {
        emit notify(QStringLiteral("Nom refusé : lettres, chiffres, espaces, apostrophes et traits d'union uniquement (200 caractères maximum)."), true);
    });
    if (kCrudActif) {
        connect(ui->btnAdd,  &QPushButton::clicked, this, &MachinesPage::onAdd);      // Create
        connect(ui->btnMod,  &QPushButton::clicked, this, &MachinesPage::onModify);   // Update
        connect(ui->btnDel,  &QPushButton::clicked, this, &MachinesPage::onDelete);   // Delete
    }
    connect(ui->btnReset,  &QPushButton::clicked, this, &MachinesPage::onReset);
    connect(ui->btnExport, &QPushButton::clicked, this, &MachinesPage::onExportPdf);
    connect(ui->btnPen,    &QToolButton::clicked, this, &MachinesPage::onChoosePhoto);
    connect(ui->btnPlus,   &QPushButton::clicked, this, &MachinesPage::onChoosePhoto);

    connect(ui->comboEtat, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this] { updateDetection(); });
    connect(ui->dateMaintenance, &QDateEdit::dateChanged, this, [this] { updateDetection(); });

    connect(ui->tableMachines, &QTableWidget::itemSelectionChanged, this, &MachinesPage::onRowSelected);
    connect(ui->lineSearch, &QLineEdit::textChanged, this, &MachinesPage::onSearchChanged);
    connect(ui->comboSort, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this] { m_page = 0; applyFilter(); });
}

/* ============================== données =============================== */

const Machine *MachinesPage::findMachine(const QString &id) const
{
    for (const Machine &m : m_all)
        if (m.id == id) return &m;
    return nullptr;
}

void MachinesPage::reloadData()
{
    m_all = Database::all();
    checkAlerts();
    updateCharts();
    applyFilter();
}

void MachinesPage::onSearchChanged(const QString &text)
{
    // les deux champs de recherche restent synchronisés
    m_page = 0;
    applyFilter();
    emit searchTextChanged(text);      // la barre de recherche du haut se met à jour
}

void MachinesPage::setSearchText(const QString &text)
{
    if (ui->lineSearch->text() != text)
        ui->lineSearch->setText(text);  // déclenche onSearchChanged()
}

void MachinesPage::applyFilter()
{
    const QString q = ui->lineSearch->text().trimmed();
    m_view.clear();
    for (const Machine &m : std::as_const(m_all)) {
        // recherche par ID_Machine, Nom_Machine, Type_Machine
        const bool match = q.isEmpty()
            || m.id.contains(q, Qt::CaseInsensitive)
            || m.nom.contains(q, Qt::CaseInsensitive)
            || m.type.contains(q, Qt::CaseInsensitive);
        if (match) m_view.append(m);
    }

    // tri par Nom_Machine, Type_Machine ou Etat
    const int key = ui->comboSort->currentIndex();
    if (key > 0) {
        std::stable_sort(m_view.begin(), m_view.end(), [key](const Machine &a, const Machine &b) {
            const QString x = key == 1 ? a.nom : key == 2 ? a.type : a.etat;
            const QString y = key == 1 ? b.nom : key == 2 ? b.type : b.etat;
            return QString::localeAwareCompare(x, y) < 0;
        });
    }

    renderPage();
}

void MachinesPage::renderPage()
{
    const int total = m_view.size();
    const int pages = qMax(1, (total + kPageSize - 1) / kPageSize);
    m_page = qBound(0, m_page, pages - 1);
    const int from = m_page * kPageSize;
    const int to = qMin(total, from + kPageSize);

    {
        QSignalBlocker blocker(ui->tableMachines);
        ui->tableMachines->clearContents();
        ui->tableMachines->setRowCount(to - from);

        for (int r = 0; r < to - from; ++r) {
            const Machine &m = m_view.at(from + r);

            auto *chk = new QTableWidgetItem;
            chk->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
            chk->setCheckState(Qt::Unchecked);
            chk->setData(Qt::UserRole, m.id);
            ui->tableMachines->setItem(r, 0, chk);

            auto mk = [&](const QString &txt) {
                auto *it = new QTableWidgetItem(txt);
                it->setData(Qt::UserRole, m.id);
                return it;
            };
            ui->tableMachines->setItem(r, 1, mk(m.id));
            ui->tableMachines->setItem(r, 2, mk(m.nom));
            ui->tableMachines->setItem(r, 3, mk(m.type));
            ui->tableMachines->setItem(r, 5, mk(QStringLiteral("%1 h").arg(m.heuresFonctionnement, 0, 'f', 1)));
            ui->tableMachines->setItem(r, 6, mk(QLocale().toString(m.piecesProduites)));
            ui->tableMachines->setItem(r, 7, mk(QStringLiteral("%1 h").arg(m.heuresArret, 0, 'f', 1)));

            // pastille d'état
            QString bg = QStringLiteral("#dcefd9"), fg = QStringLiteral("#3f8a45");
            if (m.etat == Etat::maintenance())  { bg = QStringLiteral("#f7e6cc"); fg = QStringLiteral("#b9781a"); }
            if (m.etat == Etat::indisponible()) { bg = QStringLiteral("#f8d6d2"); fg = QStringLiteral("#c8402f"); }
            auto *pill = new QLabel(m.etat);
            pill->setStyleSheet(QStringLiteral("background:%1;color:%2;border-radius:5px;padding:4px 9px;font-size:12px;")
                                    .arg(bg, fg));
            auto *pw = new QWidget;
            pw->setAttribute(Qt::WA_TransparentForMouseEvents);
            auto *pl = new QHBoxLayout(pw);
            pl->setContentsMargins(4, 0, 4, 0);
            pl->addWidget(pill);
            pl->addStretch();
            ui->tableMachines->setCellWidget(r, 4, pw);

            // date + détection automatique
            const QString reason = m.raisonMaintenance();
            auto *dt = mk((reason.isEmpty() ? QString() : QStringLiteral("⚠ "))
                          + m.prochaineMaintenance.toString(QStringLiteral("dd/MM/yyyy")));
            if (!reason.isEmpty()) {
                dt->setForeground(m.critique() ? QColor("#c8402f") : QColor("#b9781a"));
                dt->setToolTip(reason);
            }
            ui->tableMachines->setItem(r, 8, dt);

            auto *ap = new QLabel;
            ap->setPixmap(UI::cover(UI::machinePixmap(m.photo), QSize(64, 38), 4));
            auto *aw = new QWidget;
            aw->setAttribute(Qt::WA_TransparentForMouseEvents);
            auto *al = new QHBoxLayout(aw);
            al->setContentsMargins(4, 0, 4, 0);
            al->addWidget(ap);
            ui->tableMachines->setCellWidget(r, 9, aw);
        }

        for (int r = 0; r < to - from; ++r)
            if (ui->tableMachines->item(r, 1)->data(Qt::UserRole).toString() == m_currentId)
                ui->tableMachines->selectRow(r);
    }
    buildPager(pages);
}

void MachinesPage::buildPager(int pages)
{
    clearLayout(m_pager);
    m_pager->addStretch();
    auto add = [&](const QString &text, int target, bool current, bool enabled) {
        auto *b = new QPushButton(text);
        b->setObjectName(current ? QStringLiteral("pageCur") : QStringLiteral("page"));
        b->setFixedSize(36, 36);
        b->setEnabled(enabled);
        b->setCursor(Qt::PointingHandCursor);
        connect(b, &QPushButton::clicked, this, [this, target] { m_page = target; renderPage(); });
        m_pager->addWidget(b);
    };
    add(QStringLiteral("‹"), qMax(0, m_page - 1), false, m_page > 0);
    const int start = qMax(0, qMin(m_page - 2, pages - 5));
    const int end = qMin(pages, start + 5);
    if (start > 0) { add(QStringLiteral("1"), 0, false, true); if (start > 1) add(QStringLiteral("…"), start - 1, false, true); }
    for (int i = start; i < end; ++i) add(QString::number(i + 1), i, i == m_page, true);
    if (end < pages) { if (end < pages - 1) add(QStringLiteral("…"), end, false, true); add(QString::number(pages), pages - 1, false, true); }
    add(QStringLiteral("›"), qMin(pages - 1, m_page + 1), false, m_page < pages - 1);
}

void MachinesPage::goToMachine(const QString &id)
{
    for (int i = 0; i < m_view.size(); ++i)
        if (m_view[i].id == id) { m_page = i / kPageSize; break; }
    renderPage();
}

/* ============================ statistiques ============================ */

void MachinesPage::updateCharts()
{
    // Répartition par type
    QMap<QString, int> count;
    for (const Machine &m : std::as_const(m_all)) count[m.type]++;
    QVector<MachinesDonutChart::Slice> slices;
    for (const QString &t : typesMachine())
        if (count.value(t) > 0)
            slices.append(MachinesDonutChart::Slice{ t, count.value(t), UI::typeColor(t) });
    ui->donutChart->setData(slices, QStringLiteral("machines"));

    // Les performances sont calculées à partir des trois attributs de la fiche machine.
    const QVector<Metier::Performance> perf = Metier::performanceParType(m_all, {});
    QVector<MachinesPerformanceBars::Entry> bars;
    for (const Metier::Performance &p : perf)
        bars.append(MachinesPerformanceBars::Entry{ p.type, qBound(0, p.pourcentage(), 100),
                                             qRound(p.disponibilite), qRound(p.productionAtteinte),
                                             p.nombreMachines });
    const QString perfSummary = perf.isEmpty()
        ? QStringLiteral("Ajoutez les heures et pièces produites dans les fiches machines")
        : QStringLiteral("Type le plus performant : %1 · %2 pièces/h · disponibilité %3 %")
              .arg(perf.first().type).arg(perf.first().piecesParHeure, 0, 'f', 1)
              .arg(qRound(perf.first().disponibilite));
    ui->performanceBars->setData(bars, perfSummary);

    // Métier 2 : le plan utilise aussi le score des types pour avancer les entretiens nécessaires.
    const QVector<Metier::Intervention> plan = Metier::planifierMaintenances(
        m_all, perf, Database::allMaintenanceRecords());
    int ajustees = 0;
    QVector<MachinesPlanningList::Entry> rows;
    for (const Metier::Intervention &iv : plan) {
        if (iv.modifiee()) ++ajustees;
        if (rows.size() >= 5) continue;
        QColor c("#2e9e5b");                                  // date conservée
        if (iv.urgente()) c = QColor("#e5484d");              // indisponible ou en retard
        else if (iv.motif == Metier::Intervention::PerformanceFaible) c = QColor("#c47b16");
        else if (iv.modifiee()) c = QColor("#f0a020");        // ajustée (en cours, week-end, même type)
        const QString fmt = QStringLiteral("dd/MM");
        rows.append(MachinesPlanningList::Entry{ iv.nom, iv.actuelle.toString(fmt), iv.proposee.toString(fmt),
                                         iv.raison(), iv.modifiee(), c });
    }
    const QString planSummary = plan.isEmpty()
        ? QStringLiteral("Aucune machine enregistrée")
        : (ajustees == 0 ? QStringLiteral("Planning à jour : aucune date à ajuster")
                         : QStringLiteral("%1 date(s) ajustée(s) · priorité aux retards et types faibles")
                               .arg(ajustees));
    ui->planningList->setData(rows, planSummary);
}

/* ================================ alertes ================================ */

void MachinesPage::checkAlerts()
{
    QSet<QString> unavailable;
    int needing = 0;
    for (const Machine &m : std::as_const(m_all)) {
        if (m.indisponible()) unavailable.insert(m.id);
        if (m.maintenanceRequise()) ++needing;
    }
    // alerte : une machine vient de devenir indisponible
    if (m_alertsReady) {
        for (const QString &id : std::as_const(unavailable)) {
            if (!m_knownUnavailable.contains(id)) {
                const Machine *m = findMachine(id);
                if (m) emit notify(QStringLiteral("Alerte : la machine %1 – %2 est indisponible.").arg(m->id, m->nom), true);
            }
        }
    }
    m_knownUnavailable = unavailable;
    m_alertsReady = true;

    if (needing != m_alertCount) {
        m_alertCount = needing;
        emit alertCountChanged(m_alertCount);
    }
}

void MachinesPage::showAlertsMenu(QWidget *anchor)
{
    QMenu menu(this);
    bool any = false;
    for (const Machine &m : std::as_const(m_all)) {
        const QString reason = m.raisonMaintenance();
        if (reason.isEmpty()) continue;
        any = true;
        QAction *a = menu.addAction(UI::dotIcon(m.critique() ? QColor("#e5484d") : QColor("#e6a23c")),
                                    QStringLiteral("%1 – %2 : %3").arg(m.id, m.nom, reason));
        const QString id = m.id;
        connect(a, &QAction::triggered, this, [this, id] {
            m_currentId = id;
            ui->lineSearch->clear();
            goToMachine(id);
            if (const Machine *mm = findMachine(id)) loadForm(*mm);
        });
    }
    if (!any)
        menu.addAction(QStringLiteral("Aucune alerte – tout le parc est à jour"))->setEnabled(false);
    menu.exec(anchor->mapToGlobal(QPoint(-260, anchor->height() + 4)));
}

/* ============================== formulaire ============================== */

bool MachinesPage::readForm(Machine &m, QString *error) const
{
    m.nom = ui->lineNom->text().trimmed();
    m.type = ui->comboType->currentText();
    m.etat = ui->comboEtat->currentText();
    m.prochaineMaintenance = ui->dateMaintenance->date();
    m.heuresFonctionnement = ui->spinHeuresFonctionnement->value();
    m.piecesProduites = ui->spinPiecesProduites->value();
    m.heuresArret = ui->spinHeuresArret->value();
    m.photo = m_formPhoto;
    return MachineValidation::validate(m, error);
}

void MachinesPage::loadForm(const Machine &m)
{
    ui->lineId->setText(m.id);
    ui->lineNom->setText(m.nom);
    int i = ui->comboType->findText(m.type);
    if (i < 0) { ui->comboType->addItem(m.type); i = ui->comboType->findText(m.type); }
    ui->comboType->setCurrentIndex(i);
    ui->comboEtat->setCurrentIndex(qMax(0, ui->comboEtat->findText(m.etat)));
    ui->dateMaintenance->setDate(m.prochaineMaintenance);
    ui->spinHeuresFonctionnement->setValue(m.heuresFonctionnement);
    ui->spinPiecesProduites->setValue(m.piecesProduites);
    ui->spinHeuresArret->setValue(m.heuresArret);
    m_formPhoto = m.photo;
    updatePreview();
    updateDetection();
}

void MachinesPage::clearForm()
{
    m_currentId.clear();
    m_formPhoto.clear();
    ui->lineId->setText(Database::nextId());
    ui->lineNom->clear();
    ui->comboType->setCurrentIndex(0);
    ui->comboEtat->setCurrentIndex(0);
    ui->dateMaintenance->setDate(QDate::currentDate().addDays(60));
    ui->spinHeuresFonctionnement->setValue(0.0);
    ui->spinPiecesProduites->setValue(0);
    ui->spinHeuresArret->setValue(0.0);
    updatePreview();
    updateDetection();
}

void MachinesPage::updatePreview()
{
    ui->labelPhoto->setPixmap(UI::cover(UI::machinePixmap(m_formPhoto), QSize(142, 214), 8));
}

void MachinesPage::updateDetection()
{
    if (!ui->labelDetection) return;
    Machine tmp;
    tmp.etat = ui->comboEtat->currentText();
    tmp.prochaineMaintenance = ui->dateMaintenance->date();
    const QString reason = tmp.raisonMaintenance();
    if (reason.isEmpty()) {
        ui->labelDetection->setText(QStringLiteral("✔  Détection automatique : maintenance à jour"));
        ui->labelDetection->setProperty("level", QStringLiteral("ok"));
    } else {
        ui->labelDetection->setText(QStringLiteral("⚠  Détection automatique : %1").arg(reason));
        ui->labelDetection->setProperty("level", tmp.critique() ? QStringLiteral("crit") : QStringLiteral("warn"));
    }
    repolish(ui->labelDetection);
}

void MachinesPage::onRowSelected()
{
    const QList<QTableWidgetItem *> items = ui->tableMachines->selectedItems();
    if (items.isEmpty()) return;
    const QString id = items.first()->data(Qt::UserRole).toString();
    if (const Machine *m = findMachine(id)) {
        m_currentId = id;
        loadForm(*m);
    }
}

/* =============================== actions =============================== */

void MachinesPage::onAdd()
{
    Machine m;
    QString err;
    if (!readForm(m, &err)) { QMessageBox::warning(this, QStringLiteral("Ajouter"), err); return; }
    m.id = Database::nextId();
    if (m.id.isEmpty()) { QMessageBox::critical(this, QStringLiteral("Ajouter"), QStringLiteral("Impossible de générer le prochain ID.")); return; }
    if (!Database::insert(m, &err)) { QMessageBox::critical(this, QStringLiteral("Ajouter"), err); return; }
    const QString createdId = m.id;
    ui->lineSearch->clear();
    reloadData();
    goToMachine(createdId);
    emit notify(QStringLiteral("Machine %1 ajoutée avec succès.").arg(createdId), false);
    clearForm();
}

void MachinesPage::onModify()
{
    if (m_currentId.isEmpty()) {
        QMessageBox::information(this, QStringLiteral("Modifier"), QStringLiteral("Sélectionnez d'abord une machine dans la liste."));
        return;
    }
    const Machine *selected = findMachine(m_currentId);
    if (!selected) {
        QMessageBox::warning(this, QStringLiteral("Modifier"), QStringLiteral("Cette machine n'existe plus. Actualisez la liste."));
        return;
    }
    // Conserve les heures et la production, absentes du formulaire simplifié.
    Machine m = *selected;
    QString err;
    if (!readForm(m, &err)) { QMessageBox::warning(this, QStringLiteral("Modifier"), err); return; }
    m.id = m_currentId;
    if (!Database::update(m, &err)) { QMessageBox::critical(this, QStringLiteral("Modifier"), err); return; }
    ui->lineSearch->clear();
    reloadData();
    goToMachine(m.id);
    loadForm(m);
    emit notify(QStringLiteral("Machine %1 modifiée.").arg(m.id), false);
}

void MachinesPage::onDelete()
{
    if (m_currentId.isEmpty()) {
        QMessageBox::information(this, QStringLiteral("Supprimer"), QStringLiteral("Sélectionnez d'abord une machine dans la liste."));
        return;
    }
    const auto r = QMessageBox::question(this, QStringLiteral("Supprimer"),
                                         QStringLiteral("Supprimer définitivement la machine %1 ?").arg(m_currentId));
    if (r != QMessageBox::Yes) return;
    QString err;
    if (!Database::remove(m_currentId, &err)) { QMessageBox::critical(this, QStringLiteral("Supprimer"), err); return; }
    const QString id = m_currentId;
    clearForm();
    reloadData();
    emit notify(QStringLiteral("Machine %1 supprimée.").arg(id), false);
}

void MachinesPage::onReset()
{
    clearForm();
    ui->tableMachines->clearSelection();
}

void MachinesPage::onChoosePhoto()
{
    const QString f = QFileDialog::getOpenFileName(this, QStringLiteral("Choisir une photo"), QString(),
                                                   QStringLiteral("Images (*.png *.jpg *.jpeg *.bmp)"));
    if (f.isEmpty()) return;
    m_formPhoto = f;
    updatePreview();
    if (!m_currentId.isEmpty())
        emit notify(QStringLiteral("Photo sélectionnée : cliquez sur « Modifier » pour l'enregistrer."), false);
}

void MachinesPage::onAddReading()
{
    if (m_all.isEmpty()) {
        QMessageBox::information(this, QStringLiteral("Relevé de production"),
                                 QStringLiteral("Ajoutez d'abord une machine au parc."));
        return;
    }
    MachineReadingDialog dialog(m_all, this);
    if (dialog.exec() != QDialog::Accepted) return;
    QString error;
    if (!Database::insertReading(dialog.reading(), &error)) {
        QMessageBox::warning(this, QStringLiteral("Relevé non enregistré"),
                             QStringLiteral("Le relevé n'a pas été enregistré. Vérifiez qu'il n'existe pas déjà pour cette machine et cette date.\n\n%1").arg(error));
        return;
    }
    reloadData();
    emit notify(QStringLiteral("Relevé enregistré. Les scores de performance ont été recalculés."), false);
}

void MachinesPage::onPlanMaintenance()
{
    if (m_all.isEmpty()) {
        QMessageBox::information(this, QStringLiteral("Planifier une maintenance"),
                                 QStringLiteral("Ajoutez d'abord une machine au parc."));
        return;
    }
    MaintenancePlanDialog dialog(m_all, Database::allMaintenanceRecords(), this);
    if (dialog.exec() != QDialog::Accepted) return;
    QList<MaintenanceRecord> records;
    records.append(dialog.record());
    QString error;
    if (!Database::applyMaintenancePlan(records, &error)) {
        QMessageBox::critical(this, QStringLiteral("Plan non enregistré"), error);
        return;
    }
    reloadData();
    emit notify(QStringLiteral("Maintenance planifiée et ajoutée à l'historique."), false);
}

void MachinesPage::onApplyOptimizedPlan()
{
    if (m_all.isEmpty()) return;
    const QVector<Metier::Performance> perf = Metier::performanceParType(m_all, {});
    if (perf.isEmpty()) {
        QMessageBox::information(this, QStringLiteral("Planning optimisé"),
            QStringLiteral("Renseignez les heures de fonctionnement, pièces produites et heures d'arrêt dans les fiches machines."));
        return;
    }
    const QVector<Metier::Intervention> plan = Metier::planifierMaintenances(
        m_all, perf, Database::allMaintenanceRecords());
    const auto answer = QMessageBox::question(this, QStringLiteral("Appliquer le planning optimisé"),
        QStringLiteral("Enregistrer les dates proposées pour les %1 machines ?\n\n"
                       "Les anciennes dates seront conservées dans l'historique comme replanifiées.")
            .arg(plan.size()), QMessageBox::Yes | QMessageBox::Cancel, QMessageBox::Cancel);
    if (answer != QMessageBox::Yes) return;

    QList<MaintenanceRecord> records;
    for (const Metier::Intervention &iv : plan) {
        MaintenanceRecord r;
        r.machineId = iv.id;
        r.plannedDate = iv.proposee;
        r.priority = iv.urgente() ? 5
                     : iv.motif == Metier::Intervention::PerformanceFaible ? 4 : 2;
        r.estimatedHours = iv.dureeEstimee;
        r.description = iv.raison();
        records.append(r);
    }
    QString error;
    if (!Database::applyMaintenancePlan(records, &error)) {
        QMessageBox::critical(this, QStringLiteral("Planning non enregistré"), error);
        return;
    }
    reloadData();
    emit notify(QStringLiteral("Planning optimisé enregistré pour %1 machines.").arg(records.size()), false);
}

void MachinesPage::onExportPdf()
{
    const QString path = QFileDialog::getSaveFileName(this, QStringLiteral("Exporter la liste des machines"),
                                                      QStringLiteral("liste_machines.pdf"),
                                                      QStringLiteral("PDF (*.pdf)"));
    if (path.isEmpty()) return;

    QString rows;
    for (const Machine &m : std::as_const(m_view)) {
        const QString alert = m.raisonMaintenance();
        rows += QStringLiteral("<tr><td>%1</td><td>%2</td><td>%3</td><td>%4</td><td>%5</td>"
                               "<td>%6</td><td>%7</td><td>%8</td><td>%9</td></tr>")
                    .arg(m.id, m.nom.toHtmlEscaped(), m.type, m.etat,
                         QStringLiteral("%1 h").arg(m.heuresFonctionnement, 0, 'f', 1),
                         QString::number(m.piecesProduites),
                         QStringLiteral("%1 h").arg(m.heuresArret, 0, 'f', 1),
                         m.prochaineMaintenance.toString(QStringLiteral("dd/MM/yyyy")), alert);
    }
    const QString html = QStringLiteral(
        "<h1 style='color:#0a2748'>FashioNova – Liste des machines</h1>"
        "<p>Édité le %1 – %2 machine(s)</p>"
        "<table border='1' cellspacing='0' cellpadding='5' width='100%'>"
        "<tr style='background:#efe3cf'><th>ID_Machine</th><th>Nom_Machine</th><th>Type_Machine</th>"
        "<th>Etat</th><th>Heures fonctionnement</th><th>Pièces produites</th><th>Heures d'arrêt</th>"
        "<th>Date prochaine maintenance</th><th>Alerte</th></tr>%3</table>")
        .arg(QDate::currentDate().toString(QStringLiteral("dd/MM/yyyy")))
        .arg(m_view.size())
        .arg(rows);

    QPdfWriter writer(path);
    writer.setPageSize(QPageSize(QPageSize::A4));
    writer.setPageMargins(QMarginsF(15, 15, 15, 15));
    QTextDocument doc;
    doc.setHtml(html);
    doc.print(&writer);

    emit notify(QStringLiteral("PDF exporté : %1").arg(path), false);
    QDesktopServices::openUrl(QUrl::fromLocalFile(path));
}
