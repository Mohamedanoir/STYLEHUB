#include "articlespage.h"
#include "ui_articlespage.h"
#include "widgets.h"
#include <QtWidgets>
#include <QHash>
#include <QPdfWriter>
#include <QTextDocument>
#include <QPageSize>
#include <algorithm>

// Miniature carrée (mise en cache) affichée devant l'article dans la liste
static QIcon thumb(const QString&p){
    static QHash<QString,QIcon> cache;
    if(p.isEmpty())return QIcon();
    if(cache.contains(p))return cache[p];
    QPixmap pm(p);QIcon ic;
    if(!pm.isNull()){pm=pm.scaled(46,46,Qt::KeepAspectRatioByExpanding,Qt::SmoothTransformation);
        ic=QIcon(pm.copy((pm.width()-46)/2,(pm.height()-46)/2,46,46));}
    cache[p]=ic;return ic;}

ArticlesPage::ArticlesPage(QWidget*parent):QWidget(parent),ui(new Ui::ArticlesPage){
    ui->setupUi(this);   // toute l'interface vient de articlespage.ui

    ui->banner->setTexts("Gestion des Articles","Du stock à la vente",{"Un stock bien tenu,","c'est une vente","qui se prépare."});
    ui->chDonut->setMode(ChartWidget::Donut);
    ui->chLine->setMode(ChartWidget::Line);
    ui->chBars->setMode(ChartWidget::Bars);

    A={{"ART001","Soie naturelle","Tissus",45,120,QDate(2026,1,12),"Disponible","Soie fluide pour robes de soirée",85,":/articles/images/art001.png"},
       {"ART002","Dentelle française","Tissus",68.5,35,QDate(2026,2,8),"Disponible","Dentelle fine ivoire",60,":/articles/images/art002.png"},
       {"ART003","Fil polyester noir","Fils",2.9,0,QDate(2026,2,20),"Rupture de stock","Cône de 1000 m",90,":/articles/images/art003.png"},
       {"ART004","Boutons nacre","Boutons",0.8,900,QDate(2026,3,5),"Disponible","Lot de 100 pièces",40,":/articles/images/art004.png"},
       {"ART005","Fermeture invisible","Fermetures",1.5,240,QDate(2026,3,18),"Disponible","Longueur 50 cm",30,":/articles/images/art005.png"},
       {"ART006","Satin duchesse","Tissus",39,18,QDate(2026,4,2),"Disponible","Satin épais brillant",55,":/articles/images/art006.png"},
       {"ART007","Ruban velours","Accessoires",6.2,75,QDate(2026,4,25),"Archivé","Ancienne collection",5,":/articles/images/art007.png"},
       {"ART008","Fil de soie","Fils",4.4,310,QDate(2026,5,14),"Disponible","Fil à broder",70,":/articles/images/art008.png"}};

    ui->dDate->setDate(QDate::currentDate());
    ui->cSort->setCurrentIndex(6);   // par défaut : articles classés selon la demande
    ui->tb->horizontalHeader()->setSectionResizeMode(2,QHeaderView::Stretch);
    ui->tb->setIconSize(QSize(46,46));ui->tb->setColumnWidth(0,62);
    ui->tb->setTextElideMode(Qt::ElideRight);

    connect(ui->btnAdd,&QPushButton::clicked,this,&ArticlesPage::onAdd);
    connect(ui->btnMod,&QPushButton::clicked,this,&ArticlesPage::onMod);
    connect(ui->btnImg,&QPushButton::clicked,this,&ArticlesPage::onImg);
    connect(ui->btnDel,&QPushButton::clicked,this,&ArticlesPage::onDel);
    connect(ui->btnView,&QPushButton::clicked,this,&ArticlesPage::onView);
    connect(ui->btnReset,&QPushButton::clicked,this,&ArticlesPage::reset);
    connect(ui->btnPdf,&QPushButton::clicked,this,&ArticlesPage::exportPdf);
    connect(ui->search,&QLineEdit::textChanged,this,&ArticlesPage::refresh);
    connect(ui->cSort,QOverload<int>::of(&QComboBox::currentIndexChanged),this,&ArticlesPage::refresh);
    connect(ui->tb,&QTableWidget::itemSelectionChanged,this,&ArticlesPage::onRow);
    refresh();

}

ArticlesPage::~ArticlesPage(){delete ui;}

void ArticlesPage::refresh(){
    QString q=ui->search->text().toLower();view.clear();
    for(int i=0;i<A.size();++i){auto&a=A[i];if((a.id+a.nom+a.cat+a.st+a.desc).toLower().contains(q))view<<i;}
    int k=ui->cSort->currentIndex();
    if(k>0)std::sort(view.begin(),view.end(),[&](int x,int y){auto&a=A[x];auto&b=A[y];
        switch(k){case 1:return a.nom<b.nom;case 2:return a.cat<b.cat;case 3:return a.prix<b.prix;case 4:return a.qte<b.qte;case 6:return a.dem>b.dem;default:return a.date<b.date;}});
    QSignalBlocker sb(ui->tb);ui->tb->setRowCount(view.size());
    for(int r=0;r<view.size();++r){
        auto&a=A[view[r]];
        QString niv=a.dem>=70?"Forte":a.dem>=40?"Moyenne":"Faible";
        QStringList v{QString(),a.id,a.nom,a.cat,QString::number(a.prix,'f',3)+" DT",QString::number(a.qte),QString("%1 · %2").arg(a.dem).arg(niv),a.st};
        for(int c=0;c<8;++c){
            auto it=new QTableWidgetItem(v[c]);
            if(c==0){it->setIcon(thumb(a.img));it->setTextAlignment(Qt::AlignCenter);if(a.img.isEmpty())it->setText("—");}
            if(c==6){bool f=a.dem>=70,m=a.dem>=40&&!f;
                it->setBackground(f?QColor("#fde9cf"):m?QColor("#e3ecf8"):QColor("#e7e9ee"));
                it->setForeground(f?QColor("#a8591a"):m?QColor("#2f5d9e"):QColor("#5b6675"));it->setTextAlignment(Qt::AlignCenter);}
            if(c==7){bool ok=a.st=="Disponible",ar=a.st=="Archivé";it->setBackground(ok?QColor("#dff1e2"):ar?QColor("#e7e9ee"):QColor("#f8dcdc"));it->setForeground(ok?QColor("#2e7d4f"):ar?QColor("#5b6675"):QColor("#b03a3a"));it->setTextAlignment(Qt::AlignCenter);}
            ui->tb->setItem(r,c,it);}
        if(view[r]==cur)ui->tb->selectRow(r);
    }
    stats();
}

void ArticlesPage::stats(){
    QMap<QString,int> c;for(auto&a:A)c[a.cat]++;
    QVector<QPair<QString,double>> d;for(auto k:c.keys())d<<qMakePair(k,double(c[k]));ui->chDonut->setData(d);
    QVector<QPair<QString,double>> m;const char*mn[]={"Jan","Fév","Mar","Avr","Mai","Juin"};
    for(int i=0;i<6;++i){int n=0;for(auto&a:A)if(a.date.month()==i+1)n++;m<<qMakePair(QString(mn[i]),double(n));}ui->chLine->setData(m);
    auto s=A;std::sort(s.begin(),s.end(),[](const Article&x,const Article&y){return x.qte>y.qte;});
    QVector<QPair<QString,double>> t;for(int i=0;i<qMin(5,int(s.size()));++i)t<<qMakePair(s[i].nom,double(s[i].qte));ui->chBars->setData(t);
}

bool ArticlesPage::readForm(Article&a){
    a={ui->eId->text().trimmed(),ui->eNom->text().trimmed(),ui->cCat->currentText(),ui->sPrix->value(),ui->sQte->value(),ui->dDate->date(),ui->cSt->currentText(),ui->eDesc->toPlainText(),ui->sDem->value(),img};
    if(a.id.isEmpty()||a.nom.isEmpty()){QMessageBox::warning(this,"Champs requis","Renseignez l'ID et le nom de l'article.");return false;}
    return true;
}
void ArticlesPage::fill(const Article&a){
    ui->eId->setText(a.id);ui->eNom->setText(a.nom);ui->cCat->setCurrentText(a.cat);ui->sPrix->setValue(a.prix);ui->sQte->setValue(a.qte);
    ui->dDate->setDate(a.date);ui->cSt->setCurrentText(a.st);ui->eDesc->setPlainText(a.desc);ui->sDem->setValue(a.dem);setImg(a.img);
}
void ArticlesPage::setImg(const QString&p){
    img=p;QPixmap pm(p);
    if(p.isEmpty()||pm.isNull()){img.clear();ui->lImg->setPixmap(QPixmap());ui->lImg->setText("Aucune\nimage");}
    else ui->lImg->setPixmap(pm.scaled(ui->lImg->size()-QSize(4,4),Qt::KeepAspectRatio,Qt::SmoothTransformation));
}
void ArticlesPage::onImg(){
    QString f=QFileDialog::getOpenFileName(this,"Choisir l'image de l'article",QString(),"Images (*.png *.jpg *.jpeg *.bmp *.webp)");
    if(!f.isEmpty())setImg(f);
}
void ArticlesPage::onRow(){
    auto s=ui->tb->selectionModel()->selectedRows();if(s.isEmpty())return;
    cur=view[s[0].row()];fill(A[cur]);
}
void ArticlesPage::onAdd(){
    Article a;if(!readForm(a))return;
    for(auto&x:A)if(x.id==a.id){QMessageBox::warning(this,"Doublon","Cet ID existe déjà.");return;}
    if(img.isEmpty()){   // clic sur Ajouter -> on propose d'insérer l'image de l'article
        QString f=QFileDialog::getOpenFileName(this,"Insérer l'image de l'article",QString(),"Images (*.png *.jpg *.jpeg *.bmp *.webp)");
        if(!f.isEmpty()){setImg(f);a.img=f;}
    }
    A<<a;cur=A.size()-1;refresh();emit statusMessage("Article ajouté",3000);
}
void ArticlesPage::onMod(){
    if(cur<0){QMessageBox::information(this,"Modifier","Sélectionnez un article dans la liste.");return;}
    Article a;if(!readForm(a))return;A[cur]=a;refresh();emit statusMessage("Article modifié",3000);
}
void ArticlesPage::onDel(){
    if(cur<0){QMessageBox::information(this,"Supprimer","Sélectionnez un article dans la liste.");return;}
    if(QMessageBox::question(this,"Supprimer","Supprimer l'article « "+A[cur].nom+" » ?")!=QMessageBox::Yes)return;
    A.remove(cur);cur=-1;refresh();emit statusMessage("Article supprimé",3000);
}
void ArticlesPage::onView(){
    if(cur<0){QMessageBox::information(this,"Afficher","Sélectionnez un article dans la liste.");return;}
    auto&a=A[cur];
    QMessageBox b(this);b.setWindowTitle("Article "+a.id);
    b.setText(QString("<b>%1</b><br>Catégorie : %2<br>Prix : %3 DT<br>Stock : %4<br>Demande : %5<br>Ajouté le : %6<br>Statut : %7<br><br>%8")
        .arg(a.nom,a.cat).arg(a.prix,0,'f',3).arg(a.qte).arg(a.dem).arg(a.date.toString("dd/MM/yyyy"),a.st,a.desc.toHtmlEscaped()));
    QPixmap pm(a.img);if(!pm.isNull())b.setIconPixmap(pm.scaled(140,140,Qt::KeepAspectRatio,Qt::SmoothTransformation));
    b.exec();
}
void ArticlesPage::reset(){
    ui->eId->clear();ui->eNom->clear();ui->cCat->setCurrentIndex(0);ui->sPrix->setValue(0);ui->sQte->setValue(0);ui->dDate->setDate(QDate::currentDate());
    ui->cSt->setCurrentIndex(0);ui->eDesc->clear();ui->sDem->setValue(0);setImg(QString());cur=-1;ui->tb->clearSelection();
}
void ArticlesPage::exportPdf(){
    QString path=QFileDialog::getSaveFileName(this,"Exporter en PDF","articles.pdf","PDF (*.pdf)");if(path.isEmpty())return;
    QString h="<h2 style='color:#0a2a4a'>Fashionova – Liste des articles</h2><table border='1' cellspacing='0' cellpadding='5' width='100%'>"
              "<tr bgcolor='#f1e4d3'><th>ID</th><th>Nom</th><th>Catégorie</th><th>Prix</th><th>Stock</th><th>Demande</th><th>Statut</th></tr>";
    for(int i:view){auto&a=A[i];h+=QString("<tr><td>%1</td><td>%2</td><td>%3</td><td>%4 DT</td><td>%5</td><td>%6</td><td>%7</td></tr>")
        .arg(a.id,a.nom.toHtmlEscaped(),a.cat).arg(a.prix,0,'f',3).arg(a.qte).arg(a.dem).arg(a.st);}
    h+="</table>";
    QPdfWriter w(path);w.setPageSize(QPageSize(QPageSize::A4));QTextDocument d;d.setHtml(h);d.print(&w);
    emit statusMessage("PDF exporté",3000);
}

void ArticlesPage::setSearchText(const QString&t){ui->search->setText(t);}
