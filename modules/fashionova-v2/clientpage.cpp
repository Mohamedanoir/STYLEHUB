#include "clientpage.h"

#include <QAbstractItemView>
#include <QColor>
#include <QComboBox>
#include <QDate>
#include <QDateEdit>
#include <QDialog>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QRegularExpression>
#include <QTableWidget>
#include <QTextEdit>
#include <QVBoxLayout>

namespace {
const QColor Navy("#0B2A45");
const QColor Gold("#C59A62");

QString prochainId(const QVector<Client> &clients)
{
    static const QRegularExpression re(QStringLiteral("^CL-(\\d{3})$"));
    int maximum = 0;
    for (const Client &client : clients) {
        const auto match = re.match(client.id);
        if (match.hasMatch()) maximum = qMax(maximum, match.captured(1).toInt());
    }
    if (maximum >= 999) return QString();
    return QStringLiteral("CL-%1").arg(maximum + 1, 3, 10, QLatin1Char('0'));
}

class ClientDialog : public QDialog
{
public:
    ClientDialog(const Client &client, bool edition, const QVector<Client> &clients,
                 QWidget *parent = nullptr)
        : QDialog(parent), m_client(client), m_edition(edition), m_clients(clients)
    {
        setWindowTitle(edition ? QString::fromUtf8("Modifier un client")
                               : QString::fromUtf8("Ajouter un client"));
        setMinimumWidth(500);

        auto *layout = new QVBoxLayout(this);
        auto *form = new QFormLayout;
        m_id = new QLineEdit(edition ? client.id : prochainId(clients));
        m_nom = new QLineEdit(client.nom);
        m_prenom = new QLineEdit(client.prenom);
        m_tel = new QLineEdit(client.tel);
        m_email = new QLineEdit(client.email);
        m_adresse = new QLineEdit(client.adresse);
        m_type = new QComboBox;
        m_type->addItems({"Standard", "VIP", "Premium"});
        if (!client.type.isEmpty()) m_type->setCurrentText(client.type);
        m_date = new QDateEdit;
        m_date->setCalendarPopup(true);
        m_date->setDisplayFormat("dd/MM/yyyy");
        const QDate parsed = QDate::fromString(client.date, "dd/MM/yyyy");
        m_date->setDate(parsed.isValid() ? parsed : QDate::currentDate());
        m_remarques = new QTextEdit(client.remarques);
        m_remarques->setMaximumHeight(80);

        m_id->setPlaceholderText("CL-001");
        m_tel->setPlaceholderText("+216 12 345 678");
        m_adresse->setMaxLength(200);
        m_id->setEnabled(!edition);
        form->addRow("Identifiant :", m_id);
        form->addRow("Nom :", m_nom);
        form->addRow(QString::fromUtf8("Prénom :"), m_prenom);
        form->addRow(QString::fromUtf8("Téléphone :"), m_tel);
        form->addRow("Email :", m_email);
        form->addRow("Adresse :", m_adresse);
        form->addRow(QString::fromUtf8("Type de client :"), m_type);
        form->addRow(QString::fromUtf8("Date d'inscription :"), m_date);
        form->addRow("Remarques :", m_remarques);
        layout->addLayout(form);

        auto *buttons = new QDialogButtonBox(QDialogButtonBox::Save | QDialogButtonBox::Cancel);
        buttons->button(QDialogButtonBox::Save)->setText("Enregistrer");
        buttons->button(QDialogButtonBox::Cancel)->setText("Annuler");
        connect(buttons, &QDialogButtonBox::accepted, this, [this] { enregistrer(); });
        connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
        layout->addWidget(buttons);
    }

    Client client() const { return m_client; }

private:
    void enregistrer()
    {
        Client candidate = m_client;
        candidate.id = m_id->text().trimmed().toUpper();
        candidate.nom = m_nom->text().trimmed();
        candidate.prenom = m_prenom->text().trimmed();
        candidate.tel = Client::normaliserTel(m_tel->text());
        candidate.email = m_email->text().trimmed();
        candidate.adresse = Client::normaliserAdresse(m_adresse->text());
        candidate.type = m_type->currentText();
        candidate.date = m_date->date().toString("dd/MM/yyyy");
        candidate.remarques = m_remarques->toPlainText().trimmed();

        QString erreur = Client::erreurId(candidate.id);
        if (erreur.isEmpty()) erreur = Client::erreurTel(candidate.tel);
        if (erreur.isEmpty()) erreur = Client::erreurAdresse(candidate.adresse);
        if (!erreur.isEmpty()) {
            QMessageBox::warning(this, "Vérification", erreur);
            return;
        }

        bool requeteOk = false;
        const bool existe = Client::idExiste(candidate.id, m_edition ? m_client.id : QString(), &requeteOk);
        if (!requeteOk) {
            QMessageBox::critical(this, "Oracle", Client::derniereErreur());
            return;
        }
        if (existe) {
            QMessageBox::warning(this, "Identifiant déjà utilisé",
                                 "Cet identifiant client existe déjà. Choisissez-en un autre.");
            return;
        }

        m_client = candidate;
        accept();
    }

    Client m_client;
    bool m_edition;
    QVector<Client> m_clients;
    QLineEdit *m_id = nullptr;
    QLineEdit *m_nom = nullptr;
    QLineEdit *m_prenom = nullptr;
    QLineEdit *m_tel = nullptr;
    QLineEdit *m_email = nullptr;
    QLineEdit *m_adresse = nullptr;
    QComboBox *m_type = nullptr;
    QDateEdit *m_date = nullptr;
    QTextEdit *m_remarques = nullptr;
};
} // namespace

ClientPage::ClientPage(QWidget *parent) : QWidget(parent)
{
    auto *root = new QVBoxLayout(this);
    root->setContentsMargins(28, 24, 28, 24);
    root->setSpacing(14);

    auto *title = new QLabel(QString::fromUtf8("Gestion des clients"));
    title->setStyleSheet("font-size:24px;font-weight:700;color:#0B2A45;");
    root->addWidget(title);

    auto *toolbar = new QHBoxLayout;
    m_recherche = new QLineEdit;
    m_recherche->setPlaceholderText(QString::fromUtf8("Rechercher un client…"));
    m_recherche->setMinimumHeight(38);
    toolbar->addWidget(m_recherche, 1);
    const auto makeButton = [this, toolbar](const QString &text, const QString &role) {
        auto *button = new QPushButton(text);
        button->setProperty("role", role);
        button->setCursor(Qt::PointingHandCursor);
        button->setFixedHeight(40);
        toolbar->addWidget(button);
        return button;
    };
    auto *add = makeButton("Ajouter", "btnPrimaire");
    auto *edit = makeButton("Modifier", "btnGris");
    auto *remove = makeButton("Supprimer", "btnBeige");
    auto *refresh = makeButton("Actualiser", "btnClair");

    m_erreur = new QLabel;
    m_erreur->setWordWrap(true);
    m_erreur->setStyleSheet("color:#A12B2B;font-weight:600;");
    root->addWidget(m_erreur);

    m_table = new QTableWidget(0, 8, this);
    m_table->setHorizontalHeaderLabels({"ID", "Nom", "Prénom", "Téléphone", "Email",
                                        "Adresse", "Type", "Inscription"});
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setSelectionMode(QAbstractItemView::SingleSelection);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->verticalHeader()->setVisible(false);
    m_table->verticalHeader()->setDefaultSectionSize(38);
    m_table->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    m_table->horizontalHeader()->setSectionResizeMode(5, QHeaderView::Stretch);
    m_table->setAlternatingRowColors(true);
    root->addWidget(m_table, 1);

    connect(m_recherche, &QLineEdit::textChanged, this, [this] { remplirTable(); });
    connect(add, &QPushButton::clicked, this, &ClientPage::ajouter);
    connect(edit, &QPushButton::clicked, this, &ClientPage::modifier);
    connect(remove, &QPushButton::clicked, this, &ClientPage::supprimer);
    connect(refresh, &QPushButton::clicked, this, &ClientPage::reload);
    connect(m_table, &QTableWidget::cellDoubleClicked, this, [this](int, int) { modifier(); });
}

void ClientPage::reload()
{
    m_clients = Client::lister();
    const QString error = Client::derniereErreur();
    m_erreur->setText(error.isEmpty() ? QString() : QString::fromUtf8("Lecture Oracle impossible : ") + error);
    remplirTable();
}

void ClientPage::remplirTable()
{
    m_table->setSortingEnabled(false);
    m_table->setRowCount(0);
    const QString filtre = m_recherche->text().trimmed().toLower();
    for (const Client &client : m_clients) {
        const QStringList cells = {client.id, client.nom, client.prenom, client.tel, client.email,
                                   client.adresse, client.type, client.date};
        if (!filtre.isEmpty() && !cells.join(QLatin1Char(' ')).toLower().contains(filtre)) continue;
        const int row = m_table->rowCount();
        m_table->insertRow(row);
        for (int column = 0; column < cells.size(); ++column) {
            auto *item = new QTableWidgetItem(cells[column]);
            if (column == 0) item->setData(Qt::UserRole, client.id);
            if (column == 6) {
                item->setBackground(client.type == "VIP" ? QColor("#F3D58C") :
                                    client.type == "Premium" ? QColor("#DCE7EF") : QColor("#F2F1ED"));
                item->setTextAlignment(Qt::AlignCenter);
            }
            m_table->setItem(row, column, item);
        }
    }
    m_table->setSortingEnabled(true);
}

Client *ClientPage::clientSelectionne()
{
    const int row = m_table->currentRow();
    if (row < 0 || !m_table->item(row, 0)) return nullptr;
    const QString id = m_table->item(row, 0)->data(Qt::UserRole).toString();
    for (Client &client : m_clients)
        if (client.id == id) return &client;
    return nullptr;
}

void ClientPage::ajouter()
{
    Client initial;
    initial.id = prochainId(m_clients);
    if (initial.id.isEmpty()) {
        QMessageBox::warning(this, "Limite atteinte", "La numérotation CL-001 à CL-999 est complète.");
        return;
    }
    ClientDialog dialog(initial, false, m_clients, this);
    if (dialog.exec() != QDialog::Accepted) return;
    const Client client = dialog.client();
    if (!client.ajouter()) {
        QMessageBox::critical(this, "Oracle", QString::fromUtf8("L'ajout a échoué : ") + Client::derniereErreur());
        return;
    }
    reload();
}

void ClientPage::modifier()
{
    Client *selected = clientSelectionne();
    if (!selected) {
        QMessageBox::information(this, "Modifier", "Sélectionnez un client dans le tableau.");
        return;
    }
    const QString oldId = selected->id;
    ClientDialog dialog(*selected, true, m_clients, this);
    if (dialog.exec() != QDialog::Accepted) return;
    const Client updated = dialog.client();
    if (!updated.modifier(oldId)) {
        QMessageBox::critical(this, "Oracle", QString::fromUtf8("La modification a échoué : ") + Client::derniereErreur());
        return;
    }
    reload();
}

void ClientPage::supprimer()
{
    Client *selected = clientSelectionne();
    if (!selected) {
        QMessageBox::information(this, "Supprimer", "Sélectionnez un client dans le tableau.");
        return;
    }
    const QString id = selected->id;
    const QString name = selected->prenom + " " + selected->nom;
    if (QMessageBox::question(this, "Supprimer le client",
            QString::fromUtf8("Supprimer %1 (%2) ? Cette action est définitive.").arg(name.trimmed(), id),
            QMessageBox::Yes | QMessageBox::No, QMessageBox::No) != QMessageBox::Yes) return;
    if (!Client::supprimer(id)) {
        QMessageBox::critical(this, "Oracle", QString::fromUtf8("La suppression a échoué : ") + Client::derniereErreur());
        return;
    }
    reload();
}
