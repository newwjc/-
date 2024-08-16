#include "UdpNet.h"
#include"UdpNetMediator.h"





UdpNet::UdpNet(INetMediator* pMediator):m_sock(INVALID_SOCKET), m_handle(0),m_bStop(false)
{
	m_pMediator = pMediator;
}
UdpNet::~UdpNet()
{
	unInitNet();
}

//初始化网络：加载库，创建套接字，绑定ip地址，创建一个接收数据的线程。
bool UdpNet::InitNet()
{
	//1、加载库
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
	m_sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	if (INVALID_SOCKET == m_sock)
	{
		cout << "socket error: " << WSAGetLastError() << endl;
		return false;
	}
	else
	{
		cout << "socket success" << endl;
	}
	//绑定ip地址（告诉操作系统，发给某个网卡+某个端口的数据是当前进程要接收的数据）
	sockaddr_in addrServer;
	addrServer.sin_family = AF_INET;  //ipv4协议
	addrServer.sin_port = htons(_DEF_UDP_PORT); //小端变大端（转换成网络字节序）
	addrServer.sin_addr.S_un.S_addr = INADDR_ANY; //任意网卡

	err = bind(m_sock, (sockaddr*)&addrServer, sizeof(addrServer));

	if (SOCKET_ERROR == err)
	{
		cout << "bind error: " << WSAGetLastError() << endl;
		return false;
	}
	else
	{
		cout << "bind success" << endl;
	}

	//申请广播权限
	bool bVal = 1;
	setsockopt(m_sock, SOL_SOCKET, SO_BROADCAST, (char*)&bVal, sizeof(bVal));

	//创建一个接收数据的线程
	//DWORD id = 0;
	//char buf[1024] = "";
	//handle = ::CreateThread(nullptr, 0, &ThreadProc, &buf, 0, &id);
	m_handle = (HANDLE)_beginthreadex(nullptr, 0,&RecvThread, (void*)this, 0, nullptr);

	return true;
}
//线程函数（静态：接收数据)
unsigned __stdcall UdpNet::RecvThread(void* lpVoid)
{
	UdpNet* pThis = (UdpNet*)lpVoid;
	pThis->RecvData();
	return 1;
}

bool UdpNet::SendData(char* buf, int nLen, long lSend)
{
	sockaddr_in sockAddr;
	sockAddr.sin_family = AF_INET;
	sockAddr.sin_port = htons(_DEF_UDP_PORT);
	sockAddr.sin_addr.S_un.S_addr = lSend;
	
	if (sendto(m_sock, buf,nLen,0,(sockaddr*)&sockAddr,sizeof(sockAddr)) >= 0)
	{
		return true;
	}
	cout << "UdpNet::SendData error" << WSAGetLastError() << endl;
	return false;
}

void UdpNet::unInitNet()
{
	//结束线程,为了回收内核对象（结束线程工作，关闭句柄）
	if (m_handle)
	{
		//结束线程工作
		m_bStop = false;
		if (WAIT_TIMEOUT == WaitForSingleObject(m_handle,100))
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

void UdpNet::RecvData()
{
	char recvBuf[4096] = "";
	sockaddr_in sockAddr;
	int sockAddrLen = sizeof(sockAddr);
	int nRecvNum = 0;
	while (!m_bStop)
	{
		nRecvNum = recvfrom(m_sock, recvBuf, sizeof(recvBuf), 0, (sockaddr*)&sockAddr, &sockAddrLen);
		if (nRecvNum >0)
		{
			//new 一个新的空间
			char* packBuf = new char[nRecvNum];
			//把数据拷贝到新的空间
			memcpy(packBuf, recvBuf, nRecvNum);
			//把新的空间传给中介者
			m_pMediator->DealData(packBuf, nRecvNum, sockAddr.sin_addr.S_un.S_addr);
		}
		else
		{
			cout << "UdpNet::RecvData error:" << WSAGetLastError() << endl;
			break;
		}
	}
	
}


