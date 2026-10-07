/********************************************************************************
** Form generated from reading UI file 'articlespage.ui'
**
** Created by: Qt User Interface Compiler version 6.11.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ARTICLESPAGE_H
#define UI_ARTICLESPAGE_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDateEdit>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "widgets.h"

QT_BEGIN_NAMESPACE

class Ui_ArticlesPage
{
public:
    QVBoxLayout *rightLayout;
    BannerWidget *banner;
    QFrame *body;
    QVBoxLayout *bodyLayout;
    QHBoxLayout *rowLayout;
    QFrame *formPanel;
    QVBoxLayout *formPanelLayout;
    QFrame *formHeader;
    QHBoxLayout *formHeaderLayout;
    QLabel *lblFormTitle;
    QSpacerItem *spcFH;
    QPushButton *btnReset;
    QWidget *formBody;
    QVBoxLayout *formBodyLayout;
    QFormLayout *formLayout;
    QLabel *lbl_eId;
    QLineEdit *eId;
    QLabel *lbl_eNom;
    QLineEdit *eNom;
    QLabel *lbl_cCat;
    QComboBox *cCat;
    QLabel *lbl_sPrix;
    QDoubleSpinBox *sPrix;
    QLabel *lbl_sQte;
    QSpinBox *sQte;
    QLabel *lbl_dDate;
    QDateEdit *dDate;
    QLabel *lbl_cSt;
    QComboBox *cSt;
    QLabel *lbl_sDem;
    QSpinBox *sDem;
    QLabel *lbl_img;
    QHBoxLayout *imgLayout;
    QLabel *lImg;
    QPushButton *btnImg;
    QSpacerItem *spcImg;
    QLabel *lbl_eDesc;
    QTextEdit *eDesc;
    QGridLayout *btnGrid;
    QPushButton *btnAdd;
    QPushButton *btnMod;
    QPushButton *btnDel;
    QPushButton *btnView;
    QSpacerItem *spcForm;
    QFrame *listPanel;
    QVBoxLayout *listPanelLayout;
    QFrame *listHeader;
    QHBoxLayout *listHeaderLayout;
    QLabel *lblListTitle;
    QSpacerItem *spcLH;
    QPushButton *btnPdf;
    QWidget *listBody;
    QVBoxLayout *listBodyLayout;
    QHBoxLayout *toolsLayout;
    QLineEdit *search;
    QLabel *lblSortBy;
    QComboBox *cSort;
    QTableWidget *tb;
    QFrame *statsPanel;
    QVBoxLayout *statsPanelLayout;
    QFrame *statsHeader;
    QHBoxLayout *statsHeaderLayout;
    QLabel *lblStatsTitle;
    QWidget *statsBody;
    QHBoxLayout *statsBodyLayout;
    QFrame *cardDonut;
    QVBoxLayout *cardDonutLayout;
    QLabel *lbl_cardDonut;
    ChartWidget *chDonut;
    QFrame *cardLine;
    QVBoxLayout *cardLineLayout;
    QLabel *lbl_cardLine;
    ChartWidget *chLine;
    QFrame *cardBars;
    QVBoxLayout *cardBarsLayout;
    QLabel *lbl_cardBars;
    ChartWidget *chBars;

    void setupUi(QWidget *ArticlesPage)
    {
        if (ArticlesPage->objectName().isEmpty())
            ArticlesPage->setObjectName("ArticlesPage");
        ArticlesPage->resize(1200, 800);
        ArticlesPage->setStyleSheet(QString::fromUtf8("QWidget#ArticlesPage{background:#f7f1e8}\n"
"#body{background:#f7f1e8}\n"
"QLabel{color:#1d2a3a;background:transparent}\n"
"QLineEdit{border:1px solid #e6dccd;border-radius:8px;padding:8px 10px;background:white;color:#1d2a3a;selection-background-color:#d9a566;selection-color:#1d2a3a}\n"
"#formPanel,#listPanel,#statsPanel{background:white;border-radius:12px}\n"
"#formHeader,#listHeader,#statsHeader{background:#f1e4d3;border-top-left-radius:12px;border-top-right-radius:12px}\n"
"#lblFormTitle,#lblListTitle,#lblSortBy,#lblStatsTitle{font:600 13pt Georgia;background:transparent}\n"
"#cardDonut,#cardLine,#cardBars{background:white;border:1px solid #efe6d8;border-radius:10px}\n"
"QComboBox,QDoubleSpinBox,QSpinBox,QDateEdit,QTextEdit{border:1px solid #e6dccd;border-radius:8px;padding:6px 8px;background:white;color:#1d2a3a;selection-background-color:#d9a566;selection-color:#1d2a3a}\n"
"QComboBox QAbstractItemView{background:white;color:#1d2a3a;selection-background-color:#f1e4d3;selection-color:#1d2a3a;border:1px solid"
                        " #e6dccd}\n"
"QCalendarWidget QWidget{background:white;color:#1d2a3a}\n"
"QCalendarWidget QAbstractItemView{selection-background-color:#d9a566;selection-color:#1d2a3a}\n"
"#btnAdd,#btnPdf{background:#0f2d55;color:white;border-radius:8px;font-weight:600;padding:0 14px}\n"
"#btnMod{background:#8993a2;color:white;border-radius:8px;font-weight:600}\n"
"#btnDel{background:#f1e4d3;color:#5b3b18;border-radius:8px;font-weight:600}\n"
"#btnView{background:#e4e7ec;color:#0f2d55;border-radius:8px;font-weight:600}\n"
"#btnImg{background:#e4e7ec;color:#0f2d55;border-radius:8px;font-weight:600;padding:6px 12px}\n"
"#lImg{background:#fbf6ee;border:1px dashed #d9c3a3;border-radius:8px;color:#9aa3b0}\n"
"#btnReset{background:transparent;color:#0f2d55;border:0;font-weight:600}\n"
"QHeaderView::section{background:#f1e4d3;color:#1d2a3a;border:0;padding:8px;font-weight:600}\n"
"QTableWidget{background:white;alternate-background-color:#fbf6ee;color:#1d2a3a;border:0;gridline-color:#efe6d8;selection-background-color:#f6e7d0;selection"
                        "-color:#1d2a3a}\n"
"QTableWidget::item{padding:4px 6px}\n"
"QTableCornerButton::section{background:#f1e4d3;border:0}\n"
"QScrollBar:vertical{background:#f7f1e8;width:10px;border-radius:5px}\n"
"QScrollBar::handle:vertical{background:#d9c3a3;border-radius:5px;min-height:24px}\n"
"QScrollBar::add-line,QScrollBar::sub-line{height:0;width:0}\n"
"QMessageBox{background:white}\n"
"QMessageBox QLabel{color:#1d2a3a}\n"
"QMessageBox QPushButton{background:#0f2d55;color:white;border-radius:6px;padding:6px 16px;min-width:70px}\n"
""));
        rightLayout = new QVBoxLayout(ArticlesPage);
        rightLayout->setSpacing(0);
        rightLayout->setObjectName("rightLayout");
        rightLayout->setContentsMargins(0, 0, 0, 0);
        banner = new BannerWidget(ArticlesPage);
        banner->setObjectName("banner");

        rightLayout->addWidget(banner);

        body = new QFrame(ArticlesPage);
        body->setObjectName("body");
        bodyLayout = new QVBoxLayout(body);
        bodyLayout->setSpacing(12);
        bodyLayout->setObjectName("bodyLayout");
        bodyLayout->setContentsMargins(16, 12, 16, 12);
        rowLayout = new QHBoxLayout();
        rowLayout->setSpacing(14);
        rowLayout->setObjectName("rowLayout");
        formPanel = new QFrame(body);
        formPanel->setObjectName("formPanel");
        formPanelLayout = new QVBoxLayout(formPanel);
        formPanelLayout->setSpacing(0);
        formPanelLayout->setObjectName("formPanelLayout");
        formPanelLayout->setContentsMargins(0, 0, 0, 0);
        formHeader = new QFrame(formPanel);
        formHeader->setObjectName("formHeader");
        formHeaderLayout = new QHBoxLayout(formHeader);
        formHeaderLayout->setObjectName("formHeaderLayout");
        formHeaderLayout->setContentsMargins(14, 4, 14, 4);
        lblFormTitle = new QLabel(formHeader);
        lblFormTitle->setObjectName("lblFormTitle");

        formHeaderLayout->addWidget(lblFormTitle);

        spcFH = new QSpacerItem(20, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        formHeaderLayout->addItem(spcFH);

        btnReset = new QPushButton(formHeader);
        btnReset->setObjectName("btnReset");
        btnReset->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));
        btnReset->setMinimumSize(QSize(0, 34));

        formHeaderLayout->addWidget(btnReset);


        formPanelLayout->addWidget(formHeader);

        formBody = new QWidget(formPanel);
        formBody->setObjectName("formBody");
        formBodyLayout = new QVBoxLayout(formBody);
        formBodyLayout->setObjectName("formBodyLayout");
        formBodyLayout->setContentsMargins(16, 10, 16, 10);
        formLayout = new QFormLayout();
        formLayout->setObjectName("formLayout");
        formLayout->setVerticalSpacing(6);
        lbl_eId = new QLabel(formBody);
        lbl_eId->setObjectName("lbl_eId");

        formLayout->setWidget(0, QFormLayout::ItemRole::LabelRole, lbl_eId);

        eId = new QLineEdit(formBody);
        eId->setObjectName("eId");

        formLayout->setWidget(0, QFormLayout::ItemRole::FieldRole, eId);

        lbl_eNom = new QLabel(formBody);
        lbl_eNom->setObjectName("lbl_eNom");

        formLayout->setWidget(1, QFormLayout::ItemRole::LabelRole, lbl_eNom);

        eNom = new QLineEdit(formBody);
        eNom->setObjectName("eNom");

        formLayout->setWidget(1, QFormLayout::ItemRole::FieldRole, eNom);

        lbl_cCat = new QLabel(formBody);
        lbl_cCat->setObjectName("lbl_cCat");

        formLayout->setWidget(2, QFormLayout::ItemRole::LabelRole, lbl_cCat);

        cCat = new QComboBox(formBody);
        cCat->addItem(QString());
        cCat->addItem(QString());
        cCat->addItem(QString());
        cCat->addItem(QString());
        cCat->addItem(QString());
        cCat->setObjectName("cCat");

        formLayout->setWidget(2, QFormLayout::ItemRole::FieldRole, cCat);

        lbl_sPrix = new QLabel(formBody);
        lbl_sPrix->setObjectName("lbl_sPrix");

        formLayout->setWidget(3, QFormLayout::ItemRole::LabelRole, lbl_sPrix);

        sPrix = new QDoubleSpinBox(formBody);
        sPrix->setObjectName("sPrix");
        sPrix->setDecimals(3);
        sPrix->setMaximum(999999.000000000000000);

        formLayout->setWidget(3, QFormLayout::ItemRole::FieldRole, sPrix);

        lbl_sQte = new QLabel(formBody);
        lbl_sQte->setObjectName("lbl_sQte");

        formLayout->setWidget(4, QFormLayout::ItemRole::LabelRole, lbl_sQte);

        sQte = new QSpinBox(formBody);
        sQte->setObjectName("sQte");
        sQte->setMaximum(9999999);

        formLayout->setWidget(4, QFormLayout::ItemRole::FieldRole, sQte);

        lbl_dDate = new QLabel(formBody);
        lbl_dDate->setObjectName("lbl_dDate");

        formLayout->setWidget(5, QFormLayout::ItemRole::LabelRole, lbl_dDate);

        dDate = new QDateEdit(formBody);
        dDate->setObjectName("dDate");
        dDate->setCalendarPopup(true);

        formLayout->setWidget(5, QFormLayout::ItemRole::FieldRole, dDate);

        lbl_cSt = new QLabel(formBody);
        lbl_cSt->setObjectName("lbl_cSt");

        formLayout->setWidget(6, QFormLayout::ItemRole::LabelRole, lbl_cSt);

        cSt = new QComboBox(formBody);
        cSt->addItem(QString());
        cSt->addItem(QString());
        cSt->addItem(QString());
        cSt->setObjectName("cSt");

        formLayout->setWidget(6, QFormLayout::ItemRole::FieldRole, cSt);

        lbl_sDem = new QLabel(formBody);
        lbl_sDem->setObjectName("lbl_sDem");

        formLayout->setWidget(7, QFormLayout::ItemRole::LabelRole, lbl_sDem);

        sDem = new QSpinBox(formBody);
        sDem->setObjectName("sDem");
        sDem->setMaximum(9999999);

        formLayout->setWidget(7, QFormLayout::ItemRole::FieldRole, sDem);

        lbl_img = new QLabel(formBody);
        lbl_img->setObjectName("lbl_img");

        formLayout->setWidget(8, QFormLayout::ItemRole::LabelRole, lbl_img);

        imgLayout = new QHBoxLayout();
        imgLayout->setObjectName("imgLayout");
        lImg = new QLabel(formBody);
        lImg->setObjectName("lImg");
        lImg->setMinimumSize(QSize(64, 64));
        lImg->setMaximumSize(QSize(64, 64));
        lImg->setAlignment(Qt::AlignCenter);

        imgLayout->addWidget(lImg);

        btnImg = new QPushButton(formBody);
        btnImg->setObjectName("btnImg");
        btnImg->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));

        imgLayout->addWidget(btnImg);

        spcImg = new QSpacerItem(20, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        imgLayout->addItem(spcImg);


        formLayout->setLayout(8, QFormLayout::ItemRole::FieldRole, imgLayout);

        lbl_eDesc = new QLabel(formBody);
        lbl_eDesc->setObjectName("lbl_eDesc");

        formLayout->setWidget(9, QFormLayout::ItemRole::LabelRole, lbl_eDesc);

        eDesc = new QTextEdit(formBody);
        eDesc->setObjectName("eDesc");
        eDesc->setMaximumSize(QSize(16777215, 56));

        formLayout->setWidget(9, QFormLayout::ItemRole::FieldRole, eDesc);


        formBodyLayout->addLayout(formLayout);

        btnGrid = new QGridLayout();
        btnGrid->setObjectName("btnGrid");
        btnAdd = new QPushButton(formBody);
        btnAdd->setObjectName("btnAdd");
        btnAdd->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));
        btnAdd->setMinimumSize(QSize(0, 34));

        btnGrid->addWidget(btnAdd, 0, 0, 1, 1);

        btnMod = new QPushButton(formBody);
        btnMod->setObjectName("btnMod");
        btnMod->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));
        btnMod->setMinimumSize(QSize(0, 34));

        btnGrid->addWidget(btnMod, 0, 1, 1, 1);

        btnDel = new QPushButton(formBody);
        btnDel->setObjectName("btnDel");
        btnDel->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));
        btnDel->setMinimumSize(QSize(0, 34));

        btnGrid->addWidget(btnDel, 0, 2, 1, 1);

        btnView = new QPushButton(formBody);
        btnView->setObjectName("btnView");
        btnView->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));
        btnView->setMinimumSize(QSize(0, 34));

        btnGrid->addWidget(btnView, 1, 1, 1, 1);


        formBodyLayout->addLayout(btnGrid);

        spcForm = new QSpacerItem(20, 10, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        formBodyLayout->addItem(spcForm);


        formPanelLayout->addWidget(formBody);


        rowLayout->addWidget(formPanel);

        listPanel = new QFrame(body);
        listPanel->setObjectName("listPanel");
        listPanelLayout = new QVBoxLayout(listPanel);
        listPanelLayout->setSpacing(0);
        listPanelLayout->setObjectName("listPanelLayout");
        listPanelLayout->setContentsMargins(0, 0, 0, 0);
        listHeader = new QFrame(listPanel);
        listHeader->setObjectName("listHeader");
        listHeaderLayout = new QHBoxLayout(listHeader);
        listHeaderLayout->setObjectName("listHeaderLayout");
        listHeaderLayout->setContentsMargins(14, 4, 14, 4);
        lblListTitle = new QLabel(listHeader);
        lblListTitle->setObjectName("lblListTitle");

        listHeaderLayout->addWidget(lblListTitle);

        spcLH = new QSpacerItem(20, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        listHeaderLayout->addItem(spcLH);

        btnPdf = new QPushButton(listHeader);
        btnPdf->setObjectName("btnPdf");
        btnPdf->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));
        btnPdf->setMinimumSize(QSize(0, 34));

        listHeaderLayout->addWidget(btnPdf);


        listPanelLayout->addWidget(listHeader);

        listBody = new QWidget(listPanel);
        listBody->setObjectName("listBody");
        listBodyLayout = new QVBoxLayout(listBody);
        listBodyLayout->setObjectName("listBodyLayout");
        listBodyLayout->setContentsMargins(16, 10, 16, 10);
        toolsLayout = new QHBoxLayout();
        toolsLayout->setObjectName("toolsLayout");
        search = new QLineEdit(listBody);
        search->setObjectName("search");

        toolsLayout->addWidget(search);

        lblSortBy = new QLabel(listBody);
        lblSortBy->setObjectName("lblSortBy");

        toolsLayout->addWidget(lblSortBy);

        cSort = new QComboBox(listBody);
        cSort->addItem(QString());
        cSort->addItem(QString());
        cSort->addItem(QString());
        cSort->addItem(QString());
        cSort->addItem(QString());
        cSort->addItem(QString());
        cSort->addItem(QString());
        cSort->setObjectName("cSort");

        toolsLayout->addWidget(cSort);

        toolsLayout->setStretch(0, 1);
        toolsLayout->setStretch(2, 1);

        listBodyLayout->addLayout(toolsLayout);

        tb = new QTableWidget(listBody);
        if (tb->columnCount() < 8)
            tb->setColumnCount(8);
        QTableWidgetItem *__qtablewidgetitem = new QTableWidgetItem();
        tb->setHorizontalHeaderItem(0, __qtablewidgetitem);
        QTableWidgetItem *__qtablewidgetitem1 = new QTableWidgetItem();
        tb->setHorizontalHeaderItem(1, __qtablewidgetitem1);
        QTableWidgetItem *__qtablewidgetitem2 = new QTableWidgetItem();
        tb->setHorizontalHeaderItem(2, __qtablewidgetitem2);
        QTableWidgetItem *__qtablewidgetitem3 = new QTableWidgetItem();
        tb->setHorizontalHeaderItem(3, __qtablewidgetitem3);
        QTableWidgetItem *__qtablewidgetitem4 = new QTableWidgetItem();
        tb->setHorizontalHeaderItem(4, __qtablewidgetitem4);
        QTableWidgetItem *__qtablewidgetitem5 = new QTableWidgetItem();
        tb->setHorizontalHeaderItem(5, __qtablewidgetitem5);
        QTableWidgetItem *__qtablewidgetitem6 = new QTableWidgetItem();
        tb->setHorizontalHeaderItem(6, __qtablewidgetitem6);
        QTableWidgetItem *__qtablewidgetitem7 = new QTableWidgetItem();
        tb->setHorizontalHeaderItem(7, __qtablewidgetitem7);
        tb->setObjectName("tb");
        tb->setEditTriggers(QAbstractItemView::NoEditTriggers);
        tb->setAlternatingRowColors(true);
        tb->setSelectionMode(QAbstractItemView::SingleSelection);
        tb->setSelectionBehavior(QAbstractItemView::SelectRows);
        tb->setShowGrid(false);
        tb->setWordWrap(false);
        tb->setFrameShape(QFrame::NoFrame);
        tb->horizontalHeader()->setStretchLastSection(true);
        tb->verticalHeader()->setVisible(false);
        tb->verticalHeader()->setDefaultSectionSize(54);

        listBodyLayout->addWidget(tb);


        listPanelLayout->addWidget(listBody);


        rowLayout->addWidget(listPanel);

        rowLayout->setStretch(0, 5);
        rowLayout->setStretch(1, 6);

        bodyLayout->addLayout(rowLayout);

        statsPanel = new QFrame(body);
        statsPanel->setObjectName("statsPanel");
        statsPanel->setMinimumSize(QSize(0, 200));
        statsPanel->setMaximumSize(QSize(16777215, 200));
        statsPanelLayout = new QVBoxLayout(statsPanel);
        statsPanelLayout->setSpacing(0);
        statsPanelLayout->setObjectName("statsPanelLayout");
        statsPanelLayout->setContentsMargins(0, 0, 0, 0);
        statsHeader = new QFrame(statsPanel);
        statsHeader->setObjectName("statsHeader");
        statsHeaderLayout = new QHBoxLayout(statsHeader);
        statsHeaderLayout->setObjectName("statsHeaderLayout");
        statsHeaderLayout->setContentsMargins(14, 4, 14, 4);
        lblStatsTitle = new QLabel(statsHeader);
        lblStatsTitle->setObjectName("lblStatsTitle");

        statsHeaderLayout->addWidget(lblStatsTitle);


        statsPanelLayout->addWidget(statsHeader);

        statsBody = new QWidget(statsPanel);
        statsBody->setObjectName("statsBody");
        statsBodyLayout = new QHBoxLayout(statsBody);
        statsBodyLayout->setSpacing(12);
        statsBodyLayout->setObjectName("statsBodyLayout");
        statsBodyLayout->setContentsMargins(16, 10, 16, 10);
        cardDonut = new QFrame(statsBody);
        cardDonut->setObjectName("cardDonut");
        cardDonutLayout = new QVBoxLayout(cardDonut);
        cardDonutLayout->setSpacing(2);
        cardDonutLayout->setObjectName("cardDonutLayout");
        cardDonutLayout->setContentsMargins(12, 8, 12, 8);
        lbl_cardDonut = new QLabel(cardDonut);
        lbl_cardDonut->setObjectName("lbl_cardDonut");
        lbl_cardDonut->setStyleSheet(QString::fromUtf8("font-weight:bold;"));

        cardDonutLayout->addWidget(lbl_cardDonut);

        chDonut = new ChartWidget(cardDonut);
        chDonut->setObjectName("chDonut");

        cardDonutLayout->addWidget(chDonut);


        statsBodyLayout->addWidget(cardDonut);

        cardLine = new QFrame(statsBody);
        cardLine->setObjectName("cardLine");
        cardLineLayout = new QVBoxLayout(cardLine);
        cardLineLayout->setSpacing(2);
        cardLineLayout->setObjectName("cardLineLayout");
        cardLineLayout->setContentsMargins(12, 8, 12, 8);
        lbl_cardLine = new QLabel(cardLine);
        lbl_cardLine->setObjectName("lbl_cardLine");
        lbl_cardLine->setStyleSheet(QString::fromUtf8("font-weight:bold;"));

        cardLineLayout->addWidget(lbl_cardLine);

        chLine = new ChartWidget(cardLine);
        chLine->setObjectName("chLine");

        cardLineLayout->addWidget(chLine);


        statsBodyLayout->addWidget(cardLine);

        cardBars = new QFrame(statsBody);
        cardBars->setObjectName("cardBars");
        cardBarsLayout = new QVBoxLayout(cardBars);
        cardBarsLayout->setSpacing(2);
        cardBarsLayout->setObjectName("cardBarsLayout");
        cardBarsLayout->setContentsMargins(12, 8, 12, 8);
        lbl_cardBars = new QLabel(cardBars);
        lbl_cardBars->setObjectName("lbl_cardBars");
        lbl_cardBars->setStyleSheet(QString::fromUtf8("font-weight:bold;"));

        cardBarsLayout->addWidget(lbl_cardBars);

        chBars = new ChartWidget(cardBars);
        chBars->setObjectName("chBars");

        cardBarsLayout->addWidget(chBars);


        statsBodyLayout->addWidget(cardBars);


        statsPanelLayout->addWidget(statsBody);


        bodyLayout->addWidget(statsPanel);

        bodyLayout->setStretch(0, 1);

        rightLayout->addWidget(body);

        rightLayout->setStretch(1, 1);

        retranslateUi(ArticlesPage);

        QMetaObject::connectSlotsByName(ArticlesPage);
    } // setupUi

    void retranslateUi(QWidget *ArticlesPage)
    {
        lblFormTitle->setText(QCoreApplication::translate("ArticlesPage", "Informations de l'article", nullptr));
        btnReset->setText(QCoreApplication::translate("ArticlesPage", "\342\206\273 R\303\251initialiser", nullptr));
        lbl_eId->setText(QCoreApplication::translate("ArticlesPage", "ID Article", nullptr));
        eId->setPlaceholderText(QCoreApplication::translate("ArticlesPage", "ART001", nullptr));
        lbl_eNom->setText(QCoreApplication::translate("ArticlesPage", "Nom", nullptr));
        lbl_cCat->setText(QCoreApplication::translate("ArticlesPage", "Cat\303\251gorie", nullptr));
        cCat->setItemText(0, QCoreApplication::translate("ArticlesPage", "Tissus", nullptr));
        cCat->setItemText(1, QCoreApplication::translate("ArticlesPage", "Fils", nullptr));
        cCat->setItemText(2, QCoreApplication::translate("ArticlesPage", "Boutons", nullptr));
        cCat->setItemText(3, QCoreApplication::translate("ArticlesPage", "Fermetures", nullptr));
        cCat->setItemText(4, QCoreApplication::translate("ArticlesPage", "Accessoires", nullptr));

        lbl_sPrix->setText(QCoreApplication::translate("ArticlesPage", "Prix", nullptr));
        sPrix->setSuffix(QCoreApplication::translate("ArticlesPage", " DT", nullptr));
        lbl_sQte->setText(QCoreApplication::translate("ArticlesPage", "Quantit\303\251 en stock", nullptr));
        lbl_dDate->setText(QCoreApplication::translate("ArticlesPage", "Date d'ajout", nullptr));
        dDate->setDisplayFormat(QCoreApplication::translate("ArticlesPage", "dd/MM/yyyy", nullptr));
        lbl_cSt->setText(QCoreApplication::translate("ArticlesPage", "Statut", nullptr));
        cSt->setItemText(0, QCoreApplication::translate("ArticlesPage", "Disponible", nullptr));
        cSt->setItemText(1, QCoreApplication::translate("ArticlesPage", "Rupture de stock", nullptr));
        cSt->setItemText(2, QCoreApplication::translate("ArticlesPage", "Archiv\303\251", nullptr));

        lbl_sDem->setText(QCoreApplication::translate("ArticlesPage", "Demande", nullptr));
        sDem->setSuffix(QCoreApplication::translate("ArticlesPage", " demandes", nullptr));
        lbl_img->setText(QCoreApplication::translate("ArticlesPage", "Image", nullptr));
        lImg->setText(QCoreApplication::translate("ArticlesPage", "Aucune\n"
"image", nullptr));
        btnImg->setText(QCoreApplication::translate("ArticlesPage", "\360\237\226\274 Choisir une image...", nullptr));
        lbl_eDesc->setText(QCoreApplication::translate("ArticlesPage", "Description", nullptr));
        btnAdd->setText(QCoreApplication::translate("ArticlesPage", "\357\274\213 Ajouter", nullptr));
        btnMod->setText(QCoreApplication::translate("ArticlesPage", "\342\234\216 Modifier", nullptr));
        btnDel->setText(QCoreApplication::translate("ArticlesPage", "\360\237\227\221 Supprimer", nullptr));
        btnView->setText(QCoreApplication::translate("ArticlesPage", "\360\237\221\201 Afficher", nullptr));
        lblListTitle->setText(QCoreApplication::translate("ArticlesPage", "Liste des articles", nullptr));
        btnPdf->setText(QCoreApplication::translate("ArticlesPage", "\360\237\223\204 Exporter PDF", nullptr));
        search->setPlaceholderText(QCoreApplication::translate("ArticlesPage", "Rechercher un article...", nullptr));
        lblSortBy->setText(QCoreApplication::translate("ArticlesPage", "Trier par :", nullptr));
        cSort->setItemText(0, QCoreApplication::translate("ArticlesPage", "Choisir un crit\303\250re", nullptr));
        cSort->setItemText(1, QCoreApplication::translate("ArticlesPage", "Nom", nullptr));
        cSort->setItemText(2, QCoreApplication::translate("ArticlesPage", "Cat\303\251gorie", nullptr));
        cSort->setItemText(3, QCoreApplication::translate("ArticlesPage", "Prix", nullptr));
        cSort->setItemText(4, QCoreApplication::translate("ArticlesPage", "Stock", nullptr));
        cSort->setItemText(5, QCoreApplication::translate("ArticlesPage", "Date d'ajout", nullptr));
        cSort->setItemText(6, QCoreApplication::translate("ArticlesPage", "Demande (forte \342\206\222 faible)", nullptr));

        QTableWidgetItem *___qtablewidgetitem = tb->horizontalHeaderItem(0);
        ___qtablewidgetitem->setText(QCoreApplication::translate("ArticlesPage", "Image", nullptr));
        QTableWidgetItem *___qtablewidgetitem1 = tb->horizontalHeaderItem(1);
        ___qtablewidgetitem1->setText(QCoreApplication::translate("ArticlesPage", "ID", nullptr));
        QTableWidgetItem *___qtablewidgetitem2 = tb->horizontalHeaderItem(2);
        ___qtablewidgetitem2->setText(QCoreApplication::translate("ArticlesPage", "Nom", nullptr));
        QTableWidgetItem *___qtablewidgetitem3 = tb->horizontalHeaderItem(3);
        ___qtablewidgetitem3->setText(QCoreApplication::translate("ArticlesPage", "Cat\303\251gorie", nullptr));
        QTableWidgetItem *___qtablewidgetitem4 = tb->horizontalHeaderItem(4);
        ___qtablewidgetitem4->setText(QCoreApplication::translate("ArticlesPage", "Prix", nullptr));
        QTableWidgetItem *___qtablewidgetitem5 = tb->horizontalHeaderItem(5);
        ___qtablewidgetitem5->setText(QCoreApplication::translate("ArticlesPage", "Stock", nullptr));
        QTableWidgetItem *___qtablewidgetitem6 = tb->horizontalHeaderItem(6);
        ___qtablewidgetitem6->setText(QCoreApplication::translate("ArticlesPage", "Demande", nullptr));
        QTableWidgetItem *___qtablewidgetitem7 = tb->horizontalHeaderItem(7);
        ___qtablewidgetitem7->setText(QCoreApplication::translate("ArticlesPage", "Statut", nullptr));
        lblStatsTitle->setText(QCoreApplication::translate("ArticlesPage", "Statistiques des articles", nullptr));
        lbl_cardDonut->setText(QCoreApplication::translate("ArticlesPage", "R\303\251partition par cat\303\251gorie", nullptr));
        lbl_cardLine->setText(QCoreApplication::translate("ArticlesPage", "\303\211volution des ajouts d'articles", nullptr));
        lbl_cardBars->setText(QCoreApplication::translate("ArticlesPage", "Top 5 des stocks", nullptr));
        (void)ArticlesPage;
    } // retranslateUi

};

namespace Ui {
    class ArticlesPage: public Ui_ArticlesPage {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ARTICLESPAGE_H
