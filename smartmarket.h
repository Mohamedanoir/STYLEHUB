#ifndef SMARTMARKET_H
#define SMARTMARKET_H

#include <QMainWindow>
#include <QTableWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QLabel>
#include <QPushButton>
#include <QStackedWidget>
#include <QFrame>
#include <QVBoxLayout>

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QDate>
#include <QDateTime>
#include <QList>
#include <QPixmap>
#include <QSet>
#include <QVector>

#include "integrated/clients/client.h"
#include "integrated/articles/article.h"
#include "integrated/employes/employee.h"
#include "modules/machines/src/machine.h"

QT_BEGIN_NAMESPACE
namespace Ui { class SmartMarket; }
QT_END_NAMESPACE

class DeliveryMap;
class StatsWidget;
class Maquette;
class QHBoxLayout;
class QTimer;

class SmartMarket : public QMainWindow
{
    Q_OBJECT

public:
    // Index des pages du QStackedWidget « SWSmartMarket » (smartmarket.ui)
    enum Page { PLogin = 0, PMotDePasse, PMenu, PClient, PEmpl, PCommande, PMaquette, PMachine, PArticle };

    explicit SmartMarket(QWidget *parent = nullptr);
    ~SmartMarket() override;

protected:
    void resizeEvent(QResizeEvent *event) override;
    bool eventFilter(QObject *obj, QEvent *event) override;

private slots:
    // ---- Slots créés depuis Qt Designer (connexion automatique : on_<objet>_<signal>) ----
    // Menu : navigation entre les pages du stackedWidget principal
    void on_btnMClient_clicked();
    void on_btnMEmpl_clicked();
    void on_btnMCommande_clicked();
    void on_btnMMaquette_clicked();
    void on_btnMMachine_clicked();
    void on_btnMArticle_clicked();
    void on_btnDeconnexion_clicked();          // déconnexion : retour à la page de connexion

    // Menu principal (page PMenu) : un bouton par module
    void on_btnMenuClients_clicked();
    void on_btnMenuEmployes_clicked();
    void on_btnMenuCommandes_clicked();
    void on_btnMenuMaquettes_clicked();
    void on_btnMenuArticles_clicked();
    void on_btnMenuMachines_clicked();
    void on_btnMenuQuitter_clicked();
    void on_SWSmartMarket_currentChanged(int index);

    // Page de connexion
    void on_btnConnecter_clicked();
    void on_txtLogin_returnPressed();
    void on_txtMotDePasse_returnPressed();
    void on_btnMotDePasseOublie_clicked();     // -> page « Mot de passe oublié »
    void on_btnEnvoyerReset_clicked();
    void on_btnRetourLogin_clicked();

    // ---- Module Commandes ----
    void toggleSortOrders();
    void selectOrder(int row);
    void generateInvoicePdf();
    void openBarcodeScannerModal();
    void centerMap();
    void showSelectedRoute();

    // Chatbot IA Flottant
    void toggleChatWindow();
    void handleSendChatMessage();
    void onAiResponseReceived(QNetworkReply *reply);

private:
    Ui::SmartMarket *ui;                 // interface unique : smartmarket.ui

    // Menu / connexion
    void setupSidebar();
    void setupLogin();
    void setupMenu();
    void decorerBoutonMenu(QPushButton *bouton, const QString &icone);
    void ajouterIcone(QWidget *champ, const QString &image, bool aGauche);
    QWidget *m_oeil = nullptr;           // icône « œil » du mot de passe

    // Interface & Composants principaux (module Commandes)
    QLineEdit *searchEdit;
    QTableWidget *ordersTable;
    QComboBox *statusCombo;
    QComboBox *periodCombo;

    // Panneau de détails
    QLabel *selectedOrderLabel;
    QLabel *detailProductImage;
    QLabel *detailClient;
    QLabel *detailDate;
    QLabel *detailStatus;
    QLabel *detailAmount;
    QLabel *detailAddress;
    QLabel *distanceLabel;
    QLabel *costLabel;
    QLabel *delayLabel;

    // Carte & Données
    DeliveryMap *mapView = nullptr;
    QLabel *donutLabel;
    bool sortAscending;
    int currentRowSelected;
    QString selectedClientLat;
    QString selectedClientLon;

    // Chatbot Flottant
    QPushButton *chatFloatingBtn;
    QFrame *chatWindow;
    QWidget *chatMessagesArea;
    QVBoxLayout *chatMessagesLayout;
    QLineEdit *chatInput;

    // Infrastructure Agent IA (Groq Cloud / Llama 3.1)
    QNetworkAccessManager *networkManager;
    QJsonArray conversationHistory;
    QString getApiKey();

    // Méthodes d'initialisation UI
    QWidget* makeHeader();
    QWidget* makeKpis();
    QWidget* makeOrdersPanel();
    QWidget* makeDetailsPanel();
    QWidget* makeMapPanel();
    QWidget* makeBottomPanel();

    // Méthodes Métier & IA
    void setupOrders();
    void updateDetails(int row);
    void setupFloatingChatbot();
    void addChatMessage(const QString &sender, const QString &text, bool isUser);
    QString processChatbotQuery(const QString &query);
    void initAIAgent();

    // =====================================================================
    //  MODULE CLIENTS (page PClient) — slots copiés du projet « Clients »
    // =====================================================================

    

public:
    

private slots:
    void ajouterClient();
    void modifierClient();
    void supprimerClient();
    void afficherClient();
    void reinitialiserClient();
    void exporterPdfClient();
    void remplirFormulaireClient();
    void onFilterChangedClient();

private:
    QVector<Client> sortedFilteredClient() const;
    void refreshTableClient(const QString &keepId);
    Client formDataClient() const;
    bool validerClient(const Client &c);
    QString nextIdClient() const;
    int selectedIndexClient() const;
    QString selectedIdClient() const;
    int indexOfIdClient(const QString &id) const;
    void statusClient(const QString &text);
    void updateBannerClient();
    void updateStatsClient();
    void refreshHistoriqueClient();

    QPixmap bannerPixClient;
    QPixmap pixRepartitionClient, pixEvolutionClient, pixTop5Client;
    QVector<Client> clientsClient; // toutes les données
    QVector<Commande> commandesClient; // historique de toutes les commandes
    QVector<Client> viewClient;    // lignes actuellement affichées
private:
    void initClient();      // initialisation du module (ancien constructeur)

    // =====================================================================
    //  MODULE ARTICLES (page PArticle) — slots copiés du projet « Articles »
    // =====================================================================

    
public:
    
private slots:
    void onAddArticle();void onModArticle();void onDelArticle();void onViewArticle();
    void resetArticle();void refreshArticle();void exportPdfArticle();void onRowArticle();void onImgArticle();
private:
    void setImgArticle(const QString&p);bool readFormArticle(Article&a);void fillArticle(const Article&a);void statsArticle();
    QString imgArticle;QVector<Article> AArticle;QVector<int> viewArticle;int curArticle=-1;
private:
    void initArticle();      // initialisation du module (ancien constructeur)

    // =====================================================================
    //  MODULE MAQUETTES (page PMaquette) — slots copiés du projet « Maquettes »
    // =====================================================================

    
public:
    

public:
    void onPageAfficheeMaquette();     // appelé par SmartMarket quand la page Maquettes est affichée

private slots:
    void onAjouterMaquette();
    void onModifierMaquette();
    void onSupprimerMaquette();
    void onAfficherMaquette();
    void onReinitialiserMaquette();
    void onExporterPdfMaquette();
    void onChoisirVideoMaquette();
    void onLireVideoMaquette();
    void onLigneCliqueeMaquette(int row, int column);
    void onStatutChangeMaquette();
    void verifierRetardsMaquette(bool popup);

private:
    void appliquerStyleMaquette();
    void installerIconesMaquette();
    void rafraichirToutMaquette();
    void rafraichirListeMaquette();
    void rafraichirPaginationMaquette(int totalPages);
    void rafraichirStatsMaquette();
    void remplirFormulaireMaquette(const Maquette &m);
    void afficherApercuMaquette(const QString &type, const QString &statut);
    void majBoutonVideoMaquette(const QString &video);
    QString prochainIdMaquette() const;
    QString idCourantMaquette() const;
    Maquette depuisFormulaireMaquette() const;

    QLabel *m_badgeClocheMaquette = nullptr;
    QTimer *m_timerRetardMaquette = nullptr;
    QList<QPixmap> m_miniaturesMaquette;       // images affichables via les 3 miniatures
    QString m_videoCouranteMaquette;
    int  m_pageMaquette = 0;
    bool m_alerteAfficheeMaquette = false;

    static constexpr int PAR_PAGEMaquette = 5;
    static constexpr int JOURS_RETARDMaquette = 7;
private:
    void initMaquette();      // initialisation du module (ancien constructeur)

    // =====================================================================
    //  MODULE MACHINES (page PMachine) — slots copiés du projet « Machines »
    // =====================================================================

    
public:
    

    int  alertCountMachine() const { return m_alertCountMachine; }
    void setSearchTextMachine(const QString &text);        // synchronise avec la recherche de la barre du haut
    void showAlertsMenuMachine(QWidget *anchor);           // menu de la cloche

signals:
    void searchTextChangedMachine(const QString &text);
    void alertCountChangedMachine(int count);
    void notifyMachine(const QString &message, bool error);

private slots:
    void onAddMachine();
    void onModifyMachine();
    void onDeleteMachine();
    void onViewMachine();
    void onResetMachine();
    void onExportPdfMachine();
    void onChoosePhotoMachine();
    void onRowSelectedMachine();
    void onSearchChangedMachine(const QString &text);
    void updateDetectionMachine();

private:
    // initialisation (les widgets eux-mêmes viennent du .ui)
    void setupIconsMachine();
    void setupTableMachine();
    void connectSignalsMachine();

    // données
    void reloadDataMachine();
    void applyFilterMachine();
    void renderPageMachine();
    void buildPagerMachine(int pages);
    void updateChartsMachine();
    void checkAlertsMachine();
    void goToMachineMachine(const QString &id);
    const Machine *findMachineMachine(const QString &id) const;

    // formulaire
    bool readFormMachine(Machine &m, QString *error) const;
    void loadFormMachine(const Machine &m);
    void clearFormMachine();
    void updatePreviewMachine();

    QHBoxLayout *m_pagerMachine = nullptr;

    QList<Machine> m_allMachine;
    QList<Machine> m_viewMachine;
    QString m_currentIdMachine;
    QString m_formPhotoMachine;
    int  m_pageMachine = 0;
    int  m_alertCountMachine = 0;
    bool m_onlyAlertsMachine = false;
    bool m_alertsReadyMachine = false;
    QSet<QString> m_knownUnavailableMachine;
private:
    void initMachine();      // initialisation du module (ancien constructeur)

    // =====================================================================
    //  MODULE EMPLOYÉS (page PEmpl) — slots copiés du projet « Employés »
    // =====================================================================

    
public:
private slots:
    void loginEmploye();
    void logoutEmploye();
    void addEmployeeEmploye();
    void modifyEmployeeEmploye();
    void removeEmployeeEmploye();
    void showEmployeeEmploye();
    void importCVEmploye();
    void rowClickedEmploye(int row,int col);
    void searchChangedEmploye(const QString&);
private:
    void buildUiEmploye();
    void refreshEmploye();
    void updateStatsEmploye();
    void fillFormEmploye(const Employee&);
    int selectedIndexEmploye() const;
    Employee formEmployeeEmploye() const;
    QString readTextEmploye(const QString&);
    void parseCVEmploye(const QString&);
    QList<Employee> m_employeesEmploye;
    QString m_loggedIdEmploye;
    QTableWidget *m_tableEmploye;
    QLineEdit *m_idEmploye,*m_nomEmploye,*m_prenomEmploye,*m_salaryEmploye,*m_passwordEmploye,*m_searchEmploye;
    QComboBox *m_posteEmploye;
    QLabel *m_statusEmploye,*m_totalEmploye,*m_presentEmploye,*m_absentEmploye;
    StatsWidget *m_statsEmploye;
private:
    void initEmploye();      // initialisation du module (ancien constructeur)
};

#endif // SMARTMARKET_H