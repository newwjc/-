#pragma once 
#include"INetMediator.h"

class TcpClientNetMediator :public  INetMediator
{
    Q_OBJECT

public:
	TcpClientNetMediator();
	~TcpClientNetMediator();
public:
	bool OpenNet();
	bool SendData(char* buf, int nLen, long lSend);
	void CloseNet();
	void DealData(char* buf, int nLen, long lSend);
signals:
    //把接收到的所有数据传给kernel
    void SIG_ReadyData(char* buf, int nLen, long lSend);
};
