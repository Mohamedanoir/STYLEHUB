#include "connection.h"
#include <QSqlError>
#include <QDebug>

Connection::Connection() {}

bool Connection::createconnect()
{
    db = QSqlDatabase::addDatabase("QODBC");
    db.setDatabaseName("fashionnova");   // <-- nom de la source ODBC (à adapter)
    db.setUserName("fashionnova");            // <-- utilisateur Oracle (à adapter)
    db.setPassword("123456789");             // <-- mot de passe Oracle (à adapter)

    if (db.open())
        return true;
    qDebug() << "Erreur connexion BD :" << db.lastError().text();
    return false;
}

void Connection::closeConnect() { db.close(); }
