#include "machine.h"
#include "machinevalidation.h"
#include "connexion.h"

#include <QDebug>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>

namespace {
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
}

QList<Machine> Machine::afficher(QString *error)
{
    if (error) error->clear();
    QList<Machine> list;
    const QSqlDatabase activeDb = QSqlDatabase::database(QSqlDatabase::defaultConnection, false);
    const bool oracle = activeDb.isValid() && activeDb.driverName() == QStringLiteral("QODBC");
    const QString sql = oracle
        ? QStringLiteral("SELECT ID_MACHINE, NOM_MACHINE, TYPE_MACHINE, ETAT, TO_CHAR(DATE_PROCHAINE_MAINTENANCE, 'YYYY-MM-DD'), '' AS PHOTO, NVL(HEURES_FONCTIONNEMENT,0), NVL(PIECES_PRODUITES,0), NVL(HEURES_ARRET,0) FROM MACHINES ORDER BY ID_MACHINE")
        : QStringLiteral("SELECT ID_Machine, Nom_Machine, Type_Machine, Etat, Date_Prochaine_Maintenance, Photo, Heures_Fonctionnement, Pieces_Produites, Heures_Arret FROM machine ORDER BY ID_Machine");
    QSqlQuery q(QSqlDatabase::database(QSqlDatabase::defaultConnection, false));
    q.setForwardOnly(true);
    if (!q.exec(sql)) {
        if (error) *error = q.lastError().text();
        qWarning() << "Machine::afficher failed:" << q.lastError().text();
        return list;
    }
    while (q.next()) list.append(fromQuery(q));
    if (q.lastError().isValid()) {
        if (error) *error = q.lastError().text();
        qWarning() << "Machine::afficher fetch failed:" << q.lastError().text();
    }
    return list;
}

bool Machine::ajouter(QString *error) const
{
    const Machine &m = *this;
    if (!MachineValidation::validate(m, error)) return false;

    QSqlQuery q(QSqlDatabase::database(QSqlDatabase::defaultConnection, false));
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

bool Machine::modifier(QString *error) const
{
    const Machine &m = *this;
    if (!MachineValidation::validate(m, error)) return false;
    QSqlQuery q(QSqlDatabase::database(QSqlDatabase::defaultConnection, false));
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

bool Machine::supprimer(const QString &id, QString *error)
{
    QSqlQuery q(QSqlDatabase::database(QSqlDatabase::defaultConnection, false));
    const bool oracle = isOracleConnection();
    q.prepare(oracle ? QStringLiteral("DELETE FROM MACHINES WHERE ID_MACHINE=?") : QStringLiteral("DELETE FROM machine WHERE ID_Machine=?"));
    q.addBindValue(oracle ? QVariant(oracleIdValue(id)) : QVariant(id));
    if (!q.exec()) { if (error) *error = q.lastError().text(); return false; }
    if (q.numRowsAffected() == 0) { if (error) *error = QStringLiteral("Suppression impossible : machine introuvable."); return false; }
    return true;
}

QString Machine::prochainId()
{
    int max = 0;
    if (isOracleConnection()) {
        QSqlQuery q(QStringLiteral("SELECT NVL(MAX(ID_MACHINE), 0) FROM MACHINES"));
        if (!q.next()) return QString();
        max = q.value(0).toInt();
    } else {
        const QList<Machine> list = afficher();
        for (const Machine &m : list) max = qMax(max, oracleIdValue(m.id));
    }
    return QStringLiteral("MCH%1").arg(max + 1, 3, 10, QLatin1Char('0'));
}
