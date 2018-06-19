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
#include <QDateTime>
#include <QDir>
#include <QThread>

static QMutex mutex;

const static QList<int> exchangeEnum = {0x00102000, //上交所L2静态数据
																	0x00102001, //上交所L2实时行情
																	0x00102002, //上交所L2逐笔成交
																	0x00102003, //上交所L2指数行情
																	0x00102004, //上交所L2虚拟集合竞价
																	0x00102005, //上交所L2市场总览
																	0x00103000,  //上交所个股期权静态数据
																	0x00103001,  //上交所个股期权实时行情
																	0x00202000, //深交所L2静态数据
																	0x00202001, //深交所L2实时行情
																	0x00202002, //深交所L2逐笔成交
																	0x00202003, //深交所L2指数行情
																	0x00202006, //深交所L2逐笔委托
																	0x00202007 //深交所L2证券状态
																	};

//登录线程
class LoginThread : public  QThread
{

public:
	LoginThread(LoginWindow::LoginData loginData ,QObject *parent = NULL);
	RetCode getResult() { return m_result; }

private:
	LoginWindow::LoginData m_loginData;
	RetCode m_result;

protected:
	void run();
};

LoginThread::LoginThread(LoginWindow::LoginData loginData, QObject *)
{
	m_loginData = loginData;
}


void LoginThread::run()
{
	m_result = Login(m_loginData.publicKey.toStdString().c_str(), m_loginData.secretKey.toStdString().c_str(), m_loginData.isWan);
}


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowIcon(QIcon(":/icon.png"));
	m_lastComboShowStyleIndex = 0;
    MdiSubWindow *pMdiSubView = NULL;
    for (int i = 0 ; i < exchangeEnum.count(); ++i)
    {
        m_mdiSubViewTable.insert((MsgType)exchangeEnum[i] , pMdiSubView);
    }
    ui->mdiArea->tileSubWindows();

	//创建日志文件夹（根据当前时间命名）
	m_delayLogPath = qApp->applicationDirPath() + "/log/NewsDelay-" + QDateTime::currentDateTime().toString("yyyyMMdd-HHmmss/");
	QDir dir;
	if (!dir.exists(m_delayLogPath))
	{
		dir.mkpath(m_delayLogPath);
	}
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

    if (index == 0)		//MDI子窗口
    {
        ui->mdiArea->setViewMode(QMdiArea::SubWindowView);
        ui->button_autoLayout->show();
        ui->button_autoStack->show();
		ui->mdiArea->tileSubWindows();
    }
    else if (index == 1)		//Tab标签页
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
    else		//独立窗口
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

void MainWindow::onLogin(LoginWindow::LoginData loginData)
{
    CreateInstance();
    NewsListener::InitSubscribe(this);
    RegisterService(loginData.ip.toStdString().c_str(), loginData.port);        
	LoginThread loginThread(loginData);
	loginThread.start();
	bool isThreadFinish = loginThread.wait(30000);
	RetCode ret;
	LoginWindow *pLoginWindow = (LoginWindow *)sender();
	if (!isThreadFinish)	//如果isThreadFinish为false，则代表超时，界面程序强行终止登录线程
	{
		QMessageBox::critical(pLoginWindow, "Login", "Login OverTime!");
		loginThread.terminate();
		loginThread.wait();
		return;
	}
	else
	{
		ret = loginThread.getResult();
	}
	if (ret != Ret_Success)
	{
		QMessageBox::critical(pLoginWindow, "Login", QString("Login Failed, RetCode = ") + QString::number((int)ret));
		return;
	}
    UnsubscribeAll();
	if (pLoginWindow != NULL)
		pLoginWindow->hide();
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
    QStringList headerList;	//表头项
    switch ((MsgType)msgType)
    {
    case Msg_SSEL2_Static:
        headerList <<"索引" << "QDSTime" << "Symbol" << "Reserved" << "Time"
			<< "ISINCode" << "SecurityName" << "SecurityEN" << "SymbolUnderlying" << "MarketType"
			<< "CFICode" << "SecuritySubType" << "Currency" << "ParValue" << "TradableNo" 
            << "EndDate" << "ListingDate" << "SetNo"  << "BuyVolumeUnit" << "SellVolumeUnit"
            << "DeclareVolumeFloor" << "DeclareVolumeCeiling" << "PreClosePrice"<< "TickSize" << "UpDownLimitType"
            << "PriceUpLimit" << "PriceDownLimit" << "XRRatio" << "XDAmount" << "CrdBuyUnderlying"
            << "CrdSellUnderlying" << "SecurityStatus" << "SampleNo" << "SampleAvgPrice" << "TradeAmount"
            << "AvgCapital" <<"TotalMarketValue" << "MarketValueRatio" << "StaticPERatio" << "IndexLevelStatus";
        break;
    case Msg_SSEL2_Quotation:
		headerList << "索引" << "QDSTime" << "Symbol" << "Reserved" << "Time"
			<< "PreClosePrice" << "OpenPrice" << "HighPrice" << "LowPrice" << "LastPrice"
			<< "ClosePrice" << "TradeStatus" << "SecurityPhaseTag" << "TotalNo" << "TotalVolume"
			<< "TotalAmount" << "TotalBuyOrderVolume" << "WtAvgBuyPrice" << "BondWtAvgBuyPrice" << "TotalSellOrderVolume"
			<< "WtAvgSellPrice" << "BondWtAvgSellPrice" << "IOPV" << "ETFBuyNo" << "ETFBuyVolume"
			<< "ETFBuyAmount" << "ETFSellNo" << "ETFSellVolume" << "ETFSellAmount" << "YTM"
			<< "TotalWarrantExecVol" << "WarrantDownLimit" << "WarrantUpLimit" << "WithdrawBuyNo" << "WithdrawBuyVolume"
			<< "WithdrawBuyAmount" << "WithdrawSellNo" << "WithdrawSellVolume" << "WithdrawSellAmount" << "TotalBuyNo"
			<< "TotalSellNo" << "MaxBuyDuration" << "MaxSellDuration" << "BuyOrderNo" << "SellOrderNo"
			<< "SellLevelNo" << "SellPrice01" << "SellVolume01" << "TotalSellOrderNo01" << "SellPrice02"
			<< "SellVolume02" << "TotalSellOrderNo02" << "SellPrice03" << "SellVolume03" << "TotalSellOrderNo03"
			<< "SellPrice04" << "SellVolume04" << "TotalSellOrderNo04" << "SellPrice05" << "SellVolume05"
			<< "TotalSellOrderNo05" << "SellPrice06" << "SellVolume06" << "TotalSellOrderNo06" << "SellPrice07"
			<< "SellVolume07" << "TotalSellOrderNo07" << "SellPrice08" << "SellVolume08" << "TotalSellOrderNo08"
			<< "SellPrice09" << "SellVolume09" << "TotalSellOrderNo09" << "SellPrice10" << "SellVolume10"
			<< "TotalSellOrderNo10" << "SellLevelQueueNo01" << "SellLevelQueue" << "BuyLevelNo" << "BuyPrice01"
			<< "BuyVolume01" << "TotalBuyOrderNo01" << "BuyPrice02" << "BuyVolume02" << "TotalBuyOrderNo02"
			<< "BuyPrice03" << "BuyVolume03" << "TotalBuyOrderNo03" << "BuyPrice04" << "BuyVolume04"
			<< "TotalBuyOrderNo04" << "BuyPrice05" << "BuyVolume05" << "TotalBuyOrderNo05" << "BuyPrice06"
			<< "BuyVolume06" << "TotalBuyOrderNo06" << "BuyPrice07" << "BuyVolume07" << "TotalBuyOrderNo07"
			<< "BuyPrice08" << "BuyVolume08" << "TotalBuyOrderNo08" << "BuyPrice09" << "BuyVolume09"
			<< "TotalBuyOrderNo09" << "BuyPrice10" << "BuyVolume10" << "TotalBuyOrderNo10" << "BuyLevelQueueNo01" 
			<< "BuyLevelQueue";
       break;
    case Msg_SSEL2_Transaction:
        headerList <<"索引" << "QDSTime" << "Symbol" << "Reserved" << "TradeTime" 
			<< "RecID" << "TradeChannel" << "TradePrice" << "TradeVolume" << "TradeAmount"
			<< "BuyRecID" << "SellRecID" << "BuySellFlag";
        break;
    case Msg_SSEL2_Auction:
        headerList <<"索引" << "QDSTime" << "Symbol" << "Reserved" <<"Time" 
			<< "OpenPrice" << "AuctionVolume" << "LeaveVolume" << "side";
        break;
    case Msg_SSEL2_Index:
        headerList <<"索引" << "QDSTime" << "Symbol" << "Reserved" << "Time"
			<< "TradeTime" << "PreClosePrice"<<"OpenPrice" << "TotalAmount" << "HighPrice" 
			<< "LowPrice" << "LastPrice" << "TotalVolume" << "ClosePrice";
        break;
    case Msg_SSEL2_Overview:
        headerList <<"索引" << "QDSTime" << "Symbol" << "Reserved" << "Time" 
			<< "MarketTime" << "TradeDate";
        break;
	case Msg_SSEIOL1_Static:
		headerList << "索引" << "QDSTime" << "Symbol" << "Reserved" << "ContractID"
			<< "SecurityName" << "SymbolUnderlying" << "NameUnderlying" << "UnderlyingType" << "OptionType"
			<< "CallOrPut" << "ContractMultiplierUnit" << "ExercisePrice" << "StartDate" << "EndDate"
			<< "ExerciseDate" << "DeliveryDate" << "ExpireDate" << "Version" << "TotalLongPosition"
			<< "PreClosePrice" << "PreSettlePrice" << "PreClosePriceUnderlying" << "UpDownLimitType" << "PriceUpLimit"
			<< "PriceDownLimit" << "MarginUnit" << "MarginRatioParam1" << "MarginRatioParam2" << "RoundLot"
			<< "LmtOrdFloor" << "LmtOrdCeiling" << "MktOrdFloor" << "MktOrdCeiling" << "TickSize"
			<< "SecurityStatusFlag" << "AutoSplitDate";
		break;
	case Msg_SSEIOL1_Quotation:
		headerList << "索引" << "QDSTime" << "Symbol" << "Reserved" << "Time"
			<< "PreSettlePrice" << "SettlePrice" << "OpenPrice" << "HighPrice" << "LowPrice"
			<< "LastPrice" << "ClosePrice" << "AuctionPrice" << "AuctionVolume" << "TotalPosition"
			<< "SellLevelNo" << "SellPrice01" << "SellVolume01" << "SellPrice02" << "SellVolume02"
			<< "SellPrice03" << "SellVolume03" << "SellPrice04" << "SellVolume04" << "SellPrice05"
			<< "SellVolume05" << "BuyLevelNo" << "BuyPrice01" << "BuyVolume01" << "BuyPrice02"
			<< "BuyVolume02" << "BuyPrice03" << "BuyVolume03" << "BuyPrice04" << "BuyVolume04"
			<< "BuyPrice05" << "BuyVolume05" << "TotalVolume" << "TotalAmount" << "SecurityPhaseTag";
		break;
    case Msg_SZSEL2_Static:
		headerList << "索引" << "QDSTime" << "Symbol" << "SecurityName" << "SymbolSource"
			<< "SecurityEN" << "ISINCode" << "SymbolUnderlying" << "UnderlyingSecurityIDSource" << "SecurityType"
			<< "SecurityStatusTag" << "PreClosePrice" << "ListingDate" << "Currency" << "ParValue"
			<< "IssuedVolume" << "OutstandingShare" << "IndustryType" << "PreYearEPS" << "YearEPS"
			<< "OfferingFlag" << "NAV" << "CouponRate" << "IssuePrice" << "Interest"
			<< "InterestAccrualDate" << "MaturityDate" << "ConversionPrice" << "ConversionRatio" << "ConversionBeginDate"
			<< "ConversionEndDate" << "CallOrPut" << "WarrantClearingType" << "ClearingPrice" << "OptionType"
			<< "EndDate" << "ExpirationDays" << "DayTrading" << "GageFlag" << "GageRate"
			<< "CrdBuyUnderlying" << "CrdSellUnderlying" << "CrdPriceCheckType" << "PledgeFlag" << "ContractMultiplier"
			<< "RegularShare" << "QualificationFlag" << "MarketMakerFlag" << "RoundLot" << "TickSize"
			<< "BuyQtyUpperLimit" << "SellQtyUpperLimit" << "BuyVolumeUnit" << "SellVolumeUnit" << "LimitUpRateO"
			<< "LimitDownRateO" << "LimitUpAbsoluteO" << "LimitDownAbsoluteO" << "AuctionUpDownRateO" << "AuctionUpDownAbsoluteO"
			<< "LimitUpRateT" << "LimitDownRateT" << "LimitUpAbsoluteT" << "LimitDownAbsoluteT" << "AuctionUpDownRateT"
			<< "AuctionUpDownAbsoluteT" << "LimitUpRateC" << "LimitDownRateC" << "LimitUpAbsoluteC" << "LimitDownAbsoluteC"
			<< "AuctionUpDownRateC" << "AuctionUpDownAbsoluteC";
        break;
    case Msg_SZSEL2_Quotation:
        headerList <<"索引" << "QDSTime" << "Symbol" << "Time"<< "SymbolSource"
			<< "PreClosePrice" << "OpenPrice" << "LastPrice" << "HighPrice" << "LowPrice" 
			<< "PriceUpLimit" << "PriceDownLimit" << "PriceUpdown1" << "PriceUpdown2" << "TotalNo"
			<< "TotalVolume" << "TotalAmount" << "ClosePrice" << "SecurityPhaseTag" << "PERatio1"
			<< "NAV" << "PERatio2" << "IOPV" << "PremiumRate" << "TotalSellOrderVolume"
			<< "WtAvgSellPrice" << "SellLevelNo" << "SellPrice01" << "SellVolume01" << "TotalSellOrderNo01"
			<< "SellPrice02" << "SellVolume02" << "TotalSellOrderNo02" << "SellPrice03" << "SellVolume03"
			<< "TotalSellOrderNo03" << "SellPrice04" << "SellVolume04" << "TotalSellOrderNo04" << "SellPrice05"
			<< "SellVolume05" << "TotalSellOrderNo05" << "SellPrice06" << "SellVolume06" << "TotalSellOrderNo06"
			<< "SellPrice07" << "SellVolume07" << "TotalSellOrderNo07" << "SellPrice08" << "SellVolume08"
			<< "TotalSellOrderNo08" << "SellPrice09" << "SellVolume09" << "TotalSellOrderNo09" << "SellPrice10"
			<< "SellVolume10" << "TotalSellOrderNo10" << "SellLevelQueueNo01" << "SellLevelQueue" << "TotalBuyOrderVolume"
			<< "WtAvgBuyPrice" << "BuyLevelNo" << "BuyPrice01" << "BuyVolume01" << "TotalBuyOrderNo01"
			<< "BuyPrice02" << "BuyVolume02" << "TotalBuyOrderNo02" << "BuyPrice03" << "BuyVolume03"
			<< "TotalBuyOrderNo03" << "BuyPrice04" << "BuyVolume04" << "TotalBuyOrderNo04" << "BuyPrice05"
			<< "BuyVolume05" << "TotalBuyOrderNo05" << "BuyPrice06" << "BuyVolume06" << "TotalBuyOrderNo06"
			<< "BuyPrice07" << "BuyVolume07" << "TotalBuyOrderNo07" << "BuyPrice08" << "BuyVolume08"
			<< "TotalBuyOrderNo08" << "BuyPrice09" << "BuyVolume09" << "TotalBuyOrderNo09" << "BuyPrice10"
			<< "BuyVolume10" << "TotalBuyOrderNo10" << "BuyLevelQueueNo01" << "BuyLevelQueue";
        break;
    case Msg_SZSEL2_Transaction:
        headerList <<"索引" << "QDSTime" << "Symbol" << "SetID" << "RecID"
			<< "BuyOrderID" << "SellOrderID" << "SymbolSource" <<"TradeTime" << "TradePrice" 
			<< "TradeVolume" << "TradeType";
        break;
    case Msg_SZSEL2_Index:
        headerList <<"索引" << "QDSTime" << "Symbol" << "Time" << "SymbolSource"
			<< "PreClosePrice" << "OpenPrice" << "HighPrice" << "LowPrice" << "LastPrice"
			<< "TotalAmount" << "TotalNo" << "TotalVolume" << "SecurityPhaseTag" << "SampleNo";		
        break;
    case Msg_SZSEL2_Order:
        headerList <<"索引" << "QDSTime" << "Symbol" << "SetID" << "RecID" 
			<< "SymbolSource" << "Time"  << "OrderPrice" << "OrderVolume" << "OrderCode" 
			<< "OrderType";
        break;
    case Msg_SZSEL2_Status:
        headerList <<"索引" << "QDSTime" << "Symbol" << "SymbolSource" << "Time" 
			<< "FinancialStatus" << "CrdBuyStatus" << "CrdSellStatus" << "SubscribeStatus" << "RedemptionStatus"
			<< "PurchasingStatus" << "StockDiviStatus" << "PutableStatus" << "ExerciseStatus" << "GoldPurchase"
			<< "GoldRedemption" << "AcceptedStatus" << "ReleaseStatus" << "CancStockDiviStatus" << "CancPutableStatus"
			<< "PledgeStatus" << "RemovePledge" << "VoteStatus" << "StockPledgeRepo" << "DivideStatus"
			<< "MergerStatus";
        break;

    default:
        break;
    }

    pMdiSubView = new MdiSubWindow((MsgType)msgType , ui->mdiArea);
	pMdiSubView->initTable(this, headerList);
	m_mdiSubViewTable[(MsgType)msgType] = pMdiSubView;


	if (m_lastComboShowStyleIndex == 0)	//MDI子窗口显示
	{
		ui->mdiArea->addSubWindow(pMdiSubView);
		pMdiSubView->show();
		ui->mdiArea->setViewMode(QMdiArea::SubWindowView);
		ui->mdiArea->tileSubWindows();
	}
    else if (m_lastComboShowStyleIndex == 1)	//Tab标签页显示
    {
		ui->mdiArea->addSubWindow(pMdiSubView);
		pMdiSubView->show();
        ui->mdiArea->setViewMode(QMdiArea::TabbedView);
        pMdiSubView->showMaximized();
    }
	else if (m_lastComboShowStyleIndex == 2)	//独立窗口显示
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

void MainWindow::onTableAddingNews(MsgType msgType, QStringList news, int delay)
{
	mutex.lock();
	MdiSubWindow *pMdiSubView = m_mdiSubViewTable[msgType];
	if (pMdiSubView != NULL)
		pMdiSubView->onTableAddingNews(news, delay);
	mutex.unlock();
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

		if (i == 5)
			i = i + 2;	//跳过上股期权的两个
    }

}
