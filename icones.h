#ifndef ICONES_H
#define ICONES_H

#include <QColor>
#include <QIcon>
#include <QPixmap>
#include <QString>

// Icônes vectorielles dessinées en code (nettes à toutes les tailles, aucun fichier à ajouter)
namespace Icones
{
    // noms : menu, search, bell, users, employes, clipboard, mannequin, gear, tag,
    //        chevron, idcard, namecard, type, calendar, plus, pencil, trash, eye,
    //        reset, pdf, list, stats, play, left, right, box, boxcheck, avatar
    QPixmap pixmap(const QString &nom, const QColor &couleur, int taille = 24);
    QIcon   icone(const QString &nom, const QColor &couleur, int taille = 24);

    // Icône à deux états (bouton non coché / coché) — pour le menu latéral
    QIcon   iconeDouble(const QString &nom, const QColor &normal, const QColor &coche, int taille = 32);
}

#endif // ICONES_H
