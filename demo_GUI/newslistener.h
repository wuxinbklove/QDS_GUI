#ifndef NEWSLISTENER_H
#define NEWSLISTENER_H

#include <QObject>
#include "QDSApi.h"
#include "QDSStruct.h"
#include "mainwindow.h"

class MainWindow;

namespace NewsListener {
    void InitSubscribe(MainWindow *);

    void OnSubscribe_SZSEL2_Static(const void* RealSZSEL2Static);
    void OnSubscribe_SZSEL2_Quotation(const void* RealSSEL2Quotation);
    void OnSubscribe_SZSEL2_Transaction(const void* RealSZSEL2Transaction);
    void OnSubscribe_SZSEL2_Index(const void* RealSZSEL2Index);
    void OnSubscribe_SZSEL2_Order(const void* RealSZSEL2Order);
    void OnSubscribe_SZSEL2_Status(const void* RealSZSEL2Status);

    void OnSubscribe_SSEL2_Quotation(const void* RealSSEL2Quotation);
    void OnSubscribe_SSEL2_Transaction(const void* RealSSEL2Transaction);
    void OnSubscribe_SSEL2_Auction(const void* RealSSEL2Auction);
    void OnSubscribe_SSEL2_Index(const void* RealSSEL2Index);
    void OnSubscribe_SSEL2_Overview(const void* RealSSEL2Overview);
    void OnSubscribe_SSEL2_Static(const void* RealSSEL2Static);

    void DealSubscribe(MsgType msgType, const void* RealValue);
};

#endif // NEWSLISTENER_H
