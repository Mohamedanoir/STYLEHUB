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

// Barre latérale : image Fashionova (logo en haut, mannequin en bas) dessinée en 3 tranches
// pour s'adapter à toutes les hauteurs sans déformer le logo ni le mannequin.
// Widget "promu" dans Qt Designer : classe SideWidget (base QFrame), fichier d'en-tête widgets.h
class SideWidget:public QFrame{
public:
    explicit SideWidget(QWidget*p=nullptr);
protected:
    void paintEvent(QPaintEvent*)override;
private:
    QPixmap bg;
};

// Icônes vectorielles de la barre latérale (QPainter) : blanc = non coché, bleu nuit #08213B = coché
// idx : 0 Clients, 1 Employés, 2 Commandes, 3 Maquettes, 4 Machines, 5 Articles
QIcon sideIcon(int idx);
