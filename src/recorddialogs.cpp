#include "recorddialogs.h"

#include <QComboBox>
#include <QDateEdit>
#include <QDialogButtonBox>
#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QSpinBox>
#include <QVBoxLayout>

namespace {
void setupMachineCombo(QComboBox *combo, const QList<Machine> &machines)
{
    for (const Machine &m : machines)
        combo->addItem(QStringLiteral("%1 — %2 (%3)").arg(m.id, m.nom, m.type), m.id);
}

QDoubleSpinBox *hoursSpin(QWidget *parent, double maximum = 24.0)
{
    auto *spin = new QDoubleSpinBox(parent);
    spin->setRange(0.0, maximum);
    spin->setDecimals(1);
    spin->setSingleStep(0.5);
    spin->setSuffix(QStringLiteral(" h"));
    return spin;
}
}

MachineReadingDialog::MachineReadingDialog(const QList<Machine> &machines, QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(QStringLiteral("Saisir un relevé de production"));
    setMinimumWidth(430);

    m_machine = new QComboBox(this);
    setupMachineCombo(m_machine, machines);
    m_date = new QDateEdit(QDate::currentDate(), this);
    m_date->setCalendarPopup(true);
    m_date->setDisplayFormat(QStringLiteral("dd/MM/yyyy"));
    m_date->setMaximumDate(QDate::currentDate());
    m_planned = hoursSpin(this);
    m_planned->setValue(8.0);
    m_operating = hoursSpin(this);
    m_operating->setValue(8.0);
    m_downtime = hoursSpin(this);
    m_produced = new QSpinBox(this);
    m_produced->setRange(0, 100000000);
    m_target = new QSpinBox(this);
    m_target->setRange(1, 100000000);
    m_target->setValue(100);
    m_reason = new QLineEdit(this);
    m_reason->setPlaceholderText(QStringLiteral("Ex. panne, réglage, manque de matière"));

    auto *form = new QFormLayout;
    form->addRow(QStringLiteral("Machine"), m_machine);
    form->addRow(QStringLiteral("Date du relevé"), m_date);
    form->addRow(QStringLiteral("Heures prévues"), m_planned);
    form->addRow(QStringLiteral("Heures de fonctionnement"), m_operating);
    form->addRow(QStringLiteral("Heures d'arrêt"), m_downtime);
    form->addRow(QStringLiteral("Quantité produite (pièces)"), m_produced);
    form->addRow(QStringLiteral("Objectif (pièces)"), m_target);
    form->addRow(QStringLiteral("Cause d'arrêt"), m_reason);

    auto *buttons = new QDialogButtonBox(QDialogButtonBox::Save | QDialogButtonBox::Cancel, this);
    buttons->button(QDialogButtonBox::Save)->setText(QStringLiteral("Enregistrer"));
    buttons->button(QDialogButtonBox::Cancel)->setText(QStringLiteral("Annuler"));
    connect(buttons, &QDialogButtonBox::accepted, this, &MachineReadingDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &MachineReadingDialog::reject);
    auto *layout = new QVBoxLayout(this);
    layout->addLayout(form);
    layout->addWidget(buttons);
}

MachineReading MachineReadingDialog::reading() const
{
    MachineReading r;
    r.machineId = m_machine->currentData().toString();
    r.date = m_date->date();
    r.plannedHours = m_planned->value();
    r.operatingHours = m_operating->value();
    r.downtimeHours = m_downtime->value();
    r.quantityProduced = m_produced->value();
    r.productionTarget = m_target->value();
    r.stopReason = m_reason->text().trimmed();
    return r;
}

void MachineReadingDialog::accept()
{
    if (m_machine->currentIndex() < 0) {
        QMessageBox::warning(this, windowTitle(), QStringLiteral("Ajoutez d'abord une machine."));
        return;
    }
    if (m_planned->value() <= 0.0
        || m_operating->value() + m_downtime->value() > m_planned->value() + 0.001) {
        QMessageBox::warning(this, windowTitle(),
            QStringLiteral("Les heures de fonctionnement et d'arrêt ne peuvent pas dépasser les heures prévues."));
        return;
    }
    QDialog::accept();
}

MaintenancePlanDialog::MaintenancePlanDialog(const QList<Machine> &machines,
                                               const QList<MaintenanceRecord> &existing,
                                               QWidget *parent)
    : QDialog(parent), m_machines(machines), m_existing(existing)
{
    setWindowTitle(QStringLiteral("Planifier une maintenance"));
    setMinimumWidth(430);

    m_machine = new QComboBox(this);
    setupMachineCombo(m_machine, machines);
    m_date = new QDateEdit(QDate::currentDate(), this);
    m_date->setCalendarPopup(true);
    m_date->setDisplayFormat(QStringLiteral("dd/MM/yyyy"));
    m_date->setMinimumDate(QDate::currentDate());
    m_type = new QComboBox(this);
    m_type->addItems({QStringLiteral("Préventive"), QStringLiteral("Corrective")});
    m_priority = new QSpinBox(this);
    m_priority->setRange(1, 5);
    m_priority->setValue(3);
    m_priority->setToolTip(QStringLiteral("1 = faible priorité, 5 = priorité maximale"));
    m_duration = hoursSpin(this, 8.0);
    m_duration->setValue(2.0);
    m_description = new QLineEdit(this);
    m_description->setPlaceholderText(QStringLiteral("Motif ou tâche prévue"));

    auto *form = new QFormLayout;
    form->addRow(QStringLiteral("Machine"), m_machine);
    form->addRow(QStringLiteral("Date prévue"), m_date);
    form->addRow(QStringLiteral("Intervention"), m_type);
    form->addRow(QStringLiteral("Priorité (1 à 5)"), m_priority);
    form->addRow(QStringLiteral("Durée estimée"), m_duration);
    form->addRow(QStringLiteral("Description"), m_description);

    auto *buttons = new QDialogButtonBox(QDialogButtonBox::Save | QDialogButtonBox::Cancel, this);
    buttons->button(QDialogButtonBox::Save)->setText(QStringLiteral("Enregistrer"));
    buttons->button(QDialogButtonBox::Cancel)->setText(QStringLiteral("Annuler"));
    connect(buttons, &QDialogButtonBox::accepted, this, &MaintenancePlanDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &MaintenancePlanDialog::reject);
    auto *layout = new QVBoxLayout(this);
    layout->addLayout(form);
    layout->addWidget(buttons);
}

MaintenanceRecord MaintenancePlanDialog::record() const
{
    MaintenanceRecord r;
    r.machineId = m_machine->currentData().toString();
    r.plannedDate = m_date->date();
    r.interventionType = m_type->currentText();
    r.priority = m_priority->value();
    r.estimatedHours = m_duration->value();
    r.description = m_description->text().trimmed();
    return r;
}

void MaintenancePlanDialog::accept()
{
    if (m_machine->currentIndex() < 0) {
        QMessageBox::warning(this, windowTitle(), QStringLiteral("Ajoutez d'abord une machine."));
        return;
    }
    if (m_duration->value() <= 0.0) {
        QMessageBox::warning(this, windowTitle(), QStringLiteral("La durée doit être supérieure à zéro."));
        return;
    }
    if (m_date->date().dayOfWeek() >= 6) {
        QMessageBox::warning(this, windowTitle(), QStringLiteral("Le planning est limité aux jours ouvrables (lundi à vendredi)."));
        return;
    }
    QString selectedType;
    for (const Machine &m : m_machines)
        if (m.id == m_machine->currentData().toString()) selectedType = m.type;
    for (const MaintenanceRecord &r : m_existing) {
        if (r.machineId == m_machine->currentData().toString()
            || r.status != QStringLiteral("Planifiée") || r.plannedDate != m_date->date()) continue;
        const double usedHours = [&] {
            double total = 0.0;
            for (const MaintenanceRecord &dayRecord : m_existing)
                if (dayRecord.machineId != m_machine->currentData().toString()
                    && dayRecord.status == QStringLiteral("Planifiée") && dayRecord.plannedDate == m_date->date())
                    total += dayRecord.estimatedHours;
            return total;
        }();
        if (usedHours + m_duration->value() > 8.0) {
            QMessageBox::warning(this, windowTitle(),
                QStringLiteral("La capacité maximale de maintenance pour cette journée est de 8 heures."));
            return;
        }
        QString otherType;
        for (const Machine &m : m_machines)
            if (m.id == r.machineId) otherType = m.type;
        if (otherType == selectedType) {
            QMessageBox::warning(this, windowTitle(),
                QStringLiteral("Une autre machine du type %1 est déjà planifiée ce jour-là.").arg(selectedType));
            return;
        }
    }
    QDialog::accept();
}
