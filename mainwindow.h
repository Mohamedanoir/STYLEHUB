#ifndef MAINWINDOW_H
#define MAINWINDOW_H

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
#include <QDesktopServices>
#include <QUrl>
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
    void onResetOrderForm();

    // Contrôles de saisie en temps réel
    void onIdTextChanged(const QString &text);
    void onClientCodeTextChanged(const QString &text);
    void onClientNameTextChanged(const QString &text);
    void onAmountTextChanged(const QString &text);
    void onClientPhoneTextChanged(const QString &text);
    void onBtnAutoGenerateIdClicked();

    // Actions WhatsApp et SMS
    void onBtnWhatsAppClicked();
    void onBtnSmsClicked();

    // Cascades 24 Gouvernorats
    void onWilayaChanged(const QString &wilaya);
    void onDelegationChanged(const QString &delegation);

    void toggleChatWindow();
    void handleSendChatMessage();
    void onAiResponseReceived(QNetworkReply *reply);

    void actualiserTableau();
    void onDonutAnimationStep(const QVariant &val);
    void recalculerDelaiLivraisonEstime();

private:
    Commande cTmp;

    // Formulaire de commande
    QFrame *formFrame;
    QLineEdit *formIdEdit;
    QLabel *idErrorLabel;
    QLineEdit *formClientCodeEdit;
    QLabel *clientCodeErrorLabel;
    QLineEdit *formClientNameEdit;
    QLabel *clientNameErrorLabel;
    QLineEdit *formClientPhoneEdit;
    QLabel *clientPhoneErrorLabel;

    // Contrôle du montant
    QLineEdit *formAmountEdit;
    QLabel *amountErrorLabel;

    // Hiérarchie géographique tunisienne
    QComboBox *comboWilaya;
    QComboBox *comboDelegation;
    QComboBox *comboImada;

    QDateEdit *formDateEdit;
    QComboBox *formStatusCombo;
    QComboBox *formArticleCombo;
    QComboBox *formPaymentCombo;
    QLabel *formPreviewImage;
    QLabel *smartDeliveryDateLabel;

    QPushButton *btnAdd;
    QPushButton *btnEdit;
    QPushButton *btnDel;

    // Navigation & Table
    QStackedWidget *pageStack;
    QLineEdit *searchEdit;
    QTableWidget *ordersTable;
    QFrame *tableContainerFrame;

    // Panneau Détails repositionné en bas
    QWidget *detailsPanelContainer;
    QLabel *selectedOrderLabel;
    QLabel *detailProductImage;
    QLabel *detailProdInfo;
    QLabel *detailClient;
    QLabel *detailPhone;
    QLabel *detailDate;
    QLabel *detailStatus;
    QLabel *detailAmount;
    QLabel *detailPayment;
    QLabel *detailAddress;
    QLabel *distanceLabel;
    QLabel *costLabel;
    QLabel *delayLabel;

    // Boutons d'actions flottants
    QPushButton *btnWhatsAppFloating;
    QPushButton *btnSmsFloating;
    QString currentSelectedPhone;

    // Carte & Statistiques
    QWebEngineView *mapView;
    QLabel *donutLabel;
    QVBoxLayout *legendLayout;
    QVariantAnimation *donutAnim;
    QLabel *govBarChartLabel; // Label du graphique en barres réelles
    int animCountLivree;
    int animCountEnCours;
    int animCountPrete;
    int animTotalCmd;

    // Labels KPI
    QLabel *kpiTotalVal;
    QLabel *kpiEnAttenteVal;
    QLabel *kpiExpedieesVal;
    QLabel *kpiAtelierVal;
    QLabel *kpiMontantTotalVal;

    bool sortAscending;
    int currentRowSelected;
    QString selectedClientLat;
    QString selectedClientLon;

    // Chatbot IA
    AnimatedBotButton *chatFloatingBtn;
    QFrame *chatWindow;
    QWidget *chatMessagesArea;
    QVBoxLayout *chatMessagesLayout;
    QLineEdit *chatInput;

    QNetworkAccessManager *networkManager;
    QJsonArray conversationHistory;
    QString getApiKey();

    void initTunisianLocationData();
    QPair<double, double> getCoordinatesFromAddress(const QString &wilaya, const QString &delegation, const QString &imada);
    void calculerTempsTrajet(double latDest, double lonDest);
    void rafraichirMarqueursCarte();
    void recalculerStatistiquesEtDonut();
    void dessinerDonut(double progress);

    bool validateTunisianPhoneNumber(const QString &cleanNumber, QString &operatorName);

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
    void setupFloatingActionButtons();
    void addChatMessage(const QString &sender, const QString &text, bool isUser);
    QString processChatbotQuery(const QString &query);
    void initAIAgent();
};

#endif // MAINWINDOW_H