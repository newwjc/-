#include"TcpServerMediator.h"
#include"TcpServer.h"
#include"../CKernel.h"

TcpServerNetMediator::TcpServerNetMediator()
{
	m_Net = new TcpServerNet(this);
}
TcpServerNetMediator::~TcpServerNetMediator()
{
	delete m_Net;
	m_Net = nullptr;
}

bool TcpServerNetMediator::OpenNet()
{
	return m_Net->InitNet();
}
bool TcpServerNetMediator::SendData(char* buf, int nLen, long lSend)
{
	return m_Net->SendData(buf, nLen, lSend);
}
void TcpServerNetMediator::CloseNet()
{
	m_Net->unInitNet();
}

#include"PackDef.h"
void TcpServerNetMediator::DealData(char* buf, int nLen, long lSend)
{
	//接收到的数据传给kernel类
	CKernel::pKernel->dealReadyData(buf, nLen, lSend);
	
}
