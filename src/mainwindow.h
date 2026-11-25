#pragma once
#include <QMainWindow>
#include <QList>
#include "employe.h"

class QTableWidget;
class QLineEdit;
class QComboBox;
class QLabel;
class QGroupBox;
class QCloseEvent;
class StatsWidget;

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent=nullptr);
private slots:
    void employeeLoginFlow();
    void logoutEmployee();
    void addEmployee();
    void modifyEmployee();
    void removeEmployee();
    void showEmployee();
    void importCV();
    void rowClicked(int row,int col);
    void searchChanged(const QString&);
protected:
    void closeEvent(QCloseEvent *event) override;
private:
    void buildUi();
    bool loadEmployees();
    void refresh();
    void updateStats();
    void fillForm(const Employe&);
    void clearForm();
    int selectedIndex() const;
    Employe formEmployee() const;
    QString readText(const QString&);
    int parseCV(const QString&);
    QList<Employe> m_employees;
    QString m_sortField = QStringLiteral("Nom");
    qlonglong m_sessionEmployeeId = 0;
    QTableWidget *m_table;
    QLineEdit *m_id,*m_nom,*m_prenom,*m_salary,*m_password,*m_search,*m_listSearch;
    QComboBox *m_poste;
    QLabel *m_presenceLabel;
    StatsWidget *m_stats;
};
