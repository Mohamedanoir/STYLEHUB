#pragma once

#include <QDateTime>
#include <QList>
#include <QString>

class Employe
{
public:
    qlonglong id = 0;
    QString nom;
    QString prenom;
    QString poste;
    double salaire = 0.0;
    QString motDePasse;

    // Attendance fields are UI-only; the approved EMPLOYES table has no such columns.
    bool present = false;
    QDateTime derniereConnexion;

    QString code() const;

    bool ajouter(QString *erreur = nullptr);
    bool modifier(QString *erreur = nullptr) const;
    static bool supprimer(qlonglong id, QString *erreur = nullptr);
    static QList<Employe> afficher(QString *erreur = nullptr);
};
