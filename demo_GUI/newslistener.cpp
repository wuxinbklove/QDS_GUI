#include "newslistener.h"
#include <QDebug>
#include <QTime>

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

	void OnSubscribe_SSEIOL1_Static(const void* RealSSEIOL1Static)
	{
		DealSubscribe(Msg_SSEIOL1_Static, RealSSEIOL1Static);
	}

	void OnSubscribe_SSEIOL1_Quotation(const void* RealSSEIOL1Quotation)
	{
		DealSubscribe(Msg_SSEIOL1_Quotation, RealSSEIOL1Quotation);
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
		setFuncOnData(Msg_SSEIOL1_Static, OnSubscribe_SSEIOL1_Static);
		setFuncOnData(Msg_SSEIOL1_Quotation, OnSubscribe_SSEIOL1_Quotation);
    }

    QString TimeToStr(long long time)
    {
        return QString::number(time);
//        QString timeStr = QString::number(time);
//        if (timeStr.length() > 6)
//            return timeStr.mid(0, 2) + ":" + timeStr.mid(2, 2) + ":" +timeStr.mid(4, 2) + "." + timeStr.mid(6);
//        else
//            return timeStr;
    }

	QString UInt32ArrToString(unsigned int* pArr, int length)
	{
		QString str = "[";
		for (int i = 0; i < length; i++)
		{
			str += QString::number(*(pArr + i));
			if (i != length - 1)
				str += " , ";
		}
		str += "]";
		return str;
	}

	QString DoubleArrToString(double* pArr, int length)
	{
		QString str = "[";
		for (int i = 0; i < length; i++)
		{
			str += QString::number(*(pArr + i));
			if (i != length - 1)
				str += " , ";
		}
		str += "]";
		return str;
	}

	int CalcDelay(MsgType msgType, QString QDSTime, QString timeStr)
	{
		int delay = 0;
		if (msgType == Msg_SSEL2_Static || msgType == Msg_SSEL2_Quotation || msgType == Msg_SSEL2_Transaction || msgType == Msg_SSEL2_Index || msgType == Msg_SSEL2_Auction || msgType == Msg_SSEL2_Overview || msgType == Msg_SSEIOL1_Quotation)
		{
			QTime time1 = QTime::fromString(QDSTime.left(12), "hh:mm:ss.zzz");
			QTime time2;
			time2.setHMS(timeStr.mid(0, 2).toInt(), timeStr.mid(2, 2).toInt(), timeStr.mid(4, 2).toInt(), timeStr.mid(6, 3).toInt());
			delay = time1.msecsTo(time2);
		}
		else if (msgType == Msg_SZSEL2_Static || msgType == Msg_SZSEL2_Quotation || msgType == Msg_SZSEL2_Transaction || msgType == Msg_SZSEL2_Index || msgType == Msg_SZSEL2_Order)
		{
			QTime time1 = QTime::fromString(QDSTime.left(12), "hh:mm:ss.zzz");
			QTime time2;
			time2.setHMS(timeStr.mid(8, 2).toInt(), timeStr.mid(10, 2).toInt(), timeStr.mid(12, 2).toInt(), timeStr.mid(14, 3).toInt());
			delay = time1.msecsTo(time2);
		}
		return delay;
	}

    void DealSubscribe(MsgType msgType, const void* value)
    {
        QStringList dataList;

        int delay = INT_MAX;


		if (msgType == Msg_SSEL2_Static)
		{
			SSEL2_Static * realValue = (SSEL2_Static *)value;
            dataList << QString(QDSTimeToStr(realValue->QDSTime)) << QString(realValue->Symbol) << QString::number(realValue->Reserved) << QString::number(realValue->Time) 
				<< QString(realValue->ISINCode) << QString(realValue->SecurityName) << QString(realValue->SecurityEN) << QString(realValue->SymbolUnderlying) << QString(realValue->MarketType)
				<< QString(realValue->CFICode) << QString(realValue->SecuritySubType) << QString(realValue->Currency) << QString::number(realValue->ParValue) << QString::number(realValue->TradableNo)
				<< QString::number(realValue->EndDate) << QString::number(realValue->ListingDate) << QString::number(realValue->SetNo)<< QString::number(realValue->BuyVolumeUnit) << QString::number(realValue->SellVolumeUnit) 
				<< QString::number(realValue->DeclareVolumeFloor) << QString::number(realValue->DeclareVolumeCeiling) << QString::number(realValue->PreClosePrice) << QString::number(realValue->TickSize) << QString(realValue->UpDownLimitType) 
				<< QString::number(realValue->PriceUpLimit) << QString::number(realValue->PriceDownLimit) << QString::number(realValue->XRRatio) << QString::number(realValue->XDAmount) << QString(realValue->CrdBuyUnderlying) 
				<< QString(realValue->CrdSellUnderlying) << QString(realValue->SecurityStatus) << QString::number(realValue->SampleNo) << QString::number(realValue->SampleAvgPrice)  << QString::number(realValue->TradeAmount) 
				<< QString::number(realValue->AvgCapital) << QString::number(realValue->TotalMarketValue) << QString::number(realValue->MarketValueRatio) << QString::number(realValue->StaticPERatio)  << QString(realValue->IndexLevelStatus);
			delay = CalcDelay(msgType, QString(QDSTimeToStr(realValue->QDSTime)), QString::number(realValue->Time));
		}
		else if (msgType == Msg_SSEL2_Quotation)
		{
			SSEL2_Quotation * realValue = (SSEL2_Quotation *)value;
            dataList << QString(QDSTimeToStr(realValue->QDSTime)) << QString(realValue->Symbol) << QString::number(realValue->Reserved) << QString::number(realValue->Time)
				<< QString::number(realValue->PreClosePrice) << QString::number(realValue->OpenPrice) << QString::number(realValue->HighPrice) << QString::number(realValue->LowPrice) << QString::number(realValue->LastPrice)
				<< QString::number(realValue->ClosePrice) << QString(realValue->TradeStatus) << QByteArray(realValue->SecurityPhaseTag, 8) << QString::number(realValue->TotalNo) << QString::number(realValue->TotalVolume)
				<< QString::number(realValue->TotalAmount) << QString::number(realValue->TotalBuyOrderVolume) << QString::number(realValue->WtAvgBuyPrice) << QString::number(realValue->BondWtAvgBuyPrice) << QString::number(realValue->TotalSellOrderVolume)
				<< QString::number(realValue->WtAvgSellPrice) << QString::number(realValue->BondWtAvgSellPrice) << QString::number(realValue->IOPV) << QString::number(realValue->ETFBuyNo) << QString::number(realValue->ETFBuyVolume)
				<< QString::number(realValue->ETFBuyAmount) << QString::number(realValue->ETFSellNo) << QString::number(realValue->ETFSellVolume) << QString::number(realValue->ETFSellAmount) << QString::number(realValue->YTM)
				<< QString::number(realValue->TotalWarrantExecVol) << QString::number(realValue->WarrantDownLimit) << QString::number(realValue->WarrantUpLimit) << QString::number(realValue->WithdrawBuyNo) << QString::number(realValue->WithdrawBuyVolume)
				<< QString::number(realValue->WithdrawBuyAmount) << QString::number(realValue->WithdrawSellNo) << QString::number(realValue->WithdrawSellVolume) << QString::number(realValue->WithdrawSellAmount) << QString::number(realValue->TotalBuyNo)
				<< QString::number(realValue->TotalSellNo) << QString::number(realValue->MaxBuyDuration) << QString::number(realValue->MaxSellDuration) << QString::number(realValue->BuyOrderNo) << QString::number(realValue->SellOrderNo)
				<< QString::number(realValue->SellLevelNo) << QString::number(realValue->SellPrice01) << QString::number(realValue->SellVolume01) << QString::number(realValue->TotalSellOrderNo01) << QString::number(realValue->SellPrice02)
				<< QString::number(realValue->SellVolume02) << QString::number(realValue->TotalSellOrderNo02) << QString::number(realValue->SellPrice03) << QString::number(realValue->SellVolume03) << QString(realValue->TotalSellOrderNo03)
				<< QString::number(realValue->SellPrice04) << QString::number(realValue->SellVolume04) << QString::number(realValue->TotalSellOrderNo04) << QString::number(realValue->SellPrice05) << QString::number(realValue->SellVolume05)
				<< QString::number(realValue->TotalSellOrderNo05) << QString::number(realValue->SellPrice06) << QString::number(realValue->SellVolume06) << QString::number(realValue->TotalSellOrderNo06) << QString::number(realValue->SellPrice07)
				<< QString::number(realValue->SellVolume07) << QString::number(realValue->TotalSellOrderNo07) << QString::number(realValue->SellPrice08) << QString::number(realValue->SellVolume08) << QString(realValue->TotalSellOrderNo08)
				<< QString::number(realValue->SellPrice09) << QString::number(realValue->SellVolume09) << QString::number(realValue->TotalSellOrderNo09) << QString::number(realValue->SellPrice10) << QString::number(realValue->SellVolume10)
				<< QString::number(realValue->TotalSellOrderNo10) << QString::number(realValue->SellLevelQueueNo01) << UInt32ArrToString(realValue->SellLevelQueue, 50) << QString::number(realValue->BuyLevelNo) << QString::number(realValue->BuyPrice01)
				<< QString::number(realValue->BuyVolume01) << QString::number(realValue->TotalBuyOrderNo01) << QString::number(realValue->BuyPrice02) << QString::number(realValue->BuyVolume02) << QString::number(realValue->TotalBuyOrderNo02)
				<< QString::number(realValue->BuyPrice03) << QString::number(realValue->BuyVolume03) << QString::number(realValue->TotalBuyOrderNo03) << QString::number(realValue->BuyPrice04) << QString::number(realValue->BuyVolume04)
				<< QString::number(realValue->TotalBuyOrderNo04) << QString::number(realValue->BuyPrice05) << QString::number(realValue->BuyVolume05) << QString::number(realValue->TotalBuyOrderNo05) << QString::number(realValue->BuyPrice06)
				<< QString::number(realValue->BuyVolume06) << QString::number(realValue->TotalBuyOrderNo06) << QString::number(realValue->BuyPrice07) << QString::number(realValue->BuyVolume07) << QString::number(realValue->TotalBuyOrderNo07)
				<< QString::number(realValue->BuyPrice08) << QString::number(realValue->BuyVolume08) << QString::number(realValue->TotalBuyOrderNo08) << QString::number(realValue->BuyPrice09) << QString::number(realValue->BuyVolume09)
				<< QString::number(realValue->TotalBuyOrderNo09) << QString::number(realValue->BuyPrice10) << QString::number(realValue->BuyVolume10) << QString::number(realValue->TotalBuyOrderNo10) << QString::number(realValue->BuyLevelQueueNo01)
				<< UInt32ArrToString(realValue->BuyLevelQueue, 50);
			delay = CalcDelay(msgType, QString(QDSTimeToStr(realValue->QDSTime)), QString::number(realValue->Time));
		}
		else if (msgType == Msg_SSEL2_Transaction)
		{
			SSEL2_Transaction * realValue = (SSEL2_Transaction *)value;
            dataList << QString(QDSTimeToStr(realValue->QDSTime)) << QString(realValue->Symbol) << QString::number(realValue->Reserved) << QString::number(realValue->TradeTime)
				<< QString::number(realValue->RecID) << QString::number(realValue->TradeChannel) << QString::number(realValue->TradePrice) << QString::number(realValue->TradeVolume) << QString::number(realValue->TradeAmount)
				<< QString::number(realValue->BuyRecID) << QString::number(realValue->SellRecID) << QString(realValue->BuySellFlag);
			delay = CalcDelay(msgType, QString(QDSTimeToStr(realValue->QDSTime)), QString::number(realValue->TradeTime));
		}
        else if (msgType == Msg_SSEL2_Index)
        {
            SSEL2_Index * realValue = (SSEL2_Index *)value;
			dataList << QString(QDSTimeToStr(realValue->QDSTime)) << QString(realValue->Symbol) << QString::number(realValue->Reserved) << QString::number(realValue->Time)
			<< QString::number(realValue->TradeTime) << QString::number(realValue->PreClosePrice) << QString::number(realValue->OpenPrice) << QString::number(realValue->TotalAmount) << QString::number(realValue->HighPrice) 
			<< QString::number(realValue->LowPrice) << QString::number(realValue->LastPrice) << QString::number(realValue->TotalVolume) << QString::number(realValue->ClosePrice);
			delay = CalcDelay(msgType, QString(QDSTimeToStr(realValue->QDSTime)), QString::number(realValue->Time));
        }
		else if (msgType == Msg_SSEL2_Auction)
		{
			SSEL2_Auction * realValue = (SSEL2_Auction *)value;
            dataList << QString(QDSTimeToStr(realValue->QDSTime)) << QString(realValue->Symbol) << QString::number(realValue->Reserved) << QString::number(realValue->Time)
                     << QString::number(realValue->OpenPrice) << QString::number(realValue->AuctionVolume) << QString::number(realValue->LeaveVolume) << QString(realValue->Side);
			delay = CalcDelay(msgType, QString(QDSTimeToStr(realValue->QDSTime)), QString::number(realValue->Time));
		}
		else if (msgType == Msg_SSEL2_Overview)
		{
			SSEL2_Overview * realValue = (SSEL2_Overview *)value;
			dataList << QString(QDSTimeToStr(realValue->QDSTime)) << QString(realValue->Symbol) << QString::number(realValue->Reserved) << QString::number(realValue->Time)
				<< QString::number(realValue->MarketTime) << QString::number(realValue->TradeDate);
			delay = CalcDelay(msgType, QString(QDSTimeToStr(realValue->QDSTime)), QString::number(realValue->Time));
		}
		else if (msgType == Msg_SSEIOL1_Static)
		{
			SSEIOL1_Static * realValue = (SSEIOL1_Static *)value;
			dataList << QString(QDSTimeToStr(realValue->QDSTime)) << QString(realValue->Symbol) << QString::number(realValue->Reserved) << QString(realValue->ContractID)
				<< QString(realValue->SecurityName) << QString(realValue->SymbolUnderlying) << QString(realValue->NameUnderlying) << QString(realValue->UnderlyingType) << QString(realValue->OptionType)
				<< QString(realValue->CallOrPut) << QString::number(realValue->ContractMultiplierUnit) << QString::number(realValue->ExercisePrice) << QString::number(realValue->StartDate) << QString::number(realValue->EndDate)
				<< QString::number(realValue->ExerciseDate) << QString::number(realValue->DeliveryDate) << QString::number(realValue->ExpireDate) << QString(realValue->Version) << QString::number(realValue->TotalLongPosition)
				<< QString::number(realValue->PreClosePrice) << QString::number(realValue->PreSettlePrice) << QString::number(realValue->PreClosePriceUnderlying) << QString(realValue->UpDownLimitType) << QString::number(realValue->PriceUpLimit)
				<< QString::number(realValue->PriceDownLimit) << QString::number(realValue->MarginUnit) << QString::number(realValue->MarginRatioParam1) << QString::number(realValue->MarginRatioParam2) << QString::number(realValue->RoundLot)
				<< QString::number(realValue->LmtOrdFloor) << QString::number(realValue->LmtOrdCeiling) << QString::number(realValue->MktOrdFloor) << QString::number(realValue->MktOrdCeiling) << QString::number(realValue->TickSize)
				<< QString(realValue->SecurityStatusFlag) << QString::number(realValue->AutoSplitDate);
		
		}
		else if (msgType == Msg_SSEIOL1_Quotation)
		{
			SSEIOL1_Quotation * realValue = (SSEIOL1_Quotation *)value;
			dataList << QString(QDSTimeToStr(realValue->QDSTime)) << QString(realValue->Symbol) << QString::number(realValue->Reserved) << QString::number(realValue->Time)
				<< QString::number(realValue->PreSettlePrice) << QString::number(realValue->SettlePrice) << QString::number(realValue->OpenPrice) << QString::number(realValue->HighPrice) << QString::number(realValue->LowPrice)
				<< QString::number(realValue->LastPrice) << QString::number(realValue->ClosePrice) << QString::number(realValue->AuctionPrice) << QString::number(realValue->AuctionVolume) << QString::number(realValue->TotalPosition)
				<< QString::number(realValue->SellLevelNo) << QString::number(realValue->SellPrice01) << QString::number(realValue->SellVolume01) << QString::number(realValue->SellPrice02) << QString::number(realValue->SellVolume02)
				<< QString::number(realValue->SellPrice03) << QString::number(realValue->SellVolume03) << QString::number(realValue->SellPrice04) << QString::number(realValue->SellVolume04) << QString::number(realValue->SellPrice05)
				<< QString::number(realValue->SellVolume05) << QString::number(realValue->BuyLevelNo) << QString::number(realValue->BuyPrice01) << QString::number(realValue->BuyVolume01) << QString::number(realValue->BuyPrice02)
				<< QString::number(realValue->BuyVolume02) << QString::number(realValue->BuyPrice03) << QString::number(realValue->BuyVolume03) << QString::number(realValue->BuyPrice04) << QString::number(realValue->BuyVolume04)
				<< QString::number(realValue->BuyPrice05) << QString::number(realValue->BuyVolume05) << QString::number(realValue->TotalVolume) << QString::number(realValue->TotalAmount) << QByteArray(realValue->SecurityPhaseTag, 4);
		}
		else if (msgType == Msg_SZSEL2_Static)
		{
			SZSEL2_Static * realValue = (SZSEL2_Static *)value;
            dataList << QString(QDSTimeToStr(realValue->QDSTime)) << QString(realValue->Symbol) << QString(realValue->SecurityName) << QString(realValue->SymbolSource)
				<< QString(realValue->SecurityEN) << QString(realValue->ISINCode) << QString(realValue->SymbolUnderlying) << QString(realValue->UnderlyingSecurityIDSource) << QString::number(realValue->SecurityType)
				<< QString(realValue->SecurityStatusTag) << QString::number(realValue->PreClosePrice) << QString::number(realValue->ListingDate) << QString(realValue->Currency) << QString::number(realValue->ParValue)
				<< QString::number(realValue->IssuedVolume) << QString::number(realValue->OutstandingShare) << QString(realValue->IndustryType) << QString::number(realValue->PreYearEPS) << QString::number(realValue->YearEPS)
				<< QString(realValue->OfferingFlag) << QString::number(realValue->NAV) << QString::number(realValue->CouponRate) << QString::number(realValue->IssuePrice) << QString::number(realValue->Interest)
				<< QString::number(realValue->InterestAccrualDate) << QString::number(realValue->MaturityDate) << QString::number(realValue->ConversionPrice) << QString::number(realValue->ConversionRatio) << QString::number(realValue->ConversionBeginDate)
				<< QString::number(realValue->ConversionEndDate) << QString(realValue->CallOrPut) << QString(realValue->WarrantClearingType) << QString::number(realValue->ClearingPrice) << QString(realValue->OptionType)
				<< QString::number(realValue->EndDate) << QString::number(realValue->ExpirationDays) << QString(realValue->DayTrading) << QString(realValue->GageFlag) << QString::number(realValue->GageRate)
				<< QString(realValue->CrdBuyUnderlying) << QString(realValue->CrdSellUnderlying) << QString(realValue->CrdPriceCheckType) << QString(realValue->PledgeFlag) << QString::number(realValue->ContractMultiplier)
				<< QString(realValue->RegularShare) << QString(realValue->QualificationFlag) << QString(realValue->MarketMakerFlag) << QString::number(realValue->RoundLot) << QString::number(realValue->TickSize)
				<< QString::number(realValue->BuyQtyUpperLimit) << QString::number(realValue->SellQtyUpperLimit) << QString::number(realValue->BuyVolumeUnit) << QString::number(realValue->SellVolumeUnit) << QString::number(realValue->LimitUpRateO)
				<< QString::number(realValue->LimitDownRateO) << QString::number(realValue->LimitUpAbsoluteO) << QString::number(realValue->LimitDownAbsoluteO) << QString::number(realValue->AuctionUpDownRateO) << QString::number(realValue->AuctionUpDownAbsoluteO)
				<< QString::number(realValue->LimitUpRateT) << QString::number(realValue->LimitDownRateT) << QString::number(realValue->LimitUpAbsoluteT) << QString::number(realValue->LimitDownAbsoluteT) << QString::number(realValue->AuctionUpDownRateT)
				<< QString::number(realValue->AuctionUpDownAbsoluteT) << QString::number(realValue->LimitUpRateC) << QString::number(realValue->LimitDownRateC) << QString::number(realValue->LimitUpAbsoluteC) << QString::number(realValue->LimitDownAbsoluteC)
				<< QString::number(realValue->AuctionUpDownRateC) << QString::number(realValue->AuctionUpDownAbsoluteC);
		}
		else if (msgType == Msg_SZSEL2_Quotation)
		{
			SZSEL2_Quotation * realValue = (SZSEL2_Quotation *)value;
            dataList << QString(QDSTimeToStr(realValue->QDSTime)) << QString(realValue->Symbol) << TimeToStr(realValue->Time) << QString(realValue->SymbolSource)
				<< QString::number(realValue->PreClosePrice) << QString::number(realValue->OpenPrice) << QString::number(realValue->LastPrice) << QString::number(realValue->HighPrice) << QString::number(realValue->LowPrice)
				<< QString::number(realValue->PriceUpLimit) << QString::number(realValue->PriceDownLimit) << QString::number(realValue->PriceUpdown1) << QString::number(realValue->PriceUpdown2) << QString::number(realValue->TotalNo)
				<< QString::number(realValue->TotalVolume) << QString::number(realValue->TotalAmount) << QString::number(realValue->ClosePrice) << QByteArray(realValue->SecurityPhaseTag, 8) << QString::number(realValue->PERatio1)
				<< QString::number(realValue->NAV) << QString::number(realValue->PERatio2) << QString::number(realValue->IOPV) << QString::number(realValue->PremiumRate) << QString::number(realValue->TotalSellOrderVolume)
				<< QString::number(realValue->WtAvgSellPrice) << QString::number(realValue->SellLevelNo) << QString::number(realValue->SellPrice01) << QString::number(realValue->SellVolume01) << QString::number(realValue->TotalSellOrderNo01)
				<< QString::number(realValue->SellPrice02) << QString::number(realValue->SellVolume02) << QString::number(realValue->TotalSellOrderNo02) << QString::number(realValue->SellPrice03) << QString::number(realValue->SellVolume03)
				<< QString::number(realValue->TotalSellOrderNo03) << QString::number(realValue->SellPrice04) << QString::number(realValue->SellVolume04) << QString::number(realValue->TotalSellOrderNo04) << QString::number(realValue->SellPrice05)
				<< QString::number(realValue->SellVolume05) << QString::number(realValue->TotalSellOrderNo05) << QString::number(realValue->SellPrice06) << QString::number(realValue->SellVolume06) << QString::number(realValue->TotalSellOrderNo06)
				<< QString::number(realValue->SellPrice07) << QString::number(realValue->SellVolume07) << QString::number(realValue->TotalSellOrderNo07) << QString::number(realValue->SellPrice08) << QString::number(realValue->SellVolume08)
				<< QString::number(realValue->TotalSellOrderNo08) << QString::number(realValue->SellPrice09) << QString::number(realValue->SellVolume09) << QString::number(realValue->TotalSellOrderNo09) << QString::number(realValue->SellPrice10)
				<< QString::number(realValue->SellVolume10) << QString::number(realValue->TotalSellOrderNo10) << QString::number(realValue->SellLevelQueueNo01) << DoubleArrToString(realValue->SellLevelQueue, 50) << QString::number(realValue->TotalBuyOrderVolume)
				<< QString::number(realValue->WtAvgBuyPrice) << QString::number(realValue->BuyLevelNo) << QString::number(realValue->BuyPrice01) << QString::number(realValue->BuyVolume01) << QString::number(realValue->TotalBuyOrderNo01)
				<< QString::number(realValue->BuyPrice02) << QString::number(realValue->BuyVolume02) << QString::number(realValue->TotalBuyOrderNo02) << QString::number(realValue->BuyPrice03) << QString::number(realValue->BuyVolume03)
				<< QString::number(realValue->TotalBuyOrderNo03) << QString::number(realValue->BuyPrice04) << QString::number(realValue->BuyVolume04) << QString::number(realValue->TotalBuyOrderNo04) << QString::number(realValue->BuyPrice05)
				<< QString::number(realValue->BuyVolume05) << QString::number(realValue->TotalBuyOrderNo05) << QString::number(realValue->BuyPrice06) << QString::number(realValue->BuyVolume06) << QString::number(realValue->TotalBuyOrderNo06)
				<< QString::number(realValue->BuyPrice07) << QString::number(realValue->BuyVolume07) << QString::number(realValue->TotalBuyOrderNo07) << QString::number(realValue->BuyPrice08) << QString::number(realValue->BuyVolume08)
				<< QString::number(realValue->TotalBuyOrderNo08) << QString::number(realValue->BuyPrice09) << QString::number(realValue->BuyVolume09) << QString::number(realValue->TotalBuyOrderNo09) << QString::number(realValue->BuyPrice10)
				<< QString::number(realValue->BuyVolume10) << QString::number(realValue->TotalBuyOrderNo10) << QString::number(realValue->BuyLevelQueueNo01) << DoubleArrToString(realValue->BuyLevelQueue, 50);
			delay = CalcDelay(msgType, QString(QDSTimeToStr(realValue->QDSTime)), QString::number(realValue->Time));
		}
		else if (msgType == Msg_SZSEL2_Transaction)
		{
			SZSEL2_Transaction * realValue = (SZSEL2_Transaction *)value;
            dataList << QString(QDSTimeToStr(realValue->QDSTime)) << QString(realValue->Symbol) << QString::number(realValue->SetID) << QString::number(realValue->RecID)
				<< QString::number(realValue->BuyOrderID) << QString::number(realValue->SellOrderID) << QString(realValue->SymbolSource) << QString::number(realValue->TradeTime) << QString::number(realValue->TradePrice)
				<< QString::number(realValue->TradeVolume) << QString(realValue->TradeType);
			delay = CalcDelay(msgType, QString(QDSTimeToStr(realValue->QDSTime)), QString::number(realValue->TradeTime));
		}
		else if (msgType == Msg_SZSEL2_Index)
		{
			SZSEL2_Index * realValue = (SZSEL2_Index *)value;
			dataList << QString(QDSTimeToStr(realValue->QDSTime)) << QString(realValue->Symbol) << QString::number(realValue->Time) << QString(realValue->SymbolSource)
				<< QString::number(realValue->PreClosePrice) << QString::number(realValue->OpenPrice) << QString::number(realValue->HighPrice) << QString::number(realValue->LowPrice) << QString::number(realValue->LastPrice)
				<< QString::number(realValue->TotalAmount) << QString::number(realValue->TotalNo) << QString::number(realValue->TotalVolume) << QByteArray(realValue->SecurityPhaseTag, 8) << QString::number(realValue->SampleNo);
			delay = CalcDelay(msgType, QString(QDSTimeToStr(realValue->QDSTime)), QString::number(realValue->Time));
		}
		else if (msgType == Msg_SZSEL2_Order)
		{
			SZSEL2_Order * realValue = (SZSEL2_Order *)value;
            dataList << QString(QDSTimeToStr(realValue->QDSTime)) << QString(realValue->Symbol) << QString::number(realValue->SetID) << QString::number(realValue->RecID)
				<< QString(realValue->SymbolSource) << QString::number(realValue->Time) << QString::number(realValue->OrderPrice) << QString::number(realValue->OrderVolume) << QString(realValue->OrderCode)
				<< QString(realValue->OrderType);
			delay = CalcDelay(msgType, QString(QDSTimeToStr(realValue->QDSTime)), QString::number(realValue->Time));
		}
		else if (msgType == Msg_SZSEL2_Status)
		{
			SZSEL2_Status * realValue = (SZSEL2_Status *)value;
            dataList << QString(QDSTimeToStr(realValue->QDSTime)) << QString(realValue->Symbol) << QString(realValue->SymbolSource) << QString::number(realValue->Time)
				<< QString(realValue->FinancialStatus) << QString(realValue->CrdBuyStatus) << QString(realValue->CrdSellStatus) << QString(realValue->SubscribeStatus) << QString(realValue->RedemptionStatus)
				<< QString(realValue->PurchasingStatus) << QString(realValue->StockDiviStatus) << QString(realValue->PutableStatus) << QString(realValue->ExerciseStatus) << QString(realValue->GoldPurchase)
				<< QString(realValue->GoldRedemption) << QString(realValue->AcceptedStatus) << QString(realValue->ReleaseStatus) << QString(realValue->CancStockDiviStatus) << QString(realValue->CancPutableStatus)
				<< QString(realValue->PledgeStatus) << QString(realValue->RemovePledge) << QString(realValue->VoteStatus) << QString(realValue->StockPledgeRepo) << QString(realValue->DivideStatus)
				<< QString(realValue->MergerStatus);
			//delay = CalcDelay(msgType, QString(QDSTimeToStr(realValue->QDSTime)), QString::number(realValue->Time));
		}

        g_pMainWindow->onTableAddingNews(msgType, dataList, delay);
    }
}
