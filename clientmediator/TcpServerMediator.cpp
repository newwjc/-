#include"TcpServerMediator.h"
#include"TcpServer.h"

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
void TcpServerNetMediator::DealData(char* buf, int nLen, long lSend)
{
	//���յ������ݴ���kernel��
	//���Դ���
	cout << "TcpServerNetMediator::DealData " << buf << endl;

	//���ظ��ͻ���
	SendData("TcpServerMediator", sizeof("TcpServerMediator"), lSend);
}
