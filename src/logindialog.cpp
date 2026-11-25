#include "logindialog.h"
#include "ui_logindialog.h"

#include <QFile>
#include <QAction>
#include <QApplication>
#include <QGraphicsDropShadowEffect>
#include <QHBoxLayout>
#include <QIcon>
#include <QGuiApplication>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPixmap>
#include <QPushButton>
#include <QScreen>
#include <QSet>
#include <QSettings>

LoginDialog::LoginDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::LoginDialog)
{
    ui->setupUi(this);
    setObjectName(QStringLiteral("LoginDialog"));
    setWindowTitle(QStringLiteral("FashionNova — Connexion employé"));
    setWindowFlag(Qt::WindowContextHelpButtonHint, false);
    setSizeGripEnabled(false);

    const QScreen *screen = QGuiApplication::primaryScreen();
    if (screen) {
        const QRect available = screen->availableGeometry();
        const int width = qMin(1536, available.width() - 32);
        const int height = qMin(790, available.height() - 24);
        resize(qMax(1200, width), qMax(680, height));
        move(available.center() - rect().center());
    } else {
        resize(1536, 790);
    }

    QFile styleFile(QStringLiteral(":/style.qss"));
    if (styleFile.open(QFile::ReadOnly | QFile::Text))
        setStyleSheet(QString::fromUtf8(styleFile.readAll()) +
                      QStringLiteral("\nQDialog#LoginDialog{background-color:#F4EBDF;}"));

    ui->lblLogin->setText(QStringLiteral("ID Employé"));
    ui->loginEdit->setPlaceholderText(QStringLiteral("Entrez votre ID, ex. EMP001"));
    ui->roleCombo->clear();
    ui->roleCombo->setPlaceholderText(QStringLiteral("Sélectionnez votre rôle"));
    ui->roleCombo->setCurrentIndex(-1);
    QString databaseError;
    const QList<Employe> employees = Employe::afficher(&databaseError);
    QSet<QString> roles;
    for (const Employe &employee : employees) {
        const QString role = employee.poste.trimmed();
        if (!role.isEmpty() && !roles.contains(role.toCaseFolded())) {
            roles.insert(role.toCaseFolded());
            ui->roleCombo->addItem(role);
        }
    }
    ui->roleCombo->setCurrentIndex(-1);

    ajouterIcone(ui->loginEdit, QStringLiteral(":/images/utilisateur.png"), true);
    ajouterIcone(ui->motDePasseEdit, QStringLiteral(":/images/cadenas.png"), true);
    ajouterIcone(ui->roleCombo, QStringLiteral(":/images/groupe.png"), true);
    auto *showPassword = new QAction(QIcon(QStringLiteral(":/images/oeil.png")),
                                     QStringLiteral("Afficher le mot de passe"), ui->motDePasseEdit);
    ui->motDePasseEdit->addAction(showPassword, QLineEdit::TrailingPosition);
    connect(showPassword, &QAction::triggered, this, [this, showPassword] {
        const bool show = ui->motDePasseEdit->echoMode() == QLineEdit::Password;
        ui->motDePasseEdit->setEchoMode(show ? QLineEdit::Normal : QLineEdit::Password);
        showPassword->setToolTip(show ? QStringLiteral("Masquer le mot de passe")
                                      : QStringLiteral("Afficher le mot de passe"));
    });
    ui->btnConnecter->setIcon(QIcon(QStringLiteral(":/images/fleche.png")));
    ui->btnConnecter->setIconSize(QSize(22, 22));
    ui->btnConnecter->setText(ui->btnConnecter->text() + QStringLiteral("   "));

    auto *shadow = new QGraphicsDropShadowEffect(this);
    shadow->setBlurRadius(28);
    shadow->setOffset(0, 8);
    shadow->setColor(QColor(15, 27, 46, 70));
    ui->btnConnecter->setGraphicsEffect(shadow);

    QSettings settings;
    const QString savedId = settings.value(QStringLiteral("login/employeeId")).toString();
    ui->loginEdit->setText(savedId);
    ui->checkSouvenir->setChecked(!savedId.isEmpty());

    connect(ui->btnConnecter, &QPushButton::clicked,
            this, &LoginDialog::tenterConnexion);
    connect(ui->loginEdit, &QLineEdit::returnPressed,
            this, &LoginDialog::tenterConnexion);
    connect(ui->motDePasseEdit, &QLineEdit::returnPressed,
            this, &LoginDialog::tenterConnexion);
    connect(ui->btnMotDePasseOublie, &QPushButton::clicked,
            this, &LoginDialog::motDePasseOublie);
}

LoginDialog::~LoginDialog()
{
    delete ui;
}

const Employe &LoginDialog::employeConnecte() const
{
    return m_employe;
}

void LoginDialog::reinitialiser()
{
    ui->motDePasseEdit->clear();
    ui->loginEdit->setFocus();
}

void LoginDialog::tenterConnexion()
{
    if (ui->roleCombo->currentIndex() < 0) {
        QMessageBox::warning(this, QStringLiteral("Connexion"),
                             QStringLiteral("Sélectionnez votre rôle."));
        ui->roleCombo->setFocus();
        return;
    }

    QString erreur;
    Employe employe;
    if (!Employe::authentifier(ui->loginEdit->text(),
                              ui->motDePasseEdit->text(),
                              &employe, &erreur)) {
        QMessageBox::warning(this, QStringLiteral("Connexion refusée"), erreur);
        ui->motDePasseEdit->selectAll();
        ui->motDePasseEdit->setFocus();
        return;
    }

    if (employe.poste.compare(ui->roleCombo->currentText(), Qt::CaseInsensitive) != 0) {
        QMessageBox::warning(this, QStringLiteral("Connexion refusée"),
                             QStringLiteral("Le rôle choisi ne correspond pas au poste associé à cet ID."));
        ui->motDePasseEdit->selectAll();
        ui->motDePasseEdit->setFocus();
        return;
    }

    QSettings settings;
    if (ui->checkSouvenir->isChecked())
        settings.setValue(QStringLiteral("login/employeeId"), ui->loginEdit->text().trimmed());
    else
        settings.remove(QStringLiteral("login/employeeId"));

    m_employe = employe;
    accept();
}

void LoginDialog::motDePasseOublie()
{
    QMessageBox::information(this, QStringLiteral("Mot de passe"),
        QStringLiteral("Contactez le responsable pour réinitialiser votre mot de passe."));
}

void LoginDialog::ajouterIcone(QWidget *champ, const QString &image, bool aGauche)
{
    auto *icone = new QLabel(champ);
    icone->setPixmap(QPixmap(image).scaled(24, 24, Qt::KeepAspectRatio,
                                           Qt::SmoothTransformation));
    icone->setAttribute(Qt::WA_TransparentForMouseEvents, aGauche);
    icone->setStyleSheet(QStringLiteral("background:transparent;border:none;"));
    if (!aGauche)
        icone->setCursor(Qt::PointingHandCursor);

    auto *layout = qobject_cast<QHBoxLayout *>(champ->layout());
    if (!layout) {
        layout = new QHBoxLayout(champ);
        layout->setContentsMargins(22, 0, 22, 0);
        layout->addStretch();
    }
    if (aGauche)
        layout->insertWidget(0, icone);
    else
        layout->addWidget(icone);
}
