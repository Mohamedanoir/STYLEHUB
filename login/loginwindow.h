#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class LoginWindow; }
QT_END_NAMESPACE

// Écran d'authentification FASHIONOVA.
// Mode actuel : n'importe quelle saisie (même vide) est acceptée -> ouverture de l'application.
class LoginWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit LoginWindow(QWidget *parent = nullptr);
    ~LoginWindow() override;

signals:
    // émis quand l'utilisateur clique « Se connecter » (ou appuie sur Entrée)
    void connecte(const QString &login, const QString &role);

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;

private slots:
    void onConnecter();
    void onMotDePasseOublie();

private:
    void ajouterIcone(QWidget *champ, const QString &image, bool aGauche);
    QWidget *m_oeil = nullptr;

    Ui::LoginWindow *ui;
};

#endif // LOGINWINDOW_H
