#include "icones.h"

#include <QPainter>
#include <QPainterPath>
#include <QtMath>

namespace
{
QPainterPath personne(qreal cx, qreal cy, qreal rTete, qreal largeur, qreal bas)
{
    QPainterPath p;
    p.addEllipse(QPointF(cx, cy), rTete, rTete);
    const qreal haut = cy + rTete + 1.2;
    QPainterPath corps;
    corps.moveTo(cx - largeur / 2, bas);
    corps.cubicTo(cx - largeur / 2, haut + 1, cx - largeur / 4, haut, cx, haut);
    corps.cubicTo(cx + largeur / 4, haut, cx + largeur / 2, haut + 1, cx + largeur / 2, bas);
    corps.closeSubpath();
    p.addPath(corps);
    return p;
}

void dessiner(QPainter &p, const QString &nom, const QColor &c)
{
    QPen trait(c, 2.0, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    p.setPen(trait);
    p.setBrush(Qt::NoBrush);

    if (nom == "menu") {
        p.drawLine(QPointF(4, 7), QPointF(20, 7));
        p.drawLine(QPointF(4, 12), QPointF(20, 12));
        p.drawLine(QPointF(4, 17), QPointF(20, 17));
    }
    else if (nom == "search") {
        p.drawEllipse(QPointF(10.5, 10.5), 6, 6);
        p.drawLine(QPointF(15, 15), QPointF(20, 20));
    }
    else if (nom == "bell") {
        QPainterPath b;
        b.moveTo(6.5, 16);
        b.lineTo(6.5, 11);
        b.cubicTo(6.5, 7.6, 8.9, 5.2, 12, 5.2);
        b.cubicTo(15.1, 5.2, 17.5, 7.6, 17.5, 11);
        b.lineTo(17.5, 16);
        b.lineTo(19.5, 18);
        b.lineTo(4.5, 18);
        b.closeSubpath();
        p.drawPath(b);
        p.drawLine(QPointF(12, 3), QPointF(12, 5));
        p.drawArc(QRectF(9.8, 18.2, 4.4, 3.6), 180 * 16, 180 * 16);
    }
    else if (nom == "users" || nom == "employes") {
        p.setPen(Qt::NoPen);
        p.setBrush(c);
        QPainterPath arriere = personne(16.5, 7.5, 3.2, 9.0, 18.5);
        QPainterPath avant = personne(9.0, 8.0, 3.8, 13.0, 20.5);
        QPainterPath marge;
        marge.addEllipse(QPointF(9.0, 8.0), 5.2, 5.2);
        QPainterPath margeCorps = personne(9.0, 8.0, 3.8, 15.6, 22);
        p.drawPath(arriere.subtracted(margeCorps.united(marge)));
        p.drawPath(avant);
    }
    else if (nom == "clipboard") {
        p.drawRoundedRect(QRectF(5, 4.5, 14, 17), 2, 2);
        p.setBrush(c);
        p.drawRoundedRect(QRectF(8.5, 2.5, 7, 4), 1.2, 1.2);
        p.drawLine(QPointF(8.5, 11), QPointF(15.5, 11));
        p.drawLine(QPointF(8.5, 14.5), QPointF(15.5, 14.5));
        p.drawLine(QPointF(8.5, 18), QPointF(13, 18));
    }
    else if (nom == "mannequin") {
        p.setPen(Qt::NoPen);
        p.setBrush(c);
        p.drawEllipse(QPointF(12, 2.6), 1.6, 1.6);
        p.drawRect(QRectF(11.2, 3.8, 1.6, 2.2));
        QPainterPath t;
        t.moveTo(8.2, 6);
        t.lineTo(15.8, 6);
        t.cubicTo(17.4, 8, 16.6, 10.6, 15.2, 12.2);
        t.cubicTo(16.8, 14, 17.4, 16.4, 16.4, 18);
        t.lineTo(7.6, 18);
        t.cubicTo(6.6, 16.4, 7.2, 14, 8.8, 12.2);
        t.cubicTo(7.4, 10.6, 6.6, 8, 8.2, 6);
        t.closeSubpath();
        p.drawPath(t);
        p.drawRect(QRectF(11.2, 18, 1.6, 3));
        p.drawRoundedRect(QRectF(8, 20.6, 8, 1.8), 0.9, 0.9);
    }
    else if (nom == "gear") {
        p.setPen(Qt::NoPen);
        p.setBrush(c);
        QPainterPath g;
        g.addEllipse(QPointF(12, 12), 6.6, 6.6);
        for (int i = 0; i < 8; ++i) {
            QTransform tr;
            tr.translate(12, 12);
            tr.rotate(i * 45);
            QPainterPath dent;
            dent.addRoundedRect(QRectF(-1.9, -10.2, 3.8, 5), 1, 1);
            g = g.united(tr.map(dent));
        }
        QPainterPath trou;
        trou.addEllipse(QPointF(12, 12), 3, 3);
        p.drawPath(g.subtracted(trou));
    }
    else if (nom == "tag") {
        p.setPen(Qt::NoPen);
        p.setBrush(c);
        QPainterPath t;
        t.moveTo(3, 5);
        t.quadTo(3, 3, 5, 3);
        t.lineTo(11, 3);
        t.lineTo(21, 13);
        t.lineTo(13, 21);
        t.lineTo(3, 11);
        t.closeSubpath();
        QPainterPath trou;
        trou.addEllipse(QPointF(7.6, 7.6), 1.9, 1.9);
        p.drawPath(t.subtracted(trou));
    }
    else if (nom == "chevron") {
        p.drawPolyline(QPolygonF({QPointF(6.5, 9.5), QPointF(12, 15), QPointF(17.5, 9.5)}));
    }
    else if (nom == "left") {
        p.drawPolyline(QPolygonF({QPointF(14.5, 6.5), QPointF(9, 12), QPointF(14.5, 17.5)}));
    }
    else if (nom == "right") {
        p.drawPolyline(QPolygonF({QPointF(9.5, 6.5), QPointF(15, 12), QPointF(9.5, 17.5)}));
    }
    else if (nom == "idcard") {
        p.setPen(QPen(c, 1.7, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        p.drawRoundedRect(QRectF(4, 3.5, 16, 17), 2.5, 2.5);
        p.drawEllipse(QPointF(12, 10), 2.6, 2.6);
        QPainterPath s;
        s.moveTo(7.5, 17.5);
        s.cubicTo(8.2, 14.6, 15.8, 14.6, 16.5, 17.5);
        p.drawPath(s);
    }
    else if (nom == "namecard") {
        p.setPen(QPen(c, 1.7, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        p.drawRoundedRect(QRectF(3.5, 3.5, 17, 17), 2.5, 2.5);
        p.drawEllipse(QPointF(12, 9.5), 2.8, 2.8);
        QPainterPath s;
        s.moveTo(7, 17);
        s.cubicTo(7.8, 13.6, 16.2, 13.6, 17, 17);
        p.drawPath(s);
        p.drawLine(QPointF(7, 17), QPointF(17, 17));
    }
    else if (nom == "type") {
        p.setPen(QPen(c, 1.7, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        p.drawRoundedRect(QRectF(4, 4, 16, 16), 3, 3);
        p.drawEllipse(QPointF(12, 13), 3.4, 3.4);
        p.drawLine(QPointF(8, 7.5), QPointF(10, 7.5));
    }
    else if (nom == "calendar") {
        p.setPen(QPen(c, 1.7, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        p.drawRoundedRect(QRectF(3.5, 5, 17, 15.5), 2.5, 2.5);
        p.drawLine(QPointF(3.5, 9.5), QPointF(20.5, 9.5));
        p.drawLine(QPointF(8, 3), QPointF(8, 6.5));
        p.drawLine(QPointF(16, 3), QPointF(16, 6.5));
        p.setPen(Qt::NoPen);
        p.setBrush(c);
        for (int r = 0; r < 2; ++r)
            for (int k = 0; k < 3; ++k)
                p.drawRect(QRectF(7 + k * 4, 12.2 + r * 3.8, 2, 2));
    }
    else if (nom == "plus") {
        p.drawLine(QPointF(12, 4.5), QPointF(12, 19.5));
        p.drawLine(QPointF(4.5, 12), QPointF(19.5, 12));
    }
    else if (nom == "pencil") {
        QPainterPath s;
        s.moveTo(4, 20);
        s.lineTo(5, 15.5);
        s.lineTo(15.5, 5);
        s.lineTo(19, 8.5);
        s.lineTo(8.5, 19);
        s.closeSubpath();
        p.drawPath(s);
        p.drawLine(QPointF(13.5, 7), QPointF(17, 10.5));
    }
    else if (nom == "trash") {
        p.drawLine(QPointF(4, 6.5), QPointF(20, 6.5));
        p.drawPolyline(QPolygonF({QPointF(9, 6.5), QPointF(9, 3.5), QPointF(15, 3.5), QPointF(15, 6.5)}));
        p.drawPolyline(QPolygonF({QPointF(6, 6.5), QPointF(7, 20.5), QPointF(17, 20.5), QPointF(18, 6.5)}));
        p.drawLine(QPointF(10, 10), QPointF(10, 17));
        p.drawLine(QPointF(14, 10), QPointF(14, 17));
    }
    else if (nom == "eye") {
        p.setPen(Qt::NoPen);
        p.setBrush(c);
        QPainterPath o;
        o.moveTo(1.5, 12);
        o.cubicTo(5, 5.5, 19, 5.5, 22.5, 12);
        o.cubicTo(19, 18.5, 5, 18.5, 1.5, 12);
        o.closeSubpath();
        QPainterPath anneau;
        anneau.addEllipse(QPointF(12, 12), 4.4, 4.4);
        QPainterPath pupille;
        pupille.addEllipse(QPointF(12, 12), 2.5, 2.5);
        p.drawPath(o.subtracted(anneau).united(pupille));
    }
    else if (nom == "reset") {
        p.drawArc(QRectF(4.5, 4.5, 15, 15), 150 * 16, -300 * 16);
        p.drawPolyline(QPolygonF({QPointF(4.2, 3.8), QPointF(5.2, 8.6), QPointF(10, 7.4)}));
    }
    else if (nom == "pdf") {
        p.setPen(QPen(c, 1.7, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        p.drawPolygon(QPolygonF({QPointF(6, 3), QPointF(14, 3), QPointF(19, 8), QPointF(19, 21), QPointF(6, 21)}));
        p.drawPolyline(QPolygonF({QPointF(14, 3), QPointF(14, 8), QPointF(19, 8)}));
        p.drawLine(QPointF(9, 13), QPointF(16, 13));
        p.drawLine(QPointF(9, 16.5), QPointF(16, 16.5));
    }
    else if (nom == "list") {
        p.setPen(Qt::NoPen);
        p.setBrush(c);
        QPainterPath r;
        r.addRoundedRect(QRectF(3.5, 3.5, 17, 17), 4, 4);
        QPainterPath fenetre;
        fenetre.addRoundedRect(QRectF(8, 7.5, 8, 5), 1, 1);
        p.drawPath(r.subtracted(fenetre));
    }
    else if (nom == "stats") {
        p.setPen(Qt::NoPen);
        p.setBrush(c);
        p.drawRoundedRect(QRectF(3.5, 12, 4.4, 9), 1, 1);
        p.drawRoundedRect(QRectF(9.8, 9, 4.4, 12), 1, 1);
        p.drawRoundedRect(QRectF(16.1, 13.5, 4.4, 7.5), 1, 1);
        p.drawEllipse(QPointF(5.7, 8.6), 2, 2);
        p.drawEllipse(QPointF(12, 5.4), 2, 2);
        p.drawEllipse(QPointF(18.3, 10), 2, 2);
    }
    else if (nom == "play") {
        p.setPen(Qt::NoPen);
        p.setBrush(c);
        p.drawPolygon(QPolygonF({QPointF(8, 5), QPointF(19, 12), QPointF(8, 19)}));
    }
    else if (nom == "box" || nom == "boxcheck") {
        p.setPen(QPen(c, 1.6));
        if (nom == "boxcheck") p.setBrush(c);
        p.drawRoundedRect(QRectF(4.5, 4.5, 15, 15), 3, 3);
        if (nom == "boxcheck") {
            p.setPen(QPen(Qt::white, 2.2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
            p.drawPolyline(QPolygonF({QPointF(8, 12.2), QPointF(10.8, 15), QPointF(16.2, 9)}));
        }
    }
    else if (nom == "avatar") {
        p.setPen(Qt::NoPen);
        p.setBrush(c);
        p.drawEllipse(QRectF(0.5, 0.5, 23, 23));
        p.setBrush(Qt::white);
        QPainterPath clip;
        clip.addEllipse(QRectF(0.5, 0.5, 23, 23));
        p.setClipPath(clip);
        p.drawPath(personne(12, 9.2, 4.3, 15, 26));
    }
}
} // namespace

QPixmap Icones::pixmap(const QString &nom, const QColor &couleur, int taille)
{
    const qreal ratio = 2.0;  // rendu haute définition
    QPixmap pm(int(taille * ratio), int(taille * ratio));
    pm.fill(Qt::transparent);
    QPainter p(&pm);
    p.setRenderHint(QPainter::Antialiasing);
    p.scale(taille * ratio / 24.0, taille * ratio / 24.0);
    dessiner(p, nom, couleur);
    p.end();
    pm.setDevicePixelRatio(ratio);
    return pm;
}

QIcon Icones::icone(const QString &nom, const QColor &couleur, int taille)
{
    return QIcon(pixmap(nom, couleur, taille));
}

QIcon Icones::iconeDouble(const QString &nom, const QColor &normal, const QColor &coche, int taille)
{
    QIcon ic;
    ic.addPixmap(pixmap(nom, normal, taille), QIcon::Normal, QIcon::Off);
    ic.addPixmap(pixmap(nom, coche, taille), QIcon::Normal, QIcon::On);
    ic.addPixmap(pixmap(nom, coche, taille), QIcon::Active, QIcon::On);
    ic.addPixmap(pixmap(nom, normal, taille), QIcon::Active, QIcon::Off);
    return ic;
}
