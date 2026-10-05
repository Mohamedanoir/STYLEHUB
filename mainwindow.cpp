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
#include <QDialog>
#include <QApplication>
#include <QDateTime>
#include <QNetworkRequest>
#include <QScrollBar>

static const QString NAVY = "#082849";
static const QString BLUE = "#1B5FA7";
static const QString GOLD = "#D8A64B";

static QString cardStyle()
{
    return "QFrame { background:#FFFFFF; border:1px solid #E2E8F0; border-radius:8px; }";
}

// ---------------------------------------------------------------------------
// Widget pour afficher l'image de fond de la sidebar (200x900) sans rognage
// ---------------------------------------------------------------------------
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
            p.fillRect(rect(), QColor("#082849"));
        }
    }
};

// ---------------------------------------------------------------------------
// Générateur de QR Code vectoriel stylisé
// ---------------------------------------------------------------------------
static QPixmap generateQrCodePixmap(const QString &text, int size = 110)
{
    QPixmap pix(size, size);
    pix.fill(Qt::white);
    QPainter p(&pix);
    p.setRenderHint(QPainter::Antialiasing, false);

    const int modules = 21; // Grille standard de QR code (Version 1)
    const double step = double(size) / modules;

    quint32 hash = qHash(text);
    p.setPen(Qt::NoPen);
    p.setBrush(QColor("#082849"));

    auto drawFinder = [&](int startX, int startY) {
        p.drawRect(QRectF(startX * step, startY * step, 7 * step, 7 * step));
        p.setBrush(Qt::white);
        p.drawRect(QRectF((startX + 1) * step, (startY + 1) * step, 5 * step, 5 * step));
        p.setBrush(QColor("#082849"));
        p.drawRect(QRectF((startX + 2) * step, (startY + 2) * step, 3 * step, 3 * step));
    };

    drawFinder(0, 0);
    drawFinder(modules - 7, 0);
    drawFinder(0, modules - 7);

    for (int y = 0; y < modules; ++y) {
        for (int x = 0; x < modules; ++x) {
            if ((x < 7 && y < 7) || (x >= modules - 7 && y < 7) || (x < 7 && y >= modules - 7))
                continue;

            hash = hash * 1103515245 + 12345;
            if ((hash & 1) == 0) {
                p.drawRect(QRectF(x * step, y * step, step, step));
            }
        }
    }

    p.end();
    return pix;
}

// ---------------------------------------------------------------------------
// Icônes vectorielles dynamiques de la sidebar (Bicolore On/Off)
// ---------------------------------------------------------------------------
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
        case 0: { // Clients
            p.drawEllipse(4, 2, 8, 8);
            QPainterPath b1; b1.moveTo(1, 20); b1.arcTo(1, 10, 14, 12, 0, 180); b1.closeSubpath();
            p.drawPath(b1);
            p.drawEllipse(13, 4, 6, 6);
            QPainterPath b2; b2.moveTo(11, 20); b2.arcTo(11, 11, 11, 10, 0, 180); b2.closeSubpath();
            p.drawPath(b2);
            break;
        }
        case 1: { // Employés
            p.drawEllipse(7, 2, 10, 10);
            QPainterPath b; b.moveTo(2, 21); b.arcTo(2, 12, 20, 14, 0, 180); b.closeSubpath();
            p.drawPath(b);
            break;
        }
        case 2: { // Commandes
            p.setPen(QPen(color, 1.8));
            p.setBrush(Qt::NoBrush);
            p.drawRoundedRect(4, 4, 16, 18, 2, 2);
            p.setBrush(color);
            p.setPen(Qt::NoPen);
            p.drawRoundedRect(8, 2, 8, 4, 1.5, 1.5);
            p.setPen(QPen(color, 1.6));
            p.drawLine(7, 10, 17, 10);
            p.drawLine(7, 14, 17, 14);
            p.drawLine(7, 18, 14, 18);
            break;
        }
        case 3: { // Maquettes
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
        case 4: { // Machines
            p.drawEllipse(6, 6, 12, 12);
            for (int i = 0; i < 8; ++i) {
                p.save();
                p.translate(12, 12);
                p.rotate(i * 45);
                p.drawRect(-2, -10, 4, 3);
                p.restore();
            }
            p.setCompositionMode(QPainter::CompositionMode_Clear);
            p.drawEllipse(9, 9, 6, 6);
            break;
        }
        case 5: { // Articles
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
    icon.addPixmap(drawPix(type, QColor("#FFFFFF")), QIcon::Normal, QIcon::Off);
    icon.addPixmap(drawPix(type, QColor("#08213B")), QIcon::Normal, QIcon::On);
    return icon;
}

// ---------------------------------------------------------------------------
// Icônes monochromes pour les boutons d'action
// ---------------------------------------------------------------------------
enum ActionIconType { IconEye, IconTrash, IconCar, IconLocation, IconFile, IconScan };

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
        p.setBrush(QColor("#10B981"));
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
        p.drawRoundedRect(3, 2, size - 6, size - 4, 2, 2);
        p.drawLine(6, 6, size - 6, 6);
        p.drawLine(6, 10, size - 6, 10);
        p.drawLine(6, 14, size - 9, 14);
        break;
    }
    case IconScan: {
        p.setPen(QPen(color, 1.8));
        p.drawLine(3, 7, 3, 3); p.drawLine(3, 3, 7, 3);
        p.drawLine(size - 3, 7, size - 3, 3); p.drawLine(size - 3, 3, size - 7, 3);
        p.drawLine(3, size - 7, 3, size - 3); p.drawLine(3, size - 3, 7, size - 3);
        p.drawLine(size - 3, size - 7, size - 3, size - 3); p.drawLine(size - 3, size - 3, size - 7, size - 3);
        p.setPen(QPen(QColor("#EF4444"), 1.8));
        p.drawLine(3, size / 2, size - 3, size / 2);
        break;
    }
    }
    p.end();
    return QIcon(pix);
}

// ---------------------------------------------------------------------------
// KPIs
// ---------------------------------------------------------------------------
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
        p.drawRoundedRect(7, 10, 20, 15, 2, 2);
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
        p.drawRoundedRect(5, 9, 14, 13, 1.5, 1.5);
        QPainterPath cab;
        cab.moveTo(19, 13); cab.lineTo(25, 13); cab.lineTo(28, 17); cab.lineTo(28, 22); cab.lineTo(19, 22);
        cab.closeSubpath();
        p.drawPath(cab);
        p.setBrush(QColor("#FFFFFF"));
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
            p.drawRoundedRect(-2.2, -10.5, 4.4, 3.5, 1, 1);
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
        QFont f("Segoe UI", 9, QFont::Bold);
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

// ---------------------------------------------------------------------------
// Constructeur Principal
// ---------------------------------------------------------------------------
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), chatFloatingBtn(nullptr), chatWindow(nullptr), sortAscending(true), currentRowSelected(0)
{
    resize(1440, 880);
    setMinimumSize(1100, 680);
    setWindowTitle("FASHIONOVA — Gestion des Commandes");

    // Initialisation Réseau et Agent IA
    networkManager = new QNetworkAccessManager(this);
    connect(networkManager, &QNetworkAccessManager::finished, this, &MainWindow::onAiResponseReceived);
    initAIAgent();

    setStyleSheet(QString(R"(
        QWidget { font-family: "Segoe UI", sans-serif; color: #1E293B; font-size: 11px; }
        QMainWindow { background-color: #F8FAFC; }
        QScrollArea { background-color: #F8FAFC; border: none; }
        QLineEdit, QComboBox {
            background-color: #FFFFFF; border: 1px solid #CBD5E1; border-radius: 5px;
            padding: 2px 6px; color: #1E293B; font-size: 11px;
        }
        QLineEdit:focus, QComboBox:focus { border: 1px solid #3B82F6; }
        QTableWidget {
            background-color: #FFFFFF; border: none; gridline-color: #F1F5F9;
            selection-background-color: #F0F7FF; selection-color: #1E293B;
        }
        QHeaderView::section {
            background-color: #F8FAFC; color: #64748B; padding: 4px 2px;
            border: none; border-bottom: 1px solid #E2E8F0;
            font-weight: 600; font-size: 10px;
        }
        QScrollBar:vertical { width: 6px; background: transparent; }
        QScrollBar::handle:vertical { background: #CBD5E1; border-radius: 3px; }
    )"));

    auto *root = new QWidget;
    root->setStyleSheet("background-color: #F8FAFC;");
    auto *rootLayout = new QHBoxLayout(root);
    rootLayout->setContentsMargins(0, 0, 0, 0);
    rootLayout->setSpacing(0);

    // Sidebar fixe à 200px
    rootLayout->addWidget(makeSidebar(), 0);

    // Zone défilable pour Commandes
    auto *scrollArea = new QScrollArea;
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    auto *scrollContent = new QWidget;
    scrollContent->setStyleSheet("background-color: #F8FAFC;");
    auto *main = new QVBoxLayout(scrollContent);
    main->setContentsMargins(10, 6, 10, 8);
    main->setSpacing(6);

    main->addWidget(makeHeader(), 0);
    main->addWidget(makeKpis(), 0);

    auto *middleWidget = new QWidget;
    middleWidget->setStyleSheet("background: transparent;");
    auto *middleHBox = new QHBoxLayout(middleWidget);
    middleHBox->setContentsMargins(0, 0, 0, 0);
    middleHBox->setSpacing(8);

    auto *leftCol = new QWidget;
    leftCol->setStyleSheet("background: transparent;");
    auto *leftVBox = new QVBoxLayout(leftCol);
    leftVBox->setContentsMargins(0, 0, 0, 0);
    leftVBox->setSpacing(6);
    leftVBox->addWidget(makeOrdersPanel());
    leftVBox->addWidget(makeMapPanel());

    middleHBox->addWidget(leftCol, 1);

    auto *detailsPanel = makeDetailsPanel();
    detailsPanel->setFixedWidth(295);
    middleHBox->addWidget(detailsPanel, 0);

    main->addWidget(middleWidget, 0);
    main->addWidget(makeBottomPanel(), 0);

    scrollArea->setWidget(scrollContent);

    // Stack multi-pages
    pageStack = new QStackedWidget;
    pageStack->setStyleSheet("background-color: #F8FAFC; border: none;");

    pageStack->addWidget(createEmptyPage("Clients"));     // Index 0
    pageStack->addWidget(createEmptyPage("Employés"));    // Index 1
    pageStack->addWidget(scrollArea);                     // Index 2 : Commandes
    pageStack->addWidget(createEmptyPage("Maquettes"));   // Index 3
    pageStack->addWidget(createEmptyPage("Machines"));    // Index 4
    pageStack->addWidget(createEmptyPage("Articles"));    // Index 5

    pageStack->setCurrentIndex(2);

    rootLayout->addWidget(pageStack, 1);
    setCentralWidget(root);

    setupOrders();
    if (ordersTable->rowCount() > 0) {
        ordersTable->selectRow(0);
        updateDetails(0);
    }

    setupFloatingChatbot();
}

// ---------------------------------------------------------------------------
// Page épurée pour les autres modules
// ---------------------------------------------------------------------------
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

// ---------------------------------------------------------------------------
// Sidebar
// ---------------------------------------------------------------------------
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
                color: #08213B;
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

// ---------------------------------------------------------------------------
// En-tête
// ---------------------------------------------------------------------------
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

// ---------------------------------------------------------------------------
// KPIs du haut
// ---------------------------------------------------------------------------
QWidget* MainWindow::makeKpis()
{
    auto *w = new QWidget;
    w->setFixedWidth(1310);
    auto *g = new QGridLayout(w);
    g->setContentsMargins(0, 0, 0, 0);
    g->setSpacing(8);

    struct KpiConfig {
        QString title, value, trend, iconColor, bgCircleColor;
        bool isDown;
    };

    const QList<KpiConfig> kpis = {
        {"Total commandes", "124",       "⬆  +12% ce mois", "#2563EB", "#EBF2FE", false},
        {"En attente",      "18",        "⬇  +9% ce mois",  "#B45309", "#FEF3C7", false},
        {"Expédiées",       "72",        "⬆  +16% ce mois", "#10B981", "#E6F8F0", false},
        {"En atelier",      "14",        "⬇  -6% ce mois",   "#EF4444", "#FEE8E8", true},
        {"Montant total",   "45 680 DT", "⬆  +22% ce mois", "#1D4ED8", "#EBF2FE", false}
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
        iconLabel->setPixmap(makeKpiIcon(i, QColor(k.iconColor)));
        cardLayout->addWidget(iconLabel);

        auto *infoLayout = new QVBoxLayout;
        infoLayout->setContentsMargins(0, 0, 0, 0);
        infoLayout->setSpacing(1);

        auto *titleLbl = new QLabel(k.title);
        titleLbl->setStyleSheet("color: #475569; font-size: 10.5px; font-weight: 600;");

        auto *valLbl = new QLabel(k.value);
        valLbl->setStyleSheet("color: #0F172A; font-size: 15px; font-weight: bold;");

        auto *trendLbl = new QLabel(k.trend);
        QString trendCol = (i == 1) ? "#D97706" : ((i == 3) ? "#EF4444" : "#10B981");
        trendLbl->setStyleSheet(QString("color: %1; font-size: 9px; font-weight: 600;").arg(trendCol));

        infoLayout->addWidget(titleLbl);
        infoLayout->addWidget(valLbl);
        infoLayout->addWidget(trendLbl);
        cardLayout->addLayout(infoLayout, 1);

        auto *sparkLabel = new QLabel;
        sparkLabel->setFixedSize(60, 36);
        sparkLabel->setPixmap(makeSparklineWithGradient(QColor(k.iconColor), k.isDown, 60, 36));
        cardLayout->addWidget(sparkLabel, 0, Qt::AlignBottom);

        g->addWidget(card, 0, i);
    }
    return w;
}

// ---------------------------------------------------------------------------
// Tableau des commandes avec bouton Scanner Express
// ---------------------------------------------------------------------------
QWidget* MainWindow::makeOrdersPanel()
{
    auto *f = new QFrame;
    f->setStyleSheet(cardStyle());
    f->setFixedWidth(1010);
    auto *v = new QVBoxLayout(f);
    v->setContentsMargins(8, 5, 8, 5);
    v->setSpacing(4);

    auto *title = new QHBoxLayout;
    title->addWidget(new QLabel("<b style='font-size:12px; color:#0F172A;'>📋  Liste des commandes</b>"));
    title->addStretch();

    auto *btnScanModal = new QPushButton("  Scanner Express");
    btnScanModal->setIcon(makeButtonIcon(IconScan, QColor("#FFFFFF")));
    btnScanModal->setIconSize(QSize(15, 15));
    btnScanModal->setCursor(Qt::PointingHandCursor);
    btnScanModal->setFixedHeight(24);
    btnScanModal->setStyleSheet("QPushButton{background:#D97706; color:white; border:none; border-radius:4px; padding:2px 10px; font-weight:bold; font-size:10.5px;} QPushButton:hover{background:#B45309;}");
    connect(btnScanModal, &QPushButton::clicked, this, &MainWindow::openBarcodeScannerModal);
    title->addWidget(btnScanModal);

    auto *localSearch = new QLineEdit;
    localSearch->setPlaceholderText("🔍  Filtrer...");
    localSearch->setFixedWidth(110);
    connect(localSearch, &QLineEdit::textChanged, this, [this, localSearch](){
        for(int r = 0; r < ordersTable->rowCount(); ++r) {
            bool ok = localSearch->text().isEmpty();
            for(int c = 1; c <= 4 && !ok; ++c) {
                if(ordersTable->item(r,c) && ordersTable->item(r,c)->text().contains(localSearch->text(), Qt::CaseInsensitive))
                    ok = true;
            }
            ordersTable->setRowHidden(r, !ok);
        }
    });
    title->addWidget(localSearch);

    statusCombo = new QComboBox;
    statusCombo->addItems({"Statut : Tous", "Livrée", "En cours", "Prête"});
    title->addWidget(statusCombo);

    periodCombo = new QComboBox;
    periodCombo->addItems({"Période : Tous", "Aujourd'hui", "Ce mois"});
    title->addWidget(periodCombo);
    v->addLayout(title);

    ordersTable = new QTableWidget(0, 8);
    ordersTable->setIconSize(QSize(28, 28));
    ordersTable->setHorizontalHeaderLabels({" ", "ID Cmd", "Client", "Nom", "Date", "Statut", "Montant", "Actions"});
    ordersTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ordersTable->setSelectionMode(QAbstractItemView::SingleSelection);
    ordersTable->verticalHeader()->setVisible(false);
    ordersTable->setFixedHeight(190);

    ordersTable->setColumnWidth(0, 24);
    ordersTable->setColumnWidth(1, 95);
    ordersTable->setColumnWidth(2, 60);
    ordersTable->setColumnWidth(3, 110);
    ordersTable->setColumnWidth(4, 105);
    ordersTable->setColumnWidth(5, 85);
    ordersTable->setColumnWidth(6, 75);
    ordersTable->setColumnWidth(7, 65);
    ordersTable->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch);

    connect(ordersTable, &QTableWidget::cellClicked, this, &MainWindow::selectOrder);
    v->addWidget(ordersTable);

    auto *pageBar = new QHBoxLayout;
    auto *lblPage = new QLabel("1 à 6 sur 124 commandes");
    lblPage->setStyleSheet("color:#64748B; font-size:9.5px;");
    pageBar->addWidget(lblPage);
    pageBar->addStretch();

    const QStringList pages = {"<", "1", "2", "3", "...", ">"};
    for(const auto &pg : pages) {
        auto *pb = new QPushButton(pg);
        pb->setFixedSize(18, 18);
        if(pg == "1") pb->setStyleSheet("QPushButton{background:#082849; color:white; border-radius:3px; font-weight:bold; font-size:9px;}");
        else pb->setStyleSheet("QPushButton{background:transparent; color:#64748B; border:none; font-size:9px;}");
        pageBar->addWidget(pb);
    }
    v->addLayout(pageBar);

    auto *buttons = new QHBoxLayout;
    auto *btnAdd = new QPushButton("＋  Ajouter");
    btnAdd->setFixedHeight(26);
    btnAdd->setStyleSheet("QPushButton{background:#0A2540; color:white; border:none; border-radius:4px; padding:0 10px; font-weight:bold; font-size:11px;}");
    buttons->addWidget(btnAdd);

    auto *btnEdit = new QPushButton("✏  Modifier");
    btnEdit->setFixedHeight(26);
    btnEdit->setStyleSheet("QPushButton{background:#6E7D93; color:white; border:none; border-radius:4px; padding:0 10px; font-weight:600; font-size:11px;}");
    buttons->addWidget(btnEdit);

    auto *btnDel = new QPushButton("  Supprimer");
    btnDel->setIcon(makeButtonIcon(IconTrash, QColor("#3F3427")));
    btnDel->setIconSize(QSize(15, 15));
    btnDel->setFixedHeight(26);
    btnDel->setStyleSheet("QPushButton{background:#EFE5D5; color:#3F3427; border:none; border-radius:4px; padding:0 10px; font-weight:600; font-size:11px;} QPushButton:hover{background:#E5DAC6;}");
    buttons->addWidget(btnDel);

    auto *btnShow = new QPushButton("  Afficher");
    btnShow->setIcon(makeButtonIcon(IconEye, QColor("#0A2540")));
    btnShow->setIconSize(QSize(16, 16));
    btnShow->setFixedHeight(26);
    btnShow->setCursor(Qt::PointingHandCursor);
    btnShow->setStyleSheet("QPushButton{background:#CCD7E4; color:#0A2540; border:none; border-radius:4px; padding:0 12px; font-weight:bold; font-size:11px;} QPushButton:hover{background:#B8C8DB;}");
    buttons->addWidget(btnShow);

    auto *btnSort = new QPushButton("⇅  Trier");
    btnSort->setFixedHeight(26);
    btnSort->setStyleSheet("QPushButton{background:#F1F5F9; color:#0F172A; border:1px solid #CBD5E1; border-radius:4px; padding:0 10px; font-weight:600; font-size:11px;}");
    connect(btnSort, &QPushButton::clicked, this, &MainWindow::toggleSortOrders);
    buttons->addWidget(btnSort);

    buttons->addStretch();
    v->addLayout(buttons);

    return f;
}

void MainWindow::toggleSortOrders()
{
    sortAscending = !sortAscending;
    ordersTable->sortItems(1, sortAscending ? Qt::AscendingOrder : Qt::DescendingOrder);
}

// ---------------------------------------------------------------------------
// Panneau Détails avec bouton Facture PDF & QR Code
// ---------------------------------------------------------------------------
QWidget* MainWindow::makeDetailsPanel()
{
    auto *f = new QFrame;
    f->setStyleSheet(cardStyle());
    auto *v = new QVBoxLayout(f);
    v->setContentsMargins(10, 8, 10, 8);
    v->setSpacing(6);

    auto *headerBox = new QHBoxLayout;
    headerBox->addWidget(new QLabel("<b style='font-size:13px; color:#0F172A;'>⚙️  Détails</b>"));
    headerBox->addStretch();
    selectedOrderLabel = new QLabel("CMD-003");
    selectedOrderLabel->setStyleSheet("background:#EFF6FF; color:#1D4ED8; border-radius:4px; padding:2px 7px; font-weight:bold; font-size:10.5px;");
    headerBox->addWidget(selectedOrderLabel);
    v->addLayout(headerBox);

    auto *prodCard = new QFrame;
    prodCard->setStyleSheet("QFrame{background:#F8FAFC; border:1px solid #E2E8F0; border-radius:6px;}");
    auto *prodLayout = new QHBoxLayout(prodCard);
    prodLayout->setContentsMargins(6, 6, 6, 6);
    prodLayout->setSpacing(10);

    detailProductImage = new QLabel;
    detailProductImage->setFixedSize(60, 60);
    detailProductImage->setScaledContents(true);
    detailProductImage->setAlignment(Qt::AlignCenter);
    detailProductImage->setStyleSheet("background:white; border:1px solid #CBD5E1; border-radius:5px; padding:2px;");
    prodLayout->addWidget(detailProductImage);

    auto *prodInfo = new QLabel("<b style='font-size:12px; color:#0F172A;'>Robe Élégance</b><br><span style='color:#64748B; font-size:10.5px; line-height:16px;'>Taille : M &nbsp;|&nbsp; Couleur : Beige<br>Quantité : 1</span>");
    prodLayout->addWidget(prodInfo, 1);
    v->addWidget(prodCard);

    detailClient  = new QLabel;
    detailDate    = new QLabel;
    detailStatus  = new QLabel;
    detailAmount  = new QLabel;
    auto *detailPayment = new QLabel("💳  Paiement : <b>Carte bancaire</b>");
    detailAddress = new QLabel;

    for(auto *x : {detailClient, detailDate, detailStatus, detailAmount, detailPayment, detailAddress}) {
        x->setStyleSheet("font-size: 11.5px; color: #1E293B; padding: 2px 0px;");
        x->setWordWrap(true);
        v->addWidget(x);
    }

    auto *actionBtnsLayout = new QHBoxLayout;
    actionBtnsLayout->setSpacing(6);

    auto *detailsBtn = new QPushButton("  Détails");
    detailsBtn->setIcon(makeButtonIcon(IconEye, QColor("#FFFFFF")));
    detailsBtn->setIconSize(QSize(15, 15));
    detailsBtn->setCursor(Qt::PointingHandCursor);
    detailsBtn->setFixedHeight(26);
    detailsBtn->setStyleSheet(QString("QPushButton{background:%1; color:white; border:none; border-radius:5px; font-weight:600; font-size:10.5px;} QPushButton:hover{background:#1B5FA7;}").arg(NAVY));
    actionBtnsLayout->addWidget(detailsBtn, 1);

    auto *pdfBtn = new QPushButton("  Facture PDF");
    pdfBtn->setIcon(makeButtonIcon(IconFile, QColor("#08213B")));
    pdfBtn->setIconSize(QSize(15, 15));
    pdfBtn->setCursor(Qt::PointingHandCursor);
    pdfBtn->setFixedHeight(26);
    pdfBtn->setStyleSheet("QPushButton{background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #F3D082, stop:1 #C89538); color:#08213B; border:none; border-radius:5px; font-weight:bold; font-size:10.5px;} QPushButton:hover{background:#FFE4A3;}");
    connect(pdfBtn, &QPushButton::clicked, this, &MainWindow::generateInvoicePdf);
    actionBtnsLayout->addWidget(pdfBtn, 1);

    v->addLayout(actionBtnsLayout);

    auto *est = new QFrame;
    est->setStyleSheet("QFrame{background:#FFFDF7; border:1px solid #F3E4C8; border-radius:6px;}");
    auto *eg = new QGridLayout(est);
    eg->setContentsMargins(8, 6, 8, 6);
    eg->setSpacing(4);

    eg->addWidget(new QLabel("<b style='color:#B45309; font-size:10.5px;'>◎  Estimation livraison</b>"), 0, 0, 1, 2);

    distanceLabel = new QLabel;
    costLabel     = new QLabel;
    delayLabel    = new QLabel;

    eg->addWidget(new QLabel("<span style='color:#78350F; font-size:10px;'>Distance atelier → client</span>"), 1, 0);
    eg->addWidget(distanceLabel, 1, 1, Qt::AlignRight);

    eg->addWidget(new QLabel("<span style='color:#78350F; font-size:10px;'>Frais de port estimés</span>"), 2, 0);
    eg->addWidget(costLabel, 2, 1, Qt::AlignRight);

    eg->addWidget(new QLabel("<span style='color:#78350F; font-size:10px;'>Délai de livraison</span>"), 3, 0);
    eg->addWidget(delayLabel, 3, 1, Qt::AlignRight);

    auto *route = new QPushButton("📍  Voir l'itinéraire sur la carte");
    route->setCursor(Qt::PointingHandCursor);
    route->setFixedHeight(22);
    route->setStyleSheet("QPushButton{background:white; border:1px solid #CBD5E1; border-radius:4px; font-size:9.5px; font-weight:600; color:#1E293B;} QPushButton:hover{background:#F8FAFC;}");
    connect(route, &QPushButton::clicked, this, &MainWindow::showSelectedRoute);
    eg->addWidget(route, 4, 0, 1, 2);
    v->addWidget(est);

    auto *follow = new QFrame;
    follow->setStyleSheet("QFrame{background:#FFFFFF; border:1px solid #E2E8F0; border-radius:6px;}");
    auto *fg = new QVBoxLayout(follow);
    fg->setContentsMargins(8, 5, 8, 5);
    fg->setSpacing(3);
    fg->addWidget(new QLabel("<b style='color:#0F172A; font-size:10px;'>◉  Suivi</b>"));

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

    v->addWidget(follow);
    v->addStretch();
    return f;
}

// ---------------------------------------------------------------------------
// FACTURE & BON DE LIVRAISON PDF AVEC QR CODE
// ---------------------------------------------------------------------------
void MainWindow::generateInvoicePdf()
{
    int r = currentRowSelected;
    if (r < 0 || r >= ordersTable->rowCount()) return;

    QString id     = ordersTable->item(r, 1)->text();
    QString client = ordersTable->item(r, 2)->text();
    QString name   = ordersTable->item(r, 3)->text();
    QString date   = ordersTable->item(r, 4)->text();
    QString amount = ordersTable->item(r, 6)->text();
    QString addr   = detailAddress ? detailAddress->text().remove("📍  Livraison : <b>").remove("</b>") : "Tunis, Tunisie";

    QString defaultName = QString("Facture_Fashionova_%1.pdf").arg(id);
    QString fileName = QFileDialog::getSaveFileName(this, "Enregistrer la Facture PDF", defaultName, "Documents PDF (*.pdf)");
    if (fileName.isEmpty()) return;

    QString qrPath = QDir::tempPath() + QString("/qr_%1.png").arg(id);
    QString trackingData = QString("https://fashionova.luxury/track?order=%1&client=%2").arg(id, client);
    QPixmap qrPix = generateQrCodePixmap(trackingData, 140);
    qrPix.save(qrPath, "PNG");

    QString html = QString(R"(
        <!DOCTYPE html>
        <html>
        <head>
            <meta charset="utf-8">
            <style>
                body { font-family: 'Segoe UI', Arial, sans-serif; color: #1E293B; padding: 25px; margin: 0; }
                .header-table { width: 100%; border-bottom: 2px solid #D8A64B; padding-bottom: 15px; margin-bottom: 25px; }
                .brand { font-size: 26px; font-weight: bold; color: #082849; letter-spacing: 2px; }
                .sub-brand { font-size: 11px; color: #B45309; text-transform: uppercase; letter-spacing: 1.5px; }
                .title { font-size: 20px; font-weight: bold; color: #082849; text-align: right; }
                .details-table { width: 100%; margin-bottom: 30px; }
                .box { background: #F8FAFC; border: 1px solid #E2E8F0; border-radius: 6px; padding: 12px; font-size: 12px; }
                .items-table { width: 100%; border-collapse: collapse; margin-bottom: 30px; }
                .items-table th { background: #082849; color: #FFFFFF; font-size: 11px; padding: 10px; text-align: left; }
                .items-table td { padding: 12px 10px; border-bottom: 1px solid #E2E8F0; font-size: 12px; }
                .total-box { float: right; width: 40%; text-align: right; margin-bottom: 35px; }
                .total-amount { font-size: 18px; font-weight: bold; color: #082849; }
                .qr-container { border-top: 1px dashed #CBD5E1; padding-top: 20px; margin-top: 40px; }
            </style>
        </head>
        <body>
            <table class="header-table">
                <tr>
                    <td>
                        <div class="brand">FASHIONOVA</div>
                        <div class="sub-brand">Haute Couture & Prêt-à-Porter</div>
                    </td>
                    <td class="title">FACTURE OFFICIELLE<br><span style="font-size: 13px; color: #64748B;">N° %1</span></td>
                </tr>
            </table>

            <table class="details-table">
                <tr>
                    <td width="48%" valign="top" class="box">
                        <b style="color: #082849;">Émetteur :</b><br>
                        <b>Fashionova Prestige SARL</b><br>
                        Atelier Central & Logistique<br>
                        Les Berges du Lac, Tunis 1053<br>
                        Tél : +216 71 000 000
                    </td>
                    <td width="4%"></td>
                    <td width="48%" valign="top" class="box">
                        <b style="color: #082849;">Destinataire :</b><br>
                        <b>%2</b> (Code : %3)<br>
                        Adresse : %4<br>
                        Date d'émission : %5<br>
                        Mode de règlement : Carte bancaire
                    </td>
                </tr>
            </table>

            <table class="items-table">
                <thead>
                    <tr>
                        <th>Désignation Article</th>
                        <th>Quantité</th>
                        <th>Prix Unitaire HT</th>
                        <th>TVA (19%)</th>
                        <th>Total TTC</th>
                    </tr>
                </thead>
                <tbody>
                    <tr>
                        <td><b>Confection Sur-Mesure Fashionova</b><br><span style="color:#64748B; font-size:10px;">Ref: %1 - Finition Atelier</span></td>
                        <td>1</td>
                        <td>%6 TND</td>
                        <td>19%</td>
                        <td><b>%7</b></td>
                    </tr>
                </tbody>
            </table>

            <div class="total-box">
                <table width="100%">
                    <tr><td style="color:#64748B;">Total Net HT :</td><td><b>%6 TND</b></td></tr>
                    <tr><td style="color:#64748B;">TVA (19%) :</td><td><b>%8 TND</b></td></tr>
                    <tr><td style="color:#64748B;">Frais de livraison :</td><td><b>Offerts</b></td></tr>
                    <tr><td colspan="2"><hr style="border: 0.5px solid #E2E8F0;"></td></tr>
                    <tr><td><b class="total-amount">Total TTC :</b></td><td><b class="total-amount">%7</b></td></tr>
                </table>
            </div>

            <div style="clear: both;"></div>

            <table class="qr-container" width="100%">
                <tr>
                    <td width="120" align="center">
                        <img src="%9" width="105" height="105" />
                    </td>
                    <td style="font-size: 11px; color: #475569; padding-left: 15px;">
                        <b style="color: #082849; font-size: 12px;">Passeport Numérique & Suivi Express :</b><br>
                        Scannez ce QR Code avec un smartphone pour vérifier l'authenticité de votre pièce de confection et suivre la géolocalisation de votre livreur en temps réel.
                    </td>
                </tr>
            </table>
        </body>
        </html>
    )").arg(id, name, client, addr, date)
                       .arg(QString::number(amount.split(" ").first().replace(",", ".").toDouble() / 1.19, 'f', 2))
                       .arg(amount)
                       .arg(QString::number(amount.split(" ").first().replace(",", ".").toDouble() - (amount.split(" ").first().replace(",", ".").toDouble() / 1.19), 'f', 2))
                       .arg(qrPath);

    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(fileName);
    printer.setPageSize(QPageSize(QPageSize::A4));
    printer.setPageMargins(QMarginsF(12, 12, 12, 12), QPageLayout::Millimeter);

    QTextDocument doc;
    doc.setHtml(html);
    doc.print(&printer);

    QMessageBox::information(this, "Facture Générée",
                             QString("La facture officielle avec QR Code pour la commande <b>%1</b> a été enregistrée avec succès !").arg(id));
}

// ---------------------------------------------------------------------------
// SCANNER / TERMINAL EXPRESS DE PRÉPARATION D'EXPÉDITION
// ---------------------------------------------------------------------------
void MainWindow::openBarcodeScannerModal()
{
    QDialog dlg(this);
    dlg.setWindowTitle("Terminal d'Atelier — Scanner Express");
    dlg.setFixedSize(400, 310);
    dlg.setStyleSheet("background-color: #FFFFFF; font-family: 'Segoe UI';");

    auto *vl = new QVBoxLayout(&dlg);
    vl->setContentsMargins(20, 20, 20, 20);
    vl->setSpacing(12);

    auto *headLbl = new QLabel("<b style='font-size:14px; color:#082849;'>📦 Scanner Express d'Expédition</b><br><span style='color:#64748B; font-size:10.5px;'>Entrez ou scannez le code-barres / QR Code du colis pour validation immédiate.</span>");
    headLbl->setWordWrap(true);
    vl->addWidget(headLbl);

    auto *laserBox = new QFrame;
    laserBox->setFixedHeight(90);
    laserBox->setStyleSheet("background: #08213B; border-radius: 8px; border: 2px solid #D8A64B;");
    auto *lblLaser = new QLabel("<span style='color:#EF4444; font-size:12px; font-weight:bold;'>━━━ FAISCEAU LASER ACTIF ━━━</span><br><span style='color:#CBD5E1; font-size:10px;'>Prêt pour scan CMD-xxx</span>", laserBox);
    lblLaser->setAlignment(Qt::AlignCenter);
    auto *laserLayout = new QVBoxLayout(laserBox);
    laserLayout->addWidget(lblLaser);
    vl->addWidget(laserBox);

    auto *editScan = new QLineEdit;
    editScan->setPlaceholderText("Exemple : CMD-003 puis Entrée...");
    editScan->setFixedHeight(36);
    editScan->setStyleSheet("QLineEdit { background:#F8FAFC; border:2px solid #CBD5E1; border-radius:6px; padding:0 12px; font-size:13px; font-weight:bold; color:#082849; } QLineEdit:focus{ border-color:#2563EB; }");
    vl->addWidget(editScan);

    auto *statusFeedback = new QLabel("");
    statusFeedback->setAlignment(Qt::AlignCenter);
    vl->addWidget(statusFeedback);

    auto *btnValidate = new QPushButton("Valider le Colis");
    btnValidate->setFixedHeight(34);
    btnValidate->setCursor(Qt::PointingHandCursor);
    btnValidate->setStyleSheet("QPushButton{ background:#10B981; color:white; border:none; border-radius:6px; font-weight:bold; font-size:12px; } QPushButton:hover{ background:#059669; }");
    vl->addWidget(btnValidate);

    auto processScan = [this, editScan, statusFeedback, &dlg]() {
        QString code = editScan->text().trimmed().toUpper();
        if (code.isEmpty()) return;

        bool found = false;
        for (int r = 0; r < ordersTable->rowCount(); ++r) {
            if (ordersTable->item(r, 1) && ordersTable->item(r, 1)->text().toUpper() == code) {
                found = true;
                QApplication::beep();

                auto *badgeWidget = ordersTable->cellWidget(r, 5);
                if (badgeWidget) {
                    auto *badge = badgeWidget->findChild<QLabel*>();
                    if (badge) {
                        badge->setText("Expédiée");
                        badge->setStyleSheet("background:#ECFDF5; color:#059669; border-radius:6px; padding:1px 6px; font-weight:bold; font-size:8.5px;");
                    }
                }

                ordersTable->selectRow(r);
                updateDetails(r);

                statusFeedback->setStyleSheet("color: #059669; font-weight: bold; font-size: 11px;");
                statusFeedback->setText(QString("✅ %1 validée et marquée comme EXPÉDIÉE !").arg(code));
                editScan->clear();
                break;
            }
        }

        if (!found) {
            statusFeedback->setStyleSheet("color: #DC2626; font-weight: bold; font-size: 11px;");
            statusFeedback->setText("❌ Commande introuvable dans la base !");
        }
    };

    connect(btnValidate, &QPushButton::clicked, &dlg, processScan);
    connect(editScan, &QLineEdit::returnPressed, &dlg, processScan);

    editScan->setFocus();
    dlg.exec();
}

// ---------------------------------------------------------------------------
// Carte WebEngine
// ---------------------------------------------------------------------------
QWidget* MainWindow::makeMapPanel()
{
    auto *f = new QFrame;
    f->setStyleSheet(cardStyle());
    f->setFixedWidth(1010);
    auto *v = new QVBoxLayout(f);
    v->setContentsMargins(8, 5, 8, 5);
    v->setSpacing(4);

    auto *title = new QHBoxLayout;
    title->addWidget(new QLabel("<b style='font-size:12px; color:#0F172A;'>📍  Carte des livraisons</b>"));
    title->addStretch();

    auto *btnRoute = new QPushButton("  Itinéraire");
    btnRoute->setIcon(makeButtonIcon(IconCar, QColor("#FFFFFF")));
    btnRoute->setIconSize(QSize(15, 15));
    btnRoute->setCursor(Qt::PointingHandCursor);
    btnRoute->setStyleSheet("QPushButton{background:#10B981; color:white; border:none; border-radius:4px; padding:2px 10px; font-weight:600; font-size:10.5px;} QPushButton:hover{background:#059669;}");
    connect(btnRoute, &QPushButton::clicked, this, &MainWindow::showSelectedRoute);
    title->addWidget(btnRoute);

    auto *loc = new QPushButton("  Localiser");
    loc->setIcon(makeButtonIcon(IconLocation, QColor("#FFFFFF")));
    loc->setIconSize(QSize(15, 15));
    loc->setCursor(Qt::PointingHandCursor);
    loc->setStyleSheet("QPushButton{background:#082849; color:white; border:none; border-radius:4px; padding:2px 10px; font-weight:600; font-size:10.5px;} QPushButton:hover{background:#1B5FA7;}");
    connect(loc, &QPushButton::clicked, this, &MainWindow::centerMap);
    title->addWidget(loc);

    v->addLayout(title);

    mapView = new QWebEngineView;
    mapView->setFixedHeight(500);
    mapView->setUrl(QUrl("qrc:/web/map.html"));
    v->addWidget(mapView, 1);
    return f;
}

// ---------------------------------------------------------------------------
// Statistiques en bas
// ---------------------------------------------------------------------------
QWidget* MainWindow::makeBottomPanel()
{
    auto *w = new QWidget;
    w->setFixedWidth(1310);
    w->setFixedHeight(160);
    auto *g = new QGridLayout(w);
    g->setContentsMargins(0, 0, 0, 0);
    g->setSpacing(6);

    auto *donut = new QFrame;
    donut->setStyleSheet(cardStyle());
    auto *dv = new QVBoxLayout(donut);
    dv->setContentsMargins(8, 6, 8, 6);
    dv->setSpacing(2);
    dv->addWidget(new QLabel("<b style='color:#0F172A; font-size:11px;'>Statuts</b>"));

    auto *donutContent = new QHBoxLayout;
    donutLabel = new QLabel;
    donutLabel->setFixedSize(70, 70);

    QPixmap px(70, 70);
    px.fill(Qt::transparent);
    QPainter p(&px);
    p.setRenderHint(QPainter::Antialiasing);
    QRectF r(5, 5, 60, 60);

    p.setPen(QPen(QColor("#E2E8F0"), 7));
    p.drawArc(r, 0, 360 * 16);
    p.setPen(QPen(QColor("#10B981"), 7));
    p.drawArc(r, 90 * 16, 210 * 16);
    p.setPen(QPen(QColor("#F59E0B"), 7));
    p.drawArc(r, 300 * 16, 54 * 16);
    p.setPen(QPen(QColor("#3B82F6"), 7));
    p.drawArc(r, 354 * 16, 39 * 16);
    p.setPen(QPen(QColor("#6366F1"), 7));
    p.drawArc(r, 393 * 16, 57 * 16);

    p.setPen(QColor(NAVY));
    p.setFont(QFont("Segoe UI", 8, QFont::Bold));
    p.drawText(r, Qt::AlignCenter, "124\ncmd");
    p.end();

    donutLabel->setPixmap(px);
    donutContent->addWidget(donutLabel);

    auto *legend = new QVBoxLayout;
    legend->setSpacing(2);
    auto addLeg = [&](const QString &col, const QString &text, const QString &val){
        auto *lh = new QHBoxLayout;
        auto *dot = new QLabel("●");
        dot->setStyleSheet(QString("color:%1; font-size:10px;").arg(col));
        auto *lt = new QLabel(text);
        lt->setStyleSheet("font-size:9.5px; color:#475569;");
        auto *lv = new QLabel(QString("<b>%1</b>").arg(val));
        lv->setStyleSheet("font-size:9.5px; color:#0F172A;");
        lh->addWidget(dot);
        lh->addWidget(lt, 1);
        lh->addWidget(lv);
        legend->addLayout(lh);
    };
    addLeg("#10B981", "Livrées",    "58%");
    addLeg("#F59E0B", "En cours",   "15%");
    addLeg("#3B82F6", "En atelier", "11%");
    addLeg("#6366F1", "Prêtes",     "16%");
    donutContent->addLayout(legend, 1);
    dv->addLayout(donutContent);

    auto *products = new QFrame;
    products->setStyleSheet(cardStyle());
    auto *pv = new QVBoxLayout(products);
    pv->setContentsMargins(8, 6, 8, 6);
    pv->setSpacing(3);
    pv->addWidget(new QLabel("<b style='color:#0F172A; font-size:11px;'>Top 5 produits</b>"));

    const QStringList names = {"Robe Élégance", "Manteau Chic", "Sac Premium", "T-shirt Classique", "Jean Élégant"};
    const QStringList nums  = {"28", "18", "15", "12", "10"};
    const QStringList imgs  = {"prod_veste.png", "prod_veste.png", "prod_sac.png", "prod_veste.png", "prod_chaussure.png"};

    for(int i = 0; i < 5; ++i) {
        auto *row = new QHBoxLayout;
        row->setContentsMargins(0, 0, 0, 0);
        row->setSpacing(6);

        auto *ico = new QLabel;
        ico->setFixedSize(28, 28);
        ico->setScaledContents(true);
        ico->setStyleSheet("background:#F8FAFC; border:1px solid #E2E8F0; border-radius:4px; padding:1px;");

        QPixmap pm(":/images/" + imgs[i]);
        if(!pm.isNull()) {
            ico->setPixmap(pm.scaled(28, 28, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        } else {
            ico->setText("📦");
            ico->setAlignment(Qt::AlignCenter);
        }

        auto *nameLbl = new QLabel(names[i]);
        nameLbl->setStyleSheet("font-size:10px; color:#334155; font-weight:500;");
        auto *numLbl  = new QLabel(QString("<b>%1</b>").arg(nums[i]));
        numLbl->setStyleSheet("font-size:10px; color:#0F172A;");

        row->addWidget(ico);
        row->addWidget(nameLbl, 1);
        row->addWidget(numLbl);
        pv->addLayout(row);
    }

    auto *alerts = new QFrame;
    alerts->setStyleSheet(cardStyle());
    auto *av = new QVBoxLayout(alerts);
    av->setContentsMargins(8, 6, 8, 6);
    av->setSpacing(4);
    av->addWidget(new QLabel("<b style='color:#0F172A; font-size:11px;'>💡 Alertes</b>"));

    auto addAlert = [&](const QString &icon, const QString &text){
        auto *row = new QHBoxLayout;
        auto *ico = new QLabel(icon);
        auto *lbl = new QLabel(text);
        lbl->setWordWrap(true);
        lbl->setStyleSheet("font-size:9.5px; color:#334155;");
        auto *btn = new QPushButton("Voir");
        btn->setCursor(Qt::PointingHandCursor);
        btn->setFixedSize(30, 18);
        btn->setStyleSheet("QPushButton{background:#F1F5F9; border:1px solid #CBD5E1; border-radius:3px; font-size:8.5px; font-weight:600;} QPushButton:hover{background:#E2E8F0;}");
        row->addWidget(ico);
        row->addWidget(lbl, 1);
        row->addWidget(btn);
        av->addLayout(row);
    };

    addAlert("🔴", "3 retards de livraison (> 24h)");
    addAlert("🟢", "2 clients proches optimisables");
    addAlert("🟠", "Stock faible : Manteaux (5 restants)");

    g->addWidget(donut, 0, 0);
    g->addWidget(products, 0, 1);
    g->addWidget(alerts, 0, 2);

    g->setColumnStretch(0, 1);
    g->setColumnStretch(1, 1);
    g->setColumnStretch(2, 1);

    return w;
}

// ---------------------------------------------------------------------------
// Chatbot IA Flottant
// ---------------------------------------------------------------------------
void MainWindow::setupFloatingChatbot()
{
    chatFloatingBtn = new QPushButton(this);
    chatFloatingBtn->setFixedSize(52, 52);
    chatFloatingBtn->setCursor(Qt::PointingHandCursor);
    chatFloatingBtn->setToolTip("Assistant IA Fashionova");

    QPixmap iconPix(28, 28);
    iconPix.fill(Qt::transparent);
    {
        QPainter p(&iconPix);
        p.setRenderHint(QPainter::Antialiasing);
        p.setPen(Qt::NoPen);
        p.setBrush(QColor("#08213B"));
        QPainterPath path;
        path.addRoundedRect(2, 2, 24, 18, 6, 6);
        QPolygonF tail;
        tail << QPointF(6, 20) << QPointF(3, 26) << QPointF(12, 20);
        path.addPolygon(tail);
        p.drawPath(path);

        p.setBrush(Qt::white);
        p.drawEllipse(7, 9, 3, 3);
        p.drawEllipse(12, 9, 3, 3);
        p.drawEllipse(17, 9, 3, 3);
    }
    chatFloatingBtn->setIcon(QIcon(iconPix));
    chatFloatingBtn->setIconSize(QSize(28, 28));

    chatFloatingBtn->setStyleSheet(R"(
        QPushButton {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #F3D082, stop:1 #C89538);
            border: 2px solid #FFFFFF;
            border-radius: 26px;
        }
        QPushButton:hover {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #FFE4A3, stop:1 #DBA543);
        }
    )");

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

    auto *titleLbl = new QLabel("<b style='color:#FFFFFF; font-size:12px;'>🤖 StyleBot IA</b><br><span style='color:#94A3B8; font-size:9.5px;'>Fashionova Copilot (Llama 3.1)</span>");
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
    chatInput->setPlaceholderText("Posez une question à l'IA ou ordonnez une action...");
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

    addChatMessage("Bot", "Bonjour Mohamed Anoir ! Je suis votre copilote IA StyleHub. Je peux répondre à vos questions, contrôler vos commandes, tracer les itinéraires ou estimer vos coûts.", false);
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);

    if (chatFloatingBtn) {
        int btnX = width() - chatFloatingBtn->width() - 25;
        int btnY = height() - chatFloatingBtn->height() - 25;
        chatFloatingBtn->move(btnX, btnY);
        chatFloatingBtn->raise();

        if (chatWindow) {
            int winX = width() - chatWindow->width() - 25;
            int winY = btnY - chatWindow->height() - 10;
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

// ---------------------------------------------------------------------------
// ENVOI DU MESSAGE (Hybride : Actions directes GUI OU Requête IA Groq Llama 3.1)
// ---------------------------------------------------------------------------
void MainWindow::handleSendChatMessage()
{
    QString q = chatInput->text().trimmed();
    if (q.isEmpty()) return;

    addChatMessage("Vous", q, true);
    chatInput->clear();

    // 1. Exécute l'action locale si possible
    QString directResponse = processChatbotQuery(q);
    if (!directResponse.isEmpty()) {
        addChatMessage("Bot", directResponse, false);
        return;
    }

    // 2. Sinon, interroge l'IA en lui fournissant la liste des commandes existantes
    addChatMessage("Bot", "⏳ <i>StyleBot réfléchit...</i>", false);

    // Synthèse dynamique des données actuelles pour le modèle
    QString ordersContext = "Voici la liste des commandes actuellement enregistrées dans l'atelier :\n";
    for (int r = 0; r < ordersTable->rowCount(); ++r) {
        ordersContext += QString("- %1 | Client: %2 (%3) | Date: %4 | Statut: %5 | Montant: %6\n")
        .arg(ordersTable->item(r, 1)->text(),
             ordersTable->item(r, 3)->text(),
             ordersTable->item(r, 2)->text(),
             ordersTable->item(r, 4)->text(),
             ordersTable->cellWidget(r, 5)->findChild<QLabel*>()->text(),
             ordersTable->item(r, 6)->text());
    }

    QJsonObject userMsg;
    userMsg["role"] = "user";
    userMsg["content"] = QString("%1\n\nQuestion de l'utilisateur : %2").arg(ordersContext, q);
    conversationHistory.append(userMsg);

    QUrl apiUrl("https://api.groq.com/openai/v1/chat/completions");
    QNetworkRequest request(apiUrl);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QString authHeader = "Bearer " + GROQ_API_KEY.trimmed();
    request.setRawHeader("Authorization", authHeader.toUtf8());

    QSslConfiguration sslConfig = QSslConfiguration::defaultConfiguration();
    sslConfig.setProtocol(QSsl::TlsV1_2OrLater);
    sslConfig.setPeerVerifyMode(QSslSocket::VerifyNone);
    request.setSslConfiguration(sslConfig);

    QJsonObject payload;
    payload["model"] = "openai/gpt-oss-20b";
    payload["messages"] = conversationHistory;
    payload["temperature"] = 0.7;
    payload["max_tokens"] = 800;

    QNetworkReply *reply = networkManager->post(request, QJsonDocument(payload).toJson());

    connect(reply, &QNetworkReply::sslErrors, reply, [reply](const QList<QSslError> &errors) {
        Q_UNUSED(errors);
        reply->ignoreSslErrors();
    });
}

// ---------------------------------------------------------------------------
// Traitement intelligent des requêtes Chatbot locales
// ---------------------------------------------------------------------------
QString MainWindow::processChatbotQuery(const QString &query)
{
    const QString raw = query.trimmed();
    const QString q = raw.toLower();

    // 1. Commande directe : Facture PDF
    if (q.contains("facture") || q.contains("pdf")) {
        generateInvoicePdf();
        return "📄 <b>Génération PDF :</b> La boîte d'enregistrement de facture officielle s'est ouverte pour la commande sélectionnée.";
    }

    // 2. Commande directe : Scanner Express
    if (q.contains("scan") || q.contains("douchette") || q.contains("colis")) {
        openBarcodeScannerModal();
        return "📦 <b>Terminal Express :</b> Le scanner d'expédition est ouvert et prêt pour la lecture de colis.";
    }

    // 3. Navigation automatique vers les onglets
    const QStringList modules = {"client", "employé", "commande", "maquette", "machine", "article"};
    for (int i = 0; i < modules.size(); ++i) {
        if ((q.contains("va sur") || q.contains("ouvre") || q.contains("page") || q.contains("module")) && q.contains(modules[i])) {
            if (pageStack) {
                pageStack->setCurrentIndex(i);
                return QString("🚀 <b>Navigation :</b> Basculement vers le module <b>%1</b>.").arg(modules[i].toUpper());
            }
        }
    }

    // 4. Modification de statut en direct (ex: "change CMD-003 en Livrée")
    QRegularExpression updateStatusRx(R"((?:change|passe|met|marque)\s+(?:la\s+commande\s+)?(?:cmd-)?(\d+)\s+(?:en|comme|a)\s+([a-zA-Z\s]+))", QRegularExpression::CaseInsensitiveOption);
    QRegularExpressionMatch stMatch = updateStatusRx.match(q);
    if (stMatch.hasMatch()) {
        int num = stMatch.captured(1).toInt();
        QString targetId = QString("CMD-%1").arg(num, 3, 10, QChar('0'));
        QString newStatus = stMatch.captured(2).trimmed();

        for (int r = 0; r < ordersTable->rowCount(); ++r) {
            if (ordersTable->item(r, 1) && ordersTable->item(r, 1)->text().compare(targetId, Qt::CaseInsensitive) == 0) {
                auto *badgeWidget = ordersTable->cellWidget(r, 5);
                if (badgeWidget) {
                    auto *badge = badgeWidget->findChild<QLabel*>();
                    if (badge) {
                        if (newStatus.contains("livr")) {
                            badge->setText("Livrée");
                            badge->setStyleSheet("background:#ECFDF5; color:#059669; border-radius:6px; padding:1px 6px; font-weight:bold; font-size:8.5px;");
                        } else if (newStatus.contains("cours")) {
                            badge->setText("En cours");
                            badge->setStyleSheet("background:#FFFBEB; color:#D97706; border-radius:6px; padding:1px 6px; font-weight:bold; font-size:8.5px;");
                        } else {
                            badge->setText("Prête en atelier");
                            badge->setStyleSheet("background:#EFF6FF; color:#2563EB; border-radius:6px; padding:1px 6px; font-weight:bold; font-size:8.5px;");
                        }
                        ordersTable->selectRow(r);
                        updateDetails(r);
                        return QString("⚡ <b>Statut mis à jour :</b> La commande <b>%1</b> est désormais <b>%2</b>.").arg(targetId, badge->text());
                    }
                }
            }
        }
    }

    // 5. Calculs mathématiques
    bool isMath = false;
    double mathRes = evaluateMathExpression(raw, isMath);
    if (isMath) {
        return QString("🧮 <b>Calcul :</b><br><code>%1</code> = <b style='color:#059669; font-size:13px;'>%2</b>")
            .arg(raw, QString::number(mathRes, 'f', 2));
    }

    // 6. Détection intelligente de numéro de commande (ex: "commande 005", "route 5", "CMD-007")
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

    // Recherche par nom de client si aucun numéro trouvé
    if (matchedRow == -1) {
        for (int r = 0; r < ordersTable->rowCount(); ++r) {
            QString client = ordersTable->item(r, 2) ? ordersTable->item(r, 2)->text().toLower() : "";
            QString name   = ordersTable->item(r, 3) ? ordersTable->item(r, 3)->text().toLower() : "";
            if ((!client.isEmpty() && q.contains(client)) || (!name.isEmpty() && q.contains(name.split(" ").first()))) {
                matchedRow = r;
                break;
            }
        }
    }

    // Action sur la commande trouvée
    if (matchedRow != -1) {
        ordersTable->selectRow(matchedRow);
        updateDetails(matchedRow);

        bool wantsRoute = q.contains("route") || q.contains("chemin") || q.contains("itineraire") ||
                          q.contains("trajet") || q.contains("gps") || q.contains("carte") || q.contains("localisation");

        if (wantsRoute) {
            showSelectedRoute();
            return QString("📍 <b>Itinéraire et localisation affichés :</b><br>"
                           "• <b>Commande :</b> %1<br>"
                           "• <b>Client :</b> %2<br>"
                           "• <b>Adresse :</b> %3<br>"
                           "• <b>Distance :</b> %4 | <b>Délai :</b> %5<br>"
                           "<i>Le marqueur et le tracé GPS ont été mis à jour sur la carte.</i>")
                .arg(ordersTable->item(matchedRow, 1)->text(),
                     ordersTable->item(matchedRow, 3)->text(),
                     detailAddress ? detailAddress->text().remove("📍  Livraison : ") : "Tunis",
                     distanceLabel ? distanceLabel->text() : "5.8 km",
                     delayLabel ? delayLabel->text() : "1 jour");
        }

        return QString("🎯 <b>Commande sélectionnée :</b> <b>%1</b> (%2)<br>"
                       "• Montant : <b>%3</b><br>"
                       "• Statut : <i>%4</i>.")
            .arg(ordersTable->item(matchedRow, 1)->text(),
                 ordersTable->item(matchedRow, 3)->text(),
                 ordersTable->item(matchedRow, 6)->text(),
                 ordersTable->cellWidget(matchedRow, 5)->findChild<QLabel*>()->text());
    }

    return QString(); // Aucune commande locale spécifique trouvée -> transmettre à l'IA Cloud
}
// ---------------------------------------------------------------------------
// Logique des Commandes
// ---------------------------------------------------------------------------
void MainWindow::setupOrders()
{
    struct O { QString id, client, name, date, status, amount, img, lat, lon, address; };
    const QList<O> os = {
        {"CMD-001", "CL-001", "Sarah Ben Ali",    "26/09/2026 14:32", "Livrée",           "450,00 TND", "prod_veste.png",     "36.8065", "10.1815", "Tunis 1000"},
        {"CMD-002", "CL-002", "Yassine Trabelsi", "24/09/2026 16:20", "En cours",         "720,00 TND", "prod_lunette.png",   "36.8665", "10.1647", "Ariana"},
        {"CMD-003", "CL-004", "Lina Khroub",      "24/09/2026 16:30", "Prête en atelier", "210,00 TND", "prod_veste.png",     "36.8450", "10.2200", "12, Rue de la Liberté, Tunis 1000"},
        {"CMD-005", "CL-005", "Maya Sassi",       "21/09/2026 17:30", "Livrée",           "215,50 TND", "prod_bijoux.png",    "36.7750", "10.2450", "El Mourouj"},
        {"CMD-007", "CL-007", "Ramla Boussetta",  "20/09/2026 10:22", "En cours",         "540,00 TND", "prod_sac.png",       "36.8330", "10.1500", "Manar"},
        {"CMD-008", "CL-008", "Leila Srail",      "19/09/2026 15:40", "Livrée",           "690,70 TND", "prod_chaussure.png", "36.7350", "10.3000", "Ben Arous"}
    };

    ordersTable->setRowCount(os.size());

    for(int r = 0; r < os.size(); ++r) {
        const auto &o = os[r];

        auto *chkWidget = new QWidget;
        auto *chkLayout = new QHBoxLayout(chkWidget);
        chkLayout->setContentsMargins(0, 0, 0, 0);
        chkLayout->setAlignment(Qt::AlignCenter);
        chkLayout->addWidget(new QCheckBox);
        ordersTable->setCellWidget(r, 0, chkWidget);

        auto *idItem = new QTableWidgetItem(o.id);
        QPixmap pm(":/images/" + o.img);
        if(!pm.isNull()) idItem->setIcon(QIcon(pm.scaled(28, 28, Qt::KeepAspectRatio, Qt::SmoothTransformation)));
        idItem->setFont(QFont("Segoe UI", 8.5, QFont::Bold));
        ordersTable->setItem(r, 1, idItem);

        ordersTable->setItem(r, 2, new QTableWidgetItem(o.client));
        ordersTable->setItem(r, 3, new QTableWidgetItem(o.name));
        ordersTable->setItem(r, 4, new QTableWidgetItem(o.date));

        auto *badgeWidget = new QWidget;
        auto *bLayout = new QHBoxLayout(badgeWidget);
        bLayout->setContentsMargins(1, 1, 1, 1);
        bLayout->setAlignment(Qt::AlignCenter);
        auto *badge = new QLabel(o.status);
        badge->setAlignment(Qt::AlignCenter);

        if(o.status == "Livrée") {
            badge->setStyleSheet("background:#ECFDF5; color:#059669; border-radius:6px; padding:1px 6px; font-weight:bold; font-size:8.5px;");
        } else if(o.status == "En cours") {
            badge->setStyleSheet("background:#FFFBEB; color:#D97706; border-radius:6px; padding:1px 6px; font-weight:bold; font-size:8.5px;");
        } else {
            badge->setStyleSheet("background:#EFF6FF; color:#2563EB; border-radius:6px; padding:1px 6px; font-weight:bold; font-size:8.5px;");
        }
        bLayout->addWidget(badge);
        ordersTable->setCellWidget(r, 5, badgeWidget);

        auto *amtItem = new QTableWidgetItem(o.amount);
        amtItem->setFont(QFont("Segoe UI", 8, QFont::DemiBold));
        ordersTable->setItem(r, 6, amtItem);

        auto *actWidget = new QWidget;
        auto *aLayout = new QHBoxLayout(actWidget);
        aLayout->setContentsMargins(1, 1, 1, 1);
        aLayout->setSpacing(2);
        aLayout->setAlignment(Qt::AlignCenter);

        auto makeAction = [](const QString &ico){
            auto *btn = new QPushButton(ico);
            btn->setFixedSize(16, 16);
            btn->setCursor(Qt::PointingHandCursor);
            btn->setStyleSheet("QPushButton{background:transparent; border:none; color:#64748B;} QPushButton:hover{background:#E2E8F0; border-radius:2px;}");
            return btn;
        };

        aLayout->addWidget(makeAction("👁️"));
        aLayout->addWidget(makeAction("✏️"));
        aLayout->addWidget(makeAction("🗑️"));
        ordersTable->setCellWidget(r, 7, actWidget);

        ordersTable->setRowHeight(r, 34);
    }
}

void MainWindow::updateDetails(int row)
{
    if(row < 0 || row >= ordersTable->rowCount()) return;

    currentRowSelected = row;

    const QString id     = ordersTable->item(row, 1) ? ordersTable->item(row, 1)->text() : "";
    const QString client = ordersTable->item(row, 2) ? ordersTable->item(row, 2)->text() : "";
    const QString name   = ordersTable->item(row, 3) ? ordersTable->item(row, 3)->text() : "";
    const QString date   = ordersTable->item(row, 4) ? ordersTable->item(row, 4)->text() : "";
    const QString amount = ordersTable->item(row, 6) ? ordersTable->item(row, 6)->text() : "";

    selectedOrderLabel->setText(id);

    QPixmap p(":/images/prod_veste.png");
    if(id == "CMD-002") p = QPixmap(":/images/prod_lunette.png");
    if(id == "CMD-005") p = QPixmap(":/images/prod_bijoux.png");
    if(id == "CMD-008") p = QPixmap(":/images/prod_chaussure.png");

    if(!p.isNull())
        detailProductImage->setPixmap(p.scaled(60, 60, Qt::KeepAspectRatio, Qt::SmoothTransformation));

    detailClient->setText(QString("👤  Client : <b>%1</b> <span style='color:#64748B;'>(%2)</span>").arg(name, client));
    detailDate->setText(QString("🕒  Date : <b>%1</b>").arg(date));
    detailStatus->setText("●  Statut : <b style='color:#2563EB;'>Prête en atelier</b>");
    detailAmount->setText(QString("💰  Montant : <b style='color:#0F172A;'>%1</b>").arg(amount));
    detailAddress->setText("📍  Livraison : <b>12, Rue de la Liberté, Tunis</b>");

    double dist = 5.8 + row * 1.35;
    distanceLabel->setText(QString("<b>%1 km</b>").arg(QString::number(dist, 'f', 1)));
    costLabel->setText(QString("<b>%1 TND</b>").arg(QString::number(4.5 + dist * 0.43, 'f', 2)));
    delayLabel->setText(row % 3 == 0 ? "<b>1 jour</b>" : "<b>1–2 jours</b>");

    const QList<QPair<QString,QString>> coords = {
        {"36.8065", "10.1815"}, {"36.8665", "10.1647"}, {"36.8450", "10.2200"},
        {"36.7750", "10.2450"}, {"36.8330", "10.1500"}, {"36.7350", "10.3000"}
    };
    selectedClientLat = coords[row].first;
    selectedClientLon = coords[row].second;

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

// ---------------------------------------------------------------------------
// GESTIONNAIRE DE L'AGENT IA GROQ (LLAMA 3.1)
// ---------------------------------------------------------------------------
void MainWindow::initAIAgent()
{
    conversationHistory = QJsonArray();

    QJsonObject systemPrompt;
    systemPrompt["role"] = "system";
    systemPrompt["content"] =
        "Tu es 'StyleBot', un copilote IA expert intégré dans l'application StyleHub / Fashionova. "
        "Tu assistes les responsables dans la gestion d'atelier textile, le suivi des commandes, "
        "l'estimation des délais de confection, les calculs de remises et la logistique. "
        "Réponds toujours avec précision, courtoisie, de manière concise et en français.";
    conversationHistory.append(systemPrompt);
}

void MainWindow::onAiResponseReceived(QNetworkReply *reply)
{
    // Supprime la bulle "StyleBot réfléchit..."
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
        addChatMessage("Bot", "⚠️ <b>Erreur :</b> " + serverMsg, false);
    }

    reply->deleteLater();
}