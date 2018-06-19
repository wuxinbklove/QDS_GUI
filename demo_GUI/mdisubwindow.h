#ifndef NEWSCONTAINER_H
#define NEWSCONTAINER_H

#include <QWidget>
#include <QList>
#include <QStandardItemModel>
#include <Qtimer>
#include <QMdiSubWindow>
#include <QListWidget>
#include <QLineEdit>
#include "ui_mdisubview.h"
#include "mainwindow.h"
#include "QDSApi.h"

namespace Ui {
class MdiSubWindow;
}

class MainWindow;

//MDI子窗口
class MdiSubWindow : public QWidget
{
    Q_OBJECT

public:
    enum WorkState {
                    Working,            //正在工作
                    Pause               //暂停
                   };

    explicit MdiSubWindow(MsgType msgType, QWidget *parent = 0);
	void initTable(MainWindow* mainWindow ,QStringList headList);		//初始化表格
	~MdiSubWindow();

private:
    MsgType m_msgType;		//订阅类型
    WorkState m_workState;		//当前工作状态，界面表格处于刷新还是暂停刷新
    int m_maxShowCount;

    MainWindow *m_mainWindow;
	Ui::MdiSubWindow *ui;

	QStringList m_symbolList;		//所有symbol项
	QStringList m_headList;			//所有表头项
	QString m_headConfig;		//表头设置

	QList<int> m_tmpDelayList;		//统计近100条QDSTime - Time，用于计算延迟。
	int m_lastCalcDelay;		//最近一次延迟数

	qint64 m_curDataIndex;		//数据索引，表格中添加数据，自动加1
    QStandardItemModel* m_pTableModel;  //表格视图对应的模型数据
	QList<QStringList>* m_pTableData;  //表格最近2000条数据
    QList<QStringList>* m_pDataList;    //临时表格数据缓存，由于视图仅允许UI线程进行刷新，所以新接收的数据必须找个地方暂存起来，由UI线程来调用。
    QTimer m_timer; //计时器，用于刷新界面

	QSettings m_settings;		//订阅内容保存在配置中

	QFile *m_pLogFile;		//日志文件


public slots:
    void onTimeout();
    void onTableAddingNews(QStringList, int);


protected:
    void closeEvent(QCloseEvent *event);
private slots:
    void on_button_symbol_clicked();
};

#endif // NEWSCONTAINER_H
