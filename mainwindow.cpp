#include "mainwindow.h"
#include <QHeaderView>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QFrame>
#include <QScrollArea>
#include <QFile>
#include <QPixmap>
#include <QPainter>
#include <QPainterPath>
#include <QFont>
#include <QCheckBox>
#include <QUrl>
#include <QButtonGroup>
#include <QRegularExpression>
#include <QTextStream>
#include <QFileDialog>
#include <QMessageBox>
#include <QTextDocument>
#include <QPrinter>
#include <QApplication>
#include <QDateTime>
#include <QScrollBar>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>
#include <QtMath>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QDesktopServices>

static const QString NAVY = "#082849";
static const QString BLUE = "#1B5FA7";
static const QString GOLD = "#D8A64B";

struct ImadaInfo {
    QString name;
    double lat;
    double lon;
};

static QMap<QString, QMap<QString, QList<ImadaInfo>>> tunisiaHierarchy;

static QString cardStyle()
{
    return "QFrame { background:#FFFFFF; border:1px solid #E2E8F0; border-radius:10px; }";
}

static double evaluateMathExpression(QString expr, bool &ok)
{
    expr.replace(" ", "");
    expr.replace(",", ".");

    QRegularExpression rx(R"((-?\d+(?:\.\d+)?)\s*([\+\-\*\/])\s*(-?\d+(?:\.\d+)?))");
    QRegularExpressionMatch match = rx.match(expr);

    if (match.hasMatch()) {
        double a = match.captured(1).toDouble();
        QString op = match.captured(2);
        double b = match.captured(3).toDouble();
        ok = true;
        if (op == "+") return a + b;
        if (op == "-") return a - b;
        if (op == "*") return a * b;
        if (op == "/" && b != 0) return a / b;
    }
    ok = false;
    return 0.0;
}

static QPixmap makeFloatingIconPixmap(const QString &type, int sz = 54)
{
    QPixmap pix(sz, sz);
    pix.fill(Qt::transparent);
    QPainter p(&pix);
    p.setRenderHint(QPainter::Antialiasing);

    if (type == "whatsapp") {
        p.setPen(Qt::NoPen);
        p.setBrush(QColor(0, 0, 0, 45));
        p.drawEllipse(3, 4, sz - 6, sz - 6);

        QRadialGradient grad(sz * 0.35, sz * 0.35, sz * 0.55);
        grad.setColorAt(0.0, QColor(0x3E, 0xEF, 0x7E));
        grad.setColorAt(0.7, QColor(0x25, 0xD3, 0x66));
        grad.setColorAt(1.0, QColor(0x18, 0xA8, 0x4D));
        p.setBrush(grad);
        p.drawEllipse(2, 2, sz - 4, sz - 4);

        p.setPen(QPen(QColor(255, 255, 255, 90), 1.5));
        p.setBrush(Qt::NoBrush);
        p.drawArc(QRectF(3.5, 3.5, sz - 7, sz - 7), 45 * 16, 120 * 16);

        p.setPen(Qt::NoPen);
        p.setBrush(Qt::white);
        QPainterPath bubble;
        bubble.addEllipse(QRectF(sz * 0.23, sz * 0.23, sz * 0.54, sz * 0.54));
        bubble.moveTo(sz * 0.30, sz * 0.65);
        bubble.lineTo(sz * 0.22, sz * 0.77);
        bubble.lineTo(sz * 0.37, sz * 0.73);
        bubble.closeSubpath();
        p.drawPath(bubble);

        p.setBrush(QColor(0x25, 0xD3, 0x66));
        p.drawEllipse(QRectF(sz * 0.29, sz * 0.29, sz * 0.42, sz * 0.42));

        p.setPen(QPen(Qt::white, sz * 0.075, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        p.setBrush(Qt::NoBrush);
        QPainterPath phone;
        phone.moveTo(sz * 0.41, sz * 0.38);
        phone.quadTo(sz * 0.40, sz * 0.50, sz * 0.50, sz * 0.60);
        phone.lineTo(sz * 0.57, sz * 0.57);
        p.drawPath(phone);
    } else {
        p.setPen(Qt::NoPen);
        p.setBrush(QColor(0, 0, 0, 50));
        p.drawEllipse(3, 4, sz - 6, sz - 6);

        QLinearGradient chromeGrad(0, 0, sz, sz);
        chromeGrad.setColorAt(0.0, QColor(0xFD, 0xFD, 0xFD));
        chromeGrad.setColorAt(0.35, QColor(0xD1, 0xD5, 0xDB));
        chromeGrad.setColorAt(0.65, QColor(0x9C, 0xA3, 0xAF));
        chromeGrad.setColorAt(1.0, QColor(0x6B, 0x72, 0x80));
        p.setBrush(chromeGrad);
        p.drawEllipse(2, 2, sz - 4, sz - 4);

        QRadialGradient blueGrad(sz * 0.35, sz * 0.35, sz * 0.55);
        blueGrad.setColorAt(0.0, QColor(0x3B, 0x82, 0xF6));
        blueGrad.setColorAt(0.65, QColor(0x1D, 0x4E, 0xD8));
        blueGrad.setColorAt(1.0, QColor(0x0F, 0x2E, 0x8A));
        p.setBrush(blueGrad);
        p.drawEllipse(5, 5, sz - 10, sz - 10);

        p.setPen(Qt::NoPen);
        p.setBrush(QColor(255, 255, 255, 55));
        p.drawChord(QRectF(6, 6, sz - 12, (sz - 12) * 0.65), 0, 180 * 16);

        p.setBrush(Qt::white);
        QPainterPath bubble;
        bubble.addRoundedRect(QRectF(sz * 0.22, sz * 0.32, sz * 0.56, sz * 0.38), sz * 0.18, sz * 0.18);
        bubble.moveTo(sz * 0.36, sz * 0.68);
        bubble.lineTo(sz * 0.27, sz * 0.76);
        bubble.lineTo(sz * 0.44, sz * 0.69);
        bubble.closeSubpath();
        p.drawPath(bubble);

        p.setPen(QColor(0x1D, 0x4E, 0xD8));
        QFont f("Segoe UI");
        f.setBold(true);
        f.setPixelSize(static_cast<int>(sz * 0.17));
        p.setFont(f);
        p.drawText(QRectF(sz * 0.22, sz * 0.32, sz * 0.56, sz * 0.38), Qt::AlignCenter, "SMS");
    }

    p.end();
    return pix;
}

class SidebarWidget : public QWidget
{
public:
    explicit SidebarWidget(QWidget *parent = nullptr) : QWidget(parent) {}

protected:
    void paintEvent(QPaintEvent *) override
    {
        QPainter p(this);
        QPixmap bg(":/images/sidebar.png");
        if (!bg.isNull()) {
            p.drawPixmap(rect(), bg);
        } else {
            p.fillRect(rect(), QColor(0x08, 0x28, 0x49));
        }
    }
};

static QIcon makeDynamicSidebarIcon(int type)
{
    auto drawPix = [](int t, const QColor &color) -> QPixmap {
        QPixmap pix(24, 24);
        pix.fill(Qt::transparent);
        QPainter p(&pix);
        p.setRenderHint(QPainter::Antialiasing);
        p.setPen(Qt::NoPen);
        p.setBrush(color);

        switch (t) {
        case 0: {
            p.drawEllipse(4, 2, 8, 8);
            QPainterPath b1; b1.moveTo(1, 20); b1.arcTo(1, 10, 14, 12, 0, 180); b1.closeSubpath();
            p.drawPath(b1);
            p.drawEllipse(13, 4, 6, 6);
            QPainterPath b2; b2.moveTo(11, 20); b2.arcTo(11, 11, 11, 10, 0, 180); b2.closeSubpath();
            p.drawPath(b2);
            break;
        }
        case 1: {
            p.drawEllipse(7, 2, 10, 10);
            QPainterPath b; b.moveTo(2, 21); b.arcTo(2, 12, 20, 14, 0, 180); b.closeSubpath();
            p.drawPath(b);
            break;
        }
        case 2: {
            p.setPen(QPen(color, 1.8));
            p.setBrush(Qt::NoBrush);
            p.drawRoundedRect(QRectF(4.0, 4.0, 16.0, 18.0), 2.0, 2.0);
            p.setBrush(color);
            p.setPen(Qt::NoPen);
            p.drawRoundedRect(QRectF(8.0, 2.0, 8.0, 4.0), 1.5, 1.5);
            p.setPen(QPen(color, 1.6));
            p.drawLine(7, 10, 17, 10);
            p.drawLine(7, 14, 17, 14);
            p.drawLine(7, 18, 14, 18);
            break;
        }
        case 3: {
            QPainterPath m;
            m.moveTo(8, 3); m.lineTo(16, 3);
            m.cubicTo(14, 7, 18, 9, 17, 13);
            m.lineTo(7, 13);
            m.cubicTo(6, 9, 10, 7, 8, 3);
            p.drawPath(m);
            p.setPen(QPen(color, 2));
            p.drawLine(12, 13, 12, 21);
            p.drawLine(8, 21, 16, 21);
            break;
        }
        case 4: {
            p.drawEllipse(6, 6, 12, 12);
            for (int i = 0; i < 8; ++i) {
                p.save();
                p.translate(12, 12);
                p.rotate(i * 45);
                p.drawRect(QRectF(-2.0, -10.0, 4.0, 3.0));
                p.restore();
            }
            p.setCompositionMode(QPainter::CompositionMode_Clear);
            p.drawEllipse(9, 9, 6, 6);
            break;
        }
        case 5: {
            QPainterPath tag;
            tag.moveTo(3, 13); tag.lineTo(11, 21); tag.lineTo(21, 11);
            tag.lineTo(21, 3); tag.lineTo(13, 3);
            tag.closeSubpath();
            p.drawPath(tag);
            p.setCompositionMode(QPainter::CompositionMode_Clear);
            p.drawEllipse(16, 5, 3, 3);
            break;
        }
        }
        p.end();
        return pix;
    };

    QIcon icon;
    icon.addPixmap(drawPix(type, QColor(0xFF, 0xFF, 0xFF)), QIcon::Normal, QIcon::Off);
    icon.addPixmap(drawPix(type, QColor(0x08, 0x21, 0x3B)), QIcon::Normal, QIcon::On);
    return icon;
}

enum ActionIconType { IconEye, IconTrash, IconCar, IconLocation, IconFile };

static QIcon makeButtonIcon(ActionIconType type, const QColor &color, int size = 18)
{
    QPixmap pix(size, size);
    pix.fill(Qt::transparent);
    QPainter p(&pix);
    p.setRenderHint(QPainter::Antialiasing);

    switch (type) {
    case IconEye: {
        p.setPen(QPen(color, 1.8, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        p.setBrush(Qt::NoBrush);
        QPainterPath path;
        path.moveTo(2, size / 2.0);
        path.quadTo(size / 2.0, 2, size - 2, size / 2.0);
        path.quadTo(size / 2.0, size - 2, 2, size / 2.0);
        p.drawPath(path);
        p.setPen(Qt::NoPen);
        p.setBrush(color);
        p.drawEllipse(QPointF(size / 2.0, size / 2.0), 3.2, 3.2);
        break;
    }
    case IconTrash: {
        p.setPen(QPen(color, 1.6, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        p.setBrush(Qt::NoBrush);
        QPainterPath bac;
        bac.moveTo(4, 5); bac.lineTo(5, size - 2); bac.lineTo(size - 5, size - 2); bac.lineTo(size - 4, 5);
        p.drawPath(bac);
        p.drawLine(2, 5, size - 2, 5);
        p.drawLine(6, 3, size - 6, 3);
        p.drawLine(7, 8, 7, size - 4);
        p.drawLine(size / 2, 8, size / 2, size - 4);
        p.drawLine(size - 7, 8, size - 7, size - 4);
        break;
    }
    case IconCar: {
        p.setPen(Qt::NoPen);
        p.setBrush(color);
        QPainterPath car;
        car.moveTo(2, size - 6);
        car.lineTo(2, size - 9);
        car.lineTo(5, size - 12);
        car.lineTo(size - 7, size - 12);
        car.lineTo(size - 3, size - 9);
        car.lineTo(size - 2, size - 6);
        car.closeSubpath();
        p.drawPath(car);
        p.setBrush(QColor(0x10, 0xB9, 0x81));
        p.drawEllipse(QPointF(5, size - 4), 2.2, 2.2);
        p.drawEllipse(QPointF(size - 5, size - 4), 2.2, 2.2);
        break;
    }
    case IconLocation: {
        p.setPen(Qt::NoPen);
        p.setBrush(color);
        QPainterPath pin;
        pin.moveTo(size / 2.0, size - 1);
        pin.cubicTo(size / 2.0, size - 4, 2, size / 2.0 + 1, 2, size / 2.0 - 2);
        pin.arcTo(2, 2, size - 4, size - 4, 180, -180);
        pin.cubicTo(size - 2, size / 2.0 + 1, size / 2.0, size - 4, size / 2.0, size - 1);
        pin.closeSubpath();
        p.drawPath(pin);
        p.setCompositionMode(QPainter::CompositionMode_Clear);
        p.drawEllipse(QPointF(size / 2.0, size / 2.0 - 1.5), 2.5, 2.5);
        break;
    }
    case IconFile: {
        p.setPen(QPen(color, 1.6));
        p.setBrush(Qt::NoBrush);
        p.drawRoundedRect(QRectF(3.0, 2.0, size - 6.0, size - 4.0), 2.0, 2.0);
        p.drawLine(6, 6, size - 6, 6);
        p.drawLine(6, 10, size - 6, 10);
        p.drawLine(6, 14, size - 9, 14);
        break;
    }
    }
    p.end();
    return QIcon(pix);
}

static QPixmap makeKpiIcon(int type, const QColor &color)
{
    QPixmap pix(34, 34);
    pix.fill(Qt::transparent);
    QPainter p(&pix);
    p.setRenderHint(QPainter::Antialiasing);

    switch (type) {
    case 0: {
        p.setPen(QPen(color, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        p.setBrush(Qt::NoBrush);
        p.drawRoundedRect(QRectF(7.0, 10.0, 20.0, 15.0), 2.0, 2.0);
        QPainterPath path;
        path.moveTo(7, 10); path.lineTo(17, 17); path.lineTo(27, 10);
        p.drawPath(path);
        p.drawLine(17, 17, 17, 25);
        break;
    }
    case 1: {
        p.setPen(QPen(color, 2));
        p.setBrush(Qt::NoBrush);
        p.drawEllipse(7, 7, 20, 20);
        p.setPen(QPen(color, 2, Qt::SolidLine, Qt::RoundCap));
        p.drawLine(17, 17, 17, 11);
        p.drawLine(17, 17, 22, 17);
        break;
    }
    case 2: {
        p.setPen(Qt::NoPen);
        p.setBrush(color);
        p.drawRoundedRect(QRectF(5.0, 9.0, 14.0, 13.0), 1.5, 1.5);
        QPainterPath cab;
        cab.moveTo(19, 13); cab.lineTo(25, 13); cab.lineTo(28, 17);
        cab.lineTo(28, 22); cab.lineTo(19, 22);
        cab.closeSubpath();
        p.drawPath(cab);
        p.setBrush(QColor(0xFF, 0xFF, 0xFF));
        p.drawEllipse(8, 20, 5, 5);
        p.drawEllipse(22, 20, 5, 5);
        p.setBrush(color);
        p.drawEllipse(9, 21, 3, 3);
        p.drawEllipse(23, 21, 3, 3);
        break;
    }
    case 3: {
        p.setPen(Qt::NoPen);
        p.setBrush(color);
        p.drawEllipse(10, 10, 14, 14);
        for (int i = 0; i < 8; ++i) {
            p.save();
            p.translate(17, 17);
            p.rotate(i * 45);
            p.drawRoundedRect(QRectF(-2.2, -10.5, 4.4, 3.5), 1.0, 1.0);
            p.restore();
        }
        p.setCompositionMode(QPainter::CompositionMode_Clear);
        p.drawEllipse(13, 13, 8, 8);
        break;
    }
    case 4: {
        p.setPen(QPen(color, 2));
        p.setBrush(Qt::NoBrush);
        p.drawEllipse(8, 11, 18, 17);
        p.drawLine(13, 10, 21, 10);
        p.drawLine(14, 7, 17, 10);
        p.drawLine(20, 7, 17, 10);
        p.setPen(color);
        QFont f("Segoe UI");
        f.setBold(true);
        f.setPixelSize(9);
        p.setFont(f);
        p.drawText(QRect(8, 11, 18, 17), Qt::AlignCenter, "$");
        break;
    }
    }
    p.end();
    return pix;
}

static QPixmap makeSparklineWithGradient(const QColor &baseColor, bool isDown, int w = 60, int h = 34)
{
    QPixmap pix(w, h);
    pix.fill(Qt::transparent);
    QPainter p(&pix);
    p.setRenderHint(QPainter::Antialiasing);

    QPolygonF curve;
    if (!isDown) {
        curve << QPointF(2, h - 5)
        << QPointF(12, h - 11)
        << QPointF(22, h - 8)
        << QPointF(32, h - 18)
        << QPointF(42, h - 14)
        << QPointF(52, h - 26)
        << QPointF(w - 2, 4);
    } else {
        curve << QPointF(2, 5)
        << QPointF(12, 11)
        << QPointF(22, 8)
        << QPointF(32, 18)
        << QPointF(42, 14)
        << QPointF(52, 26)
        << QPointF(w - 2, h - 4);
    }

    QPainterPath fillPath;
    fillPath.moveTo(curve.first());
    for (int i = 1; i < curve.size(); ++i) fillPath.lineTo(curve[i]);
    fillPath.lineTo(curve.last().x(), h);
    fillPath.lineTo(curve.first().x(), h);
    fillPath.closeSubpath();

    QLinearGradient grad(0, 0, 0, h);
    QColor topCol = baseColor;
    topCol.setAlpha(65);
    QColor botCol = baseColor;
    botCol.setAlpha(0);
    grad.setColorAt(0, topCol);
    grad.setColorAt(1, botCol);

    p.setPen(Qt::NoPen);
    p.setBrush(grad);
    p.drawPath(fillPath);

    p.setPen(QPen(baseColor, 1.8, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    p.setBrush(Qt::NoBrush);
    for (int i = 0; i < curve.size() - 1; ++i) {
        p.drawLine(curve[i], curve[i + 1]);
    }

    p.end();
    return pix;
}

static QPixmap makeBarChartPixmap(const QMap<QString, int> &govCounts, int w = 380, int h = 135)
{
    QPixmap pix(w, h);
    pix.fill(Qt::transparent);
    QPainter p(&pix);
    p.setRenderHint(QPainter::Antialiasing);

    QList<QPair<QString, int>> list;
    for (auto it = govCounts.begin(); it != govCounts.end(); ++it) {
        if (it.value() > 0) list.append({it.key(), it.value()});
    }

    std::sort(list.begin(), list.end(), [](const QPair<QString, int> &a, const QPair<QString, int> &b) {
        return a.second > b.second;
    });

    if (list.size() > 5) list = list.mid(0, 5);

    if (list.isEmpty()) {
        p.setPen(QColor(0x94, 0xA3, 0xB8));
        QFont f("Segoe UI", 9, QFont::DemiBold);
        p.setFont(f);
        p.drawText(QRect(0, 0, w, h), Qt::AlignCenter, "Aucune commande enregistrée");
        p.end();
        return pix;
    }

    int maxVal = 0;
    for (const auto &item : list) if (item.second > maxVal) maxVal = item.second;
    if (maxVal < 4) maxVal = 4;

    int leftMargin = 30;
    int bottomMargin = 25;
    int chartW = w - leftMargin - 15;
    int chartH = h - bottomMargin - 15;

    p.setPen(QPen(QColor(0xE2, 0xE8, 0xF0), 1, Qt::DashLine));
    for (int i = 0; i <= maxVal; i += qMax(1, maxVal / 3)) {
        int y = 10 + chartH - (i * chartH / maxVal);
        p.drawLine(leftMargin, y, w - 15, y);
        p.setPen(QColor(0x94, 0xA3, 0xB8));
        QFont f("Segoe UI", 7.5);
        p.setFont(f);
        p.drawText(QRect(0, y - 6, leftMargin - 5, 12), Qt::AlignRight | Qt::AlignVCenter, QString::number(i));
        p.setPen(QPen(QColor(0xE2, 0xE8, 0xF0), 1, Qt::DashLine));
    }

    int count = list.size();
    int slotW = chartW / count;
    int barW = qMin(34, slotW - 14);

    for (int i = 0; i < count; ++i) {
        int val = list[i].second;
        int barH = (val * chartH) / maxVal;
        int x = leftMargin + (i * slotW) + (slotW - barW) / 2;
        int y = 10 + chartH - barH;

        QLinearGradient barGrad(x, y, x + barW, y);
        barGrad.setColorAt(0.0, QColor(0x1B, 0x5F, 0xA7));
        barGrad.setColorAt(0.4, QColor(0x3B, 0x82, 0xF6));
        barGrad.setColorAt(1.0, QColor(0x08, 0x28, 0x49));

        p.setPen(Qt::NoPen);
        p.setBrush(barGrad);
        p.drawRoundedRect(QRect(x, y, barW, barH), 4, 4);

        p.setPen(QColor(0x08, 0x28, 0x49));
        QFont fVal("Segoe UI", 8.5, QFont::Bold);
        p.setFont(fVal);
        p.drawText(QRect(x - 5, y - 14, barW + 10, 12), Qt::AlignCenter, QString::number(val));

        p.setPen(QColor(0x47, 0x55, 0x69));
        QFont fName("Segoe UI", 8, QFont::DemiBold);
        p.setFont(fName);
        p.drawText(QRect(x - 14, 10 + chartH + 4, barW + 28, 16), Qt::AlignCenter, list[i].first);
    }

    p.end();
    return pix;
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    formFrame(nullptr),
    formIdEdit(nullptr),
    idErrorLabel(nullptr),
    formClientCodeEdit(nullptr),
    clientCodeErrorLabel(nullptr),
    formClientNameEdit(nullptr),
    clientNameErrorLabel(nullptr),
    formClientPhoneEdit(nullptr),
    clientPhoneErrorLabel(nullptr),
    formAmountEdit(nullptr),
    amountErrorLabel(nullptr),
    comboWilaya(nullptr),
    comboDelegation(nullptr),
    comboImada(nullptr),
    formDateEdit(nullptr),
    formStatusCombo(nullptr),
    formArticleCombo(nullptr),
    formPaymentCombo(nullptr),
    formPreviewImage(nullptr),
    smartDeliveryDateLabel(nullptr),
    btnAdd(nullptr),
    btnEdit(nullptr),
    btnDel(nullptr),
    tableContainerFrame(nullptr),
    detailsPanelContainer(nullptr),
    detailPhone(nullptr),
    btnWhatsAppFloating(nullptr),
    btnSmsFloating(nullptr),
    donutLabel(nullptr),
    legendLayout(nullptr),
    donutAnim(nullptr),
    govBarChartLabel(nullptr),
    animCountLivree(0),
    animCountEnCours(0),
    animCountPrete(0),
    animTotalCmd(0),
    kpiTotalVal(nullptr),
    kpiEnAttenteVal(nullptr),
    kpiExpedieesVal(nullptr),
    kpiAtelierVal(nullptr),
    kpiMontantTotalVal(nullptr),
    sortAscending(true),
    currentRowSelected(0),
    chatFloatingBtn(nullptr),
    chatWindow(nullptr)
{
    resize(1440, 960);
    setMinimumSize(1100, 680);
    setWindowTitle("FASHIONOVA — Gestion des Commandes");

    initTunisianLocationData();

    networkManager = new QNetworkAccessManager(this);
    connect(networkManager, &QNetworkAccessManager::finished, this, &MainWindow::onAiResponseReceived);
    initAIAgent();

    donutAnim = new QVariantAnimation(this);
    donutAnim->setDuration(600);
    donutAnim->setStartValue(0.0);
    donutAnim->setEndValue(1.0);
    donutAnim->setEasingCurve(QEasingCurve::OutCubic);
    connect(donutAnim, &QVariantAnimation::valueChanged, this, &MainWindow::onDonutAnimationStep);

    setStyleSheet(QString(R"(
        QWidget { font-family: "Segoe UI", sans-serif; color: #1E293B; font-size: 11px; }
        QMainWindow { background-color: #F8FAFC; }
        QScrollArea { background-color: #F8FAFC; border: none; }
        QLineEdit, QComboBox, QDateEdit {
            background-color: #FFFFFF; border: 1px solid #CBD5E1; border-radius: 5px;
            padding: 3px 6px; color: #1E293B; font-size: 11px;
        }
        QLineEdit:focus, QComboBox:focus, QDateEdit:focus { border: 1px solid #1B5FA7; }
        QTableWidget {
            background-color: #FFFFFF; border: none; gridline-color: #F1F5F9;
            selection-background-color: #EFF6FF; selection-color: #1E293B;
        }
        QHeaderView::section {
            background-color: #F8FAFC; color: #64748B; padding: 5px 4px;
            border: none; border-bottom: 1px solid #E2E8F0;
            font-weight: 600; font-size: 10px;
        }
        QScrollBar:vertical { width: 6px; background: transparent; }
        QScrollBar::handle:vertical { background: #CBD5E1; border-radius: 3px; }

        QMessageBox {
            background-color: #082849;
            border: 1px solid #1B5FA7;
            border-radius: 8px;
        }
        QMessageBox QLabel {
            color: #FFFFFF !important;
            font-size: 11.5px;
            font-weight: 500;
        }
        QMessageBox QPushButton {
            background-color: #1B5FA7;
            color: #FFFFFF !important;
            border: 1px solid #3B82F6;
            border-radius: 4px;
            padding: 4px 14px;
            font-weight: bold;
            min-width: 65px;
        }
        QMessageBox QPushButton:hover {
            background-color: #2563EB;
        }
    )"));

    auto *root = new QWidget;
    root->setStyleSheet("background-color: #F8FAFC;");
    auto *rootLayout = new QHBoxLayout(root);
    rootLayout->setContentsMargins(0, 0, 0, 0);
    rootLayout->setSpacing(0);

    rootLayout->addWidget(makeSidebar(), 0);

    auto *scrollArea = new QScrollArea;
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    auto *scrollContent = new QWidget;
    scrollContent->setStyleSheet("background-color: #F8FAFC;");
    auto *main = new QVBoxLayout(scrollContent);
    main->setContentsMargins(10, 6, 10, 8);
    main->setSpacing(10);

    main->addWidget(makeHeader(), 0);
    main->addWidget(makeOrdersAndFormCombinedPanel(), 0);
    main->addWidget(makeMapPanel(), 0);

    detailsPanelContainer = makeDetailsPanel();
    detailsPanelContainer->setFixedWidth(1310);
    main->addWidget(detailsPanelContainer, 0);

    main->addWidget(makeKpis(), 0);
    main->addWidget(makeBottomPanel(), 0);

    scrollArea->setWidget(scrollContent);

    pageStack = new QStackedWidget;
    pageStack->setStyleSheet("background-color: #F8FAFC; border: none;");

    pageStack->addWidget(createEmptyPage("Clients"));
    pageStack->addWidget(createEmptyPage("Employés"));
    pageStack->addWidget(scrollArea);
    pageStack->addWidget(createEmptyPage("Maquettes"));
    pageStack->addWidget(createEmptyPage("Machines"));
    pageStack->addWidget(createEmptyPage("Articles"));

    pageStack->setCurrentIndex(2);

    rootLayout->addWidget(pageStack, 1);
    setCentralWidget(root);

    setupOrders();
    setupFloatingChatbot();
    setupFloatingActionButtons();
}

void MainWindow::initTunisianLocationData()
{
    tunisiaHierarchy.clear();

    tunisiaHierarchy["Tunis"]["Les Berges du Lac"] = { {"Lac 1", 36.8333, 10.2333}, {"Lac 2", 36.8400, 10.2600} };
    tunisiaHierarchy["Tunis"]["Tunis Médina"] = { {"Bab Souika", 36.8080, 10.1650}, {"El Hafsia", 36.8010, 10.1740} };
    tunisiaHierarchy["Tunis"]["La Marsa"] = { {"Marsa Plage", 36.8782, 10.3247}, {"Gammarth", 36.9189, 10.2889} };
    tunisiaHierarchy["Tunis"]["Le Bardo"] = { {"Bardo Centre", 36.8092, 10.1406}, {"Ksar Saïd", 36.8150, 10.1320} };

    tunisiaHierarchy["Ariana"]["Ariana Ville"] = { {"Nouvelle Ariana", 36.8665, 10.1647}, {"Menzah 8", 36.8520, 10.1550} };
    tunisiaHierarchy["Ariana"]["La Soukra"] = { {"Chotrana", 36.8830, 10.2100}, {"Dar Fadhal", 36.8740, 10.2310} };
    tunisiaHierarchy["Ariana"]["Raoued"] = { {"Enkhilet", 36.9050, 10.1800}, {"Raoued Plage", 36.9250, 10.2100} };

    tunisiaHierarchy["Ben Arous"]["Ben Arous Centre"] = { {"Sidi Rezig", 36.7533, 10.2222}, {"Zone Industrielle", 36.7480, 10.2300} };
    tunisiaHierarchy["Ben Arous"]["Radès"] = { {"Radès Forêt", 36.7680, 10.2750}, {"Radès Plage", 36.7720, 10.2850} };
    tunisiaHierarchy["Ben Arous"]["Megrine"] = { {"Megrine Riadh", 36.7700, 10.2400}, {"Megrine Coteaux", 36.7650, 10.2350} };

    tunisiaHierarchy["Manouba"]["Manouba Ville"] = { {"Denden", 36.8080, 10.0972}, {"Ksar Slama", 36.8120, 10.0890} };
    tunisiaHierarchy["Manouba"]["Oued Ellil"] = { {"Cité Ennajat", 36.8250, 10.0500}, {"Sanheja", 36.8350, 10.0350} };

    tunisiaHierarchy["Nabeul"]["Nabeul Ville"] = { {"Bir Challouf", 36.4561, 10.7376}, {"El Faouara", 36.4620, 10.7250} };
    tunisiaHierarchy["Nabeul"]["Hammamet"] = { {"Yasmine Hammamet", 36.4000, 10.6167}, {"Hammamet Nord", 36.4150, 10.6250} };
    tunisiaHierarchy["Nabeul"]["Kélibia"] = { {"Kélibia Port", 36.8475, 11.0939}, {"El Mansourah", 36.8550, 11.1100} };

    tunisiaHierarchy["Zaghouan"]["Zaghouan Ville"] = { {"Sidi Bougabrine", 36.4028, 10.1429}, {"El Moghrane", 36.4250, 10.1200} };
    tunisiaHierarchy["Zaghouan"]["El Fahs"] = { {"El Fahs Centre", 36.3740, 9.9050} };

    tunisiaHierarchy["Bizerte"]["Bizerte Nord"] = { {"Vieux Port", 37.2744, 9.8739}, {"Corniche", 37.2850, 9.8800} };
    tunisiaHierarchy["Bizerte"]["Menzel Bourguiba"] = { {"Ennajah", 37.1530, 9.7850} };
    tunisiaHierarchy["Bizerte"]["Ras Jebel"] = { {"Ghar El Melh", 37.1680, 10.1900} };

    tunisiaHierarchy["Béja"]["Béja Nord"] = { {"Béja Centre", 36.7256, 9.1817}, {"Zouagha", 36.7350, 9.1950} };
    tunisiaHierarchy["Béja"]["Medjez El Bab"] = { {"Medjez Centre", 36.6500, 9.6100} };

    tunisiaHierarchy["Jendouba"]["Jendouba Ville"] = { {"Hédi Ben Hassine", 36.5011, 8.7802}, {"Cité El Khadra", 36.5100, 8.7900} };
    tunisiaHierarchy["Jendouba"]["Tabarka"] = { {"Port Tabarka", 36.9544, 8.7580}, {"Aïn Draham", 36.7800, 8.6900} };

    tunisiaHierarchy["Le Kef"]["Le Kef Est"] = { {"Kef Ville", 36.1822, 8.7149}, {"Chrife", 36.1750, 8.7250} };
    tunisiaHierarchy["Le Kef"]["Dahmani"] = { {"Dahmani Centre", 35.9400, 8.8300} };

    tunisiaHierarchy["Siliana"]["Siliana Ville"] = { {"Siliana Centre", 36.0850, 9.3708}, {"Jama", 36.0950, 9.3600} };
    tunisiaHierarchy["Siliana"]["Makthar"] = { {"Makthar Centre", 35.8580, 9.2050} };

    tunisiaHierarchy["Sousse"]["Sousse Ville"] = { {"Khezama", 35.8450, 10.6180}, {"Corniche", 35.8320, 10.6369} };
    tunisiaHierarchy["Sousse"]["Hammam Sousse"] = { {"Port El Kantaoui", 35.8940, 10.5980} };
    tunisiaHierarchy["Sousse"]["Msaken"] = { {"Msaken Centre", 35.7300, 10.5800} };

    tunisiaHierarchy["Monastir"]["Monastir Ville"] = { {"Skanès", 35.7779, 10.8261}, {"Ksar Hellal", 35.6500, 10.8900} };
    tunisiaHierarchy["Monastir"]["Sahline"] = { {"Sahline Centre", 35.7500, 10.7100} };

    tunisiaHierarchy["Mahdia"]["Mahdia Ville"] = { {"Borj Erras", 35.5047, 11.0622}, {"Zone Touristique", 35.5150, 11.0500} };
    tunisiaHierarchy["Mahdia"]["El Jem"] = { {"Amphithéâtre", 35.3000, 10.7100} };

    tunisiaHierarchy["Sfax"]["Sfax Ville"] = { {"Sfax Médina", 34.7406, 10.7603}, {"Picville", 34.7450, 10.7520} };
    tunisiaHierarchy["Sfax"]["Sakiet Ezzit"] = { {"Sakiet Centre", 34.7950, 10.7700} };
    tunisiaHierarchy["Sfax"]["Sakiet Eddaier"] = { {"Eddaier Centre", 34.7800, 10.8100} };

    tunisiaHierarchy["Kairouan"]["Kairouan Nord"] = { {"Grande Mosquée", 35.6781, 10.0963}, {"Ansar", 35.6850, 10.1050} };
    tunisiaHierarchy["Kairouan"]["Bouhajla"] = { {"Bouhajla Centre", 35.4000, 10.0500} };

    tunisiaHierarchy["Kasserine"]["Laayoun"] = { {"Centre Laayoun", 35.5348, 8.8789}, {"El Brek", 35.5450, 8.8650}, {"Tiwicha", 35.5250, 8.8950} };
    tunisiaHierarchy["Kasserine"]["Thala"] = { {"Thala Ville", 35.5714, 8.6703}, {"Ain Eddefla", 35.5850, 8.6550} };
    tunisiaHierarchy["Kasserine"]["Sbiba"] = { {"Sbiba Centre", 35.5431, 9.0734}, {"Wadi El Htab", 35.5310, 9.0850} };
    tunisiaHierarchy["Kasserine"]["Sbeitla"] = { {"Sbeitla Ville", 35.2289, 9.1294}, {"Athar Sbeitla", 35.2410, 9.1180} };
    tunisiaHierarchy["Kasserine"]["Kasserine Nord"] = { {"Ezzouhour", 35.1800, 8.8300}, {"Cité Ennour", 35.1720, 8.8410} };
    tunisiaHierarchy["Kasserine"]["Feriana"] = { {"Feriana Centre", 34.9542, 8.5708} };
    tunisiaHierarchy["Kasserine"]["Foussana"] = { {"Foussana Centre", 35.3400, 8.6800} };

    tunisiaHierarchy["Sidi Bouzid"]["Sidi Bouzid Ouest"] = { {"Sidi Bouzid Centre", 35.0382, 9.4849}, {"Ennour", 35.0450, 9.4900} };
    tunisiaHierarchy["Sidi Bouzid"]["Regueb"] = { {"Regueb Centre", 34.8600, 9.7800} };

    tunisiaHierarchy["Gafsa"]["Gafsa Ville"] = { {"Piscines Romaines", 34.4250, 8.7842}, {"Cité Ennour", 34.4350, 8.7900} };
    tunisiaHierarchy["Gafsa"]["Métlaoui"] = { {"Métlaoui Centre", 34.3300, 8.4000} };

    tunisiaHierarchy["Tozeur"]["Tozeur Ville"] = { {"Palmeraie Tozeur", 33.9197, 8.1335}, {"Ouled El Hadef", 33.9250, 8.1400} };
    tunisiaHierarchy["Tozeur"]["Nefta"] = { {"Corbeille Nefta", 33.8700, 7.8800} };

    tunisiaHierarchy["Kébili"]["Kébili Ville"] = { {"Kébili Centre", 33.7044, 8.9690}, {"Jemna", 33.5700, 9.0100} };
    tunisiaHierarchy["Kébili"]["Douz"] = { {"Douz Ville", 33.4600, 9.0200} };

    tunisiaHierarchy["Gabès"]["Gabès Ville"] = { {"Jara", 33.8815, 10.0982}, {"Bab Bhar", 33.8850, 10.1050} };
    tunisiaHierarchy["Gabès"]["Matmata"] = { {"Matmata Ancienne", 33.5400, 9.9700} };

    tunisiaHierarchy["Médenine"]["Médenine Ville"] = { {"Médenine Centre", 33.3549, 10.5055} };
    tunisiaHierarchy["Médenine"]["Djerba Houmt Souk"] = { {"Houmt Souk", 33.8750, 10.8550}, {"Midoun", 33.8075, 10.9900} };
    tunisiaHierarchy["Médenine"]["Zarzis"] = { {"Zarzis Port", 33.5040, 11.1122} };

    tunisiaHierarchy["Tataouine"]["Tataouine Ville"] = { {"Tataouine Centre", 32.9297, 10.4518}, {"Ksar Ouled Soltane", 32.7800, 10.5100} };
    tunisiaHierarchy["Tataouine"]["Ghomrassen"] = { {"Ghomrassen Centre", 33.0600, 10.3300} };
}

QPair<double, double> MainWindow::getCoordinatesFromAddress(const QString &wilaya, const QString &delegation, const QString &imada)
{
    if (tunisiaHierarchy.contains(wilaya) && tunisiaHierarchy[wilaya].contains(delegation)) {
        const auto &list = tunisiaHierarchy[wilaya][delegation];
        for (const auto &item : list) {
            if (item.name.compare(imada, Qt::CaseInsensitive) == 0) {
                return {item.lat, item.lon};
            }
        }
        if (!list.isEmpty()) return {list.first().lat, list.first().lon};
    }
    return {36.8065, 10.1815};
}

bool MainWindow::validateTunisianPhoneNumber(const QString &clean, QString &op)
{
    if (clean.length() != 8) return false;
    bool isDigitsOnly = false;
    clean.toLongLong(&isDigitsOnly);
    if (!isDigitsOnly) return false;

    int p2 = clean.left(2).toInt();
    int p4 = clean.left(4).toInt();
    int p5 = clean.left(5).toInt();

    if (clean.startsWith("7")) {
        op = "Tunisie Télécom (Fixe)";
        return true;
    }
    if ((p5 >= 30010 && p5 <= 30019) || (p4 >= 3110 && p4 <= 3140) ||
        (p4 >= 3150 && p4 <= 3159)   || (p5 >= 32400 && p5 <= 32409)) {
        op = "Orange Tunisie (Fixe)";
        return true;
    }
    if ((p4 >= 3600 && p4 <= 3649) || (p4 >= 3910 && p4 <= 3919)) {
        op = "Ooredoo Tunisie (Fixe)";
        return true;
    }

    if (clean.startsWith("9") || p2 == 40 || p2 == 41 || p2 == 44 || (p4 >= 4210 && p4 <= 4269)) {
        op = "Tunisie Télécom (Mobile)";
        return true;
    }
    if (p2 == 20 || p2 == 21 || p2 == 22 || p2 == 23 || p2 == 24 || p2 == 25 || p2 == 26 || p2 == 27 || p2 == 28 || p2 == 29 || (p5 >= 46000 && p5 <= 46009)) {
        op = "Ooredoo Tunisie (Mobile)";
        return true;
    }
    if ((p2 >= 50 && p2 <= 55) || p2 == 58 || (p5 >= 56000 && p5 <= 56009)) {
        op = "Orange Tunisie (Mobile)";
        return true;
    }

    if (p5 >= 43000 && p5 <= 43009) {
        op = "Lyca Mobile";
        return true;
    }
    if (p5 >= 45000 && p5 <= 45009) {
        op = "Watany Ettisalat";
        return true;
    }

    if (p2 == 80 || p2 == 81 || p2 == 82 || p2 == 88) {
        op = "Service Spécial (VAS)";
        return true;
    }

    return false;
}

QWidget* MainWindow::createEmptyPage(const QString &title)
{
    auto *page = new QWidget;
    page->setStyleSheet("background-color: #F8FAFC;");

    auto *layout = new QVBoxLayout(page);
    layout->setContentsMargins(40, 40, 40, 40);
    layout->setAlignment(Qt::AlignCenter);

    auto *card = new QFrame;
    card->setStyleSheet("background: #FFFFFF; border: 1px dashed #CBD5E1; border-radius: 12px; padding: 40px;");
    auto *cardLayout = new QVBoxLayout(card);
    cardLayout->setAlignment(Qt::AlignCenter);
    cardLayout->setSpacing(12);

    auto *titleLbl = new QLabel(QString("<h2 style='color: #082849; margin: 0;'>Module %1</h2>").arg(title));
    titleLbl->setAlignment(Qt::AlignCenter);

    auto *descLbl = new QLabel("<p style='color: #64748B; font-size: 13px; margin: 0;'>Cet espace est prêt pour le développement de l'interface.</p>");
    descLbl->setAlignment(Qt::AlignCenter);

    cardLayout->addWidget(titleLbl);
    cardLayout->addWidget(descLbl);
    layout->addWidget(card);

    return page;
}

QWidget* MainWindow::makeSidebar()
{
    auto *side = new SidebarWidget;
    side->setFixedWidth(200);

    auto *ol = new QVBoxLayout(side);
    ol->setContentsMargins(0, 175, 0, 15);
    ol->setSpacing(6);

    auto *btnGroup = new QButtonGroup(side);
    btnGroup->setExclusive(true);

    const QStringList labels = {"Clients", "Employés", "Commandes", "Maquettes", "Machines", "Articles"};

    for (int i = 0; i < labels.size(); ++i) {
        auto *b = new QPushButton(labels[i]);
        b->setCursor(Qt::PointingHandCursor);
        b->setFixedHeight(40);
        b->setIconSize(QSize(20, 20));
        b->setIcon(makeDynamicSidebarIcon(i));

        b->setStyleSheet(R"(
            QPushButton {
                text-align: left;
                padding-left: 20px;
                border: none;
                color: #FFFFFF;
                background: transparent;
                font-size: 13px;
                font-weight: 500;
            }
            QPushButton:hover {
                background: rgba(255, 255, 255, 0.12);
                color: #FFFFFF;
                border-radius: 6px;
                margin: 0 6px;
            }
            QPushButton:checked {
                background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #E5C378, stop:1 #C89538);
                color: #082849;
                border-radius: 6px;
                margin: 0 6px;
                font-weight: bold;
            }
        )");
        b->setCheckable(true);
        btnGroup->addButton(b, i);
        if (i == 2) b->setChecked(true);
        ol->addWidget(b);
    }

    connect(btnGroup, &QButtonGroup::idClicked, this, &MainWindow::onSidebarButtonClicked);
    ol->addStretch();
    return side;
}

void MainWindow::onSidebarButtonClicked(int id)
{
    if (pageStack && id >= 0 && id < pageStack->count()) {
        pageStack->setCurrentIndex(id);
    }
}

QWidget* MainWindow::makeHeader()
{
    auto *header = new QWidget;
    header->setStyleSheet("background: transparent;");
    auto *v = new QVBoxLayout(header);
    v->setContentsMargins(0, 0, 0, 0);
    v->setSpacing(4);

    auto *top = new QHBoxLayout;
    auto *menu = new QLabel("☰");
    menu->setStyleSheet("font-size: 16px; color: #082849; padding-right: 2px;");
    top->addWidget(menu);

    searchEdit = new QLineEdit;
    searchEdit->setPlaceholderText("🔍  Rechercher une commande, un client, un produit...");
    searchEdit->setFixedHeight(28);
    searchEdit->setStyleSheet("QLineEdit { background:#FFFFFF; border:1px solid #E2E8F0; border-radius:14px; padding:0 10px; font-size:11px; }");
    top->addWidget(searchEdit, 1);

    auto *bell = new QLabel("🔔");
    bell->setStyleSheet("font-size: 14px; color: #E11D48; margin: 0 4px;");
    top->addWidget(bell);

    auto *avatar = new QLabel("👤");
    avatar->setFixedSize(26, 26);
    avatar->setAlignment(Qt::AlignCenter);
    avatar->setStyleSheet("background: #082849; color: white; border-radius: 13px; font-size: 12px;");
    top->addWidget(avatar);

    auto *user = new QLabel("<b>Mohamed Anoir</b><br><span style='color:#64748B; font-size:9.5px;'>Admin</span>");
    top->addWidget(user);
    v->addLayout(top);

    auto *banner = new QLabel;
    banner->setFixedHeight(80);
    banner->setScaledContents(true);
    banner->setFixedWidth(1310);
    QPixmap p(":/images/image_tete.png");
    if (!p.isNull()) banner->setPixmap(p);
    else banner->setStyleSheet("background: #FFFDF9; border: 1px solid #F1E5D1; border-radius: 6px;");
    v->addWidget(banner);

    return header;
}

QWidget* MainWindow::makeOrdersAndFormCombinedPanel()
{
    auto *container = new QWidget;
    container->setStyleSheet("background: transparent;");
    container->setFixedWidth(1310);
    auto *hBox = new QHBoxLayout(container);
    hBox->setContentsMargins(0, 0, 0, 0);
    hBox->setSpacing(10);

    formFrame = new QFrame;
    formFrame->setStyleSheet(cardStyle());
    formFrame->setFixedWidth(380);

    auto *leftLayout = new QVBoxLayout(formFrame);
    leftLayout->setContentsMargins(12, 10, 12, 10);
    leftLayout->setSpacing(4);

    auto *formHeader = new QHBoxLayout;
    auto *iconForm = new QLabel("📋");
    iconForm->setStyleSheet("font-size: 14px;");
    auto *titleForm = new QLabel("<b style='font-size: 12px; color: #082849;'>Informations de la commande</b>");

    auto *btnReset = new QPushButton("↺ Réinitialiser");
    btnReset->setCursor(Qt::PointingHandCursor);
    btnReset->setFixedHeight(24);
    btnReset->setStyleSheet("QPushButton { background: #FFFFFF; color: #475569; border: 1px solid #CBD5E1; border-radius: 4px; font-size: 10px; padding: 2px 8px; font-weight:600; } QPushButton:hover { background: #F1F5F9; }");
    connect(btnReset, &QPushButton::clicked, this, &MainWindow::onResetOrderForm);

    formHeader->addWidget(iconForm);
    formHeader->addWidget(titleForm);
    formHeader->addStretch();
    formHeader->addWidget(btnReset);
    leftLayout->addLayout(formHeader);

    auto *bodyLayout = new QHBoxLayout;
    bodyLayout->setSpacing(8);

    auto *fieldsLayout = new QVBoxLayout;
    fieldsLayout->setSpacing(2);

    auto makeField = [](const QString &label, QWidget *editor) {
        auto *hl = new QHBoxLayout;
        auto *lbl = new QLabel(label);
        lbl->setFixedWidth(80);
        lbl->setStyleSheet("font-weight: 600; color: #334155; font-size: 10px;");
        hl->addWidget(lbl);
        hl->addWidget(editor, 1);
        return hl;
    };

    // 1. Champ ID
    auto *idContainer = new QWidget;
    auto *idVBox = new QVBoxLayout(idContainer);
    idVBox->setContentsMargins(0, 0, 0, 0);
    idVBox->setSpacing(1);

    auto *idHBox = new QHBoxLayout;
    idHBox->setContentsMargins(0, 0, 0, 0);
    idHBox->setSpacing(4);

    formIdEdit = new QLineEdit;
    formIdEdit->setPlaceholderText("CMD-001");
    connect(formIdEdit, &QLineEdit::textChanged, this, &MainWindow::onIdTextChanged);

    auto *btnAutoId = new QPushButton("⚡ Auto ID");
    btnAutoId->setCursor(Qt::PointingHandCursor);
    btnAutoId->setFixedHeight(24);
    btnAutoId->setStyleSheet("QPushButton { background: #EFF6FF; color: #1D4ED8; border: 1px solid #BFDBFE; border-radius: 4px; font-weight: bold; font-size: 9px; padding: 0 6px; } QPushButton:hover { background: #DBEAFE; }");
    connect(btnAutoId, &QPushButton::clicked, this, &MainWindow::onBtnAutoGenerateIdClicked);

    idHBox->addWidget(formIdEdit, 1);
    idHBox->addWidget(btnAutoId);
    idVBox->addLayout(idHBox);

    idErrorLabel = new QLabel;
    idErrorLabel->setStyleSheet("color: #DC2626; font-size: 8.5px; font-weight: 600;");
    idErrorLabel->hide();
    idVBox->addWidget(idErrorLabel);

    fieldsLayout->addLayout(makeField("ID Commande", idContainer));

    // 2. Champ Nom Client
    auto *nameContainer = new QWidget;
    auto *nameVBox = new QVBoxLayout(nameContainer);
    nameVBox->setContentsMargins(0, 0, 0, 0);
    nameVBox->setSpacing(1);

    formClientNameEdit = new QLineEdit;
    formClientNameEdit->setPlaceholderText("Sarah Ben Ali");
    connect(formClientNameEdit, &QLineEdit::textChanged, this, &MainWindow::onClientNameTextChanged);
    nameVBox->addWidget(formClientNameEdit);

    clientNameErrorLabel = new QLabel;
    clientNameErrorLabel->setStyleSheet("color: #DC2626; font-size: 8.5px; font-weight: 600;");
    clientNameErrorLabel->hide();
    nameVBox->addWidget(clientNameErrorLabel);

    fieldsLayout->addLayout(makeField("Nom Client", nameContainer));

    // 3. Champ Code Client
    auto *cliContainer = new QWidget;
    auto *cliVBox = new QVBoxLayout(cliContainer);
    cliVBox->setContentsMargins(0, 0, 0, 0);
    cliVBox->setSpacing(1);

    formClientCodeEdit = new QLineEdit;
    formClientCodeEdit->setPlaceholderText("CL-001");
    connect(formClientCodeEdit, &QLineEdit::textChanged, this, &MainWindow::onClientCodeTextChanged);
    cliVBox->addWidget(formClientCodeEdit);

    clientCodeErrorLabel = new QLabel;
    clientCodeErrorLabel->setStyleSheet("color: #DC2626; font-size: 8.5px; font-weight: 600;");
    clientCodeErrorLabel->hide();
    cliVBox->addWidget(clientCodeErrorLabel);

    fieldsLayout->addLayout(makeField("Code Client", cliContainer));

    // 4. Champ Téléphone Client
    auto *phoneContainer = new QWidget;
    auto *phoneVBox = new QVBoxLayout(phoneContainer);
    phoneVBox->setContentsMargins(0, 0, 0, 0);
    phoneVBox->setSpacing(1);

    formClientPhoneEdit = new QLineEdit;
    formClientPhoneEdit->setPlaceholderText("ex: 98123456 ou 22123456");
    connect(formClientPhoneEdit, &QLineEdit::textChanged, this, &MainWindow::onClientPhoneTextChanged);
    phoneVBox->addWidget(formClientPhoneEdit);

    clientPhoneErrorLabel = new QLabel;
    clientPhoneErrorLabel->setStyleSheet("color: #DC2626; font-size: 8.5px; font-weight: 600;");
    clientPhoneErrorLabel->hide();
    phoneVBox->addWidget(clientPhoneErrorLabel);

    fieldsLayout->addLayout(makeField("📞 Téléphone", phoneContainer));

    // 5. Adresses tunisiennes
    comboWilaya = new QComboBox;
    comboDelegation = new QComboBox;
    comboImada = new QComboBox;

    comboWilaya->addItems(tunisiaHierarchy.keys());
    connect(comboWilaya, &QComboBox::currentTextChanged, this, &MainWindow::onWilayaChanged);
    connect(comboDelegation, &QComboBox::currentTextChanged, this, &MainWindow::onDelegationChanged);

    fieldsLayout->addLayout(makeField("Gouvernorat", comboWilaya));
    fieldsLayout->addLayout(makeField("Délégation", comboDelegation));
    fieldsLayout->addLayout(makeField("Secteur (Imada)", comboImada));

    onWilayaChanged(comboWilaya->currentText());

    formDateEdit = new QDateEdit(QDate::currentDate());
    formDateEdit->setCalendarPopup(true);
    formDateEdit->setDisplayFormat("dd/MM/yyyy");
    fieldsLayout->addLayout(makeField("Date", formDateEdit));
    connect(formDateEdit, &QDateEdit::dateChanged, this, &MainWindow::recalculerDelaiLivraisonEstime);

    formStatusCombo = new QComboBox;
    formStatusCombo->addItems({"Prête en atelier", "En cours", "Livrée"});
    fieldsLayout->addLayout(makeField("Statut", formStatusCombo));
    connect(formStatusCombo, &QComboBox::currentTextChanged, this, &MainWindow::recalculerDelaiLivraisonEstime);

    formArticleCombo = new QComboBox;
    formArticleCombo->addItems({"Robe Élégance", "Veste Tailleur Luxe", "Lunettes Stylées", "Bijoux Dorés", "Chaussures de Soirée", "Sac Premium"});
    fieldsLayout->addLayout(makeField("Article", formArticleCombo));

    connect(formArticleCombo, &QComboBox::currentTextChanged, this, [this](const QString &art){
        QString imgName = "prod_veste.png";
        if (art.contains("Lunette", Qt::CaseInsensitive))    imgName = "prod_lunette.png";
        else if (art.contains("Bijou", Qt::CaseInsensitive)) imgName = "prod_bijoux.png";
        else if (art.contains("Chaus", Qt::CaseInsensitive)) imgName = "prod_chaussure.png";
        else if (art.contains("Sac", Qt::CaseInsensitive))   imgName = "prod_sac.png";

        QPixmap p(":/images/" + imgName);
        if (!p.isNull() && formPreviewImage) {
            formPreviewImage->setPixmap(p.scaled(80, 95, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        }
    });

    formPaymentCombo = new QComboBox;
    formPaymentCombo->addItems({"Carte bancaire", "Espèces à la livraison", "Virement bancaire"});
    fieldsLayout->addLayout(makeField("Paiement", formPaymentCombo));

    // 6. Champ Montant
    auto *amtContainer = new QWidget;
    auto *amtVBox = new QVBoxLayout(amtContainer);
    amtVBox->setContentsMargins(0, 0, 0, 0);
    amtVBox->setSpacing(1);

    formAmountEdit = new QLineEdit;
    formAmountEdit->setPlaceholderText("ex: 350.00");
    connect(formAmountEdit, &QLineEdit::textChanged, this, &MainWindow::onAmountTextChanged);
    amtVBox->addWidget(formAmountEdit);

    amountErrorLabel = new QLabel;
    amountErrorLabel->setStyleSheet("color: #DC2626; font-size: 8.5px; font-weight: 600;");
    amountErrorLabel->hide();
    amtVBox->addWidget(amountErrorLabel);

    fieldsLayout->addLayout(makeField("Montant (TND)", amtContainer));

    smartDeliveryDateLabel = new QLabel;
    smartDeliveryDateLabel->setStyleSheet("background: #F1F5F9; border: 1px dashed #CBD5E1; border-radius: 5px; padding: 2px 5px; font-size: 9.5px; color: #082849; font-weight: 600;");
    fieldsLayout->addLayout(makeField("⏱ Livraison", smartDeliveryDateLabel));

    bodyLayout->addLayout(fieldsLayout, 1);

    auto *previewCol = new QVBoxLayout;
    previewCol->setAlignment(Qt::AlignTop);

    formPreviewImage = new QLabel;
    formPreviewImage->setFixedSize(85, 100);
    formPreviewImage->setStyleSheet("background: #F8FAFC; border: 1px solid #E2E8F0; border-radius: 6px;");
    formPreviewImage->setAlignment(Qt::AlignCenter);
    QPixmap pInit(":/images/prod_veste.png");
    if (!pInit.isNull()) {
        formPreviewImage->setPixmap(pInit.scaled(80, 95, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    } else {
        formPreviewImage->setText("👗");
    }
    previewCol->addWidget(formPreviewImage);

    bodyLayout->addLayout(previewCol);
    leftLayout->addLayout(bodyLayout);

    auto *actionsGrid = new QHBoxLayout;
    actionsGrid->setSpacing(6);

    btnAdd = new QPushButton("＋ Ajouter");
    btnAdd->setFixedHeight(28);
    btnAdd->setCursor(Qt::PointingHandCursor);
    btnAdd->setStyleSheet("QPushButton { background: #082849; color: white; border-radius: 4px; font-weight: bold; font-size: 10px; } QPushButton:hover { background: #1B5FA7; }");
    connect(btnAdd, &QPushButton::clicked, this, &MainWindow::onBtnAddClicked);

    btnEdit = new QPushButton("✏ Modifier");
    btnEdit->setFixedHeight(28);
    btnEdit->setCursor(Qt::PointingHandCursor);
    btnEdit->setStyleSheet("QPushButton { background: #1B5FA7; color: white; border-radius: 4px; font-weight: bold; font-size: 10px; } QPushButton:hover { background: #2563EB; }");
    connect(btnEdit, &QPushButton::clicked, this, &MainWindow::onBtnEditClicked);

    btnDel = new QPushButton("🗑 Supprimer");
    btnDel->setFixedHeight(28);
    btnDel->setCursor(Qt::PointingHandCursor);
    btnDel->setStyleSheet("QPushButton { background: #EFE5D5; color: #3F3427; border: none; border-radius: 4px; font-weight: bold; font-size: 10px; } QPushButton:hover { background: #E5DAC6; }");
    connect(btnDel, &QPushButton::clicked, this, &MainWindow::onBtnDeleteClicked);

    actionsGrid->addWidget(btnAdd, 1);
    actionsGrid->addWidget(btnEdit, 1);
    actionsGrid->addWidget(btnDel, 1);

    leftLayout->addLayout(actionsGrid);
    hBox->addWidget(formFrame, 0);

    // Cadre du Tableau
    tableContainerFrame = new QFrame;
    tableContainerFrame->setStyleSheet(cardStyle());
    auto *rightLayout = new QVBoxLayout(tableContainerFrame);
    rightLayout->setContentsMargins(10, 8, 10, 8);
    rightLayout->setSpacing(6);

    auto *tableHeader = new QHBoxLayout;
    auto *iconTable = new QLabel("📋");
    iconTable->setStyleSheet("font-size: 14px;");
    auto *titleTable = new QLabel("<b style='font-size: 12px; color: #082849;'>Liste des commandes</b>");

    auto *searchBox = new QLineEdit;
    searchBox->setPlaceholderText("🔍 Rechercher (ID, client, ville)...");
    searchBox->setFixedHeight(24);
    searchBox->setFixedWidth(160);
    connect(searchBox, &QLineEdit::textChanged, this, [this, searchBox](){
        for(int r = 0; r < ordersTable->rowCount(); ++r) {
            bool ok = searchBox->text().isEmpty();
            for(int c = 1; c <= 5 && !ok; ++c) {
                if(ordersTable->item(r,c) && ordersTable->item(r,c)->text().contains(searchBox->text(), Qt::CaseInsensitive))
                    ok = true;
            }
            ordersTable->setRowHidden(r, !ok);
        }
    });

    auto *btnPdf = new QPushButton("📄 Exporter PDF");
    btnPdf->setCursor(Qt::PointingHandCursor);
    btnPdf->setFixedHeight(24);
    btnPdf->setStyleSheet("QPushButton { background: #082849; color: white; border-radius: 4px; font-weight: bold; padding: 0 8px; font-size: 10px; } QPushButton:hover { background: #1B5FA7; }");
    connect(btnPdf, &QPushButton::clicked, this, &MainWindow::generateInvoicePdf);

    tableHeader->addWidget(iconTable);
    tableHeader->addWidget(titleTable);
    tableHeader->addStretch();
    tableHeader->addWidget(searchBox);
    tableHeader->addWidget(btnPdf);
    rightLayout->addLayout(tableHeader);

    ordersTable = new QTableWidget(0, 9);
    ordersTable->setIconSize(QSize(22, 22));
    ordersTable->setHorizontalHeaderLabels({" ", "ID", "Code", "Nom Client", "Téléphone", "Adresse", "Date", "Statut", "Montant"});
    ordersTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ordersTable->setSelectionMode(QAbstractItemView::SingleSelection);
    ordersTable->verticalHeader()->setVisible(false);
    ordersTable->setFixedHeight(225);

    ordersTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed);
    ordersTable->setColumnWidth(0, 26);
    ordersTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    ordersTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    ordersTable->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch);
    ordersTable->horizontalHeader()->setSectionResizeMode(4, QHeaderView::ResizeToContents);
    ordersTable->horizontalHeader()->setSectionResizeMode(5, QHeaderView::Stretch);
    ordersTable->horizontalHeader()->setSectionResizeMode(6, QHeaderView::ResizeToContents);
    ordersTable->horizontalHeader()->setSectionResizeMode(7, QHeaderView::ResizeToContents);
    ordersTable->horizontalHeader()->setSectionResizeMode(8, QHeaderView::ResizeToContents);

    connect(ordersTable, &QTableWidget::cellClicked, this, &MainWindow::selectOrder);
    rightLayout->addWidget(ordersTable, 1);

    auto *pageBar = new QHBoxLayout;
    auto *lblCnt = new QLabel("Synchronisation active");
    lblCnt->setStyleSheet("color:#64748B; font-size:9.5px;");
    pageBar->addWidget(lblCnt);
    pageBar->addStretch();

    const QStringList pages = {"<", "1", "2", "3", ">"};
    for(const auto &pg : pages) {
        auto *pb = new QPushButton(pg);
        pb->setFixedSize(20, 20);
        if(pg == "1") pb->setStyleSheet("QPushButton{background:#082849; color:white; border-radius:3px; font-weight:bold; font-size:9px;}");
        else pb->setStyleSheet("QPushButton{background:#F1F5F9; color:#64748B; border:1px solid #CBD5E1; border-radius:3px; font-size:9px;}");
        pageBar->addWidget(pb);
    }
    rightLayout->addLayout(pageBar);

    hBox->addWidget(tableContainerFrame, 1);
    recalculerDelaiLivraisonEstime();
    return container;
}

void MainWindow::onIdTextChanged(const QString &text)
{
    if (!idErrorLabel || !formIdEdit) return;

    QString upperText = text.toUpper();
    if (text != upperText) {
        int pos = formIdEdit->cursorPosition();
        formIdEdit->blockSignals(true);
        formIdEdit->setText(upperText);
        formIdEdit->setCursorPosition(pos);
        formIdEdit->blockSignals(false);
        return;
    }

    if (text.isEmpty()) {
        idErrorLabel->hide();
        formIdEdit->setStyleSheet("background-color: #FFFFFF; border: 1px solid #CBD5E1; border-radius: 5px;");
        return;
    }

    static const QRegularExpression rx(R"(^CMD-\d{3,}$)");
    if (!rx.match(text).hasMatch()) {
        idErrorLabel->setText("❌ Format requis : CMD-001 (Majuscules, tiret '-', au moins 3 chiffres)");
        idErrorLabel->setStyleSheet("color: #DC2626; font-size: 8.5px; font-weight: 600;");
        idErrorLabel->show();
        formIdEdit->setStyleSheet("background-color: #FEF2F2; border: 1px solid #EF4444; border-radius: 5px;");
    } else {
        idErrorLabel->setText("✅ Identifiant conforme");
        idErrorLabel->setStyleSheet("color: #059669; font-size: 8.5px; font-weight: 600;");
        idErrorLabel->show();
        formIdEdit->setStyleSheet("background-color: #F0FDF4; border: 1px solid #10B981; border-radius: 5px;");
    }
}

void MainWindow::onClientNameTextChanged(const QString &text)
{
    if (!clientNameErrorLabel || !formClientNameEdit) return;

    if (text.trimmed().isEmpty()) {
        clientNameErrorLabel->hide();
        formClientNameEdit->setStyleSheet("background-color: #FFFFFF; border: 1px solid #CBD5E1; border-radius: 5px;");
        return;
    }

    static const QRegularExpression rxName(R"(^[a-zA-ZÀ-ÿ\s'-]{3,50}$)");
    if (!rxName.match(text.trimmed()).hasMatch()) {
        clientNameErrorLabel->setText("❌ Nom invalide : au moins 3 lettres (sans chiffres ni symboles)");
        clientNameErrorLabel->setStyleSheet("color: #DC2626; font-size: 8.5px; font-weight: 600;");
        clientNameErrorLabel->show();
        formClientNameEdit->setStyleSheet("background-color: #FEF2F2; border: 1px solid #EF4444; border-radius: 5px;");
    } else {
        clientNameErrorLabel->setText("✅ Nom valide");
        clientNameErrorLabel->setStyleSheet("color: #059669; font-size: 8.5px; font-weight: 600;");
        clientNameErrorLabel->show();
        formClientNameEdit->setStyleSheet("background-color: #F0FDF4; border: 1px solid #10B981; border-radius: 5px;");
    }
}

void MainWindow::onClientCodeTextChanged(const QString &text)
{
    if (!clientCodeErrorLabel || !formClientCodeEdit) return;

    QString upperText = text.toUpper();
    if (text != upperText) {
        int pos = formClientCodeEdit->cursorPosition();
        formClientCodeEdit->blockSignals(true);
        formClientCodeEdit->setText(upperText);
        formClientCodeEdit->setCursorPosition(pos);
        formClientCodeEdit->blockSignals(false);
        return;
    }

    if (text.isEmpty()) {
        clientCodeErrorLabel->hide();
        formClientCodeEdit->setStyleSheet("background-color: #FFFFFF; border: 1px solid #CBD5E1; border-radius: 5px;");
        return;
    }

    static const QRegularExpression rx(R"(^CL-\d{3,}$)");
    if (!rx.match(text).hasMatch()) {
        clientCodeErrorLabel->setText("❌ Format requis : CL-001 (Majuscules, tiret '-', au moins 3 chiffres)");
        clientCodeErrorLabel->setStyleSheet("color: #DC2626; font-size: 8.5px; font-weight: 600;");
        clientCodeErrorLabel->show();
        formClientCodeEdit->setStyleSheet("background-color: #FEF2F2; border: 1px solid #EF4444; border-radius: 5px;");
    } else {
        clientCodeErrorLabel->setText("✅ Code client conforme");
        clientCodeErrorLabel->setStyleSheet("color: #059669; font-size: 8.5px; font-weight: 600;");
        clientCodeErrorLabel->show();
        formClientCodeEdit->setStyleSheet("background-color: #F0FDF4; border: 1px solid #10B981; border-radius: 5px;");
    }
}

void MainWindow::onAmountTextChanged(const QString &text)
{
    if (!amountErrorLabel || !formAmountEdit) return;

    QString clean = text.trimmed().replace(",", ".");
    if (clean.isEmpty()) {
        amountErrorLabel->hide();
        formAmountEdit->setStyleSheet("background-color: #FFFFFF; border: 1px solid #CBD5E1; border-radius: 5px;");
        return;
    }

    static const QRegularExpression rxAmt(R"(^\d+(\.\d{1,3})?$)");
    double val = clean.toDouble();
    if (!rxAmt.match(clean).hasMatch() || val <= 0.0) {
        amountErrorLabel->setText("❌ Montant invalide : nombre positif attendu (ex: 150.50)");
        amountErrorLabel->setStyleSheet("color: #DC2626; font-size: 8.5px; font-weight: 600;");
        amountErrorLabel->show();
        formAmountEdit->setStyleSheet("background-color: #FEF2F2; border: 1px solid #EF4444; border-radius: 5px;");
    } else {
        amountErrorLabel->setText("✅ Montant conforme");
        amountErrorLabel->setStyleSheet("color: #059669; font-size: 8.5px; font-weight: 600;");
        amountErrorLabel->show();
        formAmountEdit->setStyleSheet("background-color: #F0FDF4; border: 1px solid #10B981; border-radius: 5px;");
    }
}

void MainWindow::onClientPhoneTextChanged(const QString &text)
{
    if (!clientPhoneErrorLabel || !formClientPhoneEdit) return;

    QString clean = text;
    clean.remove(' ').remove('-').remove('.').remove('+');

    if (clean.startsWith("216") && clean.length() > 8) clean = clean.mid(3);
    else if (clean.startsWith("00216")) clean = clean.mid(5);

    if (clean.isEmpty()) {
        clientPhoneErrorLabel->hide();
        formClientPhoneEdit->setStyleSheet("background-color: #FFFFFF; border: 1px solid #CBD5E1; border-radius: 5px;");
        return;
    }

    QString opName;
    if (validateTunisianPhoneNumber(clean, opName)) {
        clientPhoneErrorLabel->setText(QString("✅ Numéro conforme (%1)").arg(opName));
        clientPhoneErrorLabel->setStyleSheet("color: #059669; font-size: 8.5px; font-weight: 600;");
        clientPhoneErrorLabel->show();
        formClientPhoneEdit->setStyleSheet("background-color: #F0FDF4; border: 1px solid #10B981; border-radius: 5px;");
    } else {
        clientPhoneErrorLabel->setText("❌ Numéro non attribué dans le Plan National (INT)");
        clientPhoneErrorLabel->setStyleSheet("color: #DC2626; font-size: 8.5px; font-weight: 600;");
        clientPhoneErrorLabel->show();
        formClientPhoneEdit->setStyleSheet("background-color: #FEF2F2; border: 1px solid #EF4444; border-radius: 5px;");
    }
}

void MainWindow::onBtnWhatsAppClicked()
{
    QString phone = currentSelectedPhone;
    phone.remove(' ').remove('-').remove('+').remove('.');
    if (phone.startsWith("00216")) phone = phone.mid(5);
    else if (phone.startsWith("216") && phone.length() > 8) phone = phone.mid(3);

    QString dummyOp;
    if (!validateTunisianPhoneNumber(phone, dummyOp)) {
        QMessageBox::warning(this, "Numéro requis", "Veuillez sélectionner une commande avec un numéro tunisien valide.");
        return;
    }

    int r = currentRowSelected;
    QString clientName = (r >= 0 && ordersTable->item(r, 3)) ? ordersTable->item(r, 3)->text() : "Client";
    QString cmdId = (r >= 0 && ordersTable->item(r, 1)) ? ordersTable->item(r, 1)->text() : "";
    QString amount = (r >= 0 && ordersTable->item(r, 8)) ? ordersTable->item(r, 8)->text() : "";

    QString message = QString("Bonjour %1,\nVotre commande *%2* d'un montant de *%3* est actuellement préparée avec soin dans notre atelier Fashionova Prestige.\nMerci pour votre confiance !")
                          .arg(clientName, cmdId, amount);

    QString url = QString("https://wa.me/216%1?text=%2").arg(phone, QString::fromUtf8(QUrl::toPercentEncoding(message)));
    QDesktopServices::openUrl(QUrl(url));
}

void MainWindow::onBtnSmsClicked()
{
    QString phone = currentSelectedPhone;
    phone.remove(' ').remove('-').remove('+').remove('.');
    if (phone.startsWith("00216")) phone = phone.mid(5);
    else if (phone.startsWith("216") && phone.length() > 8) phone = phone.mid(3);

    QString dummyOp;
    if (!validateTunisianPhoneNumber(phone, dummyOp)) {
        QMessageBox::warning(this, "Numéro requis", "Veuillez sélectionner une commande avec un numéro tunisien valide.");
        return;
    }

    int r = currentRowSelected;
    QString cmdId = (r >= 0 && ordersTable->item(r, 1)) ? ordersTable->item(r, 1)->text() : "";
    QString amount = (r >= 0 && ordersTable->item(r, 8)) ? ordersTable->item(r, 8)->text() : "";

    QString message = QString("Fashionova: Votre commande %1 (%2) est en cours de preparation. Merci pour votre confiance.").arg(cmdId, amount);
    QString url = QString("sms:+216%1?body=%2").arg(phone, QString::fromUtf8(QUrl::toPercentEncoding(message)));
    QDesktopServices::openUrl(QUrl(url));
}

// Génération intelligente qui assure un ID de commande ET un code client uniques pour chaque ajout
void MainWindow::onBtnAutoGenerateIdClicked()
{
    int maxCmd = 0;
    int maxCli = 0;

    for (int r = 0; r < ordersTable->rowCount(); ++r) {
        if (ordersTable->item(r, 1)) {
            QString idStr = ordersTable->item(r, 1)->text().trimmed();
            if (idStr.startsWith("CMD-")) {
                int num = idStr.mid(4).toInt();
                if (num > maxCmd) maxCmd = num;
            }
        }
        if (ordersTable->item(r, 2)) {
            QString cliStr = ordersTable->item(r, 2)->text().trimmed();
            if (cliStr.startsWith("CL-")) {
                int num = cliStr.mid(3).toInt();
                if (num > maxCli) maxCli = num;
            }
        }
    }

    QSqlQuery q("SELECT ID_COMMANDE, ID_CLIENT FROM COMMANDES");
    while (q.next()) {
        QString dbId = q.value(0).toString().trimmed();
        if (dbId.startsWith("CMD-")) {
            int num = dbId.mid(4).toInt();
            if (num > maxCmd) maxCmd = num;
        }
        QString dbCli = q.value(1).toString().trimmed();
        if (dbCli.startsWith("CL-")) {
            int num = dbCli.mid(3).toInt();
            if (num > maxCli) maxCli = num;
        }
    }

    int nextCmd = maxCmd + 1;
    int nextCli = qMax(maxCli + 1, nextCmd);

    formIdEdit->setText(QString("CMD-%1").arg(nextCmd, 3, 10, QChar('0')));
    formClientCodeEdit->setText(QString("CL-%1").arg(nextCli, 3, 10, QChar('0')));
}

void MainWindow::onWilayaChanged(const QString &wilaya)
{
    if (!comboDelegation || !comboImada) return;
    comboDelegation->blockSignals(true);
    comboDelegation->clear();

    if (tunisiaHierarchy.contains(wilaya)) {
        comboDelegation->addItems(tunisiaHierarchy[wilaya].keys());
    }
    comboDelegation->blockSignals(false);
    onDelegationChanged(comboDelegation->currentText());
}

void MainWindow::onDelegationChanged(const QString &delegation)
{
    if (!comboImada || !comboWilaya) return;
    comboImada->clear();
    QString wilaya = comboWilaya->currentText();

    if (tunisiaHierarchy.contains(wilaya) && tunisiaHierarchy[wilaya].contains(delegation)) {
        for (const auto &item : tunisiaHierarchy[wilaya][delegation]) {
            comboImada->addItem(item.name);
        }
    }
}

void MainWindow::recalculerDelaiLivraisonEstime()
{
    if (!smartDeliveryDateLabel || !formDateEdit) return;

    QDate baseDate = formDateEdit->date();
    int joursOuvrables = 2;

    QSqlQuery q("SELECT COUNT(*) FROM COMMANDES WHERE STATUT = 'En cours'");
    int atelierCount = 0;
    if (q.exec() && q.next()) {
        atelierCount = q.value(0).toInt();
    }

    if (atelierCount > 5) {
        joursOuvrables += (atelierCount / 4);
    }

    QDate dateEstimee = baseDate;
    int ajoutes = 0;
    while (ajoutes < joursOuvrables) {
        dateEstimee = dateEstimee.addDays(1);
        if (dateEstimee.dayOfWeek() != 7) {
            ajoutes++;
        }
    }

    smartDeliveryDateLabel->setText(QString("🟢 %1 (+%2 jrs)").arg(dateEstimee.toString("dd/MM/yyyy")).arg(joursOuvrables));
}

void MainWindow::onResetOrderForm()
{
    onBtnAutoGenerateIdClicked();
    formClientNameEdit->clear();
    formClientPhoneEdit->clear();
    formAmountEdit->clear();

    if (comboWilaya && comboWilaya->count() > 0) comboWilaya->setCurrentIndex(0);
    formDateEdit->setDate(QDate::currentDate());
    formStatusCombo->setCurrentIndex(0);
    if (formArticleCombo) formArticleCombo->setCurrentIndex(0);
    if (formPaymentCombo) formPaymentCombo->setCurrentIndex(0);

    if (idErrorLabel) idErrorLabel->hide();
    if (clientNameErrorLabel) clientNameErrorLabel->hide();
    if (clientCodeErrorLabel) clientCodeErrorLabel->hide();
    if (clientPhoneErrorLabel) clientPhoneErrorLabel->hide();
    if (amountErrorLabel) amountErrorLabel->hide();

    recalculerDelaiLivraisonEstime();
    formClientNameEdit->setFocus();
}

void MainWindow::onBtnAddClicked()
{
    QString id = formIdEdit->text().trimmed();
    QString client = formClientCodeEdit->text().trimmed();
    QString name = formClientNameEdit->text().trimmed();
    QString phone = formClientPhoneEdit->text().trimmed().remove(' ').remove('-').remove('.').remove('+');
    if (phone.startsWith("216") && phone.length() > 8) phone = phone.mid(3);
    else if (phone.startsWith("00216")) phone = phone.mid(5);

    QString amountStr = formAmountEdit->text().trimmed().replace(",", ".");

    static const QRegularExpression rxId(R"(^CMD-\d{3,}$)");
    if (!rxId.match(id).hasMatch()) {
        QMessageBox::warning(this, "Format ID incorrect", "L'identifiant doit respecter le format <b>CMD-001</b>.");
        formIdEdit->setFocus();
        return;
    }

    static const QRegularExpression rxName(R"(^[a-zA-ZÀ-ÿ\s'-]{3,50}$)");
    if (!rxName.match(name).hasMatch()) {
        QMessageBox::warning(this, "Nom Client requis", "Veuillez saisir un nom client valide (au moins 3 lettres alphabétiques).");
        formClientNameEdit->setFocus();
        return;
    }

    static const QRegularExpression rxCli(R"(^CL-\d{3,}$)");
    if (!rxCli.match(client).hasMatch()) {
        QMessageBox::warning(this, "Code Client incorrect", "Le code client doit respecter le format <b>CL-001</b>.");
        formClientCodeEdit->setFocus();
        return;
    }

    QString dummyOp;
    if (!validateTunisianPhoneNumber(phone, dummyOp)) {
        QMessageBox::warning(this, "Numéro non conforme", "Le numéro saisi n'appartient pas au Plan National de Numérotation de l'INT.");
        formClientPhoneEdit->setFocus();
        return;
    }

    double montant = amountStr.toDouble();
    if (montant <= 0.0) {
        QMessageBox::warning(this, "Montant requis", "Veuillez saisir un montant valide et strictement positif.");
        formAmountEdit->setFocus();
        return;
    }

    QString wilaya = comboWilaya ? comboWilaya->currentText() : "Tunis";
    QString delegation = comboDelegation ? comboDelegation->currentText() : "Les Berges du Lac";
    QString imada = comboImada ? comboImada->currentText() : "Lac 1";
    QString fullAddress = QString("%1, %2, %3").arg(wilaya, delegation, imada);

    QDate dateCmd = formDateEdit->date();
    QString status = formStatusCombo->currentText();
    QPair<double, double> geo = getCoordinatesFromAddress(wilaya, delegation, imada);
    QString selectedArticle = formArticleCombo ? formArticleCombo->currentText() : "Robe Élégance";
    QString selectedPayment = formPaymentCombo ? formPaymentCombo->currentText() : "Carte bancaire";

    // Gestion Client : enregistrer ou mettre à jour la fiche du client correspondant
    QSqlQuery checkClientQuery;
    checkClientQuery.prepare("SELECT COUNT(*) FROM CLIENTS WHERE ID_CLIENT = :code");
    checkClientQuery.bindValue(":code", client);
    if (checkClientQuery.exec() && checkClientQuery.next()) {
        QString nom = name.section(' ', 0, 0);
        QString prenom = name.section(' ', 1, -1);
        if (prenom.isEmpty()) prenom = "Client";

        if (checkClientQuery.value(0).toInt() == 0) {
            QSqlQuery insCli;
            insCli.prepare("INSERT INTO CLIENTS (ID_CLIENT, NOM_CLIENT, PRENOM_CLIENT, HISTORIQUE_ACHATS, TELEPHONE) VALUES (:code, :nom, :prenom, 'Nouveau client', :tel)");
            insCli.bindValue(":code", client);
            insCli.bindValue(":nom", nom);
            insCli.bindValue(":prenom", prenom);
            insCli.bindValue(":tel", phone);
            insCli.exec();
        } else {
            QSqlQuery upCli;
            upCli.prepare("UPDATE CLIENTS SET NOM_CLIENT = :nom, PRENOM_CLIENT = :prenom, TELEPHONE = :tel WHERE ID_CLIENT = :code");
            upCli.bindValue(":nom", nom);
            upCli.bindValue(":prenom", prenom);
            upCli.bindValue(":tel", phone);
            upCli.bindValue(":code", client);
            upCli.exec();
        }
    }

    Commande cmd(id, client, name, dateCmd, status, montant, selectedArticle, "M", 1, selectedPayment, fullAddress, geo.first, geo.second);

    if (cmd.ajouter()) {
        QSqlQuery qUp;
        qUp.prepare("UPDATE COMMANDES SET ARTICLE = :art, TELEPHONE = :tel WHERE ID_COMMANDE = :id");
        qUp.bindValue(":art", selectedArticle);
        qUp.bindValue(":tel", phone);
        qUp.bindValue(":id", id);
        qUp.exec();

        QMessageBox::information(this, "Ajout réussi", QString("La commande <b>%1</b> a été enregistrée avec succès !").arg(id));
        actualiserTableau();
        onResetOrderForm();
    } else {
        QMessageBox::critical(this, "Erreur", QString("Impossible d'ajouter la commande <b>%1</b>. Cet identifiant existe déjà.").arg(id));
    }
}

void MainWindow::onBtnEditClicked()
{
    QString id = formIdEdit->text().trimmed();
    if (id.isEmpty()) {
        QMessageBox::warning(this, "Sélection requise", "Veuillez sélectionner une commande dans le tableau à modifier.");
        return;
    }

    QString client = formClientCodeEdit->text().trimmed();
    QString name = formClientNameEdit->text().trimmed();
    QString phone = formClientPhoneEdit->text().trimmed().remove(' ').remove('-').remove('.').remove('+');
    if (phone.startsWith("216") && phone.length() > 8) phone = phone.mid(3);
    else if (phone.startsWith("00216")) phone = phone.mid(5);

    QString amountStr = formAmountEdit->text().trimmed().replace(",", ".");

    static const QRegularExpression rxName(R"(^[a-zA-ZÀ-ÿ\s'-]{3,50}$)");
    if (!rxName.match(name).hasMatch()) {
        QMessageBox::warning(this, "Nom Client requis", "Veuillez saisir un nom client valide.");
        formClientNameEdit->setFocus();
        return;
    }

    QString dummyOp;
    if (!validateTunisianPhoneNumber(phone, dummyOp)) {
        QMessageBox::warning(this, "Numéro non conforme", "Le numéro saisi n'est pas conforme au plan de l'INT.");
        formClientPhoneEdit->setFocus();
        return;
    }

    double montant = amountStr.toDouble();
    if (montant <= 0.0) {
        QMessageBox::warning(this, "Montant requis", "Veuillez saisir un montant valide.");
        formAmountEdit->setFocus();
        return;
    }

    QString wilaya = comboWilaya ? comboWilaya->currentText() : "Tunis";
    QString delegation = comboDelegation ? comboDelegation->currentText() : "Les Berges du Lac";
    QString imada = comboImada ? comboImada->currentText() : "Lac 1";
    QString fullAddress = QString("%1, %2, %3").arg(wilaya, delegation, imada);

    QDate dateCmd = formDateEdit->date();
    QString status = formStatusCombo->currentText();
    QPair<double, double> geo = getCoordinatesFromAddress(wilaya, delegation, imada);
    QString selectedArticle = formArticleCombo ? formArticleCombo->currentText() : "Robe Élégance";
    QString selectedPayment = formPaymentCombo ? formPaymentCombo->currentText() : "Carte bancaire";

    QString nom = name.section(' ', 0, 0);
    QString prenom = name.section(' ', 1, -1);
    if (prenom.isEmpty()) prenom = "Client";
    QSqlQuery upCli;
    upCli.prepare("UPDATE CLIENTS SET NOM_CLIENT = :nom, PRENOM_CLIENT = :prenom, TELEPHONE = :tel WHERE ID_CLIENT = :code");
    upCli.bindValue(":nom", nom);
    upCli.bindValue(":prenom", prenom);
    upCli.bindValue(":tel", phone);
    upCli.bindValue(":code", client);
    upCli.exec();

    Commande cmd(id, client, name, dateCmd, status, montant, selectedArticle, "M", 1, selectedPayment, fullAddress, geo.first, geo.second);

    if (cmd.modifier()) {
        QSqlQuery qUp;
        qUp.prepare("UPDATE COMMANDES SET ARTICLE = :art, TELEPHONE = :tel WHERE ID_COMMANDE = :id");
        qUp.bindValue(":art", selectedArticle);
        qUp.bindValue(":tel", phone);
        qUp.bindValue(":id", id);
        qUp.exec();

        QMessageBox::information(this, "Modification réussie", QString("La commande <b>%1</b> a été mise à jour avec succès !").arg(id));
        actualiserTableau();

        for (int r = 0; r < ordersTable->rowCount(); ++r) {
            if (ordersTable->item(r, 1) && ordersTable->item(r, 1)->text() == id) {
                ordersTable->selectRow(r);
                updateDetails(r);
                break;
            }
        }
    } else {
        QMessageBox::critical(this, "Erreur", "Impossible de mettre à jour la commande.");
    }
}

void MainWindow::onBtnDeleteClicked()
{
    int r = ordersTable->currentRow();
    if (r < 0) {
        QMessageBox::warning(this, "Sélection requise", "Veuillez sélectionner une commande à supprimer.");
        return;
    }

    QString id = ordersTable->item(r, 1) ? ordersTable->item(r, 1)->text() : "";
    if (QMessageBox::question(this, "Confirmer suppression", QString("Voulez-vous supprimer définitivement la commande <b>%1</b> ?").arg(id)) == QMessageBox::Yes) {
        QSqlQuery qDelContenir;
        qDelContenir.prepare("DELETE FROM CONTENIR WHERE ID_COMMANDE = :id");
        qDelContenir.bindValue(":id", id);
        qDelContenir.exec();

        if (cTmp.supprimer(id)) {
            actualiserTableau();
            onResetOrderForm();
            QMessageBox::information(this, "Suppression réussie", QString("La commande <b>%1</b> a été supprimée avec succès.").arg(id));
        } else {
            QMessageBox::critical(this, "Erreur", "Échec de la suppression.");
        }
    }
}

void MainWindow::calculerTempsTrajet(double latDest, double lonDest)
{
    const double latAtelier = 36.8333;
    const double lonAtelier = 10.2333;

    double dLat = qDegreesToRadians(latDest - latAtelier);
    double dLon = qDegreesToRadians(lonDest - lonAtelier);
    double a = qSin(dLat / 2) * qSin(dLat / 2) +
               qCos(qDegreesToRadians(latAtelier)) * qCos(qDegreesToRadians(latDest)) *
                   qSin(dLon / 2) * qSin(dLon / 2);
    double c = 2 * qAtan2(qSqrt(a), qSqrt(1 - a));
    double distanceKm = 6371.0 * c * 1.25;

    double tVoiture = (distanceKm / 70.0) * 60.0;
    double tMoto    = (distanceKm / 55.0) * 60.0;
    double tVelo    = (distanceKm / 16.0) * 60.0;
    double tMarche  = (distanceKm / 4.8)  * 60.0;

    auto formater = [](double minTotal) -> QString {
        int m = static_cast<int>(minTotal);
        if (m < 60) return QString("%1 min").arg(m);
        return QString("%1h %2m").arg(m / 60).arg(m % 60);
    };

    if (distanceLabel)
        distanceLabel->setText(QString("<b>%1 km</b>").arg(QString::number(distanceKm, 'f', 1)));

    if (costLabel)
        costLabel->setText(QString("<b>%1 TND</b>").arg(QString::number(7.0 + distanceKm * 0.35, 'f', 2)));

    if (delayLabel) {
        delayLabel->setText(QString(
                                "🚗 <b>%1</b> &nbsp;|&nbsp; 🏍️ <b>%2</b><br>"
                                "🚲 <b>%3</b> &nbsp;|&nbsp; 🚶 <b>%4</b>"
                                ).arg(formater(tVoiture), formater(tMoto), formater(tVelo), formater(tMarche)));
    }
}

void MainWindow::toggleSortOrders()
{
    sortAscending = !sortAscending;
    ordersTable->sortItems(1, sortAscending ? Qt::AscendingOrder : Qt::DescendingOrder);
}

void MainWindow::actualiserTableau()
{
    setupOrders();
}

void MainWindow::rafraichirMarqueursCarte()
{
    if (!mapView) return;

    QJsonArray ordersArray;

    for (int r = 0; r < ordersTable->rowCount(); ++r) {
        QString id = ordersTable->item(r, 1) ? ordersTable->item(r, 1)->text() : "";
        QString name = ordersTable->item(r, 3) ? ordersTable->item(r, 3)->text() : "";
        QString addr = ordersTable->item(r, 5) ? ordersTable->item(r, 5)->text() : "";
        QString amount = ordersTable->item(r, 8) ? ordersTable->item(r, 8)->text() : "";

        auto *badgeWidget = ordersTable->cellWidget(r, 7);
        QString status = "Prête en atelier";
        if (badgeWidget) {
            auto *lbl = badgeWidget->findChild<QLabel*>();
            if (lbl) status = lbl->text();
        }

        QStringList parts = addr.split(", ");
        QPair<double, double> coords = {36.8065, 10.1815};
        if (parts.size() >= 3) {
            coords = getCoordinatesFromAddress(parts[0], parts[1], parts[2]);
        }

        QJsonObject o;
        o["id"] = id;
        o["name"] = name;
        o["status"] = status;
        o["amount"] = amount;
        o["address"] = addr;
        o["lat"] = coords.first;
        o["lon"] = coords.second;

        ordersArray.append(o);
    }

    QJsonDocument doc(ordersArray);
    QString jsonString = QString::fromUtf8(doc.toJson(QJsonDocument::Compact));
    sendMapCommand(QString("if (typeof setAllOrderMarkers === 'function') { setAllOrderMarkers(%1); }").arg(jsonString));
}

void MainWindow::recalculerStatistiquesEtDonut()
{
    animCountLivree = 0;
    animCountEnCours = 0;
    animCountPrete = 0;
    double sommeMontants = 0.0;

    QMap<QString, int> gouvernoratCounts;

    for (int r = 0; r < ordersTable->rowCount(); ++r) {
        auto *badgeWidget = ordersTable->cellWidget(r, 7);
        QString status = "Prête en atelier";
        if (badgeWidget) {
            auto *lbl = badgeWidget->findChild<QLabel*>();
            if (lbl) status = lbl->text();
        }

        if (status.contains("Livr")) animCountLivree++;
        else if (status.contains("cours")) animCountEnCours++;
        else animCountPrete++;

        if (ordersTable->item(r, 8)) {
            QString mStr = ordersTable->item(r, 8)->text().replace(" TND", "").replace(",", ".").trimmed();
            sommeMontants += mStr.toDouble();
        }

        if (ordersTable->item(r, 5)) {
            QString addr = ordersTable->item(r, 5)->text();
            QString gov = addr.section(',', 0, 0).trimmed();
            if (!gov.isEmpty()) {
                gouvernoratCounts[gov]++;
            }
        }
    }

    animTotalCmd = ordersTable->rowCount();

    if (kpiTotalVal) kpiTotalVal->setText(QString::number(animTotalCmd));
    if (kpiEnAttenteVal) kpiEnAttenteVal->setText(QString::number(animCountEnCours));
    if (kpiExpedieesVal) kpiExpedieesVal->setText(QString::number(animCountLivree));
    if (kpiAtelierVal) kpiAtelierVal->setText(QString::number(animCountPrete));
    if (kpiMontantTotalVal) kpiMontantTotalVal->setText(QString("%1 DT").arg(QString::number(sommeMontants, 'f', 0)));

    if (legendLayout) {
        QLayoutItem *child;
        while ((child = legendLayout->takeAt(0)) != nullptr) {
            delete child->widget();
            delete child;
        }

        double pctLiv = animTotalCmd > 0 ? (animCountLivree * 100.0 / animTotalCmd) : 0;
        double pctEnc = animTotalCmd > 0 ? (animCountEnCours * 100.0 / animTotalCmd) : 0;
        double pctPre = animTotalCmd > 0 ? (animCountPrete * 100.0 / animTotalCmd) : 0;

        auto addLeg = [&](const QString &col, const QString &text, double val){
            auto *lh = new QHBoxLayout;
            auto *dot = new QLabel("●");
            dot->setStyleSheet(QString("color:%1; font-size:12px;").arg(col));
            auto *lt = new QLabel(text);
            lt->setStyleSheet("font-size:11px; color:#475569;");
            auto *lv = new QLabel(QString("<b>%1%</b>").arg(QString::number(val, 'f', 0)));
            lv->setStyleSheet("font-size:11px; color:#0F172A;");
            lh->addWidget(dot);
            lh->addWidget(lt, 1);
            lh->addWidget(lv);
            legendLayout->addLayout(lh);
        };

        addLeg("#10B981", "Livrées", pctLiv);
        addLeg("#F59E0B", "En cours", pctEnc);
        addLeg("#2563EB", "Prêtes", pctPre);
    }

    if (donutAnim) {
        donutAnim->stop();
        donutAnim->start();
    } else {
        dessinerDonut(1.0);
    }

    if (govBarChartLabel) {
        govBarChartLabel->setPixmap(makeBarChartPixmap(gouvernoratCounts, 380, 135));
    }

    QString topGov = "";
    int maxCmd = 0;
    QJsonObject jsonCounts;
    for (auto it = gouvernoratCounts.begin(); it != gouvernoratCounts.end(); ++it) {
        jsonCounts[it.key()] = it.value();
        if (it.value() > maxCmd) {
            maxCmd = it.value();
            topGov = it.key();
        }
    }

    QJsonDocument doc(jsonCounts);
    QString countsStr = QString::fromUtf8(doc.toJson(QJsonDocument::Compact));

    sendMapCommand(QString("if (typeof updateAllGovernoratesColors === 'function') { updateAllGovernoratesColors(%1, %2, '%3'); }")
                       .arg(countsStr).arg(maxCmd).arg(topGov));
}

void MainWindow::onDonutAnimationStep(const QVariant &val)
{
    dessinerDonut(val.toDouble());
}

// Donut 3D avec biseau volumétrique, ombre portée et relief
void MainWindow::dessinerDonut(double progress)
{
    if (!donutLabel) return;

    const int sz = 130;
    const int penWidth = 16;

    QPixmap px(sz, sz);
    px.fill(Qt::transparent);
    QPainter p(&px);
    p.setRenderHint(QPainter::Antialiasing);

    // 1. Ombre portée 3D en arrière-plan
    QRectF shadowRect(penWidth / 2.0 + 3, penWidth / 2.0 + 6, sz - penWidth - 6, sz - penWidth - 6);
    p.setPen(QPen(QColor(0, 0, 0, 30), penWidth + 2));
    p.drawArc(shadowRect, 0, 360 * 16);

    // 2. Base circulaire avec relief 3D
    QRectF r(penWidth / 2.0 + 3, penWidth / 2.0 + 3, sz - penWidth - 6, sz - penWidth - 6);
    p.setPen(QPen(QColor(0xE2, 0xE8, 0xF0), penWidth));
    p.drawArc(r, 0, 360 * 16);

    if (animTotalCmd > 0) {
        double spanLivree  = (animCountLivree * 360.0 / animTotalCmd) * progress;
        double spanEnCours = (animCountEnCours * 360.0 / animTotalCmd) * progress;
        double spanPrete   = (animCountPrete * 360.0 / animTotalCmd) * progress;

        double currentAngle = 90.0;

        auto draw3DSegment = [&](double angle, double span, const QColor &colBase) {
            if (span <= 0) return;

            // Couche d'ombre biseautée
            QColor colDark = colBase.darker(140);
            p.setPen(QPen(colDark, penWidth, Qt::SolidLine, Qt::FlatCap));
            p.drawArc(r, static_cast<int>((angle - 0.5) * 16), static_cast<int>(span * 16));

            // Corps principal coloré
            p.setPen(QPen(colBase, penWidth - 2, Qt::SolidLine, Qt::FlatCap));
            p.drawArc(r, static_cast<int>(angle * 16), static_cast<int>(span * 16));

            // Biseau lumineux supérieur (effet de lumière 3D)
            QColor colLight = colBase.lighter(135);
            colLight.setAlpha(160);
            QRectF innerHighlight(r.x() + 2, r.y() + 2, r.width() - 4, r.height() - 4);
            p.setPen(QPen(colLight, 2, Qt::SolidLine, Qt::FlatCap));
            p.drawArc(innerHighlight, static_cast<int>(angle * 16), static_cast<int>(span * 16));
        };

        if (spanLivree > 0) {
            draw3DSegment(currentAngle, spanLivree, QColor(0x10, 0xB9, 0x81));
            currentAngle += spanLivree;
        }

        if (spanEnCours > 0) {
            draw3DSegment(currentAngle, spanEnCours, QColor(0xF5, 0x9E, 0x0B));
            currentAngle += spanEnCours;
        }

        if (spanPrete > 0) {
            draw3DSegment(currentAngle, spanPrete, QColor(0x25, 0x63, 0xEB));
        }
    }

    // Cercle central avec relief interne
    p.setPen(QPen(QColor(0, 0, 0, 15), 1.5));
    p.setBrush(QColor(0xFF, 0xFF, 0xFF));
    p.drawEllipse(QPointF(sz / 2.0, sz / 2.0), (sz / 2.0) - penWidth - 1, (sz / 2.0) - penWidth - 1);

    // Texte central
    p.setPen(QColor(NAVY));
    QFont fDonut("Segoe UI");
    fDonut.setPixelSize(14);
    fDonut.setBold(true);
    p.setFont(fDonut);
    p.drawText(QRectF(0, 0, sz, sz), Qt::AlignCenter, QString("%1\ncmd").arg(animTotalCmd));
    p.end();

    donutLabel->setPixmap(px);
}

void MainWindow::setupOrders()
{
    // Requête robuste : s'il n'y a pas de correspondance dans CLIENTS, on récupère directement
    // les données enregistrées sans écraser le nom de toutes les lignes
    QSqlQuery q("SELECT c.ID_COMMANDE, "
                "       NVL(c.ID_CLIENT, 'CL-001') AS ID_CLIENT, "
                "       NVL(cl.NOM_CLIENT || ' ' || cl.PRENOM_CLIENT, 'Client') AS NOM_CLIENT, "
                "       NVL(c.TELEPHONE, NVL(cl.TELEPHONE, '')) AS TELEPHONE, "
                "       NVL(c.ADRESSE_LIVRAISON, 'Tunis, Les Berges du Lac, Lac 1') AS ADRESSE_LIVRAISON, "
                "       TO_CHAR(c.DATE_COMMANDE, 'DD/MM/YYYY') AS DATE_COMMANDE, "
                "       NVL(c.STATUT, 'En cours') AS STATUT, "
                "       NVL(c.MONTANT_TOTAL, 0) AS MONTANT_TOTAL, "
                "       NVL(c.ARTICLE, '') AS ARTICLE "
                "FROM COMMANDES c "
                "LEFT JOIN CLIENTS cl ON c.ID_CLIENT = cl.ID_CLIENT "
                "ORDER BY c.ID_COMMANDE ASC");

    ordersTable->setRowCount(0);
    int r = 0;

    while (q.next()) {
        ordersTable->insertRow(r);

        QString id = q.value(0).toString();
        QString client = q.value(1).toString();
        QString name = q.value(2).toString();
        QString phone = q.value(3).toString();
        QString addr = q.value(4).toString();
        QString date = q.value(5).toString();
        QString status = q.value(6).toString();
        double amountVal = q.value(7).toDouble();
        QString amount = QString::number(amountVal, 'f', 2) + " TND";
        QString article = q.value(8).toString();

        auto *chkWidget = new QWidget;
        auto *chkLayout = new QHBoxLayout(chkWidget);
        chkLayout->setContentsMargins(0, 0, 0, 0);
        chkLayout->setAlignment(Qt::AlignCenter);
        chkLayout->addWidget(new QCheckBox);
        ordersTable->setCellWidget(r, 0, chkWidget);

        auto *idItem = new QTableWidgetItem(id);

        QString imgName = "prod_veste.png";
        if (article.contains("Bijou", Qt::CaseInsensitive))        imgName = "prod_bijoux.png";
        else if (article.contains("Lunette", Qt::CaseInsensitive)) imgName = "prod_lunette.png";
        else if (article.contains("Chaus", Qt::CaseInsensitive))   imgName = "prod_chaussure.png";
        else if (article.contains("Sac", Qt::CaseInsensitive))      imgName = "prod_sac.png";
        else if (article.contains("Veste", Qt::CaseInsensitive))   imgName = "prod_veste.png";

        QPixmap pm(":/images/" + imgName);
        if (!pm.isNull()) idItem->setIcon(QIcon(pm.scaled(22, 22, Qt::KeepAspectRatio, Qt::SmoothTransformation)));

        QFont fId("Segoe UI");
        fId.setPointSizeF(8.5);
        fId.setBold(true);
        idItem->setFont(fId);

        ordersTable->setItem(r, 1, idItem);
        ordersTable->setItem(r, 2, new QTableWidgetItem(client));
        ordersTable->setItem(r, 3, new QTableWidgetItem(name));
        ordersTable->setItem(r, 4, new QTableWidgetItem(phone));
        ordersTable->setItem(r, 5, new QTableWidgetItem(addr));
        ordersTable->setItem(r, 6, new QTableWidgetItem(date));

        auto *badgeWidget = new QWidget;
        auto *bLayout = new QHBoxLayout(badgeWidget);
        bLayout->setContentsMargins(1, 1, 1, 1);
        bLayout->setAlignment(Qt::AlignCenter);
        auto *badge = new QLabel(status);
        badge->setAlignment(Qt::AlignCenter);

        if (status == "Livrée") {
            badge->setStyleSheet("background:#ECFDF5; color:#059669; border-radius:6px; padding:1px 6px; font-weight:bold; font-size:8.5px;");
        } else if (status == "En cours") {
            badge->setStyleSheet("background:#FFFBEB; color:#D97706; border-radius:6px; padding:1px 6px; font-weight:bold; font-size:8.5px;");
        } else {
            badge->setStyleSheet("background:#EFF6FF; color:#2563EB; border-radius:6px; padding:1px 6px; font-weight:bold; font-size:8.5px;");
        }
        bLayout->addWidget(badge);
        ordersTable->setCellWidget(r, 7, badgeWidget);

        auto *amtItem = new QTableWidgetItem(amount);
        QFont fAmt("Segoe UI");
        fAmt.setPointSizeF(8.0);
        fAmt.setWeight(QFont::DemiBold);
        amtItem->setFont(fAmt);
        ordersTable->setItem(r, 8, amtItem);

        ordersTable->setRowHeight(r, 30);
        r++;
    }

    if (ordersTable->rowCount() > 0) {
        ordersTable->selectRow(0);
        updateDetails(0);
    }

    rafraichirMarqueursCarte();
    recalculerStatistiquesEtDonut();
    recalculerDelaiLivraisonEstime();
}

QWidget* MainWindow::makeDetailsPanel()
{
    auto *f = new QFrame;
    f->setStyleSheet(cardStyle());
    auto *v = new QVBoxLayout(f);
    v->setContentsMargins(12, 10, 12, 10);
    v->setSpacing(6);

    auto *headerBox = new QHBoxLayout;
    headerBox->addWidget(new QLabel("<b style='font-size:13px; color:#0F172A;'>⚙️  Détails de la commande</b>"));
    headerBox->addStretch();
    selectedOrderLabel = new QLabel("CMD-001");
    selectedOrderLabel->setStyleSheet("background:#EFF6FF; color:#1D4ED8; border-radius:4px; padding:2px 7px; font-weight:bold; font-size:10.5px;");
    headerBox->addWidget(selectedOrderLabel);
    v->addLayout(headerBox);

    auto *hContent = new QHBoxLayout;
    hContent->setSpacing(12);

    auto *colClient = new QVBoxLayout;
    colClient->setSpacing(4);

    auto *prodCard = new QFrame;
    prodCard->setStyleSheet("QFrame{background:#F8FAFC; border:1px solid #E2E8F0; border-radius:6px;}");
    auto *prodLayout = new QHBoxLayout(prodCard);
    prodLayout->setContentsMargins(6, 6, 6, 6);
    prodLayout->setSpacing(10);

    detailProductImage = new QLabel;
    detailProductImage->setFixedSize(55, 55);
    detailProductImage->setScaledContents(true);
    detailProductImage->setAlignment(Qt::AlignCenter);
    detailProductImage->setStyleSheet("background:white; border:1px solid #CBD5E1; border-radius:5px; padding:2px;");
    prodLayout->addWidget(detailProductImage);

    detailProdInfo = new QLabel("<b style='font-size:12px; color:#0F172A;'>Robe Élégance</b><br><span style='color:#64748B; font-size:10.5px; line-height:16px;'>Taille : Standard &nbsp;|&nbsp; Quantité : 1</span>");
    prodLayout->addWidget(detailProdInfo, 1);
    colClient->addWidget(prodCard);

    detailClient  = new QLabel;
    detailPhone   = new QLabel;
    detailDate    = new QLabel;
    detailStatus  = new QLabel;
    detailAmount  = new QLabel;
    detailPayment = new QLabel("💳  Paiement : <b>Carte bancaire</b>");
    detailAddress = new QLabel;

    for(auto *x : {detailClient, detailPhone, detailDate, detailStatus, detailAmount, detailPayment, detailAddress}) {
        x->setStyleSheet("font-size: 11px; color: #1E293B; padding: 1px 0px;");
        x->setWordWrap(true);
        colClient->addWidget(x);
    }

    auto *actionBtnsLayout = new QHBoxLayout;
    actionBtnsLayout->setSpacing(6);

    auto *detailsBtn = new QPushButton("  Détails");
    detailsBtn->setIcon(makeButtonIcon(IconEye, QColor(0xFF, 0xFF, 0xFF)));
    detailsBtn->setIconSize(QSize(15, 15));
    detailsBtn->setCursor(Qt::PointingHandCursor);
    detailsBtn->setFixedHeight(26);
    detailsBtn->setStyleSheet(QString("QPushButton{background:%1; color:white; border:none; border-radius:5px; font-weight:600; font-size:10.5px;} QPushButton:hover{background:#1B5FA7;}").arg(NAVY));
    actionBtnsLayout->addWidget(detailsBtn, 1);

    auto *pdfBtn = new QPushButton("  Facture PDF");
    pdfBtn->setIcon(makeButtonIcon(IconFile, QColor(0x08, 0x21, 0x3B)));
    pdfBtn->setIconSize(QSize(15, 15));
    pdfBtn->setCursor(Qt::PointingHandCursor);
    pdfBtn->setFixedHeight(26);
    pdfBtn->setStyleSheet("QPushButton{background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #F3D082, stop:1 #C89538); color:#082849; border:none; border-radius:5px; font-weight:bold; font-size:10.5px;} QPushButton:hover{background:#FFE4A3;}");
    connect(pdfBtn, &QPushButton::clicked, this, &MainWindow::generateInvoicePdf);
    actionBtnsLayout->addWidget(pdfBtn, 1);

    colClient->addLayout(actionBtnsLayout);
    hContent->addLayout(colClient, 1);

    auto *est = new QFrame;
    est->setStyleSheet("QFrame{background:#FFFDF7; border:1px solid #F3E4C8; border-radius:6px;}");
    auto *eg = new QGridLayout(est);
    eg->setContentsMargins(10, 8, 10, 8);
    eg->setSpacing(6);

    eg->addWidget(new QLabel("<b style='color:#B45309; font-size:11px;'>◎  Itinéraire multimodal & Coûts</b>"), 0, 0, 1, 2);

    distanceLabel = new QLabel;
    costLabel     = new QLabel;
    delayLabel    = new QLabel;

    eg->addWidget(new QLabel("<span style='color:#78350F; font-size:10.5px;'>Distance atelier → client</span>"), 1, 0);
    eg->addWidget(distanceLabel, 1, 1, Qt::AlignRight);

    eg->addWidget(new QLabel("<span style='color:#78350F; font-size:10.5px;'>Frais de livraison estimés</span>"), 2, 0);
    eg->addWidget(costLabel, 2, 1, Qt::AlignRight);

    eg->addWidget(new QLabel("<span style='color:#78350F; font-size:10.5px;'>Temps de trajet estimé</span>"), 3, 0);
    eg->addWidget(delayLabel, 3, 1, Qt::AlignRight);

    auto *route = new QPushButton("📍  Voir l'itinéraire sur la carte");
    route->setCursor(Qt::PointingHandCursor);
    route->setFixedHeight(24);
    route->setStyleSheet("QPushButton{background:white; border:1px solid #CBD5E1; border-radius:4px; font-size:10px; font-weight:600; color:#1E293B;} QPushButton:hover{background:#F8FAFC;}");
    connect(route, &QPushButton::clicked, this, &MainWindow::showSelectedRoute);
    eg->addWidget(route, 4, 0, 1, 2);
    hContent->addWidget(est, 1);

    auto *follow = new QFrame;
    follow->setStyleSheet("QFrame{background:#FFFFFF; border:1px solid #E2E8F0; border-radius:6px;}");
    auto *fg = new QVBoxLayout(follow);
    fg->setContentsMargins(10, 8, 10, 8);
    fg->setSpacing(4);
    fg->addWidget(new QLabel("<b style='color:#0F172A; font-size:11px;'>◉  Suivi</b>"));

    auto makeStep = [](const QString &icon, const QString &text, const QString &date, bool done){
        auto *row = new QHBoxLayout;
        row->setContentsMargins(0, 1, 0, 1);
        row->setSpacing(6);

        auto *lblIco = new QLabel(icon);
        lblIco->setFixedWidth(24);
        lblIco->setAlignment(Qt::AlignCenter);

        auto *lblTxt = new QLabel(QString("<span style='color:%1; font-weight:%2; font-size:10px;'>%3</span>")
                                      .arg(done ? "#0F172A" : "#94A3B8", done ? "600" : "normal", text));
        lblTxt->setStyleSheet("background:transparent; border:none;");

        auto *lblDt  = new QLabel(QString("<span style='color:#94A3B8; font-size:9.5px;'>%1</span>").arg(date));
        lblDt->setStyleSheet("background:transparent; border:none;");

        row->addWidget(lblIco);
        row->addWidget(lblTxt, 1);
        row->addWidget(lblDt);
        return row;
    };

    fg->addLayout(makeStep("🟢", "Commande créée", "26/09 14:32", true));
    fg->addLayout(makeStep("🟢", "En préparation", "26/09 16:10", true));
    fg->addLayout(makeStep("🔵", "Prête en atelier", "24/09 16:50", true));
    fg->addLayout(makeStep("⚪", "Expédiée", "—", false));
    fg->addLayout(makeStep("⚪", "Livrée", "—", false));

    hContent->addWidget(follow, 1);

    v->addLayout(hContent);
    return f;
}

void MainWindow::generateInvoicePdf()
{
    int r = currentRowSelected;
    if (r < 0 || r >= ordersTable->rowCount()) {
        QMessageBox::warning(this, "Sélection requise", "Veuillez sélectionner une commande dans le tableau pour générer sa facture.");
        return;
    }

    QString id     = ordersTable->item(r, 1)->text();
    QString client = ordersTable->item(r, 2)->text();
    QString name   = ordersTable->item(r, 3)->text();
    QString addr   = ordersTable->item(r, 5)->text();
    QString date   = ordersTable->item(r, 6)->text();
    QString amount = ordersTable->item(r, 8)->text();

    QString articleName = "Robe Élégance";
    QString paymentMode = "Carte bancaire";

    QSqlQuery q;
    q.prepare("SELECT MODE_PAIEMENT, NVL(ARTICLE, '') FROM COMMANDES WHERE ID_COMMANDE = :id");
    q.bindValue(":id", id);
    if (q.exec() && q.next()) {
        paymentMode = q.value(0).toString();
        QString art = q.value(1).toString().trimmed();
        if (!art.isEmpty()) articleName = art;
    }

    double totalTTC = amount.split(" ").first().replace(",", ".").toDouble();
    double totalHT  = totalTTC / 1.19;
    double totalTVA = totalTTC - totalHT;

    QString defaultName = QString("Facture_Fashionova_%1.pdf").arg(id);
    QString fileName = QFileDialog::getSaveFileName(this, "Enregistrer la Facture PDF", defaultName, "Documents PDF (*.pdf)");
    if (fileName.isEmpty()) return;

    QString html = QString(R"(
        <!DOCTYPE html>
        <html>
        <head>
            <meta charset="utf-8">
            <style>
                body { font-family: 'Segoe UI', Arial, sans-serif; color: #1E293B; margin: 0; padding: 20px; }
                .header-table { width: 100%; border-bottom: 3px solid #D8A64B; padding-bottom: 20px; margin-bottom: 25px; }
                .brand { font-size: 32px; font-weight: bold; color: #082849; letter-spacing: 2px; }
                .sub-brand { font-size: 13px; color: #B45309; text-transform: uppercase; letter-spacing: 1.5px; font-weight: 600; }
                .title { font-size: 26px; font-weight: bold; color: #082849; text-align: right; }
                .details-table { width: 100%; margin-bottom: 30px; }
                .box { background: #F8FAFC; border: 1px solid #CBD5E1; border-radius: 8px; padding: 16px; font-size: 13px; line-height: 20px; }
                .items-table { width: 100%; border-collapse: collapse; margin-bottom: 30px; }
                .items-table th { background-color: #082849; color: #FFFFFF; font-size: 13px; padding: 12px 10px; text-align: left; }
                .items-table td { padding: 14px 10px; border-bottom: 1px solid #E2E8F0; font-size: 13px; }
                .total-box { float: right; width: 45%; margin-bottom: 30px; }
                .total-table { width: 100%; font-size: 14px; border-collapse: collapse; }
                .total-table td { padding: 6px 4px; }
                .total-amount { font-size: 20px; font-weight: bold; color: #082849; }
                .footer { margin-top: 50px; text-align: center; font-size: 11px; color: #94A3B8; border-top: 1px solid #E2E8F0; padding-top: 15px; }
            </style>
        </head>
        <body>
            <table class="header-table">
                <tr>
                    <td>
                        <div class="brand">FASHIONOVA</div>
                        <div class="sub-brand">Haute Couture & Prêt-à-Porter Tunisien</div>
                    </td>
                    <td class="title">FACTURE OFFICIELLE<br><span style="font-size: 14px; color: #64748B;">N° %1</span></td>
                </tr>
            </table>

            <table class="details-table">
                <tr>
                    <td width="48%" valign="top" class="box">
                        <b style="color: #082849; font-size: 14px;">Émetteur :</b><br>
                        <b>Fashionova Prestige SARL</b><br>
                        Atelier Central & Logistique Haute Couture<br>
                        Les Berges du Lac, Tunis 1053<br>
                        Tél : +216 71 000 000
                    </td>
                    <td width="4%"></td>
                    <td width="48%" valign="top" class="box">
                        <b style="color: #082849; font-size: 14px;">Destinataire :</b><br>
                        <b>%2</b> (Code Client : %3)<br>
                        Adresse : %4<br>
                        Date d'émission : %5<br>
                        Mode de règlement : <b>%6</b>
                    </td>
                </tr>
            </table>

            <table class="items-table">
                <thead>
                    <tr>
                        <th width="45%">Désignation Article</th>
                        <th width="15%" style="text-align: center;">Quantité</th>
                        <th width="20%">Prix Unitaire HT</th>
                        <th width="20%">Total TTC</th>
                    </tr>
                </thead>
                <tbody>
                    <tr>
                        <td><b>%7</b><br><span style="color: #64748B; font-size: 11px;">Réf: %1 &nbsp;|&nbsp; Finition Atelier Haute Qualité</span></td>
                        <td align="center">1</td>
                        <td>%8 TND</td>
                        <td><b>%9 TND</b></td>
                    </tr>
                </tbody>
            </table>

            <div class="total-box">
                <table class="total-table">
                    <tr><td style="color: #64748B;">Total Net HT :</td><td align="right"><b>%8 TND</b></td></tr>
                    <tr><td style="color: #64748B;">TVA (19%) :</td><td align="right"><b>%10 TND</b></td></tr>
                    <tr><td style="color: #64748B;">Frais de livraison :</td><td align="right"><b style="color: #10B981;">Offerts (Gratuit)</b></td></tr>
                    <tr><td colspan="2"><hr style="border: none; border-top: 1px solid #CBD5E1; margin: 8px 0;"></td></tr>
                    <tr><td><span class="total-amount">Total TTC :</span></td><td align="right"><span class="total-amount">%9 TND</span></td></tr>
                </table>
            </div>
            <div style="clear: both;"></div>
            <div class="footer">
                Merci de votre confiance. Fashionova Prestige SARL — Système Fashionova ERP.
            </div>
        </body>
        </html>
    )").arg(id, name, client, addr, date, paymentMode, articleName,
                            QString::number(totalHT, 'f', 2), QString::number(totalTTC, 'f', 2), QString::number(totalTVA, 'f', 2));

    QPrinter printer(QPrinter::ScreenResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(fileName);
    printer.setPageSize(QPageSize(QPageSize::A4));
    printer.setPageMargins(QMarginsF(15, 15, 15, 15), QPageLayout::Millimeter);

    QTextDocument doc;
    doc.setPageSize(printer.pageRect(QPrinter::Point).size());
    doc.setHtml(html);
    doc.print(&printer);

    QMessageBox::information(this, "Facture Générée", QString("La facture officielle <b>%1</b> a été générée avec succès !").arg(id));
}

QWidget* MainWindow::makeMapPanel()
{
    auto *f = new QFrame;
    f->setStyleSheet(cardStyle());
    f->setFixedWidth(1310);
    auto *v = new QVBoxLayout(f);
    v->setContentsMargins(8, 5, 8, 5);
    v->setSpacing(4);

    auto *title = new QHBoxLayout;
    title->addWidget(new QLabel("<b style='font-size:12px; color:#0F172A;'>📍  Carte interactive & Répartition géographique par gouvernorat (Tunisie)</b>"));
    title->addStretch();

    auto *btnRoute = new QPushButton("  Itinéraire");
    btnRoute->setIcon(makeButtonIcon(IconCar, QColor(0xFF, 0xFF, 0xFF)));
    btnRoute->setIconSize(QSize(15, 15));
    btnRoute->setCursor(Qt::PointingHandCursor);
    btnRoute->setStyleSheet("QPushButton{background:#10B981; color:white; border:none; border-radius:4px; padding:2px 10px; font-weight:600; font-size:10.5px;} QPushButton:hover{background:#059669;}");
    connect(btnRoute, &QPushButton::clicked, this, &MainWindow::showSelectedRoute);
    title->addWidget(btnRoute);

    auto *loc = new QPushButton("  Localiser");
    loc->setIcon(makeButtonIcon(IconLocation, QColor(0xFF, 0xFF, 0xFF)));
    loc->setIconSize(QSize(15, 15));
    loc->setCursor(Qt::PointingHandCursor);
    loc->setStyleSheet("QPushButton{background:#082849; color:white; border:none; border-radius:4px; padding:2px 10px; font-weight:600; font-size:10.5px;} QPushButton:hover{background:#1B5FA7;}");
    connect(loc, &QPushButton::clicked, this, &MainWindow::centerMap);
    title->addWidget(loc);

    v->addLayout(title);

    mapView = new QWebEngineView;
    mapView->setFixedHeight(440);
    mapView->setUrl(QUrl("qrc:/web/map.html"));

    connect(mapView, &QWebEngineView::loadFinished, this, [this](bool ok){
        if (ok) {
            rafraichirMarqueursCarte();
            recalculerStatistiquesEtDonut();
        }
    });

    v->addWidget(mapView, 1);
    return f;
}

QWidget* MainWindow::makeKpis()
{
    auto *w = new QWidget;
    w->setFixedWidth(1310);
    auto *g = new QGridLayout(w);
    g->setContentsMargins(0, 8, 0, 0);
    g->setSpacing(8);

    struct KpiConfig {
        QString title;
        QColor iconColor;
        QString bgCircleColor;
        bool isDown;
    };

    const QList<KpiConfig> kpis = {
        {"Total commandes", QColor(0x25, 0x63, 0xEB), "#EBF2FE", false},
        {"En attente",      QColor(0xB4, 0x53, 0x09), "#FEF3C7", false},
        {"Expédiées",       QColor(0x10, 0xB9, 0x81), "#E6F8F0", false},
        {"En atelier",      QColor(0xEF, 0x44, 0x44), "#FEE8E8", true},
        {"Montant total",   QColor(0x1D, 0x4E, 0xD8), "#EBF2FE", false}
    };

    for (int i = 0; i < kpis.size(); ++i) {
        const auto &k = kpis[i];

        auto *card = new QFrame;
        card->setStyleSheet("QFrame { background:#FFFFFF; border:1px solid #E2E8F0; border-radius:10px; }");
        card->setFixedHeight(76);

        auto *cardLayout = new QHBoxLayout(card);
        cardLayout->setContentsMargins(10, 8, 10, 8);
        cardLayout->setSpacing(8);

        auto *iconLabel = new QLabel;
        iconLabel->setFixedSize(38, 38);
        iconLabel->setAlignment(Qt::AlignCenter);
        iconLabel->setStyleSheet(QString("background-color: %1; border-radius: 19px;").arg(k.bgCircleColor));
        iconLabel->setPixmap(makeKpiIcon(i, k.iconColor));
        cardLayout->addWidget(iconLabel);

        auto *infoLayout = new QVBoxLayout;
        infoLayout->setContentsMargins(0, 0, 0, 0);
        infoLayout->setSpacing(1);

        auto *titleLbl = new QLabel(k.title);
        titleLbl->setStyleSheet("color: #475569; font-size: 10.5px; font-weight: 600;");

        auto *valLbl = new QLabel("0");
        valLbl->setStyleSheet("color: #0F172A; font-size: 15px; font-weight: bold;");

        if (i == 0) kpiTotalVal = valLbl;
        else if (i == 1) kpiEnAttenteVal = valLbl;
        else if (i == 2) kpiExpedieesVal = valLbl;
        else if (i == 3) kpiAtelierVal = valLbl;
        else if (i == 4) kpiMontantTotalVal = valLbl;

        auto *trendLbl = new QLabel("Synchronisé");
        trendLbl->setStyleSheet("color: #10B981; font-size: 9px; font-weight: 600;");

        infoLayout->addWidget(titleLbl);
        infoLayout->addWidget(valLbl);
        infoLayout->addWidget(trendLbl);
        cardLayout->addLayout(infoLayout, 1);

        auto *sparkLabel = new QLabel;
        sparkLabel->setFixedSize(60, 36);
        sparkLabel->setPixmap(makeSparklineWithGradient(k.iconColor, k.isDown, 60, 36));
        cardLayout->addWidget(sparkLabel, 0, Qt::AlignBottom);

        g->addWidget(card, 0, i);
    }
    return w;
}

QWidget* MainWindow::makeBottomPanel()
{
    auto *w = new QWidget;
    w->setFixedWidth(1310);
    w->setFixedHeight(180);
    auto *g = new QGridLayout(w);
    g->setContentsMargins(0, 0, 0, 10);
    g->setSpacing(8);

    // 1. Donut 3D volumétrique (Statut des commandes)
    auto *donut = new QFrame;
    donut->setStyleSheet(cardStyle());
    auto *dv = new QVBoxLayout(donut);
    dv->setContentsMargins(10, 8, 10, 8);
    dv->setSpacing(4);
    dv->addWidget(new QLabel("<b style='color:#0F172A; font-size:11.5px;'>Statuts des commandes</b>"));

    auto *donutContent = new QHBoxLayout;
    donutLabel = new QLabel;
    donutLabel->setFixedSize(130, 130);
    donutContent->addWidget(donutLabel);

    legendLayout = new QVBoxLayout;
    legendLayout->setSpacing(4);
    donutContent->addLayout(legendLayout, 1);
    dv->addLayout(donutContent);

    // 2. Bar Chart des gouvernorats (3D dégradé)
    auto *govChartFrame = new QFrame;
    govChartFrame->setStyleSheet(cardStyle());
    auto *gv = new QVBoxLayout(govChartFrame);
    gv->setContentsMargins(10, 8, 10, 8);
    gv->setSpacing(3);
    gv->addWidget(new QLabel("<b style='color:#0F172A; font-size:11.5px;'>Top Régions (Commandes)</b>"));

    govBarChartLabel = new QLabel;
    govBarChartLabel->setAlignment(Qt::AlignCenter);
    gv->addWidget(govBarChartLabel);

    // 3. Alertes & Logistique
    auto *alerts = new QFrame;
    alerts->setStyleSheet(cardStyle());
    auto *av = new QVBoxLayout(alerts);
    av->setContentsMargins(10, 8, 10, 8);
    av->setSpacing(5);
    av->addWidget(new QLabel("<b style='color:#0F172A; font-size:11.5px;'>💡 Alertes & Logistique</b>"));

    auto addAlert = [&](const QString &icon, const QString &text){
        auto *row = new QHBoxLayout;
        auto *ico = new QLabel(icon);
        auto *lbl = new QLabel(text);
        lbl->setWordWrap(true);
        lbl->setStyleSheet("font-size:10px; color:#334155;");
        auto *btn = new QPushButton("Voir");
        btn->setCursor(Qt::PointingHandCursor);
        btn->setFixedSize(32, 19);
        btn->setStyleSheet("QPushButton{background:#F1F5F9; border:1px solid #CBD5E1; border-radius:3px; font-size:9px; font-weight:600;} QPushButton:hover{background:#E2E8F0;}");
        row->addWidget(ico);
        row->addWidget(lbl, 1);
        row->addWidget(btn);
        av->addLayout(row);
    };

    addAlert("🔴", "3 retards de livraison (> 24h)");
    addAlert("🟢", "Délégations Kasserine synchronisées");
    addAlert("🟠", "Stock faible : Manteaux (5 restants)");

    g->addWidget(donut, 0, 0);
    g->addWidget(govChartFrame, 0, 1);
    g->addWidget(alerts, 0, 2);

    g->setColumnStretch(0, 1);
    g->setColumnStretch(1, 1);
    g->setColumnStretch(2, 1);

    return w;
}

void MainWindow::setupFloatingChatbot()
{
    chatFloatingBtn = new AnimatedBotButton(this);
    chatFloatingBtn->setToolTip("Assistant IA Fashionova (Cliquez pour ouvrir)");
    connect(chatFloatingBtn, &QPushButton::clicked, this, &MainWindow::toggleChatWindow);

    chatWindow = new QFrame(this);
    chatWindow->setFixedSize(330, 420);
    chatWindow->setStyleSheet(R"(
        QFrame#ChatMain {
            background-color: #FFFFFF;
            border: 1px solid #CBD5E1;
            border-radius: 12px;
        }
    )");
    chatWindow->setObjectName("ChatMain");
    chatWindow->hide();

    auto *winLayout = new QVBoxLayout(chatWindow);
    winLayout->setContentsMargins(0, 0, 0, 0);
    winLayout->setSpacing(0);

    auto *header = new QFrame;
    header->setFixedHeight(50);
    header->setStyleSheet("background: #082849; border-top-left-radius: 11px; border-top-right-radius: 11px;");
    auto *headLayout = new QHBoxLayout(header);
    headLayout->setContentsMargins(12, 0, 12, 0);

    auto *titleLbl = new QLabel("<b style='color:#FFFFFF; font-size:12px;'>🤖 StyleBot IA</b><br><span style='color:#94A3B8; font-size:9.5px;'>Fashionova Copilot</span>");
    auto *closeBtn = new QPushButton("✕");
    closeBtn->setFixedSize(24, 24);
    closeBtn->setCursor(Qt::PointingHandCursor);
    closeBtn->setStyleSheet("QPushButton{background:transparent; color:#CBD5E1; border:none; font-weight:bold; font-size:12px;} QPushButton:hover{color:#FFFFFF;}");
    connect(closeBtn, &QPushButton::clicked, this, &MainWindow::toggleChatWindow);

    headLayout->addWidget(titleLbl);
    headLayout->addStretch();
    headLayout->addWidget(closeBtn);
    winLayout->addWidget(header);

    auto *scroll = new QScrollArea;
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);
    scroll->setStyleSheet("background-color: #F8FAFC; border: none;");

    chatMessagesArea = new QWidget;
    chatMessagesArea->setStyleSheet("background-color: #F8FAFC;");
    chatMessagesLayout = new QVBoxLayout(chatMessagesArea);
    chatMessagesLayout->setContentsMargins(10, 10, 10, 10);
    chatMessagesLayout->setSpacing(8);
    chatMessagesLayout->addStretch();

    scroll->setWidget(chatMessagesArea);
    winLayout->addWidget(scroll, 1);

    auto *inputFrame = new QFrame;
    inputFrame->setFixedHeight(50);
    inputFrame->setStyleSheet("background: #FFFFFF; border-top: 1px solid #E2E8F0; border-bottom-left-radius: 11px; border-bottom-right-radius: 11px;");
    auto *inLayout = new QHBoxLayout(inputFrame);
    inLayout->setContentsMargins(8, 6, 8, 6);
    inLayout->setSpacing(6);

    chatInput = new QLineEdit;
    chatInput->setPlaceholderText("Posez une question ou ordonnez une commande...");
    chatInput->setStyleSheet("QLineEdit { background:#F1F5F9; border:1px solid #CBD5E1; border-radius:16px; padding:0 12px; font-size:11px; }");
    connect(chatInput, &QLineEdit::returnPressed, this, &MainWindow::handleSendChatMessage);

    auto *sendBtn = new QPushButton("➤");
    sendBtn->setFixedSize(32, 32);
    sendBtn->setCursor(Qt::PointingHandCursor);
    sendBtn->setStyleSheet("QPushButton{background:#082849; color:white; border-radius:16px; font-weight:bold;} QPushButton:hover{background:#1B5FA7;}");
    connect(sendBtn, &QPushButton::clicked, this, &MainWindow::handleSendChatMessage);

    inLayout->addWidget(chatInput, 1);
    inLayout->addWidget(sendBtn);
    winLayout->addWidget(inputFrame);

    addChatMessage("Bot", "Bonjour Mohamed Anoir ! Je suis votre copilote IA StyleHub. Je peux analyser vos livraisons, tracer les itinéraires vers n'importe quelle délégation et actualiser vos commandes en temps réel.", false);
}

void MainWindow::setupFloatingActionButtons()
{
    int btnSz = 52;
    if (chatFloatingBtn) btnSz = chatFloatingBtn->width();

    btnWhatsAppFloating = new QPushButton(this);
    btnWhatsAppFloating->setFixedSize(btnSz, btnSz);
    btnWhatsAppFloating->setIcon(QIcon(makeFloatingIconPixmap("whatsapp", btnSz)));
    btnWhatsAppFloating->setIconSize(QSize(btnSz, btnSz));
    btnWhatsAppFloating->setToolTip("💬 Envoyer un message WhatsApp au client");
    btnWhatsAppFloating->setCursor(Qt::PointingHandCursor);
    btnWhatsAppFloating->setStyleSheet("QPushButton { border: none; background: transparent; }");
    connect(btnWhatsAppFloating, &QPushButton::clicked, this, &MainWindow::onBtnWhatsAppClicked);

    btnSmsFloating = new QPushButton(this);
    btnSmsFloating->setFixedSize(btnSz, btnSz);
    btnSmsFloating->setIcon(QIcon(makeFloatingIconPixmap("sms", btnSz)));
    btnSmsFloating->setIconSize(QSize(btnSz, btnSz));
    btnSmsFloating->setToolTip("📱 Envoyer un SMS direct au client");
    btnSmsFloating->setCursor(Qt::PointingHandCursor);
    btnSmsFloating->setStyleSheet("QPushButton { border: none; background: transparent; }");
    connect(btnSmsFloating, &QPushButton::clicked, this, &MainWindow::onBtnSmsClicked);

    btnWhatsAppFloating->show();
    btnSmsFloating->show();
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);

    int marginX = 25;
    int marginY = 25;
    int spacing = 12;

    if (chatFloatingBtn) {
        int btnW = chatFloatingBtn->width();
        int btnH = chatFloatingBtn->height();

        int botX = width() - btnW - marginX;
        int botY = height() - btnH - marginY;
        chatFloatingBtn->move(botX, botY);
        chatFloatingBtn->raise();

        if (btnWhatsAppFloating) {
            int waY = botY - btnH - spacing;
            btnWhatsAppFloating->move(botX, waY);
            btnWhatsAppFloating->raise();
        }

        if (btnSmsFloating) {
            int smsY = botY - (2 * btnH) - (2 * spacing);
            btnSmsFloating->move(botX, smsY);
            btnSmsFloating->raise();
        }

        if (chatWindow) {
            int winX = width() - chatWindow->width() - marginX;
            int winY = botY - chatWindow->height() - 10;
            chatWindow->move(winX, winY);
            chatWindow->raise();
        }
    }
}

void MainWindow::toggleChatWindow()
{
    if (!chatWindow) return;
    bool visible = !chatWindow->isVisible();
    chatWindow->setVisible(visible);
    if (visible) {
        chatInput->setFocus();
        chatWindow->raise();
    }
}

void MainWindow::addChatMessage(const QString &sender, const QString &text, bool isUser)
{
    Q_UNUSED(sender);
    auto *msgFrame = new QFrame;
    auto *msgLayout = new QHBoxLayout(msgFrame);
    msgLayout->setContentsMargins(0, 0, 0, 0);

    auto *bubble = new QLabel(text);
    bubble->setWordWrap(true);
    bubble->setMaximumWidth(240);

    if (isUser) {
        msgLayout->addStretch();
        bubble->setStyleSheet("background: #082849; color: #FFFFFF; border-radius: 10px; padding: 7px 11px; font-size: 11px;");
        msgLayout->addWidget(bubble);
    } else {
        bubble->setStyleSheet("background: #FFFFFF; color: #1E293B; border: 1px solid #E2E8F0; border-radius: 10px; padding: 7px 11px; font-size: 11px;");
        msgLayout->addWidget(bubble);
        msgLayout->addStretch();
    }

    int idx = qMax(0, chatMessagesLayout->count() - 1);
    chatMessagesLayout->insertWidget(idx, msgFrame);
}

QString MainWindow::getApiKey()
{
    QStringList paths = {
        QCoreApplication::applicationDirPath() + "/api_key.txt",
        "api_key.txt"
    };

    for (const QString &path : paths) {
        QFile file(path);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QString key = QTextStream(&file).readLine().trimmed();
            if (!key.isEmpty()) return key;
        }
    }
    return "";
}

void MainWindow::handleSendChatMessage()
{
    QString q = chatInput->text().trimmed();
    if (q.isEmpty()) return;

    addChatMessage("Vous", q, true);
    chatInput->clear();

    QString directResponse = processChatbotQuery(q);
    if (!directResponse.isEmpty()) {
        addChatMessage("Bot", directResponse, false);
        return;
    }

    addChatMessage("Bot", "⏳ <i>StyleBot réfléchit...</i>", false);

    QString ordersContext = "Commandes enregistrées dans l'atelier Fashionova :\n";
    for (int r = 0; r < ordersTable->rowCount(); ++r) {
        ordersContext += QString("- %1 | Client: %2 | Ville: %3 | Date: %4 | Statut: %5 | Montant: %6\n")
        .arg(ordersTable->item(r, 1)->text(),
             ordersTable->item(r, 3)->text(),
             ordersTable->item(r, 5)->text(),
             ordersTable->item(r, 6)->text(),
             ordersTable->cellWidget(r, 7)->findChild<QLabel*>()->text(),
             ordersTable->item(r, 8)->text());
    }

    QJsonObject userMsg;
    userMsg["role"] = "user";
    userMsg["content"] = QString("%1\n\nConsigne ou Question : %2").arg(ordersContext, q);
    conversationHistory.append(userMsg);

    QUrl apiUrl("https://api.groq.com/openai/v1/chat/completions");
    QNetworkRequest request(apiUrl);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QString authHeader = "Bearer " + getApiKey();
    request.setRawHeader("Authorization", authHeader.toUtf8());

    QSslConfiguration sslConfig = QSslConfiguration::defaultConfiguration();
    sslConfig.setProtocol(QSsl::TlsV1_2OrLater);
    sslConfig.setPeerVerifyMode(QSslSocket::VerifyNone);
    request.setSslConfiguration(sslConfig);

    QJsonObject payload;
    payload["model"] = "openai/gpt-oss-120b";
    payload["messages"] = conversationHistory;
    payload["temperature"] = 0.5;
    payload["max_tokens"] = 600;

    QNetworkReply *reply = networkManager->post(request, QJsonDocument(payload).toJson());

    connect(reply, &QNetworkReply::sslErrors, reply, [reply](const QList<QSslError> &errors) {
        Q_UNUSED(errors);
        reply->ignoreSslErrors();
    });
}

QString MainWindow::processChatbotQuery(const QString &query)
{
    const QString raw = query.trimmed();
    const QString q = raw.toLower();

    if (q.contains("facture") || q.contains("pdf")) {
        generateInvoicePdf();
        return "📄 <b>Génération PDF :</b> La boîte de dialogue de facture officielle s'est ouverte pour la commande sélectionnée.";
    }

    const QStringList modules = {"client", "employé", "commande", "maquette", "machine", "article"};
    for (int i = 0; i < modules.size(); ++i) {
        if ((q.contains("va sur") || q.contains("ouvre") || q.contains("page") || q.contains("module")) && q.contains(modules[i])) {
            if (pageStack) {
                pageStack->setCurrentIndex(i);
                return QString("🚀 <b>Navigation :</b> Basculement vers le module <b>%1</b>.").arg(modules[i].toUpper());
            }
        }
    }

    QRegularExpression updateStatusRx(R"((?:change|passe|met|marque)\s+(?:la\s+commande\s+)?(?:cmd-)?(\d+)\s+(?:en|comme|a)\s+([a-zA-Z\s]+))", QRegularExpression::CaseInsensitiveOption);
    QRegularExpressionMatch stMatch = updateStatusRx.match(q);
    if (stMatch.hasMatch()) {
        int num = stMatch.captured(1).toInt();
        QString targetId = QString("CMD-%1").arg(num, 3, 10, QChar('0'));
        QString newStatus = stMatch.captured(2).trimmed();

        for (int r = 0; r < ordersTable->rowCount(); ++r) {
            if (ordersTable->item(r, 1) && ordersTable->item(r, 1)->text().compare(targetId, Qt::CaseInsensitive) == 0) {
                auto *badgeWidget = ordersTable->cellWidget(r, 7);
                if (badgeWidget) {
                    auto *badge = badgeWidget->findChild<QLabel*>();
                    if (badge) {
                        if (newStatus.contains("livr")) newStatus = "Livrée";
                        else if (newStatus.contains("cours")) newStatus = "En cours";
                        else newStatus = "Prête en atelier";

                        QSqlQuery qUpdate;
                        qUpdate.prepare("UPDATE COMMANDES SET STATUT = :st WHERE ID_COMMANDE = :id");
                        qUpdate.bindValue(":st", newStatus);
                        qUpdate.bindValue(":id", targetId);
                        qUpdate.exec();

                        actualiserTableau();
                        return QString("⚡ <b>Statut mis à jour :</b> La commande <b>%1</b> est désormais <b>%2</b>.").arg(targetId, newStatus);
                    }
                }
            }
        }
    }

    bool isMath = false;
    double mathRes = evaluateMathExpression(raw, isMath);
    if (isMath) {
        return QString("🧮 <b>Calcul :</b><br><code>%1</code> = <b style='color:#059669; font-size:13px;'>%2</b>")
            .arg(raw, QString::number(mathRes, 'f', 2));
    }

    QRegularExpression cmdNumRx(R"((?:cmd[- ]?|commande\s+|#)(\d+)|(?:\b0*(\d{1,3})\b))", QRegularExpression::CaseInsensitiveOption);
    QRegularExpressionMatchIterator it = cmdNumRx.globalMatch(q);
    int matchedRow = -1;

    while (it.hasNext()) {
        QRegularExpressionMatch match = it.next();
        QString numStr = match.captured(1);
        if (numStr.isEmpty()) numStr = match.captured(2);
        int targetNum = numStr.toInt();
        if (targetNum <= 0) continue;

        QString formattedId = QString("CMD-%1").arg(targetNum, 3, 10, QChar('0'));
        for (int r = 0; r < ordersTable->rowCount(); ++r) {
            if (ordersTable->item(r, 1) && ordersTable->item(r, 1)->text().compare(formattedId, Qt::CaseInsensitive) == 0) {
                matchedRow = r;
                break;
            }
        }
        if (matchedRow != -1) break;
    }

    if (matchedRow == -1) {
        for (int r = 0; r < ordersTable->rowCount(); ++r) {
            QString client = ordersTable->item(r, 2) ? ordersTable->item(r, 2)->text().toLower() : "";
            QString name   = ordersTable->item(r, 3) ? ordersTable->item(r, 3)->text().toLower() : "";
            QString addr   = ordersTable->item(r, 5) ? ordersTable->item(r, 5)->text().toLower() : "";
            if ((!client.isEmpty() && q.contains(client)) || (!name.isEmpty() && q.contains(name.split(" ").first())) || (!addr.isEmpty() && q.contains(addr))) {
                matchedRow = r;
                break;
            }
        }
    }

    if (matchedRow != -1) {
        ordersTable->selectRow(matchedRow);
        updateDetails(matchedRow);

        bool wantsRoute = q.contains("route") || q.contains("chemin") || q.contains("itineraire") ||
                          q.contains("trajet") || q.contains("gps") || q.contains("carte") || q.contains("localisation") || q.contains("temps");

        if (wantsRoute) {
            showSelectedRoute();
            return QString("📍 <b>Itinéraire et localisation en temps réel :</b><br>"
                           "• <b>Commande :</b> %1<br>"
                           "• <b>Client :</b> %2<br>"
                           "• <b>Destination :</b> %3<br>"
                           "• <b>Distance :</b> %4<br>"
                           "• <b>Modes de transport :</b> %5<br>"
                           "<i>Le tracé et le marqueur ont été positionnés sur la carte.</i>")
                .arg(ordersTable->item(matchedRow, 1)->text(),
                     ordersTable->item(matchedRow, 3)->text(),
                     ordersTable->item(matchedRow, 5)->text(),
                     distanceLabel ? distanceLabel->text() : "—",
                     delayLabel ? delayLabel->text() : "—");
        }

        return QString("🎯 <b>Commande sélectionnée :</b> <b>%1</b> (%2)<br>"
                       "• Localisation : <b>%3</b><br>"
                       "• Montant : <b>%4</b><br>"
                       "• Statut : <i>%5</i>.")
            .arg(ordersTable->item(matchedRow, 1)->text(),
                 ordersTable->item(matchedRow, 3)->text(),
                 ordersTable->item(matchedRow, 5)->text(),
                 ordersTable->item(matchedRow, 8)->text(),
                 ordersTable->cellWidget(matchedRow, 7)->findChild<QLabel*>()->text());
    }

    return QString();
}

void MainWindow::updateDetails(int row)
{
    if(row < 0 || row >= ordersTable->rowCount()) return;

    currentRowSelected = row;

    const QString id     = ordersTable->item(row, 1) ? ordersTable->item(row, 1)->text() : "";
    const QString client = ordersTable->item(row, 2) ? ordersTable->item(row, 2)->text() : "";
    const QString name   = ordersTable->item(row, 3) ? ordersTable->item(row, 3)->text() : "";
    const QString phone  = ordersTable->item(row, 4) ? ordersTable->item(row, 4)->text() : "";
    const QString addr   = ordersTable->item(row, 5) ? ordersTable->item(row, 5)->text() : "";
    const QString date   = ordersTable->item(row, 6) ? ordersTable->item(row, 6)->text() : "";
    const QString amount = ordersTable->item(row, 8) ? ordersTable->item(row, 8)->text() : "";

    selectedOrderLabel->setText(id);

    formIdEdit->setText(id);
    formClientCodeEdit->setText(client);
    formClientNameEdit->setText(name);
    formClientPhoneEdit->setText(phone);
    formAmountEdit->setText(QString(amount).remove(" TND").trimmed());

    QStringList parts = addr.split(", ");
    if (parts.size() >= 3) {
        int idxW = comboWilaya->findText(parts[0], Qt::MatchContains);
        if (idxW >= 0) comboWilaya->setCurrentIndex(idxW);
        int idxD = comboDelegation->findText(parts[1], Qt::MatchContains);
        if (idxD >= 0) comboDelegation->setCurrentIndex(idxD);
        int idxI = comboImada->findText(parts[2], Qt::MatchContains);
        if (idxI >= 0) comboImada->setCurrentIndex(idxI);
    }

    QDate d = QDate::fromString(date.left(10), "dd/MM/yyyy");
    if (d.isValid()) formDateEdit->setDate(d);

    auto *badgeWidget = ordersTable->cellWidget(row, 7);
    QString stText = "En cours";
    if (badgeWidget) {
        auto *lbl = badgeWidget->findChild<QLabel*>();
        if (lbl) {
            stText = lbl->text();
            formStatusCombo->setCurrentText(stText);
        }
    }

    QString paymentMode = "Carte bancaire";
    QString articleName = "Robe Élégance";
    currentSelectedPhone = phone;

    QSqlQuery q;
    q.prepare("SELECT MODE_PAIEMENT, NVL(ARTICLE, '') FROM COMMANDES WHERE ID_COMMANDE = :id");
    q.bindValue(":id", id);
    if (q.exec() && q.next()) {
        paymentMode = q.value(0).toString();
        QString art = q.value(1).toString().trimmed();
        if (!art.isEmpty()) articleName = art;
    }

    if (formArticleCombo) formArticleCombo->setCurrentText(articleName);
    if (formPaymentCombo) formPaymentCombo->setCurrentText(paymentMode);

    QString imgName = "prod_veste.png";
    if (articleName.contains("Bijou", Qt::CaseInsensitive))        imgName = "prod_bijoux.png";
    else if (articleName.contains("Lunette", Qt::CaseInsensitive)) imgName = "prod_lunette.png";
    else if (articleName.contains("Chaus", Qt::CaseInsensitive))   imgName = "prod_chaussure.png";
    else if (articleName.contains("Sac", Qt::CaseInsensitive))      imgName = "prod_sac.png";
    else if (articleName.contains("Veste", Qt::CaseInsensitive))   imgName = "prod_veste.png";

    QPixmap p(":/images/" + imgName);
    if (!p.isNull()) {
        detailProductImage->setPixmap(p.scaled(55, 55, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        if (formPreviewImage) formPreviewImage->setPixmap(p.scaled(80, 95, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }

    if (detailProdInfo) {
        detailProdInfo->setText(QString("<b style='font-size:12px; color:#0F172A;'>%1</b><br>"
                                        "<span style='color:#64748B; font-size:10.5px; line-height:16px;'>Taille : Standard &nbsp;|&nbsp; Quantité : 1</span>")
                                    .arg(articleName));
    }

    detailClient->setText(QString("👤  Client : <b>%1</b> <span style='color:#64748B;'>(%2)</span>").arg(name, client));
    detailPhone->setText(QString("📞  Tél : <b>+216 %1</b>").arg(currentSelectedPhone));
    detailDate->setText(QString("🕒  Date : <b>%1</b>").arg(date));
    detailStatus->setText(QString("●  Statut : <b style='color:#2563EB;'>%1</b>").arg(stText));
    detailAmount->setText(QString("💰  Montant : <b style='color:#0F172A;'>%1</b>").arg(amount));
    detailPayment->setText(QString("💳  Paiement : <b>%1</b>").arg(paymentMode.isEmpty() ? "Carte bancaire" : paymentMode));
    detailAddress->setText(QString("📍  Livraison : <b>%1</b>").arg(addr.isEmpty() ? "Tunis" : addr));

    QPair<double, double> geo = {36.8065, 10.1815};
    if (parts.size() >= 3) {
        geo = getCoordinatesFromAddress(parts[0], parts[1], parts[2]);
    }

    selectedClientLat = QString::number(geo.first, 'f', 6);
    selectedClientLon = QString::number(geo.second, 'f', 6);

    calculerTempsTrajet(geo.first, geo.second);
    sendMapCommand(QString("selectClient(%1,%2,'%3');").arg(selectedClientLat, selectedClientLon, id));
}

void MainWindow::selectOrder(int row)
{
    updateDetails(row);
}

void MainWindow::sendMapCommand(const QString &js)
{
    if(mapView) mapView->page()->runJavaScript(js);
}

void MainWindow::centerMap()
{
    sendMapCommand("centerWorkshop();");
}

void MainWindow::showSelectedRoute()
{
    sendMapCommand(QString("showRoute(%1,%2);").arg(selectedClientLat, selectedClientLon));
}

void MainWindow::initAIAgent()
{
    conversationHistory = QJsonArray();

    QJsonObject systemPrompt;
    systemPrompt["role"] = "system";
    systemPrompt["content"] =
        "Tu es 'StyleBot', l'assistant IA officiel de l'ERP Fashionova (confection textile & prêt-à-porter de luxe en Tunisie). "
        "Tu assistes le responsable d'atelier Mohamed Anoir. "
        "Informations sur l'interface Fashionova :\n"
        "- Pour modifier une commande : l'utilisateur sélectionne la ligne, modifie directement les champs puis clique sur '✏ Modifier' pour valider.\n"
        "- Pour ajouter : bouton '＋ Ajouter'. Pour réinitialiser : bouton '↺ Réinitialiser'. Pour supprimer : '🗑 Supprimer'.\n"
        "- Chaque commande calcule dynamiquement un délai de confection ('⏱ Livraison estimée').\n"
        "- L'atelier central est basé aux Berges du Lac, Tunis.\n"
        "Réponds toujours avec politesse, concision et en français.";
    conversationHistory.append(systemPrompt);
}

void MainWindow::onAiResponseReceived(QNetworkReply *reply)
{
    if (chatMessagesLayout->count() > 1) {
        QLayoutItem *lastItem = chatMessagesLayout->itemAt(chatMessagesLayout->count() - 2);
        if (lastItem && lastItem->widget()) {
            QLabel *lastLbl = lastItem->widget()->findChild<QLabel*>();
            if (lastLbl && lastLbl->text().contains("réfléchit")) {
                QWidget *w = lastItem->widget();
                chatMessagesLayout->removeWidget(w);
                w->deleteLater();
            }
        }
    }

    QByteArray responseBytes = reply->readAll();

    if (reply->error() == QNetworkReply::NoError) {
        QJsonDocument doc = QJsonDocument::fromJson(responseBytes);
        QJsonObject root = doc.object();

        if (root.contains("choices") && root["choices"].isArray()) {
            QJsonArray choices = root["choices"].toArray();
            if (!choices.isEmpty()) {
                QString botText = choices[0].toObject()["message"].toObject()["content"].toString();
                addChatMessage("Bot", botText, false);

                QJsonObject assistantMsg;
                assistantMsg["role"] = "assistant";
                assistantMsg["content"] = botText;
                conversationHistory.append(assistantMsg);
            }
        }
    } else {
        QString serverMsg = QString::fromUtf8(responseBytes);
        if (serverMsg.isEmpty()) {
            serverMsg = reply->errorString();
        }
        addChatMessage("Bot", QString::fromUtf8("⚠️ <b>Erreur :</b> ") + serverMsg, false);
    }

    reply->deleteLater();
}