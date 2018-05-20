#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QList>
#include <QStandardItemModel>
#include <Qtimer>
#include <QMap>
#include "loginwindow.h"
#include "newslistener.h"
#include "mdisubwindow.h"

namespace Ui {
class MainWindow;
}

class MdiSubWindow;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_button_subscribe_clicked();
    void on_button_autoLayout_clicked();
    void on_comboBox_showStyle_currentIndexChanged(int index);
    void on_button_autoStack_clicked();

    void on_button_subscribeAll_clicked();

public slots:
    void onLoginSucceed(LoginWindow::LoginData);
    void onTableAddingNews(MsgType MsgType ,QStringList);

public:
    QMap<MsgType,MdiSubWindow *> m_mdiSubViewTable;
	int m_lastComboShowStyleIndex;
    void needSubscribe(MsgType msgType, QString codeStr);
    void needUnsubscribe(MsgType msgType, QString codeStr);
    void closeEvent(QCloseEvent *event);

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
