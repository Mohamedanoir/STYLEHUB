#include "connection.h"
#include <QSqlError>
#include <QDebug>

Connection::Connection() {}

bool Connection::createconnect()
{
    db = QSqlDatabase::addDatabase("QODBC");
    db.setDatabaseName(qEnvironmentVariable("FASHIONOVA_DSN", "fashionnova"));
    db.setUserName(qEnvironmentVariable("FASHIONOVA_DB_USER", "fashionnova"));
    db.setPassword(qEnvironmentVariable("FASHIONOVA_DB_PASSWORD"));

    if (db.password().isEmpty()) {
        qDebug() << "Erreur connexion BD : définissez FASHIONOVA_DB_PASSWORD.";
        return false;
    }
    if (db.open())
        return true;
    qDebug() << "Erreur connexion BD :" << db.lastError().text();
    return false;
}

void Connection::closeConnect() { db.close(); }
