#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "widgets.h"
#include <QPdfWriter>
#include <QTextDocument>
#include <QPageSize>
#include <QRegularExpression>
#include <algorithm>

// true = interface interactive (boutons, saisie, tri...) ; false = affichage seul (maquette statique)
static const bool INTERACTIF=true;

// Miniature carrée (mise en cache) affichée devant l'article dans la liste
static QIcon thumb(const QString&p){
    static QHash<QString,QIcon> cache;
    if(p.isEmpty())return QIcon();
    if(cache.contains(p))return cache[p];
    QPixmap pm(p);QIcon ic;
    if(!pm.isNull()){pm=pm.scaled(46,46,Qt::KeepAspectRatioByExpanding,Qt::SmoothTransformation);
        ic=QIcon(pm.copy((pm.width()-46)/2,(pm.height()-46)/2,46,46));}
    cache[p]=ic;return ic;}

// Galerie d'images : affiche les images de l'application (resources.qrc -> :/images) sous forme de miniatures.
// Aucune base de données n'est nécessaire. Le bouton « Parcourir… » permet de prendre une image ailleurs sur le PC.
static QString pickImage(QWidget*parent){
    QDialog d(parent);d.setWindowTitle("Choisir l'image de l'article");d.resize(720,480);
    auto*lay=new QVBoxLayout(&d);
    auto*lst=new QListWidget;
    lst->setViewMode(QListView::IconMode);lst->setIconSize(QSize(120,120));lst->setResizeMode(QListView::Adjust);
    lst->setMovement(QListView::Static);lst->setSpacing(10);lst->setSelectionMode(QAbstractItemView::SingleSelection);
    const QStringList fl=QDir(":/images").entryList({"*.png","*.jpg","*.jpeg","*.bmp","*.webp"},QDir::Files,QDir::Name);
    for(const QString&n:fl){
        QString p=":/images/"+n;
        auto*it=new QListWidgetItem(QIcon(p),QFileInfo(n).baseName());
        it->setData(Qt::UserRole,p);lst->addItem(it);}
    lay->addWidget(lst,1);
    auto*bb=new QDialogButtonBox(QDialogButtonBox::Ok|QDialogButtonBox::Cancel);
    auto*br=bb->addButton("Parcourir…",QDialogButtonBox::ActionRole);
    lay->addWidget(bb);
    QString res;
    QObject::connect(bb,&QDialogButtonBox::accepted,&d,&QDialog::accept);
    QObject::connect(bb,&QDialogButtonBox::rejected,&d,&QDialog::reject);
    QObject::connect(lst,&QListWidget::itemDoubleClicked,&d,&QDialog::accept);
    QObject::connect(br,&QPushButton::clicked,&d,[&]{
        QString f=QFileDialog::getOpenFileName(&d,"Choisir une image",QString(),"Images (*.png *.jpg *.jpeg *.bmp *.webp)");
        if(!f.isEmpty()){res=f;d.accept();}});
    if(d.exec()!=QDialog::Accepted)return QString();
    if(!res.isEmpty())return res;
    auto*it=lst->currentItem();return it?it->data(Qt::UserRole).toString():QString();
}

MainWindow::MainWindow():ui(new Ui::MainWindow){
    ui->setupUi(this);   // toute l'interface vient de mainwindow.ui

    {auto g=QGuiApplication::primaryScreen()->availableGeometry();
     resize(int(g.width()*0.94),int(g.height()*0.92));move(g.center()-rect().center());}
    setMinimumSize(1000,620);

    // Widgets personnalisés (promus dans Designer)
    ui->banner->setTexts("Gestion des Articles","Du stock à la vente",{"Un stock bien tenu,","c'est une vente","qui se prépare."});
    // Barre latérale : boutons cochables exclusifs + icônes QPainter (blanc / bleu nuit si coché)
    {auto*grp=new QButtonGroup(this);grp->setExclusive(true);
     QPushButton*bt[6]={ui->btnClients,ui->btnEmployes,ui->btnCommandes,ui->btnMaquettes,ui->btnMachines,ui->btnArticles};
     for(int i=0;i<6;++i){bt[i]->setCheckable(true);bt[i]->setIcon(sideIcon(i));bt[i]->setIconSize(QSize(20,20));
        bt[i]->setCursor(Qt::PointingHandCursor);grp->addButton(bt[i],i);}
     ui->btnArticles->setChecked(true);

     // ---- Navigation : chaque module de la barre latérale ouvre sa propre page ----
     // On sort la bannière + le corps (formulaire/liste/stats) de rightLayout pour les ranger
     // dans la page « Articles » d'un QStackedWidget ; les autres modules ont leur page vide.
     ui->rightLayout->removeWidget(ui->banner);
     ui->rightLayout->removeWidget(ui->body);
     auto*stack=new QStackedWidget(ui->centralwidget);
     ui->rightLayout->addWidget(stack,1);

     // titre, sous-titre, citation de la bannière pour chaque module (index = id du bouton)
     struct Mod{const char*t;const char*s;QStringList q;};
     const Mod mods[6]={
        {"Gestion des Clients","Relation client",{"Un client satisfait,","c'est une cliente","qui revient."}},
        {"Gestion des Employés","Équipe & planning",{"Une équipe soudée,","c'est une collection","qui se réussit."}},
        {"Gestion des Commandes","Du devis à la livraison",{"Chaque commande,","est une promesse","à tenir."}},
        {"Gestion des Maquettes","De l'idée au patron",{"Un beau modèle","commence par","un beau croquis."}},
        {"Gestion des Machines","Parc & maintenance",{"Des machines en forme,","c'est une production","sans surprise."}},
        {"","",{}}};   // 5 = Articles (page existante)

     for(int i=0;i<6;++i){
        auto*page=new QWidget;auto*lay=new QVBoxLayout(page);
        lay->setContentsMargins(0,0,0,0);lay->setSpacing(0);
        if(i==5){                       // page Articles = bannière + corps déjà dessinés dans Designer
            // Le corps (formulaire + liste + stats) est placé dans une zone défilante : sur un petit écran
            // il garde sa hauteur minimale (rien n'est écrasé) et une barre de défilement apparaît.
            auto*sc=new QScrollArea;sc->setFrameShape(QFrame::NoFrame);sc->setWidgetResizable(true);
            sc->viewport()->setObjectName("scvp");sc->setStyleSheet("#scvp{background:#f7f1e8}");
            ui->body->setMinimumHeight(720);
            sc->setWidget(ui->body);
            lay->addWidget(ui->banner);lay->addWidget(sc,1);
        }else{                          // autres modules : bannière + zone à remplir
            auto*bn=new BannerWidget;bn->setTexts(mods[i].t,mods[i].s,mods[i].q);
            auto*body=new QFrame;body->setObjectName("body");   // même fond que les Articles
            auto*bl=new QVBoxLayout(body);
            auto*lb=new QLabel(QString("Module « %1 » — page en cours de construction").arg(bt[i]->text()));
            lb->setAlignment(Qt::AlignCenter);lb->setStyleSheet("font:600 16pt Georgia;color:#9aa3b0");
            bl->addWidget(lb);
            lay->addWidget(bn);lay->addWidget(body,1);
        }
        stack->addWidget(page);         // index du stack == index du bouton
     }
     stack->setCurrentIndex(5);          // démarrage sur Articles
     connect(grp,&QButtonGroup::idClicked,stack,&QStackedWidget::setCurrentIndex);
    }
    ui->chDonut->setMode(ChartWidget::Donut);
    ui->chLine->setMode(ChartWidget::Line);
    ui->chBars->setMode(ChartWidget::Bars);

    ui->dDate->setDate(QDate::currentDate());
    ui->cSort->setCurrentIndex(6);   // par défaut : articles classés selon la demande
    ui->tb->horizontalHeader()->setSectionResizeMode(2,QHeaderView::Stretch);
    ui->tb->setIconSize(QSize(46,46));ui->tb->setColumnWidth(0,62);
    ui->tb->setTextElideMode(Qt::ElideRight);

    connect(ui->btnAdd,&QPushButton::clicked,this,&MainWindow::onAdd);
    connect(ui->btnMod,&QPushButton::clicked,this,&MainWindow::onMod);
    connect(ui->btnImg,&QPushButton::clicked,this,&MainWindow::onImg);
    connect(ui->btnDel,&QPushButton::clicked,this,&MainWindow::onDel);
    connect(ui->btnReset,&QPushButton::clicked,this,&MainWindow::reset);
    connect(ui->btnPdf,&QPushButton::clicked,this,&MainWindow::exportPdf);
    connect(ui->search,&QLineEdit::textChanged,this,&MainWindow::refresh);
    connect(ui->cSort,QOverload<int>::of(&QComboBox::currentIndexChanged),this,&MainWindow::refresh);
    connect(ui->gSearch,&QLineEdit::textChanged,ui->search,&QLineEdit::setText);
    connect(ui->tb,&QTableWidget::itemSelectionChanged,this,&MainWindow::onRow);
    loadData();

    if(!INTERACTIF){
        // Affichage seul : le formulaire montre un exemple, plus rien ne réagit à la souris ni au clavier
        fill(A[0]);
        ui->tb->setSelectionMode(QAbstractItemView::NoSelection);
        const auto ws=ui->centralwidget->findChildren<QWidget*>();
        for(QWidget*w:ws){
            w->setFocusPolicy(Qt::NoFocus);
            w->setContextMenuPolicy(Qt::NoContextMenu);
            w->setAttribute(Qt::WA_TransparentForMouseEvents,true);
        }
        ui->centralwidget->setAttribute(Qt::WA_TransparentForMouseEvents,true);
        ui->centralwidget->setFocusPolicy(Qt::NoFocus);
    }
}

MainWindow::~MainWindow(){delete ui;}

// Recharge la liste depuis la base Oracle (CRUD : Afficher) puis réaffiche le tableau
void MainWindow::loadData(){
    A=Article::afficher();
    refresh();
}

void MainWindow::refresh(){
    QString q=ui->search->text().toLower();view.clear();
    for(int i=0;i<A.size();++i){auto&a=A[i];if((a.getId()+a.getNom()+a.getCat()+a.getSt()+a.getDesc()).toLower().contains(q))view<<i;}
    int k=ui->cSort->currentIndex();
    if(k>0)std::sort(view.begin(),view.end(),[&](int x,int y){auto&a=A[x];auto&b=A[y];
        switch(k){case 1:return a.getNom()<b.getNom();case 2:return a.getCat()<b.getCat();case 3:return a.getPrix()<b.getPrix();case 4:return a.getQte()<b.getQte();case 6:return a.getDem()>b.getDem();default:return a.getDate()<b.getDate();}});
    QSignalBlocker sb(ui->tb);ui->tb->setRowCount(view.size());
    for(int r=0;r<view.size();++r){
        auto&a=A[view[r]];
        QString niv=a.getDem()>=70?"Forte":a.getDem()>=40?"Moyenne":"Faible";
        QStringList v{QString(),a.getId(),a.getNom(),a.getCat(),QString::number(a.getPrix(),'f',3)+" DT",QString::number(a.getQte()),QString("%1 · %2").arg(a.getDem()).arg(niv),a.getSt()};
        for(int c=0;c<8;++c){
            auto it=new QTableWidgetItem(v[c]);
            if(c==0){it->setIcon(thumb(a.getImg()));it->setTextAlignment(Qt::AlignCenter);if(a.getImg().isEmpty())it->setText("—");}
            if(c==6){bool f=a.getDem()>=70,m=a.getDem()>=40&&!f;
                it->setBackground(f?QColor("#fde9cf"):m?QColor("#e3ecf8"):QColor("#e7e9ee"));
                it->setForeground(f?QColor("#a8591a"):m?QColor("#2f5d9e"):QColor("#5b6675"));it->setTextAlignment(Qt::AlignCenter);}
            if(c==7){bool ok=a.getSt()=="Disponible",ar=a.getSt()=="Archivé";it->setBackground(ok?QColor("#dff1e2"):ar?QColor("#e7e9ee"):QColor("#f8dcdc"));it->setForeground(ok?QColor("#2e7d4f"):ar?QColor("#5b6675"):QColor("#b03a3a"));it->setTextAlignment(Qt::AlignCenter);}
            ui->tb->setItem(r,c,it);}
        if(view[r]==cur)ui->tb->selectRow(r);
    }
    stats();
}

void MainWindow::stats(){
    QMap<QString,int> c;for(auto&a:A)c[a.getCat()]++;
    QVector<QPair<QString,double>> d;for(auto k:c.keys())d<<qMakePair(k,double(c[k]));ui->chDonut->setData(d);
    QVector<QPair<QString,double>> m;const char*mn[]={"Jan","Fév","Mar","Avr","Mai","Juin"};
    for(int i=0;i<6;++i){int n=0;for(auto&a:A)if(a.getDate().month()==i+1)n++;m<<qMakePair(QString(mn[i]),double(n));}ui->chLine->setData(m);
    auto s=A;std::sort(s.begin(),s.end(),[](const Article&x,const Article&y){return x.getQte()>y.getQte();});
    QVector<QPair<QString,double>> t;for(int i=0;i<qMin(5,int(s.size()));++i)t<<qMakePair(s[i].getNom(),double(s[i].getQte()));ui->chBars->setData(t);
}

bool MainWindow::readForm(Article&a){
    a={ui->eId->text().trimmed(),ui->eNom->text().trimmed(),ui->cCat->currentText(),ui->sPrix->value(),ui->sQte->value(),ui->dDate->date(),ui->cSt->currentText(),ui->eDesc->toPlainText(),ui->sDem->value(),img};
    if(a.getId().isEmpty()||a.getNom().isEmpty()){QMessageBox::warning(this,"Champs requis","Renseignez l'ID et le nom de l'article.");return false;}
    static const QRegularExpression nomValide(QStringLiteral("^[\\p{L}\\p{M}\\p{N} ]+$"));
    if(!nomValide.match(a.getNom()).hasMatch()){QMessageBox::warning(this,"Nom invalide","Le nom peut contenir uniquement des lettres, des chiffres et des espaces.");return false;}
    return true;
}
void MainWindow::fill(const Article&a){
    ui->eId->setText(a.getId());ui->eNom->setText(a.getNom());ui->cCat->setCurrentText(a.getCat());ui->sPrix->setValue(a.getPrix());ui->sQte->setValue(a.getQte());
    ui->dDate->setDate(a.getDate());ui->cSt->setCurrentText(a.getSt());ui->eDesc->setPlainText(a.getDesc());ui->sDem->setValue(a.getDem());setImg(a.getImg());
}
void MainWindow::setImg(const QString&p){
    img=p;QPixmap pm(p);
    if(p.isEmpty()||pm.isNull()){img.clear();ui->lImg->setPixmap(QPixmap());ui->lImg->setText("Aucune\nimage");}
    else ui->lImg->setPixmap(pm.scaled(ui->lImg->size()-QSize(4,4),Qt::KeepAspectRatio,Qt::SmoothTransformation));
}
void MainWindow::onImg(){
    QString f=pickImage(this);
    if(!f.isEmpty())setImg(f);
}
void MainWindow::onRow(){
    auto s=ui->tb->selectionModel()->selectedRows();if(s.isEmpty())return;
    cur=view[s[0].row()];fill(A[cur]);
}
void MainWindow::onAdd(){
    Article a;if(!readForm(a))return;
    if(Article::existe(a.getId())){QMessageBox::warning(this,"Doublon","Cet ID existe déjà.");return;}
    if(img.isEmpty()){   // clic sur Ajouter -> on propose d'insérer l'image de l'article
        QString f=pickImage(this);
        if(!f.isEmpty()){setImg(f);a.setImg(f);}
    }
    if(!a.ajouter()){QMessageBox::critical(this,"Erreur BD","L'ajout a échoué (voir la console).");return;}
    cur=-1;loadData();statusBar()->showMessage("Article ajouté",3000);
}
void MainWindow::onMod(){
    if(cur<0){QMessageBox::information(this,"Modifier","Sélectionnez un article dans la liste.");return;}
    Article a;if(!readForm(a))return;
    a.setId(A[cur].getId());   // la clé primaire ne change pas
    if(!a.modifier()){QMessageBox::critical(this,"Erreur BD","La modification a échoué (voir la console).");return;}
    loadData();statusBar()->showMessage("Article modifié",3000);
}
void MainWindow::onDel(){
    if(cur<0){QMessageBox::information(this,"Supprimer","Sélectionnez un article dans la liste.");return;}
    if(QMessageBox::question(this,"Supprimer","Supprimer l'article « "+A[cur].getNom()+" » ?")!=QMessageBox::Yes)return;
    if(!Article::supprimer(A[cur].getId())){QMessageBox::critical(this,"Erreur BD","La suppression a échoué (voir la console).");return;}
    cur=-1;loadData();statusBar()->showMessage("Article supprimé",3000);
}
void MainWindow::reset(){
    ui->eId->clear();ui->eNom->clear();ui->cCat->setCurrentIndex(0);ui->sPrix->setValue(0);ui->sQte->setValue(0);ui->dDate->setDate(QDate::currentDate());
    ui->cSt->setCurrentIndex(0);ui->eDesc->clear();ui->sDem->setValue(0);setImg(QString());cur=-1;ui->tb->clearSelection();
}
void MainWindow::exportPdf(){
    QString path=QFileDialog::getSaveFileName(this,"Exporter en PDF","articles.pdf","PDF (*.pdf)");if(path.isEmpty())return;
    QString h="<h2 style='color:#0a2a4a'>Fashionova – Liste des articles</h2><table border='1' cellspacing='0' cellpadding='5' width='100%'>"
              "<tr bgcolor='#f1e4d3'><th>ID</th><th>Nom</th><th>Catégorie</th><th>Prix</th><th>Stock</th><th>Demande</th><th>Statut</th></tr>";
    for(int i:view){auto&a=A[i];h+=QString("<tr><td>%1</td><td>%2</td><td>%3</td><td>%4 DT</td><td>%5</td><td>%6</td><td>%7</td></tr>")
        .arg(a.getId(),a.getNom().toHtmlEscaped(),a.getCat()).arg(a.getPrix(),0,'f',3).arg(a.getQte()).arg(a.getDem()).arg(a.getSt());}
    h+="</table>";
    QPdfWriter w(path);w.setPageSize(QPageSize(QPageSize::A4));QTextDocument d;d.setHtml(h);d.print(&w);
    statusBar()->showMessage("PDF exporté",3000);
}
