#pragma once
#include <QColor>
#include <QStringList>
#include <QVector>
#include <QWidget>

// Anneau « Répartition par type » avec légende
class DonutChart : public QWidget
{
public:
    struct Slice { QString label; int value; QColor color; };
    explicit DonutChart(QWidget *parent = nullptr);
    void setData(const QVector<Slice> &slices, const QString &unit);
    QSize minimumSizeHint() const override { return QSize(300, 190); }
protected:
    void paintEvent(QPaintEvent *) override;
private:
    QVector<Slice> m_slices;
    QString m_unit;
};

// Métier 1 : barres « Performance par type de machine »
// (part de machines opérationnelles par type + phrase de synthèse)
class PerformanceBars : public QWidget
{
public:
    struct Entry { QString type; int operationnelles; int total; };   // triées : la meilleure d'abord
    explicit PerformanceBars(QWidget *parent = nullptr);
    void setData(const QVector<Entry> &entries, const QString &summary);
    QSize minimumSizeHint() const override { return QSize(280, 190); }
protected:
    void paintEvent(QPaintEvent *) override;
private:
    QVector<Entry> m_entries;
    QString m_summary;
};

// Métier 2 : liste « Planning de maintenance optimisé »
// (date enregistrée -> date proposée + raison de l'ajustement)
class PlanningList : public QWidget
{
public:
    struct Entry { QString nom; QString ancienne; QString proposee; QString raison; bool modifiee; QColor couleur; };
    explicit PlanningList(QWidget *parent = nullptr);
    void setData(const QVector<Entry> &entries, const QString &summary);
    QSize minimumSizeHint() const override { return QSize(280, 190); }
protected:
    void paintEvent(QPaintEvent *) override;
private:
    QVector<Entry> m_entries;
    QString m_summary;
};
