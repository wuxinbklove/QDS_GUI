#include "mainwindow.h"
#include "loginwindow.h"
#include <QApplication>
#include <QDebug>

int main(int argc, char *argv[])
{
    qApp->setAttribute(Qt::AA_EnableHighDpiScaling, true);

    QApplication a(argc, argv);

    LoginWindow loginWindow;
    loginWindow.show();

    MainWindow mainWindow;
    QObject::connect(&loginWindow, SIGNAL(loginSucceed(LoginWindow::LoginData)), &mainWindow, SLOT(onLoginSucceed(LoginWindow::LoginData)));

    return a.exec();
}
