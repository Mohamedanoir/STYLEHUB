#ifndef EMPLOYEE_H
#define EMPLOYEE_H

#include <QDateTime>
#include <QString>

struct Employee {
    QString id, nom, prenom, poste, salaire, password;
    bool present=false;
    QDateTime lastLogin;
};

#endif // EMPLOYEE_H
