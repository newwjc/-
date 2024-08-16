#include "UdpNetMediator.h"
#include"../net/UdpNet.h"


UdpNetMediator::UdpNetMediator() 
{
	m_Net = new UdpNet(this);
}
UdpNetMediator::~UdpNetMediator()
{
	delete m_Net;
	m_Net = nullptr;
}


bool UdpNetMediator::OpenNet()
{
	return m_Net->InitNet();
}

bool UdpNetMediator::SendData(char* buf, int nLen, long lSend)
{
	return m_Net->SendData(buf, nLen,lSend);
}

void UdpNetMediator::CloseNet()
{
	m_Net->unInitNet();
}

void UdpNetMediator::DealData(char* buf, int nLen, long lSend)
{
	cout << "UDpNetMediator::DealData:" << buf << endl;
}
