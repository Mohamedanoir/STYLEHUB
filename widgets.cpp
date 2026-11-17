#include "widgets.h"
#include "icones.h"

#include <QMovie>
#include <QPainter>
#include <QPainterPath>
#include <QToolButton>

// Dessine "pm" pour remplir "zone" (comme background-size: cover)
static void dessinerCouvrant(QPainter &p, const QRectF &zone, const QPixmap &pm)
{
    if (pm.isNull()) return;
    QSizeF s = pm.size();
    s.scale(zone.size(), Qt::KeepAspectRatioByExpanding);
    QRectF cible(zone.center().x() - s.width() / 2, zone.center().y() - s.height() / 2, s.width(), s.height());
    p.drawPixmap(cible, pm, QRectF(QPointF(0, 0), pm.size()));
}

// ============================================================ Banniere
Banniere::Banniere(QWidget *parent) : QWidget(parent)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    setFixedHeight(135);
}

void Banniere::setImage(const QPixmap &image)
{
    m_image = image;
    setFixedHeight(heightForWidth(qMax(width(), 1000)));
    update();
}

int Banniere::heightForWidth(int w) const
{
    if (m_image.isNull()) return 135;
    return qMax(110, w * m_image.height() / m_image.width());
}

QSize Banniere::sizeHint() const { return QSize(1250, 135); }

void Banniere::resizeEvent(QResizeEvent *)
{
    setFixedHeight(heightForWidth(width()));
}

void Banniere::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::SmoothPixmapTransform);
    dessinerCouvrant(p, rect(), m_image);
}

// ============================================================ ApercuMaquette
ApercuMaquette::ApercuMaquette(QWidget *parent) : QWidget(parent)
{
    m_crayon = new QToolButton(this);
    m_crayon->setIcon(Icones::icone("pencil", Qt::white, 16));
    m_crayon->setIconSize(QSize(15, 15));
    m_crayon->setFixedSize(26, 26);
    m_crayon->setCursor(Qt::PointingHandCursor);
    m_crayon->setStyleSheet("QToolButton { background:#0B2A45; border:none; border-radius:6px; }"
                            "QToolButton:hover { background:#163D63; }");
}

void ApercuMaquette::arreterAnimation()
{
    if (m_movie) {
        m_movie->stop();
        m_movie->deleteLater();
        m_movie = nullptr;
    }
}

void ApercuMaquette::afficherImage(const QPixmap &pm)
{
    arreterAnimation();
    m_image = pm;
    m_texte.clear();
    update();
}

void ApercuMaquette::afficherAnimation(const QString &cheminGif)
{
    arreterAnimation();
    m_movie = new QMovie(cheminGif, QByteArray(), this);
    if (!m_movie->isValid()) {
        arreterAnimation();
        afficherVide("GIF introuvable");
        return;
    }
    m_texte.clear();
    connect(m_movie, &QMovie::frameChanged, this, [this]() {
        if (m_movie) { m_image = m_movie->currentPixmap(); update(); }
    });
    if (cheminGif.contains("veste_validee"))
        m_movie->setSpeed(40);   // veste : 40% de la vitesse (plus lent). 100 = normal
    m_movie->start();
}

void ApercuMaquette::afficherVide(const QString &texte)
{
    arreterAnimation();
    m_image = QPixmap();
    m_texte = texte;
    update();
}

void ApercuMaquette::resizeEvent(QResizeEvent *)
{
    m_crayon->move(width() - m_crayon->width() - 8, 8);
}

void ApercuMaquette::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    p.setRenderHint(QPainter::SmoothPixmapTransform);

    QPainterPath arrondi;
    arrondi.addRoundedRect(QRectF(rect()).adjusted(0.5, 0.5, -0.5, -0.5), 8, 8);
    p.fillPath(arrondi, QColor("#EADFD1"));
    p.setClipPath(arrondi);

    if (!m_image.isNull()) {
        dessinerCouvrant(p, rect(), m_image);
    } else {
        p.drawPixmap(QRect(width() / 2 - 24, height() / 2 - 40, 48, 48),
                     Icones::pixmap("mannequin", QColor("#B9A58A"), 48));
        p.setPen(QColor("#8C7B66"));
        p.drawText(QRect(8, height() / 2 + 14, width() - 16, 40), Qt::AlignHCenter | Qt::TextWordWrap, m_texte);
    }
}

// ============================================================ Miniature
Miniature::Miniature(QWidget *parent) : QLabel(parent)
{
    setCursor(Qt::PointingHandCursor);
}

void Miniature::setImage(const QPixmap &pm)
{
    m_image = pm;
    update();
}

void Miniature::mousePressEvent(QMouseEvent *)
{
    emit clique();
}

void Miniature::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    p.setRenderHint(QPainter::SmoothPixmapTransform);
    QPainterPath arrondi;
    arrondi.addRoundedRect(QRectF(rect()).adjusted(0.5, 0.5, -0.5, -0.5), 5, 5);
    p.fillPath(arrondi, QColor("#EADFD1"));
    p.setClipPath(arrondi);
    dessinerCouvrant(p, rect(), m_image);
}
