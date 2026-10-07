#include "loginwindow.h"
#include "ui_loginwindow.h"

#include <QColor>
#include <QEvent>
#include <QFile>
#include <QFontDatabase>
#include <QGraphicsDropShadowEffect>
#include <QHBoxLayout>
#include <QIcon>
#include <QLabel>
#include <QMessageBox>
#include <QPixmap>

LoginWindow::LoginWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::LoginWindow)
{
    // Polices de l'écran de connexion (enregistrées pour toute l'application, sans effet sur les autres styles)
    static bool policesChargees = false;
    if (!policesChargees) {
        const QStringList polices = {
            ":/login/fonts/Jost-Regular.ttf",
            ":/login/fonts/Jost-Medium.ttf",
            ":/login/fonts/Jost-SemiBold.ttf",
            ":/login/fonts/CormorantGaramond-Medium.ttf",
            ":/login/fonts/CormorantGaramond-MediumItalic.ttf",
        };
        for (const QString &p : polices) QFontDatabase::addApplicationFont(p);
        policesChargees = true;
    }

    ui->setupUi(this);

    // Le style est posé sur CETTE fenêtre uniquement (et non sur QApplication),
    // pour ne pas modifier l'apparence des 6 modules de l'application.
    QFile qss(":/login/login.qss");
    if (qss.open(QFile::ReadOnly | QFile::Text))
        setStyleSheet(QString::fromUtf8(qss.readAll()));

    // Icônes dans les champs
    ajouterIcone(ui->loginEdit, ":/login/images/utilisateur.png", true);
    ajouterIcone(ui->motDePasseEdit, ":/login/images/cadenas.png", true);
    ajouterIcone(ui->motDePasseEdit, ":/login/images/oeil.png", false);   // afficher / masquer le mot de passe
    ajouterIcone(ui->roleCombo, ":/login/images/groupe.png", true);

    ui->motDePasseEdit->setEchoMode(QLineEdit::Password);

    // Flèche dorée dans le bouton « Se connecter »
    ui->btnConnecter->setIcon(QIcon(":/login/images/fleche.png"));
    ui->btnConnecter->setIconSize(QSize(22, 22));
    ui->btnConnecter->setText(ui->btnConnecter->text() + "   ");
    ui->btnConnecter->setDefault(true);
    ui->btnConnecter->setCursor(Qt::PointingHandCursor);

    auto *ombreBouton = new QGraphicsDropShadowEffect(this);
    ombreBouton->setBlurRadius(28);
    ombreBouton->setOffset(0, 8);
    ombreBouton->setColor(QColor(15, 27, 46, 70));
    ui->btnConnecter->setGraphicsEffect(ombreBouton);

    // Actions
    connect(ui->btnConnecter, &QPushButton::clicked, this, &LoginWindow::onConnecter);
    connect(ui->loginEdit, &QLineEdit::returnPressed, this, &LoginWindow::onConnecter);
    connect(ui->motDePasseEdit, &QLineEdit::returnPressed, this, &LoginWindow::onConnecter);
    connect(ui->btnMotDePasseOublie, &QPushButton::clicked, this, &LoginWindow::onMotDePasseOublie);
}

LoginWindow::~LoginWindow()
{
    delete ui;
}

void LoginWindow::onConnecter()
{
    // Pour l'instant : aucune vérification, on laisse entrer quelle que soit la saisie.
    emit connecte(ui->loginEdit->text().trimmed(), ui->roleCombo->currentText());
}

void LoginWindow::onMotDePasseOublie()
{
    QMessageBox::information(this, "Mot de passe oublié",
                             "Contactez l'administrateur de l'atelier pour réinitialiser votre mot de passe.");
}

// Œil : clic = afficher / masquer le mot de passe
bool LoginWindow::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == m_oeil && event->type() == QEvent::MouseButtonRelease) {
        const bool masque = ui->motDePasseEdit->echoMode() == QLineEdit::Password;
        ui->motDePasseEdit->setEchoMode(masque ? QLineEdit::Normal : QLineEdit::Password);
        return true;
    }
    return QMainWindow::eventFilter(obj, event);
}

// Place une petite icône à gauche (ou à droite) d'un champ
void LoginWindow::ajouterIcone(QWidget *champ, const QString &image, bool aGauche)
{
    auto *icone = new QLabel(champ);
    icone->setPixmap(QPixmap(image).scaled(24, 24, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    icone->setAttribute(Qt::WA_TransparentForMouseEvents, aGauche);
    icone->setStyleSheet("background: transparent; border: none;");
    if (!aGauche) {
        icone->setCursor(Qt::PointingHandCursor);
        icone->installEventFilter(this);
        m_oeil = icone;
    }

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
