#pragma once
#include <QWidget>
#include <QDate>
#include <QString>
#include <QVector>

QT_BEGIN_NAMESPACE
namespace Ui { class ArticlesPage; }
QT_END_NAMESPACE

struct Article {
    QString id, nom, cat;
    double prix = 0;
    int qte = 0;
    QDate date;
    QString st, desc;
    int dem = 0;
    QString img;
};

// Module « Articles » (ex-application Fashionova_Articles) sous forme de page
// intégrable dans le QStackedWidget du HighFashionWorkshop.
class ArticlesPage : public QWidget
{
    Q_OBJECT
public:
    explicit ArticlesPage(QWidget *parent = nullptr);
    ~ArticlesPage() override;

    void setSearchText(const QString &text);   // recherche globale de la barre du haut

signals:
    void statusMessage(const QString &text, int ms);

private slots:
    void onAdd();
    void onMod();
    void onDel();
    void onView();
    void reset();
    void refresh();
    void exportPdf();
    void onRow();
    void onImg();

private:
    void setImg(const QString &p);
    bool readForm(Article &a);
    void fill(const Article &a);
    void stats();

    Ui::ArticlesPage *ui;
    QString img;
    QVector<Article> A;
    QVector<int> view;
    int cur = -1;
};
