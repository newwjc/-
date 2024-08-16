#pragma once
#include <list>

#define  _DEF_UDP_PORT (34523)
#define  _DEF_TCP_PORT (78965)
#define  _DEF_MAX_SIZE (100)
#define  _DEF_CONTENT_SIZE (4096)
#define  _DEF_MAX_FILE_PATH (512)
#define  _DEF_FILE_CONTENT_SIZE (8*1024)
#define  _DEF_PROTOCOL_COUNT (20)

//Э��ͷ
#define _DEF_TCP_PROTOCOL_BASE (1000)
//ע��
#define _DEF_TCP_REGISTER_RQ   (_DEF_TCP_PROTOCOL_BASE +1)
#define _DEF_TCP_REGISTER_RS   (_DEF_TCP_PROTOCOL_BASE +2)

//��¼
#define _DEF_TCP_LOGIN_RQ   (_DEF_TCP_PROTOCOL_BASE +3)
#define _DEF_TCP_LOGIN_RS   (_DEF_TCP_PROTOCOL_BASE +4)

//��ȡ������Ϣ
#define _DEF_TCP_FRIEND_INFO   (_DEF_TCP_PROTOCOL_BASE +5)
//��ȡȺ����Ϣ
#define _DEF_TCP_GROUP_INFO (_DEF_TCP_PROTOCOL_BASE +6)
//����
#define _DEF_TCP_CHAT_RQ   (_DEF_TCP_PROTOCOL_BASE +7)
#define _DEF_TCP_CHAT_RS   (_DEF_TCP_PROTOCOL_BASE +8)
//��Ӻ���
#define _DEF_TCP_ADD_FRIEND_RQ   (_DEF_TCP_PROTOCOL_BASE +9)
#define _DEF_TCP_ADD_FRIEND_RS   (_DEF_TCP_PROTOCOL_BASE +10)


//����
#define _DEF_TCP_OFFLINE_RQ   (_DEF_TCP_PROTOCOL_BASE +11)

//����Ⱥ��
#define _DEF_TCP_CREATE_GROUP_RQ  (_DEF_TCP_PROTOCOL_BASE + 12)
#define _DEF_TCP_CREATE_GROUP_RS  (_DEF_TCP_PROTOCOL_BASE + 13)
//���Ⱥ��Ա
#define _DEF_TCP_ADD_GROUPFRIEND_RQ  (_DEF_TCP_PROTOCOL_BASE + 14)
#define _DEF_TCP_ADD_GROUPFRIEND_RS  (_DEF_TCP_PROTOCOL_BASE + 15)
//Ⱥ������
#define _DEF_TCP_GROUPCHAT_RQ  (_DEF_TCP_PROTOCOL_BASE + 16)
#define _DEF_TCP_GROUPCHAT_RS  (_DEF_TCP_PROTOCOL_BASE + 17)

//�����ļ�����
#define _DEF_TCP_FILE_INFO_RQ  (_DEF_TCP_PROTOCOL_BASE + 17)
#define _DEF_TCP_FILE_INFO_RS  (_DEF_TCP_PROTOCOL_BASE + 18)

//�����ļ�������
#define _DEF_TCP_SEND_FILEBLK_RQ  (_DEF_TCP_PROTOCOL_BASE + 19)




//�������
//ע����
#define register_success    (0)
#define tel_is_used         (1)
#define name_is_exists      (2)

//��½���
#define login_success       (0)
#define password_error		(1)
#define user_not_exists		(2)

//������
#define send_success        (0)
#define send_fail           (1)

//��Ӻ��ѽ��
#define add_success         (0)
#define friend_not_exists   (1)
#define friend_refuse       (2)
#define friend_offline      (3)

//�û�״̬
#define status_online       (0)
#define status_offline      (1)

//�����ļ����
#define file_friend_offline (0)
#define file_friend_refuse  (1)
#define file_friend_success (2)

//���¶���һ��Э��ͷ����
typedef int PackType;

//����ṹ��
//ע������Э��ͷ���ǳơ��ֻ��š�����
typedef struct _STRU_REGISTER_RQ {
    _STRU_REGISTER_RQ() :type(_DEF_TCP_REGISTER_RQ)
    {
        memset(name, 0, _DEF_MAX_SIZE);
        memset(tel, 0, _DEF_MAX_SIZE);
        memset(password, 0, _DEF_MAX_SIZE);
    }
    PackType type;
    char name[_DEF_MAX_SIZE];
    char tel[_DEF_MAX_SIZE];
    char password[_DEF_MAX_SIZE];
}_STRU_REGISTER_RQ;

//ע��ظ���Э��ͷ��ע����
typedef struct _STRU_REGISTER_RS {
    _STRU_REGISTER_RS() :type(_DEF_TCP_REGISTER_RS), result(name_is_exists)
    {}
    PackType type;
    int result;
}_STRU_REGISTER_RS;

//��¼����Э��ͷ���绰������
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
//��½�ظ���Э��ͷ����½���
typedef struct _STRU_LOGIN_RS {
    _STRU_LOGIN_RS() :type(_DEF_TCP_LOGIN_RS), result(user_not_exists), id(0)
    {}
    PackType type;
    int result;
    int id;
}_STRU_LOGIN_RS;

//��ȡ������Ϣ��Э��ͷ���ǳơ�ͷ��id��״̬��ǩ����
typedef struct _STRU_FRIEND_INFO {
    _STRU_FRIEND_INFO() :type(_DEF_TCP_FRIEND_INFO), iconId(-1), status(status_offline), id(0)
    {
        memset(name, 0, _DEF_MAX_SIZE);
        memset(feeling, 0, _DEF_MAX_SIZE);
        memset(GroupList, 0, _DEF_MAX_SIZE * sizeof(int));
        memset(GroupName, 0, _DEF_MAX_SIZE * sizeof(char*));
    }
    PackType type;
    char name[_DEF_MAX_SIZE];
    int id;
    int iconId;
    int status;
    char feeling[_DEF_MAX_SIZE];
    int GroupList[_DEF_MAX_SIZE];
    char* GroupName[_DEF_MAX_SIZE];
}_STRU_FRIEND_INFO;

//��ȡȺ����Ϣ��Э��ͷ��Ⱥ��id���顢Ⱥ��name����
typedef struct _STRU_GROUP_INFO {
    _STRU_GROUP_INFO() :type(_DEF_TCP_GROUP_INFO), GroupId(0)
    {

        memset(GroupName, 0, _DEF_MAX_SIZE);
    }
    PackType type;
    int GroupId;
    char GroupName[_DEF_MAX_SIZE];
}_STRU_GROUP_INFO;

//��������Э��ͷ���������ݡ�����id���Լ���id
typedef struct _STRU_CHAT_RQ {
    _STRU_CHAT_RQ() :type(_DEF_TCP_CHAT_RQ), userId(0), friendId(0)
    {
        memset(content, 0, _DEF_CONTENT_SIZE);
        memset(name, 0, _DEF_MAX_SIZE);
    }
    PackType type;
    int userId;
    int friendId;
    char name[_DEF_MAX_SIZE];
    char content[_DEF_CONTENT_SIZE];
}_STRU_CHAT_RQ;

//����ظ���Э��ͷ�����ͽ��
typedef struct _STRU_CHAT_RS {
    _STRU_CHAT_RS() :type(_DEF_TCP_CHAT_RS), result(send_fail), friendId(0)
    {
    }
    PackType type;
    int result;
    int friendId;
}_STRU_CHAT_RS;

//��Ӻ�������Э��ͷ�������ǳơ��Լ���id���Լ����ǳ�
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
//��Ӻ��ѻظ���Э��ͷ����ӽ�����Լ���id�����ѵ�id�����ѵ��ǳ�
typedef struct _STRU_ADD_FRIEND_RS {
    _STRU_ADD_FRIEND_RS() :type(_DEF_TCP_ADD_FRIEND_RS), userId(0), friendId(0), result(friend_offline)
    {
        memset(friendName, 0, _DEF_MAX_SIZE);
    }
    PackType type;
    int userId;
    int friendId;
    char friendName[_DEF_MAX_SIZE];
    int result;
}_STRU_ADD_FRIEND_RS;

//����Ⱥ����Э��ͷ���Լ���id��Ⱥ������Ⱥ�����ơ�Ҫ��ӵ�Ⱥ�ĵ��û�����
typedef struct _STRU_CREATE_GROUP_RQ {
    _STRU_CREATE_GROUP_RQ() :type(_DEF_TCP_CREATE_GROUP_RQ), userid(0)
    {
        memset(GroupName, 0, _DEF_MAX_SIZE);
        memset(FriendNameList, 0, _DEF_MAX_SIZE * sizeof(int));
    }

    PackType type;
    int userid;
    char GroupName[_DEF_MAX_SIZE];
    int FriendNameList[_DEF_MAX_SIZE];

}_STRU_CREATE_GROUP_RQ;
//����Ⱥ�ظ���Э��ͷ���Լ���id��Ⱥ������Ⱥ�š�Ⱥ������
typedef struct _STRU_CREATE_GROUP_RS {
    _STRU_CREATE_GROUP_RS() :type(_DEF_TCP_CREATE_GROUP_RS), userid(0), Groupid(0)
    {
        memset(GroupName, 0, _DEF_MAX_SIZE);
    }

    PackType type;
    int userid;
    int Groupid;
    char GroupName[_DEF_MAX_SIZE];

}_STRU_CREATE_GROUP_RS;

//Ⱥ������:Э��ͷ���������ݡ�Ⱥ��id���Լ���id
typedef struct _STRU_GROUPCHAT_RQ {
    _STRU_GROUPCHAT_RQ() :type(_DEF_TCP_GROUPCHAT_RQ), GroupId(0), userId(0) {
        memset(content, 0, sizeof(_DEF_CONTENT_SIZE));
        memset(content, 0, sizeof(_DEF_MAX_SIZE));
    }
    PackType type;
    int GroupId;
    int userId;
    char userName[_DEF_MAX_SIZE];
    char content[_DEF_CONTENT_SIZE];
}_STRU_GROUPCHAT_RQ;
//Ⱥ�Ļظ���Э��ͷ�����ͽ��
typedef struct _STRU_GROUPCHAT_RS {

}_STRU_GROUPCHAT_RS;

//�����ļ���Ϣ����Э��ͷ�������ļ��û�id�������ļ��û�id���ļ���־���ļ������ļ���С
typedef struct _STRU_FILE_INFO_RQ {
    _STRU_FILE_INFO_RQ() :type(_DEF_TCP_FILE_INFO_RQ), userId(0), friendId(0), szFileSize(0) {
        memset(szFileID, 0, sizeof(_DEF_MAX_FILE_PATH));
        memset(szFileName, 0, sizeof(_DEF_MAX_FILE_PATH));
    }
    PackType type;
    int userId;
    int friendId;
    char szFileID[_DEF_MAX_FILE_PATH];
    char szFileName[_DEF_MAX_FILE_PATH];
    int  szFileSize;

}_STRU_FILE_INFO_RQ;

//�����ļ���Ϣ�ظ���Э��ͷ����������ͻظ��û�id�����ջظ��û�id���ļ���־
typedef struct _STRU_FILE_INFO_RS {
    _STRU_FILE_INFO_RS() :type(_DEF_TCP_FILE_INFO_RS), userId(0), friendId(0), szFileSize(0), result(file_friend_offline) {
        memset(szFileID, 0, sizeof(_DEF_MAX_FILE_PATH));
    }
    PackType type;
    int userId;
    int friendId;
    //char friendName[_DEF_MAX_SIZE];
    char szFileID[_DEF_MAX_FILE_PATH];
    int szFileSize;
    int result;

}_STRU_FILE_INFO_RS;

//�����ļ�������Э��ͷ�������ļ����û�id�������ļ����û�id���ļ��顢���С���ļ���־
typedef struct _STRU_FILE_BLOCK_RQ {
    _STRU_FILE_BLOCK_RQ() :type(_DEF_TCP_SEND_FILEBLK_RQ),userId(0), friendId(0), nBlockSize(0) {
        memset(szFileID, 0, sizeof(_DEF_MAX_FILE_PATH));
        memset(szFileContent, 0, sizeof(_DEF_FILE_CONTENT_SIZE));
    }
    PackType type;
    int userId;
    int friendId;
    char szFileID[_DEF_MAX_FILE_PATH];
    char szFileContent[_DEF_FILE_CONTENT_SIZE];
    int nBlockSize;

}_STRU_FILE_BLOCK_RQ;

//�ļ���Ϣ:�ļ������ļ���־���ļ�·�����ļ���С����ǰλ�á��ܴ�С���ļ�ָ��
typedef struct FileInfo {
    FileInfo() : nFileSize(0), nPos(0) {
        memset(szFileName, 0, sizeof(_DEF_MAX_FILE_PATH));
        memset(szFileID, 0, sizeof(_DEF_MAX_FILE_PATH));
        memset(szFilePath, 0, sizeof(_DEF_MAX_FILE_PATH));
    }
    char szFileName[_DEF_MAX_FILE_PATH];
    char szFileID[_DEF_MAX_FILE_PATH];
    char szFilePath[_DEF_MAX_FILE_PATH];
    int	 nFileSize;
    int nPos;
    FILE* pFile;
}FileInfo;


//��������Э��ͷ���Լ���id
typedef struct _STRU_OFFLINE_RQ {
    _STRU_OFFLINE_RQ() :type(_DEF_TCP_OFFLINE_RQ), userId(0)
    {

    }
    PackType type;
    int userId;
}_STRU_OFFLINE_RQ;
