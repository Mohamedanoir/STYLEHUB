#ifndef CLIENTPAGE_H
#define CLIENTPAGE_H

#include <QWidget>
#include <QVector>

#include "client.h"

class QLabel;
class QLineEdit;
class QTableWidget;

class ClientPage : public QWidget
{
    Q_OBJECT
public:
    explicit ClientPage(QWidget *parent = nullptr);
    void reload();

private:
    void remplirTable();
    Client *clientSelectionne();
    void ajouter();
    void modifier();
    void supprimer();

    QVector<Client> m_clients;
    QLabel *m_erreur = nullptr;
    QLineEdit *m_recherche = nullptr;
    QTableWidget *m_table = nullptr;
};

#endif // CLIENTPAGE_H
