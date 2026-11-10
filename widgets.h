#ifndef WIDGETS_H
#define WIDGETS_H

#include <QLabel>
#include <QPixmap>
#include <QWidget>

class QMovie;
class QToolButton;

// Bannière "Gestion des Maquettes" : l'image garde ses proportions
class Banniere : public QWidget
{
    Q_OBJECT
public:
    explicit Banniere(QWidget *parent = nullptr);
    void setImage(const QPixmap &image);
    bool hasHeightForWidth() const override { return true; }
    int heightForWidth(int w) const override;
    QSize sizeHint() const override;
protected:
    void paintEvent(QPaintEvent *) override;
    void resizeEvent(QResizeEvent *) override;
private:
    QPixmap m_image;
};

// Zone d'aperçu de la maquette : image fixe OU GIF animé, coins arrondis, remplissage
class ApercuMaquette : public QWidget
{
    Q_OBJECT
public:
    explicit ApercuMaquette(QWidget *parent = nullptr);
    void afficherImage(const QPixmap &pm);
    void afficherAnimation(const QString &cheminGif);
    void afficherVide(const QString &texte);
    QToolButton *boutonCrayon() const { return m_crayon; }
protected:
    void paintEvent(QPaintEvent *) override;
    void resizeEvent(QResizeEvent *) override;
private:
    void arreterAnimation();
    QPixmap      m_image;
    QMovie      *m_movie = nullptr;
    QString      m_texte;
    QToolButton *m_crayon = nullptr;
};

// Miniature cliquable (sous l'aperçu)
class Miniature : public QLabel
{
    Q_OBJECT
public:
    explicit Miniature(QWidget *parent = nullptr);
    void setImage(const QPixmap &pm);
    QPixmap image() const { return m_image; }
signals:
    void clique();
protected:
    void mousePressEvent(QMouseEvent *) override;
    void paintEvent(QPaintEvent *) override;
private:
    QPixmap m_image;
};

#endif // WIDGETS_H
