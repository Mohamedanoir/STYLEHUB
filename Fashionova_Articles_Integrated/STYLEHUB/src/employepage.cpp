#include "employepage.h"
#include "logindialog.h"
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QTableWidget>
#include <QHeaderView>
#include <QLineEdit>
#include <QSignalBlocker>
#include <QComboBox>
#include <QLabel>
#include <QPushButton>
#include <QGroupBox>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSplitter>
#include <QPixmap>
#include <QCheckBox>
#include <QRegularExpression>
#include <QDoubleValidator>
#include <QPainter>
#include <QPainterPath>
#include <QScrollBar>
#include <QScrollArea>
#include <QMap>
#include <QSpacerItem>
#include <algorithm>

// ============================================================================
//  Graphiques statistiques internes au module Employés
// ============================================================================
class EmployeStatsWidget : public QWidget {
public:
    QList<Employe> *data = nullptr;
    explicit EmployeStatsWidget(QWidget *p = nullptr) : QWidget(p) {
        setMinimumHeight(310);
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    }
protected:
    void paintEvent(QPaintEvent *) override {
        QPainter p(this); p.setRenderHint(QPainter::Antialiasing);
        p.setPen(QColor("#0B3B67"));
        int W = width(), H = height(), gap = 12, cardW = (W - 2*gap) / 3;
        for (int c = 0; c < 3; c++) {
            QRect r(c*(cardW+gap), 0, cardW, H);
            p.setBrush(Qt::white); p.setPen(QColor("#E6DDD0")); p.drawRoundedRect(r, 10, 10);
        }
        QFont title = p.font(); title.setBold(true); title.setPointSize(11); p.setFont(title);
        p.setPen(QColor("#0B3B67"));
        p.drawText(15, 28, "Répartition par poste (Présents)");
        p.drawText(cardW+gap+15, 28, "Présences en direct");
        p.drawText(2*(cardW+gap)+15, 28, "Top 5 des salaires (% max)");

        int present = 0; QMap<QString,int> by;
        if (data) for (const auto &e : *data) { if (e.present) { by[e.poste]++; present++; } }
        const int totalEmployees = data ? data->size() : 0;
        QPoint center(105, 142); int rad = 62;
        QRectF pie(center.x()-rad, center.y()-rad, rad*2, rad*2);
        QList<QColor> colors = {QColor("#0B3B67"),QColor("#3E96D2"),QColor("#8BD0E8"),QColor("#E6A84E")};
        double start = 0; int k = 0; int total = qMax(1, present);
        for (auto it = by.begin(); it != by.end(); ++it) {
            double span = 360.0 * it.value() / total;
            p.setBrush(colors[k%colors.size()]); p.setPen(Qt::NoPen);
            p.drawPie(pie, start*16, span*16); start += span; k++;
        }
        p.setBrush(Qt::white); p.drawEllipse(center, 35, 35);
        p.setPen(QColor("#0B3B67")); QFont big = p.font(); big.setPointSize(18); big.setBold(true); p.setFont(big);
        p.drawText(QRect(center.x()-35, center.y()-5, 70, 25), Qt::AlignCenter, QString::number(present));
        QFont sm = p.font(); sm.setPointSize(8); sm.setBold(false); p.setFont(sm);
        p.drawText(QRect(center.x()-48, center.y()+16, 96, 18), Qt::AlignCenter, "employés présents");
        int y = 78; k = 0;
        for (auto it = by.begin(); it != by.end() && k < 5; ++it, ++k) {
            p.setBrush(colors[k%colors.size()]); p.drawEllipse(cardW-155, y-8, 14, 14);
            p.setPen(QColor("#0B3B67")); p.drawText(cardW-132, y+4, it.key());
            const int pct = present > 0 ? qRound(100.0*it.value()/present) : 0;
            p.drawText(cardW-60, y+4, QString::number(pct)+"% "+QString::number(it.value()));
            y += 30;
        }
        const int chartX = cardW+gap+28, chartY = 74, chartW2 = cardW-55, chartH = H-112;
        const int absent = qMax(0, totalEmployees-present), maxCount = qMax(1, totalEmployees);
        p.setPen(QColor("#D5DEE7"));
        for (int i = 0; i <= 4; i++) { int yy = chartY+chartH-i*chartH/4; p.drawLine(chartX, yy, chartX+chartW2, yy); }
        const int barW = qMin(72, (chartW2-55)/2), baseY = chartY+chartH;
        const int presentH = present*chartH/maxCount, absentH = absent*chartH/maxCount;
        p.setPen(Qt::NoPen);
        p.setBrush(QColor("#39A96B")); p.drawRoundedRect(chartX+chartW2/4-barW/2, baseY-presentH, barW, presentH, 6, 6);
        p.setBrush(QColor("#D8A24A")); p.drawRoundedRect(chartX+3*chartW2/4-barW/2, baseY-absentH, barW, absentH, 6, 6);
        p.setPen(QColor("#0B3B67"));
        const int pPct = totalEmployees > 0 ? qRound(100.0*present/totalEmployees) : 0;
        const int aPct = totalEmployees > 0 ? qRound(100.0*absent/totalEmployees) : 0;
        p.drawText(chartX+chartW2/4-28, baseY-presentH-8, QString::number(pPct)+"% ("+QString::number(present)+")");
        p.drawText(chartX+3*chartW2/4-28, baseY-absentH-8, QString::number(aPct)+"% ("+QString::number(absent)+")");
        p.drawText(chartX+chartW2/4-barW/2-8, baseY+20, "Présents");
        p.drawText(chartX+3*chartW2/4-barW/2-8, baseY+20, "Absents");

        QList<Employe> top = data ? *data : QList<Employe>();
        std::stable_sort(top.begin(), top.end(), [](const Employe &a, const Employe &b){ return a.salaire > b.salaire; });
        const double maxSalary = top.isEmpty() ? 0.0 : top.first().salaire;
        int yy = 76; const int left = 2*(cardW+gap)+15, barX = left+145, barMax = qMax(20, cardW-315);
        for (int i = 0; i < qMin(5, top.size()); i++) {
            const auto &e = top[i]; p.setPen(QColor("#0B3B67"));
            p.drawText(left, yy, (e.nom+" "+e.prenom).left(20));
            int sw = maxSalary > 0 ? int(barMax*e.salaire/maxSalary) : 0;
            int pct2 = maxSalary > 0 ? qRound(100.0*e.salaire/maxSalary) : 0;
            p.setBrush(QColor("#17639A")); p.setPen(Qt::NoPen); p.drawRoundedRect(barX, yy-13, sw, 16, 8, 8);
            p.setPen(QColor("#0B3B67")); p.drawText(barX+barMax+5, yy, QString::number(pct2)+"% / "+QString::number(e.salaire,'f',0)+" DT");
            yy += 34;
        }
        if (top.isEmpty()) { p.setPen(QColor("#778899")); p.drawText(left, 90, "Aucun employé enregistré"); }
    }
};

// ============================================================================
//  Helpers de style
// ============================================================================
static QString styleField() {
    return "QLineEdit,QComboBox{padding:8px;border:1px solid #D8DEE5;border-radius:7px;background:white;color:#0B3B67;}";
}
static QString styleButton(const QString &bg, const QString &fg = "white") {
    return QString("QPushButton{background:%1;color:%2;border:0;border-radius:7px;padding:10px;font-weight:700;}").arg(bg, fg);
}

// ============================================================================
//  EmployePage
// ============================================================================
EmployePage::EmployePage(QWidget *parent) : QWidget(parent) {
    buildUi();
    loadEmployees();
    refresh();
    updateStats();
}

void EmployePage::buildUi() {
    auto *root = new QVBoxLayout(this);
    root->setContentsMargins(0, 0, 0, 0);
    root->setSpacing(0);

    // ---- body scrollable ----
    QWidget *body = new QWidget; body->setStyleSheet("background:#F8F5EF;");
    auto *bv = new QVBoxLayout(body); bv->setContentsMargins(12, 10, 12, 12); bv->setSpacing(10);

    auto *split = new QSplitter(Qt::Horizontal); split->setChildrenCollapsible(false);

    // ---- Formulaire ----
    auto *formBox = new QGroupBox("👤  Informations de l'employé");
    formBox->setStyleSheet("QGroupBox{background:white;border:1px solid #E4DBCF;border-radius:10px;"
                           "margin-top:28px;padding-top:22px;font-size:17px;font-weight:700;color:#0B3B67;}"
                           "QGroupBox::title{subcontrol-origin:margin;top:-1px;left:12px;padding:2px 8px;"
                           "color:#0B3B67;background:#F8F5EF;}");
    auto *g = new QGridLayout(formBox); g->setContentsMargins(14,12,14,12); g->setVerticalSpacing(8);

    m_id       = new QLineEdit; m_id->setReadOnly(true); m_id->setPlaceholderText("Généré par Oracle");
    m_nom      = new QLineEdit; m_nom->setMaxLength(50);
    m_prenom   = new QLineEdit; m_prenom->setMaxLength(50);
    m_salary   = new QLineEdit;
    m_password = new QLineEdit; m_password->setEchoMode(QLineEdit::Password); m_password->setMaxLength(100);
    auto *salVal = new QDoubleValidator(0.0, 9999999.999, 3, m_salary);
    salVal->setNotation(QDoubleValidator::StandardNotation);
    m_salary->setValidator(salVal);
    m_poste = new QComboBox;
    m_poste->addItems({"Manager","Designer","Couturière","Technicien","Responsable RH","Styliste"});

    for (QWidget *w : {(QWidget*)m_id,(QWidget*)m_nom,(QWidget*)m_prenom,
                       (QWidget*)m_salary,(QWidget*)m_password,(QWidget*)m_poste})
        w->setStyleSheet(styleField());

    int r = 0;
    auto add = [&](const QString &l, QWidget *w) {
        auto *lab = new QLabel(l);
        lab->setStyleSheet("color:#0B3B67;font-weight:700;font-size:14px;");
        g->addWidget(lab, r, 0); g->addWidget(w, r++, 1);
    };
    add("ID Employé", m_id); add("Nom", m_nom); add("Prénom", m_prenom);
    add("Poste", m_poste);   add("Salaire", m_salary); add("Mot de passe", m_password);

    QLabel *photo = new QLabel;
    photo->setPixmap(QPixmap(":/assets/employee_photo.png").scaled(78, 68, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    photo->setStyleSheet("background:#F3E8D7;border-radius:7px;");
    g->addItem(new QSpacerItem(1,8,QSizePolicy::Minimum,QSizePolicy::Fixed), r++, 0, 1, 2);
    g->addWidget(new QLabel("Photo"), r, 0); g->addWidget(photo, r++, 1);

    QPushButton *cv = new QPushButton("📄  Importer depuis CV");
    cv->setStyleSheet(styleButton("#1769AA")); g->addWidget(cv, r++, 0, 1, 2);

    auto *hb  = new QHBoxLayout;
    QPushButton *a   = new QPushButton("＋ Ajouter");
    QPushButton *ed  = new QPushButton("✎ Modifier");
    QPushButton *del = new QPushButton("🗑 Supprimer");
    a->setStyleSheet(styleButton("#0B3B67")); ed->setStyleSheet(styleButton("#7892AF")); del->setStyleSheet(styleButton("#F1DDBE","#0B3B67"));
    hb->addWidget(a); hb->addWidget(ed); hb->addWidget(del);
    g->addLayout(hb, r++, 0, 1, 2);

    QPushButton *show = new QPushButton("◉ Afficher");
    show->setStyleSheet(styleButton("#DCE8F3","#0B3B67"));
    g->addWidget(show, r++, 0, 1, 2);
    split->addWidget(formBox);

    // ---- Liste ----
    auto *listBox = new QGroupBox("👥  Liste des employés");
    listBox->setStyleSheet(formBox->styleSheet());
    auto *lv = new QVBoxLayout(listBox); lv->setContentsMargins(10,12,10,10);

    auto *toprow = new QHBoxLayout;
    m_listSearch = new QLineEdit; m_listSearch->setPlaceholderText("Rechercher un employé..."); m_listSearch->setStyleSheet(styleField());
    QComboBox *sort = new QComboBox; sort->addItems({"Nom","ID","Poste","État"}); sort->setStyleSheet(styleField());
    toprow->addWidget(m_listSearch, 2); toprow->addWidget(new QLabel("Trier par :")); toprow->addWidget(sort);
    lv->addLayout(toprow);

    m_table = new QTableWidget(0, 8);
    m_table->setHorizontalHeaderLabels({"","ID","Nom","Prénom","Poste","Salaire","État","Actions"});
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->verticalHeader()->setVisible(false);
    m_table->horizontalHeader()->setStretchLastSection(true);
    m_table->setColumnWidth(0,38); m_table->setColumnWidth(1,88); m_table->setColumnWidth(2,105);
    m_table->setColumnWidth(3,105); m_table->setColumnWidth(4,110); m_table->setColumnWidth(5,95);
    m_table->setColumnWidth(6,105); m_table->setColumnWidth(7,95);
    m_table->setStyleSheet("QTableWidget{background:white;color:#0B3B67;gridline-color:#E2E5E8;}"
                           "QHeaderView::section{background:#F7EBD8;color:#0B3B67;font-weight:700;padding:9px;border:0;}");
    lv->addWidget(m_table, 1);

    auto *loginrow = new QHBoxLayout;
    m_status = new QLabel("● Aucun employé connecté");
    m_status->setStyleSheet("background:#DDF5E5;color:#08783E;padding:9px;border-radius:7px;font-weight:700;");
    QPushButton *loginb  = new QPushButton("Connexion employé"); loginb->setStyleSheet(styleButton("#0B3B67"));
    QPushButton *logoutb = new QPushButton("Déconnexion");       logoutb->setStyleSheet(styleButton("#D99A45"));
    loginrow->addWidget(m_status,1); loginrow->addWidget(loginb); loginrow->addWidget(logoutb);
    lv->addLayout(loginrow);
    split->addWidget(listBox);
    split->setSizes({590, 890});
    bv->addWidget(split, 3);

    // ---- Statistiques ----
    auto *statsBox = new QGroupBox("📊  Statistiques des employés");
    statsBox->setStyleSheet(formBox->styleSheet());
    auto *sv = new QVBoxLayout(statsBox); sv->setContentsMargins(8,12,8,10);
    m_stats = new EmployeStatsWidget; m_stats->data = &m_employees;
    sv->addWidget(m_stats); statsBox->setMinimumHeight(350);
    bv->addWidget(statsBox, 2);

    // ---- Scroll ----
    auto *scroll = new QScrollArea;
    scroll->setWidget(body); scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);
    scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    root->addWidget(scroll);

    // ---- Connexions signaux ----
    connect(cv,       &QPushButton::clicked, this, &EmployePage::importCV);
    connect(a,        &QPushButton::clicked, this, &EmployePage::addEmployee);
    connect(ed,       &QPushButton::clicked, this, &EmployePage::modifyEmployee);
    connect(del,      &QPushButton::clicked, this, &EmployePage::removeEmployee);
    connect(show,     &QPushButton::clicked, this, &EmployePage::showEmployee);
    connect(loginb,   &QPushButton::clicked, this, [this]() {
        LoginDialog d(this);
        if (d.exec() != QDialog::Accepted) return;
        int i = -1;
        for (int k = 0; k < m_employees.size(); k++)
            if (m_employees[k].code() == d.id()) { i = k; break; }
        if (i < 0) { QMessageBox::warning(this,"Connexion","Employé introuvable."); return; }
        if (m_employees[i].motDePasse != d.password()) { QMessageBox::warning(this,"Connexion","Mot de passe incorrect."); return; }
        m_employees[i].present = true;
        m_employees[i].derniereConnexion = QDateTime::currentDateTime();
        m_status->setText("● Présent : " + m_employees[i].nom + " " + m_employees[i].prenom);
        refresh(); updateStats();
    });
    connect(logoutb, &QPushButton::clicked, this, [this]() {
        for (auto &e : m_employees) e.present = false;
        m_status->setText("● Aucun employé connecté");
        refresh(); updateStats();
    });
    connect(m_listSearch, &QLineEdit::textChanged, this, &EmployePage::searchChanged);
    connect(sort, &QComboBox::currentTextChanged, this, [this](const QString &f){ m_sortField = f; refresh(); });
    connect(m_table, &QTableWidget::cellClicked, this, &EmployePage::rowClicked);

    // search global sync (topbar search si dispo)
    m_search = new QLineEdit; // champ de recherche global (caché, synchronisé)
    m_search->hide();
}

bool EmployePage::loadEmployees() {
    QList<Employe> previous = m_employees;
    QString erreur;
    QList<Employe> loaded = Employe::afficher(&erreur);
    if (!erreur.isEmpty()) {
        QMessageBox::critical(this, "Oracle", "Impossible de lire EMPLOYES :\n" + erreur);
        return false;
    }
    for (auto &e : loaded)
        for (const auto &old : previous)
            if (old.id == e.id) { e.present = old.present; e.derniereConnexion = old.derniereConnexion; break; }
    m_employees = loaded;
    return true;
}

void EmployePage::refresh() {
    m_table->setRowCount(0);
    QString q = m_listSearch->text().trimmed().toLower();
    QList<Employe> sorted = m_employees;
    std::stable_sort(sorted.begin(), sorted.end(), [this](const Employe &a, const Employe &b) {
        if (m_sortField == "ID")    return a.id < b.id;
        if (m_sortField == "Poste") return a.poste.localeAwareCompare(b.poste) < 0;
        if (m_sortField == "État")  return a.present > b.present;
        return a.nom.localeAwareCompare(b.nom) < 0;
    });
    for (const auto &e : sorted) {
        QString all = e.code()+" "+e.nom+" "+e.prenom+" "+e.poste+" "+QString::number(e.salaire,'f',3);
        if (!q.isEmpty() && !all.toLower().contains(q)) continue;
        int r = m_table->rowCount(); m_table->insertRow(r);
        auto *c = new QTableWidgetItem; c->setCheckState(Qt::Unchecked); m_table->setItem(r,0,c);
        m_table->setItem(r,1,new QTableWidgetItem(e.code()));
        m_table->setItem(r,2,new QTableWidgetItem(e.nom));
        m_table->setItem(r,3,new QTableWidgetItem(e.prenom));
        m_table->setItem(r,4,new QTableWidgetItem(e.poste));
        m_table->setItem(r,5,new QTableWidgetItem(QString::number(e.salaire,'f',3)+" DT"));
        auto *st = new QTableWidgetItem(e.present ? "● Présent" : "○ Absent");
        st->setForeground(e.present ? QBrush(QColor("#138A4B")) : QBrush(QColor("#777")));
        m_table->setItem(r,6,st);
        m_table->setItem(r,7,new QTableWidgetItem("◉  ✎"));
        m_table->setRowHeight(r, 42);
    }
}

void EmployePage::updateStats() { m_stats->update(); m_stats->repaint(); }

int EmployePage::selectedIndex() const {
    auto rs = m_table->selectedRanges();
    if (rs.isEmpty()) return -1;
    QString id = m_table->item(rs.first().topRow(), 1)->text();
    for (int i = 0; i < m_employees.size(); i++)
        if (m_employees[i].code() == id) return i;
    return -1;
}

void EmployePage::fillForm(const Employe &e) {
    m_id->setText(e.code()); m_nom->setText(e.nom); m_prenom->setText(e.prenom);
    m_poste->setCurrentText(e.poste);
    m_salary->setText(QString::number(e.salaire,'f',3));
    m_password->setText(e.motDePasse);
}

Employe EmployePage::formEmployee() const {
    Employe e;
    e.nom = m_nom->text().trimmed(); e.prenom = m_prenom->text().trimmed();
    e.poste = m_poste->currentText();
    e.salaire = m_salary->text().trimmed().toDouble();
    e.motDePasse = m_password->text();
    return e;
}

void EmployePage::rowClicked(int row, int) {
    QString id = m_table->item(row, 1)->text();
    for (const auto &e : m_employees)
        if (e.code() == id) { fillForm(e); break; }
}

void EmployePage::addEmployee() {
    Employe e = formEmployee();
    if (e.nom.isEmpty() || e.prenom.isEmpty() || e.motDePasse.isEmpty()) {
        QMessageBox::warning(this,"Ajouter","Nom, prénom et mot de passe sont obligatoires."); return;
    }
    bool ok; double sal = m_salary->text().trimmed().toDouble(&ok);
    if (!ok || sal < 0 || sal > 9999999.999) {
        QMessageBox::warning(this,"Ajouter","Le salaire doit être entre 0 et 9 999 999,999 DT."); return;
    }
    e.salaire = sal; QString erreur;
    if (!e.ajouter(&erreur)) { QMessageBox::critical(this,"Ajouter","Oracle a refusé l'ajout :\n"+erreur); return; }
    if (e.id > 0) m_id->setText(e.code());
    loadEmployees(); refresh(); updateStats();
}

void EmployePage::modifyEmployee() {
    int i = selectedIndex();
    if (i < 0) { QMessageBox::information(this,"Modifier","Sélectionnez un employé."); return; }
    Employe e = formEmployee(); e.id = m_employees[i].id;
    e.present = m_employees[i].present; e.derniereConnexion = m_employees[i].derniereConnexion;
    if (e.nom.isEmpty() || e.prenom.isEmpty() || e.motDePasse.isEmpty()) {
        QMessageBox::warning(this,"Modifier","Nom, prénom et mot de passe sont obligatoires."); return;
    }
    bool ok; double sal = m_salary->text().trimmed().toDouble(&ok);
    if (!ok || sal < 0 || sal > 9999999.999) {
        QMessageBox::warning(this,"Modifier","Le salaire doit être entre 0 et 9 999 999,999 DT."); return;
    }
    e.salaire = sal; QString erreur;
    if (!e.modifier(&erreur)) { QMessageBox::critical(this,"Modifier","Oracle a refusé la modification :\n"+erreur); return; }
    loadEmployees(); refresh(); updateStats();
}

void EmployePage::removeEmployee() {
    int i = selectedIndex(); if (i < 0) return;
    if (QMessageBox::question(this,"Supprimer","Supprimer cet employé ?") == QMessageBox::Yes) {
        QString erreur;
        if (!Employe::supprimer(m_employees[i].id, &erreur)) {
            QMessageBox::critical(this,"Supprimer","Oracle a refusé :\n"+erreur); return;
        }
        loadEmployees(); refresh(); updateStats();
    }
}

void EmployePage::showEmployee() {
    int i = selectedIndex(); if (i < 0) return;
    const auto &e = m_employees[i];
    QMessageBox::information(this, "Fiche employé",
        QString("ID : %1\nNom : %2 %3\nPoste : %4\nSalaire : %5 DT\nÉtat : %6")
        .arg(e.code(), e.nom, e.prenom, e.poste,
             QString::number(e.salaire,'f',3), e.present ? "Présent" : "Absent"));
}

void EmployePage::searchChanged(const QString &) {
    refresh();
}

QString EmployePage::readText(const QString &path) {
    QFile f(path); if (!f.open(QIODevice::ReadOnly|QIODevice::Text)) return {};
    QTextStream t(&f); return t.readAll();
}

void EmployePage::parseCV(const QString &t) {
    auto get = [&](const QStringList &ks) {
        for (const auto &line : t.split(QRegularExpression("[\\r\\n]+")))
            for (const auto &k : ks) {
                QRegularExpression re("^\\s*"+QRegularExpression::escape(k)+"\\s*[:\\-]\\s*(.+)$",
                                     QRegularExpression::CaseInsensitiveOption);
                auto m = re.match(line); if (m.hasMatch()) return m.captured(1).trimmed();
            }
        return QString();
    };
    QString v = get({"Nom"}); if (!v.isEmpty()) m_nom->setText(v);
    v = get({"Prénom","Prenom"}); if (!v.isEmpty()) m_prenom->setText(v);
    v = get({"Poste","Fonction"}); if (!v.isEmpty()) m_poste->setCurrentText(v);
    v = get({"Salaire","Salary"}); if (!v.isEmpty()) m_salary->setText(v);
}

void EmployePage::importCV() {
    QString p = QFileDialog::getOpenFileName(this,"Choisir un CV",{},"CV texte (*.txt *.csv);;Tous les fichiers (*.*)");
    if (p.isEmpty()) return;
    QString t = readText(p);
    if (t.isEmpty()) { QMessageBox::warning(this,"CV","Impossible de lire le fichier."); return; }
    parseCV(t);
    QMessageBox::information(this,"CV importé","Les informations ont été placées dans le formulaire.\nVérifiez puis cliquez sur Ajouter.");
}
