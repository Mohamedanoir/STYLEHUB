#pragma once
#include <QDate>
#include <QList>
#include <QString>
#include <QVector>
#include "machine.h"

// Les deux métiers innovants du module « Gestion des Machines ».
// Ils utilisent uniquement Type_Machine, Etat et Date_Prochaine_Maintenance
// (aucun historique, aucune alerte, aucun champ supplémentaire).
namespace Metier {

/* ----------------------------------------------------------------------------
 * Métier 1 : calculer la performance du parc par type de machine
 *            (part de machines opérationnelles) et identifier le type le plus performant.
 * Une machine est opérationnelle quand son état est « Disponible ».
 * performance d'un type = machines disponibles du type / machines totales du type
 * ------------------------------------------------------------------------- */
struct Performance
{
    QString type;
    int total = 0;
    int operationnelles = 0;
    int pourcentage() const { return total ? qRound(100.0 * operationnelles / total) : 0; }
};

// Types présents dans le parc, du plus performant au moins performant.
// Égalité de taux : le type qui compte le plus de machines passe devant.
// Le type le plus performant est donc le premier élément du résultat.
QVector<Performance> performanceParType(const QList<Machine> &machines);

/* ----------------------------------------------------------------------------
 * Métier 2 : optimiser automatiquement le planning de maintenance des machines.
 * Règles (par ordre de priorité de traitement) :
 *   1. machine indisponible  -> planifiée dès aujourd'hui ;
 *   2. maintenance en retard -> replanifiée dès aujourd'hui ;
 *   3. machine en maintenance -> maintenance en cours (aujourd'hui) ;
 *   4. les autres gardent leur date, sauf si elle tombe un week-end
 *      (report au prochain jour ouvrable) ou si une machine du même type
 *      est déjà planifiée ce jour-là (report au jour ouvrable suivant).
 * Résultat : jamais deux machines d'un même type en maintenance le même jour,
 * jamais de maintenance le samedi ou le dimanche.
 * ------------------------------------------------------------------------- */
struct Intervention
{
    enum Motif { Conservee, Indisponible, Retard, EnCours, Weekend, Conflit };

    QString id, nom, type;
    QDate   actuelle;       // date enregistrée
    QDate   proposee;       // date proposée par l'optimisation
    Motif   motif = Conservee;
    int     joursRetard = 0;

    bool    modifiee() const { return proposee != actuelle; }
    bool    urgente() const  { return motif == Indisponible || motif == Retard; }
    QString raison() const;
};

bool estJourOuvrable(const QDate &d);   // lundi à vendredi

// Planning complet, trié par date proposée croissante.
QVector<Intervention> planifierMaintenances(const QList<Machine> &machines,
                                            const QDate &aujourdhui = QDate::currentDate());

} // namespace Metier
