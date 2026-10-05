#include "statwidgets.h"

#include <QPainter>
#include <QPainterPath>
#include <QtMath>

QColor couleurSerie(int i)
{
    static const QColor c[] = { QColor("#0B2D52"), QColor("#7EA6D8"), QColor("#B8D2EE"),
                                QColor("#F1CFA6"), QColor("#C59A62"), QColor("#7D8597") };
    return c[i % 6];
}

static QColor couleurBarre(int i)
{
    static const QColor c[] = { QColor("#0B2D52"), QColor("#8DB4E2"), QColor("#C9DCF0"),
                                QColor("#B4C3D3"), QColor("#EDCFA6") };
    return c[i % 5];
}

static const QColor TEXTE("#1F2A3D");
static const QColor TEXTE_DOUX("#5B6472");

// ---------------------------------------------------------------- Donut
DonutChart::DonutChart(QWidget *parent) : QWidget(parent) { setMinimumSize(300, 175); }

void DonutChart::setDonnees(const QList<QPair<QString, int>> &d) { m_d = d; update(); }

void DonutChart::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    int total = 0;
    for (const auto &e : m_d) total += e.second;
    if (total == 0) {
        p.setPen(TEXTE_DOUX);
        p.drawText(rect(), Qt::AlignCenter, "Aucune donnée");
        return;
    }

    const qreal diam = qMin<qreal>(qMin<qreal>(height() - 16, 150), width() * 0.42);
    const QRectF cercle(6, (height() - diam) / 2.0, diam, diam);
    const qreal ep = diam * 0.25;
    const QRectF arc = cercle.adjusted(ep / 2, ep / 2, -ep / 2, -ep / 2);

    qreal angle = 90.0;
    for (int i = 0; i < m_d.size(); ++i) {
        const qreal span = -360.0 * m_d[i].second / total;
        p.setPen(QPen(couleurSerie(i), ep, Qt::SolidLine, Qt::FlatCap));
        p.drawArc(arc, int(angle * 16), int(span * 16));
        angle += span;
    }
    // Séparateurs blancs
    angle = 90.0;
    p.setPen(QPen(Qt::white, 2));
    const QPointF c = cercle.center();
    for (int i = 0; i < m_d.size() && m_d.size() > 1; ++i) {
        const qreal a = qDegreesToRadians(angle);
        p.drawLine(QPointF(c.x() + (diam / 2 - ep) * qCos(a), c.y() - (diam / 2 - ep) * qSin(a)),
                   QPointF(c.x() + diam / 2 * qCos(a), c.y() - diam / 2 * qSin(a)));
        angle += -360.0 * m_d[i].second / total;
    }

    p.setPen(TEXTE);
    QFont f = font();
    f.setPixelSize(int(diam * 0.17));
    f.setBold(true);
    p.setFont(f);
    p.drawText(QRectF(cercle.left(), c.y() - diam * 0.16, diam, diam * 0.2), Qt::AlignCenter, QString::number(total));
    f.setPixelSize(13);
    f.setBold(false);
    p.setFont(f);
    p.drawText(QRectF(cercle.left(), c.y() + diam * 0.04, diam, 18), Qt::AlignCenter, "total");

    // Légende
    f.setPixelSize(13);
    p.setFont(f);
    const int x = int(cercle.right()) + 22;
    int y = int(height() / 2.0 - m_d.size() * 29 / 2.0);
    for (int i = 0; i < m_d.size(); ++i) {
        p.setPen(Qt::NoPen);
        p.setBrush(couleurSerie(i));
        p.drawRoundedRect(QRectF(x, y + 5, 15, 15), 3, 3);
        p.setPen(TEXTE);
        p.drawText(QRect(x + 22, y, 78, 25), Qt::AlignVCenter | Qt::AlignLeft, m_d[i].first);
        const int pct = qRound(100.0 * m_d[i].second / total);
        p.drawText(QRect(x + 98, y, 80, 25), Qt::AlignVCenter | Qt::AlignLeft,
                   QString("%1 (%2%)").arg(m_d[i].second).arg(pct));
        y += 29;
    }
}

// ---------------------------------------------------------------- Courbe
LineChart::LineChart(QWidget *parent) : QWidget(parent) { setMinimumSize(300, 175); }

void LineChart::setDonnees(const QList<QPair<QString, int>> &d) { m_d = d; update(); }

void LineChart::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    if (m_d.isEmpty()) return;

    int maxVal = 1;
    for (const auto &e : m_d) maxVal = qMax(maxVal, e.second);
    int pas = 5;
    while (maxVal > pas * 3) pas += 5;
    const int haut = pas * 3;

    const QRectF zone(40, 12, width() - 60, height() - 44);

    QFont f = font();
    f.setPixelSize(13);
    p.setFont(f);

    // Axes
    p.setPen(QPen(QColor("#9AA3AF"), 1));
    p.drawLine(zone.bottomLeft(), zone.topLeft());
    p.drawLine(zone.bottomLeft(), zone.bottomRight());
    for (int k = 0; k <= 3; ++k) {
        const qreal y = zone.bottom() - zone.height() * k / 3.0;
        p.setPen(QPen(QColor("#9AA3AF"), 1));
        p.drawLine(QPointF(zone.left() - 4, y), QPointF(zone.left(), y));
        p.setPen(k == 0 ? QColor("#C0C6CE") : TEXTE);
        p.drawText(QRectF(0, y - 9, 30, 18), Qt::AlignRight | Qt::AlignVCenter, QString::number(pas * k));
    }

    QList<QPointF> pts;
    const int n = m_d.size();
    for (int i = 0; i < n; ++i) {
        const qreal x = zone.left() + 6 + (n == 1 ? 0 : (zone.width() - 12) * i / (n - 1));
        const qreal y = zone.bottom() - zone.height() * m_d[i].second / haut;
        pts.append(QPointF(x, y));
        p.setPen(TEXTE);
        p.drawText(QRectF(x - 30, zone.bottom() + 8, 60, 20), Qt::AlignCenter, m_d[i].first);
    }

    QPainterPath aire;
    aire.moveTo(pts.first().x(), zone.bottom());
    for (const QPointF &pt : pts) aire.lineTo(pt);
    aire.lineTo(pts.last().x(), zone.bottom());
    aire.closeSubpath();
    QLinearGradient g(0, zone.top(), 0, zone.bottom());
    g.setColorAt(0, QColor(126, 166, 216, 110));
    g.setColorAt(1, QColor(126, 166, 216, 40));
    p.fillPath(aire, g);

    p.setPen(QPen(QColor("#0B2D52"), 2));
    for (int i = 1; i < pts.size(); ++i) p.drawLine(pts[i - 1], pts[i]);
    p.setBrush(QColor("#0B2D52"));
    p.setPen(QPen(Qt::white, 1.5));
    for (const QPointF &pt : pts) p.drawEllipse(pt, 5, 5);
}

// ---------------------------------------------------------------- Barres (Top 5)
BarChart::BarChart(QWidget *parent) : QWidget(parent) { setMinimumSize(280, 175); }

void BarChart::setDonnees(const QList<QPair<QString, int>> &d) { m_d = d; update(); }

void BarChart::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    if (m_d.isEmpty()) return;

    int maxVal = 1;
    for (const auto &e : m_d) maxVal = qMax(maxVal, e.second);

    QFont f = font();
    f.setPixelSize(14);
    p.setFont(f);
    const int labelW = 125, valW = 36, h = 27;
    int y = 2;
    const int barW = width() - labelW - valW - 10;

    for (int i = 0; i < m_d.size(); ++i) {
        p.setPen(TEXTE);
        p.drawText(QRect(0, y, labelW, h), Qt::AlignVCenter | Qt::AlignLeft,
                   p.fontMetrics().elidedText(m_d[i].first, Qt::ElideRight, labelW - 6));

        const QRectF fond(labelW, y + 7, barW, 13);
        p.setPen(Qt::NoPen);
        p.setBrush(QColor("#EEF0F3"));
        p.drawRoundedRect(fond, 6.5, 6.5);
        const QRectF barre(labelW, y + 7, qMax(13.0, barW * double(m_d[i].second) / maxVal), 13);
        p.setBrush(couleurBarre(i));
        p.drawRoundedRect(barre, 6.5, 6.5);

        p.setPen(TEXTE);
        p.drawText(QRect(labelW + barW, y, valW, h), Qt::AlignVCenter | Qt::AlignRight,
                   QString::number(m_d[i].second));
        y += h;
    }
}
