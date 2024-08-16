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
	//打开服务器
	bool startServer();

	//关闭服务器
	void closeServer();

	//查询用户信息
	void getUserInfo(int id);
	//查询群组信息
	void getGroupInfo(int id, _STRU_GROUP_INFO info);
	//根据id查询信息
	void getUserInfoById(int id,_STRU_FRIEND_INFO* info);
	//初始化函数指针数组
	void setNetprotocol();
	//分发所有接收到的数据，调用对应的处理函数
	void dealReadyData(char* buf, int nLen, long lSend);

	//处理注册请求
	void dealRegisterRq(char* buf, int nLen, long lSend);

	//处理登录请求
	void dealLoginRq(char* buf, int nLen, long lSend);

	//处理聊天请求
	void dealChatRq(char* buf, int nLen, long lSend);

	//处理下线请求
	void dealOfflineRq(char* buf, int nLen, long lSend);

	//处理添加好友请求
	void dealAddFriendRq(char* buf, int nLen, long lSend);

	//处理添加好友回复
	void dealAddFriendRs(char* buf, int nLen, long lSend);

	//处理创建群聊请求
	void dealCreateGroupRq(char* buf, int nLen, long lSend);

	//处理群聊请求
	void dealGroupChatRq(char* buf, int nLen, long lSend);

	//处理传输文件请求
	void dealFileSendRq(char* buf, int nLen, long lSend);

	//处理传输文件回复
	void dealFileSendRs(char* buf, int nLen, long lSend);

	//处理文件块请求（直接转发给好友）
	void dealSendFileBlockRq(char* buf, int nLen, long lSend);
private:
	CMySql m_sql;
	INetMediator* m_pMediator;
	PFUN m_netProtocol[_DEF_PROTOCOL_COUNT];

	//管理登录成功用户的socket
	map<int, SOCKET>m_mapIdToSocket;
public:
	static CKernel* pKernel;
};

