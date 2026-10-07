#include "logindialog.h"
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QMessageBox>

LoginDialog::LoginDialog(QWidget *p) : QDialog(p) {
    setWindowTitle("Connexion employé");
    setFixedSize(420,300);
    auto *v=new QVBoxLayout(this);
    auto *title=new QLabel("Connexion employé");
    title->setStyleSheet("font-size:24px;font-weight:700;color:#0B3B67;");
    title->setAlignment(Qt::AlignCenter);
    m_id=new QLineEdit; m_id->setPlaceholderText("Ex. EMP001");
    m_password=new QLineEdit; m_password->setPlaceholderText("Mot de passe");
    m_password->setEchoMode(QLineEdit::Password);
    auto *f=new QFormLayout; f->addRow("ID :",m_id); f->addRow("Mot de passe :",m_password);
    auto *b=new QPushButton("Se connecter");
    b->setStyleSheet("background:#0B3B67;color:white;border-radius:7px;padding:10px;font-weight:700;");
    v->addWidget(title); v->addLayout(f); v->addWidget(b);
    v->addStretch();
    auto *hint=new QLabel("Démo : EMP001 / 1234");
    hint->setStyleSheet("color:#777;");
    hint->setAlignment(Qt::AlignCenter); v->addWidget(hint);
    connect(b,&QPushButton::clicked,this,&LoginDialog::validate);
    connect(m_password,&QLineEdit::returnPressed,this,&LoginDialog::validate);
}
QString LoginDialog::id() const { return m_id->text().trimmed().toUpper(); }
QString LoginDialog::password() const { return m_password->text(); }
void LoginDialog::validate() {
    if(id().isEmpty() || password().isEmpty()) {
        QMessageBox::warning(this,"Connexion","Saisissez l'ID et le mot de passe."); return;
    }
    accept();
}
