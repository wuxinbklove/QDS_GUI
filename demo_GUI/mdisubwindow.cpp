#include "mdisubwindow.h"
#include "mainwindow.h"
#include <QMutex>

static QMutex mutex;

MdiSubWindow::MdiSubWindow(MsgType msgType, QWidget *parent) :
	QWidget(parent),
    ui(new Ui::MdiSubWindow)
{
    ui->setupUi(this);
    m_msgType = msgType;
    m_workState = Working;
    m_curDataIndex = 0;
    m_pDataList = new QList<QStringList>();
	m_pTableData = new QList<QStringList>();
    m_pTableModel = NULL;
    m_timer.setInterval(50);
    m_timer.start();
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


void MdiSubWindow::onTableAddingNews(QStringList news)
{
	mutex.lock();
    m_pDataList->append(news);
	mutex.unlock();
}

void MdiSubWindow::onTimeout()
{
    if (m_pTableModel == NULL)
        return;
    if (m_workState == Pause)
        return;
    for (int i = 0 ; i< m_pDataList->count(); ++i)
    {
        QStandardItem *pNewItem = new QStandardItem(QString::number(++m_curDataIndex));
        pNewItem->setTextAlignment(Qt::AlignCenter);
		if (m_symbolList.count() > 0 && m_symbolList.indexOf((*m_pDataList)[i][1]) < 0)
			continue;
		m_pTableModel->insertRow(0);
        m_pTableModel->setItem(0, 0, pNewItem); //第一列预留给了索引
        for(int j = 0; j< (*m_pDataList)[i].count(); ++j)
        {
            pNewItem = new QStandardItem((*m_pDataList)[i][j]);
            pNewItem->setTextAlignment(Qt::AlignCenter);
            m_pTableModel->setItem(0, j + 1, pNewItem);
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
}

void MdiSubWindow::closeEvent(QCloseEvent *event)
{
    if (m_mainWindow != NULL)
        m_mainWindow->needUnsubscribe(m_msgType , "");
    event->accept();
}

void MdiSubWindow::on_button_symbol_clicked()
{
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
		QStandardItem *pNewItem = new QStandardItem(QString::number(m_curDataIndex - m_pTableData->count() + i) + 1);
		pNewItem->setTextAlignment(Qt::AlignCenter);
		if (m_symbolList.count() > 0 &&  m_symbolList.indexOf((*m_pTableData)[i][1]) < 0)
			continue;
		m_pTableModel->insertRow(0);
		m_pTableModel->setItem(0, 0, pNewItem); //第一列预留给了索引
		for (int j = 0; j< (*m_pTableData)[i].count(); ++j)
		{
			pNewItem = new QStandardItem((*m_pTableData)[i][j]);
			pNewItem->setTextAlignment(Qt::AlignCenter);
			m_pTableModel->setItem(0, j + 1, pNewItem);
		}
	}
    m_workState = Working;
}
