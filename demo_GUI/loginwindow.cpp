#include "loginwindow.h"
#include "ui_loginwindow.h"


LoginWindow::LoginWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::LoginWindow),
    m_settings(qApp->applicationDirPath() + "/config/guiconfig.ini",QSettings::IniFormat)
{
    ui->setupUi(this);
    setWindowFlags(Qt::MSWindowsFixedSizeDialogHint | Qt::WindowMinimizeButtonHint | Qt::WindowCloseButtonHint);
    ui->lineEdit_ip->setText(m_settings.value("demogui/ip").toString());
    ui->lineEdit_port->setText(m_settings.value("demogui/port").toString());
    ui->lineEdit_publicKey->setText(m_settings.value("demogui/publicKey").toString());
    ui->lineEdit_secretKey->setText(m_settings.value("demogui/secretKey").toString());
    ui->checkBox_isWAN->setChecked(m_settings.value("demogui/isWAN").toBool());
}

LoginWindow::~LoginWindow()
{
    delete ui;
}

void LoginWindow::on_button_login_clicked()
{
    LoginData loginData;
    loginData.ip = ui->lineEdit_ip->text();
    loginData.port = ui->lineEdit_port->text().toShort();
    loginData.publicKey = ui->lineEdit_publicKey->text();
    loginData.secretKey = ui->lineEdit_secretKey->text();
    loginData.isWan = ui->checkBox_isWAN->isChecked();

    m_settings.setValue("demogui/ip", loginData.ip);
    m_settings.setValue("demogui/port", QString::number(loginData.port));
    m_settings.setValue("demogui/publicKey", loginData.publicKey);
    m_settings.setValue("demogui/secretKey", loginData.secretKey);
    m_settings.setValue("demogui/isWAN", loginData.isWan);

    emit loginSucceed(loginData);
    hide();
}
