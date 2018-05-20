#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QDSApi.h"
#include "QDSStruct.h"
#include <QMessageBox>
#include <QDebug>
#include <QStringList>
#include <QMutex>
#include <QApplication>
#include <QDesktopWidget>


const static QList<int> exchangeEnum = {0x00102000, //上交所L2静态数据
                                        0x00102001, //上交所L2实时行情
                                        0x00102002, //上交所L2逐笔成交
                                        0x00102003, //上交所L2指数行情
                                        0x00102004, //上交所L2虚拟集合竞价
                                        0x00102005, //上交所L2市场总览
                                        0x00202000, //深交所L2静态数据
                                        0x00202001, //深交所L2实时行情
                                        0x00202002, //深交所L2逐笔成交
                                        0x00202003, //深交所L2指数行情
                                        0x00202006, //深交所L2逐笔委托
                                        0x00202007  //深交所L2证券状态
                                       };

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
	m_lastComboShowStyleIndex = 0;
    MdiSubWindow *pMdiSubView = NULL;
    for (int i = 0 ; i < exchangeEnum.count(); ++i)
    {
        m_mdiSubViewTable.insert((MsgType)exchangeEnum[i] , pMdiSubView);
    }
    ui->mdiArea->tileSubWindows();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_button_subscribe_clicked()
{
    QString codeText = "";
    int msgType = exchangeEnum[ui->combo_exchange->currentIndex()];
	MdiSubWindow *pMdiSubView = m_mdiSubViewTable[(MsgType)msgType];
	if (pMdiSubView != NULL)
	{
		needUnsubscribe((MsgType)msgType, NULL);

        QList<QMdiSubWindow *> allSubWindows = ui->mdiArea->subWindowList();
        for (int i = 0; i < allSubWindows.count(); ++i)
        {
            if (allSubWindows[i]->widget() == pMdiSubView)
            {
                ui->mdiArea->removeSubWindow(allSubWindows[i]);
                break;
            }
        }
        m_mdiSubViewTable[(MsgType)msgType] = NULL;
	}
    needSubscribe((MsgType)msgType, codeText);
}


void MainWindow::on_button_autoLayout_clicked()
{
    ui->mdiArea->tileSubWindows();
}

void MainWindow::on_comboBox_showStyle_currentIndexChanged(int index)
{
	
	if (index != 2 && m_lastComboShowStyleIndex == 2)
	{
		ui->mdiArea->closeAllSubWindows();
		for (auto iter = m_mdiSubViewTable.begin(); iter != m_mdiSubViewTable.end(); iter++)
		{
			MdiSubWindow *pSubWindow = iter.value();
			if (pSubWindow != NULL)
			{
				pSubWindow->setParent(ui->mdiArea);
				ui->mdiArea->addSubWindow(pSubWindow);
				pSubWindow->show();
			}
		}
	}

    if (index == 0)
    {
        ui->mdiArea->setViewMode(QMdiArea::SubWindowView);
        ui->button_autoLayout->show();
        ui->button_autoStack->show();
		ui->mdiArea->tileSubWindows();
    }
    else if (index == 1)
    {
        ui->mdiArea->setViewMode(QMdiArea::TabbedView);
        ui->button_autoLayout->hide();
        ui->button_autoStack->hide();
        ui->mdiArea->setTabsMovable(true);
		QMdiSubWindow *pCurActiveSubWindow = ui->mdiArea->activeSubWindow();
		if (pCurActiveSubWindow == NULL && ui->mdiArea->subWindowList().count() > 0)
			pCurActiveSubWindow = ui->mdiArea->subWindowList().first();
		if (pCurActiveSubWindow != NULL)
			pCurActiveSubWindow->showMaximized();
    }
    else
    {
        ui->button_autoLayout->hide();
        ui->button_autoStack->hide();
		if (m_lastComboShowStyleIndex != 2)
		{
			QList<QMdiSubWindow *> allSubWindows = ui->mdiArea->subWindowList();
			int offsetX = 0;
			int offsetY = 0;
			for (int i = 0; i < allSubWindows.count(); i++)
			{
				QMdiSubWindow *pSubWindow = allSubWindows[i];
				QWidget *pWidget = pSubWindow->widget();
				if (pWidget != NULL)
				{
					pWidget->setParent(0);
					pWidget->resize(400, 200);
					pWidget->show();
					pWidget->move((qApp->desktop()->width() - pWidget->width()) / 2 + offsetX, (qApp->desktop()->height() - pWidget->height()) / 2 + offsetY);
					offsetX += 10;
					offsetY += 15;
					pSubWindow->setWidget(0);
					ui->mdiArea->removeSubWindow(pSubWindow);
				}
			}
		}

    }
	m_lastComboShowStyleIndex = index;
}

void MainWindow::onLoginSucceed(LoginWindow::LoginData loginData)
{
    CreateInstance();
    NewsListener::InitSubscribe(this);
    RegisterService(loginData.ip.toStdString().c_str(), loginData.port);
    RetCode ret = Login(loginData.publicKey.toStdString().c_str(), loginData.secretKey.toStdString().c_str(), loginData.isWan);
    if (ret != Ret_Success)
        QMessageBox::critical(this , "Error", "Login Error");
    UnsubscribeAll();
    show();
}

void MainWindow::needSubscribe(MsgType msgType, QString codeStr)
{
    std::string tmpStr = codeStr.toStdString();
    const char* pCode = tmpStr.c_str();
    if (codeStr.isEmpty())
        pCode = NULL;

    MdiSubWindow *pMdiSubView = m_mdiSubViewTable[(MsgType)msgType];
	if (pMdiSubView != NULL)
	{
		Unsubscribe((MsgType)msgType, NULL);
		pMdiSubView->close();
	}

    Subscribe((MsgType)msgType, (char *)pCode);
    qDebug() << QString("Subscirbe---MsgType:") + QString::number(msgType) + QString("  Code:") + (pCode == NULL ? "NULL" : pCode);
    QStringList headerList;
    switch ((MsgType)msgType)
    {
    case Msg_SSEL2_Static:
        headerList <<"索引" << "QDSTime" << "Symbol" << "Time" << "SampleAvgPrice";
        break;
    case Msg_SSEL2_Quotation:
        headerList <<"索引" <<  "QDSTime" << "Symbol" << "Time"  << "OpenPrice" << "HighPrice" << "LowPrice" << "LastPrice";
        break;
    case Msg_SSEL2_Transaction:
        headerList <<"索引" << "QDSTime" << "Symbol" << "TradePrice";
        break;
    case Msg_SSEL2_Auction:
        headerList <<"索引" << "QDSTime" << "Symbol" << "Time" << "OpenPrice" << "AuctionVolume" << "LeaveVolume";
        break;
    case Msg_SSEL2_Index:
        headerList <<"索引" << "QDSTime" << "Symbol" << "Time"<< "OpenPrice" << "HighPrice" << "LowPrice" << "LastPrice";
        break;
    case Msg_SSEL2_Overview:
        headerList <<"索引" << "QDSTime" << "Symbol" << "Time"  << "TradeDate" << "TradeTime";
        break;
    case Msg_SZSEL2_Static:
        headerList <<"索引" << "QDSTime" << "Symbol" << "IssuedVolume" << "OutstandingShare" << "LimitUpAbsoluteT" << "LimitDownAbsoluteT";
        break;
    case Msg_SZSEL2_Quotation:
        headerList <<"索引" << "QDSTime" << "Symbol" << "Time"<< "OpenPrice" << "HighPrice" << "LowPrice" << "LastPrice";
        break;
    case Msg_SZSEL2_Transaction:
        headerList <<"索引" << "QDSTime" << "Symbol" << "TradeTime" << "TradePrice" << "TradeVolume" << "TradeType";
        break;
    case Msg_SZSEL2_Index:
        headerList <<"索引" << "QDSTime" << "Symbol" << "Time" << "OpenPrice" << "HighPrice" << "LowPrice" << "LastPrice";
        break;
    case Msg_SZSEL2_Order:
        headerList <<"索引" << "QDSTime" << "Symbol" << "Time"  << "OrderPrice" << "OrderVolume" << "OrderCode" << "OrderType";
        break;
    case Msg_SZSEL2_Status:
        headerList <<"索引" << "QDSTime" << "Symbol" << "Time"  << "FinancialStatus";
        break;
    default:
        break;
    }

    pMdiSubView = new MdiSubWindow((MsgType)msgType , ui->mdiArea);
    pMdiSubView->m_pTableModel = new QStandardItemModel();
    pMdiSubView->m_pTableModel->setColumnCount(headerList.count());
    pMdiSubView->m_pTableModel->setHorizontalHeaderLabels(headerList);
    pMdiSubView->ui->tableView->setModel(pMdiSubView->m_pTableModel);
    pMdiSubView->ui->tableView->setColumnWidth(0, 50);
	pMdiSubView->ui->tableView->horizontalHeader()->setSectionsMovable(true);
    pMdiSubView->m_mainWindow = this;
	m_mdiSubViewTable[(MsgType)msgType] = pMdiSubView;


	if (m_lastComboShowStyleIndex == 0)
	{
		ui->mdiArea->addSubWindow(pMdiSubView);
		pMdiSubView->show();
		ui->mdiArea->setViewMode(QMdiArea::SubWindowView);
		ui->mdiArea->tileSubWindows();
	}
    else if (m_lastComboShowStyleIndex == 1)
    {
		ui->mdiArea->addSubWindow(pMdiSubView);
		pMdiSubView->show();
        ui->mdiArea->setViewMode(QMdiArea::TabbedView);
        pMdiSubView->showMaximized();
    }
	else if (m_lastComboShowStyleIndex == 2)
	{
		pMdiSubView->setParent(0);
		pMdiSubView->resize(400, 200);
		pMdiSubView->show();
		pMdiSubView->move((qApp->desktop()->width() - pMdiSubView->width()) / 2, (qApp->desktop()->height() - pMdiSubView->height()) / 2);
	}

}

void MainWindow::needUnsubscribe(MsgType msgType, QString codeStr)
{
    std::string tmpStr = codeStr.toStdString();
    const char* pCode = tmpStr.c_str();
    if (codeStr.isEmpty())
        pCode = NULL;
    Unsubscribe((MsgType)msgType, (char *)pCode);
    m_mdiSubViewTable[(MsgType)msgType] = NULL;
}

void MainWindow::onTableAddingNews(MsgType msgType, QStringList news)
{
	MdiSubWindow *pMdiSubView = m_mdiSubViewTable[msgType];
	if (pMdiSubView != NULL)
		pMdiSubView->onTableAddingNews(news);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    UnsubscribeAll();
	qApp->closeAllWindows();
    event->accept();
}

void MainWindow::on_button_autoStack_clicked()
{
    ui->mdiArea->cascadeSubWindows();
}

void MainWindow::on_button_subscribeAll_clicked()
{
    for (int i = 0; i< exchangeEnum.count(); ++i)
    {
        QString codeText = "";
        int msgType = exchangeEnum[i];
        MdiSubWindow *pMdiSubView = m_mdiSubViewTable[(MsgType)msgType];
        if (pMdiSubView != NULL)
        {
            needUnsubscribe((MsgType)msgType, NULL);

            QList<QMdiSubWindow *> allSubWindows = ui->mdiArea->subWindowList();
            for (int i = 0; i < allSubWindows.count(); ++i)
            {
                if (allSubWindows[i]->widget() == pMdiSubView)
                {
                    ui->mdiArea->removeSubWindow(allSubWindows[i]);
                    break;
                }
            }
            m_mdiSubViewTable[(MsgType)msgType] = NULL;
        }
        needSubscribe((MsgType)msgType, codeText);
    }

}
