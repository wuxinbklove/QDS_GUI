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
#include <QTimer>
#include "ui_loginwindow.h"

static QMutex mutex;

const static QList<int> exchangeEnum = {0x00102000, //�Ͻ���L2��̬����
																	0x00102001, //�Ͻ���L2ʵʱ����
																	0x00102002, //�Ͻ���L2��ʳɽ�
																	0x00102003, //�Ͻ���L2ָ������
																	0x00102004, //�Ͻ���L2���⼯�Ͼ���
																	0x00102005, //�Ͻ���L2�г�����
																	0x00103000,  //�Ͻ���������Ȩ��̬����
																	0x00103001,  //�Ͻ���������Ȩʵʱ����
																	0x00202000, //���L2��̬����
																	0x00202001, //���L2ʵʱ����
																	0x00202002, //���L2��ʳɽ�
																	0x00202003, //���L2ָ������
																	0x00202006, //���L2���ί��
																	0x00202007 //���L2֤ȯ״̬
																	};

//��¼�߳�
class LoginThread : public  QThread
{

public:
	LoginThread(LoginWindow::LoginData loginData ,QObject *parent = NULL);
	RetCode getResult() { return m_result; }
	~LoginThread();

private:
	LoginWindow::LoginData m_loginData;
	RetCode m_result;
	QTimer timer;

protected:
	void run();
};

LoginThread::LoginThread(LoginWindow::LoginData loginData, QObject *)
{
	m_loginData = loginData;
	m_result = Ret_OutTime;
	timer.setSingleShot(true);
	connect(&timer, &QTimer::timeout, [=]() {this->terminate(); this->wait();});
	timer.start(30000);
}

LoginThread::~LoginThread()
{
	if (timer.isActive())
		timer.stop();
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

	//������־�ļ��У����ݵ�ǰʱ��������
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

    if (index == 0)		//MDI�Ӵ���
    {
        ui->mdiArea->setViewMode(QMdiArea::SubWindowView);
        ui->button_autoLayout->show();
        ui->button_autoStack->show();
		ui->mdiArea->tileSubWindows();
    }
    else if (index == 1)		//Tab��ǩҳ
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
    else		//��������
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

	LoginWindow *pLoginWindow = (LoginWindow *)sender();

	QEventLoop loginLoop;
	LoginThread loginThread(loginData);
	loginThread.start();
	//bool isThreadFinish = loginThread.wait(30000);
	connect(&loginThread, &LoginThread::finished, [&]() {loginLoop.quit(); });
	pLoginWindow->setEnabled(false);
	pLoginWindow->ui->button_getKey->setStyleSheet("color:grey;border:none;");
	loginThread.start();
	loginLoop.exec();
	pLoginWindow->setEnabled(true);
	pLoginWindow->ui->button_getKey->setStyleSheet("color:blue;border:none;");
	RetCode ret = loginThread.getResult();
	if (ret != Ret_Success)
	{
		QMessageBox::critical(pLoginWindow, QString::fromStdWString(L"��¼ʧ��"), QString("Login Failed, RetCode = ") + QString::number((int)ret));
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
    QStringList headerList;	//��ͷ��
    switch ((MsgType)msgType)
    {
    case Msg_SSEL2_Static:
        headerList <<QString::fromWCharArray(L"����") << "QDSTime" << "Symbol" << "Reserved" << "Time"
			<< "ISINCode" << "SecurityName" << "SecurityEN" << "SymbolUnderlying" << "MarketType"
			<< "CFICode" << "SecuritySubType" << "Currency" << "ParValue" << "TradableNo" 
            << "EndDate" << "ListingDate" << "SetNo"  << "BuyVolumeUnit" << "SellVolumeUnit"
            << "DeclareVolumeFloor" << "DeclareVolumeCeiling" << "PreClosePrice"<< "TickSize" << "UpDownLimitType"
            << "PriceUpLimit" << "PriceDownLimit" << "XRRatio" << "XDAmount" << "CrdBuyUnderlying"
            << "CrdSellUnderlying" << "SecurityStatus" << "SampleNo" << "SampleAvgPrice" << "TradeAmount"
            << "AvgCapital" <<"TotalMarketValue" << "MarketValueRatio" << "StaticPERatio" << "IndexLevelStatus"
            << "TradeDate";
        break;
    case Msg_SSEL2_Quotation:
		headerList << QString::fromWCharArray(L"����") << "QDSTime" << "Symbol" << "Reserved" << "Time"
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
        headerList << QString::fromWCharArray(L"����") << "QDSTime" << "Symbol" << "Reserved" << "TradeTime"
			<< "RecID" << "TradeChannel" << "TradePrice" << "TradeVolume" << "TradeAmount"
			<< "BuyRecID" << "SellRecID" << "BuySellFlag";
        break;
    case Msg_SSEL2_Auction:
        headerList << QString::fromWCharArray(L"����") << "QDSTime" << "Symbol" << "Reserved" <<"Time"
			<< "OpenPrice" << "AuctionVolume" << "LeaveVolume" << "side";
        break;
    case Msg_SSEL2_Index:
        headerList << QString::fromWCharArray(L"����") << "QDSTime" << "Symbol" << "Reserved" << "Time"
			<< "TradeTime" << "PreClosePrice"<<"OpenPrice" << "TotalAmount" << "HighPrice" 
			<< "LowPrice" << "LastPrice" << "TotalVolume" << "ClosePrice";
        break;
    case Msg_SSEL2_Overview:
        headerList << QString::fromWCharArray(L"����") << "QDSTime" << "Symbol" << "Reserved" << "Time"
			<< "MarketTime" << "TradeDate";
        break;
	case Msg_SSEIOL1_Static:
		headerList << QString::fromWCharArray(L"����") << "QDSTime" << "Symbol" << "Reserved" << "ContractID"
			<< "SecurityName" << "SymbolUnderlying" << "NameUnderlying" << "UnderlyingType" << "OptionType"
			<< "CallOrPut" << "ContractMultiplierUnit" << "ExercisePrice" << "StartDate" << "EndDate"
			<< "ExerciseDate" << "DeliveryDate" << "ExpireDate" << "Version" << "TotalLongPosition"
			<< "PreClosePrice" << "PreSettlePrice" << "PreClosePriceUnderlying" << "UpDownLimitType" << "PriceUpLimit"
			<< "PriceDownLimit" << "MarginUnit" << "MarginRatioParam1" << "MarginRatioParam2" << "RoundLot"
			<< "LmtOrdFloor" << "LmtOrdCeiling" << "MktOrdFloor" << "MktOrdCeiling" << "TickSize"
            << "SecurityStatusFlag" << "AutoSplitDate" << "TradeDate";
		break;
	case Msg_SSEIOL1_Quotation:
		headerList << QString::fromWCharArray(L"����") << "QDSTime" << "Symbol" << "Reserved" << "Time"
			<< "PreSettlePrice" << "SettlePrice" << "OpenPrice" << "HighPrice" << "LowPrice"
			<< "LastPrice" << "ClosePrice" << "AuctionPrice" << "AuctionVolume" << "TotalPosition"
			<< "SellLevelNo" << "SellPrice01" << "SellVolume01" << "SellPrice02" << "SellVolume02"
			<< "SellPrice03" << "SellVolume03" << "SellPrice04" << "SellVolume04" << "SellPrice05"
			<< "SellVolume05" << "BuyLevelNo" << "BuyPrice01" << "BuyVolume01" << "BuyPrice02"
			<< "BuyVolume02" << "BuyPrice03" << "BuyVolume03" << "BuyPrice04" << "BuyVolume04"
            << "BuyPrice05" << "BuyVolume05" << "TotalVolume" << "TotalAmount" << "SecurityPhaseTag"
            << "WtAvgRate" << "WtAvgRateUpdown" << "PreWtAvgRate";
		break;
    case Msg_SZSEL2_Static:
		headerList << QString::fromWCharArray(L"����") << "QDSTime" << "Symbol" << "SecurityName" << "SymbolSource"
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
            << "AuctionUpDownRateC" << "AuctionUpDownAbsoluteC" << "TradeDate" << "QualificationClass";
        break;
    case Msg_SZSEL2_Quotation:
        headerList << QString::fromWCharArray(L"����") << "QDSTime" << "Symbol" << "Time"<< "SymbolSource"
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
            << "BuyVolume10" << "TotalBuyOrderNo10" << "BuyLevelQueueNo01" << "BuyLevelQueue" << "WtAvgRate"
            << "WtAvgRateUpdown" << "PreWtAvgRate";
        break;
    case Msg_SZSEL2_Transaction:
        headerList << QString::fromWCharArray(L"����") << "QDSTime" << "Symbol" << "SetID" << "RecID"
			<< "BuyOrderID" << "SellOrderID" << "SymbolSource" <<"TradeTime" << "TradePrice" 
			<< "TradeVolume" << "TradeType";
        break;
    case Msg_SZSEL2_Index:
        headerList << QString::fromWCharArray(L"����") << "QDSTime" << "Symbol" << "Time" << "SymbolSource"
			<< "PreClosePrice" << "OpenPrice" << "HighPrice" << "LowPrice" << "LastPrice"
			<< "TotalAmount" << "TotalNo" << "TotalVolume" << "SecurityPhaseTag" << "SampleNo";		
        break;
    case Msg_SZSEL2_Order:
        headerList << QString::fromWCharArray(L"����") << "QDSTime" << "Symbol" << "SetID" << "RecID"
			<< "SymbolSource" << "Time"  << "OrderPrice" << "OrderVolume" << "OrderCode" 
			<< "OrderType";
        break;
    case Msg_SZSEL2_Status:
        headerList << QString::fromWCharArray(L"����") << "QDSTime" << "Symbol" << "SymbolSource" << "Time"
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


	if (m_lastComboShowStyleIndex == 0)	//MDI�Ӵ�����ʾ
	{
		ui->mdiArea->addSubWindow(pMdiSubView);
		pMdiSubView->show();
		ui->mdiArea->setViewMode(QMdiArea::SubWindowView);
		ui->mdiArea->tileSubWindows();
	}
    else if (m_lastComboShowStyleIndex == 1)	//Tab��ǩҳ��ʾ
    {
		ui->mdiArea->addSubWindow(pMdiSubView);
		pMdiSubView->show();
        ui->mdiArea->setViewMode(QMdiArea::TabbedView);
        pMdiSubView->showMaximized();
    }
	else if (m_lastComboShowStyleIndex == 2)	//����������ʾ
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
			i = i + 2;	//�����Ϲ���Ȩ������
    }

}
