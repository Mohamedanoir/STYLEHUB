#pragma once
#include <QList>
#include <QString>
#include "machine.h"

class Database
{
public:
    static bool open(QString *error = nullptr);

    static QList<Machine> all();
    static bool insert(const Machine &m, QString *error = nullptr);
    static bool update(const Machine &m, QString *error = nullptr);
    static bool remove(const QString &id, QString *error = nullptr);
    static QString nextId();
};
