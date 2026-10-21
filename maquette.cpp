
#include "maquette.h"

#include <QLocale>
#include <QRegularExpression>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>
#include <QDebug>

Maquette::Maquette(const QString &id, const QString &nom, const QString &type,
                   const QDate &date, const QString &statut,
                   const QString &description, const QString &video, int vues,
                   const QString &taille, double largeur, double longueur)
    : m_id(id.trimmed().toUpper()),
    m_nom(nom.trimmed()),
    m_type(type),
    m_statut(statut),
    m_description(description.trimmed()),
    m_video(video),
    m_taille(taille.trimmed().toUpper()),
    m_date(date),
    m_vues(vues),
    m_largeur(largeur),
    m_longueur(longueur)
{
}

QString Maquette::controleSaisie() const
{
    if (!QRegularExpression("^MQ\\d{3,}$").match(m_id).hasMatch())
        return "L'ID doit avoir la forme MQ suivi de chiffres (ex : MQ006).";

    if (m_nom.length() < 3)
        return "Le nom doit contenir au moins 3 caractères.";

    if (m_type != "Robe" && m_type != "Veste" && m_type != "Jupe")
        return "Le type doit être Robe, Veste ou Jupe.";

    if (!m_date.isValid() || m_date > QDate::currentDate())
        return "La date de création ne peut pas être dans le futur.";

    if (m_statut.isEmpty())
        return "Veuillez choisir un statut.";

    if (!tailles().contains(m_taille))
        return "La taille doit être XS, S, M, L ou XL.";

    if (m_largeur <= 0 || m_largeur > 300)
        return "La largeur doit être comprise entre 0 et 300 cm.";

    if (m_longueur <= 0 || m_longueur > 300)
        return "La longueur doit être comprise entre 0 et 300 cm.";

    return "";
}

// AJOUTER
bool Maquette::ajouter(QString &erreur) const
{
    qDebug() << "Maquette::ajouter appelée";
    erreur = controleSaisie();

    if (!erreur.isEmpty()) {
        qDebug() << "Erreur validation :" << erreur;
        return false;
    }

    QSqlQuery q;

    q.prepare(
        "INSERT INTO MAQUETTES "
        "(ID_MAQUETTE, NOM_MAQUETTE, TYPE_MAQUETTE, "
        "DATE_CREATION, STATUT, DESCRIPTION, VIDEO, "
        "NB_VUES, TAILLE, LARGEUR, LONGUEUR) "
        "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)"
        );

    q.addBindValue(m_id);
    q.addBindValue(m_nom);
    q.addBindValue(m_type);
    q.addBindValue(m_date.toString("yyyy-MM-dd"));
    q.addBindValue(m_statut);
    q.addBindValue(m_description);
    q.addBindValue(m_video);
    q.addBindValue(0);
    q.addBindValue(m_taille);
    q.addBindValue(m_largeur);
    q.addBindValue(m_longueur);

    if (!q.exec()) {
        erreur = q.lastError().text();
        qDebug() << "Erreur Oracle AJOUTER :" << erreur;
        return false;
    }

    qDebug() << "Maquette ajoutée :" << m_id;
    return true;
}
// MODIFIER
bool Maquette::modifier(QString &erreur) const
{
    erreur = controleSaisie();

    if (!erreur.isEmpty())
        return false;

    QSqlQuery q;

    q.prepare(
        "UPDATE MAQUETTES SET "
        "NOM_MAQUETTE=?, TYPE_MAQUETTE=?, DATE_CREATION=?, "
        "STATUT=?, DESCRIPTION=?, VIDEO=?, TAILLE=?, "
        "LARGEUR=?, LONGUEUR=? "
        "WHERE ID_MAQUETTE=?"
        );

    q.addBindValue(m_nom);
    q.addBindValue(m_type);
    q.addBindValue(m_date.toString("yyyy-MM-dd"));
    q.addBindValue(m_statut);
    q.addBindValue(m_description);
    q.addBindValue(m_video);
    q.addBindValue(m_taille);
    q.addBindValue(m_largeur);
    q.addBindValue(m_longueur);
    q.addBindValue(m_id);

    if (!q.exec()) {
        erreur = q.lastError().text();
        qDebug() << "Erreur Oracle MODIFIER :" << erreur;
        return false;
    }

    if (q.numRowsAffected() == 0) {
        erreur = "Aucune maquette trouvée avec l'ID " + m_id;
        return false;
    }

    return true;
}

// SUPPRIMER
bool Maquette::supprimer(const QString &id)
{
    QSqlQuery q;

    q.prepare("DELETE FROM MAQUETTES WHERE ID_MAQUETTE=?");
    q.addBindValue(id.trimmed().toUpper());

    if (!q.exec()) {
        qDebug() << "Erreur Oracle SUPPRIMER :" << q.lastError().text();
        return false;
    }

    return q.numRowsAffected() > 0;
}

// VERIFIER EXISTENCE
bool Maquette::existe(const QString &id)
{
    QSqlQuery q;

    q.prepare("SELECT 1 FROM MAQUETTES WHERE ID_MAQUETTE=?");
    q.addBindValue(id.trimmed().toUpper());

    if (!q.exec()) {
        qDebug() << "Erreur Oracle EXISTE :" << q.lastError().text();
        return false;
    }

    return q.next();
}

// CONVERSION RESULTAT SQL -> MAQUETTE
static Maquette depuisRequete(const QSqlQuery &q)
{
    return Maquette(
        q.value(0).toString(),
        q.value(1).toString(),
        q.value(2).toString(),
        QDate::fromString(q.value(3).toString(), "yyyy-MM-dd"),
        q.value(4).toString(),
        q.value(5).toString(),
        q.value(6).toString(),
        q.value(7).toInt(),
        q.value(8).toString(),
        q.value(9).toDouble(),
        q.value(10).toDouble()
        );
}

static const char *COLONNES =
    "SELECT ID_MAQUETTE, NOM_MAQUETTE, TYPE_MAQUETTE, "
    "DATE_CREATION, STATUT, DESCRIPTION, VIDEO, NB_VUES, "
    "TAILLE, LARGEUR, LONGUEUR FROM MAQUETTES ";

// CHARGER
bool Maquette::charger(const QString &id, Maquette &m)
{
    QSqlQuery q;

    q.prepare(QString(COLONNES) + "WHERE ID_MAQUETTE=?");
    q.addBindValue(id.trimmed().toUpper());

    if (!q.exec()) {
        qDebug() << "Erreur Oracle CHARGER :" << q.lastError().text();
        return false;
    }

    if (q.next()) {
        m = depuisRequete(q);
        return true;
    }

    return false;
}

// AFFICHER + RECHERCHE + TRI
QList<Maquette> Maquette::afficher(const QString &recherche,
                                   const QString &critereTri)
{
    QString sql = COLONNES;
    const bool filtrer = !recherche.trimmed().isEmpty();

    if (filtrer) {
        sql +=
            "WHERE ID_MAQUETTE LIKE ? "
            "OR NOM_MAQUETTE LIKE ? "
            "OR TYPE_MAQUETTE LIKE ? ";
    }

    if (critereTri == "date")
        sql += "ORDER BY DATE_CREATION DESC";
    else if (critereTri == "type")
        sql += "ORDER BY TYPE_MAQUETTE, ID_MAQUETTE";
    else if (critereTri == "statut")
        sql += "ORDER BY STATUT, ID_MAQUETTE";
    else
        sql += "ORDER BY ID_MAQUETTE";

    QSqlQuery q;
    q.prepare(sql);

    if (filtrer) {
        QString valeur = "%" + recherche.trimmed() + "%";
        q.addBindValue(valeur);
        q.addBindValue(valeur);
        q.addBindValue(valeur);
    }

    QList<Maquette> liste;

    if (!q.exec()) {
        qDebug() << "Erreur Oracle AFFICHER :" << q.lastError().text();
        return liste;
    }

    while (q.next())
        liste.append(depuisRequete(q));

    return liste;
}

// ENREGISTRER VIDEO
bool Maquette::enregistrerVideo(const QString &id,
                                const QString &chemin)
{
    QSqlQuery q;

    q.prepare(
        "UPDATE MAQUETTES SET VIDEO=? WHERE ID_MAQUETTE=?"
        );

    q.addBindValue(chemin);
    q.addBindValue(id.trimmed().toUpper());

    if (!q.exec()) {
        qDebug() << "Erreur Oracle VIDEO :" << q.lastError().text();
        return false;
    }

    return q.numRowsAffected() > 0;
}

// ALERTE RETARD
bool Maquette::estEnRetard(int joursMax) const
{
    return !m_statut.startsWith("Valid")
    && m_date.daysTo(QDate::currentDate()) > joursMax;
}

QList<Maquette> Maquette::enRetard(int joursMax)
{
    QList<Maquette> res;

    for (const Maquette &m : afficher("", "date")) {
        if (m.estEnRetard(joursMax))
            res.append(m);
    }

    return res;
}

// STATISTIQUES PAR TYPE
QMap<QString, int> Maquette::statsParType()
{
    QMap<QString, int> res;

    QSqlQuery q(
        "SELECT TYPE_MAQUETTE, COUNT(*) "
        "FROM MAQUETTES GROUP BY TYPE_MAQUETTE"
        );

    if (!q.isActive()) {
        qDebug() << "Erreur statistiques type :" << q.lastError().text();
        return res;
    }

    while (q.next())
        res[q.value(0).toString()] = q.value(1).toInt();

    return res;
}

// STATISTIQUES PAR STATUT
QMap<QString, int> Maquette::statsParStatut()
{
    QMap<QString, int> res;

    QSqlQuery q(
        "SELECT STATUT, COUNT(*) "
        "FROM MAQUETTES GROUP BY STATUT"
        );

    if (!q.isActive()) {
        qDebug() << "Erreur statistiques statut :" << q.lastError().text();
        return res;
    }

    while (q.next())
        res[q.value(0).toString()] = q.value(1).toInt();

    return res;
}

// STATISTIQUES PAR MOIS
QList<QPair<QString, int>> Maquette::statsParMois(int nbMois)
{
    QList<QPair<QString, int>> res;

    QLocale fr(QLocale::French);

    QDate debutMoisCourant(
        QDate::currentDate().year(),
        QDate::currentDate().month(),
        1
        );

    for (int i = nbMois - 1; i >= 0; --i) {
        QDate debut = debutMoisCourant.addMonths(-i);
        QDate fin = debut.addMonths(1);

        QSqlQuery q;

        q.prepare(
            "SELECT COUNT(*) FROM MAQUETTES "
            "WHERE DATE_CREATION >= ? "
            "AND DATE_CREATION < ?"
            );

        q.addBindValue(debut.toString("yyyy-MM-dd"));
        q.addBindValue(fin.toString("yyyy-MM-dd"));

        int n = 0;

        if (q.exec() && q.next())
            n = q.value(0).toInt();
        else
            qDebug() << "Erreur statistiques mois :" << q.lastError().text();

        QString mois = fr.toString(debut, "MMM");

        if (!mois.isEmpty())
            mois[0] = mois[0].toUpper();

        res.append({mois, n});
    }

    return res;
}

// TOP 5 MAQUETTES LES PLUS CONSULTEES
// Compatible Oracle 11g et versions suivantes
QList<QPair<QString, int>> Maquette::top5Consultees()
{
    QList<QPair<QString, int>> res;

    QSqlQuery q(
        "SELECT NOM_MAQUETTE, NB_VUES FROM "
        "(SELECT NOM_MAQUETTE, NB_VUES "
        "FROM MAQUETTES "
        "ORDER BY NB_VUES DESC, ID_MAQUETTE) "
        "WHERE ROWNUM <= 5"
        );

    if (!q.isActive()) {
        qDebug() << "Erreur Oracle TOP 5 :" << q.lastError().text();
        return res;
    }

    while (q.next()) {
        res.append({
            q.value(0).toString(),
            q.value(1).toInt()
        });
    }

    return res;
}

// INCREMENTER NOMBRE DE VUES
void Maquette::incrementerVues(const QString &id)
{
    QSqlQuery q;

    q.prepare(
        "UPDATE MAQUETTES "
        "SET NB_VUES = NVL(NB_VUES, 0) + 1 "
        "WHERE ID_MAQUETTE = ?"
        );

    q.addBindValue(id.trimmed().toUpper());

    if (!q.exec())
        qDebug() << "Erreur Oracle VUES :" << q.lastError().text();
}
