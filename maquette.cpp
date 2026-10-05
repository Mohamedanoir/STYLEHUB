#include "maquette.h"

#include <QLocale>
#include <QRegularExpression>
#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>

Maquette::Maquette(const QString &id, const QString &nom, const QString &type,
                   const QDate &date, const QString &statut,
                   const QString &description, const QString &video, int vues)
    : m_id(id.trimmed().toUpper()), m_nom(nom.trimmed()), m_type(type), m_statut(statut),
      m_description(description.trimmed()), m_video(video), m_date(date), m_vues(vues)
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
    return "";
}

bool Maquette::ajouter(QString &erreur) const
{
    erreur = controleSaisie();
    if (!erreur.isEmpty()) return false;
    if (existe(m_id)) { erreur = "Une maquette avec l'ID " + m_id + " existe déjà."; return false; }

    QSqlQuery q;
    q.prepare("INSERT INTO MAQUETTES (ID_MAQUETTE, NOM_MAQUETTE, TYPE_MAQUETTE, DATE_CREATION, STATUT, DESCRIPTION, VIDEO) "
              "VALUES (:id, :nom, :type, :date, :statut, :desc, :video)");
    q.bindValue(":id", m_id);
    q.bindValue(":nom", m_nom);
    q.bindValue(":type", m_type);
    q.bindValue(":date", m_date.toString("yyyy-MM-dd"));
    q.bindValue(":statut", m_statut);
    q.bindValue(":desc", m_description);
    q.bindValue(":video", m_video);
    if (!q.exec()) { erreur = q.lastError().text(); return false; }
    return true;
}

bool Maquette::modifier(QString &erreur) const
{
    erreur = controleSaisie();
    if (!erreur.isEmpty()) return false;
    if (!existe(m_id)) { erreur = "Aucune maquette avec l'ID " + m_id + "."; return false; }

    QSqlQuery q;
    q.prepare("UPDATE MAQUETTES SET NOM_MAQUETTE=:nom, TYPE_MAQUETTE=:type, DATE_CREATION=:date, "
              "STATUT=:statut, DESCRIPTION=:desc WHERE ID_MAQUETTE=:id");
    q.bindValue(":id", m_id);
    q.bindValue(":nom", m_nom);
    q.bindValue(":type", m_type);
    q.bindValue(":date", m_date.toString("yyyy-MM-dd"));
    q.bindValue(":statut", m_statut);
    q.bindValue(":desc", m_description);
    if (!q.exec()) { erreur = q.lastError().text(); return false; }
    return true;
}

bool Maquette::supprimer(const QString &id)
{
    QSqlQuery q;
    q.prepare("DELETE FROM MAQUETTES WHERE ID_MAQUETTE=:id");
    q.bindValue(":id", id);
    return q.exec() && q.numRowsAffected() > 0;
}

bool Maquette::existe(const QString &id)
{
    QSqlQuery q;
    q.prepare("SELECT 1 FROM MAQUETTES WHERE ID_MAQUETTE=:id");
    q.bindValue(":id", id);
    return q.exec() && q.next();
}

static Maquette depuisRequete(const QSqlQuery &q)
{
    return Maquette(q.value(0).toString(), q.value(1).toString(), q.value(2).toString(),
                    QDate::fromString(q.value(3).toString(), "yyyy-MM-dd"),
                    q.value(4).toString(), q.value(5).toString(), q.value(6).toString(),
                    q.value(7).toInt());
}

static const char *COLONNES =
    "SELECT ID_MAQUETTE, NOM_MAQUETTE, TYPE_MAQUETTE, DATE_CREATION, STATUT, DESCRIPTION, VIDEO, NB_VUES FROM MAQUETTES ";

bool Maquette::charger(const QString &id, Maquette &m)
{
    QSqlQuery q;
    q.prepare(QString(COLONNES) + "WHERE ID_MAQUETTE=:id");
    q.bindValue(":id", id);
    if (q.exec() && q.next()) { m = depuisRequete(q); return true; }
    return false;
}

QList<Maquette> Maquette::afficher(const QString &recherche, const QString &critereTri)
{
    QString sql = COLONNES;
    if (!recherche.trimmed().isEmpty())
        sql += "WHERE ID_MAQUETTE LIKE :r OR NOM_MAQUETTE LIKE :r OR TYPE_MAQUETTE LIKE :r ";

    if (critereTri == "date")        sql += "ORDER BY DATE_CREATION DESC";
    else if (critereTri == "type")   sql += "ORDER BY TYPE_MAQUETTE, ID_MAQUETTE";
    else if (critereTri == "statut") sql += "ORDER BY STATUT, ID_MAQUETTE";
    else                             sql += "ORDER BY ID_MAQUETTE";

    QSqlQuery q;
    q.prepare(sql);
    if (!recherche.trimmed().isEmpty())
        q.bindValue(":r", "%" + recherche.trimmed() + "%");

    QList<Maquette> liste;
    if (q.exec())
        while (q.next()) liste.append(depuisRequete(q));
    return liste;
}

bool Maquette::enregistrerVideo(const QString &id, const QString &chemin)
{
    QSqlQuery q;
    q.prepare("UPDATE MAQUETTES SET VIDEO=:v WHERE ID_MAQUETTE=:id");
    q.bindValue(":v", chemin);
    q.bindValue(":id", id);
    return q.exec();
}

bool Maquette::estEnRetard(int joursMax) const
{
    return !m_statut.startsWith("Valid") && m_date.daysTo(QDate::currentDate()) > joursMax;
}

QList<Maquette> Maquette::enRetard(int joursMax)
{
    QList<Maquette> res;
    for (const Maquette &m : afficher("", "date"))
        if (m.estEnRetard(joursMax)) res.append(m);
    return res;
}

QMap<QString, int> Maquette::statsParType()
{
    QMap<QString, int> res;
    QSqlQuery q("SELECT TYPE_MAQUETTE, COUNT(*) FROM MAQUETTES GROUP BY TYPE_MAQUETTE");
    while (q.next()) res[q.value(0).toString()] = q.value(1).toInt();
    return res;
}

QMap<QString, int> Maquette::statsParStatut()
{
    QMap<QString, int> res;
    QSqlQuery q("SELECT STATUT, COUNT(*) FROM MAQUETTES GROUP BY STATUT");
    while (q.next()) res[q.value(0).toString()] = q.value(1).toInt();
    return res;
}

QList<QPair<QString, int>> Maquette::statsParMois(int nbMois)
{
    QList<QPair<QString, int>> res;
    QLocale fr(QLocale::French);
    QDate debutMoisCourant(QDate::currentDate().year(), QDate::currentDate().month(), 1);

    for (int i = nbMois - 1; i >= 0; --i) {
        QDate debut = debutMoisCourant.addMonths(-i);
        QDate fin = debut.addMonths(1);
        QSqlQuery q;
        q.prepare("SELECT COUNT(*) FROM MAQUETTES WHERE DATE_CREATION < :f");
        q.bindValue(":f", fin.toString("yyyy-MM-dd"));
        int n = (q.exec() && q.next()) ? q.value(0).toInt() : 0;
        QString mois = fr.toString(debut, "MMM");
        mois[0] = mois[0].toUpper();
        res.append({mois, n});
    }
    return res;
}

QList<QPair<QString, int>> Maquette::top5Consultees()
{
    QList<QPair<QString, int>> res;
    QSqlQuery q("SELECT NOM_MAQUETTE, NB_VUES FROM MAQUETTES ORDER BY NB_VUES DESC, ID_MAQUETTE LIMIT 5");
    while (q.next()) res.append({q.value(0).toString(), q.value(1).toInt()});
    return res;
}

void Maquette::incrementerVues(const QString &id)
{
    QSqlQuery q;
    q.prepare("UPDATE MAQUETTES SET NB_VUES = COALESCE(NB_VUES, 0) + 1 WHERE ID_MAQUETTE = :id");
    q.bindValue(":id", id);
    q.exec();
}
