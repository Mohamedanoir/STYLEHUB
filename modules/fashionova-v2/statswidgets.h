#ifndef STATSWIDGETS_H
#define STATSWIDGETS_H

#include <QFont>
#include <QPair>
#include <QStringList>
#include <QVector>
#include <QWidget>

// Carte de base : fond blanc arrondi + titre. Les 3 widgets en héritent.
class QPainter;

class StatCard : public QWidget
{
    Q_OBJECT
public:
    explicit StatCard(const QString &title, QWidget *parent = nullptr);

protected:
    // dessine la carte + le titre, renvoie la zone de contenu (sous le titre)
    QRectF paintFrame(QPainter &p) const;
    double scale() const { return height() / 186.0; }   // 186 px = hauteur de référence
    QFont font(double pt, bool bold = false) const;

    QString m_title;
};

// 1) Répartition par type de client (anneau + légende)
class DonutCard : public StatCard
{
    Q_OBJECT
public:
    explicit DonutCard(QWidget *parent = nullptr);
    void setData(int standard, int vip, int premium);

protected:
    void paintEvent(QPaintEvent *) override;

private:
    int m_std = 0, m_vip = 0, m_prem = 0;
};

// 2) Évolution mensuelle (courbe + aire)
class EvolutionCard : public StatCard
{
    Q_OBJECT
public:
    explicit EvolutionCard(QWidget *parent = nullptr);
    void setData(const QStringList &labels, const QVector<double> &values);

protected:
    void paintEvent(QPaintEvent *) override;

private:
    QStringList m_labels;
    QVector<double> m_values;
};

// 3) Top 5 des clients (barres horizontales)
class TopCard : public StatCard
{
    Q_OBJECT
public:
    explicit TopCard(QWidget *parent = nullptr);
    // (nom complet, nombre de commandes)
    void setData(const QVector<QPair<QString, int>> &items);

protected:
    void paintEvent(QPaintEvent *) override;

private:
    QVector<QPair<QString, int>> m_items;
};

#endif // STATSWIDGETS_H
