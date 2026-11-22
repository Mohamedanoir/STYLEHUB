#pragma once
#include <cmath>
#include <QColor>
#include <QDate>
#include <QDialog>
#include <QFrame>
#include <QIcon>
#include <QList>
#include <QMainWindow>
#include <QPixmap>
#include <QRegularExpression>
#include <QSet>
#include <QSize>
#include <QString>
#include <QStringList>
#include <QtMath>
#include <QVector>
#include <QWidget>

// Les déclarations des classes du projet sont regroupées dans ce fichier.

// ---- contenu fusionné depuis machine.h ----
// États possibles d'une machine (attribut « Etat » du cahier des charges)
namespace Etat {
inline QString disponible()   { return QStringLiteral("Disponible"); }
inline QString maintenance()  { return QStringLiteral("En maintenance"); }
inline QString indisponible() { return QStringLiteral("Indisponible"); }
inline QStringList all()      { return QStringList() << disponible() << maintenance() << indisponible(); }
}

inline QStringList typesMachine()
{
    return QStringList() << QStringLiteral("Couture") << QStringLiteral("Coupe")
                         << QStringLiteral("Finition") << QStringLiteral("Broderie")
                         << QStringLiteral("Repassage") << QStringLiteral("Autre");
}

// Entité « Machine » : ID_Machine, Nom_Machine, Type_Machine, Etat,
// Date_Prochaine_Maintenance et les indicateurs de fonctionnement.
class Machine
{
public:
    QString id;
    QString nom;
    QString type;
    QString etat = Etat::disponible();
    QDate   prochaineMaintenance = QDate::currentDate().addDays(60);
    QString photo;                       // chemin d'une photo (optionnel)
    double heuresFonctionnement = 0.0;
    int piecesProduites = 0;
    double heuresArret = 0.0;

    int joursRestants() const;
    bool indisponible() const;

    // Détection automatique : texte vide = aucune maintenance nécessaire
    QString raisonMaintenance() const;
    bool maintenanceRequise() const;
    bool critique() const;
};
// ---- contenu fusionné depuis businessrecords.h ----
// Relevé de production saisi pour une machine et une journée.
struct MachineReading
{
    int id = 0;
    QString machineId;
    QDate date;
    double plannedHours = 0.0;
    double operatingHours = 0.0;
    double downtimeHours = 0.0;
    int quantityProduced = 0;
    int productionTarget = 0;
    QString stopReason;
};

// Intervention enregistrée dans l'historique de planification.
struct MaintenanceRecord
{
    int id = 0;
    QString machineId;
    QDate plannedDate;
    QString interventionType = QStringLiteral("Préventive");
    int priority = 3;
    double estimatedHours = 2.0;
    QString status = QStringLiteral("Planifiée");
    QString description;
};
// ---- contenu fusionné depuis bannerwidget.h ----
// Bandeau « Gestion des Machines » (tissu + machine à coudre + citation)
class BannerWidget : public QWidget
{
public:
    explicit BannerWidget(QWidget *parent = nullptr);
protected:
    void paintEvent(QPaintEvent *) override;
    void resizeEvent(QResizeEvent *) override;
private:
    void rebuild();
    QPixmap m_fabric;
    QPixmap m_machine;
};
// ---- contenu fusionné depuis chartwidgets.h ----
// Anneau « Répartition par type » avec légende
class DonutChart : public QWidget
{
public:
    struct Slice { QString label; int value; QColor color; };
    explicit DonutChart(QWidget *parent = nullptr);
    void setData(const QVector<Slice> &slices, const QString &unit);
    QSize minimumSizeHint() const override { return QSize(300, 190); }
protected:
    void paintEvent(QPaintEvent *) override;
private:
    QVector<Slice> m_slices;
    QString m_unit;
};

// Métier 1 : score réel par type, calculé à partir des relevés quotidiens.
class PerformanceBars : public QWidget
{
public:
    struct Entry { QString type; int score; int disponibilite; int production; int machines; };
    explicit PerformanceBars(QWidget *parent = nullptr);
    void setData(const QVector<Entry> &entries, const QString &summary);
    QSize minimumSizeHint() const override { return QSize(280, 190); }
protected:
    void paintEvent(QPaintEvent *) override;
private:
    QVector<Entry> m_entries;
    QString m_summary;
};

// Métier 2 : liste « Planning de maintenance optimisé »
// (date enregistrée -> date proposée + raison de l'ajustement)
class PlanningList : public QWidget
{
public:
    struct Entry { QString nom; QString ancienne; QString proposee; QString raison; bool modifiee; QColor couleur; };
    explicit PlanningList(QWidget *parent = nullptr);
    void setData(const QVector<Entry> &entries, const QString &summary);
    QSize minimumSizeHint() const override { return QSize(280, 190); }
protected:
    void paintEvent(QPaintEvent *) override;
private:
    QVector<Entry> m_entries;
    QString m_summary;
};
// ---- contenu fusionné depuis database.h ----
class Database
{
public:
    static bool open(QString *error = nullptr);

    static QList<Machine> all();
    static QList<MachineReading> allReadings();
    static QList<MaintenanceRecord> allMaintenanceRecords();
    static bool insert(const Machine &m, QString *error = nullptr);
    static bool update(const Machine &m, QString *error = nullptr);
    static bool remove(const QString &id, QString *error = nullptr);
    static bool insertReading(const MachineReading &reading, QString *error = nullptr);
    static bool applyMaintenancePlan(const QList<MaintenanceRecord> &records, QString *error = nullptr);
    static QString nextId();
};
// ---- contenu fusionné depuis detailsdialog.h ----
namespace Ui { class DetailsDialog; }

// Boîte « Afficher » : détails d'une machine + état de la maintenance (detailsdialog.ui)
class DetailsDialog : public QDialog
{
    Q_OBJECT
public:
    explicit DetailsDialog(const Machine &machine, QWidget *parent = nullptr);
    ~DetailsDialog() override;

private:
    Ui::DetailsDialog *ui;
};
// ---- contenu fusionné depuis machinespage.h ----
class QHBoxLayout;

namespace Ui { class MachinesPage; }

// Page « Gestion des Machines » : formulaire, liste, statistiques.
// L'interface est définie dans machinespage.ui (Qt Designer) ; ce fichier ne contient que la logique.
class MachinesPage : public QWidget
{
    Q_OBJECT
public:
    explicit MachinesPage(QWidget *parent = nullptr);
    ~MachinesPage() override;

    int  alertCount() const { return m_alertCount; }
    void setSearchText(const QString &text);        // synchronise avec la recherche de la barre du haut
    void showAlertsMenu(QWidget *anchor);           // menu de la cloche

signals:
    void searchTextChanged(const QString &text);
    void alertCountChanged(int count);
    void notify(const QString &message, bool error);

private slots:
    void onAdd();
    void onModify();
    void onDelete();
    void onReset();
    void onExportPdf();
    void onChoosePhoto();
    void onAddReading();
    void onPlanMaintenance();
    void onApplyOptimizedPlan();
    void onRowSelected();
    void onSearchChanged(const QString &text);
    void updateDetection();

private:
    // initialisation (les widgets eux-mêmes viennent du .ui)
    void setupIcons();
    void setupBusinessActions();
    void setupTable();
    void connectSignals();

    // données
    void reloadData();
    void applyFilter();
    void renderPage();
    void buildPager(int pages);
    void updateCharts();
    void checkAlerts();
    void goToMachine(const QString &id);
    const Machine *findMachine(const QString &id) const;

    // formulaire
    bool readForm(Machine &m, QString *error) const;
    void loadForm(const Machine &m);
    void clearForm();
    void updatePreview();

    Ui::MachinesPage *ui;
    QHBoxLayout *m_pager = nullptr;

    QList<Machine> m_all;
    QList<Machine> m_view;
    QString m_currentId;
    QString m_formPhoto;
    int  m_page = 0;
    int  m_alertCount = 0;
    bool m_alertsReady = false;
    QSet<QString> m_knownUnavailable;
};
// ---- contenu fusionné depuis machinevalidation.h ----
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
// ---- contenu fusionné depuis mainwindow.h ----
class QLabel;

namespace Ui { class MainWindow; }

// Fenêtre principale : menu latéral, barre du haut, pile de pages.
// L'interface est définie dans mainwindow.ui (Qt Designer) ;
// la page « Machines » est un widget promu : voir machinespage.ui / machinespage.cpp.
class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void onBellClicked();
    void updateBadge(int count);
    void showToast(const QString &message, bool error);

private:
    void setupSidebar();
    void setupTopBar();

    Ui::MainWindow *ui;
    QLabel *m_badge = nullptr;   // pastille de la cloche (superposée au bouton)
    QLabel *m_toast = nullptr;   // notification flottante
};
// ---- contenu fusionné depuis metiers.h ----
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
// ---- contenu fusionné depuis recorddialogs.h ----
class QComboBox;
class QDateEdit;
class QDoubleSpinBox;
class QLineEdit;
class QSpinBox;

class MachineReadingDialog : public QDialog
{
    Q_OBJECT
public:
    explicit MachineReadingDialog(const QList<Machine> &machines, QWidget *parent = nullptr);
    MachineReading reading() const;

protected:
    void accept() override;

private:
    QComboBox *m_machine = nullptr;
    QDateEdit *m_date = nullptr;
    QDoubleSpinBox *m_planned = nullptr;
    QDoubleSpinBox *m_operating = nullptr;
    QDoubleSpinBox *m_downtime = nullptr;
    QSpinBox *m_produced = nullptr;
    QSpinBox *m_target = nullptr;
    QLineEdit *m_reason = nullptr;
};

class MaintenancePlanDialog : public QDialog
{
    Q_OBJECT
public:
    explicit MaintenancePlanDialog(const QList<Machine> &machines,
                                   const QList<MaintenanceRecord> &existing,
                                   QWidget *parent = nullptr);
    MaintenanceRecord record() const;

protected:
    void accept() override;

private:
    QList<Machine> m_machines;
    QList<MaintenanceRecord> m_existing;
    QComboBox *m_machine = nullptr;
    QDateEdit *m_date = nullptr;
    QComboBox *m_type = nullptr;
    QSpinBox *m_priority = nullptr;
    QDoubleSpinBox *m_duration = nullptr;
    QLineEdit *m_description = nullptr;
};
// ---- contenu fusionné depuis sidebarwidget.h ----
// Menu latéral avec image de fond (logo en haut, photo en bas).
// L'image garde sa largeur : le haut (logo) et le bas (photo) restent à taille fixe,
// seule la zone du milieu (bleu uni) s'étire selon la hauteur de la fenêtre.
// Utilisé comme widget promu dans mainwindow.ui.
class SidebarWidget : public QFrame
{
    Q_OBJECT
public:
    explicit SidebarWidget(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QPixmap m_bg;
};
// ---- contenu fusionné depuis uihelpers.h ----
namespace UI {
QPixmap svgPixmap(const QString &name, const QColor &color, int size);
QIcon   svgIcon(const QString &name, const QColor &color, int size = 24);
QIcon   dotIcon(const QColor &color, int size = 14);
QPixmap cover(const QPixmap &src, const QSize &size, int radius = 0);
QPixmap machinePixmap(const QString &photoPath);      // photo de la machine ou image par défaut
QColor  etatColor(const QString &etat);
QColor  typeColor(const QString &type);
}