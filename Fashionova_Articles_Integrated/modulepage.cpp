#include "modulepage.h"

#include <QHeaderView>
#include <QMessageBox>
#include <QPushButton>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlTableModel>
#include <QTableView>
#include <QVBoxLayout>
#include <QHBoxLayout>

ModulePage::ModulePage(const QString &table, const QString &title, QWidget *parent)
    : QWidget(parent), m_model(new QSqlTableModel(this)), m_view(new QTableView(this))
{
    auto *layout = new QVBoxLayout(this);
    auto *toolbar = new QHBoxLayout;
    auto *add = new QPushButton("＋ Ajouter une ligne", this);
    auto *save = new QPushButton("Enregistrer", this);
    auto *remove = new QPushButton("− Supprimer la sélection", this);
    auto *refresh = new QPushButton("Actualiser", this);
    toolbar->addWidget(add);
    toolbar->addWidget(save);
    toolbar->addWidget(remove);
    toolbar->addStretch();
    toolbar->addWidget(refresh);
    layout->addLayout(toolbar);
    layout->addWidget(m_view, 1);

    m_model->setTable(table);
    m_model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    m_view->setModel(m_model);
    m_view->setAlternatingRowColors(true);
    m_view->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_view->setSelectionMode(QAbstractItemView::SingleSelection);
    m_view->horizontalHeader()->setStretchLastSection(true);
    m_view->verticalHeader()->setVisible(false);
    m_view->setStyleSheet("QTableView{background:white;alternate-background-color:#fbf6ee;"
                          "gridline-color:#e7dfd4;border:1px solid #e7dfd4;}");
    setStyleSheet("QPushButton{padding:8px 12px;border:1px solid #d9cbb9;border-radius:6px;"
                  "background:white;color:#1d2a3a;} QPushButton:hover{background:#f1e4d3;}");
    setWindowTitle(title);

    connect(add, &QPushButton::clicked, this, &ModulePage::ajouterLigne);
    connect(save, &QPushButton::clicked, this, &ModulePage::enregistrer);
    connect(remove, &QPushButton::clicked, this, &ModulePage::supprimerLigne);
    connect(refresh, &QPushButton::clicked, this, &ModulePage::actualiser);
    actualiser();
}

void ModulePage::actualiser()
{
    if (!m_model->select())
        QMessageBox::warning(this, "Base de données", m_model->lastError().text());
}

void ModulePage::ajouterLigne()
{
    static const QHash<QString, QPair<QString, QString>> ids = {
        {"CLIENTS", {"ID_CLIENT", "SEQ_CLIENTS"}},
        {"EMPLOYES", {"ID_EMPLOYE", "SEQ_EMPLOYES"}},
        {"COMMANDES", {"ID_COMMANDE", "SEQ_COMMANDES"}},
        {"MAQUETTES", {"ID_MAQUETTE", "SEQ_MAQUETTES"}},
        {"MACHINES", {"ID_MACHINE", "SEQ_MACHINES"}}
    };
    const auto idInfo = ids.value(m_model->tableName());
    QSqlQuery sequence;
    if (idInfo.second.isEmpty() || !sequence.exec("SELECT " + idInfo.second + ".NEXTVAL FROM DUAL")
        || !sequence.next()) {
        QMessageBox::warning(this, "Ajout impossible", sequence.lastError().text());
        return;
    }
    const int row = m_model->rowCount();
    if (!m_model->insertRow(row)) {
        QMessageBox::warning(this, "Ajout impossible", m_model->lastError().text());
        return;
    }
    const int idColumn = m_model->record().indexOf(idInfo.first);
    if (idColumn < 0 || !m_model->setData(m_model->index(row, idColumn), sequence.value(0))) {
        m_model->revertAll();
        QMessageBox::warning(this, "Ajout impossible", "Colonne d'identifiant introuvable dans la table.");
        return;
    }
    m_view->selectRow(row);
    m_view->scrollToBottom();
    if (m_view->model()->columnCount() > 0)
        m_view->edit(m_model->index(row, 0));
}

void ModulePage::enregistrer()
{
    if (!m_model->submitAll()) {
        QMessageBox::warning(this, "Enregistrement impossible", m_model->lastError().text());
        return;
    }
    actualiser();
}

void ModulePage::supprimerLigne()
{
    const QModelIndex index = m_view->currentIndex();
    if (!index.isValid()) {
        QMessageBox::information(this, "Suppression", "Sélectionnez une ligne à supprimer.");
        return;
    }
    if (QMessageBox::question(this, "Confirmer", "Supprimer la ligne sélectionnée ?") != QMessageBox::Yes)
        return;
    if (!m_model->removeRow(index.row()) || !m_model->submitAll()) {
        QMessageBox::warning(this, "Suppression impossible", m_model->lastError().text());
        m_model->revertAll();
    }
}
