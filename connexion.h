#ifndef CONNEXION_H
#define CONNEXION_H

#include <QString>

// Connexion à la base SQLite (fichier fashionova.db créé à côté de l'exécutable)
class Connexion
{
public:
    bool creerConnexion();
    QString derniereErreur() const { return m_erreur; }

private:
    QString m_erreur;
};

#endif // CONNEXION_H
