#ifndef CONNEXION_H
#define CONNEXION_H

#include <QString>

// ============================================================================
//  Connexion à la base Oracle (QODBC, source ODBC "Source_Projet2A")
//  Patron Singleton : une seule connexion pour toute l'application.
//      Connexion::instance().creerConnexion();
// ============================================================================
class Connexion
{
public:
    static Connexion &instance();

    bool creerConnexion(const QString &utilisateur, const QString &motDePasse);
    void fermerConnexion();
    QString derniereErreur() const { return m_erreur; }

    Connexion(const Connexion &) = delete;
    Connexion &operator=(const Connexion &) = delete;

private:
    Connexion() = default;
    QString m_erreur;
};

#endif // CONNEXION_H
