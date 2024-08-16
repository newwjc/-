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
	//long类型可以存ulong类型的ip地址，也可以存SOCKET，兼容udp和tcp协议。
	bool SendData(char* buf, int nLen, long lSend);
	void unInitNet();
	static unsigned __stdcall RecvThread(void* lpVoid);
	static unsigned __stdcall AcceptThread(void* lpVoid);
protected:
	void RecvData();
	SOCKET m_sock;
	bool m_bStop;
	//线程句柄链表
	list<HANDLE>m_listHandle;
	//线程id与客户端连接socket绑定
	map<unsigned int, SOCKET>m_mapThreadIdToSocket;


};