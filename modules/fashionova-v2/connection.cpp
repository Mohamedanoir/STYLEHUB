#include "connection.h"
#include <QSqlError>
#include <QDebug>

Connection& Connection::createInstance()
{
    static Connection instance;
    return instance;
}

bool Connection::createConnect()
{
    // Les QSqlQuery du modèle utilisent la connexion Qt par défaut.
    // Il faut donc ouvrir le pilote sur cette même connexion.
    const QString connectionName = QSqlDatabase::defaultConnection;
    if (QSqlDatabase::contains(connectionName))
        db = QSqlDatabase::database(connectionName);
    else
        db = QSqlDatabase::addDatabase("QODBC");

    if (db.isOpen()) return true;
    db.setDatabaseName("Source_Projet2A");
    db.setUserName(qEnvironmentVariable("FASHIONOVA_DB_USER", "aziz"));
    db.setPassword(qEnvironmentVariable("FASHIONOVA_DB_PASSWORD"));

    if (db.open()) {
        qDebug() << "Connexion reussie";
        return true;
    }
    qDebug() << "Echec :" << db.lastError().text();
    return false;
}

void Connection::closeConnect()
{
    db.close();
}
