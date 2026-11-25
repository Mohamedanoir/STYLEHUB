#pragma once

#include <QDialog>
#include "employe.h"

namespace Ui { class LoginDialog; }

class LoginDialog : public QDialog
{
    Q_OBJECT
public:
    explicit LoginDialog(QWidget *parent = nullptr);
    ~LoginDialog() override;

    const Employe &employeConnecte() const;
    void reinitialiser();

private slots:
    void tenterConnexion();
    void motDePasseOublie();

private:
    void ajouterIcone(QWidget *champ, const QString &image, bool aGauche);
    Ui::LoginDialog *ui;
    Employe m_employe;
};
