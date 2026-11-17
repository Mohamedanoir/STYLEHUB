#ifndef STATWIDGETS_H
#define STATWIDGETS_H

#include <QList>
#include <QPair>
#include <QWidget>

// Petits graphiques dessinés à la main (pas besoin du module Qt Charts)

class DonutChart : public QWidget
{
    Q_OBJECT
public:
    explicit DonutChart(QWidget *parent = nullptr);
    void setDonnees(const QList<QPair<QString, int>> &d);
protected:
    void paintEvent(QPaintEvent *) override;
private:
    QList<QPair<QString, int>> m_d;
};

class LineChart : public QWidget
{
    Q_OBJECT
public:
    explicit LineChart(QWidget *parent = nullptr);
    void setDonnees(const QList<QPair<QString, int>> &d);
protected:
    void paintEvent(QPaintEvent *) override;
private:
    QList<QPair<QString, int>> m_d;
};

class BarChart : public QWidget
{
    Q_OBJECT
public:
    explicit BarChart(QWidget *parent = nullptr);
    void setDonnees(const QList<QPair<QString, int>> &d);
protected:
    void paintEvent(QPaintEvent *) override;
private:
    QList<QPair<QString, int>> m_d;
};

// Couleurs partagées
QColor couleurSerie(int i);

#endif // STATWIDGETS_H
