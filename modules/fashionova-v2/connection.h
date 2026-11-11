#ifndef CONNECTION_H
#define CONNECTION_H

#include <QSqlDatabase>

class Connection
{
public:
    static Connection& createInstance();
    bool createConnect();
    void closeConnect();

private:
    Connection() {}
    Connection(const Connection&) = delete;
    Connection& operator=(const Connection&) = delete;
    QSqlDatabase db;
};

#endif // CONNECTION_H