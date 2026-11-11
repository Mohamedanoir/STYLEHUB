#include "article.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

Article::Article() {}

Article::Article(const QString &id, const QString &nom, const QString &cat, double prix, int qte,
                 const QDate &date, const QString &st, const QString &desc, int dem, const QString &img)
    : id(id), nom(nom), cat(cat), prix(prix), qte(qte), date(date), st(st), desc(desc), dem(dem), img(img) {}

// ---------- CREATE ----------
bool Article::ajouter()
{
    QSqlQuery q;
    // These foreign-key columns are NOT NULL in the existing Oracle schema.
    // Use the first available maquette and machine as the default association.
    q.prepare("INSERT INTO ARTICLES (ID_Article, Nom_Article, Categorie, Prix, Quantite_Stock, "
              "Date_Ajout, Statut, Demande, Image, Description, ID_MAQUETTE, ID_MACHINE) "
              "VALUES (:id, :nom, :cat, :prix, :qte, :date, :st, :dem, :img, :desc, "
              "(SELECT MIN(ID_MAQUETTE) FROM MAQUETTES), "
              "(SELECT MIN(ID_MACHINE) FROM MACHINES))");
    q.bindValue(":id",   id);
    q.bindValue(":nom",  nom);
    q.bindValue(":cat",  cat);
    q.bindValue(":prix", prix);
    q.bindValue(":qte",  qte);
    q.bindValue(":date", date);
    q.bindValue(":st",   st);
    q.bindValue(":dem",  dem);
    q.bindValue(":img",  img);
    q.bindValue(":desc", desc);
    if (!q.exec()) { qDebug() << "ajouter :" << q.lastError().text(); return false; }
    return true;
}

// ---------- READ ----------
QVector<Article> Article::afficher()
{
    QVector<Article> list;
    QSqlQuery q;
    q.prepare("SELECT ID_Article, Nom_Article, Categorie, Prix, Quantite_Stock, Date_Ajout, "
              "Statut, Demande, Image, Description FROM ARTICLES ORDER BY ID_Article");
    if (!q.exec()) { qDebug() << "afficher :" << q.lastError().text(); return list; }
    while (q.next()) {
        list << Article(q.value(0).toString(), q.value(1).toString(), q.value(2).toString(),
                        q.value(3).toDouble(), q.value(4).toInt(), q.value(5).toDate(),
                        q.value(6).toString(), q.value(9).toString(), q.value(7).toInt(),
                        q.value(8).toString());
    }
    return list;
}

// ---------- UPDATE ----------
bool Article::modifier()
{
    QSqlQuery q;
    q.prepare("UPDATE ARTICLES SET Nom_Article=:nom, Categorie=:cat, Prix=:prix, Quantite_Stock=:qte, "
              "Date_Ajout=:date, Statut=:st, Demande=:dem, Image=:img, Description=:desc "
              "WHERE ID_Article=:id");
    q.bindValue(":nom",  nom);
    q.bindValue(":cat",  cat);
    q.bindValue(":prix", prix);
    q.bindValue(":qte",  qte);
    q.bindValue(":date", date);
    q.bindValue(":st",   st);
    q.bindValue(":dem",  dem);
    q.bindValue(":img",  img);
    q.bindValue(":desc", desc);
    q.bindValue(":id",   id);
    if (!q.exec()) { qDebug() << "modifier :" << q.lastError().text(); return false; }
    return q.numRowsAffected() > 0;
}

// ---------- DELETE ----------
bool Article::supprimer(const QString &id)
{
    QSqlQuery q;
    q.prepare("DELETE FROM ARTICLES WHERE ID_Article=:id");
    q.bindValue(":id", id);
    if (!q.exec()) { qDebug() << "supprimer :" << q.lastError().text(); return false; }
    return q.numRowsAffected() > 0;
}

bool Article::existe(const QString &id)
{
    QSqlQuery q;
    q.prepare("SELECT COUNT(*) FROM ARTICLES WHERE ID_Article=:id");
    q.bindValue(":id", id);
    return q.exec() && q.next() && q.value(0).toInt() > 0;
}
