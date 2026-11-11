#pragma once

#include <QSqlDatabase>
#include <QString>

// Connexion Oracle partagée par le module Gestion des machines.
class Connection
{
public:
    static bool open(QString *error = nullptr);
    static bool isOpen();
    static QSqlDatabase database();

private:
    Connection() = delete;
};
