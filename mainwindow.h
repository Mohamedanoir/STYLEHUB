#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMap>
#include <QPixmap>
#include <QTableWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QDateEdit>
#include <QLabel>
#include <QPushButton>
#include <QStackedWidget>
#include <QFrame>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTextEdit>
#include <QTimer>
#include <QToolButton>
#include <QButtonGroup>
#include <QWidget>
#include <QWebEngineView>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QVariantAnimation>
#include <QDesktopServices>
#include <QUrl>

#include "maquette.h"
#include "commande.h"
#include "AnimatedBotButton.h"

class ApercuMaquette;
class Miniature;
class DonutChart;
class LineChart;
class BarChart;

namespace Ui { class MainWindow; }

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

protected:
    bool eventFilter(QObject *obj, QEvent *ev) override;
    void changeEvent(QEvent *ev) override;   // retour sur la fenêtre -> recharger depuis Oracle
    void resizeEvent(QResizeEvent *event) override;

private slots:
    // ==========================================
    // MODULE MAQUETTES
    // ==========================================
    // CRUD
    void onAjouter();
    void onModifier();
    void onSupprimer();
    void onReinitialiser();

    // Liste
    void rafraichirListe();
    void onLigneCliquee(int ligne, int colonne);
    void onExporterPdf();

    // Image qui change selon Type + Statut
    void mettreAJourImageMaquette();
    // Attributs (mesures) qui changent selon le Type
    void mettreAJourMesures();

    // Métiers innovants Maquettes
    void onChoisirVideo();
    void onBoutonPlus();
    void verifierRetards();
    void onCloche();

    // ==========================================
    // MODULE COMMANDES, CARTE & CHATBOT IA
    // ==========================================
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

    // Chatbot IA
    void toggleChatWindow();
    void handleSendChatMessage();
    void onAiResponseReceived(QNetworkReply *reply);

    void actualiserTableau();
    void onDonutAnimationStep(const QVariant &val);
    void recalculerDelaiLivraisonEstime();

private:
    Ui::MainWindow *ui;

    // ==========================================
    // ATTRIBUTS & MÉTHODES PRIVÉES : MAQUETTES
    // ==========================================
    void placerImageSidebar();          // image du mannequin en bas du menu
    QLabel *m_imageSidebar = nullptr;
    int     m_navShift = 0;             // décalage vers le haut des boutons pendant le défilement

    // Interface (mainwindow.ui) : réglages faits en code (icônes, ombres, connexions)
    void configurerInterface();
    void appliquerStyle();

    void afficherPage(int page);
    int  nbPages() const;                        // nombre de pages du tableau
    void afficherMaquetteDansTableau(const QString &id);   // page de la maquette + ligne surlignée
    void construirePagination(int nbPages);
    void rafraichirStatistiques();
    void remplirFormulaire(const QString &id);
    Maquette lireFormulaire() const;    // lit tous les champs du formulaire
    QMap<QString, double> lireMesuresDuType() const;   // mesures affichées pour le type choisi
    void genererId();                   // champ ID = prochain ID libre (dernier ID + 1)
    void mettreAJourBoutonVideo(const QString &video);
    QString nomImage(const QString &type) const;   // "robe", "veste", "jupe"... ou "" (pas d'image)
    QPixmap apercuPour(const Maquette &m) const;

    // Raccourcis vers les widgets de mainwindow.ui
    QWidget        *m_sidebar = nullptr;
    QStackedWidget *m_pages = nullptr;
    QButtonGroup   *m_navGroupe = nullptr;

    QLineEdit   *m_rechercheGlobale = nullptr;
    QLabel      *m_badgeCloche = nullptr;

    QLineEdit   *m_editId = nullptr;
    QLineEdit   *m_editNom = nullptr;
    QComboBox   *m_comboType = nullptr;
    QDateEdit   *m_dateCreation = nullptr;
    QComboBox   *m_comboStatut = nullptr;
    QLabel      *m_pastilleStatut = nullptr;
    QFrame      *m_fondPastille = nullptr;
    QComboBox   *m_comboTaille = nullptr;
    QLineEdit   *m_editLargeur = nullptr;        // mesures en cm (nombre réel), tous les types
    QLineEdit   *m_editLongueur = nullptr;
    // Mesures qui changent selon le type : colonne Oracle -> champ / ligne (label + champ)
    QMap<QString, QLineEdit *>      m_editMesures;
    QMap<QString, QList<QWidget *>> m_lignesMesures;
    QTextEdit   *m_editDescription = nullptr;
    QLineEdit   *m_editVideo = nullptr;

    ApercuMaquette *m_apercu = nullptr;
    Miniature      *m_mini[3] = {nullptr, nullptr, nullptr};
    QToolButton    *m_boutonPlus = nullptr;

    QLineEdit    *m_rechercheListe = nullptr;
    QComboBox    *m_comboTri = nullptr;
    QTableWidget *m_table = nullptr;
    QHBoxLayout  *m_paginationLayout = nullptr;
    QLabel       *m_labelRetard = nullptr;
    QList<Maquette> m_liste;
    int  m_pageCourante = 1;
    bool m_toutCoche = false;

    DonutChart *m_donut = nullptr;
    LineChart  *m_courbe = nullptr;
    BarChart   *m_barres = nullptr;

    QTimer *m_timerRetard = nullptr;
    bool    m_alerteDejaAffichee = false;

    QMap<QString, QPixmap> m_imgAvant;     // type (robe, veste...) -> toile
    QMap<QString, QPixmap> m_imgValidee;   // type -> 1re image du GIF
    QMap<QString, QPixmap> m_imgValidee2;  // type -> image du milieu du GIF

    static constexpr int JOURS_MAX_VALIDATION = 7;
    static constexpr int LIGNES_PAR_PAGE = 5;

    // ==========================================
    // ATTRIBUTS & MÉTHODES PRIVÉES : COMMANDES
    // ==========================================
    Commande cTmp;

    // Formulaire de commande
    QFrame *formFrame = nullptr;
    QLineEdit *formIdEdit = nullptr;
    QLabel *idErrorLabel = nullptr;
    QLineEdit *formClientCodeEdit = nullptr;
    QLabel *clientCodeErrorLabel = nullptr;
    QLineEdit *formClientNameEdit = nullptr;
    QLabel *clientNameErrorLabel = nullptr;
    QLineEdit *formClientPhoneEdit = nullptr;
    QLabel *clientPhoneErrorLabel = nullptr;

    // Contrôle du montant
    QLineEdit *formAmountEdit = nullptr;
    QLabel *amountErrorLabel = nullptr;

    // Hiérarchie géographique tunisienne
    QComboBox *comboWilaya = nullptr;
    QComboBox *comboDelegation = nullptr;
    QComboBox *comboImada = nullptr;

    QDateEdit *formDateEdit = nullptr;
    QComboBox *formStatusCombo = nullptr;
    QComboBox *formArticleCombo = nullptr;
    QComboBox *formPaymentCombo = nullptr;
    QLabel *formPreviewImage = nullptr;
    QLabel *smartDeliveryDateLabel = nullptr;

    QPushButton *btnAdd = nullptr;
    QPushButton *btnEdit = nullptr;
    QPushButton *btnDel = nullptr;

    // Navigation & Table
    QStackedWidget *pageStack = nullptr;
    QLineEdit *searchEdit = nullptr;
    QTableWidget *ordersTable = nullptr;
    QFrame *tableContainerFrame = nullptr;

    // Panneau Détails repositionné en bas
    QWidget *detailsPanelContainer = nullptr;
    QLabel *selectedOrderLabel = nullptr;
    QLabel *detailProductImage = nullptr;
    QLabel *detailProdInfo = nullptr;
    QLabel *detailClient = nullptr;
    QLabel *detailPhone = nullptr;
    QLabel *detailDate = nullptr;
    QLabel *detailStatus = nullptr;
    QLabel *detailAmount = nullptr;
    QLabel *detailPayment = nullptr;
    QLabel *detailAddress = nullptr;
    QLabel *distanceLabel = nullptr;
    QLabel *costLabel = nullptr;
    QLabel *delayLabel = nullptr;

    // Boutons d'actions flottants
    QPushButton *btnWhatsAppFloating = nullptr;
    QPushButton *btnSmsFloating = nullptr;
    QString currentSelectedPhone;

    // Carte & Statistiques
    QWebEngineView *mapView = nullptr;
    QLabel *donutLabel = nullptr;
    QVBoxLayout *legendLayout = nullptr;
    QVariantAnimation *donutAnim = nullptr;
    QLabel *govBarChartLabel = nullptr;
    int animCountLivree = 0;
    int animCountEnCours = 0;
    int animCountPrete = 0;
    int animTotalCmd = 0;

    // Labels KPI
    QLabel *kpiTotalVal = nullptr;
    QLabel *kpiEnAttenteVal = nullptr;
    QLabel *kpiExpedieesVal = nullptr;
    QLabel *kpiAtelierVal = nullptr;
    QLabel *kpiMontantTotalVal = nullptr;

    bool sortAscending = true;
    int currentRowSelected = -1;
    QString selectedClientLat;
    QString selectedClientLon;

    // Chatbot IA
    AnimatedBotButton *chatFloatingBtn = nullptr;
    QFrame *chatWindow = nullptr;
    QWidget *chatMessagesArea = nullptr;
    QVBoxLayout *chatMessagesLayout = nullptr;
    QLineEdit *chatInput = nullptr;

    QNetworkAccessManager *networkManager = nullptr;
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