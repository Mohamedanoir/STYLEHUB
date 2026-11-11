#pragma once

#include <QDialog>
#include <QList>
#include "businessrecords.h"
#include "machine.h"

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
