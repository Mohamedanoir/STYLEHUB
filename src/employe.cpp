#include "employe.h"

#include "connexion.h"

#include <QSqlError>
#include <QSqlQuery>
#include <QStringList>

namespace {
QString diagnostic(const QSqlError &error)
{
    QStringList details;
    if (!error.nativeErrorCode().isEmpty())
        details << QStringLiteral("Code Oracle : ") + error.nativeErrorCode();
    if (!error.databaseText().isEmpty())
        details << QStringLiteral("Base : ") + error.databaseText();
    if (!error.driverText().isEmpty())
        details << QStringLiteral("Pilote : ") + error.driverText();
    return details.isEmpty() ? error.text() : details.join(QLatin1Char('\n'));
}
}

QString Employe::code() const
{
    return QStringLiteral("EMP%1").arg(id, 3, 10, QChar('0'));
}

bool Employe::preparerSchemaPresence(QString *erreur)
{
    QSqlQuery query(Connexion::instance().base());
    query.prepare(QStringLiteral(
        "SELECT COUNT(*) FROM USER_TAB_COLUMNS "
        "WHERE TABLE_NAME='EMPLOYES' AND COLUMN_NAME='ETAT_PRESENCE'"));
    if (!query.exec() || !query.next()) {
        if (erreur)
            *erreur = diagnostic(query.lastError());
        return false;
    }

    if (query.value(0).toInt() == 0) {
        QSqlQuery alter(Connexion::instance().base());
        if (!alter.exec(QStringLiteral(
                "ALTER TABLE EMPLOYES ADD (ETAT_PRESENCE VARCHAR2(10) "
                "DEFAULT 'ABSENT' NOT NULL)"))) {
            if (erreur)
                *erreur = diagnostic(alter.lastError());
            return false;
        }
    }

    query.prepare(QStringLiteral(
        "SELECT COUNT(*) FROM USER_CONSTRAINTS "
        "WHERE TABLE_NAME='EMPLOYES' AND CONSTRAINT_NAME='CK_EMP_PRESENCE'"));
    if (!query.exec() || !query.next()) {
        if (erreur)
            *erreur = diagnostic(query.lastError());
        return false;
    }
    if (query.value(0).toInt() == 0) {
        QSqlQuery alter(Connexion::instance().base());
        if (!alter.exec(QStringLiteral(
                "ALTER TABLE EMPLOYES ADD CONSTRAINT CK_EMP_PRESENCE "
                "CHECK (ETAT_PRESENCE IN ('PRESENT','ABSENT'))"))) {
            if (erreur)
                *erreur = diagnostic(alter.lastError());
            return false;
        }
    }
    return true;
}

bool Employe::authentifier(const QString &identifiant,
                           const QString &motDePasse,
                           Employe *employe,
                           QString *erreur)
{
    QString numero = identifiant.trimmed().toUpper();
    if (numero.startsWith(QStringLiteral("EMP")))
        numero.remove(0, 3);

    bool idValide = false;
    const qlonglong id = numero.toLongLong(&idValide);
    if (!idValide || id <= 0 || motDePasse.isEmpty()) {
        if (erreur)
            *erreur = QStringLiteral("Saisissez un ID (ex. EMP001) et un mot de passe.");
        return false;
    }

    QSqlQuery query(Connexion::instance().base());
    query.prepare(QStringLiteral(
        "SELECT ID_Employe, Nom_Employe, Prenom_Employe, Poste, Salaire, Mot_de_passe, ETAT_PRESENCE "
        "FROM EMPLOYES WHERE ID_Employe=%1 AND Mot_de_passe=?")
        .arg(QString::number(id)));
    query.addBindValue(motDePasse);

    if (!query.exec()) {
        if (erreur)
            *erreur = diagnostic(query.lastError());
        return false;
    }
    if (!query.next()) {
        if (erreur)
            *erreur = QStringLiteral("ID employé ou mot de passe incorrect.");
        return false;
    }

    if (employe) {
        employe->id = query.value(0).toLongLong();
        employe->nom = query.value(1).toString();
        employe->prenom = query.value(2).toString();
        employe->poste = query.value(3).toString();
        employe->salaire = query.value(4).toDouble();
        employe->motDePasse = query.value(5).toString();
        employe->etatPresence = query.value(6).toString();
    }
    return true;
}

bool Employe::ajouter(QString *erreur)
{
    QSqlQuery query(Connexion::instance().base());
    query.prepare(QStringLiteral(
        "INSERT INTO EMPLOYES "
        "(ID_Employe, Nom_Employe, Prenom_Employe, Poste, Salaire, Mot_de_passe) "
        "VALUES (SEQ_EMPLOYES.NEXTVAL, ?, ?, ?, ?, ?)"));
    query.addBindValue(nom);
    query.addBindValue(prenom);
    query.addBindValue(poste);
    query.addBindValue(salaire);
    query.addBindValue(motDePasse);

    if (!query.exec()) {
        if (erreur)
            *erreur = diagnostic(query.lastError());
        return false;
    }

    QSqlQuery idQuery(Connexion::instance().base());
    if (idQuery.exec(QStringLiteral("SELECT SEQ_EMPLOYES.CURRVAL FROM DUAL")) && idQuery.next())
        id = idQuery.value(0).toLongLong();

    return true;
}

bool Employe::modifier(QString *erreur) const
{
    if (id <= 0) {
        if (erreur)
            *erreur = QStringLiteral("Identifiant employe invalide.");
        return false;
    }

    QSqlQuery query(Connexion::instance().base());
    query.prepare(QStringLiteral(
        "UPDATE EMPLOYES SET Nom_Employe=?, Prenom_Employe=?, "
        "Poste=?, Salaire=?, Mot_de_passe=? WHERE ID_Employe=%1")
        .arg(QString::number(id)));
    query.addBindValue(nom);
    query.addBindValue(prenom);
    query.addBindValue(poste);
    query.addBindValue(salaire);
    query.addBindValue(motDePasse);
    if (!query.exec()) {
        if (erreur)
            *erreur = diagnostic(query.lastError());
        return false;
    }
    if (query.numRowsAffected() == 0) {
        if (erreur)
            *erreur = QStringLiteral("Aucun employé correspondant à cet identifiant.");
        return false;
    }
    return true;
}

bool Employe::supprimer(qlonglong id, QString *erreur)
{
    if (id <= 0) {
        if (erreur)
            *erreur = QStringLiteral("Identifiant employe invalide.");
        return false;
    }

    QSqlQuery query(Connexion::instance().base());
    // The ID comes from the selected database row and is an integer, so putting
    // its decimal representation directly in SQL avoids ODBC NUMBER bind issues.
    query.prepare(QStringLiteral("DELETE FROM EMPLOYES WHERE ID_Employe=%1")
                      .arg(QString::number(id)));

    if (!query.exec()) {
        if (erreur)
            *erreur = diagnostic(query.lastError());
        return false;
    }
    if (query.numRowsAffected() == 0) {
        if (erreur)
            *erreur = QStringLiteral("Aucun employé correspondant à cet identifiant.");
        return false;
    }
    return true;
}

bool Employe::changerPresence(qlonglong id, bool present, QString *erreur)
{
    if (id <= 0) {
        if (erreur)
            *erreur = QStringLiteral("Identifiant employe invalide.");
        return false;
    }

    QSqlQuery query(Connexion::instance().base());
    query.prepare(QStringLiteral("UPDATE EMPLOYES SET ETAT_PRESENCE=? WHERE ID_Employe=%1")
                      .arg(QString::number(id)));
    query.addBindValue(present ? QStringLiteral("PRESENT") : QStringLiteral("ABSENT"));
    if (!query.exec()) {
        if (erreur)
            *erreur = diagnostic(query.lastError());
        return false;
    }
    if (query.numRowsAffected() == 0) {
        if (erreur)
            *erreur = QStringLiteral("Aucun employe correspondant a cet identifiant.");
        return false;
    }
    return true;
}

QList<Employe> Employe::afficher(QString *erreur)
{
    QList<Employe> employes;
    QSqlQuery query(Connexion::instance().base());
    query.prepare(QStringLiteral(
        "SELECT ID_Employe, Nom_Employe, Prenom_Employe, Poste, Salaire, Mot_de_passe, ETAT_PRESENCE "
        "FROM EMPLOYES ORDER BY ID_Employe"));

    if (!query.exec()) {
        if (erreur)
            *erreur = diagnostic(query.lastError());
        return employes;
    }

    while (query.next()) {
        Employe e;
        e.id = query.value(0).toLongLong();
        e.nom = query.value(1).toString();
        e.prenom = query.value(2).toString();
        e.poste = query.value(3).toString();
        e.salaire = query.value(4).toDouble();
        e.motDePasse = query.value(5).toString();
        e.etatPresence = query.value(6).toString();
        employes.append(e);
    }
    return employes;
}
