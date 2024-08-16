#include"TcpServer.h"
#include"TcpServerMediator.h"

TcpServerNet::TcpServerNet(INetMediator* pMediator):m_sock(INVALID_SOCKET),m_bStop(false)
{
	m_pMediator = pMediator;
}
TcpServerNet::~TcpServerNet()
{
	unInitNet();
}

bool TcpServerNet::InitNet()
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
	//绑定IP地址
	sockaddr_in addrServer;
	addrServer.sin_family = AF_INET;
	addrServer.sin_port = htons(_DEF_TCP_PORT);
	addrServer.sin_addr.S_un.S_addr = INADDR_ANY;

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
	//监听

	err = listen(m_sock, 10);
	if (SOCKET_ERROR == err)
	{
		cout << "listen error: " << WSAGetLastError() << endl;
		return false;
	}
	else
	{
		cout << "listen success" << endl;
	}


	//创建一个接收连接的线程
	HANDLE handle = (HANDLE)_beginthreadex(nullptr, 0, &AcceptThread, (void*)this, 0, nullptr);
	m_listHandle.push_back(handle);

	return true;





}
//接收连接的线程
unsigned __stdcall TcpServerNet::AcceptThread(void* lpVoid)
{
	TcpServerNet* pThis = (TcpServerNet*)lpVoid;
	//接受连接
	sockaddr_in addr;
	int addrSize = sizeof(addr);
	unsigned int threadId = 0;
	SOCKET sock = INVALID_SOCKET;
	while (!pThis->m_bStop)                          //有问题！
	{
		sock = accept(pThis->m_sock, (sockaddr*)&addr, &addrSize);
		//打印客户端IP地址
		cout << "Client ip: " << inet_ntoa(addr.sin_addr) << endl;
		//每次连接成功，都给这个客户端创建一个接收数据的线程
		HANDLE handle = (HANDLE)_beginthreadex(nullptr, 0, &RecvThread, (void*)pThis, 0, &threadId);
		//把线程id和socket对应关系保存起来
		pThis->m_mapThreadIdToSocket[threadId] = sock;
		//把线程句柄保存下来
	    pThis->m_listHandle.push_back(handle);

		return 1;
	}
	
}
//回复消息的线程
unsigned __stdcall TcpServerNet::RecvThread(void* lpVoid)
{
	TcpServerNet* pThis = (TcpServerNet*)lpVoid;
	pThis->RecvData();
	return 1;
}

bool TcpServerNet::SendData(char* buf, int nLen, long lSend)
{
	//1、校验参数
	if (!buf || nLen < 0)
	{
		cout << "TcpServerNet::SendData 参数错误" << endl;
		return false;
	}
	//2、先发包大小
	if (send(lSend, (char*)&nLen, sizeof(int), 0) <= 0)
	{
		cout << "TcpServerNet::SendData error" << WSAGetLastError()<< endl;
		return false;
	}
	//3、再发包内容
	if (send(lSend, buf, nLen, 0) <= 0)
	{
		cout << "TcpServerNet::SendData error" << WSAGetLastError() << endl;
		return false;

	}
	
	return true;
}



void TcpServerNet::RecvData()
{
	//需要先休眠几秒
	Sleep(100);

	//根据线程id，取出当前线程对应的socket
	SOCKET sock = m_mapThreadIdToSocket[GetCurrentThreadId()];
	if (!sock || sock == INVALID_SOCKET)
	{
		cout << "socket 错误：" << sock << "threadId:" << GetCurrentThreadId() << endl;
		return;
	}

	int nRecvNum = 0;
	int nPackSize = 0; //包大小
	int nOffset = 0;  //接收的大小
	while (!m_bStop)
	{
		nRecvNum = recv(sock, (char*)&nPackSize, sizeof(int), 0);
		//根据包大小new一个空间接收包内容
		char* packBuf = new char[nPackSize];
		if (nRecvNum > 0)
		{
			while (nPackSize > 0)
			{
				//如果接受包大小成功，再接收包内容
				nRecvNum = recv(sock, packBuf + nOffset, nPackSize, 0);
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
			m_pMediator->DealData(packBuf,nOffset,sock);
			nOffset = 0;
		}
		else
		{
			cout << "recv packSize error:" << WSAGetLastError() << endl;
			break;
		}
	}

}

//关闭网络：回收内核对象（结束线程工作，关闭句柄）、关闭套接字、卸载库
void TcpServerNet::unInitNet()
{
	//回收内核对象（结束线程工作，关闭句柄）
	HANDLE handle = nullptr;
	for (auto ite = m_listHandle.begin(); ite != m_listHandle.end();)
	{
		handle = *ite;
		if (handle)
		{
			//结束线程工作
			m_bStop = true;
			//等待一段时间,让线程自己退出循环结束工作
			if (WAIT_TIMEOUT == WaitForSingleObject(handle, 100))
			{
				//如果线程没有退出，再强制杀死线程
				TerminateThread(handle, -1);
			}
			//关闭句柄
			::CloseHandle(handle);
			handle = nullptr;
		}
		//把无效的节点移除链表
		ite = m_listHandle.erase(ite);
	}

	//关闭套接字
	if (m_sock && m_sock != INVALID_SOCKET)
	{
		closesocket(m_sock);
	}
	SOCKET sock = INVALID_SOCKET;
	for (auto ite = m_mapThreadIdToSocket.begin(); ite != m_mapThreadIdToSocket.end();)
	{
		sock = ite->second;
		if (sock || sock != INVALID_SOCKET)
		{
			closesocket(sock);
		}
		//把无效节点从map中删除
		ite = m_mapThreadIdToSocket.erase(ite);
	}
	//卸载库
	WSACleanup();
}