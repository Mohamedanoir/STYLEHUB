#include "connexion.h"
#include "maquette.h"

#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QDebug>

Connexion &Connexion::instance()
{
    static Connexion unique;   // créée une seule fois
    return unique;
}

bool Connexion::creerConnexion()
{
    // Déjà ouverte ? (Singleton : on ne crée pas une deuxième connexion)
    if (QSqlDatabase::contains() && QSqlDatabase::database().isOpen())
        return true;

    QSqlDatabase db = QSqlDatabase::contains()
                          ? QSqlDatabase::database(QSqlDatabase::defaultConnection, false)
                          : QSqlDatabase::addDatabase("QODBC");
    db.setDatabaseName("Source_Projet2A");        // nom de la source ODBC (64 bits)
    db.setUserName("fashionova");
    db.setPassword(qEnvironmentVariable("FASHIONOVA_DB_PASSWORD"));

    if (!db.open()) {
        m_erreur = "Connexion Oracle impossible :\n" + db.lastError().text();
        qDebug() << m_erreur;
        return false;
    }
    qDebug() << "Connexion Oracle réussie !";

    // Vérifier que MAQUETTES a bien toutes les colonnes utilisées par l'application
    QSqlQuery q(db);
    if (!q.exec("SELECT ID_MAQUETTE, NOM_MAQUETTE, TYPE_MAQUETTE, DATE_DE_CREATION, STATUT, "
                "TAILLE, LARGEUR, LONGUEUR, DESCRIPTION, VIDEO, ID_EMPLOYE "
                "FROM MAQUETTES WHERE 1 = 0")) {
        m_erreur = "La table MAQUETTES est introuvable ou incomplète :\n" + q.lastError().text();
        qDebug() << m_erreur;
        return false;
    }
    qDebug() << "Table MAQUETTES accessible !";

    // Colonne optionnelle NB_VUES (Top 5 des maquettes les plus consultées)
    QSqlQuery qVues(db);
    Maquette::setColonneVues(qVues.exec("SELECT NB_VUES FROM MAQUETTES WHERE 1 = 0"));
    qDebug() << "Colonne NB_VUES :" << (Maquette::colonneVues() ? "présente" : "absente (compteur en mémoire)");

    // Colonnes des mesures qui dépendent du type (Tour de cou, Tour de poitrine, Tour de taille,
    // Tour de hanches, Largeur d'épaules, Longueur de manche, Entrejambe, Tour de tête) :
    // si une colonne n'existe pas, elle est ajoutée à MAQUETTES (colonne vide : aucune donnée
    // n'est modifiée, aucune autre table n'est touchée). Même chose que la partie 7 du script SQL.
    QSet<QString> presentes;
    for (const MesureType &m : Maquette::mesuresPossibles()) {
        QSqlQuery test(db);
        if (test.exec("SELECT " + m.colonne + " FROM MAQUETTES WHERE 1 = 0")) {
            presentes.insert(m.colonne);
            continue;
        }
        QSqlQuery ajout(db);
        if (ajout.exec("ALTER TABLE MAQUETTES ADD " + m.colonne + " NUMBER(6,2)")) {
            presentes.insert(m.colonne);
            qDebug() << "Colonne ajoutée à MAQUETTES :" << m.colonne;
        } else {
            qDebug() << "Impossible d'ajouter la colonne" << m.colonne << ":" << ajout.lastError().text();
        }
    }
    Maquette::setColonnesMesures(presentes);
    qDebug() << "Colonnes des mesures présentes :" << presentes.size() << "/" << Maquette::mesuresPossibles().size();

    return true;
}

void Connexion::fermerConnexion()
{
    QSqlDatabase db = QSqlDatabase::database(QSqlDatabase::defaultConnection, false);
    if (db.isOpen())
        db.close();
}
