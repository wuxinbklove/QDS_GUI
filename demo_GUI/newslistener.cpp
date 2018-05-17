#include "newslistener.h"
#include <QDebug>


namespace NewsListener {
    MainWindow *g_pMainWindow = NULL;

    void OnSubscribe_SZSEL2_Static(const void* RealSZSEL2Static)
    {
        DealSubscribe(Msg_SZSEL2_Static, RealSZSEL2Static);
    }

    void OnSubscribe_SZSEL2_Quotation(const void* RealSSEL2Quotation)
    {
        DealSubscribe(Msg_SZSEL2_Quotation, RealSSEL2Quotation);
    }

    void OnSubscribe_SZSEL2_Transaction(const void* RealSZSEL2Transaction)
    {
        DealSubscribe(Msg_SZSEL2_Transaction, RealSZSEL2Transaction);
    }

    void OnSubscribe_SZSEL2_Index(const void* RealSZSEL2Index)
    {
        DealSubscribe(Msg_SZSEL2_Index, RealSZSEL2Index);
    }

    void OnSubscribe_SZSEL2_Order(const void* RealSZSEL2Order)
    {
        DealSubscribe(Msg_SZSEL2_Order, RealSZSEL2Order);
    }

    void OnSubscribe_SZSEL2_Status(const void* RealSZSEL2Status)
    {
        DealSubscribe(Msg_SZSEL2_Status, RealSZSEL2Status);
    }

    void OnSubscribe_SSEL2_Quotation(const void* RealSSEL2Quotation)
    {
        DealSubscribe(Msg_SSEL2_Quotation, RealSSEL2Quotation);
    }

    void OnSubscribe_SSEL2_Transaction(const void* RealSSEL2Transaction)
    {
        DealSubscribe(Msg_SSEL2_Transaction, RealSSEL2Transaction);
    }

    void OnSubscribe_SSEL2_Auction(const void* RealSSEL2Auction)
    {
        DealSubscribe(Msg_SSEL2_Auction, RealSSEL2Auction);
    }

    void OnSubscribe_SSEL2_Index(const void* RealSSEL2Index)
    {
        DealSubscribe(Msg_SSEL2_Index, RealSSEL2Index);
    }

    void OnSubscribe_SSEL2_Overview(const void* RealSSEL2Overview)
    {
        DealSubscribe(Msg_SSEL2_Overview, RealSSEL2Overview);
    }

    void OnSubscribe_SSEL2_Static(const void* RealSSEL2Static)
    {
        DealSubscribe(Msg_SSEL2_Static, RealSSEL2Static);
    }

    void InitSubscribe(MainWindow* mainWindow)
    {
        g_pMainWindow = mainWindow;

        setFuncOnData(Msg_SZSEL2_Static, OnSubscribe_SZSEL2_Static);
        setFuncOnData(Msg_SZSEL2_Quotation, OnSubscribe_SZSEL2_Quotation);
        setFuncOnData(Msg_SZSEL2_Transaction, OnSubscribe_SZSEL2_Transaction);
        setFuncOnData(Msg_SZSEL2_Index, OnSubscribe_SZSEL2_Index);
        setFuncOnData(Msg_SZSEL2_Order, OnSubscribe_SZSEL2_Order);
        setFuncOnData(Msg_SZSEL2_Status, OnSubscribe_SZSEL2_Status);
        setFuncOnData(Msg_SSEL2_Quotation, OnSubscribe_SSEL2_Quotation);
        setFuncOnData(Msg_SSEL2_Transaction, OnSubscribe_SSEL2_Transaction);
        setFuncOnData(Msg_SSEL2_Auction, OnSubscribe_SSEL2_Auction);
        setFuncOnData(Msg_SSEL2_Index, OnSubscribe_SSEL2_Index);
        setFuncOnData(Msg_SSEL2_Overview, OnSubscribe_SSEL2_Overview);
        setFuncOnData(Msg_SSEL2_Static, OnSubscribe_SSEL2_Static);
    }

    QString TimeToStr(int time)
    {
        return QString::number(time);
//        QString timeStr = QString::number(time);
//        if (timeStr.length() > 6)
//            return timeStr.mid(0, 2) + ":" + timeStr.mid(2, 2) + ":" +timeStr.mid(4, 2) + "." + timeStr.mid(6);
//        else
//            return timeStr;
    }

    void DealSubscribe(MsgType msgType, const void* value)
    {
        QStringList dataList;

		if (msgType == Msg_SSEL2_Static)
		{
			SSEL2_Static * realValue = (SSEL2_Static *)value;
            dataList << QString(QDSTimeToStr(realValue->QDSTime)) << TimeToStr(realValue->Time) << QString(realValue->Symbol)
                     << QString::number(realValue->SampleAvgPrice);
		}
		else if (msgType == Msg_SSEL2_Quotation)
		{
			SSEL2_Quotation * realValue = (SSEL2_Quotation *)value;
            dataList << QString(QDSTimeToStr(realValue->QDSTime)) << TimeToStr(realValue->Time) << QString(realValue->Symbol)
                     << QString::number(realValue->OpenPrice) << QString::number(realValue->HighPrice) << QString::number(realValue->LowPrice) << QString::number(realValue->LastPrice);
		}
		else if (msgType == Msg_SSEL2_Transaction)
		{
			SSEL2_Transaction * realValue = (SSEL2_Transaction *)value;
            dataList << QString(QDSTimeToStr(realValue->QDSTime)) << QString(realValue->Symbol)
                     << QString::number(realValue->TradePrice);
		}
        else if (msgType == Msg_SSEL2_Index)
        {
            SSEL2_Index * realValue = (SSEL2_Index *)value;
            dataList << QString(QDSTimeToStr(realValue->QDSTime)) << TimeToStr(realValue->Time) << QString(realValue->Symbol)
                     << QString::number(realValue->OpenPrice) << QString::number(realValue->HighPrice) << QString::number(realValue->LowPrice) << QString::number(realValue->LastPrice);
        }
		else if (msgType == Msg_SSEL2_Auction)
		{
			SSEL2_Auction * realValue = (SSEL2_Auction *)value;
            dataList << QString(QDSTimeToStr(realValue->QDSTime)) << TimeToStr(realValue->Time) << QString(realValue->Symbol)
                     << QString::number(realValue->OpenPrice) << QString::number(realValue->AuctionVolume) << QString::number(realValue->LeaveVolume);
		}
		else if (msgType == Msg_SSEL2_Overview)
		{
			SSEL2_Overview * realValue = (SSEL2_Overview *)value;
            dataList << QString(QDSTimeToStr(realValue->QDSTime)) << TimeToStr(realValue->Time) << QString(realValue->Symbol)
                     << QString::number(realValue->TradeDate) << QString::number(realValue->MarketTime);
		}
		else if (msgType == Msg_SZSEL2_Static)
		{
			SZSEL2_Static * realValue = (SZSEL2_Static *)value;
            dataList << QString(QDSTimeToStr(realValue->QDSTime)) << QString(realValue->Symbol)
                     << QString::number(realValue->IssuedVolume) << QString::number(realValue->OutstandingShare) << QString::number(realValue->LimitUpAbsoluteT) << QString::number(realValue->LimitDownAbsoluteT);
		}
		else if (msgType == Msg_SZSEL2_Quotation)
		{
			SZSEL2_Quotation * realValue = (SZSEL2_Quotation *)value;
            dataList << QString(QDSTimeToStr(realValue->QDSTime)) << TimeToStr(realValue->Time) << QString(realValue->Symbol)
                     << QString::number(realValue->OpenPrice) << QString::number(realValue->HighPrice) << QString::number(realValue->LowPrice) << QString::number(realValue->LastPrice);
		}
		else if (msgType == Msg_SZSEL2_Transaction)
		{
			SZSEL2_Transaction * realValue = (SZSEL2_Transaction *)value;
            dataList << QString(QDSTimeToStr(realValue->QDSTime)) << QString(realValue->Symbol)
                     << QString::number(realValue->TradeTime) << QString::number(realValue->TradePrice) << QString::number(realValue->TradeVolume) << QString(realValue->TradeType);
		}
		else if (msgType == Msg_SZSEL2_Index)
		{
			SZSEL2_Index * realValue = (SZSEL2_Index *)value;
            dataList << QString(QDSTimeToStr(realValue->QDSTime)) << TimeToStr(realValue->Time) << QString(realValue->Symbol)
                     << QString::number(realValue->OpenPrice) << QString::number(realValue->HighPrice) << QString::number(realValue->LowPrice) << QString::number(realValue->LastPrice);
		}
		else if (msgType == Msg_SZSEL2_Order)
		{
			SZSEL2_Order * realValue = (SZSEL2_Order *)value;
            dataList << QString(QDSTimeToStr(realValue->QDSTime)) << QString::number(realValue->Time) << QString(realValue->Symbol)
                     << QString::number(realValue->OrderPrice) << QString::number(realValue->OrderVolume) << QString(realValue->OrderCode) << QString(realValue->OrderType);
		}
		else if (msgType == Msg_SZSEL2_Status)
		{
			SZSEL2_Status * realValue = (SZSEL2_Status *)value;
            dataList << QString(QDSTimeToStr(realValue->QDSTime)) << TimeToStr(realValue->Time) << QString(realValue->Symbol)
                     << QString(realValue->FinancialStatus);
		}

		g_pMainWindow->onTableAddingNews(dataList);
    }
}
