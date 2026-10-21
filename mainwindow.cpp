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
#include <QtMath>

static const QString NAVY = "#082849";
static const QString BLUE = "#1B5FA7";
static const QString GOLD = "#D8A64B";

static QString cardStyle()
{
    return "QFrame { background:#FFFFFF; border:1px solid #E2E8F0; border-radius:10px; }";
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
                p.drawRect(QRectF(-2.0, -10.0, 4.0, 3.0));
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

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    formFrame(nullptr),
    formIdEdit(nullptr),
    formClientCodeEdit(nullptr),
    formClientNameEdit(nullptr),
    formAddressEdit(nullptr),
    formDateEdit(nullptr),
    formStatusCombo(nullptr),
    formAmountEdit(nullptr),
    formPreviewImage(nullptr),
    smartDeliveryDateLabel(nullptr),
    btnAdd(nullptr),
    isEditingMode(false),
    editingRow(-1),
    donutLabel(nullptr),
    legendLayout(nullptr),
    donutAnim(nullptr),
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
    resize(1440, 920);
    setMinimumSize(1100, 680);
    setWindowTitle("FASHIONOVA — Gestion des Commandes");

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
    main->setSpacing(8);

    main->addWidget(makeHeader(), 0);

    auto *middleWidget = new QWidget;
    middleWidget->setStyleSheet("background: transparent;");
    auto *middleHBox = new QHBoxLayout(middleWidget);
    middleHBox->setContentsMargins(0, 0, 0, 0);
    middleHBox->setSpacing(8);

    auto *leftCol = new QWidget;
    leftCol->setStyleSheet("background: transparent;");
    auto *leftVBox = new QVBoxLayout(leftCol);
    leftVBox->setContentsMargins(0, 0, 0, 0);
    leftVBox->setSpacing(8);

    leftVBox->addWidget(makeOrdersAndFormCombinedPanel());
    leftVBox->addWidget(makeMapPanel());

    middleHBox->addWidget(leftCol, 1);

    auto *detailsPanel = makeDetailsPanel();
    detailsPanel->setFixedWidth(295);
    middleHBox->addWidget(detailsPanel, 0);

    main->addWidget(middleWidget, 0);
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
    auto *hBox = new QHBoxLayout(container);
    hBox->setContentsMargins(0, 0, 0, 0);
    hBox->setSpacing(10);

    formFrame = new QFrame;
    formFrame->setStyleSheet(cardStyle());
    formFrame->setFixedWidth(440);

    auto *leftLayout = new QVBoxLayout(formFrame);
    leftLayout->setContentsMargins(12, 10, 12, 10);
    leftLayout->setSpacing(7);

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
    fieldsLayout->setSpacing(5);

    auto makeField = [](const QString &label, QWidget *editor) {
        auto *hl = new QHBoxLayout;
        auto *lbl = new QLabel(label);
        lbl->setFixedWidth(85);
        lbl->setStyleSheet("font-weight: 600; color: #334155; font-size: 10.5px;");
        hl->addWidget(lbl);
        hl->addWidget(editor, 1);
        return hl;
    };

    formIdEdit = new QLineEdit;
    formIdEdit->setPlaceholderText("CMD-009");
    fieldsLayout->addLayout(makeField("ID Commande", formIdEdit));

    formClientNameEdit = new QLineEdit;
    formClientNameEdit->setPlaceholderText("Sarah Ben Ali");
    fieldsLayout->addLayout(makeField("Nom Client", formClientNameEdit));

    formClientCodeEdit = new QLineEdit;
    formClientCodeEdit->setPlaceholderText("CL-009");
    fieldsLayout->addLayout(makeField("Code Client", formClientCodeEdit));

    formAddressEdit = new QLineEdit;
    formAddressEdit->setPlaceholderText("ex: Laayoun, Thala, Sbiba, Kasserine...");
    fieldsLayout->addLayout(makeField("Adresse / Ville", formAddressEdit));

    formDateEdit = new QDateEdit(QDate::currentDate());
    formDateEdit->setCalendarPopup(true);
    formDateEdit->setDisplayFormat("dd/MM/yyyy");
    fieldsLayout->addLayout(makeField("Date", formDateEdit));
    connect(formDateEdit, &QDateEdit::dateChanged, this, &MainWindow::recalculerDelaiLivraisonEstime);

    formStatusCombo = new QComboBox;
    formStatusCombo->addItems({"Prête en atelier", "En cours", "Livrée"});
    fieldsLayout->addLayout(makeField("Statut", formStatusCombo));
    connect(formStatusCombo, &QComboBox::currentTextChanged, this, &MainWindow::recalculerDelaiLivraisonEstime);

    formAmountEdit = new QLineEdit;
    formAmountEdit->setPlaceholderText("ex: 350.00");
    fieldsLayout->addLayout(makeField("Montant (TND)", formAmountEdit));

    // Smart Delivery Date
    smartDeliveryDateLabel = new QLabel;
    smartDeliveryDateLabel->setStyleSheet("background: #F1F5F9; border: 1px dashed #CBD5E1; border-radius: 5px; padding: 4px 6px; font-size: 10px; color: #082849; font-weight: 600;");
    fieldsLayout->addLayout(makeField("⏱ Livraison estimée", smartDeliveryDateLabel));

    bodyLayout->addLayout(fieldsLayout, 1);

    auto *previewCol = new QVBoxLayout;
    previewCol->setAlignment(Qt::AlignTop);

    formPreviewImage = new QLabel;
    formPreviewImage->setFixedSize(95, 115);
    formPreviewImage->setStyleSheet("background: #F8FAFC; border: 1px solid #E2E8F0; border-radius: 6px;");
    formPreviewImage->setAlignment(Qt::AlignCenter);
    QPixmap pInit(":/images/prod_veste.png");
    if (!pInit.isNull()) {
        formPreviewImage->setPixmap(pInit.scaled(85, 105, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    } else {
        formPreviewImage->setText("👗");
    }
    previewCol->addWidget(formPreviewImage);

    bodyLayout->addLayout(previewCol);
    leftLayout->addLayout(bodyLayout);

    // Bloc boutons d'action
    auto *actionsGrid = new QHBoxLayout;
    actionsGrid->setSpacing(6);

    btnAdd = new QPushButton("＋ Ajouter");
    btnAdd->setFixedHeight(28);
    btnAdd->setCursor(Qt::PointingHandCursor);
    btnAdd->setStyleSheet("QPushButton { background: #082849; color: white; border-radius: 4px; font-weight: bold; font-size: 10.5px; } QPushButton:hover { background: #1B5FA7; }");
    connect(btnAdd, &QPushButton::clicked, this, &MainWindow::onSaveOrderForm);

    auto *btnEdit = new QPushButton("✏ Modifier");
    btnEdit->setFixedHeight(28);
    btnEdit->setCursor(Qt::PointingHandCursor);
    btnEdit->setStyleSheet("QPushButton { background: #64748B; color: white; border-radius: 4px; font-weight: bold; font-size: 10.5px; } QPushButton:hover { background: #475569; }");
    connect(btnEdit, &QPushButton::clicked, this, &MainWindow::onBtnEditClicked);

    auto *btnDel = new QPushButton("🗑 Supprimer");
    btnDel->setFixedHeight(28);
    btnDel->setCursor(Qt::PointingHandCursor);
    btnDel->setStyleSheet("QPushButton { background: #EFE5D5; color: #3F3427; border: none; border-radius: 4px; font-weight: bold; font-size: 10.5px; } QPushButton:hover { background: #E5DAC6; }");
    connect(btnDel, &QPushButton::clicked, this, &MainWindow::onBtnDeleteClicked);

    actionsGrid->addWidget(btnAdd, 1);
    actionsGrid->addWidget(btnEdit, 1);
    actionsGrid->addWidget(btnDel, 1);

    leftLayout->addLayout(actionsGrid);
    hBox->addWidget(formFrame, 0);

    auto *rightFrame = new QFrame;
    rightFrame->setStyleSheet(cardStyle());
    auto *rightLayout = new QVBoxLayout(rightFrame);
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
            for(int c = 1; c <= 4 && !ok; ++c) {
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

    ordersTable = new QTableWidget(0, 8);
    ordersTable->setIconSize(QSize(24, 24));
    ordersTable->setHorizontalHeaderLabels({" ", "ID", "Code", "Nom Client", "Adresse", "Date", "Statut", "Montant"});
    ordersTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ordersTable->setSelectionMode(QAbstractItemView::SingleSelection);
    ordersTable->verticalHeader()->setVisible(false);
    ordersTable->setFixedHeight(200);

    ordersTable->setColumnWidth(0, 24);
    ordersTable->setColumnWidth(1, 75);
    ordersTable->setColumnWidth(2, 60);
    ordersTable->setColumnWidth(4, 110);
    ordersTable->setColumnWidth(5, 80);
    ordersTable->setColumnWidth(6, 80);
    ordersTable->setColumnWidth(7, 75);
    ordersTable->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch);

    connect(ordersTable, &QTableWidget::cellClicked, this, &MainWindow::selectOrder);
    rightLayout->addWidget(ordersTable, 1);

    auto *pageBar = new QHBoxLayout;
    auto *lblCnt = new QLabel("Synchronisation active avec Oracle XE");
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

    hBox->addWidget(rightFrame, 1);
    recalculerDelaiLivraisonEstime();
    return container;
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

void MainWindow::onBtnAddClicked()
{
    onResetOrderForm();
}

void MainWindow::onResetOrderForm()
{
    isEditingMode = false;
    editingRow = -1;

    formIdEdit->setText(QString("CMD-%1").arg(ordersTable->rowCount() + 1, 3, 10, QChar('0')));
    formClientCodeEdit->clear();
    formClientNameEdit->clear();
    formAddressEdit->clear();
    formDateEdit->setDate(QDate::currentDate());
    formStatusCombo->setCurrentIndex(0);
    formAmountEdit->clear();

    if (btnAdd) {
        btnAdd->setText("＋ Ajouter");
        btnAdd->setStyleSheet("QPushButton { background: #082849; color: white; border-radius: 4px; font-weight: bold; font-size: 10.5px; } QPushButton:hover { background: #1B5FA7; }");
    }

    recalculerDelaiLivraisonEstime();
    formClientNameEdit->setFocus();
}

void MainWindow::onBtnEditClicked()
{
    int r = ordersTable->currentRow();
    if (r < 0) {
        QMessageBox::warning(this, "Sélection requise", "Veuillez sélectionner une commande dans le tableau.");
        return;
    }

    isEditingMode = true;
    editingRow = r;

    formIdEdit->setText(ordersTable->item(r, 1) ? ordersTable->item(r, 1)->text() : "");
    formClientCodeEdit->setText(ordersTable->item(r, 2) ? ordersTable->item(r, 2)->text() : "");
    formClientNameEdit->setText(ordersTable->item(r, 3) ? ordersTable->item(r, 3)->text() : "");
    formAddressEdit->setText(ordersTable->item(r, 4) ? ordersTable->item(r, 4)->text() : "");

    QString dateTxt = ordersTable->item(r, 5) ? ordersTable->item(r, 5)->text() : "";
    QDate d = QDate::fromString(dateTxt.left(10), "dd/MM/yyyy");
    if (d.isValid()) formDateEdit->setDate(d);

    auto *badgeWidget = ordersTable->cellWidget(r, 6);
    if (badgeWidget) {
        auto *lbl = badgeWidget->findChild<QLabel*>();
        if (lbl) formStatusCombo->setCurrentText(lbl->text());
    }

    QString amt = ordersTable->item(r, 7) ? ordersTable->item(r, 7)->text().replace(" TND", "").replace(",", ".").trimmed() : "";
    formAmountEdit->setText(amt);

    if (btnAdd) {
        btnAdd->setText("💾 Enregistrer");
        btnAdd->setStyleSheet("QPushButton { background: #10B981; color: white; border-radius: 4px; font-weight: bold; font-size: 10.5px; } QPushButton:hover { background: #059669; }");
    }

    recalculerDelaiLivraisonEstime();
    formClientNameEdit->setFocus();
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
        if (cTmp.supprimer(id)) {
            actualiserTableau();
            onResetOrderForm();
            QMessageBox::information(this, "Suppression réussie", QString("La commande <b>%1</b> a été supprimée d'Oracle.").arg(id));
        } else {
            QMessageBox::critical(this, "Erreur", "Échec de la suppression de la commande dans Oracle.");
        }
    }
}

QPair<double, double> MainWindow::geolocaliserVille(const QString &nomVille)
{
    QString v = nomVille.toLower().trimmed();

    if (v.contains("laayoun") || v.contains("layoun") || v.contains("el ayoun")) return {35.5348, 8.8789};
    if (v.contains("thala") || v.contains("tala"))                               return {35.5714, 8.6703};
    if (v.contains("sbiba"))                                                     return {35.5431, 9.0734};
    if (v.contains("sbeitla") || v.contains("subaytila"))                       return {35.2289, 9.1294};
    if (v.contains("foussana"))                                                 return {35.3400, 8.6800};
    if (v.contains("feriana"))                                                  return {34.9542, 8.5708};
    if (v.contains("haidra") || v.contains("hydra"))                            return {35.5647, 8.4489};
    if (v.contains("jedelienne") || v.contains("jedliene"))                     return {35.5800, 8.9200};
    if (v.contains("majel"))                                                    return {34.6800, 8.8400};
    if (v.contains("ezzouhour"))                                                return {35.1800, 8.8300};
    if (v.contains("kasserine"))                                                return {35.1676, 8.8365};

    static const QMap<QString, QPair<double, double>> coords = {
        {"tunis", {36.8065, 10.1815}},
        {"ariana", {36.8665, 10.1647}},
        {"ben arous", {36.7533, 10.2222}},
        {"manouba", {36.8080, 10.0972}},
        {"marsa", {36.8782, 10.3247}},
        {"carthage", {36.8583, 10.3308}},
        {"lac", {36.8333, 10.2333}},
        {"nabeul", {36.4561, 10.7376}},
        {"hammamet", {36.4000, 10.6167}},
        {"kelibia", {36.8475, 11.0939}},
        {"sousse", {35.8256, 10.6369}},
        {"monastir", {35.7779, 10.8261}},
        {"mahdia", {35.5047, 11.0622}},
        {"kairouan", {35.6781, 10.0963}},
        {"sidi bouzid", {35.0382, 9.4849}},
        {"sfax", {34.7406, 10.7603}},
        {"bizerte", {37.2744, 9.8739}},
        {"beja", {36.7256, 9.1817}},
        {"jendouba", {36.5011, 8.7802}},
        {"tabarka", {36.9544, 8.7580}},
        {"kef", {36.1822, 8.7149}},
        {"siliana", {36.0850, 9.3708}},
        {"zaghouan", {36.4028, 10.1429}},
        {"gafsa", {34.4250, 8.7842}},
        {"tozeur", {33.9197, 8.1335}},
        {"kebili", {33.7044, 8.9690}},
        {"gabes", {33.8815, 10.0982}},
        {"medenine", {33.3549, 10.5055}},
        {"djerba", {33.8075, 10.8451}},
        {"zarzis", {33.5040, 11.1122}},
        {"tataouine", {32.9297, 10.4518}}
    };

    for (auto it = coords.begin(); it != coords.end(); ++it) {
        if (v.contains(it.key())) {
            return it.value();
        }
    }

    return {35.5348, 8.8789};
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

void MainWindow::onSaveOrderForm()
{
    QString id = formIdEdit->text().trimmed();
    QString client = formClientCodeEdit->text().trimmed();
    QString name = formClientNameEdit->text().trimmed();
    QString address = formAddressEdit->text().trimmed();
    QDate dateCmd = formDateEdit->date();
    QString status = formStatusCombo->currentText();
    QString amountStr = formAmountEdit->text().trimmed().replace(" TND", "").replace(",", ".");

    if (id.isEmpty() || name.isEmpty() || amountStr.isEmpty()) {
        QMessageBox::warning(this, "Champs requis", "Veuillez remplir l'ID, le nom du client et le montant.");
        return;
    }

    if (client.isEmpty()) client = "CL-001";
    if (address.isEmpty()) address = "Laayoun";
    double montant = amountStr.toDouble();

    // 1. GESTION AUTOMATIQUE DU CLIENT :
    // Si le code client n'existe pas encore dans la table CLIENTS, on l'ajoute automatiquement
    QSqlQuery checkClientQuery;
    checkClientQuery.prepare("SELECT COUNT(*) FROM CLIENTS WHERE CODE_CLIENT = :code");
    checkClientQuery.bindValue(":code", client);
    if (checkClientQuery.exec() && checkClientQuery.next()) {
        if (checkClientQuery.value(0).toInt() == 0) {
            // Le client n'existe pas encore, on l'insère dans CLIENTS
            QSqlQuery insertClientQuery;
            insertClientQuery.prepare("INSERT INTO CLIENTS (CODE_CLIENT, NOM_PRENOM, ADRESSE) VALUES (:code, :nom, :adr)");
            insertClientQuery.bindValue(":code", client);
            insertClientQuery.bindValue(":nom", name);
            insertClientQuery.bindValue(":adr", address);
            insertClientQuery.exec();
        }
    }

    // 2. Géolocalisation et création de la commande
    QPair<double, double> geo = geolocaliserVille(address);

    Commande cmd(id, client, name, dateCmd, status, montant,
                 "Robe Élégante", "M", 1, "Carte bancaire",
                 address, geo.first, geo.second);

    if (isEditingMode) {
        if (cmd.modifier()) {
            QMessageBox::information(this, "Modification réussie", QString("La commande <b>%1</b> a été mise à jour dans Oracle !").arg(id));
            actualiserTableau();
            onResetOrderForm();
        } else {
            QMessageBox::critical(this, "Erreur", "Échec de la modification dans Oracle.");
        }
    } else {
        if (cmd.ajouter()) {
            QMessageBox::information(this, "Ajout réussi", QString("La commande <b>%1</b> a été enregistrée avec succès dans Oracle !").arg(id));
            actualiserTableau();
            onResetOrderForm();
        } else {
            QMessageBox::critical(this, "Erreur", "Échec de l'ajout dans Oracle. Vérifiez si cet ID n'existe pas déjà.");
        }
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
        QString addr = ordersTable->item(r, 4) ? ordersTable->item(r, 4)->text() : "";
        QString amount = ordersTable->item(r, 7) ? ordersTable->item(r, 7)->text() : "";

        auto *badgeWidget = ordersTable->cellWidget(r, 6);
        QString status = "Prête en atelier";
        if (badgeWidget) {
            auto *lbl = badgeWidget->findChild<QLabel*>();
            if (lbl) status = lbl->text();
        }

        QPair<double, double> coords = geolocaliserVille(addr);

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

    for (int r = 0; r < ordersTable->rowCount(); ++r) {
        auto *badgeWidget = ordersTable->cellWidget(r, 6);
        QString status = "Prête en atelier";
        if (badgeWidget) {
            auto *lbl = badgeWidget->findChild<QLabel*>();
            if (lbl) status = lbl->text();
        }

        if (status.contains("Livr")) animCountLivree++;
        else if (status.contains("cours")) animCountEnCours++;
        else animCountPrete++;

        if (ordersTable->item(r, 7)) {
            QString mStr = ordersTable->item(r, 7)->text().replace(" TND", "").replace(",", ".").trimmed();
            sommeMontants += mStr.toDouble();
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
            dot->setStyleSheet(QString("color:%1; font-size:10px;").arg(col));
            auto *lt = new QLabel(text);
            lt->setStyleSheet("font-size:9.5px; color:#475569;");
            auto *lv = new QLabel(QString("<b>%1%</b>").arg(QString::number(val, 'f', 0)));
            lv->setStyleSheet("font-size:9.5px; color:#0F172A;");
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
}

void MainWindow::onDonutAnimationStep(const QVariant &val)
{
    dessinerDonut(val.toDouble());
}

void MainWindow::dessinerDonut(double progress)
{
    if (!donutLabel) return;

    QPixmap px(70, 70);
    px.fill(Qt::transparent);
    QPainter p(&px);
    p.setRenderHint(QPainter::Antialiasing);
    QRectF r(5, 5, 60, 60);

    p.setPen(QPen(QColor(0xE2, 0xE8, 0xF0), 7));
    p.drawArc(r, 0, 360 * 16);

    if (animTotalCmd > 0) {
        double spanLivree  = (animCountLivree * 360.0 / animTotalCmd) * progress;
        double spanEnCours = (animCountEnCours * 360.0 / animTotalCmd) * progress;
        double spanPrete   = (animCountPrete * 360.0 / animTotalCmd) * progress;

        double currentAngle = 90.0;

        if (spanLivree > 0) {
            p.setPen(QPen(QColor(0x10, 0xB9, 0x81), 7));
            p.drawArc(r, static_cast<int>(currentAngle * 16), static_cast<int>(spanLivree * 16));
            currentAngle += spanLivree;
        }

        if (spanEnCours > 0) {
            p.setPen(QPen(QColor(0xF5, 0x9E, 0x0B), 7));
            p.drawArc(r, static_cast<int>(currentAngle * 16), static_cast<int>(spanEnCours * 16));
            currentAngle += spanEnCours;
        }

        if (spanPrete > 0) {
            p.setPen(QPen(QColor(0x25, 0x63, 0xEB), 7));
            p.drawArc(r, static_cast<int>(currentAngle * 16), static_cast<int>(spanPrete * 16));
        }
    }

    p.setPen(QColor(NAVY));
    QFont fDonut("Segoe UI");
    fDonut.setPixelSize(8);
    fDonut.setBold(true);
    p.setFont(fDonut);
    p.drawText(r, Qt::AlignCenter, QString("%1\ncmd").arg(animTotalCmd));
    p.end();

    donutLabel->setPixmap(px);
}

void MainWindow::setupOrders()
{
    QSqlQuery q("SELECT ID_COMMANDE, CODE_CLIENT, NOM_CLIENT, "
                "NVL(ADRESSE_LIVRAISON, 'Laayoun') AS ADRESSE_LIVRAISON, "
                "TO_CHAR(DATE_COMMANDE, 'DD/MM/YYYY') AS DATE_COMMANDE, "
                "STATUT, MONTANT, NVL(LATITUDE, 35.5348), NVL(LONGITUDE, 8.8789) "
                "FROM COMMANDES ORDER BY ID_COMMANDE ASC");

    ordersTable->setRowCount(0);
    int r = 0;

    while (q.next()) {
        ordersTable->insertRow(r);

        QString id = q.value(0).toString();
        QString client = q.value(1).toString();
        QString name = q.value(2).toString();
        QString addr = q.value(3).toString();
        QString date = q.value(4).toString();
        QString status = q.value(5).toString();
        double amountVal = q.value(6).toDouble();
        QString amount = QString::number(amountVal, 'f', 2) + " TND";

        auto *chkWidget = new QWidget;
        auto *chkLayout = new QHBoxLayout(chkWidget);
        chkLayout->setContentsMargins(0, 0, 0, 0);
        chkLayout->setAlignment(Qt::AlignCenter);
        chkLayout->addWidget(new QCheckBox);
        ordersTable->setCellWidget(r, 0, chkWidget);

        auto *idItem = new QTableWidgetItem(id);
        QString imgName = "prod_veste.png";
        if (id == "CMD-002") imgName = "prod_lunette.png";
        else if (id == "CMD-005") imgName = "prod_bijoux.png";
        else if (id == "CMD-008") imgName = "prod_chaussure.png";

        QPixmap pm(":/images/" + imgName);
        if (!pm.isNull()) idItem->setIcon(QIcon(pm.scaled(24, 24, Qt::KeepAspectRatio, Qt::SmoothTransformation)));

        QFont fId("Segoe UI");
        fId.setPointSizeF(8.5);
        fId.setBold(true);
        idItem->setFont(fId);
        ordersTable->setItem(r, 1, idItem);

        ordersTable->setItem(r, 2, new QTableWidgetItem(client));
        ordersTable->setItem(r, 3, new QTableWidgetItem(name));
        ordersTable->setItem(r, 4, new QTableWidgetItem(addr));
        ordersTable->setItem(r, 5, new QTableWidgetItem(date));

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
        ordersTable->setCellWidget(r, 6, badgeWidget);

        auto *amtItem = new QTableWidgetItem(amount);
        QFont fAmt("Segoe UI");
        fAmt.setPointSizeF(8.0);
        fAmt.setWeight(QFont::DemiBold);
        amtItem->setFont(fAmt);
        ordersTable->setItem(r, 7, amtItem);

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
    pdfBtn->setStyleSheet("QPushButton{background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #F3D082, stop:1 #C89538); color:#08213B; border:none; border-radius:5px; font-weight:bold; font-size:10.5px;} QPushButton:hover{background:#FFE4A3;}");
    connect(pdfBtn, &QPushButton::clicked, this, &MainWindow::generateInvoicePdf);
    actionBtnsLayout->addWidget(pdfBtn, 1);

    v->addLayout(actionBtnsLayout);

    auto *est = new QFrame;
    est->setStyleSheet("QFrame{background:#FFFDF7; border:1px solid #F3E4C8; border-radius:6px;}");
    auto *eg = new QGridLayout(est);
    eg->setContentsMargins(8, 6, 8, 6);
    eg->setSpacing(4);

    eg->addWidget(new QLabel("<b style='color:#B45309; font-size:10.5px;'>◎  Itinéraire multimodal & Coûts</b>"), 0, 0, 1, 2);

    distanceLabel = new QLabel;
    costLabel     = new QLabel;
    delayLabel    = new QLabel;

    eg->addWidget(new QLabel("<span style='color:#78350F; font-size:10px;'>Distance atelier → client</span>"), 1, 0);
    eg->addWidget(distanceLabel, 1, 1, Qt::AlignRight);

    eg->addWidget(new QLabel("<span style='color:#78350F; font-size:10px;'>Frais de livraison estimés</span>"), 2, 0);
    eg->addWidget(costLabel, 2, 1, Qt::AlignRight);

    eg->addWidget(new QLabel("<span style='color:#78350F; font-size:10px;'>Temps de trajet estimé</span>"), 3, 0);
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

void MainWindow::generateInvoicePdf()
{
    int r = currentRowSelected;
    if (r < 0 || r >= ordersTable->rowCount()) return;

    QString id     = ordersTable->item(r, 1)->text();
    QString client = ordersTable->item(r, 2)->text();
    QString name   = ordersTable->item(r, 3)->text();
    QString addr   = ordersTable->item(r, 4)->text();
    QString date   = ordersTable->item(r, 5)->text();
    QString amount = ordersTable->item(r, 7)->text();

    QString defaultName = QString("Facture_Fashionova_%1.pdf").arg(id);
    QString fileName = QFileDialog::getSaveFileName(this, "Enregistrer la Facture PDF", defaultName, "Documents PDF (*.pdf)");
    if (fileName.isEmpty()) return;

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
        </body>
        </html>
    )").arg(id, name, client, addr, date,
                            QString::number(amount.split(" ").first().replace(",", ".").toDouble() / 1.19, 'f', 2),
                            amount,
                            QString::number(amount.split(" ").first().replace(",", ".").toDouble() - (amount.split(" ").first().replace(",", ".").toDouble() / 1.19), 'f', 2));

    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(fileName);
    printer.setPageSize(QPageSize(QPageSize::A4));
    printer.setPageMargins(QMarginsF(12, 12, 12, 12), QPageLayout::Millimeter);

    QTextDocument doc;
    doc.setHtml(html);
    doc.print(&printer);

    QMessageBox::information(this, "Facture Générée",
                             QString("La facture officielle pour la commande <b>%1</b> a été enregistrée avec succès !").arg(id));
}

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

        auto *trendLbl = new QLabel("Synchronisé Oracle");
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
    w->setFixedHeight(160);
    auto *g = new QGridLayout(w);
    g->setContentsMargins(0, 0, 0, 10);
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
    donutContent->addWidget(donutLabel);

    legendLayout = new QVBoxLayout;
    legendLayout->setSpacing(2);
    donutContent->addLayout(legendLayout, 1);
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
    addAlert("🟢", "Délégations Kasserine synchronisées");
    addAlert("🟠", "Stock faible : Manteaux (5 restants)");

    g->addWidget(donut, 0, 0);
    g->addWidget(products, 0, 1);
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

QString MainWindow::getApiKey()
{
    QFile file("api_key.txt");
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QString key = QTextStream(&file).readLine().trimmed();
        if (!key.isEmpty()) return key;
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
             ordersTable->item(r, 4)->text(),
             ordersTable->item(r, 5)->text(),
             ordersTable->cellWidget(r, 6)->findChild<QLabel*>()->text(),
             ordersTable->item(r, 7)->text());
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
                auto *badgeWidget = ordersTable->cellWidget(r, 6);
                if (badgeWidget) {
                    auto *badge = badgeWidget->findChild<QLabel*>();
                    if (badge) {
                        if (newStatus.contains("livr")) {
                            newStatus = "Livrée";
                        } else if (newStatus.contains("cours")) {
                            newStatus = "En cours";
                        } else {
                            newStatus = "Prête en atelier";
                        }

                        QSqlQuery qUpdate;
                        qUpdate.prepare("UPDATE COMMANDES SET STATUT = :st WHERE ID_COMMANDE = :id");
                        qUpdate.bindValue(":st", newStatus);
                        qUpdate.bindValue(":id", targetId);
                        qUpdate.exec();

                        actualiserTableau();
                        return QString("⚡ <b>Statut mis à jour dans Oracle et sur la carte :</b> La commande <b>%1</b> est désormais <b>%2</b>.").arg(targetId, newStatus);
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
            QString addr   = ordersTable->item(r, 4) ? ordersTable->item(r, 4)->text().toLower() : "";
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
                     ordersTable->item(matchedRow, 4)->text(),
                     distanceLabel ? distanceLabel->text() : "—",
                     delayLabel ? delayLabel->text() : "—");
        }

        return QString("🎯 <b>Commande sélectionnée :</b> <b>%1</b> (%2)<br>"
                       "• Localisation : <b>%3</b><br>"
                       "• Montant : <b>%4</b><br>"
                       "• Statut : <i>%5</i>.")
            .arg(ordersTable->item(matchedRow, 1)->text(),
                 ordersTable->item(matchedRow, 3)->text(),
                 ordersTable->item(matchedRow, 4)->text(),
                 ordersTable->item(matchedRow, 7)->text(),
                 ordersTable->cellWidget(matchedRow, 6)->findChild<QLabel*>()->text());
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
    const QString addr   = ordersTable->item(row, 4) ? ordersTable->item(row, 4)->text() : "";
    const QString date   = ordersTable->item(row, 5) ? ordersTable->item(row, 5)->text() : "";
    const QString amount = ordersTable->item(row, 7) ? ordersTable->item(row, 7)->text() : "";

    selectedOrderLabel->setText(id);

    QPixmap p(":/images/prod_veste.png");
    if(id == "CMD-002") p = QPixmap(":/images/prod_lunette.png");
    else if(id == "CMD-005") p = QPixmap(":/images/prod_bijoux.png");
    else if(id == "CMD-008") p = QPixmap(":/images/prod_chaussure.png");

    if(!p.isNull()) {
        detailProductImage->setPixmap(p.scaled(60, 60, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        if (formPreviewImage) formPreviewImage->setPixmap(p.scaled(85, 105, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }

    auto *badgeWidget = ordersTable->cellWidget(row, 6);
    QString stText = "En cours";
    if (badgeWidget) {
        auto *lbl = badgeWidget->findChild<QLabel*>();
        if (lbl) stText = lbl->text();
    }

    detailClient->setText(QString("👤  Client : <b>%1</b> <span style='color:#64748B;'>(%2)</span>").arg(name, client));
    detailDate->setText(QString("🕒  Date : <b>%1</b>").arg(date));
    detailStatus->setText(QString("●  Statut : <b style='color:#2563EB;'>%1</b>").arg(stText));
    detailAmount->setText(QString("💰  Montant : <b style='color:#0F172A;'>%1</b>").arg(amount));
    detailAddress->setText(QString("📍  Livraison : <b>%1</b>").arg(addr.isEmpty() ? "Laayoun" : addr));

    QPair<double, double> geo = geolocaliserVille(addr);
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
        "- Pour modifier une commande : l'utilisateur sélectionne la commande, clique sur '✏ Modifier', modifie les champs dans le panneau de gauche, puis clique sur le bouton qui devient '💾 Enregistrer'.\n"
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