#pragma once

#include <QRegularExpression>
#include <cmath>
#include "machine.h"

namespace MachineValidation {
inline QRegularExpression namePattern()
{
    // Au moins une lettre ; chiffres, accents et séparateurs usuels autorisés.
    return QRegularExpression(QStringLiteral("(?=[\\s\\S]*\\p{L})[\\p{L}\\p{M}0-9]+(?:[ '’-][\\p{L}\\p{M}0-9]+)*"));
}

inline bool validate(const Machine &m, QString *error)
{
    auto fail = [error](const QString &message) {
        if (error) *error = message;
        return false;
    };
    if (m.nom.trimmed().isEmpty())
        return fail(QStringLiteral("Le nom de la machine est obligatoire."));
    const QRegularExpression fullName(
        QRegularExpression::anchoredPattern(namePattern().pattern()));
    if (m.nom.size() > 200 || !fullName.match(m.nom).hasMatch())
        return fail(QStringLiteral("Le nom doit contenir au moins une lettre. Lettres, chiffres (0-9), "
                                   "des espaces, des apostrophes ou des traits d'union, sur 200 caractères maximum. "
                                   "Un nom entièrement numérique et les autres caractères spéciaux sont interdits."));
    if (!typesMachine().contains(m.type))
        return fail(QStringLiteral("Choisissez un type de machine valide."));
    if (!Etat::all().contains(m.etat))
        return fail(QStringLiteral("Choisissez un état de machine valide."));
    if (!m.prochaineMaintenance.isValid())
        return fail(QStringLiteral("La date de prochaine maintenance est invalide."));
    if (!std::isfinite(m.heuresFonctionnement) || !std::isfinite(m.heuresArret)
        || m.heuresFonctionnement < 0 || m.heuresArret < 0 || m.piecesProduites < 0)
        return fail(QStringLiteral("Les heures et le nombre de pièces doivent être positifs ou nuls."));
    return true;
}
}
