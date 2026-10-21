#include "commande.h"
#include <QSqlError>
#include <QDebug>

Commande::Commande() {
    idCommande = "";
    codeClient = "";
    nomClient = "";
    dateCommande = QDate::currentDate();
    statut = "En cours";
    montant = 0.0;
    article = "";
    taille = "";
    quantite = 1;
    modePaiement = "";
    adresseLivraison = "";
    latitude = 0.0;
    longitude = 0.0;
}

Commande::Commande(QString id, QString codeCli, QString nomCli, QDate dateCmd,
                   QString statut, double montant, QString article,
                   QString taille, int quantite, QString modePaiement,
                   QString adresse, double lat, double lng)
    : idCommande(id), codeClient(codeCli), nomClient(nomCli), dateCommande(dateCmd),
    statut(statut), montant(montant), article(article), taille(taille),
    quantite(quantite), modePaiement(modePaiement), adresseLivraison(adresse),
    latitude(lat), longitude(lng) {}

// CREATE : Ajouter une commande
bool Commande::ajouter() {
    QSqlQuery query;
    query.prepare("INSERT INTO COMMANDES (ID_COMMANDE, CODE_CLIENT, NOM_CLIENT, DATE_COMMANDE, "
                  "STATUT, MONTANT, ARTICLE, TAILLE, QUANTITE, MODE_PAIEMENT, ADRESSE_LIVRAISON, "
                  "LATITUDE, LONGITUDE) "
                  "VALUES (:id, :codeCli, :nomCli, :dateCmd, :statut, :montant, :article, "
                  ":taille, :quantite, :modePaiement, :adresse, :lat, :lng)");

    query.bindValue(":id", idCommande);
    query.bindValue(":codeCli", codeClient);
    query.bindValue(":nomCli", nomClient);
    query.bindValue(":dateCmd", dateCommande);
    query.bindValue(":statut", statut);
    query.bindValue(":montant", montant);
    query.bindValue(":article", article);
    query.bindValue(":taille", taille);
    query.bindValue(":quantite", quantite);
    query.bindValue(":modePaiement", modePaiement);
    query.bindValue(":adresse", adresseLivraison);
    query.bindValue(":lat", latitude);
    query.bindValue(":lng", longitude);

    if (!query.exec()) {
        qDebug() << "Erreur ajout commande :" << query.lastError().text();
        return false;
    }
    return true;
}

// READ : Afficher toutes les commandes dans un modèle tabulaire
QSqlQueryModel* Commande::afficher() {
    QSqlQueryModel *model = new QSqlQueryModel();
    model->setQuery("SELECT ID_COMMANDE, CODE_CLIENT, NOM_CLIENT, "
                    "TO_CHAR(DATE_COMMANDE, 'DD/MM/YYYY') AS DATE_COMMANDE, "
                    "STATUT, MONTANT FROM COMMANDES ORDER BY ID_COMMANDE ASC");

    model->setHeaderData(0, Qt::Horizontal, QObject::tr("ID Commande"));
    model->setHeaderData(1, Qt::Horizontal, QObject::tr("Code Client"));
    model->setHeaderData(2, Qt::Horizontal, QObject::tr("Nom Client"));
    model->setHeaderData(3, Qt::Horizontal, QObject::tr("Date"));
    model->setHeaderData(4, Qt::Horizontal, QObject::tr("Statut"));
    model->setHeaderData(5, Qt::Horizontal, QObject::tr("Montant (TND)"));

    return model;
}

// DELETE : Supprimer une commande par son ID
bool Commande::supprimer(QString id) {
    QSqlQuery query;
    query.prepare("DELETE FROM COMMANDES WHERE ID_COMMANDE = :id");
    query.bindValue(":id", id);

    if (!query.exec()) {
        qDebug() << "Erreur suppression commande :" << query.lastError().text();
        return false;
    }
    return true;
}

// UPDATE : Mettre à jour une commande existante
bool Commande::modifier() {
    QSqlQuery query;
    query.prepare("UPDATE COMMANDES SET CODE_CLIENT = :codeCli, NOM_CLIENT = :nomCli, "
                  "DATE_COMMANDE = :dateCmd, STATUT = :statut, MONTANT = :montant, "
                  "ARTICLE = :article, TAILLE = :taille, QUANTITE = :quantite, "
                  "MODE_PAIEMENT = :modePaiement, ADRESSE_LIVRAISON = :adresse, "
                  "LATITUDE = :lat, LONGITUDE = :lng "
                  "WHERE ID_COMMANDE = :id");

    query.bindValue(":id", idCommande);
    query.bindValue(":codeCli", codeClient);
    query.bindValue(":nomCli", nomClient);
    query.bindValue(":dateCmd", dateCommande);
    query.bindValue(":statut", statut);
    query.bindValue(":montant", montant);
    query.bindValue(":article", article);
    query.bindValue(":taille", taille);
    query.bindValue(":quantite", quantite);
    query.bindValue(":modePaiement", modePaiement);
    query.bindValue(":adresse", adresseLivraison);
    query.bindValue(":lat", latitude);
    query.bindValue(":lng", longitude);

    if (!query.exec()) {
        qDebug() << "Erreur modification commande :" << query.lastError().text();
        return false;
    }
    return true;
}