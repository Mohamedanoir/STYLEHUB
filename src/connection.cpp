#include "connection.h"

#include <QInputDialog>
#include <QLineEdit>
#include <QSqlError>

namespace {
QString environmentValue(const char *name, const QString &fallback = {})
{
    const QString value = qEnvironmentVariable(name).trimmed();
    return value.isEmpty() ? fallback : value;
}
}

bool Connection::open(QString *error)
{
    if (QSqlDatabase::contains()) {
        QSqlDatabase existing = QSqlDatabase::database({}, false);
        if (existing.isOpen()) return true;
        existing.close();
    }

    QSqlDatabase db = QSqlDatabase::addDatabase(QStringLiteral("QODBC"));
    const QString dsn = environmentValue("FASHIONOVA_ORACLE_DSN", QStringLiteral("FASHIONOVA_XE"));
    db.setDatabaseName(dsn);
    db.setUserName(environmentValue("FASHIONOVA_DB_USER", QStringLiteral("FASHIONOVA")));

    QString password = environmentValue("FASHIONOVA_DB_PASSWORD");
    if (password.isEmpty()) {
        bool accepted = false;
        password = QInputDialog::getText(nullptr,
                                         QStringLiteral("Connexion à Oracle"),
                                         QStringLiteral("Mot de passe Oracle pour FASHIONOVA :"),
                                         QLineEdit::Password,
                                         QString(),
                                         &accepted);
        if (!accepted || password.isEmpty()) {
            if (error) *error = QStringLiteral("Connexion annulée : aucun mot de passe saisi.");
            return false;
        }
    }
    db.setPassword(password);

    if (!db.open()) {
        if (error) {
            *error = QStringLiteral("%1\n\nDSN utilisé : %2\n"
                                    "Vérifiez le pilote Oracle ODBC 64 bits, le DSN FASHIONOVA_XE, "
                                    "le service Oracle et le mot de passe.")
                         .arg(db.lastError().text(), dsn);
        }
        return false;
    }
    return true;
}

bool Connection::isOpen()
{
    const QSqlDatabase db = QSqlDatabase::database({}, false);
    return db.isValid() && db.driverName() == QStringLiteral("QODBC") && db.isOpen();
}

QSqlDatabase Connection::database()
{
    return QSqlDatabase::database();
}
