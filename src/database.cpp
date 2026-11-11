#include "database.h"
#include "connexion.h"
#include "machinevalidation.h"

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
        const QSqlDatabase shared = QSqlDatabase::database(QSqlDatabase::defaultConnection, false);
        if (!shared.isValid() || !shared.isOpen()) {
            if (error) *error = QStringLiteral("La connexion commune Oracle doit être ouverte avant le module Machines.");
            return false;
        }

        QSqlQuery verifyTable;
        if (!verifyTable.exec(QStringLiteral("SELECT ID_MACHINE FROM MACHINES WHERE 1=0"))) {
            if (error) *error = QStringLiteral("La table MACHINES est introuvable dans FASHIONOVA : ") + verifyTable.lastError().text();
            return false;
        }
        QSqlQuery q(QSqlDatabase::database(QSqlDatabase::defaultConnection, false));
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

    QSqlQuery q(QSqlDatabase::database(QSqlDatabase::defaultConnection, false));
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
    QSqlQuery q(QSqlDatabase::database(QSqlDatabase::defaultConnection, false));
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
        r.machineId = q.value(1).toString();
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
        r.machineId = q.value(1).toString();
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
bool Database::update(const Machine &m, QString *error)
{
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
bool Database::remove(const QString &id, QString *error)
{
    QSqlQuery q(QSqlDatabase::database(QSqlDatabase::defaultConnection, false));
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
    QSqlQuery q(QSqlDatabase::database(QSqlDatabase::defaultConnection, false));
    const bool oracle = isOracleConnection();
    q.prepare(QStringLiteral(
        "INSERT INTO releve_machine (%1ID_Machine, Date_Releve, Heures_Planifiees, "
        "Heures_Fonctionnement, Heures_Arret, Quantite_Produite, Objectif_Production, Cause_Arret) "
        "VALUES (%2?, ?, ?, ?, ?, ?, ?, ?)" )
        .arg(oracle ? QStringLiteral("ID_Releve, ") : QString(),
             oracle ? QStringLiteral("seq_releve_machine.NEXTVAL, ") : QString()));
    q.addBindValue(isOracleConnection() ? QVariant(r.machineId.toInt()) : QVariant(r.machineId));
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
        QSqlQuery retire;
        retire.prepare(QStringLiteral(
            "UPDATE maintenance SET Statut='Replanifiée' "
            "WHERE ID_Machine=? AND Statut='Planifiée'"));
        retire.addBindValue(isOracleConnection() ? QVariant(r.machineId.toInt()) : QVariant(r.machineId));
        if (!retire.exec()) {
            if (error) *error = retire.lastError().text();
            db.rollback();
            return false;
        }

        QSqlQuery insert;
        const bool oracle = isOracleConnection();
        insert.prepare(QStringLiteral(
            "INSERT INTO maintenance (%1ID_Machine, Date_Prevue, Type_Intervention, Priorite, "
            "Duree_Estimee_Heures, Statut, Description) VALUES (%2?, ?, ?, ?, ?, 'Planifiée', ?)" )
            .arg(oracle ? QStringLiteral("ID_Maintenance, ") : QString(),
            oracle ? QStringLiteral("seq_maintenance.NEXTVAL, ") : QString()));
        insert.addBindValue(isOracleConnection() ? QVariant(r.machineId.toInt()) : QVariant(r.machineId));
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
        updateMachine.addBindValue(oracle ? QVariant(r.machineId.toInt()) : QVariant(r.machineId));
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
