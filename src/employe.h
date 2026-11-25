#pragma once

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
    QString etatPresence = QStringLiteral("ABSENT");

    QString code() const;

    static bool authentifier(const QString &identifiant,
                             const QString &motDePasse,
                             Employe *employe,
                             QString *erreur = nullptr);
    static bool preparerSchemaPresence(QString *erreur = nullptr);

    bool ajouter(QString *erreur = nullptr);
    bool modifier(QString *erreur = nullptr) const;
    static bool supprimer(qlonglong id, QString *erreur = nullptr);
    static bool changerPresence(qlonglong id, bool present, QString *erreur = nullptr);
    static QList<Employe> afficher(QString *erreur = nullptr);
};
