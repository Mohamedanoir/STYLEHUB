#include "metiers.h"

#include <algorithm>
#include <utility>
#include <QHash>
#include <QMap>
#include <QSet>
#include <QtMath>

namespace Metier {

/* ===================== Métier 1 : performance par type ===================== */

QVector<Performance> performanceParType(const QList<Machine> &machines,
                                        const QList<MachineReading> &readings)
{
    Q_UNUSED(readings);
    struct Totals {
        double operating = 0.0;
        double stopped = 0.0;
        int produced = 0;
    };
    QMap<QString, Performance> byType;
    for (const Machine &m : machines) {
        byType[m.type].type = m.type;
        ++byType[m.type].totalMachines;
    }
    QMap<QString, Totals> totals;
    for (const Machine &m : machines) {
        Totals &t = totals[m.type];
        t.operating += m.heuresFonctionnement;
        t.stopped += m.heuresArret;
        t.produced += m.piecesProduites;
    }
    double bestRate = 0.0;
    for (auto it = totals.cbegin(); it != totals.cend(); ++it)
        if (it->operating > 0.0)
            bestRate = qMax(bestRate, it->produced / it->operating);

    QVector<Performance> list;
    for (auto it = byType.cbegin(); it != byType.cend(); ++it) {
        const Totals v = totals.value(it.key());
        if (v.operating <= 0.0) continue;
        Performance p = it.value();
        p.nombreMachines = p.totalMachines;
        p.piecesParHeure = v.produced / v.operating;
        p.disponibilite = v.operating + v.stopped > 0.0
            ? 100.0 * v.operating / (v.operating + v.stopped) : 0.0;
        p.productionAtteinte = bestRate > 0.0 ? 100.0 * p.piecesParHeure / bestRate : 0.0;
        p.score = (p.disponibilite + p.productionAtteinte) / 2.0;
        list.append(p);
    }

    std::stable_sort(list.begin(), list.end(), [](const Performance &a, const Performance &b) {
        if (qAbs(a.score - b.score) > 0.001) return a.score > b.score;
        if (a.nombreMachines != b.nombreMachines) return a.nombreMachines > b.nombreMachines;
        return a.type < b.type;
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
    case PerformanceFaible: return QStringLiteral("type sous 70 % de performance : entretien avancé (%1 %)").arg(qRound(scoreType));
    case Weekend:      return QStringLiteral("week-end : reportée au jour ouvrable");
    case Conflit:      return QStringLiteral("même type déjà prévu ce jour");
    case Capacite:     return QStringLiteral("capacité journalière de 8 h atteinte : reportée");
    case Conservee:    break;
    }
    return QStringLiteral("date conservée");
}

namespace {
struct Item { const Machine *m; int rang; double score; bool hasScore; double downtimeRatio; };
}

QVector<Intervention> planifierMaintenances(const QList<Machine> &machines,
                                            const QVector<Performance> &performances,
                                            const QList<MaintenanceRecord> &maintenances,
                                            const QDate &today)
{
    QHash<QString, double> scoreParType;
    for (const Performance &p : performances) scoreParType.insert(p.type, p.score);
    QHash<QString, double> dureeParMachine;
    QHash<QString, int> recordIdParMachine;
    for (const MaintenanceRecord &r : maintenances) {
        if (r.status != QStringLiteral("Planifiée") && r.status != QStringLiteral("En cours")) continue;
        if (r.id >= recordIdParMachine.value(r.machineId, 0)) {
            recordIdParMachine.insert(r.machineId, r.id);
            dureeParMachine.insert(r.machineId, r.estimatedHours);
        }
    }

    QVector<Item> order;
    for (const Machine &m : machines) {
        const int j = int(today.daysTo(m.prochaineMaintenance));
        const bool hasScore = scoreParType.contains(m.type);
        const double score = scoreParType.value(m.type, 100.0);
        int rang = 3;
        if (m.etat == Etat::indisponible())                 rang = 0;
        else if (j < 0)                                     rang = 1;
        else if (m.etat == Etat::maintenance())             rang = 2;
        else if (hasScore && score < 70.0 && j <= 30)        rang = 3;
        else                                                 rang = 4;
        const double totalHours = m.heuresFonctionnement + m.heuresArret;
        const double downtimeRatio = totalHours > 0.0 ? m.heuresArret / totalHours : 0.0;
        order.append({ &m, rang, score, hasScore, downtimeRatio });
    }
    // priorité de traitement : rang, puis date la plus ancienne, puis identifiant
    std::stable_sort(order.begin(), order.end(), [](const Item &a, const Item &b) {
        if (a.rang != b.rang) return a.rang < b.rang;
        if (qAbs(a.downtimeRatio - b.downtimeRatio) > 0.001)
            return a.downtimeRatio > b.downtimeRatio;
        if (qAbs(a.m->heuresFonctionnement - b.m->heuresFonctionnement) > 0.001)
            return a.m->heuresFonctionnement > b.m->heuresFonctionnement;
        if (a.hasScore && b.hasScore && qAbs(a.score - b.score) > 0.001)
            return a.score < b.score;
        if (a.m->prochaineMaintenance != b.m->prochaineMaintenance)
            return a.m->prochaineMaintenance < b.m->prochaineMaintenance;
        return a.m->id < b.m->id;
    });

    QSet<QString> occupe;            // « type|date » déjà planifiés
    QHash<QString, double> heuresParJour;
    auto key = [](const QString &type, const QDate &d) { return type + QLatin1Char('|') + d.toString(Qt::ISODate); };

    QVector<Intervention> plan;
    for (const Item &it : std::as_const(order)) {
        const Machine &m = *it.m;
        Intervention iv;
        iv.id = m.id; iv.nom = m.nom; iv.type = m.type;
        iv.actuelle = m.prochaineMaintenance;
        iv.joursRetard = qMax(0, -int(today.daysTo(m.prochaineMaintenance)));
        iv.scoreDisponible = scoreParType.contains(m.type);
        iv.scoreType = scoreParType.value(m.type, 0.0);
        iv.dureeEstimee = qBound(0.5, dureeParMachine.value(m.id, 2.0), 8.0);

        QDate d = (it.rang <= 3) ? today : m.prochaineMaintenance;
        Intervention::Motif premierObstacle = Intervention::Conservee;
        while (!estJourOuvrable(d) || occupe.contains(key(m.type, d))
               || heuresParJour.value(d.toString(Qt::ISODate)) + iv.dureeEstimee > 8.0) {
            if (premierObstacle == Intervention::Conservee) {
                if (!estJourOuvrable(d)) premierObstacle = Intervention::Weekend;
                else if (occupe.contains(key(m.type, d))) premierObstacle = Intervention::Conflit;
                else premierObstacle = Intervention::Capacite;
            }
            d = d.addDays(1);
        }
        occupe.insert(key(m.type, d));
        heuresParJour[d.toString(Qt::ISODate)] += iv.dureeEstimee;
        iv.proposee = d;

        switch (it.rang) {
        case 0:  iv.motif = Intervention::Indisponible; break;
        case 1:  iv.motif = Intervention::Retard;       break;
        case 2:  iv.motif = Intervention::EnCours;      break;
        case 3:  iv.motif = Intervention::PerformanceFaible; break;
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
