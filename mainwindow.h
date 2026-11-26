#pragma once
#include <QtWidgets>
#include "article.h"

QT_BEGIN_NAMESPACE
namespace Ui{class MainWindow;}
QT_END_NAMESPACE


class MainWindow:public QMainWindow{
    Q_OBJECT
public:
    MainWindow();
    ~MainWindow();
private slots:
    void onAdd();void onMod();void onDel();
    void reset();void loadData();void refresh();void exportPdf();void onRow();void onImg();
private:
    void setImg(const QString&p);bool readForm(Article&a);void fill(const Article&a);void stats();
    Ui::MainWindow*ui;                 // généré depuis mainwindow.ui (Qt Designer)
    QString img;QVector<Article> A;QVector<int> view;int cur=-1;
};
