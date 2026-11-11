#pragma once
#include <QSqlDatabase>

// Connexion à la base Oracle commune de l'équipe (via ODBC).
// Le DSN, l'utilisateur et le mot de passe doivent être IDENTIQUES chez tous les membres.
class Connection {
public:
    Connection();
    bool createconnect();      // true si la connexion est ouverte
    void closeConnect();
private:
    QSqlDatabase db;
};
