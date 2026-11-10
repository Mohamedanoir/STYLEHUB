#pragma once
#include <QWidget>
#include <QList>
#include "employe.h"

class QTableWidget;
class QLineEdit;
class QComboBox;
class QLabel;
class QGroupBox;
class EmployeStatsWidget;

// ============================================================================
//  EmployePage — module Employés intégré dans la fenêtre principale FashioNova
//  (anciennement MainWindow du projet FashionNova_Employes)
// ============================================================================
class EmployePage : public QWidget {
    Q_OBJECT
public:
    explicit EmployePage(QWidget *parent = nullptr);

    // Appelé par MainWindow quand on entre dans la page Employés
    bool loadEmployees();
    void refresh();
    void updateStats();

private slots:
    void addEmployee();
    void modifyEmployee();
    void removeEmployee();
    void showEmployee();
    void importCV();
    void rowClicked(int row, int col);
    void searchChanged(const QString &);

private:
    void buildUi();
    void fillForm(const Employe &);
    int selectedIndex() const;
    Employe formEmployee() const;
    QString readText(const QString &);
    void parseCV(const QString &);

    QList<Employe> m_employees;
    QString m_sortField = QStringLiteral("Nom");
    QTableWidget   *m_table;
    QLineEdit      *m_id, *m_nom, *m_prenom, *m_salary, *m_password,
                   *m_search, *m_listSearch;
    QComboBox      *m_poste;
    QLabel         *m_status, *m_total, *m_present, *m_absent;
    EmployeStatsWidget *m_stats;
};
