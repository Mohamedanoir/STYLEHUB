#include "connexion.h"

#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QDebug>

bool Connexion::creerConnexion()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QODBC");

    db.setDatabaseName("Source_Projet2A");

    db.setUserName("fashionova");
    db.setPassword(qEnvironmentVariable("FASHIONOVA_DB_PASSWORD"));

    if (!db.open()) {
        m_erreur = db.lastError().text();
        qDebug() << "Erreur connexion Oracle :" << m_erreur;
        return false;
    }

    qDebug() << "Connexion Oracle réussie !";

    // Vérifier que la table MAQUETTES existe
    QSqlQuery q(db);

    if (!q.exec("SELECT COUNT(*) FROM MAQUETTES")) {
        m_erreur = q.lastError().text();
        qDebug() << "Erreur table MAQUETTES :" << m_erreur;
        return false;
    }

    qDebug() << "Table MAQUETTES accessible !";

    return true;
}
