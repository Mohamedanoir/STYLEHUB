#ifndef MAQUETTE_H
#define MAQUETTE_H

#include <QDate>
#include <QList>
#include <QMap>
#include <QString>
#include <QStringList>

class Maquette
{
public:
    Maquette() = default;
    Maquette(const QString &id, const QString &nom, const QString &type,
             const QDate &date, const QString &statut,
             const QString &description = "", const QString &video = "", int vues = 0,
             const QString &taille = "M", double largeur = 0, double longueur = 0);

    // Getters
    QString getId() const          { return m_id; }
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

    // Setters
    void setVideo(const QString &v) { m_video = v; }

    // CRUD
    bool ajouter(QString &erreur) const;
    bool modifier(QString &erreur) const;
    static bool supprimer(const QString &id);
    static bool existe(const QString &id);
    static bool charger(const QString &id, Maquette &m);

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
    static QList<QPair<QString, int>> statsParMois(int nbMois);        // cumul à la fin de chaque mois
    static QList<QPair<QString, int>> top5Consultees();                // les 5 maquettes les plus consultées
    static void incrementerVues(const QString &id);

    // Tailles autorisées
    static QStringList tailles() { return {"XS", "S", "M", "L", "XL"}; }

    // Contrôle de saisie
    QString controleSaisie() const;

private:
    QString m_id, m_nom, m_type, m_statut, m_description, m_video, m_taille;
    QDate m_date;
    int m_vues = 0;
    double m_largeur = 0, m_longueur = 0;
};

#endif // MAQUETTE_H
