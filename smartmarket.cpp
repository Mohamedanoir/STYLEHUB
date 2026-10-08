#include "smartmarket.h"
#include "ui_smartmarket.h"
#include "deliverymap.h"
#include <QMainWindow>
#include <QColor>
#include <QDate>
#include <QGuiApplication>
#include <QLocale>
#include <QScreen>
#include <QPair>
#include <QPdfWriter>
#include <QSignalBlocker>
#include <algorithm>
#include "integrated/clients/client.h"
#include "integrated/articles/articlewidgets.h"
#include <QPageSize>
#include "integrated/articles/article.h"
#include "maquette.h"
#include "icones.h"
#include "statwidgets.h"
#include "widgets.h"
#include <QDesktopServices>
#include <QKeyEvent>
#include <QLabel>
#include <QMovie>
#include <QPushButton>
#include <QTableWidget>
#include <QToolButton>
#include <utility>
#include <QAction>
#include <QComboBox>
#include <QDateEdit>
#include <QLineEdit>
#include <QMap>
#include <QMarginsF>
#include <QMenu>
#include <QStyle>
#include "chartwidgets.h"
#include "database.h"
#include "detailsdialog.h"
#include "uihelpers.h"
#include "integrated/employes/logindialog.h"
#include <QGroupBox>
#include <QSplitter>
#include "integrated/employes/employee.h"
#include "modules/machines/src/database.h"
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
#include <QTimer>
#include <QStatusBar>
#include <QEvent>
#include <QFontDatabase>
#include <QGraphicsDropShadowEffect>
#include <QIcon>

static const QString NAVY = "#082849";
static const QString BLUE = "#1B5FA7";
static const QString GOLD = "#D8A64B";

static QString cardStyle()
{
    return "QFrame { background:#FFFFFF; border:1px solid #E2E8F0; border-radius:8px; }";
}

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
SmartMarket::SmartMarket(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::SmartMarket), chatFloatingBtn(nullptr), chatWindow(nullptr), sortAscending(true), currentRowSelected(0)
{
    ui->setupUi(this);       // construit smartmarket.ui et connecte automatiquement les slots on_<objet>_<signal>()
    resize(1440, 880);
    setMinimumSize(1100, 680);
    setWindowTitle("FASHIONOVA — SmartMarket");

    // Initialisation Réseau et Agent IA
    networkManager = new QNetworkAccessManager(this);
    connect(networkManager, &QNetworkAccessManager::finished, this, &SmartMarket::onAiResponseReceived);
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

    // ---- Module Commandes : interface construite en code, insérée dans la page PCommande de smartmarket.ui
    ui->centralwidget->setStyleSheet("background-color: #F8FAFC;");

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

    ui->layoutPCommandeCRUD->addWidget(scrollArea);
    ui->PCommande->setStyleSheet("background-color: #F8FAFC; border: none;");   // même rendu que l'ancien conteneur de pages

    // ---- Modules des autres membres (méthode du guide) : leurs widgets sont dans smartmarket.ui
    //      (pages PClient, PEmpl, PMaquette, PMachine, PArticle) et leurs slots ont été copiés dans cette classe.
    QString dbError; Database::open(&dbError);
    initClient();
    initEmploye();
    initMaquette();
    initMachine();
    initArticle();

    setupSidebar();
    setupLogin();
    setupMenu();

    setupOrders();
    if (ordersTable->rowCount() > 0) {
        ordersTable->selectRow(0);
        updateDetails(0);
    }

    setupFloatingChatbot();

    // Barre d'état : visible seulement le temps d'un message (messages « ajouté », « modifié »... des modules)
    statusBar()->hide();
    connect(statusBar(), &QStatusBar::messageChanged, this, [this](const QString &msg) {
        statusBar()->setVisible(!msg.isEmpty() && ui->SWSmartMarket->currentIndex() >= PClient);
    });

    // Démarrage sur la page de connexion (menu et chatbot masqués)
    ui->SWSmartMarket->setCurrentIndex(PLogin);
    on_SWSmartMarket_currentChanged(PLogin);
}

SmartMarket::~SmartMarket()
{
    delete ui;
}

// ---------------------------------------------------------------------------
// Page épurée pour les autres modules
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// Sidebar
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// En-tête
// ---------------------------------------------------------------------------
QWidget* SmartMarket::makeHeader()
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
QWidget* SmartMarket::makeKpis()
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
QWidget* SmartMarket::makeOrdersPanel()
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
    connect(btnScanModal, &QPushButton::clicked, this, &SmartMarket::openBarcodeScannerModal);
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

    connect(ordersTable, &QTableWidget::cellClicked, this, &SmartMarket::selectOrder);
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
    connect(btnSort, &QPushButton::clicked, this, &SmartMarket::toggleSortOrders);
    buttons->addWidget(btnSort);

    buttons->addStretch();
    v->addLayout(buttons);

    return f;
}

void SmartMarket::toggleSortOrders()
{
    sortAscending = !sortAscending;
    ordersTable->sortItems(1, sortAscending ? Qt::AscendingOrder : Qt::DescendingOrder);
}

// ---------------------------------------------------------------------------
// Panneau Détails avec bouton Facture PDF & QR Code
// ---------------------------------------------------------------------------
QWidget* SmartMarket::makeDetailsPanel()
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
    connect(pdfBtn, &QPushButton::clicked, this, &SmartMarket::generateInvoicePdf);
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
    connect(route, &QPushButton::clicked, this, &SmartMarket::showSelectedRoute);
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
void SmartMarket::generateInvoicePdf()
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
void SmartMarket::openBarcodeScannerModal()
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
// Carte des livraisons (widget natif DeliveryMap - OpenStreetMap)
// ---------------------------------------------------------------------------
QWidget* SmartMarket::makeMapPanel()
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
    connect(btnRoute, &QPushButton::clicked, this, &SmartMarket::showSelectedRoute);
    title->addWidget(btnRoute);

    auto *loc = new QPushButton("  Localiser");
    loc->setIcon(makeButtonIcon(IconLocation, QColor("#FFFFFF")));
    loc->setIconSize(QSize(15, 15));
    loc->setCursor(Qt::PointingHandCursor);
    loc->setStyleSheet("QPushButton{background:#082849; color:white; border:none; border-radius:4px; padding:2px 10px; font-weight:600; font-size:10.5px;} QPushButton:hover{background:#1B5FA7;}");
    connect(loc, &QPushButton::clicked, this, &SmartMarket::centerMap);
    title->addWidget(loc);

    v->addLayout(title);

    mapView = new DeliveryMap;
    mapView->setMinimumHeight(260);
    v->addWidget(mapView, 1);

    // Clic sur un marqueur de la carte -> sélection de la commande dans le tableau
    connect(mapView, &DeliveryMap::markerClicked, this, [this](const QString &id) {
        for (int r = 0; r < ordersTable->rowCount(); ++r) {
            if (ordersTable->item(r, 1) && ordersTable->item(r, 1)->text() == id) {
                ordersTable->selectRow(r);
                updateDetails(r);
                break;
            }
        }
    });

    // Itinéraire calculé : distance / frais réels (route) dans le panneau « Estimation livraison »
    connect(mapView, &DeliveryMap::routeReady, this, [this](const QString &id, double km, int minutes, bool approx) {
        if (!ordersTable || currentRowSelected < 0 || !ordersTable->item(currentRowSelected, 1)
            || ordersTable->item(currentRowSelected, 1)->text() != id) return;
        distanceLabel->setText(QString("<b>%1 km</b>%2").arg(QString::number(km, 'f', 1), approx ? " (à vol d'oiseau)" : ""));
        costLabel->setText(QString("<b>%1 TND</b>").arg(QString::number(4.5 + km * 0.43, 'f', 2)));
        distanceLabel->setToolTip(QString("Durée estimée en voiture : %1 min").arg(minutes));
    });
    return f;
}

// ---------------------------------------------------------------------------
// Statistiques en bas
// ---------------------------------------------------------------------------
QWidget* SmartMarket::makeBottomPanel()
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
void SmartMarket::setupFloatingChatbot()
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

    connect(chatFloatingBtn, &QPushButton::clicked, this, &SmartMarket::toggleChatWindow);

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
    connect(closeBtn, &QPushButton::clicked, this, &SmartMarket::toggleChatWindow);

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
    connect(chatInput, &QLineEdit::returnPressed, this, &SmartMarket::handleSendChatMessage);

    auto *sendBtn = new QPushButton("➤");
    sendBtn->setFixedSize(32, 32);
    sendBtn->setCursor(Qt::PointingHandCursor);
    sendBtn->setStyleSheet("QPushButton{background:#082849; color:white; border-radius:16px; font-weight:bold;} QPushButton:hover{background:#1B5FA7;}");
    connect(sendBtn, &QPushButton::clicked, this, &SmartMarket::handleSendChatMessage);

    inLayout->addWidget(chatInput, 1);
    inLayout->addWidget(sendBtn);
    winLayout->addWidget(inputFrame);

    addChatMessage("Bot", "Bonjour Mohamed Anoir ! Je suis votre copilote IA StyleHub. Je peux répondre à vos questions, contrôler vos commandes, tracer les itinéraires ou estimer vos coûts.", false);
}

void SmartMarket::resizeEvent(QResizeEvent *event)
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

void SmartMarket::toggleChatWindow()
{
    if (!chatWindow) return;
    bool visible = !chatWindow->isVisible();
    chatWindow->setVisible(visible);
    if (visible) {
        chatInput->setFocus();
        chatWindow->raise();
    }
}

void SmartMarket::addChatMessage(const QString &sender, const QString &text, bool isUser)
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
QString SmartMarket::getApiKey()
{
    QFile file("api_key.txt");
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return QTextStream(&file).readLine().trimmed();
    }
    // Clé de secours si le fichier n'est pas trouvé
    return "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";
}
// ---------------------------------------------------------------------------
// ENVOI DU MESSAGE (Hybride : Actions directes GUI OU Requête IA Groq Llama 3.1)
// ---------------------------------------------------------------------------
void SmartMarket::handleSendChatMessage()
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
    QString authHeader = "Bearer " + getApiKey();

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
QString SmartMarket::processChatbotQuery(const QString &query)
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
            static const Page pages[] = { PClient, PEmpl, PCommande, PMaquette, PMachine, PArticle };
            ui->SWSmartMarket->setCurrentIndex(pages[i]);
            return QString("🚀 <b>Navigation :</b> Basculement vers le module <b>%1</b>.").arg(modules[i].toUpper());
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
void SmartMarket::setupOrders()
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

    // Marqueurs de la carte (une commande = un point de livraison)
    if (mapView) {
        QList<DeliveryMap::Marker> marqueurs;
        for (const auto &o : os)
            marqueurs.append({ o.id, o.name, o.status, o.lat.toDouble(), o.lon.toDouble() });
        mapView->setMarkers(marqueurs);
    }
}

void SmartMarket::updateDetails(int row)
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

    const QList<QPair<QString,QString>> coords = {
        {"36.8065", "10.1815"}, {"36.8665", "10.1647"}, {"36.8450", "10.2200"},
        {"36.7750", "10.2450"}, {"36.8330", "10.1500"}, {"36.7350", "10.3000"}
    };
    selectedClientLat = coords[row].first;
    selectedClientLon = coords[row].second;

    // distance à vol d'oiseau atelier -> client (remplacée par la distance routière après « Itinéraire »)
    const double dist = DeliveryMap::haversineKm(DeliveryMap::kAtelierLat, DeliveryMap::kAtelierLon,
                                                 selectedClientLat.toDouble(), selectedClientLon.toDouble());
    distanceLabel->setText(QString("<b>%1 km</b>").arg(QString::number(dist, 'f', 1)));
    distanceLabel->setToolTip(QString());
    costLabel->setText(QString("<b>%1 TND</b>").arg(QString::number(4.5 + dist * 0.43, 'f', 2)));
    delayLabel->setText(row % 3 == 0 ? "<b>1 jour</b>" : "<b>1–2 jours</b>");

    // Carte : met en évidence la commande et centre la vue dessus
    if (mapView) mapView->selectOrder(id, true);
}

void SmartMarket::selectOrder(int row)
{
    updateDetails(row);
}

void SmartMarket::centerMap()
{
    if (mapView) mapView->centerWorkshop();
}

void SmartMarket::showSelectedRoute()
{
    if (!mapView || currentRowSelected < 0 || !ordersTable->item(currentRowSelected, 1)) return;
    mapView->showRoute(ordersTable->item(currentRowSelected, 1)->text());
}

// ---------------------------------------------------------------------------
// GESTIONNAIRE DE L'AGENT IA GROQ (LLAMA 3.1)
// ---------------------------------------------------------------------------
void SmartMarket::initAIAgent()
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

void SmartMarket::onAiResponseReceived(QNetworkReply *reply)
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

// ===========================================================================
//  MENU LATÉRAL + NAVIGATION (méthode du guide : un slot par bouton)
// ===========================================================================
void SmartMarket::setupSidebar()
{
    ui->sidebar->setStyleSheet(R"(
        QPushButton { text-align:left; padding-left:20px; border:none; color:#FFFFFF; background:transparent;
                      font-size:13px; font-weight:500; }
        QPushButton:hover { background:rgba(255,255,255,0.12); color:#FFFFFF; border-radius:6px; margin:0 6px; }
        QPushButton:checked { background:qlineargradient(x1:0,y1:0,x2:1,y2:0,stop:0 #E5C378,stop:1 #C89538);
                              color:#08213B; border-radius:6px; margin:0 6px; font-weight:bold; }
        QPushButton#btnDeconnexion { color:#F5D7A1; margin:0 6px; border:1px solid rgba(229,195,120,0.45); border-radius:6px; }
        QPushButton#btnDeconnexion:hover { background:rgba(255,255,255,0.12); }
    )");
    QPushButton *boutons[] = { ui->btnMClient, ui->btnMEmpl, ui->btnMCommande, ui->btnMMaquette, ui->btnMMachine, ui->btnMArticle };
    for (int i = 0; i < 6; ++i) {
        boutons[i]->setIcon(makeDynamicSidebarIcon(i));
        boutons[i]->setIconSize(QSize(20, 20));
    }
}

void SmartMarket::on_btnMClient_clicked()   { ui->SWSmartMarket->setCurrentIndex(PClient); }
void SmartMarket::on_btnMEmpl_clicked()     { ui->SWSmartMarket->setCurrentIndex(PEmpl); }
void SmartMarket::on_btnMCommande_clicked() { ui->SWSmartMarket->setCurrentIndex(PCommande); }
void SmartMarket::on_btnMMaquette_clicked() { ui->SWSmartMarket->setCurrentIndex(PMaquette); }
void SmartMarket::on_btnMMachine_clicked()  { ui->SWSmartMarket->setCurrentIndex(PMachine); }
void SmartMarket::on_btnMArticle_clicked()  { ui->SWSmartMarket->setCurrentIndex(PArticle); }

// Déconnexion : retour à la page de connexion
void SmartMarket::on_btnDeconnexion_clicked()
{
    ui->txtMotDePasse->clear();
    ui->SWSmartMarket->setCurrentIndex(PLogin);
}

// Le menu et le chatbot ne sont visibles que dans les modules (pas sur les pages de connexion)
void SmartMarket::on_SWSmartMarket_currentChanged(int index)
{
    const bool connecte = index >= PClient;
    ui->sidebar->setVisible(connecte);
    if (!connecte) statusBar()->hide();
    if (chatFloatingBtn) {
        chatFloatingBtn->setVisible(connecte);
        if (!connecte && chatWindow) chatWindow->hide();
    }

    QPushButton *bouton = nullptr;
    switch (index) {
    case PClient:   bouton = ui->btnMClient;   break;
    case PEmpl:     bouton = ui->btnMEmpl;     break;
    case PCommande: bouton = ui->btnMCommande; break;
    case PMaquette: bouton = ui->btnMMaquette; break;
    case PMachine:  bouton = ui->btnMMachine;  break;
    case PArticle:  bouton = ui->btnMArticle;  break;
    default: break;
    }
    if (bouton) bouton->setChecked(true);
    if (connecte)      // pas de curseur clignotant dans le premier champ de la page affichée
        QTimer::singleShot(0, this, [this] { if (QWidget *fw = focusWidget()) fw->clearFocus(); });
    if (index == PMaquette) onPageAfficheeMaquette();
}

// ===========================================================================
//  PAGE DE CONNEXION (PLogin) et « Mot de passe oublié » (PMotDePasse)
// ===========================================================================
void SmartMarket::setupLogin()
{
    static bool policesChargees = false;
    if (!policesChargees) {
        const QStringList polices = {
            ":/login/fonts/Jost-Regular.ttf", ":/login/fonts/Jost-Medium.ttf", ":/login/fonts/Jost-SemiBold.ttf",
            ":/login/fonts/CormorantGaramond-Medium.ttf", ":/login/fonts/CormorantGaramond-MediumItalic.ttf",
        };
        for (const QString &p : polices) QFontDatabase::addApplicationFont(p);
        policesChargees = true;
    }

    // Le style des pages de connexion est posé sur ces pages uniquement (pas sur toute l'application)
    QFile qss(":/login/login.qss");
    if (qss.open(QFile::ReadOnly | QFile::Text)) {
        const QString style = QString::fromUtf8(qss.readAll());
        ui->PLogin->setStyleSheet(style);
        ui->PMotDePasse->setStyleSheet(style);
    }

    ajouterIcone(ui->txtLogin, ":/login/images/utilisateur.png", true);
    ajouterIcone(ui->txtMotDePasse, ":/login/images/cadenas.png", true);
    ajouterIcone(ui->txtMotDePasse, ":/login/images/oeil.png", false);   // clic = afficher / masquer
    ajouterIcone(ui->cmbRole, ":/login/images/groupe.png", true);
    ui->txtMotDePasse->setEchoMode(QLineEdit::Password);

    ui->btnConnecter->setIcon(QIcon(":/login/images/fleche.png"));
    ui->btnConnecter->setIconSize(QSize(22, 22));
    ui->btnConnecter->setText(ui->btnConnecter->text() + "   ");
    ui->btnConnecter->setCursor(Qt::PointingHandCursor);
    ui->btnConnecter->setDefault(true);

    auto *ombre = new QGraphicsDropShadowEffect(this);
    ombre->setBlurRadius(28);
    ombre->setOffset(0, 8);
    ombre->setColor(QColor(15, 27, 46, 70));
    ui->btnConnecter->setGraphicsEffect(ombre);
}

// Pour l'instant : aucune vérification, toute saisie (même vide) ouvre le menu principal.
void SmartMarket::on_btnConnecter_clicked()          { ui->SWSmartMarket->setCurrentIndex(PMenu); }
void SmartMarket::on_txtLogin_returnPressed()        { on_btnConnecter_clicked(); }
void SmartMarket::on_txtMotDePasse_returnPressed()   { on_btnConnecter_clicked(); }
void SmartMarket::on_btnMotDePasseOublie_clicked()   { ui->SWSmartMarket->setCurrentIndex(PMotDePasse); }
void SmartMarket::on_btnRetourLogin_clicked()        { ui->SWSmartMarket->setCurrentIndex(PLogin); }

void SmartMarket::on_btnEnvoyerReset_clicked()
{
    QMessageBox::information(this, "Mot de passe oublié",
                             "Si cette adresse correspond à un compte, un lien de réinitialisation vient d'être envoyé.");
    ui->txtEmailReset->clear();
    ui->SWSmartMarket->setCurrentIndex(PLogin);
}

// ===========================================================================
//  MENU PRINCIPAL (PMenu) : après la connexion, l'utilisateur choisit un module
// ===========================================================================
void SmartMarket::setupMenu()
{
    // Style du menu posé sur cette page uniquement (pas sur toute l'application)
    QFile qss(":/menu/menu.qss");
    if (qss.open(QFile::ReadOnly | QFile::Text))
        ui->PMenu->setStyleSheet(QString::fromUtf8(qss.readAll()));
    ui->scrollMenu->viewport()->setAutoFillBackground(false);
    ui->PMenu->installEventFilter(this);       // positionne « Quitter » et adapte les icônes

    decorerBoutonMenu(ui->btnMenuClients,   ":/menu/images/clients.png");
    decorerBoutonMenu(ui->btnMenuEmployes,  ":/menu/images/employes.png");
    decorerBoutonMenu(ui->btnMenuCommandes, ":/menu/images/commandes.png");
    decorerBoutonMenu(ui->btnMenuMaquettes, ":/menu/images/maquettes.png");
    decorerBoutonMenu(ui->btnMenuArticles,  ":/menu/images/articles.png");
    decorerBoutonMenu(ui->btnMenuMachines,  ":/menu/images/machines.png");
}

// Icône ronde à gauche, flèche dorée à droite et ombre sous le bouton
void SmartMarket::decorerBoutonMenu(QPushButton *bouton, const QString &icone)
{
    bouton->setIcon(QIcon(icone));
    bouton->setIconSize(QSize(86, 86));
    bouton->setText("     " + bouton->text());

    auto *fleche = new QLabel(bouton);
    fleche->setPixmap(QPixmap(":/menu/images/fleche.png"));
    fleche->setAttribute(Qt::WA_TransparentForMouseEvents);
    fleche->setStyleSheet("background: transparent;");

    auto *layout = new QHBoxLayout(bouton);
    layout->setContentsMargins(0, 0, 34, 0);
    layout->addStretch();
    layout->addWidget(fleche);

    auto *ombre = new QGraphicsDropShadowEffect(bouton);
    ombre->setBlurRadius(18);
    ombre->setOffset(0, 4);
    ombre->setColor(QColor(0, 0, 0, 60));
    bouton->setGraphicsEffect(ombre);
}

// Un slot par bouton du menu (méthode du guide : setCurrentIndex de la page du module)
void SmartMarket::on_btnMenuClients_clicked()   { ui->SWSmartMarket->setCurrentIndex(PClient); }
void SmartMarket::on_btnMenuEmployes_clicked()  { ui->SWSmartMarket->setCurrentIndex(PEmpl); }
void SmartMarket::on_btnMenuCommandes_clicked() { ui->SWSmartMarket->setCurrentIndex(PCommande); }
void SmartMarket::on_btnMenuMaquettes_clicked() { ui->SWSmartMarket->setCurrentIndex(PMaquette); }
void SmartMarket::on_btnMenuArticles_clicked()  { ui->SWSmartMarket->setCurrentIndex(PArticle); }
void SmartMarket::on_btnMenuMachines_clicked()  { ui->SWSmartMarket->setCurrentIndex(PMachine); }
void SmartMarket::on_btnMenuQuitter_clicked()   { on_btnDeconnexion_clicked(); }

// Œil : clic = afficher / masquer le mot de passe
bool SmartMarket::eventFilter(QObject *obj, QEvent *event)
{
    // --- module Clients : bannière et statistiques s'adaptent à la largeur
    if (event->type() == QEvent::Resize) {
        if (obj == ui->bannerImageClient)     { updateBannerClient(); return false; }
        else if (obj == ui->statsPanelClient) { updateStatsClient();  return false; }
    }
    // --- menu principal : « Quitter » en haut à droite, icônes proportionnelles à la hauteur des boutons
    if (obj == ui->PMenu && event->type() == QEvent::Resize) {
        ui->btnMenuQuitter->move(ui->PMenu->width() - ui->btnMenuQuitter->width() - 28, 22);
        ui->btnMenuQuitter->raise();
        QPushButton *modules[] = { ui->btnMenuClients, ui->btnMenuEmployes, ui->btnMenuCommandes,
                                   ui->btnMenuMaquettes, ui->btnMenuArticles, ui->btnMenuMachines };
        // hauteur des boutons : 104 px si la fenêtre est assez haute, sinon ils se compactent (jusqu'à 72 px)
        static int surcharge = -1;                       // hauteur du menu hors boutons (titre, marges, espacements)
        if (surcharge < 0)
            surcharge = ui->contenuMenu->layout()->minimumSize().height() - 6 * ui->btnMenuClients->minimumHeight();
        const int h = qBound(72, (ui->PMenu->height() - surcharge) / 6, 104);
        for (QPushButton *b : modules) {
            b->setMinimumHeight(h);
            const int cote = qBound(40, h - 16, 86);
            b->setIconSize(QSize(cote, cote));
        }
    }
    if (obj == m_oeil && event->type() == QEvent::MouseButtonRelease) {
        const bool masque = ui->txtMotDePasse->echoMode() == QLineEdit::Password;
        ui->txtMotDePasse->setEchoMode(masque ? QLineEdit::Normal : QLineEdit::Password);
        return true;
    }
    return QMainWindow::eventFilter(obj, event);
}

// Place une petite icône à gauche (ou à droite) d'un champ
void SmartMarket::ajouterIcone(QWidget *champ, const QString &image, bool aGauche)
{
    auto *icone = new QLabel(champ);
    icone->setPixmap(QPixmap(image).scaled(24, 24, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    icone->setAttribute(Qt::WA_TransparentForMouseEvents, aGauche);
    icone->setStyleSheet("background: transparent; border: none;");
    if (!aGauche) {
        icone->setCursor(Qt::PointingHandCursor);
        icone->installEventFilter(this);
        m_oeil = icone;
    }
    auto *layout = qobject_cast<QHBoxLayout *>(champ->layout());
    if (!layout) {
        layout = new QHBoxLayout(champ);
        layout->setContentsMargins(22, 0, 22, 0);
        layout->addStretch();
    }
    if (aGauche) layout->insertWidget(0, icone);
    else         layout->addWidget(icone);
}

// ===========================================================================
//  MODULE CLIENTS (page PClient) — slots copiés du projet « Clients »
// ===========================================================================
namespace {

const QString kDateFmt = QStringLiteral("dd/MM/yyyy");

// couleur de fond / texte de la colonne « Statut »
QPair<QColor, QColor> statutColors(const QString &type)
{
    if (type == "VIP")     return qMakePair(QColor("#F3E3C3"), QColor("#A67C2E"));
    if (type == "Premium") return qMakePair(QColor("#C9E2F5"), QColor("#1A5A8A"));
    return qMakePair(QColor("#DBEAF7"), QColor("#2A6496"));
}

int typeRank(const QString &t)
{
    if (t == "VIP") return 0;
    if (t == "Premium") return 1;
    return 2;
}

// couleur de fond / texte du statut d'une commande
QPair<QColor, QColor> cmdColors(const QString &statut)
{
    if (statut == QString::fromUtf8("Livrée"))
        return qMakePair(QColor("#DDF1E4"), QColor("#1E7B3F"));
    if (statut == "En cours")
        return qMakePair(QColor("#DBEAF7"), QColor("#2A6496"));
    if (statut == "En attente")
        return qMakePair(QColor("#F3E3C3"), QColor("#A67C2E"));
    return qMakePair(QColor("#F8DDDD"), QColor("#B03A3A")); // Annulée
}

} // namespace

void SmartMarket::initClient()
{
    
    ui->btnMenuClient->hide();     // le menu latéral est celui de SmartMarket

    auto S = [](const char *s) { return QString::fromUtf8(s); };
    clientsClient = {
        { S("CL001"), S("Ben Ali"),  S("Sofia"),   S("+216 98 765 432"), S("sofia.benali@email.com"),
          S("12 Rue de la Liberté, Tunis"), S("VIP"), S("15/03/2024"),
          S("Cliente fidèle, préfère les robes de soirée et les créations sur mesure.") },
        { S("CL002"), S("Trabelsi"), S("Amine"),   S("+216 92 123 456"), "", "", S("Standard"), S("22/04/2024"), "" },
        { S("CL003"), S("Hmidi"),    S("Salma"),   S("+216 95 987 654"), "", "", S("VIP"),      S("10/05/2024"), "" },
        { S("CL004"), S("Saidi"),    S("Yassine"), S("+216 93 456 789"), "", "", S("Premium"),  S("18/06/2024"), "" },
        { S("CL005"), S("Mansouri"), S("Rania"),   S("+216 22 334 455"), "", "", S("Standard"), S("27/07/2024"), "" },
        { S("CL006"), S("Mejri"),    S("Lilia"),   S("+216 95 667 788"), "", "", S("Premium"),  S("03/08/2024"), "" },
    };

    // --- historique des commandes (données d'exemple)
    const QString livree  = S("Livrée");
    const QString annulee = S("Annulée");
    commandesClient = {
        { "CMD-1042", "CL001", "12/09/2024", S("Robe de soirée sur mesure (satin bleu nuit)"), "En cours", 1850.0 },
        { "CMD-1019", "CL001", "02/07/2024", S("Tailleur : veston + pantalon"), livree, 980.0 },
        { "CMD-0987", "CL001", "21/05/2024", S("Retouches robe de cocktail"), livree, 120.0 },
        { "CMD-0954", "CL001", "30/03/2024", S("Robe de mariée (essayage)"), annulee, 2400.0 },
        { "CMD-1051", "CL002", "20/09/2024", S("Chemise sur mesure"), "En attente", 180.0 },
        { "CMD-1030", "CL002", "15/08/2024", S("Costume 2 pièces"), livree, 1200.0 },
        { "CMD-1038", "CL003", "03/09/2024", S("Robe de soirée"), "En cours", 1450.0 },
        { "CMD-1025", "CL003", "28/07/2024", S("Abaya brodée"), livree, 650.0 },
        { "CMD-1002", "CL003", "12/06/2024", S("Jupe plissée"), livree, 240.0 },
        { "CMD-1044", "CL004", "14/09/2024", S("Costume de mariage"), "En cours", 2100.0 },
        { "CMD-1033", "CL005", "25/08/2024", S("Ensemble casual"), livree, 520.0 },
        { "CMD-1047", "CL006", "16/09/2024", S("Retouches robe"), "En attente", 90.0 },
        { "CMD-1011", "CL006", "12/08/2024", S("Robe cocktail"), livree, 780.0 },
    };

    {
        QHeaderView *ch = ui->tableCommandesClient->horizontalHeader();
        ch->setSectionResizeMode(QHeaderView::ResizeToContents);
        ch->setSectionResizeMode(2, QHeaderView::Stretch);
        ch->setStretchLastSection(false);
        ui->tableCommandesClient->verticalHeader()->setDefaultSectionSize(34);
    }
    connect(ui->comboStatutCmdClient, qOverload<int>(&QComboBox::currentIndexChanged),
            this, [this](int) { refreshHistoriqueClient(); });
    connect(ui->btnVoirCommandesClient, &QPushButton::clicked, this, [this] {
        on_btnMCommande_clicked();          // affiche la page Commandes
    });

    // --- vérification des images intégrées (resources.qrc)
    QTimer::singleShot(0, this, [this] {
        const QStringList needed = {
            ":/images/sidebar.png", ":/images/banner.png", ":/images/logo.png",
            ":/images/stats_repartition.png",
            ":/images/stats_evolution.png", ":/images/stats_top5.png",
            ":/icons/clients_off.png", ":/icons/clients_on.png"
        };
        QStringList missing;
        for (const QString &p : needed)
            if (!QFile::exists(p)) missing << p;
        if (!missing.isEmpty()) {
            QMessageBox::warning(ui->PClient, "Images manquantes",
                "Les images ne sont pas intégrées au programme :\n" + missing.join("\n") +
                "\n\nVérifiez que les dossiers « images » et « icons » sont bien présents "
                "à côté de resources.qrc, puis :\nCompilation > Nettoyer tout, "
                "Exécuter qmake, puis Reconstruire.");
        }
    });

    QHeaderView *hh = ui->tableClientsClient->horizontalHeader();
    hh->setSectionResizeMode(QHeaderView::Stretch);
    hh->setSectionResizeMode(0, QHeaderView::ResizeToContents); // ID
    hh->setSectionResizeMode(4, QHeaderView::ResizeToContents); // Date
    hh->setMinimumHeight(44);
    ui->tableClientsClient->verticalHeader()->setDefaultSectionSize(40);

    // --- boutons du formulaire
    connect(ui->btnAjouterClient,   &QPushButton::clicked, this, &SmartMarket::ajouterClient);
    connect(ui->btnModifierClient,  &QPushButton::clicked, this, &SmartMarket::modifierClient);
    connect(ui->btnSupprimerClient, &QPushButton::clicked, this, &SmartMarket::supprimerClient);
    connect(ui->btnAfficherClient,  &QPushButton::clicked, this, &SmartMarket::afficherClient);
    connect(ui->btnReinitClient,    &QPushButton::clicked, this, &SmartMarket::reinitialiserClient);
    connect(ui->btnExportClient,    &QPushButton::clicked, this, &SmartMarket::exporterPdfClient);

    // --- recherche / tri / sélection
    connect(ui->searchListeClient,  &QLineEdit::textChanged, this, &SmartMarket::onFilterChangedClient);
    connect(ui->searchGlobalClient, &QLineEdit::textChanged, this, &SmartMarket::onFilterChangedClient);
    connect(ui->comboTrierClient, qOverload<int>(&QComboBox::currentIndexChanged),
            this, &SmartMarket::onFilterChangedClient);
    connect(ui->tableClientsClient, &QTableWidget::itemSelectionChanged,
            this, &SmartMarket::remplirFormulaireClient);
    connect(ui->tableClientsClient, &QTableWidget::cellDoubleClicked,
            this, [this](int, int) { afficherClient(); });

    // --- barre du haut
    connect(ui->btnNotifClient, &QPushButton::clicked, this, [this] {
        QMessageBox::information(ui->PClient, "Notifications", "Vous avez 3 nouvelles notifications.");
    });

    // --- bannière : l'image remplit la largeur sans déformation
    bannerPixClient = QPixmap(":/images/banner.png");
    ui->bannerImageClient->installEventFilter(this);
    updateBannerClient();

    // --- statistiques : image qui s'adapte à la largeur (sans déformation)
    pixRepartitionClient = QPixmap(":/images/stats_repartition.png");
    pixEvolutionClient  = QPixmap(":/images/stats_evolution.png");
    pixTop5Client       = QPixmap(":/images/stats_top5.png");
    ui->statsPanelClient->installEventFilter(this);
    updateStatsClient();

    refreshTableClient(QStringLiteral("CL001"));
}


void SmartMarket::updateBannerClient()
{
    const QSize s = ui->bannerImageClient->size();
    if (bannerPixClient.isNull() || s.isEmpty()) return;
    // KeepAspectRatioByExpanding : toute la largeur, léger recadrage haut/bas
    ui->bannerImageClient->setPixmap(
        bannerPixClient.scaled(s, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation));
}

void SmartMarket::updateStatsClient()
{
    // Les 3 photos (déclarées dans mainwindow.ui) gardent les proportions d'origine
    // (378 : 417 : 384 px, hauteur 186 px) et s'adaptent à la largeur de la page.
    const int gap = ui->statsLayoutClient->spacing();
    const int w = ui->statsPanelClient->width();
    const double k = double(w - 2 * gap) / (378 + 417 + 384);
    if (k <= 0.1) return;
    const int h = qRound(186 * k);
    if (ui->statsPanelClient->height() != h)
        ui->statsPanelClient->setFixedHeight(h);

    auto place = [k, h](QLabel *lab, const QPixmap &pix, int nativeW) {
        const QSize sz(qRound(nativeW * k), h);
        lab->setFixedSize(sz);
        lab->setPixmap(pix.scaled(sz, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    };
    place(ui->statsRepartitionClient, pixRepartitionClient, 378);
    place(ui->statsEvolutionClient,   pixEvolutionClient,   417);
    place(ui->statsTop5Client,        pixTop5Client,        384);
}

// ------------------------------------------------------------------ utilitaires

void SmartMarket::statusClient(const QString &text)
{
    statusBar()->showMessage(text, 4000);
}

int SmartMarket::selectedIndexClient() const
{
    const QModelIndexList rows = ui->tableClientsClient->selectionModel()->selectedRows();
    return rows.isEmpty() ? -1 : rows.first().row();
}

QString SmartMarket::selectedIdClient() const
{
    const int i = selectedIndexClient();
    return (i >= 0 && i < viewClient.size()) ? viewClient[i].id : QString();
}

int SmartMarket::indexOfIdClient(const QString &id) const
{
    for (int i = 0; i < clientsClient.size(); ++i)
        if (clientsClient[i].id == id) return i;
    return -1;
}

QString SmartMarket::nextIdClient() const
{
    static const QRegularExpression re("^CL(\\d+)$");
    int maxNum = 0;
    for (const Client &c : clientsClient) {
        const QRegularExpressionMatch m = re.match(c.id);
        if (m.hasMatch()) maxNum = qMax(maxNum, m.captured(1).toInt());
    }
    return QString("CL%1").arg(maxNum + 1, 3, 10, QChar('0'));
}

// ------------------------------------------------------------------ tableau

QVector<Client> SmartMarket::sortedFilteredClient() const
{
    const QStringList terms = (ui->searchListeClient->text() + " " + ui->searchGlobalClient->text())
                                  .toLower().split(' ', Qt::SkipEmptyParts);
    QVector<Client> data;
    for (const Client &c : clientsClient) {
        const QString hay = (c.id + " " + c.nom + " " + c.prenom + " " + c.tel + " "
                             + c.date + " " + c.type).toLower();
        bool ok = true;
        for (const QString &t : terms)
            if (!hay.contains(t)) { ok = false; break; }
        if (ok) data.append(c);
    }

    switch (ui->comboTrierClient->currentIndex()) {
    case 0: // date d'inscription
        std::sort(data.begin(), data.end(), [](const Client &a, const Client &b) {
            return QDate::fromString(a.date, kDateFmt) < QDate::fromString(b.date, kDateFmt);
        });
        break;
    case 1: // nom
        std::sort(data.begin(), data.end(), [](const Client &a, const Client &b) {
            return QString::localeAwareCompare(a.nom + a.prenom, b.nom + b.prenom) < 0;
        });
        break;
    default: // type de client
        std::sort(data.begin(), data.end(), [](const Client &a, const Client &b) {
            return typeRank(a.type) != typeRank(b.type) ? typeRank(a.type) < typeRank(b.type)
                                                         : a.nom < b.nom;
        });
    }
    return data;
}

void SmartMarket::refreshTableClient(const QString &keepId)
{
    viewClient = sortedFilteredClient();

    {
        QSignalBlocker blocker(ui->tableClientsClient);
        ui->tableClientsClient->setRowCount(viewClient.size());
        for (int r = 0; r < viewClient.size(); ++r) {
            const Client &c = viewClient[r];
            const QStringList cells = { c.id, c.nom, c.prenom, c.tel, c.date, c.type };
            for (int col = 0; col < cells.size(); ++col) {
                auto *item = new QTableWidgetItem(cells[col]);
                if (col == 5) {
                    const auto colors = statutColors(c.type);
                    item->setBackground(colors.first);
                    item->setForeground(colors.second);
                    item->setTextAlignment(Qt::AlignCenter);
                }
                ui->tableClientsClient->setItem(r, col, item);
            }
            auto *actions = new QTableWidgetItem(QString::fromUtf8("👁   ✎"));
            actions->setTextAlignment(Qt::AlignCenter);
            ui->tableClientsClient->setItem(r, 6, actions);
        }
    }

    bool found = false;
    for (int r = 0; r < viewClient.size(); ++r) {
        if (viewClient[r].id == keepId) {
            ui->tableClientsClient->selectRow(r);
            found = true;
            break;
        }
    }
    if (!found) ui->tableClientsClient->clearSelection();
    refreshHistoriqueClient();
}

void SmartMarket::onFilterChangedClient()
{
    refreshTableClient(selectedIdClient());
}

// ------------------------------------------------------------------ historique

void SmartMarket::refreshHistoriqueClient()
{
    QTableWidget *t = ui->tableCommandesClient;
    t->setRowCount(0);

    const int idx = indexOfIdClient(selectedIdClient());
    if (idx < 0) {
        ui->groupHistoriqueClient->setTitle(QString::fromUtf8("🧾  Historique des commandes"));
        ui->lblNbCommandesClient->setText("Commandes : -");
        ui->lblTotalDepenseClient->setText(QString::fromUtf8("Total dépensé : -"));
        ui->lblDerniereCommandeClient->setText(QString::fromUtf8("Dernière commande : -"));
        return;
    }
    const Client &cl = clientsClient[idx];
    ui->groupHistoriqueClient->setTitle(
        QString::fromUtf8("🧾  Historique des commandes — ") + cl.prenom + " " + cl.nom);

    // commandes du client (plus récentes d'abord)
    QVector<Commande> all;
    for (const Commande &cmd : commandesClient)
        if (cmd.clientId == cl.id) all.append(cmd);
    std::sort(all.begin(), all.end(), [](const Commande &a, const Commande &b) {
        return QDate::fromString(a.date, kDateFmt) > QDate::fromString(b.date, kDateFmt);
    });

    // résumé : toutes les commandes, total hors commandes annulées
    double total = 0.0;
    for (const Commande &cmd : all)
        if (cmd.statut != QString::fromUtf8("Annulée")) total += cmd.montant;
    const QLocale fr(QLocale::French);
    ui->lblNbCommandesClient->setText(QString("Commandes : %1").arg(all.size()));
    ui->lblTotalDepenseClient->setText(QString::fromUtf8("Total dépensé : ") + fr.toString(total, 'f', 2) + " DT");
    ui->lblDerniereCommandeClient->setText(QString::fromUtf8("Dernière commande : ")
                                     + (all.isEmpty() ? QString("-") : all.first().date));

    // tableau (filtré par statut)
    const bool filtre = ui->comboStatutCmdClient->currentIndex() > 0;
    const QString statut = ui->comboStatutCmdClient->currentText();
    for (const Commande &cmd : all) {
        if (filtre && cmd.statut != statut) continue;
        const int r = t->rowCount();
        t->insertRow(r);
        t->setItem(r, 0, new QTableWidgetItem(cmd.numero));
        t->setItem(r, 1, new QTableWidgetItem(cmd.date));
        t->setItem(r, 2, new QTableWidgetItem(cmd.designation));
        auto *m = new QTableWidgetItem(fr.toString(cmd.montant, 'f', 2) + " DT");
        m->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        t->setItem(r, 3, m);
        auto *st = new QTableWidgetItem(cmd.statut);
        const auto colors = cmdColors(cmd.statut);
        st->setBackground(colors.first);
        st->setForeground(colors.second);
        st->setTextAlignment(Qt::AlignCenter);
        t->setItem(r, 4, st);
    }
}

// ------------------------------------------------------------------ formulaire

Client SmartMarket::formDataClient() const
{
    Client c;
    c.id        = ui->editIdClient->text().trimmed();
    c.nom       = ui->editNomClient->text().trimmed();
    c.prenom    = ui->editPrenomClient->text().trimmed();
    c.tel       = ui->editTelClient->text().trimmed();
    c.email     = ui->editEmailClient->text().trimmed();
    c.adresse   = ui->editAdresseClient->text().trimmed();
    c.type      = ui->comboTypeClient->currentText();
    c.date      = ui->dateInscriptionClient->date().toString(kDateFmt);
    c.remarques = ui->editRemarquesClient->toPlainText().trimmed();
    return c;
}

void SmartMarket::remplirFormulaireClient()
{
    const int i = selectedIndexClient();
    if (i < 0 || i >= viewClient.size()) { refreshHistoriqueClient(); return; }
    const Client &c = viewClient[i];
    ui->editIdClient->setText(c.id);
    ui->editNomClient->setText(c.nom);
    ui->editPrenomClient->setText(c.prenom);
    ui->editTelClient->setText(c.tel);
    ui->editEmailClient->setText(c.email);
    ui->editAdresseClient->setText(c.adresse);
    ui->comboTypeClient->setCurrentText(c.type);
    ui->dateInscriptionClient->setDate(QDate::fromString(c.date, kDateFmt));
    ui->editRemarquesClient->setPlainText(c.remarques);
    refreshHistoriqueClient();
}

bool SmartMarket::validerClient(const Client &c)
{
    if (c.nom.isEmpty() || c.prenom.isEmpty()) {
        QMessageBox::warning(ui->PClient, "Champs manquants", "Le nom et le prénom sont obligatoires.");
        return false;
    }
    if (c.tel.isEmpty()) {
        QMessageBox::warning(ui->PClient, "Champs manquants", "Le numéro de téléphone est obligatoire.");
        return false;
    }
    return true;
}

// ------------------------------------------------------------------ actions

void SmartMarket::reinitialiserClient()
{
    ui->tableClientsClient->clearSelection();
    ui->editIdClient->setText(nextIdClient());
    ui->editNomClient->clear();
    ui->editPrenomClient->clear();
    ui->editTelClient->clear();
    ui->editEmailClient->clear();
    ui->editAdresseClient->clear();
    ui->comboTypeClient->setCurrentText("Standard");
    ui->dateInscriptionClient->setDate(QDate::currentDate());
    ui->editRemarquesClient->clear();
    ui->editNomClient->setFocus();
}

void SmartMarket::ajouterClient()
{
    Client c = formDataClient();
    if (!validerClient(c)) return;
    if (c.id.isEmpty() || indexOfIdClient(c.id) >= 0)
        c.id = nextIdClient();
    clientsClient.append(c);
    refreshTableClient(c.id);
    statusClient("Client " + c.id + " ajouté.");
}

void SmartMarket::modifierClient()
{
    const QString oldId = selectedIdClient();
    if (oldId.isEmpty()) {
        QMessageBox::warning(ui->PClient, "Aucune sélection", "Sélectionnez d'abord un client dans la liste.");
        return;
    }
    Client c = formDataClient();
    if (!validerClient(c)) return;
    if (c.id != oldId && indexOfIdClient(c.id) >= 0) {
        QMessageBox::warning(ui->PClient, "ID déjà utilisé", "L'ID " + c.id + " existe déjà.");
        return;
    }
    const int idx = indexOfIdClient(oldId);
    if (idx >= 0) clientsClient[idx] = c;
    refreshTableClient(c.id);
    statusClient("Client " + c.id + " modifié.");
}

void SmartMarket::supprimerClient()
{
    const QString id = selectedIdClient();
    const int idx = indexOfIdClient(id);
    if (idx < 0) {
        QMessageBox::warning(ui->PClient, "Aucune sélection", "Sélectionnez d'abord un client dans la liste.");
        return;
    }
    const Client &c = clientsClient[idx];
    const auto rep = QMessageBox::question(ui->PClient, "Confirmation",
        QString("Supprimer %1 %2 (%3) ?").arg(c.prenom, c.nom, c.id),
        QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    if (rep != QMessageBox::Yes) return;
    clientsClient.remove(idx);
    refreshTableClient(QString());
    statusClient("Client supprimé.");
}

void SmartMarket::afficherClient()
{
    const int idx = indexOfIdClient(selectedIdClient());
    if (idx < 0) {
        QMessageBox::warning(ui->PClient, "Aucune sélection", "Sélectionnez d'abord un client dans la liste.");
        return;
    }
    const Client &c = clientsClient[idx];
    const QString dash = "-";
    QMessageBox::information(ui->PClient, "Client " + c.id,
        QString("%1 %2\n\nTéléphone : %3\nEmail : %4\nAdresse : %5\nType : %6\n"
                "Inscrit le : %7\n\nRemarques : %8")
            .arg(c.prenom, c.nom, c.tel,
                 c.email.isEmpty() ? dash : c.email,
                 c.adresse.isEmpty() ? dash : c.adresse,
                 c.type, c.date,
                 c.remarques.isEmpty() ? dash : c.remarques));
}

void SmartMarket::exporterPdfClient()
{
    const QString path = QFileDialog::getSaveFileName(ui->PClient, "Exporter en PDF", "liste_clients.pdf", "PDF (*.pdf)");
    if (path.isEmpty()) return;

    QString rows;
    for (const Client &c : viewClient)
        rows += "<tr><td>" + c.id.toHtmlEscaped() + "</td><td>" + c.nom.toHtmlEscaped()
              + "</td><td>" + c.prenom.toHtmlEscaped() + "</td><td>" + c.tel.toHtmlEscaped()
              + "</td><td>" + c.date + "</td><td>" + c.type + "</td></tr>";

    const QString html =
        "<h2 style='color:#0b2a4a'>FashioNova - Liste des clients</h2>"
        "<table border='1' cellspacing='0' cellpadding='6' width='100%'>"
        "<tr style='background:#efe3cf'><th>ID</th><th>Nom</th><th>Prénom</th>"
        "<th>Téléphone</th><th>Date d'inscription</th><th>Statut</th></tr>" + rows + "</table>";

    QPdfWriter writer(path);
    QTextDocument doc;
    doc.setHtml(html);
    doc.print(&writer);
    statusClient("PDF exporté : " + path);
}


// ===========================================================================
//  MODULE ARTICLES (page PArticle) — slots copiés du projet « Articles »
// ===========================================================================
// true = interface interactive (boutons, saisie, tri...) ; false = affichage seul (maquette statique)
static const bool INTERACTIF=true;

// Miniature carrée (mise en cache) affichée devant l'article dans la liste
static QIcon thumb(const QString&p){
    static QHash<QString,QIcon> cache;
    if(p.isEmpty())return QIcon();
    if(cache.contains(p))return cache[p];
    QPixmap pm(p);QIcon ic;
    if(!pm.isNull()){pm=pm.scaled(46,46,Qt::KeepAspectRatioByExpanding,Qt::SmoothTransformation);
        ic=QIcon(pm.copy((pm.width()-46)/2,(pm.height()-46)/2,46,46));}
    cache[p]=ic;return ic;}

void SmartMarket::initArticle()
{


    // Widgets personnalisés (promus dans Designer)
    ui->bannerArticle->setTexts("Gestion des Articles","Du stock à la vente",{"Un stock bien tenu,","c'est une vente","qui se prépare."});
    ui->chDonutArticle->setMode(ChartWidget::Donut);
    ui->chLineArticle->setMode(ChartWidget::Line);
    ui->chBarsArticle->setMode(ChartWidget::Bars);

    AArticle={{"ART001","Soie naturelle","Tissus",45,120,QDate(2026,1,12),"Disponible","Soie fluide pour robes de soirée",85,":/images/art001.png"},
       {"ART002","Dentelle française","Tissus",68.5,35,QDate(2026,2,8),"Disponible","Dentelle fine ivoire",60,":/images/art002.png"},
       {"ART003","Fil polyester noir","Fils",2.9,0,QDate(2026,2,20),"Rupture de stock","Cône de 1000 m",90,":/images/art003.png"},
       {"ART004","Boutons nacre","Boutons",0.8,900,QDate(2026,3,5),"Disponible","Lot de 100 pièces",40,":/images/art004.png"},
       {"ART005","Fermeture invisible","Fermetures",1.5,240,QDate(2026,3,18),"Disponible","Longueur 50 cm",30,":/images/art005.png"},
       {"ART006","Satin duchesse","Tissus",39,18,QDate(2026,4,2),"Disponible","Satin épais brillant",55,":/images/art006.png"},
       {"ART007","Ruban velours","Accessoires",6.2,75,QDate(2026,4,25),"Archivé","Ancienne collection",5,":/images/art007.png"},
       {"ART008","Fil de soie","Fils",4.4,310,QDate(2026,5,14),"Disponible","Fil à broder",70,":/images/art008.png"}};

    ui->dDateArticle->setDate(QDate::currentDate());
    ui->cSortArticle->setCurrentIndex(6);   // par défaut : articles classés selon la demande
    ui->tbArticle->horizontalHeader()->setSectionResizeMode(2,QHeaderView::Stretch);
    ui->tbArticle->setIconSize(QSize(46,46));ui->tbArticle->setColumnWidth(0,62);
    ui->tbArticle->setTextElideMode(Qt::ElideRight);

    connect(ui->btnAddArticle,&QPushButton::clicked,this,&SmartMarket::onAddArticle);
    connect(ui->btnModArticle,&QPushButton::clicked,this,&SmartMarket::onModArticle);
    connect(ui->btnImgArticle,&QPushButton::clicked,this,&SmartMarket::onImgArticle);
    connect(ui->btnDelArticle,&QPushButton::clicked,this,&SmartMarket::onDelArticle);
    connect(ui->btnViewArticle,&QPushButton::clicked,this,&SmartMarket::onViewArticle);
    connect(ui->btnResetArticle,&QPushButton::clicked,this,&SmartMarket::resetArticle);
    connect(ui->btnPdfArticle,&QPushButton::clicked,this,&SmartMarket::exportPdfArticle);
    connect(ui->searchArticle,&QLineEdit::textChanged,this,&SmartMarket::refreshArticle);
    connect(ui->cSortArticle,QOverload<int>::of(&QComboBox::currentIndexChanged),this,&SmartMarket::refreshArticle);
    connect(ui->gSearchArticle,&QLineEdit::textChanged,ui->searchArticle,&QLineEdit::setText);
    connect(ui->tbArticle,&QTableWidget::itemSelectionChanged,this,&SmartMarket::onRowArticle);
    refreshArticle();

    if(!INTERACTIF){
        // Affichage seul : le formulaire montre un exemple, plus rien ne réagit à la souris ni au clavier
        fillArticle(AArticle[0]);
        ui->tbArticle->setSelectionMode(QAbstractItemView::NoSelection);
        const auto ws=ui->PArticle->findChildren<QWidget*>();
        for(QWidget*w:ws){
            w->setFocusPolicy(Qt::NoFocus);
            w->setContextMenuPolicy(Qt::NoContextMenu);
            w->setAttribute(Qt::WA_TransparentForMouseEvents,true);
        }
        ui->PArticle->setAttribute(Qt::WA_TransparentForMouseEvents,true);
        ui->PArticle->setFocusPolicy(Qt::NoFocus);
    }
}

void SmartMarket::refreshArticle(){
    QString q=ui->searchArticle->text().toLower();viewArticle.clear();
    for(int i=0;i<AArticle.size();++i){auto&a=AArticle[i];if((a.id+a.nom+a.cat+a.st+a.desc).toLower().contains(q))viewArticle<<i;}
    int k=ui->cSortArticle->currentIndex();
    if(k>0)std::sort(viewArticle.begin(),viewArticle.end(),[&](int x,int y){auto&a=AArticle[x];auto&b=AArticle[y];
        switch(k){case 1:return a.nom<b.nom;case 2:return a.cat<b.cat;case 3:return a.prix<b.prix;case 4:return a.qte<b.qte;case 6:return a.dem>b.dem;default:return a.date<b.date;}});
    QSignalBlocker sb(ui->tbArticle);ui->tbArticle->setRowCount(viewArticle.size());
    for(int r=0;r<viewArticle.size();++r){
        auto&a=AArticle[viewArticle[r]];
        QString niv=a.dem>=70?"Forte":a.dem>=40?"Moyenne":"Faible";
        QStringList v{QString(),a.id,a.nom,a.cat,QString::number(a.prix,'f',3)+" DT",QString::number(a.qte),QString("%1 · %2").arg(a.dem).arg(niv),a.st};
        for(int c=0;c<8;++c){
            auto it=new QTableWidgetItem(v[c]);
            if(c==0){it->setIcon(thumb(a.img));it->setTextAlignment(Qt::AlignCenter);if(a.img.isEmpty())it->setText("—");}
            if(c==6){bool f=a.dem>=70,m=a.dem>=40&&!f;
                it->setBackground(f?QColor("#fde9cf"):m?QColor("#e3ecf8"):QColor("#e7e9ee"));
                it->setForeground(f?QColor("#a8591a"):m?QColor("#2f5d9e"):QColor("#5b6675"));it->setTextAlignment(Qt::AlignCenter);}
            if(c==7){bool ok=a.st=="Disponible",ar=a.st=="Archivé";it->setBackground(ok?QColor("#dff1e2"):ar?QColor("#e7e9ee"):QColor("#f8dcdc"));it->setForeground(ok?QColor("#2e7d4f"):ar?QColor("#5b6675"):QColor("#b03a3a"));it->setTextAlignment(Qt::AlignCenter);}
            ui->tbArticle->setItem(r,c,it);}
        if(viewArticle[r]==curArticle)ui->tbArticle->selectRow(r);
    }
    statsArticle();
}

void SmartMarket::statsArticle(){
    QMap<QString,int> c;for(auto&a:AArticle)c[a.cat]++;
    QVector<QPair<QString,double>> d;for(auto k:c.keys())d<<qMakePair(k,double(c[k]));ui->chDonutArticle->setData(d);
    QVector<QPair<QString,double>> m;const char*mn[]={"Jan","Fév","Mar","Avr","Mai","Juin"};
    for(int i=0;i<6;++i){int n=0;for(auto&a:AArticle)if(a.date.month()==i+1)n++;m<<qMakePair(QString(mn[i]),double(n));}ui->chLineArticle->setData(m);
    auto s=AArticle;std::sort(s.begin(),s.end(),[](const Article&x,const Article&y){return x.qte>y.qte;});
    QVector<QPair<QString,double>> t;for(int i=0;i<qMin(5,int(s.size()));++i)t<<qMakePair(s[i].nom,double(s[i].qte));ui->chBarsArticle->setData(t);
}

bool SmartMarket::readFormArticle(Article&a){
    a={ui->eIdArticle->text().trimmed(),ui->eNomArticle->text().trimmed(),ui->cCatArticle->currentText(),ui->sPrixArticle->value(),ui->sQteArticle->value(),ui->dDateArticle->date(),ui->cStArticle->currentText(),ui->eDescArticle->toPlainText(),ui->sDemArticle->value(),imgArticle};
    if(a.id.isEmpty()||a.nom.isEmpty()){QMessageBox::warning(ui->PArticle,"Champs requis","Renseignez l'ID et le nom de l'article.");return false;}
    return true;
}
void SmartMarket::fillArticle(const Article&a){
    ui->eIdArticle->setText(a.id);ui->eNomArticle->setText(a.nom);ui->cCatArticle->setCurrentText(a.cat);ui->sPrixArticle->setValue(a.prix);ui->sQteArticle->setValue(a.qte);
    ui->dDateArticle->setDate(a.date);ui->cStArticle->setCurrentText(a.st);ui->eDescArticle->setPlainText(a.desc);ui->sDemArticle->setValue(a.dem);setImgArticle(a.img);
}
void SmartMarket::setImgArticle(const QString&p){
    imgArticle=p;QPixmap pm(p);
    if(p.isEmpty()||pm.isNull()){imgArticle.clear();ui->lImgArticle->setPixmap(QPixmap());ui->lImgArticle->setText("Aucune\nimage");}
    else ui->lImgArticle->setPixmap(pm.scaled(ui->lImgArticle->size()-QSize(4,4),Qt::KeepAspectRatio,Qt::SmoothTransformation));
}
void SmartMarket::onImgArticle(){
    QString f=QFileDialog::getOpenFileName(ui->PArticle,"Choisir l'image de l'article",QString(),"Images (*.png *.jpg *.jpeg *.bmp *.webp)");
    if(!f.isEmpty())setImgArticle(f);
}
void SmartMarket::onRowArticle(){
    auto s=ui->tbArticle->selectionModel()->selectedRows();if(s.isEmpty())return;
    curArticle=viewArticle[s[0].row()];fillArticle(AArticle[curArticle]);
}
void SmartMarket::onAddArticle(){
    Article a;if(!readFormArticle(a))return;
    for(auto&x:AArticle)if(x.id==a.id){QMessageBox::warning(ui->PArticle,"Doublon","Cet ID existe déjà.");return;}
    if(imgArticle.isEmpty()){   // clic sur Ajouter -> on propose d'insérer l'image de l'article
        QString f=QFileDialog::getOpenFileName(ui->PArticle,"Insérer l'image de l'article",QString(),"Images (*.png *.jpg *.jpeg *.bmp *.webp)");
        if(!f.isEmpty()){setImgArticle(f);a.img=f;}
    }
    AArticle<<a;curArticle=AArticle.size()-1;refreshArticle();statusBar()->showMessage("Article ajouté",3000);
}
void SmartMarket::onModArticle(){
    if(curArticle<0){QMessageBox::information(ui->PArticle,"Modifier","Sélectionnez un article dans la liste.");return;}
    Article a;if(!readFormArticle(a))return;AArticle[curArticle]=a;refreshArticle();statusBar()->showMessage("Article modifié",3000);
}
void SmartMarket::onDelArticle(){
    if(curArticle<0){QMessageBox::information(ui->PArticle,"Supprimer","Sélectionnez un article dans la liste.");return;}
    if(QMessageBox::question(ui->PArticle,"Supprimer","Supprimer l'article « "+AArticle[curArticle].nom+" » ?")!=QMessageBox::Yes)return;
    AArticle.remove(curArticle);curArticle=-1;refreshArticle();statusBar()->showMessage("Article supprimé",3000);
}
void SmartMarket::onViewArticle(){
    if(curArticle<0){QMessageBox::information(ui->PArticle,"Afficher","Sélectionnez un article dans la liste.");return;}
    auto&a=AArticle[curArticle];
    QMessageBox b(ui->PArticle);b.setWindowTitle("Article "+a.id);
    b.setText(QString("<b>%1</b><br>Catégorie : %2<br>Prix : %3 DT<br>Stock : %4<br>Demande : %5<br>Ajouté le : %6<br>Statut : %7<br><br>%8")
        .arg(a.nom,a.cat).arg(a.prix,0,'f',3).arg(a.qte).arg(a.dem).arg(a.date.toString("dd/MM/yyyy"),a.st,a.desc.toHtmlEscaped()));
    QPixmap pm(a.img);if(!pm.isNull())b.setIconPixmap(pm.scaled(140,140,Qt::KeepAspectRatio,Qt::SmoothTransformation));
    b.exec();
}
void SmartMarket::resetArticle(){
    ui->eIdArticle->clear();ui->eNomArticle->clear();ui->cCatArticle->setCurrentIndex(0);ui->sPrixArticle->setValue(0);ui->sQteArticle->setValue(0);ui->dDateArticle->setDate(QDate::currentDate());
    ui->cStArticle->setCurrentIndex(0);ui->eDescArticle->clear();ui->sDemArticle->setValue(0);setImgArticle(QString());curArticle=-1;ui->tbArticle->clearSelection();
}
void SmartMarket::exportPdfArticle(){
    QString path=QFileDialog::getSaveFileName(ui->PArticle,"Exporter en PDF","articles.pdf","PDF (*.pdf)");if(path.isEmpty())return;
    QString h="<h2 style='color:#0a2a4a'>Fashionova – Liste des articles</h2><table border='1' cellspacing='0' cellpadding='5' width='100%'>"
              "<tr bgcolor='#f1e4d3'><th>ID</th><th>Nom</th><th>Catégorie</th><th>Prix</th><th>Stock</th><th>Demande</th><th>Statut</th></tr>";
    for(int i:viewArticle){auto&a=AArticle[i];h+=QString("<tr><td>%1</td><td>%2</td><td>%3</td><td>%4 DT</td><td>%5</td><td>%6</td><td>%7</td></tr>")
        .arg(a.id,a.nom.toHtmlEscaped(),a.cat).arg(a.prix,0,'f',3).arg(a.qte).arg(a.dem).arg(a.st);}
    h+="</table>";
    QPdfWriter w(path);w.setPageSize(QPageSize(QPageSize::A4));QTextDocument d;d.setHtml(h);d.print(&w);
    statusBar()->showMessage("PDF exporté",3000);
}


// ===========================================================================
//  MODULE MAQUETTES (page PMaquette) — slots copiés du projet « Maquettes »
// ===========================================================================
namespace {
const QColor kNavy("#0B2A45");
const QColor kSable("#8C7B66");

QString dossierImages() { return QStringLiteral(":/maquettes/images/"); }

QString fichierAvant(const QString &type)   { return dossierImages() + type.toLower() + "_avant.png"; }
QString fichierValidee(const QString &type) { return dossierImages() + type.toLower() + "_validee.gif"; }

struct StyleStatut { QColor fond; QColor texte; QColor pastille; };
StyleStatut styleStatut(const QString &statut)
{
    if (statut.startsWith("Valid")) return { QColor("#DDF1E3"), QColor("#2E7D4F"), QColor("#2E9B5F") };
    if (statut == "En attente")     return { QColor("#E8ECF2"), QColor("#4A5568"), QColor("#7D8597") };
    return                                { QColor("#FBEBD3"), QColor("#B26B12"), QColor("#E59A2F") };   // En cours
}
} // namespace

// ============================================================================ ctor
void SmartMarket::initMaquette()
{
    

    // Le menu latéral et le bouton « hamburger » appartiennent à la fenêtre principale STYLEHUB
    ui->btnMenuMaquette->hide();

    appliquerStyleMaquette();
    installerIconesMaquette();

    // --- champs du formulaire
    ui->dateCreationMaquette->setDisplayFormat("dd/MM/yyyy");
    ui->dateCreationMaquette->setCalendarPopup(true);
    ui->dateCreationMaquette->setMaximumDate(QDate::currentDate());
    ui->dateCreationMaquette->setDate(QDate::currentDate());
    ui->comboTriMaquette->setCurrentIndex(0);

    // --- tableau
    auto *t = ui->tableMaquettesMaquette;
    t->setColumnCount(6);
    t->setHorizontalHeaderLabels({ "ID", "Nom", "Type", "Date de création", "Statut", "Aperçu" });
    t->verticalHeader()->hide();
    t->verticalHeader()->setDefaultSectionSize(46);
    t->horizontalHeader()->setFixedHeight(38);
    t->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    t->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    t->horizontalHeader()->setHighlightSections(false);
    t->setFocusPolicy(Qt::NoFocus);
    t->setFixedHeight(38 + PAR_PAGEMaquette * 46 + 2);

    // --- signaux
    connect(ui->btnAjouterMaquette,      &QPushButton::clicked, this, &SmartMarket::onAjouterMaquette);
    connect(ui->btnModifierMaquette,     &QPushButton::clicked, this, &SmartMarket::onModifierMaquette);
    connect(ui->btnSupprimerMaquette,    &QPushButton::clicked, this, &SmartMarket::onSupprimerMaquette);
    connect(ui->btnAfficherMaquette,     &QPushButton::clicked, this, &SmartMarket::onAfficherMaquette);
    connect(ui->btnReinitialiserMaquette,&QPushButton::clicked, this, &SmartMarket::onReinitialiserMaquette);
    connect(ui->btnExporterPdfMaquette,  &QPushButton::clicked, this, &SmartMarket::onExporterPdfMaquette);
    connect(ui->btnPlusMaquette,         &QToolButton::clicked, this, [this] {
        if (m_videoCouranteMaquette.isEmpty()) onChoisirVideoMaquette(); else onLireVideoMaquette();
    });
    connect(ui->apercuMaquette->boutonCrayon(), &QToolButton::clicked, this, &SmartMarket::onChoisirVideoMaquette);
    connect(ui->btnClocheMaquette,       &QToolButton::clicked, this, [this] { verifierRetardsMaquette(true); });
    connect(ui->btnCalendrierMaquette,   &QToolButton::clicked, this, [this] {
        ui->dateCreationMaquette->setFocus();
        QKeyEvent press(QEvent::KeyPress, Qt::Key_Down, Qt::AltModifier);
        QApplication::sendEvent(ui->dateCreationMaquette, &press);
    });

    connect(ui->tableMaquettesMaquette, &QTableWidget::cellClicked, this, &SmartMarket::onLigneCliqueeMaquette);
    connect(ui->editRechercheListeMaquette, &QLineEdit::textChanged, this, [this] { m_pageMaquette = 0; rafraichirListeMaquette(); });
    connect(ui->editRechercheGlobaleMaquette, &QLineEdit::textChanged, ui->editRechercheListeMaquette, &QLineEdit::setText);
    connect(ui->comboTriMaquette, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int) { m_pageMaquette = 0; rafraichirListeMaquette(); });

    connect(ui->comboTypeMaquette,   &QComboBox::currentTextChanged, this, [this] { afficherApercuMaquette(ui->comboTypeMaquette->currentText(), ui->comboStatutMaquette->currentText()); });
    connect(ui->comboStatutMaquette, &QComboBox::currentTextChanged, this, [this] { onStatutChangeMaquette(); });

    // Miniatures : image fixe / animation
    connect(ui->mini1Maquette, &Miniature::clique, this, [this] {
        ui->apercuMaquette->afficherImage(QPixmap(fichierAvant(ui->comboTypeMaquette->currentText())));
    });
    connect(ui->mini2Maquette, &Miniature::clique, this, [this] {
        ui->apercuMaquette->afficherAnimation(fichierValidee(ui->comboTypeMaquette->currentText()));
    });

    // Vérification automatique des retards toutes les minutes
    m_timerRetardMaquette = new QTimer(this);
    connect(m_timerRetardMaquette, &QTimer::timeout, this, [this] { rafraichirListeMaquette(); verifierRetardsMaquette(false); });
    m_timerRetardMaquette->start(60 * 1000);

    onReinitialiserMaquette();
    rafraichirToutMaquette();
}

void SmartMarket::onPageAfficheeMaquette()
{
    rafraichirToutMaquette();
    if (!m_alerteAfficheeMaquette)                       // pop-up une seule fois, à la première ouverture du module
        QTimer::singleShot(350, this, [this] { verifierRetardsMaquette(true); });
}

// ============================================================================ style
void SmartMarket::appliquerStyleMaquette()
{
    ui->PMaquette->setStyleSheet(QStringLiteral(R"(
QWidget#PMaquette, QWidget#zoneDroiteMaquette, QWidget#scrollContentsMaquette { background:#F7F3EC; }
QScrollArea#scrollMaquettesMaquette { background:#F7F3EC; border:none; }
QScrollBar:vertical { background:transparent; width:10px; margin:0; }
QScrollBar::handle:vertical { background:#D9CCB4; border-radius:5px; min-height:30px; }
QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height:0; }

QWidget#topbarMaquette { background:#F7F3EC; }
QLineEdit#editRechercheGlobaleMaquette { background:#FFFFFF; border:1px solid #E7DCCB; border-radius:19px; padding:0 14px; color:#1F2A3D; }
QToolButton[role="iconBtn"] { background:transparent; border:none; }
QLabel#labelUserMaquette { font-size:15px; font-weight:600; color:#0B2A45; }

QFrame[role="carte"]   { background:#FFFFFF; border:1px solid #EADFCF; border-radius:12px; }
QFrame[role="entete"]  { background:#EFE3CF; border:none; border-top-left-radius:12px; border-top-right-radius:12px; }
QLabel[role="titreCarte"] { font-size:15px; font-weight:700; color:#0B2A45; background:transparent; }
QWidget[role="corps"]  { background:transparent; }
QLabel[role="labelChamp"] { font-weight:600; color:#0B2A45; background:transparent; }

QFrame[role="champ"]   { background:#FFFFFF; border:1px solid #E1D6C3; border-radius:8px; }
QFrame[role="champ"] QLabel { background:transparent; border:none; }
QLineEdit[role="interne"], QDateEdit[role="interne"], QComboBox[role="interne"], QComboBox[role="interneGras"]
    { border:none; background:transparent; padding:2px 4px; min-height:26px; color:#1F2A3D; }
QComboBox[role="interneGras"] { font-weight:600; }
QTextEdit { background:#FFFFFF; border:1px solid #E1D6C3; border-radius:8px; padding:4px 6px; color:#1F2A3D; }
QLineEdit:focus, QTextEdit:focus { border-color:#C9A36A; }
QComboBox::drop-down { border:none; width:22px; }
QComboBox::down-arrow { image:url(:/icons/chevron_dark.svg); width:12px; height:12px; }
QDateEdit::drop-down { border:none; width:0px; }
QComboBox QAbstractItemView { background:#FFFFFF; border:1px solid #E1D6C3; selection-background-color:#EFE3CF; selection-color:#0B2A45; outline:0; }
QFrame#fondPastilleMaquette { background:#F3F0EA; border:none; border-radius:8px; }

QPushButton[role="btnPrimaire"], QPushButton[role="btnGris"], QPushButton[role="btnBeige"], QPushButton[role="btnClair"], QPushButton[role="btnBlanc"]
    { border:none; border-radius:8px; padding:8px 16px; font-weight:700; min-height:22px; }
QPushButton[role="btnPrimaire"] { background:#0B2A45; color:#FFFFFF; }
QPushButton[role="btnPrimaire"]:hover { background:#163D63; }
QPushButton[role="btnGris"]   { background:#D9DDE3; color:#3A4252; }
QPushButton[role="btnGris"]:hover { background:#C9CED6; }
QPushButton[role="btnBeige"]  { background:#EFE3CF; color:#6B4F2A; }
QPushButton[role="btnBeige"]:hover { background:#E6D5B8; }
QPushButton[role="btnClair"]  { background:#E6EAF0; color:#3A4252; }
QPushButton[role="btnClair"]:hover { background:#D6DCE6; }
QPushButton[role="btnBlanc"]  { background:#FFFFFF; color:#0B2A45; border:1px solid #E1D6C3; font-weight:600; }
QPushButton[role="btnBlanc"]:hover { background:#F7F0E3; }
QToolButton[role="btnPlus"] { background:transparent; border:none; border-radius:8px; }
QToolButton[role="btnPlus"]:hover { background:#F1E7D5; }

QComboBox[role="tri"] { background:#FFFFFF; border:1px solid #E1D6C3; border-radius:8px; padding:4px 10px; color:#1F2A3D; }
QLabel#labelTriMaquette { font-weight:700; color:#0B2A45; background:transparent; }
QLineEdit#editRechercheListeMaquette { background:#FFFFFF; border:1px solid #E1D6C3; border-radius:8px; padding:6px 10px; min-height:22px; color:#1F2A3D; }

QTableWidget { background:#FFFFFF; border:none; outline:0; color:#1F2A3D; alternate-background-color:#FBF8F2; }
QTableWidget::item { padding:4px 8px; border-bottom:1px solid #F0E8DA; }
QTableWidget::item:selected { background:#EFE3CF; color:#0B2A45; }
QHeaderView::section { background:#F7F0E3; color:#0B2A45; font-weight:700; border:none; padding:6px 8px; }

QPushButton[role="page"] { background:#FFFFFF; color:#0B2A45; border:1px solid #E1D6C3; border-radius:6px; min-width:30px; max-width:30px; min-height:30px; max-height:30px; font-weight:600; }
QPushButton[role="page"]:hover { background:#F7F0E3; }
QPushButton[role="page"][actif="true"] { background:#0B2A45; color:#FFFFFF; border-color:#0B2A45; }

QFrame[role="blocStat"] { background:#FFFFFF; border:1px solid #EADFCF; border-radius:10px; }
QLabel[role="titreStat"] { font-weight:700; color:#0B2A45; background:transparent; }
QLabel#labelRetardMaquette { background:transparent; font-weight:600; }
)"));
}

void SmartMarket::installerIconesMaquette()
{
    const QColor encre("#2C3240");

    ui->banniereMaquette->setImage(QPixmap(dossierImages() + "banniere.png"));

    // en-têtes de cartes : pictogramme blanc dans un carré bleu nuit
    const struct { QLabel *l; const char *nom; } entetes[] = {
        { ui->iconeFormulaireMaquette, "clipboard" }, { ui->iconeListeMaquette, "list" }, { ui->iconeStatsMaquette, "stats" } };
    for (const auto &e : entetes) {
        e.l->setPixmap(Icones::pixmap(e.nom, Qt::white, 18));
        e.l->setFixedSize(30, 30);
        e.l->setAlignment(Qt::AlignCenter);
        e.l->setStyleSheet("background:#0B2A45; border:none; border-radius:7px;");
    }

    // champs
    ui->iconeIdMaquette->setPixmap(Icones::pixmap("idcard", kSable, 18));
    ui->iconeNomMaquette->setPixmap(Icones::pixmap("namecard", kSable, 18));
    ui->iconeTypeMaquette->setPixmap(Icones::pixmap("type", kSable, 18));
    ui->btnCalendrierMaquette->setIcon(Icones::icone("calendar", kSable, 18));

    // boutons
    ui->btnReinitialiserMaquette->setIcon(Icones::icone("reset", kNavy, 16));
    ui->btnExporterPdfMaquette->setIcon(Icones::icone("pdf", Qt::white, 16));
    ui->btnAjouterMaquette->setIcon(Icones::icone("plus", Qt::white, 16));
    ui->btnModifierMaquette->setIcon(Icones::icone("pencil", QColor("#3A4252"), 16));
    ui->btnSupprimerMaquette->setIcon(Icones::icone("trash", QColor("#6B4F2A"), 16));
    ui->btnAfficherMaquette->setIcon(Icones::icone("eye", QColor("#3A4252"), 16));
    ui->btnPlusMaquette->setIcon(Icones::icone("plus", QColor("#C59A62"), 22));
    ui->btnPlusMaquette->setIconSize(QSize(22, 22));
    ui->btnPlusMaquette->setToolTip("Ajouter une vidéo de présentation");
    ui->apercuMaquette->boutonCrayon()->setToolTip("Changer la vidéo de présentation");

    // barre du haut
    ui->editRechercheGlobaleMaquette->addAction(Icones::icone("search", kSable, 18), QLineEdit::LeadingPosition);
    ui->editRechercheListeMaquette->addAction(Icones::icone("search", kSable, 16), QLineEdit::LeadingPosition);
    ui->btnClocheMaquette->setIcon(Icones::icone("bell", kNavy, 22));
    ui->btnClocheMaquette->setIconSize(QSize(22, 22));
    ui->labelAvatarMaquette->setPixmap(Icones::pixmap("avatar", kNavy, 30));
    ui->btnCompteMaquette->setIcon(Icones::icone("chevron", kNavy, 14));

    // pastille « nombre de maquettes en retard » sur la cloche
    m_badgeClocheMaquette = new QLabel(ui->btnClocheMaquette);
    m_badgeClocheMaquette->setAlignment(Qt::AlignCenter);
    m_badgeClocheMaquette->setFixedSize(17, 17);
    m_badgeClocheMaquette->move(ui->btnClocheMaquette->width() - 20, 2);
    m_badgeClocheMaquette->setStyleSheet("background:#D9534F; color:white; border-radius:8px; font-size:10px; font-weight:700;");
    m_badgeClocheMaquette->hide();
}

// ============================================================================ données
QString SmartMarket::idCourantMaquette() const { return ui->editIdMaquette->text().trimmed().toUpper(); }

QString SmartMarket::prochainIdMaquette() const
{
    int max = 0;
    static const QRegularExpression re("^MQ(\\d+)$");
    for (const Maquette &m : Maquette::afficher("", ""))
        if (auto mt = re.match(m.getId()); mt.hasMatch()) max = qMax(max, mt.captured(1).toInt());
    return QString("MQ%1").arg(max + 1, 3, 10, QChar('0'));
}

Maquette SmartMarket::depuisFormulaireMaquette() const
{
    return Maquette(ui->editIdMaquette->text(), ui->editNomMaquette->text(), ui->comboTypeMaquette->currentText(),
                    ui->dateCreationMaquette->date(), ui->comboStatutMaquette->currentText(),
                    ui->editDescriptionMaquette->toPlainText(), m_videoCouranteMaquette);
}

void SmartMarket::remplirFormulaireMaquette(const Maquette &m)
{
    ui->editIdMaquette->setText(m.getId());
    ui->editNomMaquette->setText(m.getNom());
    ui->comboTypeMaquette->blockSignals(true);   ui->comboTypeMaquette->setCurrentText(m.getType());     ui->comboTypeMaquette->blockSignals(false);
    ui->comboStatutMaquette->blockSignals(true); ui->comboStatutMaquette->setCurrentText(m.getStatut()); ui->comboStatutMaquette->blockSignals(false);
    ui->dateCreationMaquette->setDate(m.getDateCreation());
    ui->editDescriptionMaquette->setPlainText(m.getDescription());
    majBoutonVideoMaquette(m.getVideo());
    onStatutChangeMaquette();
    afficherApercuMaquette(m.getType(), m.getStatut());
}

void SmartMarket::majBoutonVideoMaquette(const QString &video)
{
    m_videoCouranteMaquette = video;
    const bool a = !video.isEmpty();
    ui->btnPlusMaquette->setIcon(Icones::icone(a ? "play" : "plus", QColor("#C59A62"), 22));
    ui->btnPlusMaquette->setToolTip(a ? "Lire la vidéo de présentation" : "Ajouter une vidéo de présentation");
}

void SmartMarket::onStatutChangeMaquette()
{
    const StyleStatut st = styleStatut(ui->comboStatutMaquette->currentText());
    ui->pastilleStatutMaquette->setStyleSheet(QString("background:%1; border:none; border-radius:7px;").arg(st.pastille.name()));
    afficherApercuMaquette(ui->comboTypeMaquette->currentText(), ui->comboStatutMaquette->currentText());
}

// Statut « Validée » -> GIF animé ; sinon image fixe (toile)
void SmartMarket::afficherApercuMaquette(const QString &type, const QString &statut)
{
    const QPixmap avant(fichierAvant(type));
    if (statut.startsWith("Valid")) ui->apercuMaquette->afficherAnimation(fichierValidee(type));
    else if (!avant.isNull())       ui->apercuMaquette->afficherImage(avant);
    else                            ui->apercuMaquette->afficherVide("Aucune image");

    ui->mini1Maquette->setImage(avant);
    QMovie film(fichierValidee(type));
    film.jumpToFrame(0);
    ui->mini2Maquette->setImage(film.currentPixmap());
    ui->mini3Maquette->setImage(QPixmap());
}

// ============================================================================ liste / pagination
void SmartMarket::rafraichirToutMaquette()
{
    rafraichirListeMaquette();
    rafraichirStatsMaquette();
    verifierRetardsMaquette(false);
}

void SmartMarket::rafraichirListeMaquette()
{
    QString tri;
    switch (ui->comboTriMaquette->currentIndex()) { case 1: tri = "date"; break; case 2: tri = "type"; break; case 3: tri = "statut"; break; default: break; }
    const QList<Maquette> liste = Maquette::afficher(ui->editRechercheListeMaquette->text().trimmed(), tri);

    const int totalPages = qMax(1, (liste.size() + PAR_PAGEMaquette - 1) / PAR_PAGEMaquette);
    m_pageMaquette = qBound(0, m_pageMaquette, totalPages - 1);

    auto *t = ui->tableMaquettesMaquette;
    t->blockSignals(true);
    t->setRowCount(0);
    const int debut = m_pageMaquette * PAR_PAGEMaquette;
    for (int i = debut; i < qMin(debut + PAR_PAGEMaquette, int(liste.size())); ++i) {
        const Maquette &m = liste.at(i);
        const int r = t->rowCount();
        t->insertRow(r);

        auto *id = new QTableWidgetItem(m.getId());
        id->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable);
        id->setCheckState(Qt::Unchecked);
        t->setItem(r, 0, id);
        t->setItem(r, 1, new QTableWidgetItem(m.getNom()));
        t->setItem(r, 2, new QTableWidgetItem(m.getType()));
        t->setItem(r, 3, new QTableWidgetItem(m.getDateCreation().toString("dd/MM/yyyy")));

        auto *st = new QTableWidgetItem(m.getStatut());
        const StyleStatut ss = styleStatut(m.getStatut());
        st->setBackground(ss.fond);
        st->setForeground(ss.texte);
        st->setTextAlignment(Qt::AlignCenter);
        t->setItem(r, 4, st);

        auto *ap = new QTableWidgetItem(m.getVideo().isEmpty() ? "Image" : "Vidéo");
        t->setItem(r, 5, ap);

        if (m.estEnRetard(JOURS_RETARDMaquette)) {            // métier : maquette non validée depuis > 7 jours
            for (int c : { 0, 1, 2, 3, 5 }) {
                t->item(r, c)->setForeground(QColor("#C0392B"));
                QFont f = t->item(r, c)->font(); f.setBold(true); t->item(r, c)->setFont(f);
            }
        }
    }
    t->blockSignals(false);
    rafraichirPaginationMaquette(totalPages);
}

void SmartMarket::rafraichirPaginationMaquette(int totalPages)
{
    QLayout *lay = ui->layoutPaginationMaquette;
    while (QLayoutItem *it = lay->takeAt(0)) {
        if (QWidget *w = it->widget()) w->deleteLater();
        delete it;
    }
    auto bouton = [this, lay](const QString &texte, bool actif, int cible) {
        auto *b = new QPushButton(texte);
        b->setProperty("role", "page");
        b->setProperty("actif", actif);
        b->setCursor(Qt::PointingHandCursor);
        connect(b, &QPushButton::clicked, this, [this, cible] { m_pageMaquette = cible; rafraichirListeMaquette(); });
        lay->addWidget(b);
    };
    bouton("‹", false, qMax(0, m_pageMaquette - 1));
    const int fenetre = 5;
    int a = qMax(0, m_pageMaquette - fenetre / 2);
    int z = qMin(totalPages, a + fenetre);
    a = qMax(0, z - fenetre);
    for (int p = a; p < z; ++p) bouton(QString::number(p + 1), p == m_pageMaquette, p);
    bouton("›", false, qMin(totalPages - 1, m_pageMaquette + 1));
}

void SmartMarket::onLigneCliqueeMaquette(int row, int)
{
    auto *it = ui->tableMaquettesMaquette->item(row, 0);
    if (!it) return;
    Maquette m;
    if (Maquette::charger(it->text(), m)) remplirFormulaireMaquette(m);
}

// ============================================================================ CRUD
void SmartMarket::onAjouterMaquette()
{
    QString err;
    if (!depuisFormulaireMaquette().ajouter(err)) { QMessageBox::warning(ui->PMaquette, "Ajouter une maquette", err); return; }
    QMessageBox::information(ui->PMaquette, "Ajouter une maquette", "Maquette ajoutée avec succès.");
    m_pageMaquette = 1 << 20;                                  // se placer sur la dernière page (bornée dans rafraichirListe)
    rafraichirToutMaquette();
    onReinitialiserMaquette();
}

void SmartMarket::onModifierMaquette()
{
    QString err;
    if (!depuisFormulaireMaquette().modifier(err)) { QMessageBox::warning(ui->PMaquette, "Modifier une maquette", err); return; }
    QMessageBox::information(ui->PMaquette, "Modifier une maquette", "Maquette modifiée avec succès.");
    rafraichirToutMaquette();
}

void SmartMarket::onSupprimerMaquette()
{
    // 1) cases cochées (suppression multiple)   2) sinon la maquette du formulaire
    QStringList ids;
    for (int r = 0; r < ui->tableMaquettesMaquette->rowCount(); ++r)
        if (auto *it = ui->tableMaquettesMaquette->item(r, 0); it && it->checkState() == Qt::Checked) ids << it->text();
    if (ids.isEmpty() && !idCourantMaquette().isEmpty()) ids << idCourantMaquette();
    if (ids.isEmpty()) { QMessageBox::information(ui->PMaquette, "Supprimer", "Sélectionnez ou cochez au moins une maquette."); return; }

    const QString q = ids.size() == 1 ? QString("Supprimer la maquette %1 ?").arg(ids.first())
                                      : QString("Supprimer %1 maquettes (%2) ?").arg(ids.size()).arg(ids.join(", "));
    if (QMessageBox::question(ui->PMaquette, "Supprimer", q) != QMessageBox::Yes) return;

    int echecs = 0;
    for (const QString &id : ids) if (!Maquette::supprimer(id)) ++echecs;
    if (echecs) QMessageBox::warning(ui->PMaquette, "Supprimer", QString("%1 suppression(s) impossible(s).").arg(echecs));
    onReinitialiserMaquette();
    rafraichirToutMaquette();
}

void SmartMarket::onAfficherMaquette()
{
    Maquette m;
    if (idCourantMaquette().isEmpty() || !Maquette::charger(idCourantMaquette(), m)) {
        QMessageBox::information(ui->PMaquette, "Afficher", "Maquette introuvable : saisissez un ID existant ou cliquez sur une ligne.");
        return;
    }
    Maquette::incrementerVues(m.getId());               // +1 consultation (Top 5)
    Maquette::charger(m.getId(), m);
    remplirFormulaireMaquette(m);
    rafraichirStatsMaquette();
    QMessageBox::information(ui->PMaquette, "Maquette " + m.getId(),
        QString("%1\nType : %2\nStatut : %3\nCréée le : %4\nConsultations : %5\nVidéo : %6\n\n%7")
            .arg(m.getNom(), m.getType(), m.getStatut(), m.getDateCreation().toString("dd/MM/yyyy"))
            .arg(m.getVues())
            .arg(m.getVideo().isEmpty() ? "aucune" : m.getVideo(), m.getDescription()));
}

void SmartMarket::onReinitialiserMaquette()
{
    ui->editIdMaquette->setText(prochainIdMaquette());
    ui->editNomMaquette->clear();
    ui->editDescriptionMaquette->clear();
    ui->comboTypeMaquette->setCurrentIndex(0);
    ui->comboStatutMaquette->setCurrentIndex(0);
    ui->dateCreationMaquette->setDate(QDate::currentDate());
    ui->tableMaquettesMaquette->clearSelection();
    majBoutonVideoMaquette(QString());
    onStatutChangeMaquette();
}

// ============================================================================ vidéo (métier 1)
void SmartMarket::onChoisirVideoMaquette()
{
    const QString id = idCourantMaquette();
    if (id.isEmpty() || !Maquette::existe(id)) {
        QMessageBox::information(ui->PMaquette, "Vidéo", "Enregistrez d'abord la maquette (bouton Ajouter), puis ajoutez sa vidéo.");
        return;
    }
    const QString f = QFileDialog::getOpenFileName(ui->PMaquette, "Choisir une vidéo de présentation", QString(),
                                                   "Vidéos (*.mp4 *.avi *.mov *.mkv *.wmv);;Tous les fichiers (*)");
    if (f.isEmpty()) return;
    if (!Maquette::enregistrerVideo(id, f)) { QMessageBox::warning(ui->PMaquette, "Vidéo", "Impossible d'enregistrer la vidéo."); return; }
    majBoutonVideoMaquette(f);
    rafraichirListeMaquette();
}

void SmartMarket::onLireVideoMaquette()
{
    if (m_videoCouranteMaquette.isEmpty()) return;
    if (!QDesktopServices::openUrl(QUrl::fromLocalFile(m_videoCouranteMaquette)))
        QMessageBox::warning(ui->PMaquette, "Vidéo", "Impossible d'ouvrir le fichier :\n" + m_videoCouranteMaquette);
}

// ============================================================================ retards (métier 2)
void SmartMarket::verifierRetardsMaquette(bool popup)
{
    const QList<Maquette> retards = Maquette::enRetard(JOURS_RETARDMaquette);
    const int n = retards.size();

    if (n > 0) {
        ui->labelRetardMaquette->setText(QString("⚠  %1 maquette(s) non validée(s) depuis plus de %2 jours").arg(n).arg(JOURS_RETARDMaquette));
        ui->labelRetardMaquette->setStyleSheet("color:#C0392B; background:transparent; font-weight:600;");
    } else {
        ui->labelRetardMaquette->setText("✔  Aucune maquette en retard de validation");
        ui->labelRetardMaquette->setStyleSheet("color:#2E7D4F; background:transparent; font-weight:600;");
    }
    if (m_badgeClocheMaquette) { m_badgeClocheMaquette->setText(QString::number(n)); m_badgeClocheMaquette->setVisible(n > 0); }

    if (popup) {
        m_alerteAfficheeMaquette = true;
        if (n == 0) { QMessageBox::information(ui->PMaquette, "Maquettes en retard", "Aucune maquette en retard de validation."); return; }
        QStringList lignes;
        for (const Maquette &m : retards)
            lignes << QString("• %1 — %2 (%3 jours)").arg(m.getId(), m.getNom()).arg(m.getDateCreation().daysTo(QDate::currentDate()));
        QMessageBox::warning(ui->PMaquette, "Maquettes en retard de validation",
            QString("%1 maquette(s) ne sont pas validées depuis plus de %2 jours :\n\n%3").arg(n).arg(JOURS_RETARDMaquette).arg(lignes.join("\n")));
    }
}

// ============================================================================ statistiques
void SmartMarket::rafraichirStatsMaquette()
{
    QList<QPair<QString, int>> parType;
    const QMap<QString, int> types = Maquette::statsParType();
    for (const QString &t : { "Robe", "Veste", "Jupe" })
        if (types.value(t) > 0) parType.append({ t, types.value(t) });
    ui->chartTypeMaquette->setDonnees(parType);
    ui->chartEvolutionMaquette->setDonnees(Maquette::statsParMois(6));
    ui->chartTopMaquette->setDonnees(Maquette::top5Consultees());
}

// ============================================================================ export PDF
void SmartMarket::onExporterPdfMaquette()
{
    const QString fichier = QFileDialog::getSaveFileName(ui->PMaquette, "Exporter la liste des maquettes",
                                                         "maquettes.pdf", "PDF (*.pdf)");
    if (fichier.isEmpty()) return;

    QString html = "<h2 style='color:#0B2A45'>FashioNova — Liste des maquettes</h2>"
                   "<p>Édité le " + QDate::currentDate().toString("dd/MM/yyyy") + "</p>"
                   "<table border='1' cellspacing='0' cellpadding='5' width='100%'>"
                   "<tr style='background:#EFE3CF'><th>ID</th><th>Nom</th><th>Type</th><th>Date</th><th>Statut</th><th>Consultations</th></tr>";
    for (const Maquette &m : Maquette::afficher(ui->editRechercheListeMaquette->text().trimmed(), "")) {
        html += QString("<tr><td>%1</td><td>%2</td><td>%3</td><td>%4</td><td>%5</td><td align='center'>%6</td></tr>")
                    .arg(m.getId(), m.getNom().toHtmlEscaped(), m.getType(),
                         m.getDateCreation().toString("dd/MM/yyyy"), m.getStatut()).arg(m.getVues());
    }
    html += "</table>";

    QTextDocument doc;
    doc.setHtml(html);
    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setPageSize(QPageSize(QPageSize::A4));
    printer.setOutputFileName(fichier);
    doc.print(&printer);
    QMessageBox::information(ui->PMaquette, "Export PDF", "Liste exportée :\n" + fichier);
}


// ===========================================================================
//  MODULE MACHINES (page PMachine) — slots copiés du projet « Machines »
// ===========================================================================
static const int kPageSize = 5;

// Version « maquette » : mettre true pour réactiver les boutons du CRUD
// (Ajouter, Afficher, Modifier, Supprimer). A false, ils restent visibles mais ne font rien.
static const bool kCrudActif = true;

static void repolish(QWidget *w)
{
    w->style()->unpolish(w);
    w->style()->polish(w);
    w->update();
}

static void clearLayout(QLayout *l)
{
    while (QLayoutItem *item = l->takeAt(0)) {
        if (QWidget *w = item->widget()) w->deleteLater();
        delete item;
    }
}

/* ============================ construction ============================ */

void SmartMarket::initMachine()
{
    

    // Intégration : le style du module est appliqué à cette page uniquement
    // (dans l'application autonome il était posé sur QApplication, ce qui affecterait les autres modules).
    {
        QFile qss(QStringLiteral(":/style.qss"));
        if (qss.open(QIODevice::ReadOnly))
            ui->PMachine->setStyleSheet(QString::fromUtf8(qss.readAll()));
    }
    m_pagerMachine = ui->pagerLayoutMachine;

    setupIconsMachine();
    setupTableMachine();
    connectSignalsMachine();

    reloadDataMachine();
    clearFormMachine();

    // Re-vérification périodique (une machine peut devenir « en retard » sans action de l'utilisateur)
    auto *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &SmartMarket::reloadDataMachine);
    timer->start(60 * 1000);
}

// Les icônes sont des SVG recolorés à l'exécution : elles sont donc affectées ici et non dans le .ui.
void SmartMarket::setupIconsMachine()
{
    const QColor ink("#2c3240");
    // cartes
    ui->iconCardFormMachine->setPixmap(UI::svgPixmap(QStringLiteral("gear"),  Qt::white, 18));
    ui->iconCardListMachine->setPixmap(UI::svgPixmap(QStringLiteral("clip"),  Qt::white, 18));
    ui->iconCardStatsMachine->setPixmap(UI::svgPixmap(QStringLiteral("chart"), Qt::white, 18));

    // champs du formulaire
    ui->iconIdMachine->setPixmap(UI::svgPixmap(QStringLiteral("idcard"),   ink, 18));
    ui->iconNomMachine->setPixmap(UI::svgPixmap(QStringLiteral("gear"),    ink, 18));
    ui->iconTypeMachine->setPixmap(UI::svgPixmap(QStringLiteral("box"),    ink, 18));
    ui->iconDateMachine->setPixmap(UI::svgPixmap(QStringLiteral("calendar"), ink, 18));
    for (int i = 0; i < ui->comboEtatMachine->count(); ++i)
        ui->comboEtatMachine->setItemIcon(i, UI::dotIcon(UI::etatColor(ui->comboEtatMachine->itemText(i))));

    // photo
    ui->btnPenMachine->setIcon(UI::svgIcon(QStringLiteral("pen"), Qt::white, 16));
    ui->btnPlusMachine->setIcon(UI::svgIcon(QStringLiteral("plus"), QColor("#c9a36a"), 22));
    ui->labelThumb1Machine->setPixmap(UI::cover(QPixmap(QStringLiteral(":/images/thumb1.jpg")), QSize(44, 56), 5));
    ui->labelThumb2Machine->setPixmap(UI::cover(QPixmap(QStringLiteral(":/images/thumb2.jpg")), QSize(44, 56), 5));

    // boutons
    ui->btnAddMachine->setIcon(UI::svgIcon(QStringLiteral("plus"),  Qt::white, 20));
    ui->btnModMachine->setIcon(UI::svgIcon(QStringLiteral("pen"),   Qt::white, 20));
    ui->btnDelMachine->setIcon(UI::svgIcon(QStringLiteral("trash"), QColor("#4a3a26"), 20));
    ui->btnViewMachine->setIcon(UI::svgIcon(QStringLiteral("eye"),  QColor("#1b1d25"), 20));
    ui->btnResetMachine->setIcon(UI::svgIcon(QStringLiteral("refresh"), QColor("#23252e"), 16));
    ui->btnExportMachine->setIcon(UI::svgIcon(QStringLiteral("doc"), Qt::white, 16));
    ui->btnChipMachine->setIcon(UI::svgIcon(QStringLiteral("warn"), QColor("#b9781a"), 16));

    ui->lineSearchMachine->addAction(UI::svgIcon(QStringLiteral("search"), QColor("#6b7a90"), 18),
                              QLineEdit::LeadingPosition);
}

void SmartMarket::setupTableMachine()
{
    QTableWidget *t = ui->tableMachinesMachine;
    t->verticalHeader()->setDefaultSectionSize(50);
    t->setFixedHeight(40 + 50 * kPageSize + 4);
    QHeaderView *hh = t->horizontalHeader();
    hh->setHighlightSections(false);
    hh->setFixedHeight(40);
    hh->setSectionsClickable(true);
    hh->setSectionResizeMode(QHeaderView::Fixed);
    hh->setSectionResizeMode(2, QHeaderView::Stretch);
    hh->setStretchLastSection(false);
    const int widths[] = { 34, 70, 0, 78, 122, 118, 84 };
    for (int c = 0; c < 7; ++c) if (widths[c]) t->setColumnWidth(c, widths[c]);
    hh->setDefaultAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    // case « tout cocher » de l'en-tête
    connect(hh, &QHeaderView::sectionClicked, this, [this](int c) {
        if (c != 0) return;
        QTableWidget *tw = ui->tableMachinesMachine;
        bool all = true;
        for (int r = 0; r < tw->rowCount(); ++r)
            if (tw->item(r, 0)->checkState() != Qt::Checked) all = false;
        for (int r = 0; r < tw->rowCount(); ++r)
            tw->item(r, 0)->setCheckState(all ? Qt::Unchecked : Qt::Checked);
    });
}

void SmartMarket::connectSignalsMachine()
{
    if (kCrudActif) {
        connect(ui->btnAddMachine,  &QPushButton::clicked, this, &SmartMarket::onAddMachine);      // Create
        connect(ui->btnViewMachine, &QPushButton::clicked, this, &SmartMarket::onViewMachine);     // Read
        connect(ui->btnModMachine,  &QPushButton::clicked, this, &SmartMarket::onModifyMachine);   // Update
        connect(ui->btnDelMachine,  &QPushButton::clicked, this, &SmartMarket::onDeleteMachine);   // Delete
    }
    connect(ui->btnResetMachine,  &QPushButton::clicked, this, &SmartMarket::onResetMachine);
    connect(ui->btnExportMachine, &QPushButton::clicked, this, &SmartMarket::onExportPdfMachine);
    connect(ui->btnPenMachine,    &QToolButton::clicked, this, &SmartMarket::onChoosePhotoMachine);
    connect(ui->btnPlusMachine,   &QPushButton::clicked, this, &SmartMarket::onChoosePhotoMachine);

    connect(ui->comboEtatMachine, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this] { updateDetectionMachine(); });
    connect(ui->dateMaintenanceMachine, &QDateEdit::dateChanged, this, [this] { updateDetectionMachine(); });

    connect(ui->tableMachinesMachine, &QTableWidget::itemSelectionChanged, this, &SmartMarket::onRowSelectedMachine);
    connect(ui->lineSearchMachine, &QLineEdit::textChanged, this, &SmartMarket::onSearchChangedMachine);
    connect(ui->comboSortMachine, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this] { m_pageMachine = 0; applyFilterMachine(); });
    connect(ui->btnChipMachine, &QPushButton::clicked, this, [this] { m_onlyAlertsMachine = !m_onlyAlertsMachine; m_pageMachine = 0; applyFilterMachine(); });
}

/* ============================== données =============================== */

const Machine *SmartMarket::findMachineMachine(const QString &id) const
{
    for (const Machine &m : m_allMachine)
        if (m.id == id) return &m;
    return nullptr;
}

void SmartMarket::reloadDataMachine()
{
    m_allMachine = Database::all();
    checkAlertsMachine();
    updateChartsMachine();
    applyFilterMachine();
}

void SmartMarket::onSearchChangedMachine(const QString &text)
{
    // les deux champs de recherche restent synchronisés
    m_pageMachine = 0;
    applyFilterMachine();
    emit searchTextChangedMachine(text);      // la barre de recherche du haut se met à jour
}

void SmartMarket::setSearchTextMachine(const QString &text)
{
    if (ui->lineSearchMachine->text() != text)
        ui->lineSearchMachine->setText(text);  // déclenche onSearchChanged()
}

void SmartMarket::applyFilterMachine()
{
    const QString q = ui->lineSearchMachine->text().trimmed();
    m_viewMachine.clear();
    for (const Machine &m : std::as_const(m_allMachine)) {
        // recherche par ID_Machine, Nom_Machine, Type_Machine
        const bool match = q.isEmpty()
            || m.id.contains(q, Qt::CaseInsensitive)
            || m.nom.contains(q, Qt::CaseInsensitive)
            || m.type.contains(q, Qt::CaseInsensitive);
        if (match && (!m_onlyAlertsMachine || m.maintenanceRequise()))
            m_viewMachine.append(m);
    }

    // tri par Nom_Machine, Type_Machine ou Etat
    const int key = ui->comboSortMachine->currentIndex();
    if (key > 0) {
        std::stable_sort(m_viewMachine.begin(), m_viewMachine.end(), [key](const Machine &a, const Machine &b) {
            const QString x = key == 1 ? a.nom : key == 2 ? a.type : a.etat;
            const QString y = key == 1 ? b.nom : key == 2 ? b.type : b.etat;
            return QString::localeAwareCompare(x, y) < 0;
        });
    }

    int alerts = 0;
    for (const Machine &m : std::as_const(m_allMachine)) if (m.maintenanceRequise()) ++alerts;
    ui->btnChipMachine->setVisible(alerts > 0);
    ui->btnChipMachine->setText(m_onlyAlertsMachine
        ? QStringLiteral("  %1 machine(s) à maintenir – afficher tout").arg(alerts)
        : QStringLiteral("  %1 machine(s) nécessitent une maintenance").arg(alerts));

    renderPageMachine();
}

void SmartMarket::renderPageMachine()
{
    const int total = m_viewMachine.size();
    const int pages = qMax(1, (total + kPageSize - 1) / kPageSize);
    m_pageMachine = qBound(0, m_pageMachine, pages - 1);
    const int from = m_pageMachine * kPageSize;
    const int to = qMin(total, from + kPageSize);

    {
        QSignalBlocker blocker(ui->tableMachinesMachine);
        ui->tableMachinesMachine->clearContents();
        ui->tableMachinesMachine->setRowCount(to - from);

        for (int r = 0; r < to - from; ++r) {
            const Machine &m = m_viewMachine.at(from + r);

            auto *chk = new QTableWidgetItem;
            chk->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
            chk->setCheckState(Qt::Unchecked);
            chk->setData(Qt::UserRole, m.id);
            ui->tableMachinesMachine->setItem(r, 0, chk);

            auto mk = [&](const QString &txt) {
                auto *it = new QTableWidgetItem(txt);
                it->setData(Qt::UserRole, m.id);
                return it;
            };
            ui->tableMachinesMachine->setItem(r, 1, mk(m.id));
            ui->tableMachinesMachine->setItem(r, 2, mk(m.nom));
            ui->tableMachinesMachine->setItem(r, 3, mk(m.type));

            // pastille d'état
            QString bg = QStringLiteral("#dcefd9"), fg = QStringLiteral("#3f8a45");
            if (m.etat == Etat::maintenance())  { bg = QStringLiteral("#f7e6cc"); fg = QStringLiteral("#b9781a"); }
            if (m.etat == Etat::indisponible()) { bg = QStringLiteral("#f8d6d2"); fg = QStringLiteral("#c8402f"); }
            auto *pill = new QLabel(m.etat);
            pill->setStyleSheet(QStringLiteral("background:%1;color:%2;border-radius:5px;padding:4px 9px;font-size:12px;")
                                    .arg(bg, fg));
            auto *pw = new QWidget;
            auto *pl = new QHBoxLayout(pw);
            pl->setContentsMargins(4, 0, 4, 0);
            pl->addWidget(pill);
            pl->addStretch();
            ui->tableMachinesMachine->setCellWidget(r, 4, pw);

            // date + détection automatique
            const QString reason = m.raisonMaintenance();
            auto *dt = mk((reason.isEmpty() ? QString() : QStringLiteral("⚠ "))
                          + m.prochaineMaintenance.toString(QStringLiteral("dd/MM/yyyy")));
            if (!reason.isEmpty()) {
                dt->setForeground(m.critique() ? QColor("#c8402f") : QColor("#b9781a"));
                dt->setToolTip(reason);
            }
            ui->tableMachinesMachine->setItem(r, 5, dt);

            auto *ap = new QLabel;
            ap->setPixmap(UI::cover(UI::machinePixmap(m.photo), QSize(64, 38), 4));
            auto *aw = new QWidget;
            auto *al = new QHBoxLayout(aw);
            al->setContentsMargins(4, 0, 4, 0);
            al->addWidget(ap);
            ui->tableMachinesMachine->setCellWidget(r, 6, aw);
        }

        for (int r = 0; r < to - from; ++r)
            if (ui->tableMachinesMachine->item(r, 1)->data(Qt::UserRole).toString() == m_currentIdMachine)
                ui->tableMachinesMachine->selectRow(r);
    }
    buildPagerMachine(pages);
}

void SmartMarket::buildPagerMachine(int pages)
{
    clearLayout(m_pagerMachine);
    m_pagerMachine->addStretch();
    auto add = [&](const QString &text, int target, bool current, bool enabled) {
        auto *b = new QPushButton(text);
        b->setObjectName(current ? QStringLiteral("pageCur") : QStringLiteral("page"));
        b->setFixedSize(36, 36);
        b->setEnabled(enabled);
        b->setCursor(Qt::PointingHandCursor);
        connect(b, &QPushButton::clicked, this, [this, target] { m_pageMachine = target; renderPageMachine(); });
        m_pagerMachine->addWidget(b);
    };
    add(QStringLiteral("‹"), qMax(0, m_pageMachine - 1), false, m_pageMachine > 0);
    const int start = qMax(0, qMin(m_pageMachine - 2, pages - 5));
    const int end = qMin(pages, start + 5);
    if (start > 0) { add(QStringLiteral("1"), 0, false, true); if (start > 1) add(QStringLiteral("…"), start - 1, false, true); }
    for (int i = start; i < end; ++i) add(QString::number(i + 1), i, i == m_pageMachine, true);
    if (end < pages) { if (end < pages - 1) add(QStringLiteral("…"), end, false, true); add(QString::number(pages), pages - 1, false, true); }
    add(QStringLiteral("›"), qMin(pages - 1, m_pageMachine + 1), false, m_pageMachine < pages - 1);
}

void SmartMarket::goToMachineMachine(const QString &id)
{
    for (int i = 0; i < m_viewMachine.size(); ++i)
        if (m_viewMachine[i].id == id) { m_pageMachine = i / kPageSize; break; }
    renderPageMachine();
}

/* ============================ statistiques ============================ */

void SmartMarket::updateChartsMachine()
{
    // Répartition par type
    QMap<QString, int> count;
    for (const Machine &m : std::as_const(m_allMachine)) count[m.type]++;
    QVector<MachDonutChart::Slice> slices;
    for (const QString &t : typesMachine())
        if (count.value(t) > 0)
            slices.append(MachDonutChart::Slice{ t, count.value(t), UI::typeColor(t) });
    ui->donutChartMachine->setData(slices, QStringLiteral("machines"));

    // Historique : le mois courant reflète l'état réel du parc
    Snapshot now;
    now.mois = QDate::currentDate().toString(QStringLiteral("yyyy-MM"));
    for (const Machine &m : std::as_const(m_allMachine)) {
        if (m.indisponible()) ++now.horsService;
        else if (m.maintenanceRequise()) ++now.bientot;
        else ++now.operationnelles;
    }
    Database::saveSnapshot(now);

    static const char *mois[] = { "Jan", "Fév", "Mar", "Avr", "Mai", "Juin", "Juil", "Aoû", "Sep", "Oct", "Nov", "Déc" };
    QStringList labels;
    QVector<double> ok, soon, down;
    for (const Snapshot &s : Database::snapshots(6)) {
        labels << QString::fromUtf8(mois[qBound(1, s.mois.mid(5, 2).toInt(), 12) - 1]);
        ok << s.operationnelles; soon << s.bientot; down << s.horsService;
    }
    ui->lineChartMachine->setData(labels, ok, soon, down);

    // Top 5 : machines dont la maintenance est la plus proche
    QList<Machine> sorted = m_allMachine;
    std::stable_sort(sorted.begin(), sorted.end(), [](const Machine &a, const Machine &b) {
        return a.prochaineMaintenance < b.prochaineMaintenance;
    });
    QVector<TopBars::Entry> entries;
    for (int i = 0; i < qMin(5, sorted.size()); ++i) {
        const int j = sorted[i].joursRestants();
        const QString value = j < 0 ? QStringLiteral("retard") : QStringLiteral("%1 j").arg(j);
        entries.append(TopBars::Entry{ sorted[i].nom, value, (60.0 - qBound(0, j, 60)) / 60.0 });
    }
    ui->topBarsMachine->setData(entries);
}

/* ================================ alertes ================================ */

void SmartMarket::checkAlertsMachine()
{
    QSet<QString> unavailable;
    int needing = 0;
    for (const Machine &m : std::as_const(m_allMachine)) {
        if (m.indisponible()) unavailable.insert(m.id);
        if (m.maintenanceRequise()) ++needing;
    }
    // alerte : une machine vient de devenir indisponible
    if (m_alertsReadyMachine) {
        for (const QString &id : std::as_const(unavailable)) {
            if (!m_knownUnavailableMachine.contains(id)) {
                const Machine *m = findMachineMachine(id);
                if (m) emit notifyMachine(QStringLiteral("Alerte : la machine %1 – %2 est indisponible.").arg(m->id, m->nom), true);
            }
        }
    }
    m_knownUnavailableMachine = unavailable;
    m_alertsReadyMachine = true;

    if (needing != m_alertCountMachine) {
        m_alertCountMachine = needing;
        emit alertCountChangedMachine(m_alertCountMachine);
    }
}

void SmartMarket::showAlertsMenuMachine(QWidget *anchor)
{
    QMenu menu(ui->PMachine);
    bool any = false;
    for (const Machine &m : std::as_const(m_allMachine)) {
        const QString reason = m.raisonMaintenance();
        if (reason.isEmpty()) continue;
        any = true;
        QAction *a = menu.addAction(UI::dotIcon(m.critique() ? QColor("#e5484d") : QColor("#e6a23c")),
                                    QStringLiteral("%1 – %2 : %3").arg(m.id, m.nom, reason));
        const QString id = m.id;
        connect(a, &QAction::triggered, this, [this, id] {
            m_currentIdMachine = id;
            ui->lineSearchMachine->clear();
            goToMachineMachine(id);
            if (const Machine *mm = findMachineMachine(id)) loadFormMachine(*mm);
        });
    }
    if (!any)
        menu.addAction(QStringLiteral("Aucune alerte – tout le parc est à jour"))->setEnabled(false);
    menu.exec(anchor->mapToGlobal(QPoint(-260, anchor->height() + 4)));
}

/* ============================== formulaire ============================== */

bool SmartMarket::readFormMachine(Machine &m, QString *error) const
{
    m.nom = ui->lineNomMachine->text().trimmed();
    if (m.nom.isEmpty()) {
        *error = QStringLiteral("Le nom de la machine est obligatoire.");
        return false;
    }
    m.type = ui->comboTypeMachine->currentText();
    m.etat = ui->comboEtatMachine->currentText();
    m.prochaineMaintenance = ui->dateMaintenanceMachine->date();
    m.photo = m_formPhotoMachine;
    return true;
}

void SmartMarket::loadFormMachine(const Machine &m)
{
    ui->lineIdMachine->setText(m.id);
    ui->lineNomMachine->setText(m.nom);
    int i = ui->comboTypeMachine->findText(m.type);
    if (i < 0) { ui->comboTypeMachine->addItem(m.type); i = ui->comboTypeMachine->findText(m.type); }
    ui->comboTypeMachine->setCurrentIndex(i);
    ui->comboEtatMachine->setCurrentIndex(qMax(0, ui->comboEtatMachine->findText(m.etat)));
    ui->dateMaintenanceMachine->setDate(m.prochaineMaintenance);
    m_formPhotoMachine = m.photo;
    updatePreviewMachine();
    updateDetectionMachine();
}

void SmartMarket::clearFormMachine()
{
    m_currentIdMachine.clear();
    m_formPhotoMachine.clear();
    ui->lineIdMachine->setText(Database::nextId());
    ui->lineNomMachine->clear();
    ui->comboTypeMachine->setCurrentIndex(0);
    ui->comboEtatMachine->setCurrentIndex(0);
    ui->dateMaintenanceMachine->setDate(QDate::currentDate().addDays(60));
    updatePreviewMachine();
    updateDetectionMachine();
}

void SmartMarket::updatePreviewMachine()
{
    ui->labelPhotoMachine->setPixmap(UI::cover(UI::machinePixmap(m_formPhotoMachine), QSize(142, 214), 8));
}

void SmartMarket::updateDetectionMachine()
{
    if (!ui->labelDetectionMachine) return;
    Machine tmp;
    tmp.etat = ui->comboEtatMachine->currentText();
    tmp.prochaineMaintenance = ui->dateMaintenanceMachine->date();
    const QString reason = tmp.raisonMaintenance();
    if (reason.isEmpty()) {
        ui->labelDetectionMachine->setText(QStringLiteral("✔  Détection automatique : maintenance à jour"));
        ui->labelDetectionMachine->setProperty("level", QStringLiteral("ok"));
    } else {
        ui->labelDetectionMachine->setText(QStringLiteral("⚠  Détection automatique : %1").arg(reason));
        ui->labelDetectionMachine->setProperty("level", tmp.critique() ? QStringLiteral("crit") : QStringLiteral("warn"));
    }
    repolish(ui->labelDetectionMachine);
}

void SmartMarket::onRowSelectedMachine()
{
    const QList<QTableWidgetItem *> items = ui->tableMachinesMachine->selectedItems();
    if (items.isEmpty()) return;
    const QString id = items.first()->data(Qt::UserRole).toString();
    if (const Machine *m = findMachineMachine(id)) {
        m_currentIdMachine = id;
        loadFormMachine(*m);
    }
}

/* =============================== actions =============================== */

void SmartMarket::onAddMachine()
{
    Machine m;
    QString err;
    if (!readFormMachine(m, &err)) { QMessageBox::warning(ui->PMachine, QStringLiteral("Ajouter"), err); return; }
    m.id = Database::nextId();
    if (!Database::insert(m, &err)) { QMessageBox::critical(ui->PMachine, QStringLiteral("Ajouter"), err); return; }
    m_currentIdMachine = m.id;
    reloadDataMachine();
    goToMachineMachine(m.id);
    loadFormMachine(m);
    emit notifyMachine(QStringLiteral("Machine %1 ajoutée avec succès.").arg(m.id), false);
}

void SmartMarket::onModifyMachine()
{
    if (m_currentIdMachine.isEmpty()) {
        QMessageBox::information(ui->PMachine, QStringLiteral("Modifier"), QStringLiteral("Sélectionnez d'abord une machine dans la liste."));
        return;
    }
    Machine m;
    QString err;
    if (!readFormMachine(m, &err)) { QMessageBox::warning(ui->PMachine, QStringLiteral("Modifier"), err); return; }
    m.id = m_currentIdMachine;
    if (!Database::update(m, &err)) { QMessageBox::critical(ui->PMachine, QStringLiteral("Modifier"), err); return; }
    reloadDataMachine();
    goToMachineMachine(m.id);
    emit notifyMachine(QStringLiteral("Machine %1 modifiée.").arg(m.id), false);
}

void SmartMarket::onDeleteMachine()
{
    if (m_currentIdMachine.isEmpty()) {
        QMessageBox::information(ui->PMachine, QStringLiteral("Supprimer"), QStringLiteral("Sélectionnez d'abord une machine dans la liste."));
        return;
    }
    const auto r = QMessageBox::question(ui->PMachine, QStringLiteral("Supprimer"),
                                         QStringLiteral("Supprimer définitivement la machine %1 ?").arg(m_currentIdMachine));
    if (r != QMessageBox::Yes) return;
    QString err;
    if (!Database::remove(m_currentIdMachine, &err)) { QMessageBox::critical(ui->PMachine, QStringLiteral("Supprimer"), err); return; }
    const QString id = m_currentIdMachine;
    clearFormMachine();
    reloadDataMachine();
    emit notifyMachine(QStringLiteral("Machine %1 supprimée.").arg(id), false);
}

void SmartMarket::onViewMachine()
{
    const Machine *m = m_currentIdMachine.isEmpty() ? nullptr : findMachineMachine(m_currentIdMachine);
    if (!m) {
        QMessageBox::information(ui->PMachine, QStringLiteral("Afficher"), QStringLiteral("Sélectionnez d'abord une machine dans la liste."));
        return;
    }
    DetailsDialog dlg(*m, ui->PMachine);
    dlg.exec();
}

void SmartMarket::onResetMachine()
{
    clearFormMachine();
    ui->tableMachinesMachine->clearSelection();
}

void SmartMarket::onChoosePhotoMachine()
{
    const QString f = QFileDialog::getOpenFileName(ui->PMachine, QStringLiteral("Choisir une photo"), QString(),
                                                   QStringLiteral("Images (*.png *.jpg *.jpeg *.bmp)"));
    if (f.isEmpty()) return;
    m_formPhotoMachine = f;
    updatePreviewMachine();
    if (!m_currentIdMachine.isEmpty())
        emit notifyMachine(QStringLiteral("Photo sélectionnée : cliquez sur « Modifier » pour l'enregistrer."), false);
}

void SmartMarket::onExportPdfMachine()
{
    const QString path = QFileDialog::getSaveFileName(ui->PMachine, QStringLiteral("Exporter la liste des machines"),
                                                      QStringLiteral("liste_machines.pdf"),
                                                      QStringLiteral("PDF (*.pdf)"));
    if (path.isEmpty()) return;

    QString rows;
    for (const Machine &m : std::as_const(m_viewMachine)) {
        const QString alert = m.raisonMaintenance();
        rows += QStringLiteral("<tr><td>%1</td><td>%2</td><td>%3</td><td>%4</td><td>%5</td><td>%6</td></tr>")
                    .arg(m.id, m.nom.toHtmlEscaped(), m.type, m.etat,
                         m.prochaineMaintenance.toString(QStringLiteral("dd/MM/yyyy")), alert);
    }
    const QString html = QStringLiteral(
        "<h1 style='color:#0a2748'>FashioNova – Liste des machines</h1>"
        "<p>Édité le %1 – %2 machine(s)</p>"
        "<table border='1' cellspacing='0' cellpadding='5' width='100%'>"
        "<tr style='background:#efe3cf'><th>ID_Machine</th><th>Nom_Machine</th><th>Type_Machine</th>"
        "<th>Etat</th><th>Date_Prochaine_Maintenance</th><th>Alerte</th></tr>%3</table>")
        .arg(QDate::currentDate().toString(QStringLiteral("dd/MM/yyyy")))
        .arg(m_viewMachine.size())
        .arg(rows);

    QPdfWriter writer(path);
    writer.setPageSize(QPageSize(QPageSize::A4));
    writer.setPageMargins(QMarginsF(15, 15, 15, 15));
    QTextDocument doc;
    doc.setHtml(html);
    doc.print(&writer);

    emit notifyMachine(QStringLiteral("PDF exporté : %1").arg(path), false);
    QDesktopServices::openUrl(QUrl::fromLocalFile(path));
}


// ===========================================================================
//  MODULE EMPLOYÉS (page PEmpl) — slots copiés du projet « Employés »
// ===========================================================================
class StatsWidget : public QWidget {
public:
    QList<Employee> *data=nullptr;
    explicit StatsWidget(QWidget *p=nullptr):QWidget(p){setMinimumHeight(310); setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Preferred);}
protected:
    void paintEvent(QPaintEvent*) override {
        QPainter p(this); p.setRenderHint(QPainter::Antialiasing);
        p.setPen(QColor("#0B3B67"));
        int W=width(), H=height(), gap=12, cardW=(W-2*gap)/3;
        for(int c=0;c<3;c++){
            QRect r(c*(cardW+gap),0,cardW,H);
            p.setBrush(Qt::white); p.setPen(QColor("#E6DDD0")); p.drawRoundedRect(r,10,10);
        }
        QFont title=p.font(); title.setBold(true); title.setPointSize(11); p.setFont(title);
        p.setPen(QColor("#0B3B67"));
        p.drawText(15,28,"Répartition par poste (Présents)");
        p.drawText(cardW+gap+15,28,"Évolution des présences (7 derniers jours)");
        p.drawText(2*(cardW+gap)+15,28,"Top 5 des employés présents");

        int present=0; QMap<QString,int> by;
        if(data) for(const auto &e:*data) if(e.present){present++; by[e.poste]++;}
        QPoint center(105,142); int rad=62; QRectF pie(center.x()-rad,center.y()-rad,rad*2,rad*2);
        QList<QColor> colors={QColor("#0B3B67"),QColor("#3E96D2"),QColor("#8BD0E8"),QColor("#E6A84E")};
        double start=0; int k=0; int total=qMax(1,present);
        for(auto it=by.begin();it!=by.end();++it){
            double span=360.0*it.value()/total;
            p.setBrush(colors[k%colors.size()]); p.setPen(Qt::NoPen);
            p.drawPie(pie,start*16,span*16); start+=span; k++;
        }
        p.setBrush(Qt::white); p.drawEllipse(center,35,35);
        p.setPen(QColor("#0B3B67")); QFont big=p.font(); big.setPointSize(18); big.setBold(true); p.setFont(big);
        p.drawText(QRect(center.x()-35,center.y()-5,70,25),Qt::AlignCenter,QString::number(present));
        QFont sm=p.font(); sm.setPointSize(8); sm.setBold(false); p.setFont(sm);
        p.drawText(QRect(center.x()-45,center.y()+16,90,18),Qt::AlignCenter,"employés");
        int y=78; k=0; for(auto it=by.begin();it!=by.end() && k<5;++it,++k){
            p.setBrush(colors[k%colors.size()]); p.drawEllipse(cardW-155,y-8,14,14);
            p.setPen(QColor("#0B3B67")); p.drawText(cardW-132,y+4,it.key());
            p.drawText(cardW-42,y+4,QString::number(it.value())+" ("+QString::number(it.value()*100/qMax(1,present))+"%)");
            y+=30;
        }
        // Line chart
        int x0=cardW+gap+55, y0=H-40, chartW=cardW-85, chartH=H-90;
        p.setPen(QColor("#D5DEE7")); for(int i=0;i<5;i++){int yy=y0-i*chartH/4;p.drawLine(x0,yy,x0+chartW,yy);}
        QList<int> vals={700,1450,2050,2500,3050,3600};
        p.setPen(QColor("#0B3B67")); p.setBrush(QColor("#DCEAF5"));
        QPolygon poly; poly<<QPoint(x0,y0);
        for(int i=0;i<vals.size();i++){int xx=x0+i*chartW/(vals.size()-1); int yy=y0-vals[i]*chartH/4000; poly<<QPoint(xx,yy);}
        poly<<QPoint(x0+chartW,y0); p.drawPolygon(poly);
        p.setBrush(QColor("#0B3B67")); p.setPen(QColor("#0B3B67"));
        for(int i=0;i<vals.size();i++){int xx=x0+i*chartW/(vals.size()-1);int yy=y0-vals[i]*chartH/4000;p.drawEllipse(xx-4,yy-4,8,8);}
        QStringList months={"Jan","Fév","Mar","Avr","Mai","Juin"};
        for(int i=0;i<months.size();i++){int xx=x0+i*chartW/(months.size()-1);p.drawText(xx-12,y0+20,months[i]);}
        // top 5
        QStringList names; if(data) for(const auto&e:*data) if(e.present) names<<e.nom+" "+e.prenom;
        int yy=72; for(int i=0;i<qMin(5,names.size());i++){p.setPen(QColor("#0B3B67"));p.drawText(2*(cardW+gap)+15,yy,names[i]);int bx=2*(cardW+gap)+150;p.setBrush(QColor("#17639A"));p.setPen(Qt::NoPen);p.drawRoundedRect(bx,yy-13,cardW-190,16,8,8);p.setPen(QColor("#0B3B67"));p.drawText(2*(cardW+gap)+cardW-48,yy,"100%");yy+=34;}
    }
};

static QString styleField(){return "QLineEdit,QComboBox{padding:8px;border:1px solid #D8DEE5;border-radius:7px;background:white;color:#0B3B67;}";}
static QString styleButton(const QString &bg,const QString &fg="white"){return QString("QPushButton{background:%1;color:%2;border:0;border-radius:7px;padding:10px;font-weight:700;}").arg(bg,fg);}

void SmartMarket::initEmploye(){
    m_employeesEmploye={
        {"EMP001","Ben Ali","Ahmed","Manager","3500","1234",true,QDateTime::currentDateTime()},
        {"EMP003","Trabelsi","Youssef","Designer","3000","1234",true,QDateTime::currentDateTime()},
        {"EMP004","Hmidi","Rania","Couturière","2200","1234",true,QDateTime::currentDateTime()},
        {"EMP005","Jaziri","Omar","Technicien","2500","1234",true,QDateTime::currentDateTime()}
    };
    buildUiEmploye(); refreshEmploye(); updateStatsEmploye();
}
void SmartMarket::buildUiEmploye(){
    QWidget *central=new QWidget; ui->layoutPEmplCRUD->addWidget(central);
    auto *root=new QVBoxLayout(central); root->setContentsMargins(0,0,0,0); root->setSpacing(0);
    auto *top=new QHBoxLayout; top->setContentsMargins(0,0,0,0);
    QLabel *side=new QLabel; side->setPixmap(QPixmap(":/assets/sidebar.png")); side->setScaledContents(true); side->setMinimumWidth(0); side->hide(); top->addWidget(side,0);
    QWidget *right=new QWidget; auto *rv=new QVBoxLayout(right);rv->setContentsMargins(0,0,0,0);rv->setSpacing(10);
    auto *bar=new QWidget;bar->setMinimumHeight(65);bar->setStyleSheet("background:#FBF8F2;");auto *bh=new QHBoxLayout(bar);
    QPushButton *menu=new QPushButton("☰");menu->setStyleSheet("font-size:25px;color:#0B3B67;border:0;background:transparent;");
    m_searchEmploye=new QLineEdit; m_searchEmploye->setPlaceholderText("Rechercher un employé, un poste, un salaire...");m_searchEmploye->setStyleSheet(styleField());
    QLabel *user=new QLabel("🔔   👤   Designer   ˅");user->setStyleSheet("font-size:16px;font-weight:700;color:#0B3B67;");
    bh->addWidget(menu);bh->addWidget(m_searchEmploye,1);bh->addWidget(user);rv->addWidget(bar);
    QLabel *banner=new QLabel;banner->setPixmap(QPixmap(":/assets/banner.png"));banner->setScaledContents(true);banner->setMinimumHeight(110);rv->addWidget(banner);

    QWidget *body=new QWidget; body->setStyleSheet("background:#F8F5EF;"); auto *bv=new QVBoxLayout(body);bv->setContentsMargins(12,0,12,0);bv->setSpacing(10);
    auto *split=new QSplitter(Qt::Horizontal);split->setChildrenCollapsible(false);
    QGroupBox *formBox=new QGroupBox("👤  Informations de l'employé");formBox->setStyleSheet("QGroupBox{background:white;border:1px solid #E4DBCF;border-radius:10px;margin-top:28px;padding-top:22px;font-size:17px;font-weight:700;color:#0B3B67;}QGroupBox::title{subcontrol-origin:margin;top:-1px;left:12px;padding:2px 8px;color:#0B3B67;background:#F8F5EF;}");
    auto *g=new QGridLayout(formBox);g->setContentsMargins(14,12,14,12);g->setVerticalSpacing(8);
    m_idEmploye=new QLineEdit;m_nomEmploye=new QLineEdit;m_prenomEmploye=new QLineEdit;m_salaryEmploye=new QLineEdit;m_passwordEmploye=new QLineEdit;m_passwordEmploye->setEchoMode(QLineEdit::Password);
    m_posteEmploye=new QComboBox;m_posteEmploye->addItems({"Manager","Designer","Couturière","Technicien","Responsable RH","Styliste"});
    for(QWidget*w:{(QWidget*)m_idEmploye,(QWidget*)m_nomEmploye,(QWidget*)m_prenomEmploye,(QWidget*)m_salaryEmploye,(QWidget*)m_passwordEmploye,(QWidget*)m_posteEmploye})w->setStyleSheet(styleField());
    int r=0;auto add=[&](const QString&l,QWidget*w){
    auto *lab=new QLabel(l);
    lab->setStyleSheet("color:#0B3B67;font-weight:700;font-size:14px;");
    g->addWidget(lab,r,0);
    g->addWidget(w,r++,1);
};
    add("ID Employé",m_idEmploye);add("Nom",m_nomEmploye);add("Prénom",m_prenomEmploye);add("Poste",m_posteEmploye);add("Salaire",m_salaryEmploye);add("Mot de passe",m_passwordEmploye);
    QLabel *photo=new QLabel;photo->setPixmap(QPixmap(":/assets/employee_photo.png").scaled(78,68,Qt::KeepAspectRatio,Qt::SmoothTransformation));photo->setStyleSheet("background:#F3E8D7;border-radius:7px;");
    g->addItem(new QSpacerItem(1,8,QSizePolicy::Minimum,QSizePolicy::Fixed),r++,0,1,2);g->addWidget(new QLabel("Photo"),r,0);g->addWidget(photo,r++,1);
    QPushButton *cv=new QPushButton("📄  Importer depuis CV");cv->setStyleSheet(styleButton("#1769AA"));g->addWidget(cv,r++,0,1,2);
    auto *hb=new QHBoxLayout;QPushButton*a=new QPushButton("＋ Ajouter"),*ed=new QPushButton("✎ Modifier"),*del=new QPushButton("🗑 Supprimer");a->setStyleSheet(styleButton("#0B3B67"));ed->setStyleSheet(styleButton("#7892AF"));del->setStyleSheet(styleButton("#F1DDBE","#0B3B67"));hb->addWidget(a);hb->addWidget(ed);hb->addWidget(del);g->addLayout(hb,r++,0,1,2);
    QPushButton*show=new QPushButton("◉ Afficher");show->setStyleSheet(styleButton("#DCE8F3","#0B3B67"));g->addWidget(show,r++,0,1,2);
    split->addWidget(formBox);

    QGroupBox *listBox=new QGroupBox("👥  Liste des employés");listBox->setStyleSheet(formBox->styleSheet());auto*lv=new QVBoxLayout(listBox);lv->setContentsMargins(10,12,10,10);
    auto *toprow=new QHBoxLayout;QLineEdit*search=new QLineEdit;search->setPlaceholderText("Rechercher un employé...");search->setStyleSheet(styleField());QComboBox*sort=new QComboBox;sort->addItems({"Nom","ID","Poste","État"});sort->setStyleSheet(styleField());toprow->addWidget(search,2);toprow->addWidget(new QLabel("Trier par :"));toprow->addWidget(sort);lv->addLayout(toprow);
    m_tableEmploye=new QTableWidget(0,8);m_tableEmploye->setHorizontalHeaderLabels({"","ID","Nom","Prénom","Poste","Salaire","État","Actions"});m_tableEmploye->setSelectionBehavior(QAbstractItemView::SelectRows);m_tableEmploye->setEditTriggers(QAbstractItemView::NoEditTriggers);m_tableEmploye->verticalHeader()->setVisible(false);m_tableEmploye->horizontalHeader()->setStretchLastSection(true);m_tableEmploye->setColumnWidth(0,38);m_tableEmploye->setColumnWidth(1,88);m_tableEmploye->setColumnWidth(2,105);m_tableEmploye->setColumnWidth(3,105);m_tableEmploye->setColumnWidth(4,110);m_tableEmploye->setColumnWidth(5,95);m_tableEmploye->setColumnWidth(6,105);m_tableEmploye->setColumnWidth(7,95);m_tableEmploye->setStyleSheet("QTableWidget{background:white;color:#0B3B67;gridline-color:#E2E5E8;alternate-background-color:#FFFFFF;}QHeaderView::section{background:#F7EBD8;color:#0B3B67;font-weight:700;padding:9px;border:0;}");lv->addWidget(m_tableEmploye,1);
    auto*loginrow=new QHBoxLayout;m_statusEmploye=new QLabel("● Aucun employé connecté");m_statusEmploye->setStyleSheet("background:#DDF5E5;color:#08783E;padding:9px;border-radius:7px;font-weight:700;");QPushButton*loginb=new QPushButton("Connexion employé");loginb->setStyleSheet(styleButton("#0B3B67"));QPushButton*logoutb=new QPushButton("Déconnexion");logoutb->setStyleSheet(styleButton("#D99A45"));loginrow->addWidget(m_statusEmploye,1);loginrow->addWidget(loginb);loginrow->addWidget(logoutb);lv->addLayout(loginrow);
    split->addWidget(listBox);split->setSizes({590,890});bv->addWidget(split,3);

    QGroupBox*statsBox=new QGroupBox("📊  Statistiques des employés   (Présents uniquement)");statsBox->setStyleSheet(formBox->styleSheet());auto*sv=new QVBoxLayout(statsBox);sv->setContentsMargins(8,12,8,10);m_statsEmploye=new StatsWidget; m_statsEmploye->data=&m_employeesEmploye;sv->addWidget(m_statsEmploye);statsBox->setMinimumHeight(350);bv->addWidget(statsBox,2);
    QScrollArea *scroll=new QScrollArea;
    scroll->setWidget(body);
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);
    scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scroll->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scroll->setStyleSheet("QScrollArea{background:#171717;border:0;} QScrollBar:vertical{width:14px;background:#252525;margin:0;} QScrollBar::handle:vertical{background:#888;border-radius:7px;min-height:55px;} QScrollBar::add-line:vertical,QScrollBar::sub-line:vertical{height:0;} QScrollBar::add-page:vertical,QScrollBar::sub-page:vertical{background:#252525;}");
    rv->addWidget(scroll,1);top->addWidget(right,84);root->addLayout(top,1);

    connect(cv,&QPushButton::clicked,this,&SmartMarket::importCVEmploye);connect(a,&QPushButton::clicked,this,&SmartMarket::addEmployeeEmploye);connect(ed,&QPushButton::clicked,this,&SmartMarket::modifyEmployeeEmploye);connect(del,&QPushButton::clicked,this,&SmartMarket::removeEmployeeEmploye);connect(show,&QPushButton::clicked,this,&SmartMarket::showEmployeeEmploye);connect(loginb,&QPushButton::clicked,this,&SmartMarket::loginEmploye);connect(logoutb,&QPushButton::clicked,this,&SmartMarket::logoutEmploye);connect(search,&QLineEdit::textChanged,this,&SmartMarket::searchChangedEmploye);connect(m_tableEmploye,&QTableWidget::cellClicked,this,&SmartMarket::rowClickedEmploye);
}
void SmartMarket::refreshEmploye(){
    m_tableEmploye->setRowCount(0);QString q=m_searchEmploye->text().trimmed().toLower();
    for(const auto&e:m_employeesEmploye){QString all=e.id+" "+e.nom+" "+e.prenom+" "+e.poste;if(!q.isEmpty()&&!all.toLower().contains(q))continue;int r=m_tableEmploye->rowCount();m_tableEmploye->insertRow(r);auto*c=new QTableWidgetItem; c->setCheckState(Qt::Unchecked);m_tableEmploye->setItem(r,0,c);m_tableEmploye->setItem(r,1,new QTableWidgetItem(e.id));m_tableEmploye->setItem(r,2,new QTableWidgetItem(e.nom));m_tableEmploye->setItem(r,3,new QTableWidgetItem(e.prenom));m_tableEmploye->setItem(r,4,new QTableWidgetItem(e.poste));m_tableEmploye->setItem(r,5,new QTableWidgetItem(e.salaire+" DT"));auto*st=new QTableWidgetItem(e.present?"● Présent":"○ Absent");st->setForeground(e.present?QBrush(QColor("#138A4B")):QBrush(QColor("#777")));m_tableEmploye->setItem(r,6,st);m_tableEmploye->setItem(r,7,new QTableWidgetItem("◉  ✎"));m_tableEmploye->setRowHeight(r,42);}
}
void SmartMarket::updateStatsEmploye(){int p=0;for(const auto&e:m_employeesEmploye)if(e.present)p++;m_statsEmploye->update();m_statsEmploye->repaint();}
int SmartMarket::selectedIndexEmploye()const{auto rs=m_tableEmploye->selectedRanges();if(rs.isEmpty())return -1;QString id=m_tableEmploye->item(rs.first().topRow(),1)->text();for(int i=0;i<m_employeesEmploye.size();i++)if(m_employeesEmploye[i].id==id)return i;return -1;}
void SmartMarket::fillFormEmploye(const Employee&e){m_idEmploye->setText(e.id);m_nomEmploye->setText(e.nom);m_prenomEmploye->setText(e.prenom);m_posteEmploye->setCurrentText(e.poste);m_salaryEmploye->setText(e.salaire);m_passwordEmploye->setText(e.password);}
Employee SmartMarket::formEmployeeEmploye()const{Employee e;e.id=m_idEmploye->text().trimmed().toUpper();e.nom=m_nomEmploye->text().trimmed();e.prenom=m_prenomEmploye->text().trimmed();e.poste=m_posteEmploye->currentText();e.salaire=m_salaryEmploye->text().trimmed();e.password=m_passwordEmploye->text();return e;}
void SmartMarket::rowClickedEmploye(int row,int){QString id=m_tableEmploye->item(row,1)->text();for(const auto&e:m_employeesEmploye)if(e.id==id){fillFormEmploye(e);break;}}
void SmartMarket::addEmployeeEmploye(){Employee e=formEmployeeEmploye();if(e.id.isEmpty()||e.nom.isEmpty()||e.prenom.isEmpty()){QMessageBox::warning(this,"Ajouter","ID, nom et prénom sont obligatoires.");return;}for(const auto&x:m_employeesEmploye)if(x.id==e.id){QMessageBox::warning(this,"Ajouter","ID déjà utilisé.");return;}m_employeesEmploye.append(e);refreshEmploye();updateStatsEmploye();}
void SmartMarket::modifyEmployeeEmploye(){int i=selectedIndexEmploye();if(i<0){QMessageBox::information(this,"Modifier","Sélectionnez un employé.");return;}Employee e=formEmployeeEmploye();e.present=m_employeesEmploye[i].present;e.lastLogin=m_employeesEmploye[i].lastLogin;m_employeesEmploye[i]=e;refreshEmploye();updateStatsEmploye();}
void SmartMarket::removeEmployeeEmploye(){int i=selectedIndexEmploye();if(i<0)return;if(QMessageBox::question(this,"Supprimer","Supprimer cet employé ?")==QMessageBox::Yes){m_employeesEmploye.removeAt(i);refreshEmploye();updateStatsEmploye();}}
void SmartMarket::showEmployeeEmploye(){int i=selectedIndexEmploye();if(i<0)return;auto&e=m_employeesEmploye[i];QMessageBox::information(this,"Fiche employé",QString("ID : %1\nNom : %2 %3\nPoste : %4\nSalaire : %5 DT\nÉtat : %6").arg(e.id,e.nom,e.prenom,e.poste,e.salaire,e.present?"Présent":"Absent"));}
void SmartMarket::loginEmploye(){LoginDialog d(this);if(d.exec()!=QDialog::Accepted)return;int i=-1;for(int k=0;k<m_employeesEmploye.size();k++)if(m_employeesEmploye[k].id==d.id()){i=k;break;}if(i<0){QMessageBox::warning(this,"Connexion","Employé introuvable.");return;}if(m_employeesEmploye[i].password!=d.password()){QMessageBox::warning(this,"Connexion","Mot de passe incorrect.");return;}m_employeesEmploye[i].present=true;m_employeesEmploye[i].lastLogin=QDateTime::currentDateTime();m_loggedIdEmploye=d.id();m_statusEmploye->setText("● Présent : "+m_employeesEmploye[i].nom+" "+m_employeesEmploye[i].prenom);refreshEmploye();updateStatsEmploye();}
void SmartMarket::logoutEmploye(){if(m_loggedIdEmploye.isEmpty())return;for(auto&e:m_employeesEmploye)if(e.id==m_loggedIdEmploye)e.present=false;m_loggedIdEmploye.clear();m_statusEmploye->setText("● Aucun employé connecté");refreshEmploye();updateStatsEmploye();}
void SmartMarket::searchChangedEmploye(const QString&){refreshEmploye();}
QString SmartMarket::readTextEmploye(const QString&path){QFile f(path);if(!f.open(QIODevice::ReadOnly|QIODevice::Text))return{};QTextStream t(&f);return t.readAll();}
void SmartMarket::parseCVEmploye(const QString&t){auto get=[&](const QStringList&ks){for(const auto&line:t.split(QRegularExpression("[\\r\\n]+"))){for(const auto&k:ks){QRegularExpression re("^\\s*"+QRegularExpression::escape(k)+"\\s*[:\\-]\\s*(.+)$",QRegularExpression::CaseInsensitiveOption);auto m=re.match(line);if(m.hasMatch())return m.captured(1).trimmed();}}return QString();};QString v=get({"ID","ID Employé"});if(!v.isEmpty())m_idEmploye->setText(v.toUpper());v=get({"Nom"});if(!v.isEmpty())m_nomEmploye->setText(v);v=get({"Prénom","Prenom"});if(!v.isEmpty())m_prenomEmploye->setText(v);v=get({"Poste","Fonction"});if(!v.isEmpty())m_posteEmploye->setCurrentText(v);v=get({"Salaire","Salary"});if(!v.isEmpty())m_salaryEmploye->setText(v);}
void SmartMarket::importCVEmploye(){QString p=QFileDialog::getOpenFileName(this,"Choisir un CV",{}, "CV texte (*.txt *.csv);Tous les fichiers (*.*)");if(p.isEmpty())return;QString t=readTextEmploye(p);if(t.isEmpty()){QMessageBox::warning(this,"CV","Impossible de lire le fichier.");return;}parseCVEmploye(t);QMessageBox::information(this,"CV importé","Les informations ont été placées dans le formulaire. Vérifiez puis cliquez sur Ajouter.");}
