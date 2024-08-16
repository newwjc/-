#include"INet.h"
#include"PackDef.h"
#include<list>
#include<map>


class TcpServerNet :public INet
{
public:
	TcpServerNet(INetMediator* pMediator);
	~TcpServerNet();
public:
	bool InitNet();
	//long���Ϳ��Դ�ulong���͵�ip��ַ��Ҳ���Դ�SOCKET������udp��tcpЭ�顣
	bool SendData(char* buf, int nLen, long lSend);
	void unInitNet();
	static unsigned __stdcall RecvThread(void* lpVoid);
	static unsigned __stdcall AcceptThread(void* lpVoid);
protected:
	void RecvData();
	SOCKET m_sock;
	bool m_bStop;
	//�߳̾������
	list<HANDLE>m_listHandle;
	//�߳�id��ͻ�������socket��
	map<unsigned int, SOCKET>m_mapThreadIdToSocket;


};