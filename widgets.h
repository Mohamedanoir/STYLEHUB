#pragma once
#include <QWidget>
#include <QFrame>
#include <QPixmap>
#include <QVector>
#include <QPair>
#include <QString>
#include <QStringList>
#include <QIcon>

// Graphiques dessinés avec QPainter (aucun module Qt Charts requis)
// Widget "promu" dans Qt Designer : classe ChartWidget, fichier d'en-tête widgets.h
class ChartWidget:public QWidget{
public:
    enum Mode{Donut,Line,Bars};
    explicit ChartWidget(QWidget*p=nullptr);
    void setMode(Mode m);
    void setData(const QVector<QPair<QString,double>>&d);
protected:
    void paintEvent(QPaintEvent*)override;
private:
    Mode mode=Donut;QVector<QPair<QString,double>> data;
};

// Bannière dessinée avec QPainter : image tissu + machine (banner.png), titre, citation
// Widget "promu" dans Qt Designer : classe BannerWidget, fichier d'en-tête widgets.h
class BannerWidget:public QWidget{
public:
    explicit BannerWidget(QWidget*p=nullptr);
    void setTexts(const QString&title,const QString&sub,const QStringList&quote);
protected:
    void paintEvent(QPaintEvent*)override;
private:
    QString title,sub;QStringList quote;QPixmap bg;
};
