#ifndef NEWSCONTAINER_H
#define NEWSCONTAINER_H

#include <QWidget>
#include <QList>
#include <QStandardItemModel>
#include <Qtimer>
#include <QMdiSubWindow>
#include "ui_mdisubview.h"
#include "mainwindow.h"
#include "QDSApi.h"

namespace Ui {
class MdiSubWindow;
}

class MainWindow;

#define MAXTABLEROWCOUNT 2000

class MdiSubWindow : public QWidget
{
    Q_OBJECT

public:
    enum WorkState {
                    Working,            //正在工作
                    Pause               //暂停
                   };

    explicit MdiSubWindow(MsgType msgType, QWidget *parent = 0);
    MsgType m_msgType;
    WorkState m_workState;
    MainWindow *m_mainWindow;
    QStringList m_symbolList;
    Ui::MdiSubWindow *ui;
    QStandardItemModel* m_pTableModel;  //表格视图对应的模型数据
    qint64 m_curDataIndex;		//数据索引，表格中添加数据，自动加1
	QList<QStringList>* m_pTableData;  //表格最近2000条数据
    QList<QStringList>* m_pDataList;    //临时表格数据缓存，由于视图仅允许UI线程进行刷新，所以新接收的数据必须找个地方暂存起来，由UI线程来调用。
    QTimer m_timer; //计时器，用于刷新界面
    ~MdiSubWindow();


public slots:
    void onTimeout();
    void onTableAddingNews(QStringList);

protected:
    void closeEvent(QCloseEvent *event);
private slots:
    void on_button_symbol_clicked();
};

#endif // NEWSCONTAINER_H
