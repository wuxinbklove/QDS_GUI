#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QList>
#include <QStandardItemModel>
#include <Qtimer>
#include "loginwindow.h"
#include "newslistener.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_button_confirm_clicked();
    void onTimeout();

public slots:
    void onLoginSucceed(LoginWindow::LoginData);
    void onTableAddingNews(QStringList);

private:
    Ui::MainWindow *ui;
    QStandardItemModel* m_pTableModel;  //表格视图对应的模型数据
    QList<QStringList>* m_pDataList;    //临时表格数据缓存，由于视图仅允许UI线程进行刷新，所以新接收的数据必须找个地方暂存起来，由UI线程来调用。
    QTimer m_timer; //计时器，用于刷新界面
};

#endif // MAINWINDOW_H
