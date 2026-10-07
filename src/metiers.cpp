#include "metiers.h"

#include <algorithm>
#include <QMap>
#include <QSet>

namespace Metier {

/* ===================== Métier 1 : performance par type ===================== */

QVector<Performance> performanceParType(const QList<Machine> &machines)
{
    QMap<QString, Performance> byType;
    for (const Machine &m : machines) {
        Performance &p = byType[m.type];
        p.type = m.type;
        ++p.total;
        if (m.etat == Etat::disponible())
            ++p.operationnelles;
    }

    // ordre de base : celui de la liste des types (résultat stable), puis les types inconnus
    QVector<Performance> list;
    for (const QString &t : typesMachine())
        if (byType.contains(t))
            list.append(byType.take(t));
    for (const Performance &p : std::as_const(byType))
        list.append(p);

    std::stable_sort(list.begin(), list.end(), [](const Performance &a, const Performance &b) {
        const int lhs = a.operationnelles * b.total;     // a.taux > b.taux  <=>  a.op * b.total > b.op * a.total
        const int rhs = b.operationnelles * a.total;
        if (lhs != rhs) return lhs > rhs;
        return a.total > b.total;
    });
    return list;
}

/* ===================== Métier 2 : planning de maintenance ===================== */

bool estJourOuvrable(const QDate &d)
{
    return d.dayOfWeek() < 6;      // 6 = samedi, 7 = dimanche
}

QString Intervention::raison() const
{
    switch (motif) {
    case Indisponible: return QStringLiteral("indisponible : traitée en priorité");
    case Retard:       return QStringLiteral("en retard de %1 j").arg(joursRetard);
    case EnCours:      return QStringLiteral("maintenance en cours");
    case Weekend:      return QStringLiteral("week-end : reportée au jour ouvrable");
    case Conflit:      return QStringLiteral("même type déjà prévu ce jour");
    case Conservee:    break;
    }
    return QStringLiteral("date conservée");
}

namespace {
struct Item { const Machine *m; int rang; };    // rang : 0 indisponible, 1 retard, 2 en maintenance, 3 autre
}

QVector<Intervention> planifierMaintenances(const QList<Machine> &machines, const QDate &today)
{
    QVector<Item> order;
    for (const Machine &m : machines) {
        const int j = int(today.daysTo(m.prochaineMaintenance));
        int rang = 3;
        if (m.etat == Etat::indisponible())        rang = 0;
        else if (j < 0)                            rang = 1;
        else if (m.etat == Etat::maintenance())    rang = 2;
        order.append({ &m, rang });
    }
    // priorité de traitement : rang, puis date la plus ancienne, puis identifiant
    std::stable_sort(order.begin(), order.end(), [](const Item &a, const Item &b) {
        if (a.rang != b.rang) return a.rang < b.rang;
        if (a.m->prochaineMaintenance != b.m->prochaineMaintenance)
            return a.m->prochaineMaintenance < b.m->prochaineMaintenance;
        return a.m->id < b.m->id;
    });

    QSet<QString> occupe;            // « type|date » déjà planifiés
    auto key = [](const QString &type, const QDate &d) { return type + QLatin1Char('|') + d.toString(Qt::ISODate); };

    QVector<Intervention> plan;
    for (const Item &it : std::as_const(order)) {
        const Machine &m = *it.m;
        Intervention iv;
        iv.id = m.id; iv.nom = m.nom; iv.type = m.type;
        iv.actuelle = m.prochaineMaintenance;
        iv.joursRetard = qMax(0, -int(today.daysTo(m.prochaineMaintenance)));

        QDate d = (it.rang <= 2) ? today : m.prochaineMaintenance;
        Intervention::Motif premierObstacle = Intervention::Conservee;
        while (!estJourOuvrable(d) || occupe.contains(key(m.type, d))) {
            if (premierObstacle == Intervention::Conservee)
                premierObstacle = estJourOuvrable(d) ? Intervention::Conflit : Intervention::Weekend;
            d = d.addDays(1);
        }
        occupe.insert(key(m.type, d));
        iv.proposee = d;

        switch (it.rang) {
        case 0:  iv.motif = Intervention::Indisponible; break;
        case 1:  iv.motif = Intervention::Retard;       break;
        case 2:  iv.motif = Intervention::EnCours;      break;
        default: iv.motif = premierObstacle;            break;
        }
        plan.append(iv);
    }

    // affichage : par date proposée, puis par priorité de traitement (ordre d'insertion), puis par id
    QVector<int> idx(plan.size());
    for (int i = 0; i < idx.size(); ++i) idx[i] = i;
    std::stable_sort(idx.begin(), idx.end(), [&](int a, int b) {
        if (plan[a].proposee != plan[b].proposee) return plan[a].proposee < plan[b].proposee;
        return a < b;
    });
    QVector<Intervention> sorted;
    for (int i : std::as_const(idx)) sorted.append(plan[i]);
    return sorted;
}

} // namespace Metier
