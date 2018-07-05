#include "loginwindow.h"
#include "ui_loginwindow.h"


LoginWindow::LoginWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::LoginWindow),
    m_settings(qApp->applicationDirPath() + "/config/guiconfig.ini",QSettings::IniFormat)
{
    ui->setupUi(this);
    setWindowIcon(QIcon(":/icon.png"));
    setWindowFlags(Qt::MSWindowsFixedSizeDialogHint | Qt::WindowMinimizeButtonHint | Qt::WindowCloseButtonHint);
    ui->lineEdit_ip->setText(m_settings.value("login/ip").toString());
    ui->lineEdit_port->setText(m_settings.value("login/port").toString());
    ui->lineEdit_publicKey->setText(m_settings.value("login/publicKey").toString());
    ui->lineEdit_secretKey->setText(m_settings.value("login/secretKey").toString());
    ui->checkBox_isWAN->setChecked(m_settings.value("login/isWAN").toBool());

	connect(ui->button_getKey, &QPushButton::clicked, [=]() {
		ui->stackedWidget->setCurrentIndex(1);
	});

	connect(ui->button_returnLogin, &QPushButton::clicked, [=]() {
		ui->stackedWidget->setCurrentIndex(0);
	});
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

    m_settings.setValue("login/ip", loginData.ip);
    m_settings.setValue("login/port", QString::number(loginData.port));
    m_settings.setValue("login/publicKey", loginData.publicKey);
    m_settings.setValue("login/secretKey", loginData.secretKey);
    m_settings.setValue("login/isWAN", loginData.isWan);

    emit loginSucceed(loginData);
}
