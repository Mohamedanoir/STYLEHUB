#ifndef SMARTSIDEBAR_H
#define SMARTSIDEBAR_H

#include <QColor>
#include <QFrame>
#include <QPainter>
#include <QPixmap>

// Menu latéral : dessine l'image de fond (logo en haut, mannequin en bas) sans la rogner.
// Widget « promu » dans smartmarket.ui (classe SmartSidebar, base QFrame).
class SmartSidebar : public QFrame
{
    Q_OBJECT
public:
    explicit SmartSidebar(QWidget *parent = nullptr) : QFrame(parent) {}

protected:
    void paintEvent(QPaintEvent *) override
    {
        QPainter p(this);
        const QPixmap bg(":/images/sidebar.png");
        if (!bg.isNull()) p.drawPixmap(rect(), bg);
        else p.fillRect(rect(), QColor("#082849"));
    }
};

#endif // SMARTSIDEBAR_H
