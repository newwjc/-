#pragma once 
#include"INet.h"
//#include<Windows.h>
#include"PackDef.h"

class UdpNet :public INet
{


public:
	UdpNet(INetMediator* pMediator);
	~UdpNet();
public:
	 bool InitNet();
	 //long类型可以存ulong类型的IP地址，也可以存SOCKET，兼容UDP TCP协议
	 bool SendData(char* buf, int nLen, long lSend);
	 static unsigned __stdcall RecvThread(void* lpVoid);
	 void unInitNet();
protected:
	 void RecvData();
	 SOCKET m_sock;
	 HANDLE m_handle;
	 bool m_bStop;

	

};