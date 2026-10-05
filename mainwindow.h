#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableWidget>
#include <QLineEdit>
#include <QComboBox>
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

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override = default;

protected:
    void resizeEvent(QResizeEvent *event) override;

private slots:
    // Navigation & Commandes
    void onSidebarButtonClicked(int id);
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
    // Interface & Composants principaux
    QStackedWidget *pageStack;
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
    QWebEngineView *mapView;
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
    const QString GROQ_API_KEY = "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";

    // Méthodes d'initialisation UI
    QWidget* makeSidebar();
    QWidget* makeHeader();
    QWidget* makeKpis();
    QWidget* makeOrdersPanel();
    QWidget* makeDetailsPanel();
    QWidget* makeMapPanel();
    QWidget* makeBottomPanel();
    QWidget* createEmptyPage(const QString &title);

    // Méthodes Métier & IA
    void setupOrders();
    void updateDetails(int row);
    void sendMapCommand(const QString &js);
    void setupFloatingChatbot();
    void addChatMessage(const QString &sender, const QString &text, bool isUser);
    QString processChatbotQuery(const QString &query);
    void initAIAgent();
};

#endif // MAINWINDOW_H