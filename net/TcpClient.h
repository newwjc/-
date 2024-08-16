#include"INet.h"
#include"PackDef.h"

class TcpClientNet :public INet
{
public:
	TcpClientNet(INetMediator* pMediator);
	~TcpClientNet();
public:
	bool InitNet();
	//long类型可以存ulong类型的ip地址，也可以存SOCKET，兼容udp和tcp协议。
	bool SendData(char* buf, int nLen, long lSend);
	void unInitNet();
	static unsigned __stdcall RecvThread(void* lpVoid);
protected:
	void RecvData();
	SOCKET m_sock;
	HANDLE m_handle;
	bool m_bStop;

};