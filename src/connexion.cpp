#include "connexion.h"

#include <QSqlError>
#include <QSqlQuery>

namespace {
const QString kConnectionName = QStringLiteral("FashionNovaConnection");
}

Connexion::Connexion()
    : m_db(QSqlDatabase::addDatabase(QStringLiteral("QODBC"), kConnectionName))
{
}

Connexion::~Connexion()
{
    fermer();
}

Connexion &Connexion::instance()
{
    static Connexion uniqueInstance;
    return uniqueInstance;
}

bool Connexion::ouvrir(const QString &utilisateur,
                       const QString &motDePasse,
                       QString *erreur)
{
    if (!QSqlDatabase::isDriverAvailable(QStringLiteral("QODBC"))) {
        if (erreur)
            *erreur = QStringLiteral("Le pilote Qt QODBC n'est pas disponible pour ce kit.");
        return false;
    }

    if (m_db.isOpen())
        m_db.close();

    // DSN-less ODBC uses the installed 64-bit Oracle in XE driver and local XE service.
    // Credentials are supplied at runtime and are never saved by the project.
    m_db.setDatabaseName(QStringLiteral("DRIVER={Oracle in XE};DBQ=XE;"));
    m_db.setUserName(utilisateur.trimmed());
    m_db.setPassword(motDePasse);

    if (!m_db.open()) {
        if (erreur)
            *erreur = m_db.lastError().text();
        return false;
    }

    QSqlQuery verification(m_db);
    if (!verification.exec(QStringLiteral("SELECT 1 FROM DUAL"))) {
        if (erreur)
            *erreur = verification.lastError().text();
        m_db.close();
        return false;
    }

    return true;
}

void Connexion::fermer()
{
    if (m_db.isValid() && m_db.isOpen())
        m_db.close();
}

QSqlDatabase Connexion::base() const
{
    return m_db;
}
