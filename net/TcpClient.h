#include"INet.h"
#include"PackDef.h"

class TcpClientNet :public INet
{
public:
	TcpClientNet(INetMediator* pMediator);
	~TcpClientNet();
public:
	bool InitNet();
	//long���Ϳ��Դ�ulong���͵�ip��ַ��Ҳ���Դ�SOCKET������udp��tcpЭ�顣
	bool SendData(char* buf, int nLen, long lSend);
	void unInitNet();
	static unsigned __stdcall RecvThread(void* lpVoid);
protected:
	void RecvData();
	SOCKET m_sock;
	HANDLE m_handle;
	bool m_bStop;

};