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
	//加载库
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

	//判断加载的是否为2.2版本
	if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2)
	{
		cout << "version error" << endl;
		return false;
	}
	//创建套接字
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
	//连接
	sockaddr_in addrServer;
	addrServer.sin_family = AF_INET;
	addrServer.sin_port = htons(_DEF_TCP_PORT);
    addrServer.sin_addr.S_un.S_addr = inet_addr("192.168.98.1");


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
	//创建一个接收数据的线程
	m_handle = (HANDLE)_beginthreadex(nullptr, 0, &RecvThread, (void*)this, 0, nullptr);

	return true;
}

unsigned __stdcall TcpClientNet::RecvThread(void* lpVoid)
{
	TcpClientNet* pThis = (TcpClientNet*)lpVoid;
	pThis->RecvData();
	return 1;
}

//long类型可以存ulong类型的ip地址，也可以存SOCKET，兼容udp和tcp协议。
bool TcpClientNet::SendData(char* buf, int nLen, long lSend)
{
	//1、校验参数
	if (!buf || nLen < 0)
	{
		cout << "TcpServerNet::SendData 参数错误" << endl;
		return false;
	}
	//2、先发包大小
	if (send(m_sock, (char*)&nLen, sizeof(int), 0) <= 0)
	{
		cout << "TcpServerNet::SendData error" << WSAGetLastError() << endl;
		return false;
	}
	//3、再发包内容
	if (send(m_sock, buf, nLen, 0) <= 0)
	{
		cout << "TcpServerNet::SendData error" << WSAGetLastError() << endl;
		return false;

	}

	
	return true;
}
void TcpClientNet::unInitNet()
{
	//结束线程,为了回收内核对象（结束线程工作，关闭句柄）
	if (m_handle)
	{
		//结束线程工作
		m_bStop = true;
		if (WAIT_TIMEOUT == WaitForSingleObject(m_handle, 100))
		{
			TerminateThread(m_handle, -1);
		}
		//关闭句柄
		::CloseHandle(m_handle);
		m_handle = nullptr;
	}
	//关闭套接字，卸载库
	if (m_sock && m_sock != INVALID_SOCKET)
	{
		closesocket(m_sock);
	}

	WSACleanup();
}

void TcpClientNet::RecvData()
{
	int nRecvNum = 0;
	int nPackSize = 0; //包大小
	int nOffset = 0;  //接收的大小
	while (!m_bStop)
	{
		nRecvNum = recv(m_sock, (char*)&nPackSize, sizeof(int), 0);
		//根据包大小new一个空间接收包内容
		char* packBuf = new char[nPackSize];
		if (nRecvNum > 0)
		{
			while (nPackSize > 0)
			{
				//如果接受包大小成功，再接收包内容
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
