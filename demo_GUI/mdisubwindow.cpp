#include "mdisubwindow.h"
#include "mainwindow.h"
#include <QMutex>
#include <QCheckBox>
#include <QDebug>
#include <QDateTime>
#include <QTextStream>
#include <QDir>

static QMutex mutex;
static QMutex mutex2;

MdiSubWindow::MdiSubWindow(MsgType msgType, QWidget *parent) :
	QWidget(parent),
    ui(new Ui::MdiSubWindow),
	m_settings(qApp->applicationDirPath() + "/config/guiconfig.ini", QSettings::IniFormat)
{
    ui->setupUi(this);
    m_msgType = msgType;
    m_workState = Working;
    m_curDataIndex = 0;
    m_pDataList = new QList<QStringList>();
	m_pTableData = new QList<QStringList>();
    m_pTableModel = NULL;
	m_headConfig = "111";
    m_timer.setInterval(100);
    m_timer.start();
	m_lastCalcDelay = INT_MAX;
    connect(&m_timer, SIGNAL(timeout()) , this , SLOT(onTimeout()));
	switch (msgType)
	{
	case Msg_SSEL2_Static:
		this->setWindowTitle("SSEL2_Static");
		break;
	case Msg_SSEL2_Quotation:
		this->setWindowTitle("SSEL2_Quotation");
		break;
	case Msg_SSEL2_Transaction:
		this->setWindowTitle("SSEL2_Transaction");
		break;
	case Msg_SSEL2_Index:
		this->setWindowTitle("SSEL2_Index");
		break;
	case Msg_SSEL2_Auction:
		this->setWindowTitle("SSEL2_Auction");
		break;
	case Msg_SSEL2_Overview:
		this->setWindowTitle("SSEL2_Overview");
		break;
	case Msg_SSEIOL1_Static:
		this->setWindowTitle("SSEIOL1_Static");
		break;
	case Msg_SSEIOL1_Quotation:
		this->setWindowTitle("SSEIOL1_Quotation");
		break;
	case Msg_SZSEL2_Static:
		this->setWindowTitle("SZSEL2_Static");
		break;
	case Msg_SZSEL2_Quotation:
		this->setWindowTitle("SZSEL2_Quotation");
		break;
	case Msg_SZSEL2_Transaction:
		this->setWindowTitle("SZSEL2_Transaction");
		break;
	case Msg_SZSEL2_Index:
		this->setWindowTitle("SZSEL2_Index");
		break;
	case Msg_SZSEL2_Order:
		this->setWindowTitle("SZSEL2_Order");
		break;
	case Msg_SZSEL2_Status:
		this->setWindowTitle("SZSEL2_Status");
		ui->label_showDelay->hide();
		break;
	default:
		break;
	}

	this->show();
}


MdiSubWindow::~MdiSubWindow()
{
    delete ui;
}


void MdiSubWindow::initTable(MainWindow* mainWindow, QStringList headList)
{
	m_headList = headList;
	m_headConfig = m_settings.value(QString("headConfig/") + windowTitle()).toString();
	ui->combo_selectColumn->initCombo(m_headList, m_headConfig);
	m_headConfig = ui->combo_selectColumn->getSelectItems();


	m_pTableModel = new QStandardItemModel();
	headList.clear();
	for (int i = 0; i < m_headList.count(); i++)
	{
		if (m_headConfig[i] == '1')
			headList << m_headList[i];
	}
	m_pTableModel->setHorizontalHeaderLabels(headList);
	ui->tableView->setModel(m_pTableModel);
	ui->tableView->setColumnWidth(0, 50);
	ui->tableView->horizontalHeader()->setSectionsMovable(true);
	m_mainWindow = mainWindow;

	QString fileName = m_mainWindow->m_delayLogPath + windowTitle() + ".log";
	m_pLogFile = new QFile(fileName);

}




void MdiSubWindow::onTableAddingNews(QStringList news, int delay)
{
	mutex.lock();
    m_pDataList->append(news);
	m_tmpDelayList.append(delay);
	long long totalNum = 0;
	int minNum = INT_MAX;
	if (m_tmpDelayList.count() == 100)
	{
		for (auto iter = m_tmpDelayList.begin(); iter != m_tmpDelayList.end(); ++iter)
		{
			totalNum += *iter;
			if (minNum > *iter)
				minNum = *iter;
		}
		int averageNum = totalNum / 100;
		m_lastCalcDelay = averageNum - minNum + 5;
		m_tmpDelayList.clear();
		if (m_lastCalcDelay < INT_MAX && m_pLogFile != NULL)
		{
			bool isOpen = m_pLogFile->open(QIODevice::WriteOnly | QIODevice::Append);
			if (isOpen)
			{
				QTextStream stream(m_pLogFile);
				stream << QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss.zzz -- ") << QString::number(m_lastCalcDelay) << " ms \r\n";
				m_pLogFile->close();
			}
			else
			{
				qDebug() << m_pLogFile->error();
			}
		}
	}
	mutex.unlock();
}

void MdiSubWindow::onTimeout()
{
	mutex.lock();
	if (m_pTableModel == NULL)
	{
		mutex.unlock();
		return;
	}
	if (m_workState == Pause)
	{
		mutex.unlock();
		return;
	}
       
	if (m_lastCalcDelay == INT_MAX)
	{
		ui->label_showDelay->setText("Ping: -- ms");
		ui->label_showDelay->setStyleSheet("color: grey");
	}
	else
	{
		ui->label_showDelay->setText(QString("Ping: ") + QString::number(m_lastCalcDelay) + "ms");
		if (m_lastCalcDelay < 50)
			ui->label_showDelay->setStyleSheet("color:green");
		else if (m_lastCalcDelay < 200)
			ui->label_showDelay->setStyleSheet("color:orange");
		else
			ui->label_showDelay->setStyleSheet("color:red");
	}

    for (int i = 0 ; i< m_pDataList->count(); ++i)
    {
		int k = 0;
        QStandardItem *pNewItem = new QStandardItem(QString::number(++m_curDataIndex));
        pNewItem->setTextAlignment(Qt::AlignCenter);
		if (m_symbolList.count() > 0 && m_symbolList.indexOf((*m_pDataList)[i][1]) < 0)
			continue;
		m_pTableModel->insertRow(0);
		if (m_headConfig[0] == '1')
		{
			m_pTableModel->setItem(0, 0, pNewItem); //第一列预留给了索引
			++k;
		}

        for(int j = 0; j< (*m_pDataList)[i].count(); ++j)
        {
			if (m_headConfig.length() > (j + 1) && m_headConfig[j + 1] == '1')
			{
				pNewItem = new QStandardItem((*m_pDataList)[i][j]);
				pNewItem->setTextAlignment(Qt::AlignCenter);
				m_pTableModel->setItem(0, k, pNewItem);
				++k;
			}

        }
    }
	m_pTableData->append(*m_pDataList);
    m_pDataList->clear();
    if (m_pTableModel->rowCount() > MAXTABLEROWCOUNT)
        m_pTableModel->setRowCount(MAXTABLEROWCOUNT);
	if (m_pTableData->count() > MAXTABLEROWCOUNT)
	{
		m_pTableData->erase(m_pTableData->begin(), m_pTableData->begin() + m_pTableData->count() - MAXTABLEROWCOUNT);
	}
	mutex.unlock();
}

void MdiSubWindow::closeEvent(QCloseEvent *event)
{
    if (m_mainWindow != NULL)
        m_mainWindow->needUnsubscribe(m_msgType , "");
    event->accept();
}

void MdiSubWindow::on_button_symbol_clicked()
{
	mutex.lock();

    m_headConfig = ui->combo_selectColumn->getSelectItems();
	m_settings.setValue(QString("headConfig/") + windowTitle(), m_headConfig);

	QStringList headList;
	for (int i = 0; i < m_headList.count(); i++)
	{
		if (m_headConfig[i] == '1')
			headList << m_headList[i];
	}
	m_pTableModel->clear();
	m_pTableModel->setHorizontalHeaderLabels(headList);
	ui->tableView->setColumnWidth(0, 50);

    QString symbolStr = ui->lineEdit_symbol->text();
    symbolStr= symbolStr.simplified();
    symbolStr.replace(" ","");
    symbolStr.replace("\357\274\214",",");
    m_symbolList = symbolStr.split(",");
	if (symbolStr.length() == 0)
		m_symbolList.clear();
    m_workState = Pause;
	m_pTableModel->removeRows(0, m_pTableModel->rowCount());


	for (int i = 0; i < m_pTableData->count(); i++)
	{
		int k = 0;
		QStandardItem *pNewItem = new QStandardItem(QString::number(m_curDataIndex - m_pTableData->count() + i) + 1);
		pNewItem->setTextAlignment(Qt::AlignCenter);
		if (m_symbolList.count() > 0 &&  m_symbolList.indexOf((*m_pTableData)[i][1]) < 0)
			continue;
		m_pTableModel->insertRow(0);
		if (m_headConfig[0] == '1')
		{
			m_pTableModel->setItem(0, 0, pNewItem); //第一列预留给了索引
			++k;
		}

		for (int j = 0; j< (*m_pTableData)[i].count(); ++j)
		{
			if (m_headConfig.length() > (j + 1) && m_headConfig[j + 1] == '1')
			{
				pNewItem = new QStandardItem((*m_pTableData)[i][j]);
				pNewItem->setTextAlignment(Qt::AlignCenter);
				m_pTableModel->setItem(0, k, pNewItem);
				++k;
			}

		}
	}
    m_workState = Working;
	mutex.unlock();
}
