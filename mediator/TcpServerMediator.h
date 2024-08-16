#pragma once 
#include"INetMediator.h"

class TcpServerNetMediator :public  INetMediator
{
public:
	TcpServerNetMediator();
	~TcpServerNetMediator();
public:
	bool OpenNet();
	bool SendData(char* buf, int nLen, long lSend);
	void CloseNet();
	void DealData(char* buf, int nLen, long lSend);

};