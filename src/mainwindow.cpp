#include "mainwindow.h"
#include "logindialog.h"
#include <QFile>
#include <QFileDialog>
#include <QApplication>
#include <QTextStream>
#include <QMessageBox>
#include <QTableWidget>
#include <QHeaderView>
#include <QLineEdit>
#include <QSignalBlocker>
#include <QComboBox>
#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QGroupBox>
#include <QFrame>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSplitter>
#include <QPixmap>
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <QDoubleValidator>
#include <QStringList>
#include <QPainter>
#include <QScrollBar>
#include <QScrollArea>
#include <QMap>
#include <QResizeEvent>
#include <QCloseEvent>
#include <QTimer>
#include <QVector>
#include <QPdfDocument>
#include <QFileInfo>
#include <algorithm>

class SideMenuWidget : public QWidget {
public:
    explicit SideMenuWidget(QWidget *parent = nullptr) : QWidget(parent) {
        setObjectName(QStringLiteral("sideMenu"));
        setFixedWidth(245);
        setStyleSheet(QStringLiteral(
            "QWidget#sideMenu{border-image:url(:/assets/sidebar.png) 0 0 0 0 stretch stretch;}"));
        const QStringList names = {"Clients", "Employés", "Commandes", "Maquettes", "Machines", "Articles"};
        for (const QString &name : names) {
            auto *button = new QPushButton(this);
            button->setToolTip(name);
            button->setAccessibleName(name);
            button->setFocusPolicy(Qt::NoFocus);
            button->setCursor(Qt::PointingHandCursor);
            button->setStyleSheet(QStringLiteral(
                "QPushButton{background:transparent;border:0;}"
                "QPushButton:hover{background:rgba(255,255,255,20);}"));
            m_buttons.append(button);
        }
    }

    QPushButton *button(int index) const { return m_buttons.value(index, nullptr); }

protected:
    void resizeEvent(QResizeEvent *event) override {
        QWidget::resizeEvent(event);
        const qreal scale = height() / 1024.0;
        const QList<QPair<int, int>> rows = {{190, 56}, {247, 57}, {304, 55},
                                              {360, 56}, {416, 56}, {472, 57}};
        for (int i = 0; i < m_buttons.size(); ++i) {
            const auto row = rows[i];
            m_buttons[i]->setGeometry(3, qRound(row.first * scale), width() - 6,
                                      qMax(1, qRound(row.second * scale)));
        }
    }

private:
    QVector<QPushButton *> m_buttons;
};

class StatsWidget : public QWidget {
public:
    QList<Employe> *data = nullptr;
    explicit StatsWidget(QWidget *parent = nullptr) : QWidget(parent) {
        setMinimumHeight(310);
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    }
protected:
    void paintEvent(QPaintEvent *) override {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);
        const int gap = 12, cardW = (width() - 2 * gap) / 3;
        const QColor navy("#0B3B67");
        const QList<QColor> colors = {QColor("#0B3B67"), QColor("#3E96D2"), QColor("#8BD0E8"),
                                      QColor("#E6A84E"), QColor("#7892AF"), QColor("#795A8E")};
        for (int i = 0; i < 3; ++i) {
            p.setBrush(Qt::white); p.setPen(QColor("#E6DDD0"));
            p.drawRoundedRect(i * (cardW + gap), 0, cardW, height(), 10, 10);
        }
        QFont title = p.font(); title.setBold(true); title.setPointSize(11); p.setFont(title); p.setPen(navy);
        p.drawText(15, 28, "Repartition par poste (%)");
        p.drawText(cardW + gap + 15, 28, "Effectif et salaires");
        p.drawText(2 * (cardW + gap) + 15, 28, "Top 5 des salaires (% max)");

        const QList<Employe> employees = data ? *data : QList<Employe>();
        QMap<QString, int> byPoste;
        double payroll = 0.0;
        for (const Employe &e : employees) { byPoste[e.poste]++; payroll += e.salaire; }
        const int total = employees.size();
        const QPoint center(95, 145);
        const int radius = 60;
        const QRectF pie(center.x() - radius, center.y() - radius, radius * 2, radius * 2);
        double startAngle = 0.0;
        int color = 0;
        for (auto it = byPoste.cbegin(); it != byPoste.cend(); ++it, ++color) {
            const double span = total ? 360.0 * it.value() / total : 0.0;
            p.setBrush(colors[color % colors.size()]); p.setPen(Qt::NoPen);
            p.drawPie(pie, qRound(startAngle * 16), qRound(span * 16)); startAngle += span;
        }
        p.setBrush(Qt::white); p.drawEllipse(center, 34, 34); p.setPen(navy);
        QFont countFont = p.font(); countFont.setPointSize(18); countFont.setBold(true); p.setFont(countFont);
        p.drawText(QRect(center.x()-30, center.y()-10, 60, 26), Qt::AlignCenter, QString::number(total));
        QFont small = p.font(); small.setPointSize(8); small.setBold(false); p.setFont(small);
        p.drawText(QRect(center.x()-42, center.y()+14, 84, 18), Qt::AlignCenter, "employees");
        int y = 76; color = 0;
        for (auto it = byPoste.cbegin(); it != byPoste.cend() && color < 6; ++it, ++color) {
            p.setBrush(colors[color % colors.size()]); p.setPen(Qt::NoPen); p.drawEllipse(cardW-150, y-8, 12, 12);
            p.setPen(navy); p.drawText(cardW-130, y+4, it.key().left(14));
            const int pct = total ? qRound(100.0 * it.value() / total) : 0;
            p.drawText(cardW-54, y+4, QString::number(pct) + "% (" + QString::number(it.value()) + ")");
            y += 30;
        }
        const int middleX = cardW + gap + 24;
        QFont metric = p.font(); metric.setPointSize(13); metric.setBold(true); p.setFont(metric); p.setPen(navy);
        p.drawText(middleX, 100, "Employees: " + QString::number(total));
        p.drawText(middleX, 150, "Total salary: " + QString::number(payroll, 'f', 3) + " DT");
        p.drawText(middleX, 200, "Average salary: " + QString::number(total ? payroll/total : 0.0, 'f', 3) + " DT");

        QList<Employe> top = employees;
        std::stable_sort(top.begin(), top.end(), [](const Employe &a, const Employe &b) { return a.salaire > b.salaire; });
        const double maxSalary = top.isEmpty() ? 0.0 : top.first().salaire;
        const int left = 2 * (cardW + gap) + 15, barX = left + 130, barMax = qMax(20, cardW - 220);
        int yy = 76;
        for (int i = 0; i < qMin(5, top.size()); ++i, yy += 38) {
            const Employe &e = top[i]; p.setPen(navy); p.drawText(left, yy, (e.nom + " " + e.prenom).left(18));
            const int pct = maxSalary ? qRound(100.0 * e.salaire / maxSalary) : 0;
            const int w = maxSalary ? qRound(barMax * e.salaire / maxSalary) : 0;
            p.setBrush(QColor("#17639A")); p.setPen(Qt::NoPen); p.drawRoundedRect(barX, yy-13, w, 16, 8, 8);
            p.setPen(navy); p.drawText(barX+barMax+5, yy, QString::number(pct) + "% / " + QString::number(e.salaire, 'f', 0) + " DT");
        }
        if (top.isEmpty()) { p.setPen(QColor("#778899")); p.drawText(left, 90, "No employees registered"); }
    }
};
static QString styleField(){return "QLineEdit,QComboBox{padding:8px;border:1px solid #D8DEE5;border-radius:7px;background:white;color:#0B3B67;}";}
static QString styleButton(const QString &bg,const QString &fg="white"){return QString("QPushButton{background:%1;color:%2;border:0;border-radius:7px;padding:10px;font-weight:700;}").arg(bg,fg);}

MainWindow::MainWindow(QWidget *p):QMainWindow(p){
    setMinimumSize(1200,760); resize(1536,1024); setWindowTitle("FashionNova — Gestion des Employés");
    buildUi(); loadEmployees(); refresh(); updateStats();
    QTimer::singleShot(0, this, [this] { employeeLoginFlow(); });
}
void MainWindow::buildUi(){
    QWidget *central=new QWidget; setCentralWidget(central);
    auto *root=new QHBoxLayout(central); root->setContentsMargins(0,0,0,0); root->setSpacing(0);
    auto *sidebar = new SideMenuWidget;
    root->addWidget(sidebar);
    QWidget *right = new QWidget;
    auto *rv = new QVBoxLayout(right);
    rv->setContentsMargins(0, 0, 0, 0);
    rv->setSpacing(10);
    auto *bar = new QWidget;
    bar->setMinimumHeight(65);
    bar->setStyleSheet("background:#FBF8F2;");
    auto *bh = new QHBoxLayout(bar);
    m_search = new QLineEdit;
    m_search->setPlaceholderText("Search employee, position, salary...");
    m_search->setStyleSheet(styleField());
    bh->addWidget(m_search, 1);
    m_presenceLabel = new QLabel("Aucun employe connecte");
    m_presenceLabel->setStyleSheet("color:#07823D;font-weight:700;padding:8px;background:#DDF5E6;border-radius:7px;");
    bh->addWidget(m_presenceLabel);
    auto *logoutButton = new QPushButton("Deconnexion / Login");
    logoutButton->setStyleSheet(styleButton("#D99A3E"));
    bh->addWidget(logoutButton);
    rv->addWidget(bar);
    QLabel *banner=new QLabel;banner->setPixmap(QPixmap(":/assets/banner.png"));banner->setScaledContents(true);banner->setMinimumHeight(110);rv->addWidget(banner);

    QWidget *body=new QWidget; body->setStyleSheet("background:#F8F5EF;"); auto *bv=new QVBoxLayout(body);bv->setContentsMargins(12,0,12,0);bv->setSpacing(10);
    auto *split=new QSplitter(Qt::Horizontal);split->setChildrenCollapsible(false);
    QGroupBox *formBox=new QGroupBox("👤  Informations de l'employé");formBox->setStyleSheet("QGroupBox{background:white;border:1px solid #E4DBCF;border-radius:10px;margin-top:28px;padding-top:22px;font-size:17px;font-weight:700;color:#0B3B67;}QGroupBox::title{subcontrol-origin:margin;top:-1px;left:12px;padding:2px 8px;color:#0B3B67;background:#F8F5EF;}");
    auto *g=new QGridLayout(formBox);g->setContentsMargins(14,12,14,12);g->setVerticalSpacing(8);
    m_id=new QLineEdit;m_id->setReadOnly(true);m_id->setPlaceholderText("Généré par Oracle");m_nom=new QLineEdit;m_prenom=new QLineEdit;m_salary=new QLineEdit;m_password=new QLineEdit;m_password->setEchoMode(QLineEdit::Password);
    m_nom->setMaxLength(50);m_prenom->setMaxLength(50);m_password->setMaxLength(100);
    // Lettres Unicode + espaces, apostrophes et tirets pour les noms composés.
    auto *nameValidator=new QRegularExpressionValidator(QRegularExpression(QStringLiteral("[\\p{L} '\\-]*")),this);
    m_nom->setValidator(nameValidator);
    m_prenom->setValidator(new QRegularExpressionValidator(QRegularExpression(QStringLiteral("[\\p{L} '\\-]*")),this));
    // Un salaire reste numérique; on accepte un point ou une virgule et 3 décimales.
    m_salary->setValidator(new QRegularExpressionValidator(QRegularExpression(QStringLiteral("[0-9]*([.,][0-9]{0,3})?")),this));
    m_poste=new QComboBox;m_poste->addItems({"Manager","Designer","Couturière","Technicien","Responsable RH","Styliste"});
    for(QWidget*w:{(QWidget*)m_id,(QWidget*)m_nom,(QWidget*)m_prenom,(QWidget*)m_salary,(QWidget*)m_password,(QWidget*)m_poste})w->setStyleSheet(styleField());
    int r=0;auto add=[&](const QString&l,QWidget*w){
    auto *lab=new QLabel(l);
    lab->setStyleSheet("color:#0B3B67;font-weight:700;font-size:14px;");
    g->addWidget(lab,r,0);
    g->addWidget(w,r++,1);
};
    add("ID Employé",m_id);add("Nom",m_nom);add("Prénom",m_prenom);add("Poste",m_poste);add("Salaire",m_salary);add("Mot de passe",m_password);
    QLabel *photo=new QLabel;photo->setPixmap(QPixmap(":/assets/employee_photo.png").scaled(78,68,Qt::KeepAspectRatio,Qt::SmoothTransformation));photo->setStyleSheet("background:#F3E8D7;border-radius:7px;");
    g->addItem(new QSpacerItem(1,8,QSizePolicy::Minimum,QSizePolicy::Fixed),r++,0,1,2);g->addWidget(new QLabel("Photo"),r,0);g->addWidget(photo,r++,1);
    QPushButton *cv=new QPushButton("📄  Importer depuis CV");cv->setStyleSheet(styleButton("#1769AA"));g->addWidget(cv,r++,0,1,2);
    auto *hb=new QHBoxLayout;QPushButton*a=new QPushButton("＋ Ajouter"),*ed=new QPushButton("✎ Modifier"),*del=new QPushButton("🗑 Supprimer");a->setStyleSheet(styleButton("#0B3B67"));ed->setStyleSheet(styleButton("#7892AF"));del->setStyleSheet(styleButton("#F1DDBE","#0B3B67"));hb->addWidget(a);hb->addWidget(ed);hb->addWidget(del);g->addLayout(hb,r++,0,1,2);
    QPushButton*show=new QPushButton("◉ Afficher");show->setStyleSheet(styleButton("#DCE8F3","#0B3B67"));g->addWidget(show,r++,0,1,2);
    split->addWidget(formBox);

    QGroupBox *listBox=new QGroupBox("👥  Liste des employés");listBox->setStyleSheet(formBox->styleSheet());auto*lv=new QVBoxLayout(listBox);lv->setContentsMargins(10,12,10,10);
    auto *toprow=new QHBoxLayout;m_listSearch=new QLineEdit;m_listSearch->setPlaceholderText("Rechercher un employe...");m_listSearch->setStyleSheet(styleField());QComboBox*sort=new QComboBox;sort->addItems({"Nom","ID","Poste"});sort->setStyleSheet(styleField());toprow->addWidget(m_listSearch,2);toprow->addWidget(new QLabel("Trier par :"));toprow->addWidget(sort);lv->addLayout(toprow);
    m_table=new QTableWidget(0,8);m_table->setHorizontalHeaderLabels({"","ID","Nom","Prenom","Poste","Salaire","État","Actions"});m_table->setSelectionBehavior(QAbstractItemView::SelectRows);m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);m_table->verticalHeader()->setVisible(false);m_table->horizontalHeader()->setStretchLastSection(true);m_table->setColumnWidth(0,38);m_table->setColumnWidth(1,88);m_table->setColumnWidth(2,125);m_table->setColumnWidth(3,125);m_table->setColumnWidth(4,130);m_table->setColumnWidth(5,110);m_table->setColumnWidth(6,100);m_table->setColumnWidth(7,95);m_table->setStyleSheet("QTableWidget{background:white;color:#0B3B67;gridline-color:#E2E5E8;alternate-background-color:#FFFFFF;}QHeaderView::section{background:#F7EBD8;color:#0B3B67;font-weight:700;padding:9px;border:0;}");lv->addWidget(m_table,1);
    split->addWidget(listBox);split->setSizes({590,890});bv->addWidget(split,3);

    QGroupBox*statsBox=new QGroupBox("📊  Statistiques des employés");statsBox->setStyleSheet(formBox->styleSheet());auto*sv=new QVBoxLayout(statsBox);sv->setContentsMargins(8,12,8,10);m_stats=new StatsWidget; m_stats->data=&m_employees;sv->addWidget(m_stats);statsBox->setMinimumHeight(350);bv->addWidget(statsBox,2);
    QScrollArea *scroll=new QScrollArea;
    scroll->setWidget(body);
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);
    scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scroll->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scroll->setStyleSheet("QScrollArea{background:#171717;border:0;} QScrollBar:vertical{width:14px;background:#252525;margin:0;} QScrollBar::handle:vertical{background:#888;border-radius:7px;min-height:55px;} QScrollBar::add-line:vertical,QScrollBar::sub-line:vertical{height:0;} QScrollBar::add-page:vertical,QScrollBar::sub-page:vertical{background:#252525;}");
    rv->addWidget(scroll, 1);
    root->addWidget(right, 1);

    connect(cv, &QPushButton::clicked, this, &MainWindow::importCV);
    connect(a, &QPushButton::clicked, this, &MainWindow::addEmployee);
    connect(ed, &QPushButton::clicked, this, &MainWindow::modifyEmployee);
    connect(del, &QPushButton::clicked, this, &MainWindow::removeEmployee);
    connect(show, &QPushButton::clicked, this, &MainWindow::showEmployee);
    connect(m_listSearch, &QLineEdit::textChanged, this, &MainWindow::searchChanged);
    connect(m_search, &QLineEdit::textChanged, this, &MainWindow::searchChanged);
    connect(sort, &QComboBox::currentTextChanged, this, [this](const QString &field) { m_sortField = field; refresh(); });
    connect(m_table, &QTableWidget::cellClicked, this, &MainWindow::rowClicked);
    connect(logoutButton, &QPushButton::clicked, this, &MainWindow::logoutEmployee);
    connect(sidebar->button(1), &QPushButton::clicked, this, [this] {
        if (loadEmployees()) { refresh(); updateStats(); }
    });
    const QStringList otherModules = {"Clients", "Commandes", "Maquettes", "Machines", "Articles"};
    const QList<int> menuIndexes = {0, 2, 3, 4, 5};
    for (int i = 0; i < otherModules.size(); ++i) {
        connect(sidebar->button(menuIndexes[i]), &QPushButton::clicked, this, [this, module = otherModules[i]] {
            QMessageBox::information(this, module,
                QStringLiteral("Le module %1 appartient à un autre membre de l'équipe.").arg(module));
        });
    }
}
bool MainWindow::loadEmployees(){
    QString erreur;
    QList<Employe> loaded=Employe::afficher(&erreur);
    if(!erreur.isEmpty()){
        QMessageBox::critical(this,"Oracle","Impossible de lire EMPLOYES :\n"+erreur);
        return false;
    }
    m_employees=loaded;
    return true;
}
void MainWindow::refresh(){
    m_table->setRowCount(0);
    const QString queryText=m_listSearch->text().trimmed().toLower();
    QList<Employe> sorted=m_employees;
    std::stable_sort(sorted.begin(),sorted.end(),[this](const Employe &a,const Employe &b){
        if(m_sortField==QStringLiteral("ID")) return a.id<b.id;
        if(m_sortField==QStringLiteral("Poste")) return a.poste.localeAwareCompare(b.poste)<0;
        return a.nom.localeAwareCompare(b.nom)<0;
    });
    for(const Employe &e:sorted){
        const QString all=e.code()+" "+e.nom+" "+e.prenom+" "+e.poste+" "+QString::number(e.salaire,'f',3);
        if(!queryText.isEmpty()&&!all.toLower().contains(queryText)) continue;
        const int row=m_table->rowCount();
        m_table->insertRow(row);
        auto *check=new QTableWidgetItem;
        check->setCheckState(Qt::Unchecked);
        m_table->setItem(row,0,check);
        m_table->setItem(row,1,new QTableWidgetItem(e.code()));
        m_table->setItem(row,2,new QTableWidgetItem(e.nom));
        m_table->setItem(row,3,new QTableWidgetItem(e.prenom));
        m_table->setItem(row,4,new QTableWidgetItem(e.poste));
        m_table->setItem(row,5,new QTableWidgetItem(QString::number(e.salaire,'f',3)+" DT"));
        const bool present = e.etatPresence.compare(QStringLiteral("PRESENT"), Qt::CaseInsensitive) == 0;
        auto *presence = new QTableWidgetItem(present ? "Présent" : "Absent");
        presence->setForeground(present ? QColor("#07823D") : QColor("#778899"));
        m_table->setItem(row,6,presence);
        m_table->setItem(row,7,new QTableWidgetItem("View / Edit"));
        m_table->setRowHeight(row,42);
    }
}
void MainWindow::employeeLoginFlow()
{
    if (m_sessionEmployeeId > 0)
        return;

    LoginDialog login(this);
    login.setModal(true);
    if (login.exec() != QDialog::Accepted) {
        QApplication::quit();
        return;
    }

    const Employe employe = login.employeConnecte();
    QString error;
    if (!Employe::changerPresence(employe.id, true, &error)) {
        QMessageBox::critical(this, "Presence", "Impossible d'enregistrer la presence :\n" + error);
        QApplication::quit();
        return;
    }

    m_sessionEmployeeId = employe.id;
    if (loadEmployees()) {
        refresh();
        updateStats();
    }
    m_presenceLabel->setText(QString("Present : %1 %2").arg(employe.nom, employe.prenom));
    show();
    raise();
    activateWindow();
}

void MainWindow::logoutEmployee()
{
    if (m_sessionEmployeeId <= 0)
        return;

    QString error;
    if (!Employe::changerPresence(m_sessionEmployeeId, false, &error)) {
        QMessageBox::critical(this, "Deconnexion", "Impossible d'enregistrer l'absence :\n" + error);
        return;
    }

    m_sessionEmployeeId = 0;
    m_presenceLabel->setText("Aucun employe connecte");
    if (loadEmployees()) {
        refresh();
        updateStats();
    }
    hide();
    employeeLoginFlow();
}
void MainWindow::closeEvent(QCloseEvent *event)
{
    if (m_sessionEmployeeId > 0) {
        QString error;
        Employe::changerPresence(m_sessionEmployeeId, false, &error);
        m_sessionEmployeeId = 0;
    }
    QMainWindow::closeEvent(event);
}
void MainWindow::updateStats(){m_stats->update();m_stats->repaint();}
int MainWindow::selectedIndex()const{
    auto findEmployee=[this](const QString &id){for(int i=0;i<m_employees.size();++i)if(m_employees[i].code()==id)return i;return -1;};
    const auto ranges=m_table->selectedRanges();
    if(!ranges.isEmpty()){
        const int row=ranges.first().topRow();
        if(m_table->item(row,1))return findEmployee(m_table->item(row,1)->text());
    }
    // The first table column is a checkbox; support selecting a row through it too.
    for(int row=0;row<m_table->rowCount();++row){
        const auto *check=m_table->item(row,0);
        if(check&&check->checkState()==Qt::Checked&&m_table->item(row,1))return findEmployee(m_table->item(row,1)->text());
    }
    return -1;
}
void MainWindow::fillForm(const Employe&e){m_id->setText(e.code());m_nom->setText(e.nom);m_prenom->setText(e.prenom);m_poste->setCurrentText(e.poste);m_salary->setText(QString::number(e.salaire,'f',3));m_password->setText(e.motDePasse);}
Employe MainWindow::formEmployee()const{Employe e;e.nom=m_nom->text().trimmed();e.prenom=m_prenom->text().trimmed();e.poste=m_poste->currentText();e.salaire=m_salary->text().trimmed().toDouble();e.motDePasse=m_password->text();return e;}
static bool validPersonName(const QString &value){
    static const QRegularExpression pattern(QStringLiteral("^[\\p{L}]+(?:[ '\\-][\\p{L}]+)*$"));
    return pattern.match(value.trimmed()).hasMatch();
}
static bool parseSalary(const QString &text,double *value){
    QString normalized=text.trimmed();normalized.replace(',', '.');
    bool ok=false;const double parsed=normalized.toDouble(&ok);
    if(value)*value=parsed;
    return ok;
}
void MainWindow::clearForm(){
    m_table->clearSelection();
    m_id->clear();m_nom->clear();m_prenom->clear();m_salary->clear();m_password->clear();
    m_poste->setCurrentIndex(0);
    m_nom->setFocus();
}
void MainWindow::rowClicked(int row,int){if(row<0||!m_table->item(row,1))return;QString id=m_table->item(row,1)->text();for(const auto&e:m_employees)if(e.code()==id){fillForm(e);break;}}
void MainWindow::addEmployee(){
    Employe e=formEmployee();
    if(!validPersonName(e.nom)||!validPersonName(e.prenom)||e.motDePasse.isEmpty()){
        QMessageBox::warning(this,"Ajouter","Saisissez un nom et un prénom valides (lettres uniquement) ainsi qu'un mot de passe.");return;
    }
    bool salaryOk=false;double salary=0;salaryOk=parseSalary(m_salary->text(),&salary);
    if(!salaryOk||salary<0||salary>9999999.999){QMessageBox::warning(this,"Ajouter","Le salaire doit être compris entre 0 et 9 999 999,999 DT.");return;}
    e.salaire=salary;QString erreur;
    if(!e.ajouter(&erreur)){QMessageBox::critical(this,"Ajouter","Oracle a refusé l'ajout :\n"+erreur);return;}
    loadEmployees();refresh();updateStats();clearForm();
}
void MainWindow::modifyEmployee(){
    int i=selectedIndex();if(i<0){QMessageBox::information(this,"Modifier","Sélectionnez un employé.");return;}
    Employe e=formEmployee();e.id=m_employees[i].id;
    if(!validPersonName(e.nom)||!validPersonName(e.prenom)||e.motDePasse.isEmpty()){QMessageBox::warning(this,"Modifier","Saisissez un nom et un prénom valides (lettres uniquement) ainsi qu'un mot de passe.");return;}
    bool salaryOk=false;double salary=0;salaryOk=parseSalary(m_salary->text(),&salary);
    if(!salaryOk||salary<0||salary>9999999.999){QMessageBox::warning(this,"Modifier","Le salaire doit être compris entre 0 et 9 999 999,999 DT.");return;}
    e.salaire=salary;QString erreur;
    if(!e.modifier(&erreur)){QMessageBox::critical(this,"Modifier","Oracle a refusé la modification :\n"+erreur);return;}
    loadEmployees();refresh();updateStats();
}
void MainWindow::removeEmployee(){
    int i=selectedIndex();if(i<0)return;
    if(QMessageBox::question(this,"Supprimer","Supprimer cet employé ?")==QMessageBox::Yes){
        QString erreur;
        if(!Employe::supprimer(m_employees[i].id,&erreur)){QMessageBox::critical(this,"Supprimer","Oracle a refusé la suppression :\n"+erreur);return;}
        if (loadEmployees()) { refresh(); updateStats(); clearForm(); }
    }
}
void MainWindow::showEmployee(){
    const int index=selectedIndex();
    if(index<0) return;
    const Employe &e=m_employees[index];
    QMessageBox::information(this,"Fiche employe",
        QString("ID : %1\nNom : %2 %3\nPoste : %4\nSalaire : %5 DT")
            .arg(e.code(),e.nom,e.prenom,e.poste,QString::number(e.salaire,'f',3)));
}
void MainWindow::searchChanged(const QString&){QLineEdit *source=qobject_cast<QLineEdit*>(sender());QLineEdit *target=source==m_search?m_listSearch:m_search;if(target->text()!=source->text()){const QSignalBlocker blocker(target);target->setText(source->text());}refresh();}
QString MainWindow::readText(const QString &path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return {};
    QTextStream stream(&file);
    return stream.readAll();
}

static QString normalizedCvLabel(QString value)
{
    value = value.normalized(QString::NormalizationForm_D);
    QString result;
    for (const QChar ch : value) {
        if (ch.category() == QChar::Mark_NonSpacing || ch.category() == QChar::Mark_SpacingCombining)
            continue;
        if (ch.isLetterOrNumber())
            result += ch.toLower();
    }
    return result;
}

int MainWindow::parseCV(const QString &text)
{
    QMap<QString, QString> fields;
    const QRegularExpression linePattern(QStringLiteral("^\\s*([^:=;\\t,]{2,35})\\s*[:=;\\t,]\\s*(.+?)\\s*$"));
    for (const QString &line : text.split(QRegularExpression(QStringLiteral("[\\r\\n]+")), Qt::SkipEmptyParts)) {
        const auto match = linePattern.match(line);
        if (!match.hasMatch())
            continue;
        const QString key = normalizedCvLabel(match.captured(1));
        const QString value = match.captured(2).trimmed();
        if (!key.isEmpty() && !value.isEmpty() && !fields.contains(key))
            fields.insert(key, value);
    }

    auto valueFor = [&fields](const QStringList &aliases) {
        for (const QString &alias : aliases) {
            const QString key = normalizedCvLabel(alias);
            if (fields.contains(key))
                return fields.value(key);
        }
        return QString();
    };

    QString familyName = valueFor({"nom", "nom de famille", "surname", "last name", "family name"});
    QString givenName = valueFor({"prenom", "first name", "given name"});
    QString fullName = valueFor({"nom complet", "full name", "name"});
    if (fullName.isEmpty() && (familyName.isEmpty() || givenName.isEmpty())) {
        for (const QString &line : text.split(QRegularExpression(QStringLiteral("[\\r\\n]+")), Qt::SkipEmptyParts)) {
            const QString candidate = line.trimmed();
            const QString normalized = normalizedCvLabel(candidate);
            static const QRegularExpression nameLine(QStringLiteral("^[\\p{L}][\\p{L}'’ -]{2,79}$"));
            if (nameLine.match(candidate).hasMatch() && normalized != QStringLiteral("cv") &&
                normalized != QStringLiteral("curriculumvitae") && normalized != QStringLiteral("resume") &&
                normalized != QStringLiteral("profil") && normalized != QStringLiteral("profile")) {
                fullName = candidate;
                break;
            }
        }
    }
    if ((familyName.isEmpty() || givenName.isEmpty()) && !fullName.isEmpty()) {
        QStringList parts = fullName.split(QRegularExpression(QStringLiteral("\\s+")), Qt::SkipEmptyParts);
        if (parts.size() >= 2) {
            if (givenName.isEmpty())
                givenName = parts.takeFirst();
            if (familyName.isEmpty())
                familyName = parts.join(QLatin1Char(' '));
        }
    }

    int filled = 0;
    auto setIfPresent = [&filled](QLineEdit *edit, const QString &value) {
        if (!value.trimmed().isEmpty()) {
            edit->setText(value.trimmed());
            ++filled;
        }
    };
    setIfPresent(m_nom, familyName);
    setIfPresent(m_prenom, givenName);

    const QString position = valueFor({"poste", "fonction", "titre", "titre du poste", "metier", "profession", "position", "job title", "occupation"});
    if (!position.isEmpty()) {
        int index = -1;
        for (int i = 0; i < m_poste->count(); ++i) {
            if (m_poste->itemText(i).compare(position, Qt::CaseInsensitive) == 0) {
                index = i;
                break;
            }
        }
        if (index < 0) {
            m_poste->addItem(position);
            index = m_poste->count() - 1;
        }
        m_poste->setCurrentIndex(index);
        ++filled;
    }

    const QString salary = valueFor({"salaire", "pretentions salariales", "salary", "expected salary", "remuneration"});
    if (!salary.isEmpty()) {
        const auto amount = QRegularExpression(QStringLiteral("[0-9][0-9 ,.'’]*")).match(salary);
        if (amount.hasMatch()) {
            QString number = amount.captured().trimmed();
            number.remove(QLatin1Char(' '));
            number.remove(QLatin1Char('\''));
            number.remove(QChar(0x2019));
            if (number.contains(QLatin1Char(',')) && !number.contains(QLatin1Char('.')))
                number.replace(QLatin1Char(','), QLatin1Char('.'));
            m_salary->setText(number);
            ++filled;
        }
    }
    return filled;
}

void MainWindow::importCV()
{
    const QString path = QFileDialog::getOpenFileName(
        this, "Choisir un CV", {}, "CV (*.pdf *.txt *.csv);;PDF (*.pdf);;Texte/CSV (*.txt *.csv)");
    if (path.isEmpty())
        return;

    QString text;
    if (QFileInfo(path).suffix().compare(QStringLiteral("pdf"), Qt::CaseInsensitive) == 0) {
        QPdfDocument document;
        if (document.load(path) != QPdfDocument::Error::None) {
            QMessageBox::warning(this, "CV", "Impossible d'ouvrir ce PDF. S'il est scanne, il faut un PDF contenant du texte.");
            return;
        }
        for (int page = 0; page < document.pageCount(); ++page)
            text += document.getAllText(page).text() + QLatin1Char('\n');
    } else {
        text = readText(path);
    }

    if (text.trimmed().isEmpty()) {
        QMessageBox::warning(this, "CV", "Aucun texte lisible trouve dans ce CV. Les PDF scannes ne sont pas pris en charge.");
        return;
    }

    clearForm();
    const int filled = parseCV(text);
    if (filled == 0) {
        QMessageBox::information(this, "CV", "Aucun champ reconnu. Utilisez des libelles comme Nom :, Prenom :, Poste : ou Salaire :.");
        return;
    }
    QMessageBox::information(this, "CV importe",
        QString("%1 champ(s) rempli(s). L'ID sera genere lors de l'ajout. Saisissez le mot de passe de l'employe, puis verifiez les champs avant Ajouter.").arg(filled));
}
