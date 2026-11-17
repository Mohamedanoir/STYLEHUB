#pragma once
#include <QMainWindow>
#include <QList>
#include "employe.h"

class QTableWidget;
class QLineEdit;
class QComboBox;
class QLabel;
class QGroupBox;
class StatsWidget;

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent=nullptr);
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
    bool loadEmployees();
    void refresh();
    void updateStats();
    void fillForm(const Employe&);
    int selectedIndex() const;
    Employe formEmployee() const;
    QString readText(const QString&);
    void parseCV(const QString&);
    QList<Employe> m_employees;
    QString m_loggedId;
    QString m_sortField = QStringLiteral("Nom");
    QTableWidget *m_table;
    QLineEdit *m_id,*m_nom,*m_prenom,*m_salary,*m_password,*m_search,*m_listSearch;
    QComboBox *m_poste;
    QLabel *m_status,*m_total,*m_present,*m_absent;
    StatsWidget *m_stats;
};
