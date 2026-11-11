#pragma once

#include <QDate>
#include <QString>

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
