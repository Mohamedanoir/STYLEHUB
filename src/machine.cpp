#include "machine.h"

#include "connection.h"

#include "ui_machine.h"

#include "ui_mainwindow.h"

#include "ui_detailsdialog.h"

// ================= source fusionné : machine.cpp =================
int Machine::joursRestants() const
{
    return QDate::currentDate().daysTo(prochaineMaintenance);
}

bool Machine::indisponible() const
{
    return etat == Etat::indisponible();
}

QString Machine::raisonMaintenance() const
{
    const int jours = joursRestants();
    if (indisponible())
        return QStringLiteral("Machine indisponible – maintenance requise");
    if (jours < 0)
        return QStringLiteral("Maintenance en retard de %1 j").arg(-jours);
    if (etat == Etat::maintenance())
        return QStringLiteral("Maintenance en cours");
    if (jours <= 7)
        return QStringLiteral("Maintenance à prévoir sous %1 j").arg(jours);
    return QString();
}

bool Machine::maintenanceRequise() const
{
    return !raisonMaintenance().isEmpty();
}

bool Machine::critique() const
{
    return indisponible() || joursRestants() < 0;
}

// ================= source fusionné : bannerwidget.cpp =================
#include <QFontMetrics>
#include <QImage>
#include <QLinearGradient>
#include <QPainter>
#include <QPainterPath>
#include <QPolygonF>

static QPixmap faded(const QPixmap &src, qreal leftFrac, qreal rightFrac)
{
    QImage img = src.toImage().convertToFormat(QImage::Format_ARGB32_Premultiplied);
    QPainter p(&img);
    p.setCompositionMode(QPainter::CompositionMode_DestinationIn);
    QLinearGradient g(0, 0, img.width(), 0);
    g.setColorAt(0, leftFrac > 0 ? QColor(0, 0, 0, 0) : QColor(0, 0, 0, 255));
    if (leftFrac > 0)  g.setColorAt(leftFrac, QColor(0, 0, 0, 255));
    if (rightFrac > 0) { g.setColorAt(1 - rightFrac, QColor(0, 0, 0, 255)); g.setColorAt(1, QColor(0, 0, 0, 0)); }
    else               g.setColorAt(1, QColor(0, 0, 0, 255));
    p.fillRect(img.rect(), g);
    p.end();
    return QPixmap::fromImage(img);
}

BannerWidget::BannerWidget(QWidget *parent) : QWidget(parent)
{
    setFixedHeight(132);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
}

void BannerWidget::resizeEvent(QResizeEvent *)
{
    rebuild();
}

void BannerWidget::rebuild()
{
    const int h = height();
    QPixmap fabric(QStringLiteral(":/images/banner_fabric.jpg"));
    QPixmap machine(QStringLiteral(":/images/banner_machine.jpg"));
    m_fabric  = faded(fabric.scaledToHeight(h, Qt::SmoothTransformation), 0.0, 0.55);
    m_machine = faded(machine.scaledToHeight(h, Qt::SmoothTransformation), 0.25, 0.12);
}

void BannerWidget::paintEvent(QPaintEvent *)
{
    if (m_fabric.isNull()) rebuild();

    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    p.setRenderHint(QPainter::SmoothPixmapTransform);

    const qreal w = width(), h = height();

    QLinearGradient g(0, 0, w, 0);
    g.setColorAt(0.00, QColor("#d9cdb9"));
    g.setColorAt(0.20, QColor("#ebe0cd"));
    g.setColorAt(0.55, QColor("#f2e9d9"));
    g.setColorAt(1.00, QColor("#e6d8c0"));
    p.fillRect(rect(), g);

    p.drawPixmap(0, 0, m_fabric);
    p.drawPixmap(int(w * 0.81 - m_machine.width() + 14), 0, m_machine);

    // --- Titre (engrenage + texte)
    QFont title(QStringLiteral("Georgia"));
    title.setStyleHint(QFont::Serif);
    title.setBold(true);
    title.setPixelSize(int(h * 0.36));
    QFontMetrics tm(title);
    const QString t = QStringLiteral("Gestion des Machines");
    const int gear = int(h * 0.34);
    const qreal totalW = gear + 12 + tm.horizontalAdvance(t);
    const qreal cx = w * 0.385;
    qreal x0 = cx - totalW / 2;
    const qreal baseY = h * 0.46;

    p.drawPixmap(int(x0), int(baseY - gear * 0.78), UI::svgPixmap(QStringLiteral("gear"), QColor("#0f2b50"), gear));
    p.setFont(title);
    p.setPen(QColor("#13233f"));
    p.drawText(QPointF(x0 + gear + 12, baseY + tm.ascent() * 0.36), t);

    // --- Sous-titre
    QFont sub(QStringLiteral("Georgia"));
    sub.setStyleHint(QFont::Serif);
    sub.setItalic(true);
    sub.setPixelSize(int(h * 0.135));
    p.setFont(sub);
    p.setPen(QColor("#a98652"));
    p.drawText(QRectF(cx - w * 0.2, h * 0.58, w * 0.4, h * 0.18), Qt::AlignCenter,
               QStringLiteral("Contrôlez  ·  Suivez  ·  Anticipez"));

    // --- Séparateur avec étoile
    const qreal ly = h * 0.86;
    p.setPen(QPen(QColor("#b89a68"), 1));
    p.drawLine(QPointF(cx - 130, ly), QPointF(cx - 14, ly));
    p.drawLine(QPointF(cx + 14, ly), QPointF(cx + 130, ly));
    QPolygonF star;
    const qreal r = 8, ri = 2.4;
    star << QPointF(cx, ly - r) << QPointF(cx + ri, ly - ri) << QPointF(cx + r, ly) << QPointF(cx + ri, ly + ri)
         << QPointF(cx, ly + r) << QPointF(cx - ri, ly + ri) << QPointF(cx - r, ly) << QPointF(cx - ri, ly - ri);
    p.setPen(Qt::NoPen);
    p.setBrush(QColor("#13233f"));
    p.drawPolygon(star);

    // --- Citation
    const qreal qx = w * 0.825, qw = w * 0.15;
    QFont q(QStringLiteral("Georgia"));
    q.setStyleHint(QFont::Serif);
    q.setItalic(true);
    q.setPixelSize(int(h * 0.135));
    p.setFont(q);
    p.setPen(QColor("#9a7a45"));
    p.drawText(QRectF(qx, h * 0.22, qw, h * 0.56), Qt::AlignHCenter | Qt::AlignVCenter | Qt::TextWordWrap,
               QStringLiteral("Une machine bien gérée est une production sans arrêt"));
    QFont big(QStringLiteral("Georgia"));
    big.setBold(true);
    big.setPixelSize(int(h * 0.3));
    p.setFont(big);
    p.setPen(QColor("#c9a36a"));
    p.drawText(QPointF(qx - 18, h * 0.42), QStringLiteral("\u201C"));
    p.drawText(QPointF(w - 34, h * 0.86), QStringLiteral("\u201D"));
}

// ================= source fusionné : chartwidgets.cpp =================
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
    const qreal nameW = qMin<qreal>(86, width() * 0.30);
    const qreal pctW = 42;
    const QRectF track(nameW + 6, 0, width() - nameW - pctW - 18, 10);

    for (int i = 0; i < m_entries.size(); ++i) {
        const Entry &e = m_entries[i];
        const int pct = qBound(0, e.score, 100);
        const qreal cy = headH + rowH * i + rowH / 2;

        p.setFont(px(12, i == 0));
        p.setPen(QColor("#23252e"));
        p.drawText(QRectF(0, cy - 15, nameW, 20), Qt::AlignVCenter | Qt::AlignLeft,
                   QFontMetrics(p.font()).elidedText(e.type, Qt::ElideRight, int(nameW)));

        QRectF t = track; t.moveCenter(QPointF(track.center().x(), cy - 2));
        p.setPen(Qt::NoPen);
        p.setBrush(QColor("#eee8dc"));
        p.drawRoundedRect(t, 6, 6);
        if (pct > 0) {
            QRectF f = t; f.setWidth(qMax<qreal>(10, t.width() * pct / 100.0));
            p.setBrush(performanceColor(pct));
            p.drawRoundedRect(f, 6, 6);
        }

        p.setFont(px(12, true));
        p.setPen(QColor("#23252e"));
        p.drawText(QRectF(width() - pctW, cy - 15, pctW, 20), Qt::AlignVCenter | Qt::AlignRight,
                   QStringLiteral("%1 %").arg(pct));
        p.setFont(px(9));
        p.setPen(QColor("#6b6b6b"));
        p.drawText(QRectF(nameW + 6, cy + 3, width() - nameW - 12, 14), Qt::AlignVCenter | Qt::AlignLeft,
                   QStringLiteral("Disponibilité %1% · rendement relatif %2% · %3 machine(s)")
                       .arg(e.disponibilite).arg(e.production).arg(e.machines));
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

// ================= source fusionné : database.cpp =================
#include <QCoreApplication>
#include <QDate>
#include <QDebug>
#include <QDir>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QStandardPaths>
#include <QPair>
#include <QSet>
#include <QStringList>
#include <QVariant>

static const char *kIso = "yyyy-MM-dd";
static bool isOracleConnection()
{
    const QSqlDatabase db = QSqlDatabase::database(QSqlDatabase::defaultConnection, false);
    return db.isValid() && db.driverName() == QStringLiteral("QODBC") && db.isOpen();
}

static int oracleIdValue(QString id)
{
    id = id.trimmed();
    if (id.startsWith(QStringLiteral("MCH"), Qt::CaseInsensitive))
        id = id.mid(3);
    bool ok = false;
    const int value = id.toInt(&ok);
    return ok ? value : 0;
}

static Machine fromQuery(const QSqlQuery &q)
{
    Machine m;
    const QString rawId = q.value(0).toString();
    bool numericId = false;
    const int idValue = rawId.toInt(&numericId);
    m.id = numericId
        ? QStringLiteral("MCH%1").arg(idValue, 3, 10, QLatin1Char('0'))
        : rawId;
    m.nom   = q.value(1).toString();
    m.type  = q.value(2).toString();
    m.etat  = q.value(3).toString();
    m.prochaineMaintenance = QDate::fromString(q.value(4).toString(), QLatin1String(kIso));
    m.photo = q.value(5).toString();
    m.heuresFonctionnement = q.value(6).toDouble();
    m.piecesProduites = q.value(7).toInt();
    m.heuresArret = q.value(8).toDouble();
    return m;
}

static void seed()
{
    struct Row { const char *nom; const char *type; const char *etat; int jours; };
    const Row rows[] = {
        { "Machine à coudre industrielle", "Couture",  "Disponible",     45 },
        { "Surjeteuse",                    "Couture",  "Disponible",      5 },
        { "Machine de coupe",              "Coupe",    "En maintenance", -3 },
        { "Repasseuse",                    "Finition", "Disponible",     25 },
        { "Presse à repasser",             "Finition", "Disponible",     60 },
        { "Machine à broder",              "Broderie", "Disponible",     90 },
        { "Piqueuse plate",                "Couture",  "Disponible",     18 },
        { "Machine point droit",           "Couture",  "Disponible",     33 },
        { "Machine à boutonnières",        "Couture",  "Indisponible",  -12 },
        { "Coupeuse verticale",            "Coupe",    "Disponible",     12 },
        { "Table de coupe laser",          "Coupe",    "Disponible",     75 },
        { "Compresseur d'air",             "Autre",    "Disponible",     40 },
    };
    int n = 1;
    for (const Row &r : rows) {
        Machine m;
        m.id   = QStringLiteral("MCH%1").arg(n++, 3, 10, QLatin1Char('0'));
        m.nom  = QString::fromUtf8(r.nom);
        m.type = QString::fromUtf8(r.type);
        m.etat = QString::fromUtf8(r.etat);
        m.prochaineMaintenance = QDate::currentDate().addDays(r.jours);
        m.heuresFonctionnement = 120.0 + n * 17.0;
        m.piecesProduites = int(m.heuresFonctionnement * (12 + n % 5 * 3));
        m.heuresArret = 4.0 + (n % 4) * 2.0;
        Database::insert(m);
    }
}

bool Database::open(QString *error)
{
    const QString driver = qEnvironmentVariable("FASHIONOVA_DB_DRIVER", QStringLiteral("ORACLE")).trimmed().toUpper();
    if (driver != QStringLiteral("SQLITE")) {
        if (!Connection::open(error))
            return false;

        QSqlQuery verifyTable;
        if (!verifyTable.exec(QStringLiteral("SELECT ID_MACHINE FROM MACHINES WHERE 1=0"))) {
            if (error) *error = QStringLiteral("La table Oracle MACHINES est introuvable ou inaccessible : ") + verifyTable.lastError().text();
            return false;
        }

        QSet<QString> oracleColumns;
        QSqlQuery columns;
        columns.prepare(QStringLiteral(
            "SELECT COLUMN_NAME FROM USER_TAB_COLUMNS WHERE TABLE_NAME=?"));
        columns.addBindValue(QStringLiteral("MACHINES"));
        if (!columns.exec()) {
            if (error) *error = QStringLiteral("Impossible de vérifier les colonnes Oracle de MACHINES : ")
                              + columns.lastError().text();
            return false;
        }
        while (columns.next()) oracleColumns.insert(columns.value(0).toString().toUpper());
        const QStringList requiredColumns = {
            QStringLiteral("ID_MACHINE"), QStringLiteral("NOM_MACHINE"),
            QStringLiteral("TYPE_MACHINE"), QStringLiteral("ETAT"),
            QStringLiteral("DATE_PROCHAINE_MAINTENANCE"),
            QStringLiteral("HEURES_FONCTIONNEMENT"), QStringLiteral("PIECES_PRODUITES"),
            QStringLiteral("HEURES_ARRET")
        };
        QStringList missingColumns;
        for (const QString &column : requiredColumns)
            if (!oracleColumns.contains(column)) missingColumns.append(column);
        if (!missingColumns.isEmpty()) {
            if (error) *error = QStringLiteral("Colonnes manquantes dans la table Oracle MACHINES : %1. "
                                               "Ajoutez-les avec le script oracle_schema.sql.")
                              .arg(missingColumns.join(QStringLiteral(", ")));
            return false;
        }

        QSqlQuery q;
        const QStringList oracleDdl = {
            QStringLiteral("CREATE TABLE releve_machine ("
                           "ID_Releve NUMBER(10) PRIMARY KEY, ID_Machine NUMBER(10) NOT NULL, "
                           "Date_Releve VARCHAR2(10) NOT NULL, Heures_Planifiees NUMBER(10,2) NOT NULL, "
                           "Heures_Fonctionnement NUMBER(10,2) NOT NULL, Heures_Arret NUMBER(10,2) NOT NULL, "
                           "Quantite_Produite NUMBER(10) NOT NULL, Objectif_Production NUMBER(10) NOT NULL, "
                           "Cause_Arret VARCHAR2(1000) DEFAULT '', "
                           "CONSTRAINT uq_releve_machine_date UNIQUE(ID_Machine, Date_Releve), "
                           "CONSTRAINT fk_releve_machine FOREIGN KEY(ID_Machine) REFERENCES MACHINES(ID_MACHINE) ON DELETE CASCADE)"),
            QStringLiteral("CREATE TABLE maintenance ("
                           "ID_Maintenance NUMBER(10) PRIMARY KEY, ID_Machine NUMBER(10) NOT NULL, "
                           "Date_Prevue VARCHAR2(10) NOT NULL, Type_Intervention VARCHAR2(30) NOT NULL, "
                           "Priorite NUMBER(2) NOT NULL, Duree_Estimee_Heures NUMBER(10,2) NOT NULL, "
                           "Statut VARCHAR2(30) NOT NULL, Description VARCHAR2(1000) DEFAULT '', "
                           "Date_Realisee VARCHAR2(10), Duree_Reelle_Heures NUMBER(10,2) DEFAULT 0, Cout NUMBER(12,2) DEFAULT 0, "
                           "CONSTRAINT fk_maintenance_machine FOREIGN KEY(ID_Machine) REFERENCES MACHINES(ID_MACHINE) ON DELETE CASCADE)")
        };
        for (const QString &ddl : oracleDdl) {
            if (q.exec(ddl))
                continue;
            // ORA-00955: objet déjà créé; les tables existantes restent utilisables.
            if (!q.lastError().text().contains(QStringLiteral("ORA-00955"), Qt::CaseInsensitive)) {
                if (error) *error = q.lastError().text();
                return false;
            }
        }

        const QStringList sequenceDdl = {
            QStringLiteral("CREATE SEQUENCE seq_releve_machine START WITH 1 INCREMENT BY 1 NOCACHE"),
            QStringLiteral("CREATE SEQUENCE seq_maintenance START WITH 1 INCREMENT BY 1 NOCACHE")
        };
        for (const QString &ddl : sequenceDdl) {
            if (q.exec(ddl))
                continue;
            if (!q.lastError().text().contains(QStringLiteral("ORA-00955"), Qt::CaseInsensitive)) {
                if (error) *error = q.lastError().text();
                return false;
            }
        }
        return true;
    }

    const QString dir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(dir);

    QSqlDatabase db = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"));
    db.setDatabaseName(dir + QStringLiteral("/fashionova_machines.db"));
    if (!db.open()) {
        if (error) *error = db.lastError().text();
        return false;
    }

    QSqlQuery pragma;
    pragma.exec(QStringLiteral("PRAGMA foreign_keys = ON"));

    QSqlQuery q;
    if (!q.exec(QStringLiteral(
            "CREATE TABLE IF NOT EXISTS machine ("
            " ID_Machine TEXT PRIMARY KEY,"
            " Nom_Machine TEXT NOT NULL,"
            " Type_Machine TEXT NOT NULL,"
            " Etat TEXT NOT NULL,"
            " Date_Prochaine_Maintenance TEXT NOT NULL,"
            " Photo TEXT DEFAULT '',"
            " Heures_Fonctionnement REAL NOT NULL DEFAULT 0 CHECK(Heures_Fonctionnement >= 0),"
            " Pieces_Produites INTEGER NOT NULL DEFAULT 0 CHECK(Pieces_Produites >= 0),"
            " Heures_Arret REAL NOT NULL DEFAULT 0 CHECK(Heures_Arret >= 0))"))) {
        if (error) *error = q.lastError().text();
        return false;
    }

    if (!q.exec(QStringLiteral(
            "CREATE TABLE IF NOT EXISTS app_migration (MigrationKey TEXT PRIMARY KEY)"))) {
        if (error) *error = q.lastError().text();
        return false;
    }

    // Ajoute les trois attributs aux bases créées avec une version antérieure.
    QSet<QString> columns;
    QSqlQuery tableInfo(QStringLiteral("PRAGMA table_info(machine)"));
    while (tableInfo.next()) columns.insert(tableInfo.value(1).toString());
    const QList<QPair<QString, QString>> additions = {
        {QStringLiteral("Heures_Fonctionnement"), QStringLiteral("REAL NOT NULL DEFAULT 0")},
        {QStringLiteral("Pieces_Produites"), QStringLiteral("INTEGER NOT NULL DEFAULT 0")},
        {QStringLiteral("Heures_Arret"), QStringLiteral("REAL NOT NULL DEFAULT 0")}
    };
    for (const auto &addition : additions) {
        if (columns.contains(addition.first)) continue;
        QSqlQuery alter;
        if (!alter.exec(QStringLiteral("ALTER TABLE machine ADD COLUMN %1 %2")
                            .arg(addition.first, addition.second))) {
            if (error) *error = alter.lastError().text();
            return false;
        }
    }
    QSqlQuery migrationDone;
    migrationDone.prepare(QStringLiteral("SELECT 1 FROM app_migration WHERE MigrationKey=?"));
    migrationDone.addBindValue(QStringLiteral("machine_metrics_demo_v1"));
    if (!migrationDone.exec()) {
        if (error) *error = migrationDone.lastError().text();
        return false;
    }
    if (!migrationDone.next()) {
        QSqlQuery demo;
        if (!demo.exec(QStringLiteral(
                "UPDATE machine SET "
                "Heures_Fonctionnement=120 + CAST(SUBSTR(ID_Machine, 4) AS INTEGER) * 17, "
                "Pieces_Produites=(120 + CAST(SUBSTR(ID_Machine, 4) AS INTEGER) * 17) * "
                    "(12 + (CAST(SUBSTR(ID_Machine, 4) AS INTEGER) % 5) * 3), "
                "Heures_Arret=4 + (CAST(SUBSTR(ID_Machine, 4) AS INTEGER) % 4) * 2 "
                "WHERE Heures_Fonctionnement=0 AND Pieces_Produites=0 AND Heures_Arret=0"))) {
            if (error) *error = demo.lastError().text();
            return false;
        }
        QSqlQuery mark;
        mark.prepare(QStringLiteral("INSERT INTO app_migration(MigrationKey) VALUES(?)"));
        mark.addBindValue(QStringLiteral("machine_metrics_demo_v1"));
        if (!mark.exec()) {
            if (error) *error = mark.lastError().text();
            return false;
        }
    }

    if (!q.exec(QStringLiteral(
            "CREATE TABLE IF NOT EXISTS releve_machine ("
            " ID_Releve INTEGER PRIMARY KEY AUTOINCREMENT,"
            " ID_Machine TEXT NOT NULL,"
            " Date_Releve TEXT NOT NULL,"
            " Heures_Planifiees REAL NOT NULL CHECK(Heures_Planifiees > 0),"
            " Heures_Fonctionnement REAL NOT NULL CHECK(Heures_Fonctionnement >= 0),"
            " Heures_Arret REAL NOT NULL CHECK(Heures_Arret >= 0),"
            " Quantite_Produite INTEGER NOT NULL CHECK(Quantite_Produite >= 0),"
            " Objectif_Production INTEGER NOT NULL CHECK(Objectif_Production > 0),"
            " Cause_Arret TEXT DEFAULT '',"
            " UNIQUE(ID_Machine, Date_Releve),"
            " FOREIGN KEY(ID_Machine) REFERENCES machine(ID_Machine) ON DELETE CASCADE)"))) {
        if (error) *error = q.lastError().text();
        return false;
    }

    if (!q.exec(QStringLiteral(
            "CREATE TABLE IF NOT EXISTS maintenance ("
            " ID_Maintenance INTEGER PRIMARY KEY AUTOINCREMENT,"
            " ID_Machine TEXT NOT NULL,"
            " Date_Prevue TEXT NOT NULL,"
            " Type_Intervention TEXT NOT NULL CHECK(Type_Intervention IN ('Préventive','Corrective'))," 
            " Priorite INTEGER NOT NULL CHECK(Priorite BETWEEN 1 AND 5),"
            " Duree_Estimee_Heures REAL NOT NULL CHECK(Duree_Estimee_Heures > 0),"
            " Statut TEXT NOT NULL CHECK(Statut IN ('Planifiée','En cours','Réalisée','Annulée','Replanifiée'))," 
            " Description TEXT DEFAULT '',"
            " Date_Realisee TEXT,"
            " Duree_Reelle_Heures REAL DEFAULT 0 CHECK(Duree_Reelle_Heures >= 0),"
            " Cout REAL DEFAULT 0 CHECK(Cout >= 0),"
            " FOREIGN KEY(ID_Machine) REFERENCES machine(ID_Machine) ON DELETE CASCADE)"))) {
        if (error) *error = q.lastError().text();
        return false;
    }
    // Données de démonstration au premier lancement
    if (q.exec(QStringLiteral("SELECT COUNT(*) FROM machine")) && q.next() && q.value(0).toInt() == 0)
        seed();

    return true;
}

QList<Machine> Database::all()
{
    QList<Machine> list;
    const QSqlDatabase activeDb = QSqlDatabase::database(QSqlDatabase::defaultConnection, false);
    const bool oracle = activeDb.isValid() && activeDb.driverName() == QStringLiteral("QODBC");
    const QString sql = oracle
        ? QStringLiteral("SELECT ID_MACHINE, NOM_MACHINE, TYPE_MACHINE, ETAT, TO_CHAR(DATE_PROCHAINE_MAINTENANCE, 'YYYY-MM-DD'), '' AS PHOTO, NVL(HEURES_FONCTIONNEMENT,0), NVL(PIECES_PRODUITES,0), NVL(HEURES_ARRET,0) FROM MACHINES ORDER BY ID_MACHINE")
        : QStringLiteral("SELECT ID_Machine, Nom_Machine, Type_Machine, Etat, Date_Prochaine_Maintenance, Photo, Heures_Fonctionnement, Pieces_Produites, Heures_Arret FROM machine ORDER BY ID_Machine");
    QSqlQuery q;
    q.setForwardOnly(true);
    if (!q.exec(sql)) {
        qWarning() << "Database::all failed:" << q.lastError().text();
        return list;
    }
    while (q.next()) list.append(fromQuery(q));
    if (q.lastError().isValid())
        qWarning() << "Database::all fetch failed:" << q.lastError().text();
    return list;
}
QList<MachineReading> Database::allReadings()
{
    QList<MachineReading> result;
    QSqlQuery q(QStringLiteral(
        "SELECT ID_Releve, ID_Machine, Date_Releve, Heures_Planifiees, "
        "Heures_Fonctionnement, Heures_Arret, Quantite_Produite, Objectif_Production, Cause_Arret "
        "FROM releve_machine ORDER BY Date_Releve, ID_Machine"));
    while (q.next()) {
        MachineReading r;
        r.id = q.value(0).toInt();
        const QString rawMachineId = q.value(1).toString();
        r.machineId = isOracleConnection()
            ? QStringLiteral("MCH%1").arg(oracleIdValue(rawMachineId), 3, 10, QLatin1Char('0'))
            : rawMachineId;
        r.date = QDate::fromString(q.value(2).toString(), QLatin1String(kIso));
        r.plannedHours = q.value(3).toDouble();
        r.operatingHours = q.value(4).toDouble();
        r.downtimeHours = q.value(5).toDouble();
        r.quantityProduced = q.value(6).toInt();
        r.productionTarget = q.value(7).toInt();
        r.stopReason = q.value(8).toString();
        result.append(r);
    }
    return result;
}

QList<MaintenanceRecord> Database::allMaintenanceRecords()
{
    QList<MaintenanceRecord> result;
    QSqlQuery q(QStringLiteral(
        "SELECT ID_Maintenance, ID_Machine, Date_Prevue, Type_Intervention, Priorite, "
        "Duree_Estimee_Heures, Statut, Description FROM maintenance "
        "ORDER BY Date_Prevue, Priorite DESC, ID_Maintenance"));
    while (q.next()) {
        MaintenanceRecord r;
        r.id = q.value(0).toInt();
        const QString rawMachineId = q.value(1).toString();
        r.machineId = isOracleConnection()
            ? QStringLiteral("MCH%1").arg(oracleIdValue(rawMachineId), 3, 10, QLatin1Char('0'))
            : rawMachineId;
        r.plannedDate = QDate::fromString(q.value(2).toString(), QLatin1String(kIso));
        r.interventionType = q.value(3).toString();
        r.priority = q.value(4).toInt();
        r.estimatedHours = q.value(5).toDouble();
        r.status = q.value(6).toString();
        r.description = q.value(7).toString();
        result.append(r);
    }
    return result;
}

bool Database::insert(const Machine &m, QString *error)
{
    if (!MachineValidation::validate(m, error)) return false;

    QSqlQuery q;
    const QSqlDatabase activeDb = QSqlDatabase::database(QSqlDatabase::defaultConnection, false);
    const bool oracle = activeDb.isValid() && activeDb.driverName() == QStringLiteral("QODBC");
    if (oracle) {
        if (!activeDb.isOpen()) {
            if (error) *error = QStringLiteral("Connexion Oracle ODBC fermée avant l'insertion.");
            return false;
        }
        QString idText = m.id.trimmed();
        if (idText.startsWith(QStringLiteral("MCH"), Qt::CaseInsensitive))
            idText = idText.mid(3);

        bool idOk = false;
        const int id = idText.toInt(&idOk);
        if (!idOk || id <= 0) {
            if (error) *error = QStringLiteral("L'identifiant doit contenir un numéro positif, par exemple MCH001.");
            return false;
        }

        const QString sql = QStringLiteral(
            "INSERT INTO MACHINES (ID_MACHINE, NOM_MACHINE, TYPE_MACHINE, ETAT, "
            "DATE_PROCHAINE_MAINTENANCE, HEURES_FONCTIONNEMENT, PIECES_PRODUITES, HEURES_ARRET) "
            "VALUES (?, ?, ?, ?, TO_DATE(?, 'YYYY-MM-DD'), ?, ?, ?)");
        if (!q.prepare(sql)) {
            if (error) *error = QStringLiteral("PREPARE : ") + q.lastError().text();
            return false;
        }

        q.addBindValue(id);
        q.addBindValue(m.nom);
        q.addBindValue(m.type);
        q.addBindValue(m.etat);
        q.addBindValue(m.prochaineMaintenance.toString(QLatin1String(kIso)));
        q.addBindValue(m.heuresFonctionnement);
        q.addBindValue(m.piecesProduites);
        q.addBindValue(m.heuresArret);

        if (!q.exec()) {
            if (error) *error = QStringLiteral("EXEC : ") + q.lastError().text()
                          + QStringLiteral("\nSQL : ") + sql;
            return false;
        }
        return true;
    }

    q.prepare(QStringLiteral(
        "INSERT INTO machine (ID_Machine, Nom_Machine, Type_Machine, Etat, "
        "Date_Prochaine_Maintenance, Photo, Heures_Fonctionnement, Pieces_Produites, Heures_Arret) "
        "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)"));
    q.addBindValue(m.id);
    q.addBindValue(m.nom);
    q.addBindValue(m.type);
    q.addBindValue(m.etat);
    q.addBindValue(m.prochaineMaintenance.toString(QLatin1String(kIso)));
    q.addBindValue(m.photo);
    q.addBindValue(m.heuresFonctionnement);
    q.addBindValue(m.piecesProduites);
    q.addBindValue(m.heuresArret);

    if (!q.exec()) {
        if (error) *error = q.lastError().text();
        return false;
    }
    return true;
}
bool Database::update(const Machine &m, QString *error)
{
    if (!MachineValidation::validate(m, error)) return false;
    QSqlQuery q;
    const bool oracle = isOracleConnection();
    q.prepare(oracle
        ? QStringLiteral("UPDATE MACHINES SET NOM_MACHINE=?, TYPE_MACHINE=?, ETAT=?, DATE_PROCHAINE_MAINTENANCE=?, HEURES_FONCTIONNEMENT=?, PIECES_PRODUITES=?, HEURES_ARRET=? WHERE ID_MACHINE=?")
        : QStringLiteral("UPDATE machine SET Nom_Machine=?, Type_Machine=?, Etat=?, Date_Prochaine_Maintenance=?, Photo=?, Heures_Fonctionnement=?, Pieces_Produites=?, Heures_Arret=? WHERE ID_Machine=?"));
    q.addBindValue(m.nom); q.addBindValue(m.type); q.addBindValue(m.etat);
    q.addBindValue(oracle ? QVariant(m.prochaineMaintenance) : QVariant(m.prochaineMaintenance.toString(QLatin1String(kIso))));
    if (!oracle) q.addBindValue(m.photo);
    q.addBindValue(m.heuresFonctionnement); q.addBindValue(m.piecesProduites); q.addBindValue(m.heuresArret);
    q.addBindValue(oracle ? QVariant(oracleIdValue(m.id)) : QVariant(m.id));
    if (!q.exec()) { if (error) *error = q.lastError().text(); return false; }
    if (q.numRowsAffected() == 0) { if (error) *error = QStringLiteral("Modification impossible : machine introuvable."); return false; }
    return true;
}
bool Database::remove(const QString &id, QString *error)
{
    QSqlQuery q;
    const bool oracle = isOracleConnection();
    q.prepare(oracle ? QStringLiteral("DELETE FROM MACHINES WHERE ID_MACHINE=?") : QStringLiteral("DELETE FROM machine WHERE ID_Machine=?"));
    q.addBindValue(oracle ? QVariant(oracleIdValue(id)) : QVariant(id));
    if (!q.exec()) { if (error) *error = q.lastError().text(); return false; }
    if (q.numRowsAffected() == 0) { if (error) *error = QStringLiteral("Suppression impossible : machine introuvable."); return false; }
    return true;
}
bool Database::insertReading(const MachineReading &r, QString *error)
{
    if (!r.date.isValid() || r.plannedHours <= 0.0 || r.operatingHours < 0.0 || r.downtimeHours < 0.0
        || r.operatingHours + r.downtimeHours > r.plannedHours + 0.001
        || r.quantityProduced < 0 || r.productionTarget <= 0) {
        if (error) *error = QStringLiteral("Vérifiez les heures et les quantités du relevé.");
        return false;
    }
    QSqlQuery q;
    const bool oracle = isOracleConnection();
    q.prepare(QStringLiteral(
        "INSERT INTO releve_machine (%1ID_Machine, Date_Releve, Heures_Planifiees, "
        "Heures_Fonctionnement, Heures_Arret, Quantite_Produite, Objectif_Production, Cause_Arret) "
        "VALUES (%2?, ?, ?, ?, ?, ?, ?, ?)" )
        .arg(oracle ? QStringLiteral("ID_Releve, ") : QString(),
             oracle ? QStringLiteral("seq_releve_machine.NEXTVAL, ") : QString()));
    q.addBindValue(isOracleConnection() ? QVariant(oracleIdValue(r.machineId)) : QVariant(r.machineId));
    q.addBindValue(r.date.toString(QLatin1String(kIso)));
    q.addBindValue(r.plannedHours);
    q.addBindValue(r.operatingHours);
    q.addBindValue(r.downtimeHours);
    q.addBindValue(r.quantityProduced);
    q.addBindValue(r.productionTarget);
    q.addBindValue(r.stopReason);
    if (!q.exec()) {
        if (error) *error = q.lastError().text();
        return false;
    }
    return true;
}

bool Database::applyMaintenancePlan(const QList<MaintenanceRecord> &records, QString *error)
{
    QSqlDatabase db = QSqlDatabase::database();
    if (!db.transaction()) {
        if (error) *error = db.lastError().text();
        return false;
    }

    for (const MaintenanceRecord &r : records) {
        const bool oracle = isOracleConnection();
        const QVariant machineId = oracle
            ? QVariant(oracleIdValue(r.machineId)) : QVariant(r.machineId);
        QSqlQuery parent(db);
        parent.prepare(oracle
            ? QStringLiteral("SELECT ID_MACHINE FROM MACHINES WHERE ID_MACHINE=?")
            : QStringLiteral("SELECT ID_Machine FROM machine WHERE ID_Machine=?"));
        parent.addBindValue(machineId);
        if (!parent.exec()) {
            if (error) *error = parent.lastError().text();
            db.rollback();
            return false;
        }
        if (!parent.next()) {
            if (error) *error = QStringLiteral("Planning annulé : la machine %1 n'existe plus dans la base. Rechargez la liste.").arg(r.machineId);
            db.rollback();
            return false;
        }
        parent.finish();

        QSqlQuery retire;
        retire.prepare(QStringLiteral(
            "UPDATE maintenance SET Statut='Replanifiée' "
            "WHERE ID_Machine=? AND Statut='Planifiée'"));
        retire.addBindValue(machineId);
        if (!retire.exec()) {
            if (error) *error = retire.lastError().text();
            db.rollback();
            return false;
        }

        QSqlQuery insert;
        insert.prepare(QStringLiteral(
            "INSERT INTO maintenance (%1ID_Machine, Date_Prevue, Type_Intervention, Priorite, "
            "Duree_Estimee_Heures, Statut, Description) VALUES (%2?, ?, ?, ?, ?, 'Planifiée', ?)" )
            .arg(oracle ? QStringLiteral("ID_Maintenance, ") : QString(),
            oracle ? QStringLiteral("seq_maintenance.NEXTVAL, ") : QString()));
        insert.addBindValue(machineId);
        insert.addBindValue(r.plannedDate.toString(QLatin1String(kIso)));
        insert.addBindValue(r.interventionType);
        insert.addBindValue(r.priority);
        insert.addBindValue(r.estimatedHours);
        insert.addBindValue(r.description);
        if (!insert.exec()) {
            if (error) *error = insert.lastError().text();
            db.rollback();
            return false;
        }

        QSqlQuery updateMachine;
        updateMachine.prepare(oracle
            ? QStringLiteral("UPDATE MACHINES SET DATE_PROCHAINE_MAINTENANCE=? WHERE ID_MACHINE=?")
            : QStringLiteral("UPDATE machine SET Date_Prochaine_Maintenance=? WHERE ID_Machine=?"));
        updateMachine.addBindValue(oracle ? QVariant(r.plannedDate) : QVariant(r.plannedDate.toString(QLatin1String(kIso))));
        updateMachine.addBindValue(machineId);
        if (!updateMachine.exec() || updateMachine.numRowsAffected() == 0) {
            if (error) *error = updateMachine.lastError().isValid()
                ? updateMachine.lastError().text()
                : QStringLiteral("Machine introuvable : %1").arg(r.machineId);
            db.rollback();
            return false;
        }
    }

    if (!db.commit()) {
        if (error) *error = db.lastError().text();
        db.rollback();
        return false;
    }
    return true;
}

QString Database::nextId()
{
    int max = 0;
    if (isOracleConnection()) {
        QSqlQuery q(QStringLiteral("SELECT NVL(MAX(ID_MACHINE), 0) FROM MACHINES"));
        if (!q.next()) return QString();
        max = q.value(0).toInt();
    } else {
        const QList<Machine> list = all();
        for (const Machine &m : list) max = qMax(max, oracleIdValue(m.id));
    }
    return QStringLiteral("MCH%1").arg(max + 1, 3, 10, QLatin1Char('0'));
}

// ================= source fusionné : detailsdialog.cpp =================
#include <QStyle>
DetailsDialog::DetailsDialog(const Machine &m, QWidget *parent)
    : QDialog(parent), ui(new Ui::DetailsDialog)
{
    ui->setupUi(this);

    ui->labelTitle->setText(QStringLiteral("%1 – %2").arg(m.id, m.nom));
    ui->valueType->setText(m.type);
    ui->valueEtat->setText(m.etat);
    ui->valueDate->setText(QStringLiteral("%1 (%2 j)")
                               .arg(m.prochaineMaintenance.toString(QStringLiteral("dd/MM/yyyy")))
                               .arg(m.joursRestants()));
    ui->labelPhoto->setPixmap(UI::cover(UI::machinePixmap(m.photo), QSize(90, 120), 8));

    const QString reason = m.raisonMaintenance();
    if (reason.isEmpty()) {
        ui->labelStatus->setText(QStringLiteral("✔ Maintenance à jour."));
        ui->labelStatus->setProperty("level", QStringLiteral("ok"));
    } else {
        ui->labelStatus->setText(QStringLiteral("⚠ %1").arg(reason));
        ui->labelStatus->setProperty("level", QStringLiteral("crit"));
    }
    ui->labelStatus->style()->unpolish(ui->labelStatus);
    ui->labelStatus->style()->polish(ui->labelStatus);
}

DetailsDialog::~DetailsDialog()
{
    delete ui;
}

// ================= source fusionné : machinespage.cpp =================
#include "ui_machine.h"
#include <algorithm>
#include <utility>

#include <QAction>
#include <QComboBox>
#include <QDateEdit>
#include <QDesktopServices>
#include <QDoubleSpinBox>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QLocale>
#include <QMap>
#include <QMarginsF>
#include <QMenu>
#include <QMessageBox>
#include <QPageSize>
#include <QPdfWriter>
#include <QPushButton>
#include <QRegularExpressionValidator>
#include <QSignalBlocker>
#include <QSpinBox>
#include <QStyle>
#include <QTableWidget>
#include <QTextDocument>
#include <QTimer>
#include <QToolButton>
#include <QUrl>
static const int kPageSize = 5;

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

MachinesPage::MachinesPage(QWidget *parent)
    : QWidget(parent), ui(new Ui::MachinesPage)
{
    ui->setupUi(this);          // construit toute l'interface décrite dans machinespage.ui
    ui->lineNom->setMaxLength(200);
    ui->lineNom->setValidator(new QRegularExpressionValidator(
        MachineValidation::namePattern(), ui->lineNom));
    ui->lineNom->setToolTip(QStringLiteral("Lettres et chiffres acceptés, avec au moins une lettre. Exemple : Singer 200."));
    ui->dateMaintenance->setKeyboardTracking(false);
    m_pager = ui->pagerLayout;

    setupIcons();
    setupBusinessActions();
    setupTable();
    connectSignals();

    reloadData();
    clearForm();

    // Re-vérification périodique (une machine peut devenir « en retard » sans action de l'utilisateur)
    auto *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MachinesPage::reloadData);
    timer->start(60 * 1000);
}

MachinesPage::~MachinesPage()
{
    delete ui;
}

// Les icônes sont des SVG recolorés à l'exécution : elles sont donc affectées ici et non dans le .ui.
void MachinesPage::setupIcons()
{
    const QColor ink("#2c3240");
    // cartes
    ui->iconCardForm->setPixmap(UI::svgPixmap(QStringLiteral("gear"),  Qt::white, 18));
    ui->iconCardList->setPixmap(UI::svgPixmap(QStringLiteral("clip"),  Qt::white, 18));
    ui->iconCardStats->setPixmap(UI::svgPixmap(QStringLiteral("chart"), Qt::white, 18));

    // champs du formulaire
    ui->iconId->setPixmap(UI::svgPixmap(QStringLiteral("idcard"),   ink, 18));
    ui->iconNom->setPixmap(UI::svgPixmap(QStringLiteral("gear"),    ink, 18));
    ui->iconType->setPixmap(UI::svgPixmap(QStringLiteral("box"),    ink, 18));
    ui->iconDate->setPixmap(UI::svgPixmap(QStringLiteral("calendar"), ink, 18));
    for (int i = 0; i < ui->comboEtat->count(); ++i)
        ui->comboEtat->setItemIcon(i, UI::dotIcon(UI::etatColor(ui->comboEtat->itemText(i))));

    // photo
    ui->btnPen->setIcon(UI::svgIcon(QStringLiteral("pen"), Qt::white, 16));
    ui->btnPlus->setIcon(UI::svgIcon(QStringLiteral("plus"), QColor("#c9a36a"), 22));
    ui->labelThumb1->setPixmap(UI::cover(QPixmap(QStringLiteral(":/images/thumb1.jpg")), QSize(44, 56), 5));
    ui->labelThumb2->setPixmap(UI::cover(QPixmap(QStringLiteral(":/images/thumb2.jpg")), QSize(44, 56), 5));

    // boutons
    ui->btnAdd->setIcon(UI::svgIcon(QStringLiteral("plus"),  Qt::white, 20));
    ui->btnMod->setIcon(UI::svgIcon(QStringLiteral("pen"),   Qt::white, 20));
    ui->btnDel->setIcon(UI::svgIcon(QStringLiteral("trash"), QColor("#4a3a26"), 20));
    ui->btnReset->setIcon(UI::svgIcon(QStringLiteral("refresh"), QColor("#23252e"), 16));
    ui->btnExport->setIcon(UI::svgIcon(QStringLiteral("doc"), Qt::white, 16));

    ui->lineSearch->addAction(UI::svgIcon(QStringLiteral("search"), QColor("#6b7a90"), 18),
                              QLineEdit::LeadingPosition);
}

void MachinesPage::setupBusinessActions()
{
    auto *layout = qobject_cast<QHBoxLayout *>(ui->headerStats->layout());
    if (!layout) return;

    auto *button = new QPushButton(QStringLiteral("Appliquer le planning"), ui->headerStats);
    button->setObjectName(QStringLiteral("btnApplyPlan"));
    button->setToolTip(QStringLiteral("Enregistrer les dates de maintenance proposées dans la base de données"));
    button->setMinimumWidth(175);
    button->setFixedHeight(32);
    button->setCursor(Qt::PointingHandCursor);
    layout->addWidget(button);
    connect(button, &QPushButton::clicked, this, &MachinesPage::onApplyOptimizedPlan);
}

void MachinesPage::setupTable()
{
    QTableWidget *t = ui->tableMachines;
    t->setColumnCount(10);
    t->setHorizontalHeaderLabels({QString(), QStringLiteral("ID"), QStringLiteral("Nom"),
        QStringLiteral("Type"), QStringLiteral("État"), QStringLiteral("Heures fonct."),
        QStringLiteral("Pièces prod."), QStringLiteral("Heures arrêt"),
        QStringLiteral("Prochaine maint."), QStringLiteral("Aperçu")});
    t->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    t->verticalHeader()->setDefaultSectionSize(50);
    t->setFixedHeight(40 + 50 * kPageSize + 4);
    QHeaderView *hh = t->horizontalHeader();
    hh->setHighlightSections(false);
    hh->setFixedHeight(40);
    hh->setSectionsClickable(true);
    hh->setSectionResizeMode(QHeaderView::Fixed);
    hh->setSectionResizeMode(2, QHeaderView::Fixed);
    hh->setStretchLastSection(false);
    const int widths[] = { 30, 70, 132, 74, 110, 102, 88, 92, 116, 78 };
    for (int c = 0; c < 10; ++c) t->setColumnWidth(c, widths[c]);
    hh->setDefaultAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    // case « tout cocher » de l'en-tête
    connect(hh, &QHeaderView::sectionClicked, this, [this](int c) {
        if (c != 0) return;
        QTableWidget *tw = ui->tableMachines;
        bool all = true;
        for (int r = 0; r < tw->rowCount(); ++r)
            if (tw->item(r, 0)->checkState() != Qt::Checked) all = false;
        for (int r = 0; r < tw->rowCount(); ++r)
            tw->item(r, 0)->setCheckState(all ? Qt::Unchecked : Qt::Checked);
    });
}

void MachinesPage::connectSignals()
{
    connect(ui->lineNom, &QLineEdit::inputRejected, this, [this] {
        emit notify(QStringLiteral("Nom refusé : lettres, chiffres, espaces, apostrophes et traits d'union uniquement (200 caractères maximum)."), true);
    });
    if (kCrudActif) {
        connect(ui->btnAdd,  &QPushButton::clicked, this, &MachinesPage::onAdd);      // Create
        connect(ui->btnMod,  &QPushButton::clicked, this, &MachinesPage::onModify);   // Update
        connect(ui->btnDel,  &QPushButton::clicked, this, &MachinesPage::onDelete);   // Delete
    }
    connect(ui->btnReset,  &QPushButton::clicked, this, &MachinesPage::onReset);
    connect(ui->btnExport, &QPushButton::clicked, this, &MachinesPage::onExportPdf);
    connect(ui->btnPen,    &QToolButton::clicked, this, &MachinesPage::onChoosePhoto);
    connect(ui->btnPlus,   &QPushButton::clicked, this, &MachinesPage::onChoosePhoto);

    connect(ui->comboEtat, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this] { updateDetection(); });
    connect(ui->dateMaintenance, &QDateEdit::dateChanged, this, [this] { updateDetection(); });

    connect(ui->tableMachines, &QTableWidget::itemSelectionChanged, this, &MachinesPage::onRowSelected);
    connect(ui->lineSearch, &QLineEdit::textChanged, this, &MachinesPage::onSearchChanged);
    connect(ui->comboSort, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this] { m_page = 0; applyFilter(); });
}

/* ============================== données =============================== */

const Machine *MachinesPage::findMachine(const QString &id) const
{
    for (const Machine &m : m_all)
        if (m.id == id) return &m;
    return nullptr;
}

void MachinesPage::reloadData()
{
    m_all = Database::all();
    checkAlerts();
    updateCharts();
    applyFilter();
}

void MachinesPage::onSearchChanged(const QString &text)
{
    // les deux champs de recherche restent synchronisés
    m_page = 0;
    applyFilter();
    emit searchTextChanged(text);      // la barre de recherche du haut se met à jour
}

void MachinesPage::setSearchText(const QString &text)
{
    if (ui->lineSearch->text() != text)
        ui->lineSearch->setText(text);  // déclenche onSearchChanged()
}

void MachinesPage::applyFilter()
{
    const QString q = ui->lineSearch->text().trimmed();
    m_view.clear();
    for (const Machine &m : std::as_const(m_all)) {
        // recherche par ID_Machine, Nom_Machine, Type_Machine
        const bool match = q.isEmpty()
            || m.id.contains(q, Qt::CaseInsensitive)
            || m.nom.contains(q, Qt::CaseInsensitive)
            || m.type.contains(q, Qt::CaseInsensitive);
        if (match) m_view.append(m);
    }

    // tri par Nom_Machine, Type_Machine ou Etat
    const int key = ui->comboSort->currentIndex();
    if (key > 0) {
        std::stable_sort(m_view.begin(), m_view.end(), [key](const Machine &a, const Machine &b) {
            const QString x = key == 1 ? a.nom : key == 2 ? a.type : a.etat;
            const QString y = key == 1 ? b.nom : key == 2 ? b.type : b.etat;
            return QString::localeAwareCompare(x, y) < 0;
        });
    }

    renderPage();
}

void MachinesPage::renderPage()
{
    const int total = m_view.size();
    const int pages = qMax(1, (total + kPageSize - 1) / kPageSize);
    m_page = qBound(0, m_page, pages - 1);
    const int from = m_page * kPageSize;
    const int to = qMin(total, from + kPageSize);

    {
        QSignalBlocker blocker(ui->tableMachines);
        ui->tableMachines->clearContents();
        ui->tableMachines->setRowCount(to - from);

        for (int r = 0; r < to - from; ++r) {
            const Machine &m = m_view.at(from + r);

            auto *chk = new QTableWidgetItem;
            chk->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
            chk->setCheckState(Qt::Unchecked);
            chk->setData(Qt::UserRole, m.id);
            ui->tableMachines->setItem(r, 0, chk);

            auto mk = [&](const QString &txt) {
                auto *it = new QTableWidgetItem(txt);
                it->setData(Qt::UserRole, m.id);
                return it;
            };
            ui->tableMachines->setItem(r, 1, mk(m.id));
            ui->tableMachines->setItem(r, 2, mk(m.nom));
            ui->tableMachines->setItem(r, 3, mk(m.type));
            ui->tableMachines->setItem(r, 5, mk(QStringLiteral("%1 h").arg(m.heuresFonctionnement, 0, 'f', 1)));
            ui->tableMachines->setItem(r, 6, mk(QLocale().toString(m.piecesProduites)));
            ui->tableMachines->setItem(r, 7, mk(QStringLiteral("%1 h").arg(m.heuresArret, 0, 'f', 1)));

            // pastille d'état
            QString bg = QStringLiteral("#dcefd9"), fg = QStringLiteral("#3f8a45");
            if (m.etat == Etat::maintenance())  { bg = QStringLiteral("#f7e6cc"); fg = QStringLiteral("#b9781a"); }
            if (m.etat == Etat::indisponible()) { bg = QStringLiteral("#f8d6d2"); fg = QStringLiteral("#c8402f"); }
            auto *pill = new QLabel(m.etat);
            pill->setStyleSheet(QStringLiteral("background:%1;color:%2;border-radius:5px;padding:4px 9px;font-size:12px;")
                                    .arg(bg, fg));
            auto *pw = new QWidget;
            pw->setAttribute(Qt::WA_TransparentForMouseEvents);
            auto *pl = new QHBoxLayout(pw);
            pl->setContentsMargins(4, 0, 4, 0);
            pl->addWidget(pill);
            pl->addStretch();
            ui->tableMachines->setCellWidget(r, 4, pw);

            // date + détection automatique
            const QString reason = m.raisonMaintenance();
            auto *dt = mk((reason.isEmpty() ? QString() : QStringLiteral("⚠ "))
                          + m.prochaineMaintenance.toString(QStringLiteral("dd/MM/yyyy")));
            if (!reason.isEmpty()) {
                dt->setForeground(m.critique() ? QColor("#c8402f") : QColor("#b9781a"));
                dt->setToolTip(reason);
            }
            ui->tableMachines->setItem(r, 8, dt);

            auto *ap = new QLabel;
            ap->setPixmap(UI::cover(UI::machinePixmap(m.photo), QSize(64, 38), 4));
            auto *aw = new QWidget;
            aw->setAttribute(Qt::WA_TransparentForMouseEvents);
            auto *al = new QHBoxLayout(aw);
            al->setContentsMargins(4, 0, 4, 0);
            al->addWidget(ap);
            ui->tableMachines->setCellWidget(r, 9, aw);
        }

        for (int r = 0; r < to - from; ++r)
            if (ui->tableMachines->item(r, 1)->data(Qt::UserRole).toString() == m_currentId)
                ui->tableMachines->selectRow(r);
    }
    buildPager(pages);
}

void MachinesPage::buildPager(int pages)
{
    clearLayout(m_pager);
    m_pager->addStretch();
    auto add = [&](const QString &text, int target, bool current, bool enabled) {
        auto *b = new QPushButton(text);
        b->setObjectName(current ? QStringLiteral("pageCur") : QStringLiteral("page"));
        b->setFixedSize(36, 36);
        b->setEnabled(enabled);
        b->setCursor(Qt::PointingHandCursor);
        connect(b, &QPushButton::clicked, this, [this, target] { m_page = target; renderPage(); });
        m_pager->addWidget(b);
    };
    add(QStringLiteral("‹"), qMax(0, m_page - 1), false, m_page > 0);
    const int start = qMax(0, qMin(m_page - 2, pages - 5));
    const int end = qMin(pages, start + 5);
    if (start > 0) { add(QStringLiteral("1"), 0, false, true); if (start > 1) add(QStringLiteral("…"), start - 1, false, true); }
    for (int i = start; i < end; ++i) add(QString::number(i + 1), i, i == m_page, true);
    if (end < pages) { if (end < pages - 1) add(QStringLiteral("…"), end, false, true); add(QString::number(pages), pages - 1, false, true); }
    add(QStringLiteral("›"), qMin(pages - 1, m_page + 1), false, m_page < pages - 1);
}

void MachinesPage::goToMachine(const QString &id)
{
    for (int i = 0; i < m_view.size(); ++i)
        if (m_view[i].id == id) { m_page = i / kPageSize; break; }
    renderPage();
}

/* ============================ statistiques ============================ */

void MachinesPage::updateCharts()
{
    // Répartition par type
    QMap<QString, int> count;
    for (const Machine &m : std::as_const(m_all)) count[m.type]++;
    QVector<DonutChart::Slice> slices;
    for (const QString &t : typesMachine())
        if (count.value(t) > 0)
            slices.append(DonutChart::Slice{ t, count.value(t), UI::typeColor(t) });
    ui->donutChart->setData(slices, QStringLiteral("machines"));

    // Les performances sont calculées à partir des trois attributs de la fiche machine.
    const QVector<Metier::Performance> perf = Metier::performanceParType(m_all, {});
    QVector<PerformanceBars::Entry> bars;
    for (const Metier::Performance &p : perf)
        bars.append(PerformanceBars::Entry{ p.type, qBound(0, p.pourcentage(), 100),
                                             qRound(p.disponibilite), qRound(p.productionAtteinte),
                                             p.nombreMachines });
    const QString perfSummary = perf.isEmpty()
        ? QStringLiteral("Ajoutez les heures et pièces produites dans les fiches machines")
        : QStringLiteral("Type le plus performant : %1 · %2 pièces/h · disponibilité %3 %")
              .arg(perf.first().type).arg(perf.first().piecesParHeure, 0, 'f', 1)
              .arg(qRound(perf.first().disponibilite));
    ui->performanceBars->setData(bars, perfSummary);

    // Métier 2 : le plan utilise aussi le score des types pour avancer les entretiens nécessaires.
    const QVector<Metier::Intervention> plan = Metier::planifierMaintenances(
        m_all, perf, Database::allMaintenanceRecords());
    int ajustees = 0;
    QVector<PlanningList::Entry> rows;
    for (const Metier::Intervention &iv : plan) {
        if (iv.modifiee()) ++ajustees;
        if (rows.size() >= 5) continue;
        QColor c("#2e9e5b");                                  // date conservée
        if (iv.urgente()) c = QColor("#e5484d");              // indisponible ou en retard
        else if (iv.motif == Metier::Intervention::PerformanceFaible) c = QColor("#c47b16");
        else if (iv.modifiee()) c = QColor("#f0a020");        // ajustée (en cours, week-end, même type)
        const QString fmt = QStringLiteral("dd/MM");
        rows.append(PlanningList::Entry{ iv.nom, iv.actuelle.toString(fmt), iv.proposee.toString(fmt),
                                         iv.raison(), iv.modifiee(), c });
    }
    const QString planSummary = plan.isEmpty()
        ? QStringLiteral("Aucune machine enregistrée")
        : (ajustees == 0 ? QStringLiteral("Planning à jour : aucune date à ajuster")
                         : QStringLiteral("%1 date(s) ajustée(s) · priorité aux retards et types faibles")
                               .arg(ajustees));
    ui->planningList->setData(rows, planSummary);
}

/* ================================ alertes ================================ */

void MachinesPage::checkAlerts()
{
    QSet<QString> unavailable;
    int needing = 0;
    for (const Machine &m : std::as_const(m_all)) {
        if (m.indisponible()) unavailable.insert(m.id);
        if (m.maintenanceRequise()) ++needing;
    }
    // alerte : une machine vient de devenir indisponible
    if (m_alertsReady) {
        for (const QString &id : std::as_const(unavailable)) {
            if (!m_knownUnavailable.contains(id)) {
                const Machine *m = findMachine(id);
                if (m) emit notify(QStringLiteral("Alerte : la machine %1 – %2 est indisponible.").arg(m->id, m->nom), true);
            }
        }
    }
    m_knownUnavailable = unavailable;
    m_alertsReady = true;

    if (needing != m_alertCount) {
        m_alertCount = needing;
        emit alertCountChanged(m_alertCount);
    }
}

void MachinesPage::showAlertsMenu(QWidget *anchor)
{
    QMenu menu(this);
    bool any = false;
    for (const Machine &m : std::as_const(m_all)) {
        const QString reason = m.raisonMaintenance();
        if (reason.isEmpty()) continue;
        any = true;
        QAction *a = menu.addAction(UI::dotIcon(m.critique() ? QColor("#e5484d") : QColor("#e6a23c")),
                                    QStringLiteral("%1 – %2 : %3").arg(m.id, m.nom, reason));
        const QString id = m.id;
        connect(a, &QAction::triggered, this, [this, id] {
            m_currentId = id;
            ui->lineSearch->clear();
            goToMachine(id);
            if (const Machine *mm = findMachine(id)) loadForm(*mm);
        });
    }
    if (!any)
        menu.addAction(QStringLiteral("Aucune alerte – tout le parc est à jour"))->setEnabled(false);
    menu.exec(anchor->mapToGlobal(QPoint(-260, anchor->height() + 4)));
}

/* ============================== formulaire ============================== */

bool MachinesPage::readForm(Machine &m, QString *error) const
{
    m.nom = ui->lineNom->text().trimmed();
    m.type = ui->comboType->currentText();
    m.etat = ui->comboEtat->currentText();
    m.prochaineMaintenance = ui->dateMaintenance->date();
    m.heuresFonctionnement = ui->spinHeuresFonctionnement->value();
    m.piecesProduites = ui->spinPiecesProduites->value();
    m.heuresArret = ui->spinHeuresArret->value();
    m.photo = m_formPhoto;
    return MachineValidation::validate(m, error);
}

void MachinesPage::loadForm(const Machine &m)
{
    ui->lineId->setText(m.id);
    ui->lineNom->setText(m.nom);
    int i = ui->comboType->findText(m.type);
    if (i < 0) { ui->comboType->addItem(m.type); i = ui->comboType->findText(m.type); }
    ui->comboType->setCurrentIndex(i);
    ui->comboEtat->setCurrentIndex(qMax(0, ui->comboEtat->findText(m.etat)));
    ui->dateMaintenance->setDate(m.prochaineMaintenance);
    ui->spinHeuresFonctionnement->setValue(m.heuresFonctionnement);
    ui->spinPiecesProduites->setValue(m.piecesProduites);
    ui->spinHeuresArret->setValue(m.heuresArret);
    m_formPhoto = m.photo;
    updatePreview();
    updateDetection();
}

void MachinesPage::clearForm()
{
    m_currentId.clear();
    m_formPhoto.clear();
    ui->lineId->setText(Database::nextId());
    ui->lineNom->clear();
    ui->comboType->setCurrentIndex(0);
    ui->comboEtat->setCurrentIndex(0);
    ui->dateMaintenance->setDate(QDate::currentDate().addDays(60));
    ui->spinHeuresFonctionnement->setValue(0.0);
    ui->spinPiecesProduites->setValue(0);
    ui->spinHeuresArret->setValue(0.0);
    updatePreview();
    updateDetection();
}

void MachinesPage::updatePreview()
{
    ui->labelPhoto->setPixmap(UI::cover(UI::machinePixmap(m_formPhoto), QSize(142, 214), 8));
}

void MachinesPage::updateDetection()
{
    if (!ui->labelDetection) return;
    Machine tmp;
    tmp.etat = ui->comboEtat->currentText();
    tmp.prochaineMaintenance = ui->dateMaintenance->date();
    const QString reason = tmp.raisonMaintenance();
    if (reason.isEmpty()) {
        ui->labelDetection->setText(QStringLiteral("✔  Détection automatique : maintenance à jour"));
        ui->labelDetection->setProperty("level", QStringLiteral("ok"));
    } else {
        ui->labelDetection->setText(QStringLiteral("⚠  Détection automatique : %1").arg(reason));
        ui->labelDetection->setProperty("level", tmp.critique() ? QStringLiteral("crit") : QStringLiteral("warn"));
    }
    repolish(ui->labelDetection);
}

void MachinesPage::onRowSelected()
{
    const QList<QTableWidgetItem *> items = ui->tableMachines->selectedItems();
    if (items.isEmpty()) return;
    const QString id = items.first()->data(Qt::UserRole).toString();
    if (const Machine *m = findMachine(id)) {
        m_currentId = id;
        loadForm(*m);
    }
}

/* =============================== actions =============================== */

void MachinesPage::onAdd()
{
    Machine m;
    QString err;
    if (!readForm(m, &err)) { QMessageBox::warning(this, QStringLiteral("Ajouter"), err); return; }
    m.id = Database::nextId();
    if (m.id.isEmpty()) { QMessageBox::critical(this, QStringLiteral("Ajouter"), QStringLiteral("Impossible de générer le prochain ID.")); return; }
    if (!Database::insert(m, &err)) { QMessageBox::critical(this, QStringLiteral("Ajouter"), err); return; }
    const QString createdId = m.id;
    ui->lineSearch->clear();
    reloadData();
    goToMachine(createdId);
    emit notify(QStringLiteral("Machine %1 ajoutée avec succès.").arg(createdId), false);
    clearForm();
}

void MachinesPage::onModify()
{
    if (m_currentId.isEmpty()) {
        QMessageBox::information(this, QStringLiteral("Modifier"), QStringLiteral("Sélectionnez d'abord une machine dans la liste."));
        return;
    }
    const Machine *selected = findMachine(m_currentId);
    if (!selected) {
        QMessageBox::warning(this, QStringLiteral("Modifier"), QStringLiteral("Cette machine n'existe plus. Actualisez la liste."));
        return;
    }
    // Conserve les heures et la production, absentes du formulaire simplifié.
    Machine m = *selected;
    QString err;
    if (!readForm(m, &err)) { QMessageBox::warning(this, QStringLiteral("Modifier"), err); return; }
    m.id = m_currentId;
    if (!Database::update(m, &err)) { QMessageBox::critical(this, QStringLiteral("Modifier"), err); return; }
    ui->lineSearch->clear();
    reloadData();
    goToMachine(m.id);
    loadForm(m);
    emit notify(QStringLiteral("Machine %1 modifiée.").arg(m.id), false);
}

void MachinesPage::onDelete()
{
    if (m_currentId.isEmpty()) {
        QMessageBox::information(this, QStringLiteral("Supprimer"), QStringLiteral("Sélectionnez d'abord une machine dans la liste."));
        return;
    }
    const auto r = QMessageBox::question(this, QStringLiteral("Supprimer"),
                                         QStringLiteral("Supprimer définitivement la machine %1 ?").arg(m_currentId));
    if (r != QMessageBox::Yes) return;
    QString err;
    if (!Database::remove(m_currentId, &err)) { QMessageBox::critical(this, QStringLiteral("Supprimer"), err); return; }
    const QString id = m_currentId;
    clearForm();
    reloadData();
    emit notify(QStringLiteral("Machine %1 supprimée.").arg(id), false);
}

void MachinesPage::onReset()
{
    clearForm();
    ui->tableMachines->clearSelection();
}

void MachinesPage::onChoosePhoto()
{
    const QString f = QFileDialog::getOpenFileName(this, QStringLiteral("Choisir une photo"), QString(),
                                                   QStringLiteral("Images (*.png *.jpg *.jpeg *.bmp)"));
    if (f.isEmpty()) return;
    m_formPhoto = f;
    updatePreview();
    if (!m_currentId.isEmpty())
        emit notify(QStringLiteral("Photo sélectionnée : cliquez sur « Modifier » pour l'enregistrer."), false);
}

void MachinesPage::onAddReading()
{
    if (m_all.isEmpty()) {
        QMessageBox::information(this, QStringLiteral("Relevé de production"),
                                 QStringLiteral("Ajoutez d'abord une machine au parc."));
        return;
    }
    MachineReadingDialog dialog(m_all, this);
    if (dialog.exec() != QDialog::Accepted) return;
    QString error;
    if (!Database::insertReading(dialog.reading(), &error)) {
        QMessageBox::warning(this, QStringLiteral("Relevé non enregistré"),
                             QStringLiteral("Le relevé n'a pas été enregistré. Vérifiez qu'il n'existe pas déjà pour cette machine et cette date.\n\n%1").arg(error));
        return;
    }
    reloadData();
    emit notify(QStringLiteral("Relevé enregistré. Les scores de performance ont été recalculés."), false);
}

void MachinesPage::onPlanMaintenance()
{
    if (m_all.isEmpty()) {
        QMessageBox::information(this, QStringLiteral("Planifier une maintenance"),
                                 QStringLiteral("Ajoutez d'abord une machine au parc."));
        return;
    }
    MaintenancePlanDialog dialog(m_all, Database::allMaintenanceRecords(), this);
    if (dialog.exec() != QDialog::Accepted) return;
    QList<MaintenanceRecord> records;
    records.append(dialog.record());
    QString error;
    if (!Database::applyMaintenancePlan(records, &error)) {
        QMessageBox::critical(this, QStringLiteral("Plan non enregistré"), error);
        return;
    }
    reloadData();
    emit notify(QStringLiteral("Maintenance planifiée et ajoutée à l'historique."), false);
}

void MachinesPage::onApplyOptimizedPlan()
{
    if (m_all.isEmpty()) return;
    const QVector<Metier::Performance> perf = Metier::performanceParType(m_all, {});
    if (perf.isEmpty()) {
        QMessageBox::information(this, QStringLiteral("Planning optimisé"),
            QStringLiteral("Renseignez les heures de fonctionnement, pièces produites et heures d'arrêt dans les fiches machines."));
        return;
    }
    const QVector<Metier::Intervention> plan = Metier::planifierMaintenances(
        m_all, perf, Database::allMaintenanceRecords());
    const auto answer = QMessageBox::question(this, QStringLiteral("Appliquer le planning optimisé"),
        QStringLiteral("Enregistrer les dates proposées pour les %1 machines ?\n\n"
                       "Les anciennes dates seront conservées dans l'historique comme replanifiées.")
            .arg(plan.size()), QMessageBox::Yes | QMessageBox::Cancel, QMessageBox::Cancel);
    if (answer != QMessageBox::Yes) return;

    QList<MaintenanceRecord> records;
    for (const Metier::Intervention &iv : plan) {
        MaintenanceRecord r;
        r.machineId = iv.id;
        r.plannedDate = iv.proposee;
        r.priority = iv.urgente() ? 5
                     : iv.motif == Metier::Intervention::PerformanceFaible ? 4 : 2;
        r.estimatedHours = iv.dureeEstimee;
        r.description = iv.raison();
        records.append(r);
    }
    QString error;
    if (!Database::applyMaintenancePlan(records, &error)) {
        QMessageBox::critical(this, QStringLiteral("Planning non enregistré"), error);
        return;
    }
    reloadData();
    emit notify(QStringLiteral("Planning optimisé enregistré pour %1 machines.").arg(records.size()), false);
}

void MachinesPage::onExportPdf()
{
    const QString path = QFileDialog::getSaveFileName(this, QStringLiteral("Exporter la liste des machines"),
                                                      QStringLiteral("liste_machines.pdf"),
                                                      QStringLiteral("PDF (*.pdf)"));
    if (path.isEmpty()) return;

    QString rows;
    for (const Machine &m : std::as_const(m_view)) {
        const QString alert = m.raisonMaintenance();
        rows += QStringLiteral("<tr><td>%1</td><td>%2</td><td>%3</td><td>%4</td><td>%5</td>"
                               "<td>%6</td><td>%7</td><td>%8</td><td>%9</td></tr>")
                    .arg(m.id, m.nom.toHtmlEscaped(), m.type, m.etat,
                         QStringLiteral("%1 h").arg(m.heuresFonctionnement, 0, 'f', 1),
                         QString::number(m.piecesProduites),
                         QStringLiteral("%1 h").arg(m.heuresArret, 0, 'f', 1),
                         m.prochaineMaintenance.toString(QStringLiteral("dd/MM/yyyy")), alert);
    }
    const QString html = QStringLiteral(
        "<h1 style='color:#0a2748'>FashioNova – Liste des machines</h1>"
        "<p>Édité le %1 – %2 machine(s)</p>"
        "<table border='1' cellspacing='0' cellpadding='5' width='100%'>"
        "<tr style='background:#efe3cf'><th>ID_Machine</th><th>Nom_Machine</th><th>Type_Machine</th>"
        "<th>Etat</th><th>Heures fonctionnement</th><th>Pièces produites</th><th>Heures d'arrêt</th>"
        "<th>Date prochaine maintenance</th><th>Alerte</th></tr>%3</table>")
        .arg(QDate::currentDate().toString(QStringLiteral("dd/MM/yyyy")))
        .arg(m_view.size())
        .arg(rows);

    QPdfWriter writer(path);
    writer.setPageSize(QPageSize(QPageSize::A4));
    writer.setPageMargins(QMarginsF(15, 15, 15, 15));
    QTextDocument doc;
    doc.setHtml(html);
    doc.print(&writer);

    emit notify(QStringLiteral("PDF exporté : %1").arg(path), false);
    QDesktopServices::openUrl(QUrl::fromLocalFile(path));
}

// ================= source fusionné : mainwindow.cpp =================
#include <QLabel>
#include <QPainter>
#include <QPixmap>
#include <QPushButton>
#include <QSignalBlocker>
#include <QStyle>
#include <QTimer>
static const QColor kNavy("#0a2748");

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);          // construit toute l'interface décrite dans mainwindow.ui

    setupSidebar();
    setupTopBar();

    // notification flottante (superposée au contenu, donc créée en code)
    m_toast = new QLabel(ui->central);
    m_toast->setObjectName(QStringLiteral("toast"));
    m_toast->setWordWrap(true);
    m_toast->setMargin(12);
    m_toast->hide();

    // liaison avec la page Machines
    connect(ui->pageMachines, &MachinesPage::notify, this, &MainWindow::showToast);
    connect(ui->pageMachines, &MachinesPage::alertCountChanged, this, &MainWindow::updateBadge);
    connect(ui->pageMachines, &MachinesPage::searchTextChanged, this, [this](const QString &t) {
        if (ui->topSearch->text() != t) {
            QSignalBlocker b(ui->topSearch);
            ui->topSearch->setText(t);
        }
    });
    connect(ui->topSearch, &QLineEdit::textChanged, ui->pageMachines, &MachinesPage::setSearchText);
    updateBadge(ui->pageMachines->alertCount());
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupSidebar()
{
    // le logo FashioNova fait partie de l'image de fond (sidebarwidget.cpp)

    struct Item { QPushButton *button; const char *icon; };
    const Item items[] = {
        { ui->btnNavClients,   "users" }, { ui->btnNavEmployes, "users" },
        { ui->btnNavCommandes, "clip"  }, { ui->btnNavMaquettes, "man" },
        { ui->btnNavMachines,  "gear"  }, { ui->btnNavArticles, "tag"  } };

    for (int i = 0; i < 6; ++i) {
        QPushButton *b = items[i].button;
        const QString icon = QString::fromLatin1(items[i].icon);
        b->setIcon(UI::svgIcon(icon, b->isChecked() ? kNavy : QColor(Qt::white), 26));
        // l'icône change de couleur quand le bouton est sélectionné
        connect(b, &QPushButton::toggled, this, [b, icon](bool on) {
            b->setIcon(UI::svgIcon(icon, on ? kNavy : QColor(Qt::white), 26));
        });
        connect(b, &QPushButton::clicked, this, [this, i] { ui->stackedPages->setCurrentIndex(i); });
    }
}

void MainWindow::setupTopBar()
{
    ui->btnMenu->setIcon(UI::svgIcon(QStringLiteral("menu"), QColor("#23252e"), 24));
    connect(ui->btnMenu, &QToolButton::clicked, this, [this] {
        ui->sidebar->setVisible(!ui->sidebar->isVisible());
    });

    ui->topSearch->addAction(UI::svgIcon(QStringLiteral("search"), QColor("#6b7a90"), 20),
                             QLineEdit::LeadingPosition);

    ui->btnBell->setIcon(UI::svgIcon(QStringLiteral("bell"), QColor("#23252e"), 26));
    m_badge = new QLabel(QStringLiteral("0"), ui->btnBell);
    m_badge->setObjectName(QStringLiteral("badge"));
    m_badge->setAlignment(Qt::AlignCenter);
    m_badge->setFixedSize(17, 17);
    m_badge->move(21, 1);
    m_badge->setAttribute(Qt::WA_TransparentForMouseEvents);
    connect(ui->btnBell, &QToolButton::clicked, this, &MainWindow::onBellClicked);

    QPixmap av(72, 72);
    av.fill(Qt::transparent);
    {
        QPainter p(&av);
        p.setRenderHint(QPainter::Antialiasing);
        p.setBrush(kNavy);
        p.setPen(Qt::NoPen);
        p.drawEllipse(0, 0, 72, 72);
        p.drawPixmap(18, 18, UI::svgPixmap(QStringLiteral("user"), Qt::white, 18));
    }
    av.setDevicePixelRatio(2.0);
    ui->labelAvatar->setPixmap(av);

    ui->labelChevron->setPixmap(UI::svgPixmap(QStringLiteral("chevron"), QColor("#23252e"), 16));
}

void MainWindow::onBellClicked()
{
    ui->pageMachines->showAlertsMenu(ui->btnBell);
}

void MainWindow::updateBadge(int count)
{
    m_badge->setText(QString::number(count));
    m_badge->setVisible(count > 0);
}

void MainWindow::showToast(const QString &message, bool error)
{
    m_toast->setText(message);
    m_toast->setProperty("error", error);
    m_toast->style()->unpolish(m_toast);
    m_toast->style()->polish(m_toast);
    m_toast->update();
    const int w = 360;
    m_toast->setFixedWidth(w);
    m_toast->resize(w, qMax(48, m_toast->heightForWidth(w)));
    m_toast->move(ui->central->width() - w - 22, 72);
    m_toast->show();
    m_toast->raise();
    QTimer::singleShot(5000, m_toast, &QLabel::hide);
}

// ================= source fusionné : metiers.cpp =================
#include <algorithm>
#include <utility>
#include <QHash>
#include <QMap>
#include <QSet>
#include <QtMath>

namespace Metier {

/* ===================== Métier 1 : performance par type ===================== */

QVector<Performance> performanceParType(const QList<Machine> &machines,
                                        const QList<MachineReading> &readings)
{
    Q_UNUSED(readings);
    struct Totals {
        double operating = 0.0;
        double stopped = 0.0;
        int produced = 0;
    };
    QMap<QString, Performance> byType;
    for (const Machine &m : machines) {
        byType[m.type].type = m.type;
        ++byType[m.type].totalMachines;
    }
    QMap<QString, Totals> totals;
    for (const Machine &m : machines) {
        Totals &t = totals[m.type];
        t.operating += m.heuresFonctionnement;
        t.stopped += m.heuresArret;
        t.produced += m.piecesProduites;
    }
    double bestRate = 0.0;
    for (auto it = totals.cbegin(); it != totals.cend(); ++it)
        if (it->operating > 0.0)
            bestRate = qMax(bestRate, it->produced / it->operating);

    QVector<Performance> list;
    for (auto it = byType.cbegin(); it != byType.cend(); ++it) {
        const Totals v = totals.value(it.key());
        if (v.operating <= 0.0) continue;
        Performance p = it.value();
        p.nombreMachines = p.totalMachines;
        p.piecesParHeure = v.produced / v.operating;
        p.disponibilite = v.operating + v.stopped > 0.0
            ? 100.0 * v.operating / (v.operating + v.stopped) : 0.0;
        p.productionAtteinte = bestRate > 0.0 ? 100.0 * p.piecesParHeure / bestRate : 0.0;
        p.score = (p.disponibilite + p.productionAtteinte) / 2.0;
        list.append(p);
    }

    std::stable_sort(list.begin(), list.end(), [](const Performance &a, const Performance &b) {
        if (qAbs(a.score - b.score) > 0.001) return a.score > b.score;
        if (a.nombreMachines != b.nombreMachines) return a.nombreMachines > b.nombreMachines;
        return a.type < b.type;
    });
    return list;
}

/* ===================== Métier 2 : planning de maintenance ===================== */

bool estJourOuvrable(const QDate &d)
{
    return d.dayOfWeek() < 6;      // 6 = samedi, 7 = dimanche
}

QString Intervention::raison() const
{
    switch (motif) {
    case Indisponible: return QStringLiteral("indisponible : traitée en priorité");
    case Retard:       return QStringLiteral("en retard de %1 j").arg(joursRetard);
    case EnCours:      return QStringLiteral("maintenance en cours");
    case PerformanceFaible: return QStringLiteral("type sous 70 % de performance : entretien avancé (%1 %)").arg(qRound(scoreType));
    case Weekend:      return QStringLiteral("week-end : reportée au jour ouvrable");
    case Conflit:      return QStringLiteral("même type déjà prévu ce jour");
    case Capacite:     return QStringLiteral("capacité journalière de 8 h atteinte : reportée");
    case Conservee:    break;
    }
    return QStringLiteral("date conservée");
}

namespace {
struct Item { const Machine *m; int rang; double score; bool hasScore; double downtimeRatio; };
}

QVector<Intervention> planifierMaintenances(const QList<Machine> &machines,
                                            const QVector<Performance> &performances,
                                            const QList<MaintenanceRecord> &maintenances,
                                            const QDate &today)
{
    QHash<QString, double> scoreParType;
    for (const Performance &p : performances) scoreParType.insert(p.type, p.score);
    QHash<QString, double> dureeParMachine;
    QHash<QString, int> recordIdParMachine;
    for (const MaintenanceRecord &r : maintenances) {
        if (r.status != QStringLiteral("Planifiée") && r.status != QStringLiteral("En cours")) continue;
        if (r.id >= recordIdParMachine.value(r.machineId, 0)) {
            recordIdParMachine.insert(r.machineId, r.id);
            dureeParMachine.insert(r.machineId, r.estimatedHours);
        }
    }

    QVector<Item> order;
    for (const Machine &m : machines) {
        const int j = int(today.daysTo(m.prochaineMaintenance));
        const bool hasScore = scoreParType.contains(m.type);
        const double score = scoreParType.value(m.type, 100.0);
        int rang = 3;
        if (m.etat == Etat::indisponible())                 rang = 0;
        else if (j < 0)                                     rang = 1;
        else if (m.etat == Etat::maintenance())             rang = 2;
        else if (hasScore && score < 70.0 && j <= 30)        rang = 3;
        else                                                 rang = 4;
        const double totalHours = m.heuresFonctionnement + m.heuresArret;
        const double downtimeRatio = totalHours > 0.0 ? m.heuresArret / totalHours : 0.0;
        order.append({ &m, rang, score, hasScore, downtimeRatio });
    }
    // priorité de traitement : rang, puis date la plus ancienne, puis identifiant
    std::stable_sort(order.begin(), order.end(), [](const Item &a, const Item &b) {
        if (a.rang != b.rang) return a.rang < b.rang;
        if (qAbs(a.downtimeRatio - b.downtimeRatio) > 0.001)
            return a.downtimeRatio > b.downtimeRatio;
        if (qAbs(a.m->heuresFonctionnement - b.m->heuresFonctionnement) > 0.001)
            return a.m->heuresFonctionnement > b.m->heuresFonctionnement;
        if (a.hasScore && b.hasScore && qAbs(a.score - b.score) > 0.001)
            return a.score < b.score;
        if (a.m->prochaineMaintenance != b.m->prochaineMaintenance)
            return a.m->prochaineMaintenance < b.m->prochaineMaintenance;
        return a.m->id < b.m->id;
    });

    QSet<QString> occupe;            // « type|date » déjà planifiés
    QHash<QString, double> heuresParJour;
    auto key = [](const QString &type, const QDate &d) { return type + QLatin1Char('|') + d.toString(Qt::ISODate); };

    QVector<Intervention> plan;
    for (const Item &it : std::as_const(order)) {
        const Machine &m = *it.m;
        Intervention iv;
        iv.id = m.id; iv.nom = m.nom; iv.type = m.type;
        iv.actuelle = m.prochaineMaintenance;
        iv.joursRetard = qMax(0, -int(today.daysTo(m.prochaineMaintenance)));
        iv.scoreDisponible = scoreParType.contains(m.type);
        iv.scoreType = scoreParType.value(m.type, 0.0);
        iv.dureeEstimee = qBound(0.5, dureeParMachine.value(m.id, 2.0), 8.0);

        QDate d = (it.rang <= 3) ? today : m.prochaineMaintenance;
        Intervention::Motif premierObstacle = Intervention::Conservee;
        while (!estJourOuvrable(d) || occupe.contains(key(m.type, d))
               || heuresParJour.value(d.toString(Qt::ISODate)) + iv.dureeEstimee > 8.0) {
            if (premierObstacle == Intervention::Conservee) {
                if (!estJourOuvrable(d)) premierObstacle = Intervention::Weekend;
                else if (occupe.contains(key(m.type, d))) premierObstacle = Intervention::Conflit;
                else premierObstacle = Intervention::Capacite;
            }
            d = d.addDays(1);
        }
        occupe.insert(key(m.type, d));
        heuresParJour[d.toString(Qt::ISODate)] += iv.dureeEstimee;
        iv.proposee = d;

        switch (it.rang) {
        case 0:  iv.motif = Intervention::Indisponible; break;
        case 1:  iv.motif = Intervention::Retard;       break;
        case 2:  iv.motif = Intervention::EnCours;      break;
        case 3:  iv.motif = Intervention::PerformanceFaible; break;
        default: iv.motif = premierObstacle;            break;
        }
        plan.append(iv);
    }

    // affichage : par date proposée, puis par priorité de traitement (ordre d'insertion), puis par id
    QVector<int> idx(plan.size());
    for (int i = 0; i < idx.size(); ++i) idx[i] = i;
    std::stable_sort(idx.begin(), idx.end(), [&](int a, int b) {
        if (plan[a].proposee != plan[b].proposee) return plan[a].proposee < plan[b].proposee;
        return a < b;
    });
    QVector<Intervention> sorted;
    for (int i : std::as_const(idx)) sorted.append(plan[i]);
    return sorted;
}

} // namespace Metier

// ================= source fusionné : recorddialogs.cpp =================
#include <QComboBox>
#include <QDateEdit>
#include <QDialogButtonBox>
#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QSpinBox>
#include <QVBoxLayout>

namespace {
void setupMachineCombo(QComboBox *combo, const QList<Machine> &machines)
{
    for (const Machine &m : machines)
        combo->addItem(QStringLiteral("%1 — %2 (%3)").arg(m.id, m.nom, m.type), m.id);
}

QDoubleSpinBox *hoursSpin(QWidget *parent, double maximum = 24.0)
{
    auto *spin = new QDoubleSpinBox(parent);
    spin->setRange(0.0, maximum);
    spin->setDecimals(1);
    spin->setSingleStep(0.5);
    spin->setSuffix(QStringLiteral(" h"));
    return spin;
}
}

MachineReadingDialog::MachineReadingDialog(const QList<Machine> &machines, QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(QStringLiteral("Saisir un relevé de production"));
    setMinimumWidth(430);

    m_machine = new QComboBox(this);
    setupMachineCombo(m_machine, machines);
    m_date = new QDateEdit(QDate::currentDate(), this);
    m_date->setCalendarPopup(true);
    m_date->setDisplayFormat(QStringLiteral("dd/MM/yyyy"));
    m_date->setMaximumDate(QDate::currentDate());
    m_planned = hoursSpin(this);
    m_planned->setValue(8.0);
    m_operating = hoursSpin(this);
    m_operating->setValue(8.0);
    m_downtime = hoursSpin(this);
    m_produced = new QSpinBox(this);
    m_produced->setRange(0, 100000000);
    m_target = new QSpinBox(this);
    m_target->setRange(1, 100000000);
    m_target->setValue(100);
    m_reason = new QLineEdit(this);
    m_reason->setPlaceholderText(QStringLiteral("Ex. panne, réglage, manque de matière"));

    auto *form = new QFormLayout;
    form->addRow(QStringLiteral("Machine"), m_machine);
    form->addRow(QStringLiteral("Date du relevé"), m_date);
    form->addRow(QStringLiteral("Heures prévues"), m_planned);
    form->addRow(QStringLiteral("Heures de fonctionnement"), m_operating);
    form->addRow(QStringLiteral("Heures d'arrêt"), m_downtime);
    form->addRow(QStringLiteral("Quantité produite (pièces)"), m_produced);
    form->addRow(QStringLiteral("Objectif (pièces)"), m_target);
    form->addRow(QStringLiteral("Cause d'arrêt"), m_reason);

    auto *buttons = new QDialogButtonBox(QDialogButtonBox::Save | QDialogButtonBox::Cancel, this);
    buttons->button(QDialogButtonBox::Save)->setText(QStringLiteral("Enregistrer"));
    buttons->button(QDialogButtonBox::Cancel)->setText(QStringLiteral("Annuler"));
    connect(buttons, &QDialogButtonBox::accepted, this, &MachineReadingDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &MachineReadingDialog::reject);
    auto *layout = new QVBoxLayout(this);
    layout->addLayout(form);
    layout->addWidget(buttons);
}

MachineReading MachineReadingDialog::reading() const
{
    MachineReading r;
    r.machineId = m_machine->currentData().toString();
    r.date = m_date->date();
    r.plannedHours = m_planned->value();
    r.operatingHours = m_operating->value();
    r.downtimeHours = m_downtime->value();
    r.quantityProduced = m_produced->value();
    r.productionTarget = m_target->value();
    r.stopReason = m_reason->text().trimmed();
    return r;
}

void MachineReadingDialog::accept()
{
    if (m_machine->currentIndex() < 0) {
        QMessageBox::warning(this, windowTitle(), QStringLiteral("Ajoutez d'abord une machine."));
        return;
    }
    if (m_planned->value() <= 0.0
        || m_operating->value() + m_downtime->value() > m_planned->value() + 0.001) {
        QMessageBox::warning(this, windowTitle(),
            QStringLiteral("Les heures de fonctionnement et d'arrêt ne peuvent pas dépasser les heures prévues."));
        return;
    }
    QDialog::accept();
}

MaintenancePlanDialog::MaintenancePlanDialog(const QList<Machine> &machines,
                                               const QList<MaintenanceRecord> &existing,
                                               QWidget *parent)
    : QDialog(parent), m_machines(machines), m_existing(existing)
{
    setWindowTitle(QStringLiteral("Planifier une maintenance"));
    setMinimumWidth(430);

    m_machine = new QComboBox(this);
    setupMachineCombo(m_machine, machines);
    m_date = new QDateEdit(QDate::currentDate(), this);
    m_date->setCalendarPopup(true);
    m_date->setDisplayFormat(QStringLiteral("dd/MM/yyyy"));
    m_date->setMinimumDate(QDate::currentDate());
    m_type = new QComboBox(this);
    m_type->addItems({QStringLiteral("Préventive"), QStringLiteral("Corrective")});
    m_priority = new QSpinBox(this);
    m_priority->setRange(1, 5);
    m_priority->setValue(3);
    m_priority->setToolTip(QStringLiteral("1 = faible priorité, 5 = priorité maximale"));
    m_duration = hoursSpin(this, 8.0);
    m_duration->setValue(2.0);
    m_description = new QLineEdit(this);
    m_description->setPlaceholderText(QStringLiteral("Motif ou tâche prévue"));

    auto *form = new QFormLayout;
    form->addRow(QStringLiteral("Machine"), m_machine);
    form->addRow(QStringLiteral("Date prévue"), m_date);
    form->addRow(QStringLiteral("Intervention"), m_type);
    form->addRow(QStringLiteral("Priorité (1 à 5)"), m_priority);
    form->addRow(QStringLiteral("Durée estimée"), m_duration);
    form->addRow(QStringLiteral("Description"), m_description);

    auto *buttons = new QDialogButtonBox(QDialogButtonBox::Save | QDialogButtonBox::Cancel, this);
    buttons->button(QDialogButtonBox::Save)->setText(QStringLiteral("Enregistrer"));
    buttons->button(QDialogButtonBox::Cancel)->setText(QStringLiteral("Annuler"));
    connect(buttons, &QDialogButtonBox::accepted, this, &MaintenancePlanDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &MaintenancePlanDialog::reject);
    auto *layout = new QVBoxLayout(this);
    layout->addLayout(form);
    layout->addWidget(buttons);
}

MaintenanceRecord MaintenancePlanDialog::record() const
{
    MaintenanceRecord r;
    r.machineId = m_machine->currentData().toString();
    r.plannedDate = m_date->date();
    r.interventionType = m_type->currentText();
    r.priority = m_priority->value();
    r.estimatedHours = m_duration->value();
    r.description = m_description->text().trimmed();
    return r;
}

void MaintenancePlanDialog::accept()
{
    if (m_machine->currentIndex() < 0) {
        QMessageBox::warning(this, windowTitle(), QStringLiteral("Ajoutez d'abord une machine."));
        return;
    }
    if (m_duration->value() <= 0.0) {
        QMessageBox::warning(this, windowTitle(), QStringLiteral("La durée doit être supérieure à zéro."));
        return;
    }
    if (m_date->date().dayOfWeek() >= 6) {
        QMessageBox::warning(this, windowTitle(), QStringLiteral("Le planning est limité aux jours ouvrables (lundi à vendredi)."));
        return;
    }
    QString selectedType;
    for (const Machine &m : m_machines)
        if (m.id == m_machine->currentData().toString()) selectedType = m.type;
    for (const MaintenanceRecord &r : m_existing) {
        if (r.machineId == m_machine->currentData().toString()
            || r.status != QStringLiteral("Planifiée") || r.plannedDate != m_date->date()) continue;
        const double usedHours = [&] {
            double total = 0.0;
            for (const MaintenanceRecord &dayRecord : m_existing)
                if (dayRecord.machineId != m_machine->currentData().toString()
                    && dayRecord.status == QStringLiteral("Planifiée") && dayRecord.plannedDate == m_date->date())
                    total += dayRecord.estimatedHours;
            return total;
        }();
        if (usedHours + m_duration->value() > 8.0) {
            QMessageBox::warning(this, windowTitle(),
                QStringLiteral("La capacité maximale de maintenance pour cette journée est de 8 heures."));
            return;
        }
        QString otherType;
        for (const Machine &m : m_machines)
            if (m.id == r.machineId) otherType = m.type;
        if (otherType == selectedType) {
            QMessageBox::warning(this, windowTitle(),
                QStringLiteral("Une autre machine du type %1 est déjà planifiée ce jour-là.").arg(selectedType));
            return;
        }
    }
    QDialog::accept();
}

// ================= source fusionné : sidebarwidget.cpp =================
#include <QPainter>

// Limites des 3 zones dans l'image d'origine (534 x 1600 px) :
//   0 .. kTopEnd        : logo FashioNova
//   kTopEnd .. kBotStart : fond bleu uni (zone étirable, derrière les boutons)
//   kBotStart .. fin    : photo + slogan « Fashion is a lifestyle »
static const int kTopEnd   = 420;
static const int kBotStart = 840;

SidebarWidget::SidebarWidget(QWidget *parent)
    : QFrame(parent), m_bg(QStringLiteral(":/images/sidebar_bg.jpg"))
{
}

void SidebarWidget::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::SmoothPixmapTransform);
    p.fillRect(rect(), QColor("#07213f"));
    if (m_bg.isNull())
        return;

    const qreal w  = width();
    const qreal h  = height();
    const qreal bw = m_bg.width();
    const qreal bh = m_bg.height();
    const qreal s  = w / bw;                         // échelle : l'image occupe toute la largeur

    const qreal dstTop = kTopEnd * s;
    const qreal dstBot = (bh - kBotStart) * s;

    if (h < dstTop + dstBot) {                       // fenêtre vraiment très basse : image entière
        p.drawPixmap(QRectF(0, 0, w, h), m_bg, QRectF(0, 0, bw, bh));
        return;
    }
    p.drawPixmap(QRectF(0, 0, w, dstTop), m_bg, QRectF(0, 0, bw, kTopEnd));
    p.drawPixmap(QRectF(0, dstTop, w, h - dstTop - dstBot), m_bg,
                 QRectF(0, kTopEnd, bw, kBotStart - kTopEnd));
    p.drawPixmap(QRectF(0, h - dstBot, w, dstBot), m_bg, QRectF(0, kBotStart, bw, bh - kBotStart));
}

// ================= source fusionné : uihelpers.cpp =================
#include <QFile>
#include <QFileInfo>
#include <QPainter>
#include <QPainterPath>
#include <QSvgRenderer>

namespace UI {

QPixmap svgPixmap(const QString &name, const QColor &color, int size)
{
    QFile f(QStringLiteral(":/icons/%1.svg").arg(name));
    QByteArray data;
    if (f.open(QIODevice::ReadOnly))
        data = f.readAll();
    data.replace("currentColor", color.name().toUtf8());

    QSvgRenderer renderer(data);
    const int px = size * 2;                     // rendu 2x pour les écrans HiDPI
    QPixmap pm(px, px);
    pm.fill(Qt::transparent);
    QPainter p(&pm);
    renderer.render(&p);
    p.end();
    pm.setDevicePixelRatio(2.0);
    return pm;
}

QIcon svgIcon(const QString &name, const QColor &color, int size)
{
    return QIcon(svgPixmap(name, color, size));
}

QIcon dotIcon(const QColor &color, int size)
{
    const int px = size * 2;
    QPixmap pm(px, px);
    pm.fill(Qt::transparent);
    QPainter p(&pm);
    p.setRenderHint(QPainter::Antialiasing);
    p.setBrush(color);
    p.setPen(Qt::NoPen);
    p.drawEllipse(QRectF(px * 0.12, px * 0.12, px * 0.76, px * 0.76));
    p.end();
    pm.setDevicePixelRatio(2.0);
    return QIcon(pm);
}

QPixmap cover(const QPixmap &src, const QSize &size, int radius)
{
    const qreal dpr = 2.0;
    const QSize px(int(size.width() * dpr), int(size.height() * dpr));
    QPixmap scaled = src.scaled(px, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
    QPixmap out(px);
    out.fill(Qt::transparent);
    QPainter p(&out);
    p.setRenderHint(QPainter::Antialiasing);
    QPainterPath path;
    path.addRoundedRect(QRectF(0, 0, px.width(), px.height()), radius * dpr, radius * dpr);
    p.setClipPath(path);
    p.drawPixmap((px.width() - scaled.width()) / 2, (px.height() - scaled.height()) / 2, scaled);
    p.end();
    out.setDevicePixelRatio(dpr);
    return out;
}

QPixmap machinePixmap(const QString &photoPath)
{
    if (!photoPath.isEmpty() && QFileInfo::exists(photoPath)) {
        QPixmap pm(photoPath);
        if (!pm.isNull())
            return pm;
    }
    return QPixmap(QStringLiteral(":/images/machine.jpg"));
}

QColor etatColor(const QString &etat)
{
    if (etat == QLatin1String("Disponible"))    return QColor("#2e9e5b");
    if (etat == QLatin1String("En maintenance")) return QColor("#e6a23c");
    return QColor("#e5484d");
}

QColor typeColor(const QString &type)
{
    if (type == QLatin1String("Couture"))   return QColor("#0a2f5b");
    if (type == QLatin1String("Coupe"))     return QColor("#6aa7e0");
    if (type == QLatin1String("Finition"))  return QColor("#f0c48a");
    if (type == QLatin1String("Broderie"))  return QColor("#d6a566");
    if (type == QLatin1String("Repassage")) return QColor("#b4c7dd");
    return QColor("#9a9a9a");
}

} // namespace UI

// ================= source fusionné : main.cpp =================
#include <QApplication>
#include <QFile>
#include <QMessageBox>
int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
#endif
    QApplication app(argc, argv);
    QApplication::setOrganizationName(QStringLiteral("FashioNova"));
    QApplication::setApplicationName(QStringLiteral("FashioNova Machines"));

    QFile qss(QStringLiteral(":/style.qss"));
    if (qss.open(QIODevice::ReadOnly))
        app.setStyleSheet(QString::fromUtf8(qss.readAll()));

    QString error;
    if (!Database::open(&error)) {
        QMessageBox::critical(nullptr, QStringLiteral("Base de données"),
                              QStringLiteral("Impossible de se connecter à Oracle :\n%1\n\n"
                                             "Vérifiez le pilote Oracle ODBC 64 bits, le DSN FASHIONOVA_XE, "
                                             "le service Oracle et le compte FASHIONOVA.").arg(error));
        return 1;
    }

    MainWindow w;
    w.show();
    return app.exec();
}