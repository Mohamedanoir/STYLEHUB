#include "connexion.h"
#include "maquette.h"

#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QDebug>

Connexion &Connexion::instance()
{
    static Connexion unique;
    return unique;
}

bool Connexion::creerConnexion(const QString &utilisateur, const QString &motDePasse)
{
    if (QSqlDatabase::contains() && QSqlDatabase::database().isOpen()) {
        return true;
    }

    QSqlDatabase db = QSqlDatabase::contains()
                          ? QSqlDatabase::database(QSqlDatabase::defaultConnection, false)
                          : QSqlDatabase::addDatabase("QODBC");

    // Nom exact présent dans votre administrateur ODBC
    db.setDatabaseName("Source_Oracle");

    // Définition des identifiants (SYSTEM ou utilisateur personnalisé)
    QString user = utilisateur.trimmed().isEmpty() ? "SYSTEM" : utilisateur.trimmed();
    QString pass = motDePasse.isEmpty() ? "admin" : motDePasse;

    db.setUserName(user);
    db.setPassword(pass);

    // Tentative de connexion
    if (!db.open()) {
        // En cas d'échec sur admin, essai direct avec admin123
        db.setPassword("admin123");
        if (!db.open()) {
            db.setUserName("fashionova");
            db.setPassword("fashionova123");
            if (!db.open()) {
                m_erreur = "Connexion Oracle impossible :\n" + db.lastError().text();
                qDebug() << m_erreur;
                return false;
            }
        }
    }

    qDebug() << "Connexion Oracle réussie avec :" << db.userName();

    // Vérification de la table MAQUETTES
    QSqlQuery q(db);
    if (!q.exec("SELECT ID_MAQUETTE, NOM_MAQUETTE, TYPE_MAQUETTE, DATE_DE_CREATION, STATUT, "
                "TAILLE, LARGEUR, LONGUEUR, DESCRIPTION, VIDEO, ID_EMPLOYE "
                "FROM MAQUETTES WHERE 1 = 0")) {
        qDebug() << "Avertissement table MAQUETTES :" << q.lastError().text();
    } else {
        qDebug() << "Table MAQUETTES accessible !";
    }

    // Gestion du compteur de vues
    QSqlQuery qVues(db);
    Maquette::setColonneVues(qVues.exec("SELECT NB_VUES FROM MAQUETTES WHERE 1 = 0"));

    // Vérification des colonnes de mesures
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
        }
    }
    Maquette::setColonnesMesures(presentes);

    return true;
}

void Connexion::fermerConnexion()
{
    QSqlDatabase db = QSqlDatabase::database(QSqlDatabase::defaultConnection, false);
    if (db.isOpen()) {
        db.close();
    }
}