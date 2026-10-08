#ifndef CLIENT_H
#define CLIENT_H

#include <QString>

struct Client {
    QString id, nom, prenom, tel, email, adresse, type, date, remarques;
};

struct Commande {
    QString numero, clientId, date, designation, statut;
    double montant = 0.0;
};

#endif // CLIENT_H
