#pragma once 
#include"INetMediator.h"

class UdpNetMediator :public  INetMediator
{
public:
	UdpNetMediator();
	~UdpNetMediator();
public:
	 bool OpenNet();
	 bool SendData(char* buf, int nLen, long lSend);
	 void CloseNet();
	 void DealData(char* buf, int nLen, long lSend);

};