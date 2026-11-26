#pragma once
#include <QString>
#include <QDate>
#include <QVector>

// Classe métier (Modèle) : gère les données ET les requêtes SQL de la table ARTICLES.
// Aucune requête SQL ne doit se trouver dans mainwindow.cpp.
class Article {
public:
    Article();
    Article(const QString &id, const QString &nom, const QString &cat, double prix, int qte,
            const QDate &date, const QString &st, const QString &desc, int dem, const QString &img);

    // Getters
    QString getId()   const { return id; }
    QString getNom()  const { return nom; }
    QString getCat()  const { return cat; }
    double  getPrix() const { return prix; }
    int     getQte()  const { return qte; }
    QDate   getDate() const { return date; }
    QString getSt()   const { return st; }
    QString getDesc() const { return desc; }
    int     getDem()  const { return dem; }
    QString getImg()  const { return img; }

    // Setters
    void setId(const QString &v)   { id = v; }
    void setNom(const QString &v)  { nom = v; }
    void setCat(const QString &v)  { cat = v; }
    void setPrix(double v)         { prix = v; }
    void setQte(int v)             { qte = v; }
    void setDate(const QDate &v)   { date = v; }
    void setSt(const QString &v)   { st = v; }
    void setDesc(const QString &v) { desc = v; }
    void setDem(int v)             { dem = v; }
    void setImg(const QString &v)  { img = v; }

    // CRUD
    bool ajouter();                              // Create
    static QVector<Article> afficher();          // Read
    bool modifier();                             // Update (selon l'id de l'objet)
    static bool supprimer(const QString &id);    // Delete
    static bool existe(const QString &id);       // vérifie l'unicité de l'ID

private:
    QString id, nom, cat;
    double  prix = 0;
    int     qte  = 0;
    QDate   date;
    QString st, desc;
    int     dem  = 0;
    QString img;
};
