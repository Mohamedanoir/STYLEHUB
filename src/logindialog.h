#pragma once
#include <QDialog>
class QLineEdit;
class LoginDialog : public QDialog {
    Q_OBJECT
public:
    explicit LoginDialog(QWidget *parent=nullptr);
    QString id() const;
    QString password() const;
private slots:
    void validate();
private:
    QLineEdit *m_id;
    QLineEdit *m_password;
};
