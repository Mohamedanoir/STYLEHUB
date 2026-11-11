#pragma once
#include <QList>
#include <QString>
#include "businessrecords.h"
#include "machine.h"

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
