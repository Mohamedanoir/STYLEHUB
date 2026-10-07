#include "database.h"

#include <QDate>
#include <QDir>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QStandardPaths>
#include <QVariant>

static const char *kIso = "yyyy-MM-dd";

static Machine fromQuery(const QSqlQuery &q)
{
    Machine m;
    m.id    = q.value(0).toString();
    m.nom   = q.value(1).toString();
    m.type  = q.value(2).toString();
    m.etat  = q.value(3).toString();
    m.prochaineMaintenance = QDate::fromString(q.value(4).toString(), QLatin1String(kIso));
    m.photo = q.value(5).toString();
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
        Database::insert(m);
    }
}

bool Database::open(QString *error)
{
    const QString dir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(dir);

    QSqlDatabase db = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"));
    db.setDatabaseName(dir + QStringLiteral("/fashionova_machines.db"));
    if (!db.open()) {
        if (error) *error = db.lastError().text();
        return false;
    }

    QSqlQuery q;
    if (!q.exec(QStringLiteral(
            "CREATE TABLE IF NOT EXISTS machine ("
            " ID_Machine TEXT PRIMARY KEY,"
            " Nom_Machine TEXT NOT NULL,"
            " Type_Machine TEXT NOT NULL,"
            " Etat TEXT NOT NULL,"
            " Date_Prochaine_Maintenance TEXT NOT NULL,"
            " Photo TEXT DEFAULT '')"))) {
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
    QSqlQuery q(QStringLiteral(
        "SELECT ID_Machine, Nom_Machine, Type_Machine, Etat, Date_Prochaine_Maintenance, Photo "
        "FROM machine ORDER BY ID_Machine"));
    while (q.next())
        list.append(fromQuery(q));
    return list;
}

bool Database::insert(const Machine &m, QString *error)
{
    QSqlQuery q;
    q.prepare(QStringLiteral(
        "INSERT INTO machine (ID_Machine, Nom_Machine, Type_Machine, Etat, Date_Prochaine_Maintenance, Photo) "
        "VALUES (?, ?, ?, ?, ?, ?)"));
    q.addBindValue(m.id);
    q.addBindValue(m.nom);
    q.addBindValue(m.type);
    q.addBindValue(m.etat);
    q.addBindValue(m.prochaineMaintenance.toString(QLatin1String(kIso)));
    q.addBindValue(m.photo);
    if (!q.exec()) {
        if (error) *error = q.lastError().text();
        return false;
    }
    return true;
}

bool Database::update(const Machine &m, QString *error)
{
    QSqlQuery q;
    q.prepare(QStringLiteral(
        "UPDATE machine SET Nom_Machine=?, Type_Machine=?, Etat=?, Date_Prochaine_Maintenance=?, Photo=? "
        "WHERE ID_Machine=?"));
    q.addBindValue(m.nom);
    q.addBindValue(m.type);
    q.addBindValue(m.etat);
    q.addBindValue(m.prochaineMaintenance.toString(QLatin1String(kIso)));
    q.addBindValue(m.photo);
    q.addBindValue(m.id);
    if (!q.exec()) {
        if (error) *error = q.lastError().text();
        return false;
    }
    return true;
}

bool Database::remove(const QString &id, QString *error)
{
    QSqlQuery q;
    q.prepare(QStringLiteral("DELETE FROM machine WHERE ID_Machine=?"));
    q.addBindValue(id);
    if (!q.exec()) {
        if (error) *error = q.lastError().text();
        return false;
    }
    return true;
}

QString Database::nextId()
{
    int max = 0;
    const QList<Machine> list = all();
    for (const Machine &m : list)
        max = qMax(max, m.id.mid(3).toInt());
    return QStringLiteral("MCH%1").arg(max + 1, 3, 10, QLatin1Char('0'));
}
