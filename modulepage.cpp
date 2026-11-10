#include "modulepage.h"

#include <QComboBox>
#include <QDateEdit>
#include <QDialogButtonBox>
#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QTableWidget>
#include <QTextEdit>
#include <QVBoxLayout>

// ============================================================================
//  FormulaireDialog
// ============================================================================
FormulaireDialog::FormulaireDialog(const QString &titreModule, const QList<ChampForm> &champs,
                                   Mode mode, const QStringList &valeurs, QWidget *parent)
    : QDialog(parent)
{
    const QString action = mode == Ajout ? "Ajouter" : mode == Modification ? "Modifier" : "Afficher";
    setWindowTitle(action + " — " + titreModule);
    setModal(true);
    setMinimumWidth(460);
    setStyleSheet("QDialog { background: #FFFFFF; }"
                  "QLabel { color: #1B2333; font-size: 13px; }"
                  "QLineEdit, QComboBox, QDateEdit, QDoubleSpinBox, QTextEdit {"
                  "  border: 1px solid #D9CDBB; border-radius: 6px; padding: 5px 8px;"
                  "  background: #FFFFFF; color: #1B2333; min-height: 24px; }"
                  "QLineEdit:disabled, QComboBox:disabled, QDateEdit:disabled,"
                  "QDoubleSpinBox:disabled, QTextEdit:disabled { background: #F6F1EA; color: #1B2333; }"
                  "QPushButton { border: none; border-radius: 6px; padding: 8px 22px; font-size: 13px; }");

    auto *racine = new QVBoxLayout(this);
    racine->setContentsMargins(24, 20, 24, 20);
    racine->setSpacing(14);

    auto *titre = new QLabel(action + " — " + titreModule);
    titre->setStyleSheet("font-size: 18px; font-weight: bold; color: #0B2A45;");
    racine->addWidget(titre);

    auto *form = new QFormLayout;
    form->setLabelAlignment(Qt::AlignLeft);
    form->setSpacing(10);

    for (int i = 0; i < champs.size(); ++i) {
        const ChampForm &c = champs[i];
        const QString val = i < valeurs.size() ? valeurs[i] : QString();
        QWidget *w = nullptr;

        switch (c.type) {
        case ChampForm::Choix: {
            auto *cb = new QComboBox;
            cb->addItems(c.choix);
            if (!val.isEmpty()) cb->setCurrentText(val);
            w = cb;
            break;
        }
        case ChampForm::Date: {
            auto *d = new QDateEdit;
            d->setCalendarPopup(true);
            d->setDisplayFormat("dd/MM/yyyy");
            d->setDate(val.isEmpty() ? QDate::currentDate() : QDate::fromString(val, "dd/MM/yyyy"));
            w = d;
            break;
        }
        case ChampForm::Nombre: {
            auto *s = new QDoubleSpinBox;
            s->setRange(0, 1e9);
            s->setDecimals(2);
            s->setValue(val.toDouble());
            w = s;
            break;
        }
        case ChampForm::Description: {
            auto *t = new QTextEdit;
            t->setFixedHeight(70);
            t->setPlainText(val);
            w = t;
            break;
        }
        default: {
            auto *l = new QLineEdit(val);
            l->setPlaceholderText(c.libelle);
            w = l;
        }
        }
        if (mode == Affichage) w->setEnabled(false);
        form->addRow(c.libelle + " :", w);
    }
    racine->addLayout(form);

    auto *boutons = new QHBoxLayout;
    boutons->addStretch();
    auto *annuler = new QPushButton(mode == Affichage ? "Fermer" : "Annuler");
    annuler->setStyleSheet("background: #E7E7E7; color: #1B2333;");
    boutons->addWidget(annuler);
    connect(annuler, &QPushButton::clicked, this, &QDialog::reject);

    if (mode != Affichage) {
        auto *ok = new QPushButton("Enregistrer");
        ok->setStyleSheet("background: #0B2D52; color: white;");
        ok->setDefault(true);
        boutons->addWidget(ok);
        // NON fonctionnel : on ferme simplement la fenêtre, rien n'est sauvegardé
        connect(ok, &QPushButton::clicked, this, &QDialog::accept);
    }
    racine->addLayout(boutons);
}

// ============================================================================
//  ModulePage
// ============================================================================
ModulePage::ModulePage(const QString &titre, const QList<ChampForm> &champs,
                       const QList<QStringList> &exemples, QWidget *parent)
    : QWidget(parent), m_titre(titre), m_champs(champs)
{
    auto *racine = new QVBoxLayout(this);
    racine->setContentsMargins(24, 20, 24, 20);
    racine->setSpacing(14);

    auto *label = new QLabel("Gestion des " + titre);
    label->setStyleSheet("font-size: 22px; font-weight: bold; color: #0B2A45;");
    racine->addWidget(label);

    // ---- Tableau d'exemple (données fictives)
    QStringList entetes;
    for (const ChampForm &c : champs) entetes << c.libelle;
    m_table = new QTableWidget(exemples.size(), entetes.size());
    m_table->setHorizontalHeaderLabels(entetes);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setSelectionMode(QAbstractItemView::SingleSelection);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->verticalHeader()->setVisible(false);
    m_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    for (int r = 0; r < exemples.size(); ++r)
        for (int c = 0; c < entetes.size() && c < exemples[r].size(); ++c)
            m_table->setItem(r, c, new QTableWidgetItem(exemples[r][c]));
    racine->addWidget(m_table, 1);

    // ---- Les 4 boutons (mêmes styles que le module Maquettes)
    auto *barre = new QHBoxLayout;
    barre->setSpacing(10);
    struct B { const char *texte; const char *role; };
    const B defs[4] = {{"Ajouter", "btnPrimaire"}, {"Modifier", "btnGris"},
                       {"Supprimer", "btnBeige"}, {"Afficher", "btnClair"}};
    QPushButton *btns[4];
    for (int i = 0; i < 4; ++i) {
        btns[i] = new QPushButton(defs[i].texte);
        btns[i]->setProperty("role", defs[i].role);
        btns[i]->setCursor(Qt::PointingHandCursor);
        btns[i]->setFixedHeight(42);
        btns[i]->setMinimumWidth(120);
        barre->addWidget(btns[i]);
    }
    barre->addStretch();
    racine->addLayout(barre);

    connect(btns[0], &QPushButton::clicked, this, &ModulePage::onAjouter);
    connect(btns[1], &QPushButton::clicked, this, &ModulePage::onModifier);
    connect(btns[2], &QPushButton::clicked, this, &ModulePage::onSupprimer);
    connect(btns[3], &QPushButton::clicked, this, &ModulePage::onAfficher);
}

QStringList ModulePage::ligneSelectionnee() const
{
    QStringList v;
    const int r = m_table->currentRow();
    if (r < 0) return v;
    for (int c = 0; c < m_table->columnCount(); ++c)
        v << (m_table->item(r, c) ? m_table->item(r, c)->text() : QString());
    return v;
}

void ModulePage::onAjouter()
{
    FormulaireDialog(m_titre, m_champs, FormulaireDialog::Ajout, {}, this).exec();
}

void ModulePage::onModifier()
{
    const QStringList v = ligneSelectionnee();
    if (v.isEmpty()) {
        QMessageBox::information(this, "Modifier", "Sélectionnez d'abord une ligne dans le tableau.");
        return;
    }
    FormulaireDialog(m_titre, m_champs, FormulaireDialog::Modification, v, this).exec();
}

void ModulePage::onSupprimer()
{
    if (ligneSelectionnee().isEmpty()) {
        QMessageBox::information(this, "Supprimer", "Sélectionnez d'abord une ligne dans le tableau.");
        return;
    }
    // NON fonctionnel : seulement la confirmation, la ligne n'est pas supprimée
    QMessageBox::question(this, "Supprimer", "Voulez-vous vraiment supprimer cet élément ?",
                          QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
}

void ModulePage::onAfficher()
{
    const QStringList v = ligneSelectionnee();
    if (v.isEmpty()) {
        QMessageBox::information(this, "Afficher", "Sélectionnez d'abord une ligne dans le tableau.");
        return;
    }
    FormulaireDialog(m_titre, m_champs, FormulaireDialog::Affichage, v, this).exec();
}
