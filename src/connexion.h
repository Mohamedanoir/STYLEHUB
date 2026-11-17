#pragma once

#include <QSqlDatabase>
#include <QString>

class Connexion
{
public:
    static Connexion &instance();

    bool ouvrir(const QString &utilisateur,
                const QString &motDePasse,
                QString *erreur = nullptr);
    void fermer();
    QSqlDatabase base() const;

    Connexion(const Connexion &) = delete;
    Connexion &operator=(const Connexion &) = delete;

private:
    Connexion();
    ~Connexion();

    QSqlDatabase m_db;
};
