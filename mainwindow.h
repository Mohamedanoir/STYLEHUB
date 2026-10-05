#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QLabel>
#include <QPushButton>
#include <QWebEngineView>
#include <QStackedWidget>
#include <QFrame>
#include <QVBoxLayout>
#include <QResizeEvent>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override = default;

protected:
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void selectOrder(int row);
    void toggleSortOrders();
    void showSelectedRoute();
    void centerMap();
    void onSidebarButtonClicked(int id);
    void toggleChatWindow();
    void handleSendChatMessage();

    // Nouvelles fonctionnalités innovantes
    void generateInvoicePdf();
    void openBarcodeScannerModal();

private:
    // Constructeurs des composants de l'interface
    QWidget* makeSidebar();
    QWidget* makeHeader();
    QWidget* makeKpis();
    QWidget* makeOrdersPanel();
    QWidget* makeMapPanel();
    QWidget* makeDetailsPanel();
    QWidget* makeBottomPanel();
    QWidget* createEmptyPage(const QString &title);

    // Initialisation et interactions
    void setupOrders();
    void updateDetails(int row);
    void sendMapCommand(const QString &js);

    // Moteur Chatbot
    void setupFloatingChatbot();
    void addChatMessage(const QString &sender, const QString &text, bool isUser);
    QString processChatbotQuery(const QString &query);

    // Widgets principaux
    QStackedWidget *pageStack;
    QTableWidget *ordersTable;
    QLineEdit *searchEdit;
    QComboBox *statusCombo;
    QComboBox *periodCombo;
    QWebEngineView *mapView;

    // Volet Détails
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

    // Volet Statistiques du bas
    QLabel *donutLabel;

    // Volet Chatbot flottant
    QPushButton *chatFloatingBtn;
    QFrame *chatWindow;
    QWidget *chatMessagesArea;
    QVBoxLayout *chatMessagesLayout;
    QLineEdit *chatInput;

    // État
    bool sortAscending;
    int currentRowSelected;
    QString selectedClientLat;
    QString selectedClientLon;
};

#endif // MAINWINDOW_H