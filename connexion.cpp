#include "connexion.h"

#include <QCoreApplication>
#include <QDate>
#include <QDir>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>

bool Connexion::creerConnexion()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(QDir(QCoreApplication::applicationDirPath()).filePath("maquettes_fashionova.db"));

    if (!db.open()) {
        m_erreur = db.lastError().text();
        return false;
    }

    QSqlQuery q;
    q.exec("CREATE TABLE IF NOT EXISTS MAQUETTES ("
           " ID_MAQUETTE   TEXT PRIMARY KEY,"
           " NOM_MAQUETTE  TEXT NOT NULL,"
           " TYPE_MAQUETTE TEXT NOT NULL,"
           " DATE_CREATION TEXT NOT NULL,"
           " STATUT        TEXT NOT NULL,"
           " DESCRIPTION   TEXT,"
           " VIDEO         TEXT,"
           " NB_VUES       INTEGER DEFAULT 0)");

    // Données de démonstration (seulement si la table est vide)
    q.exec("SELECT COUNT(*) FROM MAQUETTES");
    if (q.next() && q.value(0).toInt() == 0) {
        struct Ligne { const char *id, *nom, *type, *date, *statut, *desc; int vues; };
        const Ligne lignes[] = {
            {"MQ001", "Robe Élégance",    "Robe",     "2026-09-26", "En cours",   "Robe de soirée avec détails en dentelle et coupe asymétrique.", 12},
            {"MQ002", "Veste Luxe",       "Veste",    "2026-09-22", "Validée",    "Veste cintrée un bouton, revers crantés.", 8},
            {"MQ003", "Jupe Satin",       "Jupe",     "2026-09-20", "En attente", "Jupe évasée en satin avec volant drapé.", 6},
            {"MQ004", "Robe Dentelle",    "Robe",     "2026-09-18", "En cours",   "Robe midi en dentelle à manches longues.", 5},
            {"MQ005", "Veste Tailleur",   "Veste",    "2026-09-15", "Validée",    "Veste de tailleur structurée.", 4},
            {"MQ006", "Robe Bustier",     "Robe",     "2026-10-02", "En cours",   "Robe longue bustier en satin.", 3},
            {"MQ007", "Jupe Plissée",     "Jupe",     "2026-10-01", "En cours",   "Jupe midi plissée.", 2},
            {"MQ008", "Robe Cocktail",    "Robe",     "2026-09-12", "Validée",    "Robe courte de cocktail.", 3},
            {"MQ009", "Veste Croisée",    "Veste",    "2026-09-10", "Validée",    "Veste croisée double boutonnage.", 2},
            {"MQ010", "Robe Bohème",      "Robe",     "2026-09-08", "Validée",    "Robe longue fleurie.", 2},
            {"MQ011", "Jupe Soie",        "Jupe",     "2026-09-05", "Validée",    "Jupe longue en soie.", 1},
            {"MQ012", "Veste Cintrée",    "Veste",    "2026-09-03", "Validée",    "Veste cintrée à basque.", 1},
            {"MQ013", "Robe Drapée",      "Robe",     "2026-08-27", "Validée",    "Robe drapée asymétrique.", 2},
            {"MQ014", "Veste Courte",     "Veste",    "2026-08-20", "Validée",    "Veste courte style boléro.", 1},
            {"MQ015", "Jupe Crayon",      "Jupe",     "2026-08-14", "Validée",    "Jupe crayon taille haute.", 1},
            {"MQ016", "Robe Brodée",      "Robe",     "2026-08-06", "Validée",    "Robe brodée main.", 0},
            {"MQ017", "Robe Sirène",      "Robe",     "2026-07-25", "Validée",    "Robe sirène de gala.", 2},
            {"MQ018", "Jupe Fluide",      "Jupe",     "2026-07-10", "Validée",    "Jupe fluide en mousseline.", 0},
            {"MQ019", "Robe Empire",      "Robe",     "2026-06-28", "Validée",    "Robe taille empire.", 1},
            {"MQ020", "Veste Smoking",    "Veste",    "2026-06-15", "Validée",    "Veste smoking à col châle.", 1},
            {"MQ021", "Jupe Portefeuille","Jupe",     "2026-06-04", "Validée",    "Jupe portefeuille nouée.", 0},
            {"MQ022", "Robe Chemise",     "Robe",     "2026-05-26", "Validée",    "Robe chemise ceinturée.", 0},
            {"MQ023", "Jupe Volants",     "Jupe",     "2026-05-18", "Validée",    "Jupe à volants superposés.", 0},
            {"MQ024", "Veste Cargo",      "Veste",    "2026-05-09", "Validée",    "Veste cargo chic.", 0},
            {"MQ025", "Veste Saharienne", "Veste",    "2026-05-03", "Validée",    "Veste saharienne en lin.", 0},
        };
        QSqlQuery ins;
        ins.prepare("INSERT INTO MAQUETTES (ID_MAQUETTE, NOM_MAQUETTE, TYPE_MAQUETTE, DATE_CREATION, STATUT, DESCRIPTION, VIDEO, NB_VUES) "
                    "VALUES (?, ?, ?, ?, ?, ?, '', ?)");
        for (const Ligne &l : lignes) {
            ins.addBindValue(QString::fromUtf8(l.id));
            ins.addBindValue(QString::fromUtf8(l.nom));
            ins.addBindValue(QString::fromUtf8(l.type));
            ins.addBindValue(QString::fromUtf8(l.date));
            ins.addBindValue(QString::fromUtf8(l.statut));
            ins.addBindValue(QString::fromUtf8(l.desc));
            ins.addBindValue(l.vues);
            ins.exec();
        }
    }
    return true;
}
