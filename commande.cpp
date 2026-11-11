#include "commande.h"
#include <QSqlError>
#include <QDebug>
#include <QSqlQuery>

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
    modePaiement = "Carte bancaire";
    adresseLivraison = "";
    latitude = 36.8065;
    longitude = 10.1815;
}

Commande::Commande(QString id, QString codeCli, QString nomCli, QDate dateCmd,
                   QString statut, double montant, QString article,
                   QString taille, int quantite, QString modePaiement,
                   QString adresse, double lat, double lng)
    : idCommande(id), codeClient(codeCli), nomClient(nomCli), dateCommande(dateCmd),
    statut(statut), montant(montant), article(article), taille(taille),
    quantite(quantite), modePaiement(modePaiement), adresseLivraison(adresse),
    latitude(lat), longitude(lng) {}

// CREATE : Ajouter une commande conforme au schéma Oracle
bool Commande::ajouter() {
    QSqlQuery query;
    query.prepare("INSERT INTO COMMANDES (ID_COMMANDE, DATE_COMMANDE, DATE_LIVRAISON_ESTIMEE, "
                  "STATUT, MONTANT_TOTAL, ADRESSE_LIVRAISON, LATITUDE, LONGITUDE, "
                  "MODE_PAIEMENT, ID_CLIENT) "
                  "VALUES (:id, :dateCmd, :dateLiv, :statut, :montant, :adresse, :lat, :lng, :modeP, :idCli)");

    query.bindValue(":id", idCommande);
    query.bindValue(":dateCmd", dateCommande);
    query.bindValue(":dateLiv", dateCommande.addDays(2));
    query.bindValue(":statut", statut);
    query.bindValue(":montant", montant);
    query.bindValue(":adresse", adresseLivraison);
    query.bindValue(":lat", latitude);
    query.bindValue(":lng", longitude);
    query.bindValue(":modeP", modePaiement.isEmpty() ? "Carte bancaire" : modePaiement);
    query.bindValue(":idCli", codeClient);

    if (!query.exec()) {
        qDebug() << "Erreur ajout commande Oracle :" << query.lastError().text();
        return false;
    }
    return true;
}

// READ : Modèle tabulaire pour les exports ou vues directes
QSqlQueryModel* Commande::afficher() {
    QSqlQueryModel *model = new QSqlQueryModel();
    model->setQuery("SELECT c.ID_COMMANDE, c.ID_CLIENT, cl.NOM_CLIENT || ' ' || cl.PRENOM_CLIENT AS CLIENT, "
                    "c.ADRESSE_LIVRAISON, TO_CHAR(c.DATE_COMMANDE, 'DD/MM/YYYY') AS DATE_COMMANDE, "
                    "c.STATUT, c.MONTANT_TOTAL "
                    "FROM COMMANDES c JOIN CLIENTS cl ON c.ID_CLIENT = cl.ID_CLIENT "
                    "ORDER BY c.ID_COMMANDE ASC");

    model->setHeaderData(0, Qt::Horizontal, QObject::tr("ID"));
    model->setHeaderData(1, Qt::Horizontal, QObject::tr("Code Client"));
    model->setHeaderData(2, Qt::Horizontal, QObject::tr("Nom Client"));
    model->setHeaderData(3, Qt::Horizontal, QObject::tr("Adresse"));
    model->setHeaderData(4, Qt::Horizontal, QObject::tr("Date"));
    model->setHeaderData(5, Qt::Horizontal, QObject::tr("Statut"));
    model->setHeaderData(6, Qt::Horizontal, QObject::tr("Montant"));

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
    query.prepare("UPDATE COMMANDES SET ID_CLIENT = :idCli, "
                  "DATE_COMMANDE = :dateCmd, STATUT = :statut, MONTANT_TOTAL = :montant, "
                  "MODE_PAIEMENT = :modePaiement, ADRESSE_LIVRAISON = :adresse, "
                  "LATITUDE = :lat, LONGITUDE = :lng "
                  "WHERE ID_COMMANDE = :id");

    query.bindValue(":id", idCommande);
    query.bindValue(":idCli", codeClient);
    query.bindValue(":dateCmd", dateCommande);
    query.bindValue(":statut", statut);
    query.bindValue(":montant", montant);
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