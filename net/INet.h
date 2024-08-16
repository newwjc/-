#pragma once 
//#include"../mediator/INetMediator.h"
#include<iostream>
#include<winsock2.h>
#include<process.h>
using namespace std;

#pragma comment(lib,"Ws2_32.lib")

class INetMediator;
class INet
{
public:
	INetMediator* m_pMediator;
public:
	INet() {}
	~INet() {};
public:
	virtual bool InitNet() = 0;
	virtual bool SendData(char* buf, int nLen, long lSend) = 0;
	virtual void RecvData() = 0;
	virtual void unInitNet() = 0;
};