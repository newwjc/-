#include"TcpClient.h"
#include"TcpClientMediator.h"


TcpClientNet::TcpClientNet(INetMediator* pMediator):m_sock(INVALID_SOCKET),m_handle(nullptr),m_bStop(false)
{
	m_pMediator = pMediator;
}
TcpClientNet::~TcpClientNet()
{

	unInitNet();
}

bool TcpClientNet::InitNet()
{
	//���ؿ�
	WSADATA wsaData;
	WORD word = MAKEWORD(2, 2);
	int err = WSAStartup(word, &wsaData);
	if (err != 0)
	{
		cout << "WSAStartup: error:" << WSAGetLastError() << endl;
		return false;
	}
	else
	{
		cout << "WSAStartup: success: " << endl;
	}

	//�жϼ��ص��Ƿ�Ϊ2.2�汾
	if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2)
	{
		cout << "version error" << endl;
		return false;
	}
	//�����׽���
	m_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == m_sock)
	{
		cout << "socket error: " << WSAGetLastError() << endl;
		return false;
	}
	else
	{
		cout << "socket success: " << endl;
	}
	//����
	sockaddr_in addrServer;
	addrServer.sin_family = AF_INET;
	addrServer.sin_port = htons(_DEF_TCP_PORT);
	addrServer.sin_addr.S_un.S_addr = inet_addr("192.168.3.130");

	err = connect(m_sock, (sockaddr*)&addrServer, sizeof(addrServer));
	if (err == SOCKET_ERROR)
	{
		cout << "connect error " << WSAGetLastError() << endl;
		return false;
	}
	else
	{
		cout << "connect success " << endl;
	}
	//����һ���������ݵ��߳�
	m_handle = (HANDLE)_beginthreadex(nullptr, 0, &RecvThread, (void*)this, 0, nullptr);

	return true;
}

unsigned __stdcall TcpClientNet::RecvThread(void* lpVoid)
{
	TcpClientNet* pThis = (TcpClientNet*)lpVoid;
	pThis->RecvData();
	return 1;
}

//long���Ϳ��Դ�ulong���͵�ip��ַ��Ҳ���Դ�SOCKET������udp��tcpЭ�顣
bool TcpClientNet::SendData(char* buf, int nLen, long lSend)
{
	//1��У�����
	if (!buf || nLen < 0)
	{
		cout << "TcpServerNet::SendData ��������" << endl;
		return false;
	}
	//2���ȷ�����С
	if (send(m_sock, (char*)&nLen, sizeof(int), 0) <= 0)
	{
		cout << "TcpClientNet::SendData error" << WSAGetLastError() << endl;
		return false;
	}
	//3���ٷ�������
	if (send(m_sock, buf, nLen, 0) <= 0)
	{
		cout << "TcpClientNet::SendData error" << WSAGetLastError() << endl;
		return false;

	}

	
	return true;
}
void TcpClientNet::unInitNet()
{
	//�����߳�,Ϊ�˻����ں˶��󣨽����̹߳������رվ����
	if (m_handle)
	{
		//�����̹߳���
		m_bStop = true;
		if (WAIT_TIMEOUT == WaitForSingleObject(m_handle, 100))
		{
			TerminateThread(m_handle, -1);
		}
		//�رվ��
		::CloseHandle(m_handle);
		m_handle = nullptr;
	}
	//�ر��׽��֣�ж�ؿ�
	if (m_sock && m_sock != INVALID_SOCKET)
	{
		closesocket(m_sock);
	}

	WSACleanup();
}

void TcpClientNet::RecvData()
{
	int nRecvNum = 0;
	int nPackSize = 0; //����С
	int nOffset = 0;  //���յĴ�С
	while (!m_bStop)
	{
		nRecvNum = recv(m_sock, (char*)&nPackSize, sizeof(int), 0);
		//���ݰ���Сnewһ���ռ���հ�����
		char* packBuf = new char[nPackSize];
		if (nRecvNum > 0)
		{
			while (nPackSize > 0)
			{
				//������ܰ���С�ɹ����ٽ��հ�����
				nRecvNum = recv(m_sock, packBuf + nOffset, nPackSize, 0);
				if (nRecvNum > 0)
				{
					nOffset += nRecvNum;
					nPackSize -= nRecvNum;
				}
				else
				{
					cout << "recv packBuf error:" << WSAGetLastError() << endl;
					break;
				}
			}
			m_pMediator->DealData(packBuf, nOffset, m_sock);
			nOffset = 0;
		}
		else
		{
			cout << "recv packSize error:" << WSAGetLastError() << endl;
			break;
		}
	}
	

}