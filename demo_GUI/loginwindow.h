#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include <QMainWindow>
#include <QSettings>

namespace Ui {
class LoginWindow;
}

//��¼����
class LoginWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit LoginWindow(QWidget *parent = 0);
    ~LoginWindow();

public:
    typedef struct LoginData
    {
        QString ip;
        short port;
        QString publicKey;
        QString secretKey;
        bool isWan;
    }LoginData;
	Ui::LoginWindow *ui;

private:
    QSettings m_settings;

signals:
    void loginSucceed(LoginWindow::LoginData);
private slots:
    void on_button_login_clicked();
};

#endif // LOGINWINDOW_H
