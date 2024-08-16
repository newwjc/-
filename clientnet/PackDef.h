#pragma once


#define  _DEF_UDP_PORT (34523)
#define  _DEF_TCP_PORT (78965)
#define  _DEF_MAX_SIZE (100)
#define  _DEF_CONTENT_SIZE (4096)
#define  _DEF_PROTOCOL_COUNT (10)

//协议头
#define _DEF_TCP_PROTOCOL_BASE (1000)
//注册
#define _DEF_TCP_REGISTER_RQ   (_DEF_TCP_PROTOCOL_BASE +1)
#define _DEF_TCP_REGISTER_RS   (_DEF_TCP_PROTOCOL_BASE +2)

//登录
#define _DEF_TCP_LOGIN_RQ   (_DEF_TCP_PROTOCOL_BASE +3)
#define _DEF_TCP_LOGIN_RS   (_DEF_TCP_PROTOCOL_BASE +4)

//获取好友信息
#define _DEF_TCP_FRIEND_INFO   (_DEF_TCP_PROTOCOL_BASE +5)

//聊天
#define _DEF_TCP_CHAT_RQ   (_DEF_TCP_PROTOCOL_BASE +6)
#define _DEF_TCP_CHAT_RS   (_DEF_TCP_PROTOCOL_BASE +7)
//添加好友
#define _DEF_TCP_ADD_FRIEND_RQ   (_DEF_TCP_PROTOCOL_BASE +8)
#define _DEF_TCP_ADD_FRIEND_RS   (_DEF_TCP_PROTOCOL_BASE +9)
//下线
#define _DEF_TCP_OFFLINE_RQ   (_DEF_TCP_PROTOCOL_BASE +10)

//结果定义
//注册结果
#define register_success    (0)
#define tel_is_used         (1)
#define name_is_exists      (2)

//登陆结果
#define login_success       (0)
#define password_error		(1)
#define user_not_exists		(2)

//聊天结果
#define send_success        (0)
#define send_fail           (1)

//添加好友结果
#define add_success         (0)
#define friend_not_exists   (1)
#define friend_refuse       (2)
#define friend_offline      (3)

//用户状态
#define status_online       (0)
#define status_offline      (1)

//重新定义一个协议头类型
typedef int PackType;

//请求结构体
//注册请求：协议头、昵称、手机号、密码
typedef struct _STRU_REGISTER_RQ {
	_STRU_REGISTER_RQ():type(_DEF_TCP_REGISTER_RQ)
	{
		memset(name,0, _DEF_MAX_SIZE);
		memset(tel,0, _DEF_MAX_SIZE);
		memset(password,0, _DEF_MAX_SIZE);
	}
	PackType type;
	char name[_DEF_MAX_SIZE];
	char tel[_DEF_MAX_SIZE];
	char password[_DEF_MAX_SIZE];
}_STRU_REGISTER_RQ;

//注册回复：协议头、注册结果
typedef struct _STRU_REGISTER_RS {
	_STRU_REGISTER_RS():type(_DEF_TCP_REGISTER_RS),result(name_is_exists)
	{}
	PackType type;
	int result;
}_STRU_REGISTER_RS;

//登录请求：协议头、电话、密码
typedef struct _STRU_LOGIN_RQ {
	_STRU_LOGIN_RQ() :type(_DEF_TCP_LOGIN_RQ)
	{
		memset(tel, 0, _DEF_MAX_SIZE);
		memset(password, 0, _DEF_MAX_SIZE);
	}
	PackType type;
	char tel[_DEF_MAX_SIZE];
	char password[_DEF_MAX_SIZE];
}_STRU_LOGIN_RQ;
//登陆回复：协议头、登陆结果
typedef struct _STRU_LOGIN_RS {
    _STRU_LOGIN_RS() :type(_DEF_TCP_LOGIN_RS), result(user_not_exists),id(0)
    {}
    PackType type;
    int result;
    int id;
}_STRU_LOGIN_RS;

//获取好友信息：协议头、昵称、头像id、状态、签名、
typedef struct _STRU_FRIEND_INFO {
    _STRU_FRIEND_INFO() :type(_DEF_TCP_FRIEND_INFO), iconId(-1), status(status_offline),id(0)
	{
		memset(name, 0, _DEF_MAX_SIZE);
        memset(feeling,0,_DEF_MAX_SIZE);
	}
	PackType type;
	char name[_DEF_MAX_SIZE];
    int id;
	int iconId;
	int status;
    char feeling[_DEF_MAX_SIZE];
}_STRU_FRIEND_INFO;

//聊天请求：协议头、聊天内容、好友id、自己的id
typedef struct _STRU_CHAT_RQ {
	_STRU_CHAT_RQ() :type(_DEF_TCP_CHAT_RQ), userId(0), friendId(0)
	{
		memset(content, 0, _DEF_CONTENT_SIZE);
        memset(name,0,_DEF_MAX_SIZE);
	}
	PackType type;
	int userId;
	int friendId;
    char name[_DEF_MAX_SIZE];
	char content[_DEF_CONTENT_SIZE];
}_STRU_CHAT_RQ;

//聊天回复：协议头、发送结果
typedef struct _STRU_CHAT_RS {
    _STRU_CHAT_RS() :type(_DEF_TCP_CHAT_RS), result(send_fail),friendId(0)
    {
    }
    PackType type;
    int result;
    int friendId;
}_STRU_CHAT_RS;

//添加好友请求：协议头、好友昵称、自己的id、自己的昵称
typedef struct _STRU_ADD_FRIEND_RQ {
	_STRU_ADD_FRIEND_RQ() :type(_DEF_TCP_ADD_FRIEND_RQ), userId(0)
	{
		memset(userName, 0, _DEF_MAX_SIZE);
		memset(friendName, 0, _DEF_MAX_SIZE);
	}
	PackType type;
	char userName[_DEF_MAX_SIZE];
	int userId;
	char friendName[_DEF_MAX_SIZE];
}_STRU_ADD_FRIEND_RQ;
//添加好友回复：协议头、添加结果、自己的id、好友的id、好友的昵称
typedef struct _STRU_ADD_FRIEND_RS {
	_STRU_ADD_FRIEND_RS() :type(_DEF_TCP_ADD_FRIEND_RS), userId(0),friendId(0),result(friend_offline)
	{
		memset(friendName, 0, _DEF_MAX_SIZE);
	}
	PackType type;
	int userId;
	int friendId;
	char friendName[_DEF_MAX_SIZE];
	int result;
}_STRU_ADD_FRIEND_RS;

//下线请求：协议头、自己的id
typedef struct _STRU_OFFLINE_RQ {
	_STRU_OFFLINE_RQ() :type(_DEF_TCP_OFFLINE_RQ), userId(0)
	{

	}
	PackType type;
	int userId;
}_STRU_OFFLINE_RQ;
