#pragma once
#include <QtWidgets>

QT_BEGIN_NAMESPACE
namespace Ui{class ArticlesPage;}
QT_END_NAMESPACE

struct Article{QString id,nom,cat;double prix;int qte;QDate date;QString st,desc;int dem=0;QString img;};

class ArticlesPage:public QMainWindow{
    Q_OBJECT
public:
    ArticlesPage();
    ~ArticlesPage();
private slots:
    void onAdd();void onMod();void onDel();void onView();
    void reset();void refresh();void exportPdf();void onRow();void onImg();
private:
    void setImg(const QString&p);bool readForm(Article&a);void fill(const Article&a);void stats();
    Ui::ArticlesPage*ui;                 // généré depuis mainwindow.ui (Qt Designer)
    QString img;QVector<Article> A;QVector<int> view;int cur=-1;
};
