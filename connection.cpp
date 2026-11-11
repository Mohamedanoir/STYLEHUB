#include "connection.h"

// Constructeur : initialisation avec le driver QODBC dans le Singleton
Connection::Connection() {
    db = QSqlDatabase::addDatabase("QODBC");
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
    db.setDatabaseName("Source_Oracle");
    db.setUserName("fashionova_user");    // Le nouvel utilisateur dédié propre
    db.setPassword("admin123");           // Le mot de passe défini pour cet utilisateur

    if (db.open()) {
        qDebug() << "Connexion établie avec succès !";
        return true;
    }

    qDebug() << "Échec de la connexion :" << db.lastError().text();
    return false;
}