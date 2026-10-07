#include "chartwidgets.h"

#include <QFontMetrics>
#include <QPainter>
#include <QPainterPath>
#include <QPolygonF>
#include <QtMath>

static QFont px(int pixelSize, bool bold = false)
{
    QFont f;
    f.setPixelSize(pixelSize);
    f.setBold(bold);
    return f;
}

/* ------------------------------ DonutChart ------------------------------ */
DonutChart::DonutChart(QWidget *parent) : QWidget(parent) {}

void DonutChart::setData(const QVector<Slice> &slices, const QString &unit)
{
    m_slices = slices;
    m_unit = unit;
    update();
}

void DonutChart::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    int total = 0;
    for (const Slice &s : m_slices) total += s.value;

    const int side = qMin<int>(qMin(height() - 8, 168), int(width() * 0.40));
    const QRectF rc(6, (height() - side) / 2.0, side, side);
    const qreal pw = side * 0.22;
    const QRectF arc = rc.adjusted(pw / 2, pw / 2, -pw / 2, -pw / 2);

    if (total == 0) {
        QPen pen(QColor("#e6dfd1"), pw);
        p.setPen(pen);
        p.drawEllipse(arc);
    } else {
        int start = 90 * 16;
        for (const Slice &s : m_slices) {
            const int span = -qRound(360.0 * 16 * s.value / total);
            QPen pen(s.color, pw);
            pen.setCapStyle(Qt::FlatCap);
            p.setPen(pen);
            p.drawArc(arc, start, span);
            start += span;
        }
    }

    // Total au centre
    p.setPen(QColor("#0a2748"));
    p.setFont(px(24, true));
    p.drawText(QRectF(rc.x(), rc.center().y() - 26, side, 28), Qt::AlignCenter, QString::number(total));
    p.setFont(px(12));
    p.setPen(QColor("#555555"));
    p.drawText(QRectF(rc.x(), rc.center().y() + 2, side, 18), Qt::AlignCenter, m_unit);

    // Légende
    const qreal lx = rc.right() + 18;
    const qreal rowH = 29;
    qreal y = height() / 2.0 - rowH * m_slices.size() / 2.0;
    p.setFont(px(13));
    for (const Slice &s : m_slices) {
        p.setPen(Qt::NoPen);
        p.setBrush(s.color);
        p.drawEllipse(QPointF(lx + 7, y + rowH / 2), 7, 7);
        p.setPen(QColor("#23252e"));
        const qreal labelX = lx + 20;
        const qreal valueW = 70;
        p.drawText(QRectF(labelX, y, width() - valueW - 4 - labelX, rowH), Qt::AlignVCenter | Qt::AlignLeft, s.label);
        const int pct = total ? qRound(100.0 * s.value / total) : 0;
        p.drawText(QRectF(width() - valueW, y, valueW, rowH), Qt::AlignVCenter | Qt::AlignRight,
                   QStringLiteral("%1 (%2%)").arg(s.value).arg(pct));
        y += rowH;
    }
}

/* ---------------------------- PerformanceBars ---------------------------- */
PerformanceBars::PerformanceBars(QWidget *parent) : QWidget(parent) {}

void PerformanceBars::setData(const QVector<Entry> &entries, const QString &summary)
{
    m_entries = entries;
    m_summary = summary;
    update();
}

static QColor performanceColor(int pct)
{
    if (pct >= 80) return QColor("#2e9e5b");
    if (pct >= 50) return QColor("#f0a020");
    return QColor("#e5484d");
}

void PerformanceBars::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    const qreal headH = 26;
    p.setFont(px(13, true));
    p.setPen(QColor("#0a2748"));
    p.drawText(QRectF(0, 0, width(), headH), Qt::AlignVCenter | Qt::AlignLeft,
               QFontMetrics(p.font()).elidedText(m_summary, Qt::ElideRight, width()));

    const int n = qMax(5, int(m_entries.size()));
    const qreal rowH = (height() - headH) / qreal(n);
    const qreal nameW = qMin<qreal>(96, width() * 0.28);
    const qreal pctW = 50, cntW = 32;
    const QRectF track(nameW + 6, 0, width() - nameW - pctW - cntW - 14, 12);

    for (int i = 0; i < m_entries.size(); ++i) {
        const Entry &e = m_entries[i];
        const int pct = e.total ? qRound(100.0 * e.operationnelles / e.total) : 0;
        const qreal cy = headH + rowH * i + rowH / 2;

        p.setFont(px(13, i == 0));
        p.setPen(QColor("#23252e"));
        p.drawText(QRectF(0, cy - 12, nameW, 24), Qt::AlignVCenter | Qt::AlignLeft,
                   QFontMetrics(p.font()).elidedText(e.type, Qt::ElideRight, int(nameW)));

        QRectF t = track; t.moveCenter(QPointF(track.center().x(), cy));
        p.setPen(Qt::NoPen);
        p.setBrush(QColor("#eee8dc"));
        p.drawRoundedRect(t, 6, 6);
        if (pct > 0) {
            QRectF f = t; f.setWidth(qMax<qreal>(10, t.width() * pct / 100.0));
            p.setBrush(performanceColor(pct));
            p.drawRoundedRect(f, 6, 6);
        }

        p.setFont(px(13, true));
        p.setPen(QColor("#23252e"));
        p.drawText(QRectF(width() - cntW - pctW, cy - 12, pctW, 24), Qt::AlignVCenter | Qt::AlignRight,
                   QStringLiteral("%1 %").arg(pct));
        p.setFont(px(12));
        p.setPen(QColor("#6b6b6b"));
        p.drawText(QRectF(width() - cntW, cy - 12, cntW, 24), Qt::AlignVCenter | Qt::AlignRight,
                   QStringLiteral("%1/%2").arg(e.operationnelles).arg(e.total));
    }
}

/* ------------------------------- PlanningList ------------------------------- */
PlanningList::PlanningList(QWidget *parent) : QWidget(parent) {}

void PlanningList::setData(const QVector<Entry> &entries, const QString &summary)
{
    m_entries = entries;
    m_summary = summary;
    update();
}

void PlanningList::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    const qreal headH = 26;
    p.setFont(px(13, true));
    p.setPen(QColor("#0a2748"));
    p.drawText(QRectF(0, 0, width(), headH), Qt::AlignVCenter | Qt::AlignLeft,
               QFontMetrics(p.font()).elidedText(m_summary, Qt::ElideRight, width()));

    const int n = qMax(5, int(m_entries.size()));
    const qreal rowH = (height() - headH) / qreal(n);
    const qreal dateW = 62;
    const qreal textX = 18;

    for (int i = 0; i < m_entries.size(); ++i) {
        const Entry &e = m_entries[i];
        const qreal top = headH + rowH * i;
        const qreal mid = top + rowH / 2;

        if (i > 0) {
            p.setPen(QColor("#efe9dc"));
            p.drawLine(QPointF(0, top), QPointF(width(), top));
        }

        p.setPen(Qt::NoPen);
        p.setBrush(e.couleur);
        p.drawEllipse(QPointF(6, mid), 4.5, 4.5);

        p.setFont(px(13));
        p.setPen(QColor("#23252e"));
        p.drawText(QRectF(textX, mid - 16, width() - textX - dateW - 6, 16), Qt::AlignVCenter | Qt::AlignLeft,
                   QFontMetrics(p.font()).elidedText(e.nom, Qt::ElideRight, int(width() - textX - dateW - 6)));

        p.setFont(px(13, true));
        p.setPen(QColor("#0a2748"));
        p.drawText(QRectF(width() - dateW, mid - 12, dateW, 24), Qt::AlignVCenter | Qt::AlignRight, e.proposee);

        p.setFont(px(11));
        p.setPen(QColor("#6b6b6b"));
        const QString line2 = e.modifiee ? QStringLiteral("%1 → %2 · %3").arg(e.ancienne, e.proposee, e.raison)
                                         : e.raison;
        p.drawText(QRectF(textX, mid + 1, width() - textX - 4, 15), Qt::AlignVCenter | Qt::AlignLeft,
                   QFontMetrics(p.font()).elidedText(line2, Qt::ElideRight, int(width() - textX - 4)));
    }
}
