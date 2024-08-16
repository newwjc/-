#pragma once 
#include"INetMediator.h"

class TcpClientNetMediator :public  INetMediator
{
public:
	TcpClientNetMediator();
	~TcpClientNetMediator();
public:
	bool OpenNet();
	bool SendData(char* buf, int nLen, long lSend);
	void CloseNet();
	void DealData(char* buf, int nLen, long lSend);

};