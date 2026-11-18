#pragma once

#include <QWidget>

class QSqlTableModel;
class QTableView;

// Page de gestion des tables transversales de Fashionova.
// Les requêtes restent pilotées par le modèle Qt SQL et utilisent la connexion
// Oracle ouverte par Connection dans main.cpp.
class ModulePage : public QWidget
{
    Q_OBJECT
public:
    explicit ModulePage(const QString &table, const QString &title, QWidget *parent = nullptr);

private slots:
    void ajouterLigne();
    void enregistrer();
    void supprimerLigne();
    void actualiser();

private:
    QSqlTableModel *m_model = nullptr;
    QTableView *m_view = nullptr;
};
