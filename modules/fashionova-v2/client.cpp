#include "client.h"

#include <QDebug>
#include <QRegularExpression>
#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>

namespace {
QString g_derniereErreur; // dernier message d'erreur (SQL ou contrôle du modèle)

void memoriserErreur(const char *contexte, const QString &texte)
{
    g_derniereErreur = texte;
    qDebug() << contexte << ":" << texte;
}
} // namespace

QString Client::derniereErreur()
{
    return g_derniereErreur;
}

// ====================================================================
//  Contrôles de saisie
// ====================================================================

QString Client::erreurId(const QString &id)
{
    const QString v = id.trimmed();
    if (v.isEmpty())
        return QString::fromUtf8("L'identifiant client est obligatoire (format attendu : CL-001).");

    static const QRegularExpression re(QStringLiteral("^CL-[0-9]{3}$"));
    if (!v.startsWith(QStringLiteral("CL-")))
        return QString::fromUtf8("L'identifiant doit commencer par « CL- » suivi de trois chiffres "
                                 "(exemple : CL-001).");
    if (!re.match(v).hasMatch())
        return QString::fromUtf8("L'identifiant doit contenir exactement trois chiffres après « CL- » "
                                 "(exemple : CL-001).");
    if (v == QStringLiteral("CL-000"))
        return QString::fromUtf8("L'identifiant CL-000 n'est pas valide : la numérotation commence à CL-001.");
    return QString();
}

QString Client::erreurTel(const QString &tel)
{
    const QString t = tel.trimmed();
    if (t.isEmpty())
        return QString::fromUtf8("Le numéro de téléphone est obligatoire "
                                 "(exemple : +216 12 345 678).");

    for (const QChar ch : t)
        if (ch.isLetter())
            return QString::fromUtf8("Le numéro de téléphone ne doit pas contenir de lettres "
                                     "(exemple : +216 12 345 678).");

    static const QRegularExpression reCaracteres(QStringLiteral("^[+0-9 ]+$"));
    if (!reCaracteres.match(t).hasMatch())
        return QString::fromUtf8("Le numéro de téléphone contient des caractères non autorisés. "
                                 "Seuls les chiffres, les espaces et le « + » initial sont acceptés.");

    if (t.indexOf(QLatin1Char('+')) > 0 || t.count(QLatin1Char('+')) > 1)
        return QString::fromUtf8("Le signe « + » n'est autorisé qu'au tout début du numéro "
                                 "(exemple : +216 12 345 678).");

    if (!t.startsWith(QStringLiteral("+216")))
        return QString::fromUtf8("Le numéro doit être un numéro tunisien commençant par +216 "
                                 "(exemple : +216 12 345 678).");

    QString chiffres = t.mid(4);
    chiffres.remove(QLatin1Char(' '));
    if (chiffres.size() != 8)
        return QString::fromUtf8("Après l'indicatif +216, le numéro doit contenir exactement 8 chiffres "
                                 "(%1 saisi%2). Exemple : +216 12 345 678.")
            .arg(chiffres.size())
            .arg(chiffres.size() > 1 ? QStringLiteral("s") : QString());
    return QString();
}

QString Client::normaliserTel(const QString &tel)
{
    if (!erreurTel(tel).isEmpty())
        return tel.trimmed();
    QString d = tel.trimmed().mid(4);
    d.remove(QLatin1Char(' '));
    return QStringLiteral("+216 ") + d.mid(0, 2) + QLatin1Char(' ')
           + d.mid(2, 3) + QLatin1Char(' ') + d.mid(5, 3);
}

QString Client::normaliserAdresse(const QString &adresse)
{
    QString s = adresse.simplified(); // supprime les espaces en début/fin et les doublons
    // « Rue X ,Tunis » -> « Rue X, Tunis »
    static const QRegularExpression reVirgule(QStringLiteral("\\s*,\\s*"));
    s.replace(reVirgule, QStringLiteral(", "));
    return s.trimmed();
}

QString Client::erreurAdresse(const QString &adresse)
{
    const QString a = normaliserAdresse(adresse);
    if (a.isEmpty())
        return QString::fromUtf8("L'adresse est obligatoire "
                                 "(exemple : 12 Rue de la Liberté, Tunis 1000).");
    if (a.size() < 10)
        return QString::fromUtf8("L'adresse est trop courte : indiquez au minimum le numéro, la rue "
                                 "et la ville (exemple : 12 Rue de la Liberté, Tunis).");
    if (a.size() > 200)
        return QString::fromUtf8("L'adresse est trop longue (200 caractères maximum).");

    // caractères autorisés : lettres (accents/arabe inclus), chiffres, espace , ، . ' ’ - / ° # ( )
    static const QRegularExpression reAutorises(
        QString::fromUtf8("^[\\p{L}0-9 ,،.'’\\-/°#()]+$"));
    if (!reAutorises.match(a).hasMatch())
        return QString::fromUtf8("L'adresse contient des caractères non autorisés. Utilisez uniquement "
                                 "des lettres, des chiffres, des espaces et les signes , . ' - / # ( ).");

    int lettres = 0;
    for (const QChar ch : a)
        if (ch.isLetter()) ++lettres;
    if (lettres == 0)
        return QString::fromUtf8("L'adresse ne peut pas contenir uniquement des chiffres ou des symboles : "
                                 "ajoutez le nom de la rue et de la ville.");

    static const QRegularExpression reMotLong(QString::fromUtf8("\\p{L}{3,}"));
    if (lettres < 5 || !reMotLong.match(a).hasMatch())
        return QString::fromUtf8("L'adresse semble incohérente : indiquez le nom de la rue et de la ville "
                                 "avec des mots complets.");

    if (a.split(QRegularExpression(QStringLiteral("[ ,]+")), Qt::SkipEmptyParts).size() < 2)
        return QString::fromUtf8("L'adresse est incomplète : indiquez au moins la rue et la ville "
                                 "(exemple : 12 Rue de la Liberté, Tunis).");

    static const QRegularExpression reRepete(QStringLiteral("(.)\\1{3,}"));
    if (reRepete.match(a).hasMatch())
        return QString::fromUtf8("L'adresse contient des caractères répétés de façon incohérente "
                                 "(exemple : « aaaa » ou « 1111 »).");

    static const QRegularExpression reSymbolesColles(QString::fromUtf8("[,.'’\\-/#]{2,}"));
    if (reSymbolesColles.match(a).hasMatch())
        return QString::fromUtf8("L'adresse contient des signes de ponctuation consécutifs incorrects.");

    static const QRegularExpression reDebut(QString::fromUtf8("^[\\p{L}0-9]"));
    static const QRegularExpression reFin(QString::fromUtf8("[\\p{L}0-9).]$"));
    if (!reDebut.match(a).hasMatch() || !reFin.match(a).hasMatch())
        return QString::fromUtf8("L'adresse doit commencer par une lettre ou un chiffre et ne doit pas "
                                 "se terminer par un signe de ponctuation incorrect.");
    return QString();
}

// Dernier filet de sécurité : même si l'interface oubliait un contrôle,
// rien d'invalide n'est envoyé à Oracle.
QString Client::erreurModele() const
{
    QString e = erreurId(id);
    if (e.isEmpty()) e = erreurTel(tel);
    if (e.isEmpty()) e = erreurAdresse(adresse);
    return e;
}

// ====================================================================
//  Unicité de l'identifiant
// ====================================================================

bool Client::idExiste(const QString &id, const QString &ignorerId, bool *ok)
{
    if (ok) *ok = true;
    QSqlQuery query;
    if (ignorerId.isEmpty()) {
        // QODBC utilise des paramètres positionnels. Les paramètres nommés
        // sont interprétés de façon variable selon le pilote Oracle et
        // peuvent produire ORA-00904 ("ID": invalid identifier).
        query.prepare("SELECT COUNT(*) FROM CLIENTS WHERE ID_APP = ?");
    } else {
        query.prepare("SELECT COUNT(*) FROM CLIENTS WHERE ID_APP = ? AND ID_APP <> ?");
    }
    query.addBindValue(id);
    if (!ignorerId.isEmpty()) query.addBindValue(ignorerId);

    if (!query.exec() || !query.next()) {
        memoriserErreur("Client::idExiste", query.lastError().text());
        if (ok) *ok = false;
        return false;
    }
    return query.value(0).toInt() > 0;
}

// ====================================================================
//  CRUD
// ====================================================================

bool Client::ajouter() const
{
    const QString e = erreurModele();
    if (!e.isEmpty()) {
        memoriserErreur("Client::ajouter (validation)", e);
        return false;
    }

    QSqlQuery query;
    query.prepare("INSERT INTO CLIENTS (ID_CLIENT, ID_APP, NOM_CLIENT, PRENOM_CLIENT, TELEPHONE, EMAIL, ADRESSE, "
                  "TYPE_CLIENT, DATE_INSCRIPTION, REMARQUES) "
                  "VALUES (FASHIONOVA_CLIENT_SEQ.NEXTVAL, ?, ?, ?, ?, ?, ?, ?, TO_DATE(?, 'DD/MM/YYYY'), ?)");
    query.addBindValue(id);
    query.addBindValue(nom);
    query.addBindValue(prenom);
    query.addBindValue(tel);
    query.addBindValue(email);
    query.addBindValue(adresse);
    query.addBindValue(type);
    query.addBindValue(date);
    query.addBindValue(remarques);

    if (!query.exec()) {
        memoriserErreur("Client::ajouter", query.lastError().text());
        return false;
    }
    return true;
}

QVector<Client> Client::lister()
{
    g_derniereErreur.clear();
    QVector<Client> liste;
    QSqlQuery query;
    query.prepare("SELECT ID_APP, NOM_CLIENT, PRENOM_CLIENT, TELEPHONE, EMAIL, ADRESSE, TYPE_CLIENT, "
                  "TO_CHAR(DATE_INSCRIPTION, 'DD/MM/YYYY'), REMARQUES "
                  "FROM CLIENTS ORDER BY ID_APP");
    if (!query.exec()) {
        memoriserErreur("Client::lister", query.lastError().text());
        return liste;
    }
    while (query.next()) {
        Client c;
        c.id        = query.value(0).toString();
        c.nom       = query.value(1).toString();
        c.prenom    = query.value(2).toString();
        c.tel       = query.value(3).toString();
        c.email     = query.value(4).toString();
        c.adresse   = query.value(5).toString();
        c.type      = query.value(6).toString();
        c.date      = query.value(7).toString();
        c.remarques = query.value(8).toString();
        liste.append(c);
    }
    return liste;
}

bool Client::modifier(const QString &ancienId) const
{
    const QString e = erreurModele();
    if (!e.isEmpty()) {
        memoriserErreur("Client::modifier (validation)", e);
        return false;
    }

    QSqlQuery query;
    query.prepare("UPDATE CLIENTS SET ID_APP = ?, NOM_CLIENT = ?, PRENOM_CLIENT = ?, "
                  "TELEPHONE = ?, EMAIL = ?, ADRESSE = ?, TYPE_CLIENT = ?, "
                  "DATE_INSCRIPTION = TO_DATE(?, 'DD/MM/YYYY'), REMARQUES = ? "
                  "WHERE ID_APP = ?");
    query.addBindValue(id);
    query.addBindValue(nom);
    query.addBindValue(prenom);
    query.addBindValue(tel);
    query.addBindValue(email);
    query.addBindValue(adresse);
    query.addBindValue(type);
    query.addBindValue(date);
    query.addBindValue(remarques);
    query.addBindValue(ancienId);

    if (!query.exec()) {
        memoriserErreur("Client::modifier", query.lastError().text());
        return false;
    }
    if (query.numRowsAffected() <= 0) {
        memoriserErreur("Client::modifier", QString::fromUtf8("Aucune ligne modifiée (client introuvable)."));
        return false;
    }
    return true;
}

bool Client::supprimer(const QString &id)
{
    QSqlQuery query;
    query.prepare("DELETE FROM CLIENTS WHERE ID_APP = ?");
    query.addBindValue(id);

    if (!query.exec()) {
        memoriserErreur("Client::supprimer", query.lastError().text());
        return false;
    }
    return query.numRowsAffected() > 0;
}
