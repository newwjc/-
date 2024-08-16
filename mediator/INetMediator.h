#pragma once
//#include"../net/INet.h"

class INet;
class INetMediator
{
public:
	INet* m_Net;
public:
	INetMediator()  {}
	~INetMediator() {}
public:
	virtual bool OpenNet() = 0;
	virtual bool SendData(char* buf, int nLen, long lSend) = 0;
	virtual void CloseNet() = 0;
	virtual void DealData(char* buf, int nLen, long lSend) = 0;
};
