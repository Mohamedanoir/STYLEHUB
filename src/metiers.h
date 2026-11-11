#pragma once
#include <QDate>
#include <QList>
#include <QString>
#include <QVector>
#include <QtMath>
#include "businessrecords.h"
#include "machine.h"

// Les deux métiers innovants du module « Gestion des Machines ».
// Les deux métiers utilisent directement les trois compteurs de la fiche Machine.
namespace Metier {

/* ----------------------------------------------------------------------------
 * Métier 1 : performance par type à partir des compteurs Machine.
 * disponibilité = fonctionnement / (fonctionnement + arrêt).
 * rendement = pièces produites / heures de fonctionnement.
 * Le score compare disponibilité et rendement relatif entre types.
 * ------------------------------------------------------------------------- */
struct Performance
{
    QString type;
    int totalMachines = 0;
    int nombreMachines = 0;
    double disponibilite = 0.0;
    double productionAtteinte = 0.0;
    double piecesParHeure = 0.0;
    double score = 0.0;
    int pourcentage() const { return qRound(score); }
};

// Types renseignés, du score le plus élevé au plus faible.
QVector<Performance> performanceParType(const QList<Machine> &machines,
                                        const QList<MachineReading> &readings);

/* ----------------------------------------------------------------------------
 * Métier 2 : optimiser automatiquement le planning de maintenance des machines.
 * Règles (par ordre de priorité de traitement) :
 *   1. machine indisponible  -> planifiée dès aujourd'hui ;
 *   2. maintenance en retard -> replanifiée dès aujourd'hui ;
 *   3. machine en maintenance -> maintenance en cours (aujourd'hui) ;
 *   4. type sous 70 % et échéance dans 30 jours -> entretien avancé ;
 *   5. les autres gardent leur date, sauf si elle tombe un week-end
 *      (report au prochain jour ouvrable) ou si une machine du même type
 *      est déjà planifiée ce jour-là (report au jour ouvrable suivant).
 * Résultat : jamais deux machines d'un même type en maintenance le même jour,
 * jamais de maintenance le samedi ou le dimanche.
 * ------------------------------------------------------------------------- */
struct Intervention
{
    enum Motif { Conservee, Indisponible, Retard, EnCours, PerformanceFaible, Weekend, Conflit, Capacite };

    QString id, nom, type;
    QDate   actuelle;       // date enregistrée
    QDate   proposee;       // date proposée par l'optimisation
    Motif   motif = Conservee;
    int     joursRetard = 0;
    double  scoreType = 0.0;
    bool    scoreDisponible = false;
    double  dureeEstimee = 2.0;

    bool    modifiee() const { return proposee != actuelle; }
    bool    urgente() const  { return motif == Indisponible || motif == Retard; }
    QString raison() const;
};

bool estJourOuvrable(const QDate &d);   // lundi à vendredi

// Planning complet, trié par date proposée croissante.
QVector<Intervention> planifierMaintenances(const QList<Machine> &machines,
                                            const QVector<Performance> &performances,
                                            const QList<MaintenanceRecord> &maintenances,
                                            const QDate &aujourdhui = QDate::currentDate());

} // namespace Metier
