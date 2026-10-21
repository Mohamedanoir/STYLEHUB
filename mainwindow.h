#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <cstdio>
#include <QMainWindow>
#include <QTableWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QDateEdit>
#include <QLabel>
#include <QPushButton>
#include <QStackedWidget>
#include <QFrame>
#include <QVBoxLayout>
#include <QWebEngineView>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QVariantAnimation>
#include "commande.h"
#include "AnimatedBotButton.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override = default;

protected:
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void onSidebarButtonClicked(int id);
    void toggleSortOrders();
    void selectOrder(int row);
    void generateInvoicePdf();
    void centerMap();
    void showSelectedRoute();

    void onBtnAddClicked();
    void onBtnEditClicked();
    void onBtnDeleteClicked();
    void onSaveOrderForm();
    void onResetOrderForm();

    void toggleChatWindow();
    void handleSendChatMessage();
    void onAiResponseReceived(QNetworkReply *reply);

    // Synchronisation Oracle et animations
    void actualiserTableau();
    void onDonutAnimationStep(const QVariant &val);

    // Recalcul automatique du délai estimé
    void recalculerDelaiLivraisonEstime();

private:
    Commande cTmp;

    // Formulaire de commande (côté gauche)
    QFrame *formFrame;
    QLineEdit *formIdEdit;
    QLineEdit *formClientCodeEdit;
    QLineEdit *formClientNameEdit;
    QLineEdit *formAddressEdit;
    QDateEdit *formDateEdit;
    QComboBox *formStatusCombo;
    QLineEdit *formAmountEdit;
    QLabel *formPreviewImage;
    QLabel *smartDeliveryDateLabel;
    QPushButton *btnAdd; // Devient "💾 Enregistrer" en mode édition
    bool isEditingMode;
    int editingRow;

    // Navigation & Table (côté droit)
    QStackedWidget *pageStack;
    QLineEdit *searchEdit;
    QTableWidget *ordersTable;
    QComboBox *statusCombo;
    QComboBox *periodCombo;

    // Panneau Détails
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

    // Carte & Donut animé
    QWebEngineView *mapView;
    QLabel *donutLabel;
    QVBoxLayout *legendLayout;
    QVariantAnimation *donutAnim;
    int animCountLivree;
    int animCountEnCours;
    int animCountPrete;
    int animTotalCmd;

    // Labels KPI dynamiques
    QLabel *kpiTotalVal;
    QLabel *kpiEnAttenteVal;
    QLabel *kpiExpedieesVal;
    QLabel *kpiAtelierVal;
    QLabel *kpiMontantTotalVal;

    bool sortAscending;
    int currentRowSelected;
    QString selectedClientLat;
    QString selectedClientLon;

    // Chatbot IA (Bouton flottant animé)
    AnimatedBotButton *chatFloatingBtn;
    QFrame *chatWindow;
    QWidget *chatMessagesArea;
    QVBoxLayout *chatMessagesLayout;
    QLineEdit *chatInput;

    QNetworkAccessManager *networkManager;
    QJsonArray conversationHistory;
    QString getApiKey();

    // Géocodage et calcul multimodal
    QPair<double, double> geolocaliserVille(const QString &nomVille);
    void calculerTempsTrajet(double latDest, double lonDest);
    void rafraichirMarqueursCarte();
    void recalculerStatistiquesEtDonut();
    void dessinerDonut(double progress);

    // Constructeurs d'interface
    QWidget* makeSidebar();
    QWidget* makeHeader();
    QWidget* makeOrdersAndFormCombinedPanel();
    QWidget* makeDetailsPanel();
    QWidget* makeMapPanel();
    QWidget* makeKpis();
    QWidget* makeBottomPanel();
    QWidget* createEmptyPage(const QString &title);

    void setupOrders();
    void updateDetails(int row);
    void sendMapCommand(const QString &js);
    void setupFloatingChatbot();
    void addChatMessage(const QString &sender, const QString &text, bool isUser);
    QString processChatbotQuery(const QString &query);
    void initAIAgent();
};

#endif // MAINWINDOW_H