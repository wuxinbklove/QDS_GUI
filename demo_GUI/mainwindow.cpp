#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QDSApi.h"
#include "QDSStruct.h"
#include <QMessageBox>
#include <QDebug>
#include <QStringList>
#include <QMutex>


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

QMutex mutex;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_pDataList = new QList<QStringList>();
    m_pTableModel = NULL;
    m_timer.setInterval(100);
    m_timer.start();
    connect(&m_timer, SIGNAL(timeout()) , this , SLOT(onTimeout()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_button_confirm_clicked()
{
    UnsubscribeAll();
    const char* pCode = NULL;
    QString codeText = ui->lineEdit_code->text();
    if (!codeText.isEmpty())
        pCode = codeText.toStdString().c_str();
    int msgType = exchangeEnum[ui->combo_exchange->currentIndex()];
    Subscribe((MsgType)msgType, (char *)pCode);
    qDebug() << QString("Subscirbe---MsgType:") << QString::number(msgType) << QString("  Code:") << codeText;
    QStringList headerList;
    switch ((MsgType)msgType)
    {
    case Msg_SSEL2_Static:
        headerList << "QDSTime" << "Time" << "Symbol" << "SampleAvgPrice";
        break;
    case Msg_SSEL2_Quotation:
        headerList << "QDSTime" << "Time" << "Symbol" << "OpenPrice" << "HighPrice" << "LowPrice" << "LastPrice";
        break;
    case Msg_SSEL2_Transaction:
        headerList << "QDSTime" << "Symbol" << "TradePrice";
        break;
    case Msg_SSEL2_Auction:
        headerList << "QDSTime" << "Time" << "Symbol" << "OpenPrice" << "AuctionVolume" << "LeaveVolume";
        break;
    case Msg_SSEL2_Index:
        headerList << "QDSTime" << "Time" << "Symbol" << "OpenPrice" << "HighPrice" << "LowPrice" << "LastPrice";
        break;
    case Msg_SSEL2_Overview:
        headerList << "QDSTime" << "Time" << "Symbol" << "TradeDate" << "TradeTime";
        break;
    case Msg_SZSEL2_Static:
        headerList << "QDSTime" << "Symbol" << "IssuedVolume" << "OutstandingShare" << "LimitUpAbsoluteT" << "LimitDownAbsoluteT";
        break;
    case Msg_SZSEL2_Quotation:
        headerList << "QDSTime" << "Time" << "Symbol" << "OpenPrice" << "HighPrice" << "LowPrice" << "LastPrice";
        break;
    case Msg_SZSEL2_Transaction:
        headerList << "QDSTime" << "Symbol" << "TradeTime" << "TradePrice" << "TradeVolume" << "TradeType";
        break;
    case Msg_SZSEL2_Index:
        headerList << "QDSTime" << "Time" << "Symbol" << "OpenPrice" << "HighPrice" << "LowPrice" << "LastPrice";
        break;
    case Msg_SZSEL2_Order:
        headerList << "QDSTime" << "Time" << "Symbol" << "OrderPrice" << "OrderVolume" << "OrderCode" << "OrderType";
        break;
    case Msg_SZSEL2_Status:
        headerList << "QDSTime" << "Time" << "Symbol" << "FinancialStatus";
        break;

    default:
        break;
    }

    if (m_pTableModel != NULL)
    {
        QStandardItemModel *tmp = m_pTableModel;
        m_pTableModel = NULL;
        delete tmp;
    }

    m_pDataList->clear();

    m_pTableModel = new QStandardItemModel();
    m_pTableModel->setColumnCount(headerList.count());
    m_pTableModel->setHorizontalHeaderLabels(headerList);
    ui->tableView->setModel(m_pTableModel);
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

void MainWindow::onTableAddingNews(QStringList news)
{
    mutex.lock();
    m_pDataList->append(news);
    mutex.unlock();
}

void MainWindow::onTimeout()
{
    if (m_pTableModel == NULL)
        return;
    for (int i = 0 ; i< m_pDataList->count(); ++i)
    {
        m_pTableModel->insertRow(0);
        for(int j = 0; j< (*m_pDataList)[i].count(); ++j)
        {
            m_pTableModel->setItem(0, j, new QStandardItem((*m_pDataList)[i][j]));
        }
    }
    m_pDataList->clear();
    if (m_pTableModel->rowCount() > 1000)
        m_pTableModel->setRowCount(1000);
    ui->tableView->repaint();
    //qApp->processEvents();
}

