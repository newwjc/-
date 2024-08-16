#pragma once
#include"CMySql.h"
#include"PackDef.h"
#include"INetMediator.h"
#include<map>

class CKernel;
typedef void(CKernel::* PFUN)(char* buf, int nLen, long lSend);

class CKernel
{
public:
	CKernel();
	~CKernel();
	//�򿪷�����
	bool startServer();

	//�رշ�����
	void closeServer();

	//��ѯ�û���Ϣ
	void getUserInfo(int id);
	//��ѯȺ����Ϣ
	void getGroupInfo(int id, _STRU_GROUP_INFO info);
	//����id��ѯ��Ϣ
	void getUserInfoById(int id,_STRU_FRIEND_INFO* info);
	//��ʼ������ָ������
	void setNetprotocol();
	//�ַ����н��յ������ݣ����ö�Ӧ�Ĵ�����
	void dealReadyData(char* buf, int nLen, long lSend);

	//����ע������
	void dealRegisterRq(char* buf, int nLen, long lSend);

	//�����¼����
	void dealLoginRq(char* buf, int nLen, long lSend);

	//������������
	void dealChatRq(char* buf, int nLen, long lSend);

	//������������
	void dealOfflineRq(char* buf, int nLen, long lSend);

	//������Ӻ�������
	void dealAddFriendRq(char* buf, int nLen, long lSend);

	//������Ӻ��ѻظ�
	void dealAddFriendRs(char* buf, int nLen, long lSend);

	//������Ⱥ������
	void dealCreateGroupRq(char* buf, int nLen, long lSend);

	//����Ⱥ������
	void dealGroupChatRq(char* buf, int nLen, long lSend);

	//�������ļ�����
	void dealFileSendRq(char* buf, int nLen, long lSend);

	//�������ļ��ظ�
	void dealFileSendRs(char* buf, int nLen, long lSend);

	//�����ļ�������ֱ��ת�������ѣ�
	void dealSendFileBlockRq(char* buf, int nLen, long lSend);
private:
	CMySql m_sql;
	INetMediator* m_pMediator;
	PFUN m_netProtocol[_DEF_PROTOCOL_COUNT];

	//�����¼�ɹ��û���socket
	map<int, SOCKET>m_mapIdToSocket;
public:
	static CKernel* pKernel;
};

