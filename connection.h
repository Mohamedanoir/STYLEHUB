#ifndef CONNECTION_H
#define CONNECTION_H

#include <QSqlDatabase>
#include <QSqlError>
#include <QDebug>

/* Classe Connection
 * Implémentation du pattern Singleton pour gérer une connexion unique à la base de données.
 * Utilise QSqlDatabase pour établir et fermer la connexion.
 * L'instance est accessible via la méthode statique createInstance().
 * La méthode createConnection() configure et ouvre la base de données via QODBC.
 */

class Connection {
public:
    static Connection& createInstance(); // Retourne l'instance unique
    bool createConnection();            // Établit la connexion

private:
    QSqlDatabase db;

    Connection();
    ~Connection();

    Connection(const Connection&) = delete;
    Connection& operator=(const Connection&) = delete;
};

#endif // CONNECTION_H