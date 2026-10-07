#pragma once
#include <QWidget>
#include <QList>
#include <QDateTime>

// ============================================================================
//  Module « Gestion des Employés » (code repris du projet STYLEHUB-gestion-employes)
//  Avant : une fenêtre complète (image du menu + barre du haut + contenu).
//  Maintenant : une PAGE de l'application intégrée (page « Gestion Employés »
//  du QStackedWidget « stackedPages » dans mainwindow.ui, widget promu).
//  Le menu de gauche et la barre du haut sont ceux de l'application.
// ============================================================================

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

class EmployesPage : public QWidget {
    Q_OBJECT
public:
    explicit EmployesPage(QWidget *parent=nullptr);
    QString searchText() const;
    void setSearchText(const QString &text);   // synchronisé avec la recherche de la barre du haut
signals:
    void searchTextChanged(const QString &text);
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
    QTableWidget *m_table=nullptr;
    QLineEdit *m_id=nullptr,*m_nom=nullptr,*m_prenom=nullptr,*m_salary=nullptr,*m_password=nullptr;
    QLineEdit *m_search=nullptr;               // recherche de la « Liste des employés »
    QComboBox *m_poste=nullptr;
    QLabel *m_status=nullptr;
    StatsWidget *m_stats=nullptr;
};
