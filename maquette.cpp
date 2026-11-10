#include "maquette.h"

#include <QLocale>
#include <QRegularExpression>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QVariant>
#include <QDebug>

#include <algorithm>

bool Maquette::s_colonneVues = false;
QSet<QString> Maquette::s_colonnesMesures;

// Compteur de consultations en mémoire, utilisé seulement si la colonne
// NB_VUES n'existe pas dans MAQUETTES (voir maquettes_oracle.sql).
static QMap<QString, int> s_vuesSession;

// ============================================================================
//  Outils
// ============================================================================

// "6" -> 6 ; renvoie false si ce n'est pas un entier positif
static bool idNumerique(const QString &id, int &valeur)
{
    bool ok = false;
    valeur = id.trimmed().toInt(&ok);
    return ok && valeur > 0;
}

// NUMBER Oracle -> double (accepte 12.5 ou "12,5" selon les réglages NLS)
static double lireNombre(const QVariant &v)
{
    bool ok = false;
    double d = v.toDouble(&ok);
    if (!ok) d = v.toString().replace(',', '.').toDouble(&ok);
    return ok ? d : 0.0;
}

static int lireEntier(const QVariant &v)
{
    return qRound(lireNombre(v));
}

// Mesure Oracle (NUMBER ou NULL) -> double (MESURE_VIDE si NULL)
static double lireMesureSql(const QVariant &v)
{
    return v.isNull() ? Maquette::MESURE_VIDE : lireNombre(v);
}

// Mesure -> valeur à envoyer à Oracle (NULL si vide)
static QVariant mesureSql(double v)
{
    return v < 0 ? QVariant(QMetaType(QMetaType::Double)) : QVariant(v);
}

static int employeParDefaut();   // défini plus bas (section EMPLOYE PAR DEFAUT)

// ============================================================================
//  ATTRIBUTS SELON LE TYPE
//  Largeur et Longueur : pour tous les types.
//  Les autres mesures dépendent du type choisi (le formulaire n'affiche que celles-ci).
// ============================================================================
const QList<MesureType> &Maquette::mesuresPossibles()
{
    static const QList<MesureType> liste = {
        {"TOUR_COU",        "Tour de cou",        "le tour de cou",        "40"},
        {"TOUR_POITRINE",   "Tour de poitrine",   "le tour de poitrine",   "92"},
        {"TOUR_TAILLE",     "Tour de taille",     "le tour de taille",     "72"},
        {"TOUR_HANCHES",    "Tour de hanches",    "le tour de hanches",    "98"},
        {"LARGEUR_EPAULES", "Largeur d'épaules",  "la largeur d'épaules",  "45"},
        {"LONGUEUR_MANCHE", "Longueur de manche", "la longueur de manche", "62"},
        {"ENTREJAMBE",      "Entrejambe",         "l'entrejambe",          "80"},
        {"TOUR_TETE",       "Tour de tête",       "le tour de tête",       "57"},
    };
    return liste;
}

QStringList Maquette::mesuresDuType(const QString &type)
{
    static const QMap<QString, QStringList> parType = {
        {"Robe",          {"TOUR_POITRINE", "TOUR_TAILLE", "TOUR_HANCHES"}},
        {"Veste",         {"TOUR_POITRINE", "TOUR_TAILLE", "LARGEUR_EPAULES", "LONGUEUR_MANCHE"}},
        {"Jupe",          {"TOUR_TAILLE", "TOUR_HANCHES"}},
        {"Pantalon",      {"TOUR_TAILLE", "TOUR_HANCHES", "ENTREJAMBE"}},
        {"T-shirt",       {"TOUR_POITRINE", "LARGEUR_EPAULES", "LONGUEUR_MANCHE"}},
        {"Chapeau",       {"TOUR_TETE"}},
        {"Chemise homme", {"TOUR_COU", "TOUR_POITRINE", "LARGEUR_EPAULES", "LONGUEUR_MANCHE"}},
    };
    return parType.value(type.trimmed());   // type inconnu ou « Aucun » : aucune mesure en plus
}

MesureType Maquette::mesure(const QString &colonne)
{
    for (const MesureType &m : mesuresPossibles())
        if (m.colonne == colonne) return m;
    return {colonne, colonne, colonne, "50"};
}

// Message clair à partir des erreurs Oracle les plus fréquentes
static QString messageOracle(const QSqlError &e)
{
    const QString t = e.text();
    if (t.contains("ORA-00001")) return "Cet ID existe déjà dans la table MAQUETTES.";
    if (t.contains("ORA-02291")) return "L'employé (ID_EMPLOYE) n'existe pas dans la table EMPLOYES.";
    if (t.contains("ORA-02292")) return "Cette maquette est utilisée par un autre module (ex : commandes) : "
                                        "Oracle refuse la suppression.";
    if (t.contains("ORA-01400")) return "Un champ obligatoire est vide (Nom ou ID_EMPLOYE).";
    if (t.contains("ORA-02290")) return "Oracle refuse cette valeur : une contrainte CHECK de la table MAQUETTES "
                                        "limite les valeurs permises (ex : TYPE_MAQUETTE ou STATUT).\n"
                                        "Voir la partie 6 de maquettes_oracle.sql.\n" + t;
    if (t.contains("ORA-12899")) return "Une valeur est trop longue pour la colonne Oracle.\n" + t;
    if (t.contains("ORA-00904")) return "Colonne introuvable dans MAQUETTES.\n" + t;
    if (t.contains("ORA-00942")) return "Table introuvable (vérifiez l'utilisateur de la source ODBC).\n" + t;
    return t;
}

// Colonnes lues dans l'ordre utilisé par depuisRequete()
static QString colonnesSelect()
{
    return QString(
        "SELECT ID_MAQUETTE, NOM_MAQUETTE, TYPE_MAQUETTE, "
        "TO_CHAR(DATE_DE_CREATION, 'YYYY-MM-DD'), STATUT, DESCRIPTION, VIDEO, %1, "
        "TAILLE, LARGEUR, LONGUEUR, ID_EMPLOYE%2 FROM MAQUETTES ")
        .arg(Maquette::colonneVues() ? "NVL(NB_VUES, 0)" : "0",
             [] {   // mesures selon le type (colonne absente -> NULL)
                 QString cols;
                 for (const MesureType &m : Maquette::mesuresPossibles())
                     cols += ", " + (Maquette::colonnePresente(m.colonne) ? m.colonne : QString("NULL"));
                 return cols;
             }());
}

// Ligne SQL -> objet Maquette
static Maquette depuisRequete(const QSqlQuery &q)
{
    const QString id = QString::number(lireEntier(q.value(0)));
    int vues = lireEntier(q.value(7));
    if (!Maquette::colonneVues()) vues = s_vuesSession.value(id);

    QMap<QString, double> mesures;                 // colonnes 12, 13, ... (ordre de mesuresPossibles())
    int col = 12;
    for (const MesureType &m : Maquette::mesuresPossibles()) {
        const double v = lireMesureSql(q.value(col++));
        if (v >= 0) mesures[m.colonne] = v;
    }

    return Maquette(
        id,
        q.value(1).toString(),
        q.value(2).toString(),
        QDate::fromString(q.value(3).toString(), "yyyy-MM-dd"),
        q.value(4).toString(),
        q.value(5).toString(),
        q.value(6).toString(),
        vues,
        q.value(8).toString(),
        lireMesureSql(q.value(9)),
        lireMesureSql(q.value(10)),
        mesures,
        lireEntier(q.value(11)));
}

// ============================================================================
//  Constructeur + contrôle de saisie
// ============================================================================
Maquette::Maquette(const QString &id, const QString &nom, const QString &type,
                   const QDate &date, const QString &statut,
                   const QString &description, const QString &video, int vues,
                   const QString &taille, double largeur, double longueur,
                   const QMap<QString, double> &mesures, int idEmploye)
    : m_id(id.trimmed()),
    m_nom(nom.trimmed()),
    m_type(type.trimmed()),
    m_statut(statut.trimmed()),
    m_description(description.trimmed()),
    m_video(video.trimmed()),
    m_taille(taille.trimmed().toUpper()),
    m_date(date),
    m_vues(vues),
    m_largeur(largeur),
    m_longueur(longueur),
    m_mesures(mesures),
    m_idEmploye(idEmploye)
{
}

// ============================================================================
//  Mesures en cm (texte <-> nombre réel)
// ============================================================================
double Maquette::lireMesure(const QString &texte)
{
    QString t = texte.trimmed();
    if (t.endsWith("cm", Qt::CaseInsensitive)) t.chop(2);
    t = t.trimmed().replace(',', '.');
    if (t.isEmpty())
        return MESURE_VIDE;

    // nombre réel : chiffres, avec éventuellement une partie décimale (ex : 45, 45.5, -3)
    if (!QRegularExpression("^-?\\d+(\\.\\d+)?$").match(t).hasMatch())
        return MESURE_INVALIDE;

    const double v = t.toDouble();
    return v <= 0 ? 0 : v;          // 0 ou négatif : refusé par le contrôle de saisie
}

QString Maquette::texteMesure(double valeur)
{
    return valeur < 0 ? QString() : QString::number(valeur);
}

// Contrôle d'une mesure obligatoire (nombre réel en cm, entre 0 et 300)
//   article : "la largeur" (message champ vide) ; libelle : "Largeur" ; exemple : "45.5"
static QString controleMesure(double v, const QString &article, const QString &libelle, const QString &exemple)
{
    if (v == Maquette::MESURE_VIDE)
        return "Veuillez saisir " + article + " : un nombre réel (float) en cm, ex : " + exemple;
    if (v == Maquette::MESURE_INVALIDE)
        return libelle + " : valeur incorrecte ! Saisissez un nombre réel (float), pas du texte. Ex : " + exemple + " cm";
    if (v <= 0)
        return libelle + " : la valeur doit être un nombre réel supérieur à 0, ex : " + exemple;
    if (v > 300)
        return libelle + " : la valeur ne doit pas dépasser 300 cm.";
    return QString();
}

QString Maquette::controleSaisie() const
{
    int n = 0;
    if (m_id.isEmpty())
        return "L'ID est vide : cliquez sur le champ ID pour obtenir l'ID automatiquement.";

    if (!QRegularExpression("^\\d{1,9}$").match(m_id).hasMatch() || !idNumerique(m_id, n))
        return "L'ID doit être un nombre entier positif (ex : 6).";

    if (m_nom.isEmpty())
        return "Veuillez saisir le nom de la maquette.";

    if (m_nom.length() < 3)
        return "Le nom doit contenir au moins 3 caractères.";

    if (m_nom.toUtf8().size() > 50)
        return "Le nom ne doit pas dépasser 50 caractères.";

    // Le nom ne doit contenir que des lettres (majuscules, minuscules, accents) et des espaces — pas de chiffres
    if (!QRegularExpression("^[\\p{L} ]+$").match(m_nom).hasMatch())
        return "Le nom ne doit contenir que des lettres (pas de chiffres ni de caractères spéciaux).";

    if (m_type.isEmpty())
        return "Veuillez choisir un type.";

    if (!types().contains(m_type))
        return "Le type doit être : " + types().join(", ") + ".";

    if (!m_date.isValid())
        return "Veuillez choisir la date de création.";

    if (m_date > QDate::currentDate())
        return "La date de création ne peut pas être dans le futur.";

    if (m_statut.isEmpty())
        return "Veuillez choisir un statut (En cours ou Validée).";

    if (!statuts().contains(m_statut))
        return "Le statut doit être En cours ou Validée.";

    if (m_taille.isEmpty())
        return "Veuillez choisir une taille.";

    if (!tailles().contains(m_taille))
        return "La taille doit être XS, S, M, L ou XL.";

    // Largeur et Longueur : obligatoires pour tous les types
    QString e = controleMesure(m_largeur, "la largeur", "Largeur", "45.5");
    if (e.isEmpty()) e = controleMesure(m_longueur, "la longueur", "Longueur", "45.5");
    if (!e.isEmpty())
        return e;

    // Mesures du type choisi (ex : Jupe -> Tour de taille, Tour de hanches) : obligatoires
    for (const QString &colonne : mesuresDuType(m_type)) {
        if (!colonnePresente(colonne)) continue;          // colonne absente dans Oracle : champ désactivé
        const MesureType m = mesure(colonne);
        e = controleMesure(getMesure(colonne), m.article, m.libelle, m.exemple);
        if (!e.isEmpty())
            return e;
    }

    if (m_description.toUtf8().size() > 500)
        return "La description ne doit pas dépasser 500 caractères.";

    // La description ne doit contenir que des lettres, espaces et ponctuation — pas de chiffres
    if (!m_description.isEmpty() &&
        QRegularExpression("\\d").match(m_description).hasMatch())
        return "La description ne doit pas contenir de chiffres (lettres uniquement).";

    if (m_video.toUtf8().size() > 255)
        return "Le chemin de la vidéo est trop long (255 caractères maximum).";

    return "";
}

// Valeurs des mesures pour INSERT / UPDATE (dans l'ordre de mesuresPossibles()).
// Une mesure qui n'appartient pas au type choisi est enregistrée vide (NULL) :
// ex : une Robe changée en Jupe perd son Tour de poitrine.
void Maquette::lierMesures(QSqlQuery &q) const
{
    const QStringList duType = mesuresDuType(m_type);
    for (const MesureType &m : mesuresPossibles()) {
        if (!colonnePresente(m.colonne)) continue;
        q.addBindValue(mesureSql(duType.contains(m.colonne) ? getMesure(m.colonne) : MESURE_VIDE));
    }
}

// ============================================================================
//  AJOUTER
// ============================================================================
bool Maquette::ajouter(QString &erreur) const
{
    erreur = controleSaisie();
    if (!erreur.isEmpty())
        return false;

    if (existe(m_id)) {
        erreur = "L'ID " + m_id + " existe déjà. Cliquez sur le champ ID pour obtenir automatiquement un nouvel ID.";
        return false;
    }

    // ID_EMPLOYE (clé étrangère, obligatoire) n'est pas saisi dans l'interface
    const int idEmploye = m_idEmploye > 0 ? m_idEmploye : employeParDefaut();
    if (idEmploye <= 0) {
        erreur = "Aucun employé dans la table EMPLOYES.\n"
                 "Ajoutez au moins un employé dans Oracle (puis COMMIT), car ID_EMPLOYE est obligatoire.";
        return false;
    }

    // Mesures selon le type : colonnes présentes dans Oracle
    QString colsMesures, pointsMesures;
    for (const MesureType &m : mesuresPossibles()) {
        if (!colonnePresente(m.colonne)) continue;
        colsMesures += ", " + m.colonne;
        pointsMesures += ", ?";
    }

    QSqlQuery q;
    q.prepare(QString(
        "INSERT INTO MAQUETTES "
        "(ID_MAQUETTE, NOM_MAQUETTE, TYPE_MAQUETTE, DATE_DE_CREATION, STATUT, "
        "TAILLE, LARGEUR, LONGUEUR, DESCRIPTION, VIDEO, ID_EMPLOYE%1%3) "
        "VALUES (?, ?, ?, TO_DATE(?, 'YYYY-MM-DD'), ?, ?, ?, ?, ?, ?, ?%2%4)")
        .arg(colonneVues() ? ", NB_VUES" : "", colonneVues() ? ", 0" : "", colsMesures, pointsMesures));

    q.addBindValue(m_id.toInt());
    q.addBindValue(m_nom);
    q.addBindValue(m_type);
    q.addBindValue(m_date.toString("yyyy-MM-dd"));
    q.addBindValue(m_statut);
    q.addBindValue(m_taille);
    q.addBindValue(m_largeur);
    q.addBindValue(m_longueur);
    q.addBindValue(m_description);
    q.addBindValue(m_video);
    q.addBindValue(idEmploye);
    lierMesures(q);

    if (!q.exec()) {
        erreur = messageOracle(q.lastError());
        qDebug() << "Erreur Oracle AJOUTER :" << q.lastError().text();
        return false;
    }

    qDebug() << "Maquette ajoutée :" << m_id;
    return true;
}

// ============================================================================
//  MODIFIER
// ============================================================================
bool Maquette::modifier(QString &erreur) const
{
    erreur = controleSaisie();
    if (!erreur.isEmpty())
        return false;

    QSqlQuery q;
    q.prepare(QString(
        "UPDATE MAQUETTES SET "
        "NOM_MAQUETTE = ?, TYPE_MAQUETTE = ?, DATE_DE_CREATION = TO_DATE(?, 'YYYY-MM-DD'), "
        "STATUT = ?, TAILLE = ?, LARGEUR = ?, LONGUEUR = ?, "
        "DESCRIPTION = ?, VIDEO = ?%1 "        // ID_EMPLOYE reste inchangé
        "WHERE ID_MAQUETTE = ?")
        .arg([] {
            QString cols;
            for (const MesureType &m : mesuresPossibles())
                if (colonnePresente(m.colonne)) cols += ", " + m.colonne + " = ?";
            return cols;
        }()));

    q.addBindValue(m_nom);
    q.addBindValue(m_type);
    q.addBindValue(m_date.toString("yyyy-MM-dd"));
    q.addBindValue(m_statut);
    q.addBindValue(m_taille);
    q.addBindValue(m_largeur);
    q.addBindValue(m_longueur);
    q.addBindValue(m_description);
    q.addBindValue(m_video);
    lierMesures(q);
    q.addBindValue(m_id.toInt());

    if (!q.exec()) {
        erreur = messageOracle(q.lastError());
        qDebug() << "Erreur Oracle MODIFIER :" << q.lastError().text();
        return false;
    }

    if (q.numRowsAffected() == 0) {
        erreur = "Aucune maquette trouvée avec l'ID " + m_id;
        return false;
    }

    return true;
}

// ============================================================================
//  SUPPRIMER
// ============================================================================
bool Maquette::supprimer(const QString &id, QString *erreur)
{
    int n = 0;
    if (!idNumerique(id, n)) {
        if (erreur) *erreur = "ID invalide : " + id;
        return false;
    }

    QSqlQuery q;
    q.prepare("DELETE FROM MAQUETTES WHERE ID_MAQUETTE = ?");
    q.addBindValue(n);

    if (!q.exec()) {
        if (erreur) *erreur = messageOracle(q.lastError());
        qDebug() << "Erreur Oracle SUPPRIMER :" << q.lastError().text();
        return false;
    }

    if (q.numRowsAffected() <= 0) {
        if (erreur) *erreur = "Aucune maquette trouvée avec l'ID " + id;
        return false;
    }

    s_vuesSession.remove(QString::number(n));
    return true;
}

// ============================================================================
//  EXISTE / CHARGER / PROCHAIN ID
// ============================================================================
bool Maquette::existe(const QString &id)
{
    int n = 0;
    if (!idNumerique(id, n))
        return false;

    QSqlQuery q;
    q.setForwardOnly(true);
    q.prepare("SELECT 1 FROM MAQUETTES WHERE ID_MAQUETTE = ?");
    q.addBindValue(n);

    if (!q.exec()) {
        qDebug() << "Erreur Oracle EXISTE :" << q.lastError().text();
        return false;
    }

    return q.next();
}

bool Maquette::charger(const QString &id, Maquette &m)
{
    int n = 0;
    if (!idNumerique(id, n))
        return false;

    QSqlQuery q;
    q.setForwardOnly(true);
    q.prepare(colonnesSelect() + "WHERE ID_MAQUETTE = ?");
    q.addBindValue(n);

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

int Maquette::prochainId()
{
    QSqlQuery q;
    q.setForwardOnly(true);
    if (q.exec("SELECT NVL(MAX(ID_MAQUETTE), 0) + 1 FROM MAQUETTES") && q.next())
        return qMax(1, lireEntier(q.value(0)));

    qDebug() << "Erreur Oracle PROCHAIN ID :" << q.lastError().text();
    return 1;
}

// ============================================================================
//  AFFICHER + RECHERCHE + TRI
// ============================================================================
QList<Maquette> Maquette::afficher(const QString &recherche, const QString &critereTri)
{
    QString sql = colonnesSelect();
    const bool filtrer = !recherche.trimmed().isEmpty();

    if (filtrer) {
        sql +=
            "WHERE TO_CHAR(ID_MAQUETTE) LIKE ? "
            "OR UPPER(NOM_MAQUETTE) LIKE ? "
            "OR UPPER(TYPE_MAQUETTE) LIKE ? ";
    }

    if (critereTri == "date")
        sql += "ORDER BY DATE_DE_CREATION DESC NULLS LAST, ID_MAQUETTE";
    else if (critereTri == "type")
        sql += "ORDER BY TYPE_MAQUETTE, ID_MAQUETTE";
    else if (critereTri == "statut")
        sql += "ORDER BY STATUT, ID_MAQUETTE";
    else
        sql += "ORDER BY ID_MAQUETTE";

    QSqlQuery q;
    q.setForwardOnly(true);
    q.prepare(sql);

    if (filtrer) {
        const QString valeur = "%" + recherche.trimmed().toUpper() + "%";
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

// ============================================================================
//  EMPLOYE PAR DEFAUT (lecture seule de EMPLOYES) : premier ID_EMPLOYE
// ============================================================================
static int employeParDefaut()
{
    QSqlQuery q;
    q.setForwardOnly(true);
    if (q.exec("SELECT MIN(ID_EMPLOYE) FROM EMPLOYES") && q.next() && !q.value(0).isNull())
        return lireEntier(q.value(0));
    qDebug() << "Lecture EMPLOYES :" << q.lastError().text();

    // Table EMPLOYES non accessible : employé déjà utilisé par une autre maquette
    QSqlQuery q2;
    q2.setForwardOnly(true);
    if (q2.exec("SELECT MIN(ID_EMPLOYE) FROM MAQUETTES") && q2.next() && !q2.value(0).isNull())
        return lireEntier(q2.value(0));

    return 0;
}

// ============================================================================
//  METIER 1 : VIDEO
// ============================================================================
bool Maquette::enregistrerVideo(const QString &id, const QString &chemin)
{
    int n = 0;
    if (!idNumerique(id, n) || chemin.toUtf8().size() > 255)
        return false;

    QSqlQuery q;
    q.prepare("UPDATE MAQUETTES SET VIDEO = ? WHERE ID_MAQUETTE = ?");
    q.addBindValue(chemin);
    q.addBindValue(n);

    if (!q.exec()) {
        qDebug() << "Erreur Oracle VIDEO :" << q.lastError().text();
        return false;
    }

    return q.numRowsAffected() > 0;
}

// ============================================================================
//  METIER 2 : ALERTE RETARD
// ============================================================================
bool Maquette::estEnRetard(int joursMax) const
{
    return m_date.isValid()
           && !m_statut.startsWith("Valid")
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

// ============================================================================
//  STATISTIQUES
// ============================================================================
QMap<QString, int> Maquette::statsParType()
{
    QMap<QString, int> res;

    QSqlQuery q;
    q.setForwardOnly(true);
    if (!q.exec("SELECT TYPE_MAQUETTE, COUNT(*) FROM MAQUETTES GROUP BY TYPE_MAQUETTE")) {
        qDebug() << "Erreur statistiques type :" << q.lastError().text();
        return res;
    }

    while (q.next())
        res[q.value(0).toString()] = lireEntier(q.value(1));

    return res;
}

QMap<QString, int> Maquette::statsParStatut()
{
    QMap<QString, int> res;

    QSqlQuery q;
    q.setForwardOnly(true);
    if (!q.exec("SELECT STATUT, COUNT(*) FROM MAQUETTES GROUP BY STATUT")) {
        qDebug() << "Erreur statistiques statut :" << q.lastError().text();
        return res;
    }

    while (q.next())
        res[q.value(0).toString()] = lireEntier(q.value(1));

    return res;
}

// Nombre de maquettes créées chaque mois (les nbMois derniers mois)
QList<QPair<QString, int>> Maquette::statsParMois(int nbMois)
{
    QList<QPair<QString, int>> res;
    QLocale fr(QLocale::French);

    const QDate debutMoisCourant(QDate::currentDate().year(), QDate::currentDate().month(), 1);

    for (int i = nbMois - 1; i >= 0; --i) {
        const QDate debut = debutMoisCourant.addMonths(-i);
        const QDate fin = debut.addMonths(1);

        QSqlQuery q;
        q.setForwardOnly(true);
        q.prepare(
            "SELECT COUNT(*) FROM MAQUETTES "
            "WHERE DATE_DE_CREATION >= TO_DATE(?, 'YYYY-MM-DD') "
            "AND DATE_DE_CREATION < TO_DATE(?, 'YYYY-MM-DD')");
        q.addBindValue(debut.toString("yyyy-MM-dd"));
        q.addBindValue(fin.toString("yyyy-MM-dd"));

        int n = 0;
        if (q.exec() && q.next())
            n = lireEntier(q.value(0));
        else
            qDebug() << "Erreur statistiques mois :" << q.lastError().text();

        QString mois = fr.toString(debut, "MMM");
        if (!mois.isEmpty())
            mois[0] = mois[0].toUpper();

        res.append({mois, n});
    }

    return res;
}

// Top 5 des maquettes les plus consultées (compatible Oracle 11g : ROWNUM)
QList<QPair<QString, int>> Maquette::top5Consultees()
{
    QList<QPair<QString, int>> res;

    if (colonneVues()) {
        QSqlQuery q;
        q.setForwardOnly(true);
        if (!q.exec("SELECT NOM_MAQUETTE, VUES FROM "
                    "(SELECT NOM_MAQUETTE, NVL(NB_VUES, 0) AS VUES FROM MAQUETTES "
                    " ORDER BY NVL(NB_VUES, 0) DESC, ID_MAQUETTE) "
                    "WHERE ROWNUM <= 5")) {
            qDebug() << "Erreur Oracle TOP 5 :" << q.lastError().text();
            return res;
        }
        while (q.next())
            res.append({q.value(0).toString(), lireEntier(q.value(1))});
        return res;
    }

    // Sans colonne NB_VUES : consultations comptées pendant la session
    QList<Maquette> toutes = afficher();
    std::stable_sort(toutes.begin(), toutes.end(),
                     [](const Maquette &a, const Maquette &b) { return a.getVues() > b.getVues(); });
    for (int i = 0; i < toutes.size() && i < 5; ++i)
        res.append({toutes[i].getNom(), toutes[i].getVues()});
    return res;
}

void Maquette::incrementerVues(const QString &id)
{
    int n = 0;
    if (!idNumerique(id, n))
        return;

    if (!colonneVues()) {
        s_vuesSession[QString::number(n)]++;
        return;
    }

    QSqlQuery q;
    q.prepare("UPDATE MAQUETTES SET NB_VUES = NVL(NB_VUES, 0) + 1 WHERE ID_MAQUETTE = ?");
    q.addBindValue(n);

    if (!q.exec())
        qDebug() << "Erreur Oracle VUES :" << q.lastError().text();
}
