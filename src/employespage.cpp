#include "employespage.h"
#include "logindialog.h"
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QTableWidget>
#include <QHeaderView>
#include <QLineEdit>
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
#include <QPainter>
#include <QPainterPath>
#include <QScrollBar>
#include <QScrollArea>

class StatsWidget : public QWidget {
public:
    QList<Employee> *data=nullptr;
    explicit StatsWidget(QWidget *p=nullptr):QWidget(p){setMinimumHeight(310); setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Preferred);}
protected:
    void paintEvent(QPaintEvent*) override {
        QPainter p(this); p.setRenderHint(QPainter::Antialiasing);
        p.setPen(QColor("#0B3B67"));
        int W=width(), H=height(), gap=12, cardW=(W-2*gap)/3;
        for(int c=0;c<3;c++){
            QRect r(c*(cardW+gap),0,cardW,H);
            p.setBrush(Qt::white); p.setPen(QColor("#E6DDD0")); p.drawRoundedRect(r,10,10);
        }
        QFont title=p.font(); title.setBold(true); title.setPointSize(11); p.setFont(title);
        p.setPen(QColor("#0B3B67"));
        p.drawText(15,28,"Répartition par poste (Présents)");
        p.drawText(cardW+gap+15,28,"Évolution des présences (7 derniers jours)");
        p.drawText(2*(cardW+gap)+15,28,"Top 5 des employés présents");

        int present=0; QMap<QString,int> by;
        if(data) for(const auto &e:*data) if(e.present){present++; by[e.poste]++;}
        QPoint center(105,142); int rad=62; QRectF pie(center.x()-rad,center.y()-rad,rad*2,rad*2);
        QList<QColor> colors={QColor("#0B3B67"),QColor("#3E96D2"),QColor("#8BD0E8"),QColor("#E6A84E")};
        double start=0; int k=0; int total=qMax(1,present);
        for(auto it=by.begin();it!=by.end();++it){
            double span=360.0*it.value()/total;
            p.setBrush(colors[k%colors.size()]); p.setPen(Qt::NoPen);
            p.drawPie(pie,start*16,span*16); start+=span; k++;
        }
        p.setBrush(Qt::white); p.drawEllipse(center,35,35);
        p.setPen(QColor("#0B3B67")); QFont big=p.font(); big.setPointSize(18); big.setBold(true); p.setFont(big);
        p.drawText(QRect(center.x()-35,center.y()-5,70,25),Qt::AlignCenter,QString::number(present));
        QFont sm=p.font(); sm.setPointSize(8); sm.setBold(false); p.setFont(sm);
        p.drawText(QRect(center.x()-45,center.y()+16,90,18),Qt::AlignCenter,"employés");
        int y=78; k=0; for(auto it=by.begin();it!=by.end() && k<5;++it,++k){
            p.setBrush(colors[k%colors.size()]); p.drawEllipse(cardW-155,y-8,14,14);
            p.setPen(QColor("#0B3B67")); p.drawText(cardW-132,y+4,it.key());
            p.drawText(cardW-42,y+4,QString::number(it.value())+" ("+QString::number(it.value()*100/qMax(1,present))+"%)");
            y+=30;
        }
        // Line chart
        int x0=cardW+gap+55, y0=H-40, chartW=cardW-85, chartH=H-90;
        p.setPen(QColor("#D5DEE7")); for(int i=0;i<5;i++){int yy=y0-i*chartH/4;p.drawLine(x0,yy,x0+chartW,yy);}
        QList<int> vals={700,1450,2050,2500,3050,3600};
        p.setPen(QColor("#0B3B67")); p.setBrush(QColor("#DCEAF5"));
        QPolygon poly; poly<<QPoint(x0,y0);
        for(int i=0;i<vals.size();i++){int xx=x0+i*chartW/(vals.size()-1); int yy=y0-vals[i]*chartH/4000; poly<<QPoint(xx,yy);}
        poly<<QPoint(x0+chartW,y0); p.drawPolygon(poly);
        p.setBrush(QColor("#0B3B67")); p.setPen(QColor("#0B3B67"));
        for(int i=0;i<vals.size();i++){int xx=x0+i*chartW/(vals.size()-1);int yy=y0-vals[i]*chartH/4000;p.drawEllipse(xx-4,yy-4,8,8);}
        QStringList months={"Jan","Fév","Mar","Avr","Mai","Juin"};
        for(int i=0;i<months.size();i++){int xx=x0+i*chartW/(months.size()-1);p.drawText(xx-12,y0+20,months[i]);}
        // top 5
        QStringList names; if(data) for(const auto&e:*data) if(e.present) names<<e.nom+" "+e.prenom;
        int yy=72; for(int i=0;i<qMin(5,names.size());i++){p.setPen(QColor("#0B3B67"));p.drawText(2*(cardW+gap)+15,yy,names[i]);int bx=2*(cardW+gap)+150;p.setBrush(QColor("#17639A"));p.setPen(Qt::NoPen);p.drawRoundedRect(bx,yy-13,cardW-190,16,8,8);p.setPen(QColor("#0B3B67"));p.drawText(2*(cardW+gap)+cardW-48,yy,"100%");yy+=34;}
    }
};

static QString styleField(){return "QLineEdit,QComboBox{padding:8px;border:1px solid #D8DEE5;border-radius:7px;background:white;color:#0B3B67;}";}
static QString styleButton(const QString &bg,const QString &fg="white"){return QString("QPushButton{background:%1;color:%2;border:0;border-radius:7px;padding:10px;font-weight:700;}").arg(bg,fg);}

EmployesPage::EmployesPage(QWidget *p):QWidget(p){
    m_employees={
        {"EMP001","Ben Ali","Ahmed","Manager","3500","1234",true,QDateTime::currentDateTime()},
        {"EMP003","Trabelsi","Youssef","Designer","3000","1234",true,QDateTime::currentDateTime()},
        {"EMP004","Hmidi","Rania","Couturière","2200","1234",true,QDateTime::currentDateTime()},
        {"EMP005","Jaziri","Omar","Technicien","2500","1234",true,QDateTime::currentDateTime()}
    };
    buildUi(); refresh(); updateStats();
}
void EmployesPage::buildUi(){
    // Le menu de gauche (image sidebar.png) et la barre du haut (☰, recherche, utilisateur)
    // de l'ancien projet sont remplacés par ceux de l'application intégrée.
    auto *rv=new QVBoxLayout(this);rv->setContentsMargins(0,0,0,0);rv->setSpacing(10);
    QLabel *banner=new QLabel;banner->setPixmap(QPixmap(":/assets/banner.png"));banner->setScaledContents(true);banner->setMinimumHeight(110);rv->addWidget(banner);

    QWidget *body=new QWidget; body->setStyleSheet("background:#F8F5EF;"); auto *bv=new QVBoxLayout(body);bv->setContentsMargins(12,0,12,0);bv->setSpacing(10);
    auto *split=new QSplitter(Qt::Horizontal);split->setChildrenCollapsible(false);
    QGroupBox *formBox=new QGroupBox("👤  Informations de l'employé");formBox->setStyleSheet("QGroupBox{background:white;border:1px solid #E4DBCF;border-radius:10px;margin-top:28px;padding-top:22px;font-size:17px;font-weight:700;color:#0B3B67;}QGroupBox::title{subcontrol-origin:margin;top:-1px;left:12px;padding:2px 8px;color:#0B3B67;background:#F8F5EF;}");
    auto *g=new QGridLayout(formBox);g->setContentsMargins(14,12,14,12);g->setVerticalSpacing(8);
    m_id=new QLineEdit;m_nom=new QLineEdit;m_prenom=new QLineEdit;m_salary=new QLineEdit;m_password=new QLineEdit;m_password->setEchoMode(QLineEdit::Password);
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
    auto *toprow=new QHBoxLayout;m_search=new QLineEdit;m_search->setPlaceholderText("Rechercher un employé...");m_search->setStyleSheet(styleField());QComboBox*sort=new QComboBox;sort->addItems({"Nom","ID","Poste","État"});sort->setStyleSheet(styleField());toprow->addWidget(m_search,2);toprow->addWidget(new QLabel("Trier par :"));toprow->addWidget(sort);lv->addLayout(toprow);
    m_table=new QTableWidget(0,8);m_table->setHorizontalHeaderLabels({"","ID","Nom","Prénom","Poste","Salaire","État","Actions"});m_table->setSelectionBehavior(QAbstractItemView::SelectRows);m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);m_table->verticalHeader()->setVisible(false);m_table->horizontalHeader()->setStretchLastSection(true);m_table->setColumnWidth(0,38);m_table->setColumnWidth(1,88);m_table->setColumnWidth(2,105);m_table->setColumnWidth(3,105);m_table->setColumnWidth(4,110);m_table->setColumnWidth(5,95);m_table->setColumnWidth(6,105);m_table->setColumnWidth(7,95);m_table->setStyleSheet("QTableWidget{background:white;color:#0B3B67;gridline-color:#E2E5E8;alternate-background-color:#FFFFFF;}QHeaderView::section{background:#F7EBD8;color:#0B3B67;font-weight:700;padding:9px;border:0;}");lv->addWidget(m_table,1);
    auto*loginrow=new QHBoxLayout;m_status=new QLabel("● Aucun employé connecté");m_status->setStyleSheet("background:#DDF5E5;color:#08783E;padding:9px;border-radius:7px;font-weight:700;");QPushButton*loginb=new QPushButton("Connexion employé");loginb->setStyleSheet(styleButton("#0B3B67"));QPushButton*logoutb=new QPushButton("Déconnexion");logoutb->setStyleSheet(styleButton("#D99A45"));loginrow->addWidget(m_status,1);loginrow->addWidget(loginb);loginrow->addWidget(logoutb);lv->addLayout(loginrow);
    split->addWidget(listBox);split->setSizes({590,890});bv->addWidget(split,3);

    QGroupBox*statsBox=new QGroupBox("📊  Statistiques des employés   (Présents uniquement)");statsBox->setStyleSheet(formBox->styleSheet());auto*sv=new QVBoxLayout(statsBox);sv->setContentsMargins(8,12,8,10);m_stats=new StatsWidget; m_stats->data=&m_employees;sv->addWidget(m_stats);statsBox->setMinimumHeight(350);bv->addWidget(statsBox,2);
    QScrollArea *scroll=new QScrollArea;
    scroll->setWidget(body);
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);
    scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scroll->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scroll->setStyleSheet("QScrollArea{background:#171717;border:0;} QScrollBar:vertical{width:14px;background:#252525;margin:0;} QScrollBar::handle:vertical{background:#888;border-radius:7px;min-height:55px;} QScrollBar::add-line:vertical,QScrollBar::sub-line:vertical{height:0;} QScrollBar::add-page:vertical,QScrollBar::sub-page:vertical{background:#252525;}");
    rv->addWidget(scroll,1);

    connect(cv,&QPushButton::clicked,this,&EmployesPage::importCV);connect(a,&QPushButton::clicked,this,&EmployesPage::addEmployee);connect(ed,&QPushButton::clicked,this,&EmployesPage::modifyEmployee);connect(del,&QPushButton::clicked,this,&EmployesPage::removeEmployee);connect(show,&QPushButton::clicked,this,&EmployesPage::showEmployee);connect(loginb,&QPushButton::clicked,this,&EmployesPage::login);connect(logoutb,&QPushButton::clicked,this,&EmployesPage::logout);connect(m_search,&QLineEdit::textChanged,this,&EmployesPage::searchChanged);connect(m_table,&QTableWidget::cellClicked,this,&EmployesPage::rowClicked);
}
void EmployesPage::refresh(){
    m_table->setRowCount(0);QString q=m_search->text().trimmed().toLower();
    for(const auto&e:m_employees){QString all=e.id+" "+e.nom+" "+e.prenom+" "+e.poste;if(!q.isEmpty()&&!all.toLower().contains(q))continue;int r=m_table->rowCount();m_table->insertRow(r);auto*c=new QTableWidgetItem; c->setCheckState(Qt::Unchecked);m_table->setItem(r,0,c);m_table->setItem(r,1,new QTableWidgetItem(e.id));m_table->setItem(r,2,new QTableWidgetItem(e.nom));m_table->setItem(r,3,new QTableWidgetItem(e.prenom));m_table->setItem(r,4,new QTableWidgetItem(e.poste));m_table->setItem(r,5,new QTableWidgetItem(e.salaire+" DT"));auto*st=new QTableWidgetItem(e.present?"● Présent":"○ Absent");st->setForeground(e.present?QBrush(QColor("#138A4B")):QBrush(QColor("#777")));m_table->setItem(r,6,st);m_table->setItem(r,7,new QTableWidgetItem("◉  ✎"));m_table->setRowHeight(r,42);}
}
void EmployesPage::updateStats(){int p=0;for(const auto&e:m_employees)if(e.present)p++;m_stats->update();m_stats->repaint();}
int EmployesPage::selectedIndex()const{auto rs=m_table->selectedRanges();if(rs.isEmpty())return -1;QString id=m_table->item(rs.first().topRow(),1)->text();for(int i=0;i<m_employees.size();i++)if(m_employees[i].id==id)return i;return -1;}
void EmployesPage::fillForm(const Employee&e){m_id->setText(e.id);m_nom->setText(e.nom);m_prenom->setText(e.prenom);m_poste->setCurrentText(e.poste);m_salary->setText(e.salaire);m_password->setText(e.password);}
Employee EmployesPage::formEmployee()const{Employee e;e.id=m_id->text().trimmed().toUpper();e.nom=m_nom->text().trimmed();e.prenom=m_prenom->text().trimmed();e.poste=m_poste->currentText();e.salaire=m_salary->text().trimmed();e.password=m_password->text();return e;}
void EmployesPage::rowClicked(int row,int){QString id=m_table->item(row,1)->text();for(const auto&e:m_employees)if(e.id==id){fillForm(e);break;}}
void EmployesPage::addEmployee(){Employee e=formEmployee();if(e.id.isEmpty()||e.nom.isEmpty()||e.prenom.isEmpty()){QMessageBox::warning(this,"Ajouter","ID, nom et prénom sont obligatoires.");return;}for(const auto&x:m_employees)if(x.id==e.id){QMessageBox::warning(this,"Ajouter","ID déjà utilisé.");return;}m_employees.append(e);refresh();updateStats();}
void EmployesPage::modifyEmployee(){int i=selectedIndex();if(i<0){QMessageBox::information(this,"Modifier","Sélectionnez un employé.");return;}Employee e=formEmployee();e.present=m_employees[i].present;e.lastLogin=m_employees[i].lastLogin;m_employees[i]=e;refresh();updateStats();}
void EmployesPage::removeEmployee(){int i=selectedIndex();if(i<0)return;if(QMessageBox::question(this,"Supprimer","Supprimer cet employé ?")==QMessageBox::Yes){m_employees.removeAt(i);refresh();updateStats();}}
void EmployesPage::showEmployee(){int i=selectedIndex();if(i<0)return;auto&e=m_employees[i];QMessageBox::information(this,"Fiche employé",QString("ID : %1\nNom : %2 %3\nPoste : %4\nSalaire : %5 DT\nÉtat : %6").arg(e.id,e.nom,e.prenom,e.poste,e.salaire,e.present?"Présent":"Absent"));}
void EmployesPage::login(){LoginDialog d(this);if(d.exec()!=QDialog::Accepted)return;int i=-1;for(int k=0;k<m_employees.size();k++)if(m_employees[k].id==d.id()){i=k;break;}if(i<0){QMessageBox::warning(this,"Connexion","Employé introuvable.");return;}if(m_employees[i].password!=d.password()){QMessageBox::warning(this,"Connexion","Mot de passe incorrect.");return;}m_employees[i].present=true;m_employees[i].lastLogin=QDateTime::currentDateTime();m_loggedId=d.id();m_status->setText("● Présent : "+m_employees[i].nom+" "+m_employees[i].prenom);refresh();updateStats();}
void EmployesPage::logout(){if(m_loggedId.isEmpty())return;for(auto&e:m_employees)if(e.id==m_loggedId)e.present=false;m_loggedId.clear();m_status->setText("● Aucun employé connecté");refresh();updateStats();}
void EmployesPage::searchChanged(const QString&t){refresh();emit searchTextChanged(t);}
QString EmployesPage::searchText()const{return m_search->text();}
void EmployesPage::setSearchText(const QString&t){if(m_search->text()!=t)m_search->setText(t);}
QString EmployesPage::readText(const QString&path){QFile f(path);if(!f.open(QIODevice::ReadOnly|QIODevice::Text))return{};QTextStream t(&f);return t.readAll();}
void EmployesPage::parseCV(const QString&t){auto get=[&](const QStringList&ks){for(const auto&line:t.split(QRegularExpression("[\\r\\n]+"))){for(const auto&k:ks){QRegularExpression re("^\\s*"+QRegularExpression::escape(k)+"\\s*[:\\-]\\s*(.+)$",QRegularExpression::CaseInsensitiveOption);auto m=re.match(line);if(m.hasMatch())return m.captured(1).trimmed();}}return QString();};QString v=get({"ID","ID Employé"});if(!v.isEmpty())m_id->setText(v.toUpper());v=get({"Nom"});if(!v.isEmpty())m_nom->setText(v);v=get({"Prénom","Prenom"});if(!v.isEmpty())m_prenom->setText(v);v=get({"Poste","Fonction"});if(!v.isEmpty())m_poste->setCurrentText(v);v=get({"Salaire","Salary"});if(!v.isEmpty())m_salary->setText(v);}
void EmployesPage::importCV(){QString p=QFileDialog::getOpenFileName(this,"Choisir un CV",{}, "CV texte (*.txt *.csv);Tous les fichiers (*.*)");if(p.isEmpty())return;QString t=readText(p);if(t.isEmpty()){QMessageBox::warning(this,"CV","Impossible de lire le fichier.");return;}parseCV(t);QMessageBox::information(this,"CV importé","Les informations ont été placées dans le formulaire. Vérifiez puis cliquez sur Ajouter.");}
