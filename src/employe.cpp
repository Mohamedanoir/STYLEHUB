#include "employe.h"

#include "connexion.h"

#include <QSqlError>
#include <QSqlQuery>

QString Employe::code() const
{
    return QStringLiteral("EMP%1").arg(id, 3, 10, QChar('0'));
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
            *erreur = query.lastError().text();
        return false;
    }

    QSqlQuery idQuery(Connexion::instance().base());
    if (idQuery.exec(QStringLiteral("SELECT SEQ_EMPLOYES.CURRVAL FROM DUAL")) && idQuery.next())
        id = idQuery.value(0).toLongLong();

    return true;
}

bool Employe::modifier(QString *erreur) const
{
    QSqlQuery query(Connexion::instance().base());
    query.prepare(QStringLiteral(
        "UPDATE EMPLOYES SET Nom_Employe=?, Prenom_Employe=?, "
        "Poste=?, Salaire=?, Mot_de_passe=? WHERE ID_Employe=?"));
    query.addBindValue(nom);
    query.addBindValue(prenom);
    query.addBindValue(poste);
    query.addBindValue(salaire);
    query.addBindValue(motDePasse);
    // EMPLOYES.ID_Employe is Oracle NUMBER with small sequence values.
    // Bind as a 32-bit integer; some Oracle ODBC builds reject Qt's qlonglong
    // binding with only the unhelpful "QODBC: unknown error".
    query.addBindValue(static_cast<int>(id));

    if (!query.exec()) {
        if (erreur)
            *erreur = query.lastError().text();
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
    const QSqlDatabase db = Connexion::instance().base();
    // Let Oracle enforce every declared foreign key. A manual MAQUETTES
    // pre-check is fragile across team schemas and caused QODBC failures.
    // The delete is safe: Oracle rejects it if any related row exists.
    QSqlQuery query(db);
    query.prepare(QStringLiteral("DELETE FROM EMPLOYES WHERE ID_Employe=?"));
    query.addBindValue(static_cast<int>(id));

    if (!query.exec()) {
        if (erreur)
            *erreur = query.lastError().text();
        return false;
    }
    if (query.numRowsAffected() == 0) {
        if (erreur)
            *erreur = QStringLiteral("Aucun employé correspondant à cet identifiant.");
        return false;
    }
    return true;
}

QList<Employe> Employe::afficher(QString *erreur)
{
    QList<Employe> employes;
    QSqlQuery query(Connexion::instance().base());
    query.prepare(QStringLiteral(
        "SELECT ID_Employe, Nom_Employe, Prenom_Employe, Poste, Salaire, Mot_de_passe "
        "FROM EMPLOYES ORDER BY ID_Employe"));

    if (!query.exec()) {
        if (erreur)
            *erreur = query.lastError().text();
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
        employes.append(e);
    }
    return employes;
}
