#ifndef COMMANDE_H
#define COMMANDE_H

#include <QString>
#include <QDate>
#include <QSqlQuery>
#include <QSqlQueryModel>

class Commande {
public:
    // Constructeurs
    Commande();
    Commande(QString id, QString codeCli, QString nomCli, QDate dateCmd,
             QString statut, double montant, QString article = "",
             QString taille = "", int quantite = 1, QString modePaiement = "",
             QString adresse = "", double lat = 0.0, double lng = 0.0);

    // Getters
    QString getIdCommande() const { return idCommande; }
    QString getCodeClient() const { return codeClient; }
    QString getNomClient() const { return nomClient; }
    QDate getDateCommande() const { return dateCommande; }
    QString getStatut() const { return statut; }
    double getMontant() const { return montant; }
    QString getArticle() const { return article; }
    QString getTaille() const { return taille; }
    int getQuantite() const { return quantite; }
    QString getModePaiement() const { return modePaiement; }
    QString getAdresseLivraison() const { return adresseLivraison; }
    double getLatitude() const { return latitude; }
    double getLongitude() const { return longitude; }

    // Méthodes CRUD
    bool ajouter();
    QSqlQueryModel* afficher();
    bool supprimer(QString id);
    bool modifier();

private:
    QString idCommande;
    QString codeClient;
    QString nomClient;
    QDate dateCommande;
    QString statut;
    double montant;
    QString article;
    QString taille;
    int quantite;
    QString modePaiement;
    QString adresseLivraison;
    double latitude;
    double longitude;
};

#endif // COMMANDE_H