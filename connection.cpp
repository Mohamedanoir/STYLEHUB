#include "connection.h"
#include <QDebug>
#include <QSqlError>

// Constructeur : réutilise la connexion par défaut si déjà créée, sinon l'initialise
Connection::Connection() {
    if (QSqlDatabase::contains()) {
        db = QSqlDatabase::database(QSqlDatabase::defaultConnection, false);
    } else {
        db = QSqlDatabase::addDatabase("QODBC");
    }
}

// Destructeur : fermeture propre de la connexion
Connection::~Connection() {
    if (db.isOpen())
        db.close();
}

// Singleton de Meyers (instance unique persistante)
Connection& Connection::createInstance() {
    static Connection instance;
    return instance;
}

// Établissement de la connexion ODBC vers Oracle
bool Connection::createConnection() {
    if (db.isOpen())
        return true;

    db.setDatabaseName("Source_Oracle");
    db.setUserName("fashionova_user");
    db.setPassword("admin123");

    if (db.open()) {
        qDebug() << "Connexion établie avec succès !";
        return true;
    }

    qDebug() << "Échec de la connexion :" << db.lastError().text();
    return false;
}