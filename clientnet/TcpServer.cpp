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
	//��IP��ַ
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
	//����

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


	//����һ���������ӵ��߳�
	HANDLE handle = (HANDLE)_beginthreadex(nullptr, 0, &AcceptThread, (void*)this, 0, nullptr);
	m_listHandle.push_back(handle);

	return true;





}
//�������ӵ��߳�
unsigned __stdcall TcpServerNet::AcceptThread(void* lpVoid)
{
	TcpServerNet* pThis = (TcpServerNet*)lpVoid;
	//��������
	sockaddr_in addr;
	int addrSize = sizeof(addr);
	unsigned int threadId = 0;
	SOCKET sock = INVALID_SOCKET;
	while (!pThis->m_bStop)                          //�����⣡
	{
		sock = accept(pThis->m_sock, (sockaddr*)&addr, &addrSize);
		//��ӡ�ͻ���IP��ַ
		cout << "Client ip: " << inet_ntoa(addr.sin_addr) << endl;
		//ÿ�����ӳɹ�����������ͻ��˴���һ���������ݵ��߳�
		HANDLE handle = (HANDLE)_beginthreadex(nullptr, 0, &RecvThread, (void*)pThis, 0, &threadId);
		//���߳�id��socket��Ӧ��ϵ��������
		pThis->m_mapThreadIdToSocket[threadId] = sock;
		//���߳̾����������
	    pThis->m_listHandle.push_back(handle);

		return 1;
	}
	
}
//�ظ���Ϣ���߳�
unsigned __stdcall TcpServerNet::RecvThread(void* lpVoid)
{
	TcpServerNet* pThis = (TcpServerNet*)lpVoid;
	pThis->RecvData();
	return 1;
}

bool TcpServerNet::SendData(char* buf, int nLen, long lSend)
{
	//1��У�����
	if (!buf || nLen < 0)
	{
		cout << "TcpServerNet::SendData ��������" << endl;
		return false;
	}
	//2���ȷ�����С
	if (send(lSend, (char*)&nLen, sizeof(int), 0) <= 0)
	{
		cout << "TcpServerNet::SendData error" << WSAGetLastError()<< endl;
		return false;
	}
	//3���ٷ�������
	if (send(lSend, buf, nLen, 0) <= 0)
	{
		cout << "TcpServerNet::SendData error" << WSAGetLastError() << endl;
		return false;

	}
	
	return true;
}



void TcpServerNet::RecvData()
{
	//��Ҫ�����߼���
	Sleep(100);

	//�����߳�id��ȡ����ǰ�̶߳�Ӧ��socket
	SOCKET sock = m_mapThreadIdToSocket[GetCurrentThreadId()];
	if (!sock || sock == INVALID_SOCKET)
	{
		cout << "socket ����" << sock << "threadId:" << GetCurrentThreadId() << endl;
		return;
	}

	int nRecvNum = 0;
	int nPackSize = 0; //����С
	int nOffset = 0;  //���յĴ�С
	while (!m_bStop)
	{
		nRecvNum = recv(sock, (char*)&nPackSize, sizeof(int), 0);
		//���ݰ���Сnewһ���ռ���հ�����
		char* packBuf = new char[nPackSize];
		if (nRecvNum > 0)
		{
			while (nPackSize > 0)
			{
				//������ܰ���С�ɹ����ٽ��հ�����
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

//�ر����磺�����ں˶��󣨽����̹߳������رվ�������ر��׽��֡�ж�ؿ�
void TcpServerNet::unInitNet()
{
	//�����ں˶��󣨽����̹߳������رվ����
	HANDLE handle = nullptr;
	for (auto ite = m_listHandle.begin(); ite != m_listHandle.end();)
	{
		handle = *ite;
		if (handle)
		{
			//�����̹߳���
			m_bStop = true;
			//�ȴ�һ��ʱ��,���߳��Լ��˳�ѭ����������
			if (WAIT_TIMEOUT == WaitForSingleObject(handle, 100))
			{
				//����߳�û���˳�����ǿ��ɱ���߳�
				TerminateThread(handle, -1);
			}
			//�رվ��
			::CloseHandle(handle);
			handle = nullptr;
		}
		//����Ч�Ľڵ��Ƴ�����
		ite = m_listHandle.erase(ite);
	}

	//�ر��׽���
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
		//����Ч�ڵ��map��ɾ��
		ite = m_mapThreadIdToSocket.erase(ite);
	}
	//ж�ؿ�
	WSACleanup();
}