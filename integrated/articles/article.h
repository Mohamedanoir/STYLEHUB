#ifndef ARTICLE_H
#define ARTICLE_H

#include <QDate>
#include <QString>

struct Article{QString id,nom,cat;double prix;int qte;QDate date;QString st,desc;int dem=0;QString img;};

#endif // ARTICLE_H
