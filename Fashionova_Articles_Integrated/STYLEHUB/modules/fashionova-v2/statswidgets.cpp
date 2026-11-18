#include "statswidgets.h"

#include <QFontMetricsF>
#include <QPainter>
#include <QPainterPath>
#include <QPolygonF>
#include <QtMath>
#include <algorithm>
#include <cmath>

namespace {

const QColor kNavy("#0B2A4A");
const QColor kText("#1F3A57");
const QColor kBorder("#E3E8EE");
const QColor kTrack("#EDF1F5");
const QColor kStandard("#1B5E8C");
const QColor kVip("#E0BC8A");
const QColor kPremium("#6DB6EE");

// pas « rond » (1, 2, 5 × 10^k) supérieur ou égal à v
double niceStep(double v)
{
    if (v <= 0) return 1;
    const double p = qPow(10.0, qFloor(std::log10(v)));
    for (double m : {1.0, 2.0, 5.0, 10.0})
        if (m * p >= v) return m * p;
    return 10 * p;
}

} // namespace

// ============================================================ StatCard

StatCard::StatCard(const QString &title, QWidget *parent)
    : QWidget(parent), m_title(title)
{
    setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
}

QFont StatCard::font(double px, bool bold) const
{
    QFont f = QWidget::font();
    f.setPixelSize(qMax(8, qRound(px * scale())));
    f.setBold(bold);
    return f;
}

QRectF StatCard::paintFrame(QPainter &p) const
{
    p.setRenderHint(QPainter::Antialiasing, true);
    p.setRenderHint(QPainter::TextAntialiasing, true);

    const QRectF card = QRectF(rect()).adjusted(0.5, 0.5, -0.5, -0.5);
    p.setPen(QPen(kBorder, 1));
    p.setBrush(Qt::white);
    p.drawRoundedRect(card, 10 * scale(), 10 * scale());

    const double m = 14 * scale();
    p.setPen(kNavy);
    p.setFont(font(13, true));
    const QRectF titleRect(card.left() + m, card.top() + 8 * scale(),
                           card.width() - 2 * m, 22 * scale());
    p.drawText(titleRect, Qt::AlignLeft | Qt::AlignVCenter, m_title);

    return QRectF(card.left() + m, titleRect.bottom() + 4 * scale(),
                  card.width() - 2 * m, card.bottom() - titleRect.bottom() - 4 * scale() - m * 0.6);
}

// ============================================================ DonutCard

DonutCard::DonutCard(QWidget *parent)
    : StatCard(QString::fromUtf8("Répartition par type de client"), parent) {}

void DonutCard::setData(int standard, int vip, int premium)
{
    m_std = standard; m_vip = vip; m_prem = premium;
    update();
}

void DonutCard::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    const QRectF r = paintFrame(p);
    const double s = scale();
    const int total = m_std + m_vip + m_prem;

    // --- anneau
    const double d = qMin(r.height(), r.width() * 0.46);
    const double th = d * 0.24;
    const QRectF ring(r.left() + 2 * s, r.top() + (r.height() - d) / 2 + th / 2,
                      d - th, d - th);
    const QRectF ringBox(r.left() + 2 * s, r.top() + (r.height() - d) / 2, d, d);

    p.setBrush(Qt::NoBrush);
    if (total == 0) {
        p.setPen(QPen(kTrack, th, Qt::SolidLine, Qt::FlatCap));
        p.drawEllipse(ring);
    } else {
        struct Seg { int n; QColor c; } segs[] = { {m_std, kStandard}, {m_vip, kVip}, {m_prem, kPremium} };
        int start = 90 * 16;
        for (const Seg &sg : segs) {
            if (sg.n <= 0) continue;
            const int span = -qRound(360.0 * 16 * sg.n / total);
            p.setPen(QPen(sg.c, th, Qt::SolidLine, Qt::FlatCap));
            p.drawArc(ring, start, span);
            start += span;
        }
    }

    // --- texte au centre
    p.setPen(kNavy);
    p.setFont(font(24, true));
    p.drawText(QRectF(ringBox.left(), ringBox.top() + d * 0.26, d, d * 0.28),
               Qt::AlignCenter, QString::number(total));
    p.setFont(font(13, true));
    p.drawText(QRectF(ringBox.left(), ringBox.top() + d * 0.52, d, d * 0.2),
               Qt::AlignCenter, total > 1 ? "clients" : "client");

    // --- légende
    struct Row { QString name; int n; QColor c; } rows[] = {
        { "Standard", m_std, kStandard }, { "VIP", m_vip, kVip }, { "Premium", m_prem, kPremium }
    };
    const double lx = ringBox.right() + 22 * s;
    const double rowH = qMin(36 * s, r.height() / 3.0);
    const double y0 = r.top() + (r.height() - 3 * rowH) / 2;
    for (int i = 0; i < 3; ++i) {
        const double y = y0 + i * rowH;
        const double sq = 16 * s;
        p.setPen(Qt::NoPen);
        p.setBrush(rows[i].c);
        p.drawRoundedRect(QRectF(lx, y + (rowH - sq) / 2, sq, sq), 3 * s, 3 * s);

        p.setPen(kText);
        p.setFont(font(13));
        p.drawText(QRectF(lx + sq + 12 * s, y, 80 * s, rowH), Qt::AlignLeft | Qt::AlignVCenter, rows[i].name);

        const int pct = total ? qRound(100.0 * rows[i].n / total) : 0;
        const QString val = QString("%1 (%2%)").arg(rows[i].n).arg(pct);
        p.drawText(QRectF(lx, y, r.right() - lx, rowH), Qt::AlignRight | Qt::AlignVCenter, val);
    }
}

// ============================================================ EvolutionCard

EvolutionCard::EvolutionCard(QWidget *parent)
    : StatCard(QString::fromUtf8("Évolution des dépenses clients (DT)"), parent) {}

void EvolutionCard::setData(const QStringList &labels, const QVector<double> &values)
{
    m_labels = labels; m_values = values;
    update();
}

void EvolutionCard::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    const QRectF r = paintFrame(p);
    const double s = scale();
    const int n = m_values.size();
    if (n == 0) return;

    // échelle de l'axe Y : 4 graduations « rondes »
    const double vmax = *std::max_element(m_values.begin(), m_values.end());
    const double step = niceStep(vmax > 0 ? vmax / 4.0 : 1.0);
    const double ymax = step * 4;

    p.setFont(font(12));
    const QFontMetricsF fm(p.font());
    const double labelW = fm.horizontalAdvance(QString::number(qRound(ymax))) + 8 * s;
    const double bottomH = 20 * s;

    const QRectF plot(r.left() + labelW, r.top() + 6 * s,
                      r.width() - labelW - 6 * s, r.height() - bottomH - 6 * s);

    // axes + graduations Y
    p.setPen(QPen(kBorder, 1));
    p.drawLine(plot.bottomLeft(), plot.bottomRight());
    p.drawLine(plot.topLeft(), plot.bottomLeft());
    p.setPen(kText);
    for (int i = 0; i <= 4; ++i) {
        const double y = plot.bottom() - plot.height() * i / 4.0;
        p.drawText(QRectF(r.left(), y - 9 * s, labelW - 6 * s, 18 * s),
                   Qt::AlignRight | Qt::AlignVCenter, QString::number(qRound(step * i)));
    }

    // points
    const double padX = 14 * s;
    QPolygonF pts;
    for (int i = 0; i < n; ++i) {
        const double x = (n == 1) ? plot.center().x()
                                  : plot.left() + padX + (plot.width() - 2 * padX) * i / (n - 1);
        const double y = plot.bottom() - plot.height() * (m_values[i] / ymax);
        pts << QPointF(x, y);
    }

    // aire sous la courbe
    QPolygonF area = pts;
    area << QPointF(pts.last().x(), plot.bottom()) << QPointF(pts.first().x(), plot.bottom());
    p.setPen(Qt::NoPen);
    p.setBrush(QColor("#DCE9F5"));
    p.drawPolygon(area);

    // courbe + points
    p.setPen(QPen(kStandard, 2.2 * s, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    p.setBrush(Qt::NoBrush);
    p.drawPolyline(pts);
    p.setPen(Qt::NoPen);
    p.setBrush(kStandard);
    for (const QPointF &pt : pts) p.drawEllipse(pt, 4 * s, 4 * s);

    // mois
    p.setPen(kText);
    p.setFont(font(12));
    for (int i = 0; i < n && i < m_labels.size(); ++i)
        p.drawText(QRectF(pts[i].x() - 24 * s, plot.bottom() + 2 * s, 48 * s, bottomH),
                   Qt::AlignCenter, m_labels[i]);
}

// ============================================================ TopCard

TopCard::TopCard(QWidget *parent)
    : StatCard(QString::fromUtf8("Top 5 des clients les plus fidèles"), parent) {}

void TopCard::setData(const QVector<QPair<QString, int>> &items)
{
    m_items = items;
    update();
}

void TopCard::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    const QRectF r = paintFrame(p);
    const double s = scale();

    if (m_items.isEmpty()) {
        p.setPen(kText);
        p.setFont(font(13));
        p.drawText(r, Qt::AlignCenter, QString::fromUtf8("Aucune commande pour le moment"));
        return;
    }

    static const QColor colors[] = { QColor("#1B5E8C"), QColor("#6DB6EE"), QColor("#A9CCE9"),
                                     QColor("#9DB7CF"), QColor("#E0BC8A") };
    int maxV = 1;
    for (const auto &it : m_items) maxV = qMax(maxV, it.second);

    const double rowH = r.height() / 5.0;
    const double nameW = r.width() * 0.40;
    const double valW  = 26 * s;
    const double barX  = r.left() + nameW + 6 * s;
    const double barW  = r.width() - nameW - valW - 12 * s;
    const double barH  = 13 * s;

    p.setFont(font(13));
    const QFontMetricsF fm(p.font());
    for (int i = 0; i < m_items.size() && i < 5; ++i) {
        const double y = r.top() + i * rowH;
        p.setPen(kText);
        p.drawText(QRectF(r.left(), y, nameW, rowH), Qt::AlignLeft | Qt::AlignVCenter,
                   fm.elidedText(m_items[i].first, Qt::ElideRight, nameW));

        const QRectF track(barX, y + (rowH - barH) / 2, barW, barH);
        p.setPen(Qt::NoPen);
        p.setBrush(kTrack);
        p.drawRoundedRect(track, barH / 2, barH / 2);
        QRectF fill = track;
        fill.setWidth(qMax(barH, track.width() * m_items[i].second / maxV));
        p.setBrush(colors[i]);
        p.drawRoundedRect(fill, barH / 2, barH / 2);

        p.setPen(kText);
        p.drawText(QRectF(r.right() - valW, y, valW, rowH), Qt::AlignRight | Qt::AlignVCenter,
                   QString::number(m_items[i].second));
    }
}
