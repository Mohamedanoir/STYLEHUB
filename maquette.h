#ifndef MAQUETTE_H
#define MAQUETTE_H

#include <QDate>
#include <QList>
#include <QMap>
#include <QPair>
#include <QSet>
#include <QString>
#include <QStringList>

class QSqlQuery;

// ============================================================================
//  Classe Maquette  <->  table Oracle FASHIONOVA.MAQUETTES
//
//  ID_MAQUETTE       NUMBER         NOT NULL   (clé primaire)
//  NOM_MAQUETTE      VARCHAR2(50)   NOT NULL
//  TYPE_MAQUETTE     VARCHAR2(30)
//  DATE_DE_CREATION  DATE           DEFAULT SYSDATE
//  STATUT            VARCHAR2(30)
//  TAILLE            VARCHAR2(10)
//  LARGEUR           NUMBER(6,2)
//  LONGUEUR          NUMBER(6,2)
//  TOUR_COU, TOUR_POITRINE, TOUR_TAILLE, TOUR_HANCHES, LARGEUR_EPAULES,
//  LONGUEUR_MANCHE, ENTREJAMBE, TOUR_TETE   NUMBER(6,2)
//                    mesures selon le type (ajoutées automatiquement si absentes)
//  DESCRIPTION       VARCHAR2(500)
//  VIDEO             VARCHAR2(255)
//  ID_EMPLOYE        NUMBER         NOT NULL   (clé étrangère -> EMPLOYES, non affichée)
//  NB_VUES           NUMBER         (optionnelle, pour le Top 5)
//
//  Seule la table MAQUETTES est modifiée par l'application.
//  ID_EMPLOYE n'apparaît pas dans l'interface : à l'ajout, l'application prend
//  automatiquement le premier employé de la table EMPLOYES (lecture seule) ;
//  à la modification, l'employé déjà enregistré n'est pas changé.
// ============================================================================
// Une mesure qui dépend du type (ex : Tour de taille pour une Jupe)
struct MesureType
{
    QString colonne;   // colonne Oracle      ex : "TOUR_TAILLE"
    QString libelle;   // texte du formulaire ex : "Tour de taille"
    QString article;   // pour les messages   ex : "le tour de taille"
    QString exemple;   // valeur d'exemple    ex : "72"
};

class Maquette
{
public:
    // Mesures en cm : valeurs spéciales pour le contrôle de saisie
    static constexpr double MESURE_VIDE     = -1;   // champ laissé vide (NULL dans Oracle)
    static constexpr double MESURE_INVALIDE = -2;   // texte qui n'est pas un nombre réel

    Maquette() = default;
    Maquette(const QString &id, const QString &nom, const QString &type,
             const QDate &date, const QString &statut,
             const QString &description = "", const QString &video = "", int vues = 0,
             const QString &taille = "", double largeur = MESURE_VIDE, double longueur = MESURE_VIDE,
             const QMap<QString, double> &mesures = {}, int idEmploye = 0);

    // Getters
    QString getId() const          { return m_id; }          // chiffres uniquement (ex : "6")
    QString getNom() const         { return m_nom; }
    QString getType() const        { return m_type; }
    QDate   getDateCreation() const{ return m_date; }
    QString getStatut() const      { return m_statut; }
    QString getDescription() const { return m_description; }
    QString getVideo() const       { return m_video; }
    int     getVues() const        { return m_vues; }
    QString getTaille() const      { return m_taille; }     // XS, S, M, L, XL
    double  getLargeur() const     { return m_largeur; }    // en cm
    double  getLongueur() const    { return m_longueur; }   // en cm
    // Mesure selon le type, ex : getMesure("TOUR_TAILLE") (MESURE_VIDE si non renseignée)
    double  getMesure(const QString &colonne) const { return m_mesures.value(colonne, MESURE_VIDE); }
    int     getIdEmploye() const   { return m_idEmploye; }

    // Setters
    void setVideo(const QString &v) { m_video = v; }

    // CRUD
    bool ajouter(QString &erreur) const;
    bool modifier(QString &erreur) const;
    static bool supprimer(const QString &id, QString *erreur = nullptr);
    static bool existe(const QString &id);
    static bool charger(const QString &id, Maquette &m);
    static int  prochainId();                     // MAX(ID_MAQUETTE) + 1

    // Afficher + Rechercher + Trier
    // critereTri : "date", "type", "statut" (vide = par ID)
    static QList<Maquette> afficher(const QString &recherche = "", const QString &critereTri = "");

    // Métier innovant 1 : vidéo de présentation
    static bool enregistrerVideo(const QString &id, const QString &chemin);

    // Métier innovant 2 : maquettes en retard de validation
    static QList<Maquette> enRetard(int joursMax);
    bool estEnRetard(int joursMax) const;

    // Statistiques
    static QMap<QString, int> statsParType();
    static QMap<QString, int> statsParStatut();
    static QList<QPair<QString, int>> statsParMois(int nbMois);
    static QList<QPair<QString, int>> top5Consultees();                // les 5 maquettes les plus consultées
    static void incrementerVues(const QString &id);

    // La colonne NB_VUES existe-t-elle dans MAQUETTES ? (détecté à la connexion)
    static void setColonneVues(bool existe) { s_colonneVues = existe; }
    static bool colonneVues()               { return s_colonneVues; }

    // ---- Attributs qui changent selon le type ----
    // Toutes les mesures possibles (dans l'ordre d'affichage du formulaire)
    static const QList<MesureType> &mesuresPossibles();
    // Mesures d'un type, ex : "Jupe" -> {"TOUR_TAILLE", "TOUR_HANCHES"} (Largeur et Longueur : tous les types)
    static QStringList mesuresDuType(const QString &type);
    static MesureType mesure(const QString &colonne);

    // Colonnes de mesures présentes dans MAQUETTES (détecté / ajouté à la connexion)
    static void setColonnesMesures(const QSet<QString> &colonnes) { s_colonnesMesures = colonnes; }
    static bool colonnePresente(const QString &colonne)          { return s_colonnesMesures.contains(colonne); }

    // Texte saisi -> mesure : "45.5", "45,5" ou "45.5 cm" -> 45.5 ;
    // vide -> MESURE_VIDE ; pas un nombre -> MESURE_INVALIDE
    static double lireMesure(const QString &texte);
    static QString texteMesure(double valeur);   // 45.5 -> "45.5" ; MESURE_VIDE -> ""

    // Valeurs autorisées (identiques aux listes déroulantes de mainwindow.ui,
    // où la 1re ligne « Aucun » veut dire « rien de choisi »)
    static QStringList types()   { return {"Robe", "Veste", "Jupe", "Pantalon", "T-shirt", "Chapeau", "Chemise homme"}; }
    static QStringList statuts() { return {"En cours", "Validée"}; }   // « En attente » n'est plus proposé
    static QStringList tailles() { return {"XS", "S", "M", "L", "XL"}; }

    // Contrôle de saisie
    QString controleSaisie() const;

private:
    void lierMesures(QSqlQuery &q) const;   // valeurs des mesures pour INSERT / UPDATE

    QString m_id, m_nom, m_type, m_statut, m_description, m_video, m_taille;
    QDate m_date;
    int m_vues = 0;
    double m_largeur = MESURE_VIDE, m_longueur = MESURE_VIDE;
    QMap<QString, double> m_mesures;   // colonne -> valeur (seulement les mesures du type)
    int m_idEmploye = 0;

    static bool s_colonneVues;
    static QSet<QString> s_colonnesMesures;
};

#endif // MAQUETTE_H
