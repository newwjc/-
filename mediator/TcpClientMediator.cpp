#include"TcpClientMediator.h"
#include"TcpClient.h"

TcpClientNetMediator::TcpClientNetMediator()
{
	m_Net = new TcpClientNet(this);
}
TcpClientNetMediator::~TcpClientNetMediator()
{
	delete m_Net;
	m_Net = nullptr;
}

bool TcpClientNetMediator::OpenNet()
{
	return m_Net->InitNet();
}
bool TcpClientNetMediator::SendData(char* buf, int nLen, long lSend)
{
	return m_Net->SendData(buf, nLen, lSend);
}
void TcpClientNetMediator::CloseNet()
{
	m_Net->unInitNet();
}
void TcpClientNetMediator::DealData(char* buf, int nLen, long lSend)
{
	//���յ������ݴ���kernel��
	//���Դ���
	cout << "TcpClientNetMediator::DealData" << buf << endl;
}
