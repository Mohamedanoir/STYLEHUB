#ifndef CLIENT_H
#define CLIENT_H

#include <QString>
#include <QVector>

// Classe métier « Client » (Modèle) : toutes les requêtes SQL sont ici,
// aucune requête ne doit se trouver derrière un bouton de MainWindow.
// Les règles de contrôle de saisie (format ID, téléphone tunisien, adresse)
// sont également ici : elles sont ainsi partagées par l'ajout et la modification.
struct Client {
    QString id, nom, prenom, tel, email, adresse, type, date, remarques;

    // CRUD
    bool ajouter() const;                         // Create
    static QVector<Client> lister();              // Read
    bool modifier(const QString &ancienId) const; // Update
    static bool supprimer(const QString &id);     // Delete

    // ---- Contrôles de saisie : chaque fonction renvoie un message d'erreur
    //      précis, ou une chaîne vide si la valeur est valide.
    static QString erreurId(const QString &id);          // format CL-001
    static QString erreurTel(const QString &tel);        // +216 + 8 chiffres
    static QString erreurAdresse(const QString &adresse);

    // ---- Mise en forme (espaces inutiles, format canonique du téléphone)
    static QString normaliserTel(const QString &tel);       // +216 12 345 678
    static QString normaliserAdresse(const QString &adresse);

    // ---- Unicité de l'identifiant dans Oracle.
    //      ignorerId : identifiant à exclure (cas de la modification).
    //      ok (facultatif) : mis à false si la requête SQL a échoué.
    static bool idExiste(const QString &id, const QString &ignorerId = QString(),
                         bool *ok = nullptr);

    // Dernier message d'erreur SQL / de validation du modèle (pour l'affichage)
    static QString derniereErreur();

private:
    QString erreurModele() const; // dernier filet de sécurité avant l'écriture Oracle
};

#endif // CLIENT_H
