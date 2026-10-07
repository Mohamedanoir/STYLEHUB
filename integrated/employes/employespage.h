#pragma once
#include <QMainWindow>
#include <QList>
#include <QDateTime>

struct Employee {
    QString id, nom, prenom, poste, salaire, password;
    bool present=false;
    QDateTime lastLogin;
};

class QTableWidget;
class QLineEdit;
class QComboBox;
class QLabel;
class QGroupBox;
class StatsWidget;

class EmployesPage : public QMainWindow {
    Q_OBJECT
public:
    explicit EmployesPage(QWidget *parent=nullptr);
private slots:
    void login();
    void logout();
    void addEmployee();
    void modifyEmployee();
    void removeEmployee();
    void showEmployee();
    void importCV();
    void rowClicked(int row,int col);
    void searchChanged(const QString&);
private:
    void buildUi();
    void refresh();
    void updateStats();
    void fillForm(const Employee&);
    int selectedIndex() const;
    Employee formEmployee() const;
    QString readText(const QString&);
    void parseCV(const QString&);
    QList<Employee> m_employees;
    QString m_loggedId;
    QTableWidget *m_table;
    QLineEdit *m_id,*m_nom,*m_prenom,*m_salary,*m_password,*m_search;
    QComboBox *m_poste;
    QLabel *m_status,*m_total,*m_present,*m_absent;
    StatsWidget *m_stats;
};
