#include "CKernel.h"
#include"TcpServerMediator.h"

CKernel* CKernel::pKernel = nullptr;
//����Э��ͷ����������±�ĺ�
#define NetProtocol(x) m_netProtocol[x- _DEF_TCP_PROTOCOL_BASE -1]
CKernel::CKernel()
{
	pKernel = this;
    setNetprotocol();

}
CKernel::~CKernel()
{
    closeServer();
}
//��ʼ������ָ������
void CKernel::setNetprotocol()
{
    cout << "CKernel::setNetprotocol" << endl;
    //��ʼ������Ϊ0
    memset(m_netProtocol, 0, sizeof(m_netProtocol));
    NetProtocol(_DEF_TCP_REGISTER_RQ)     = &CKernel::dealRegisterRq;
    NetProtocol(_DEF_TCP_LOGIN_RQ)        = &CKernel::dealLoginRq;
    NetProtocol(_DEF_TCP_CHAT_RQ)         = &CKernel::dealChatRq;
    NetProtocol(_DEF_TCP_GROUPCHAT_RQ)    = &CKernel::dealGroupChatRq;
    NetProtocol(_DEF_TCP_OFFLINE_RQ)      = &CKernel::dealOfflineRq;
    NetProtocol(_DEF_TCP_ADD_FRIEND_RQ)   = &CKernel::dealAddFriendRq;
    NetProtocol(_DEF_TCP_ADD_FRIEND_RS)   = &CKernel::dealAddFriendRs;
    NetProtocol(_DEF_TCP_CREATE_GROUP_RQ) = &CKernel::dealCreateGroupRq;
    NetProtocol(_DEF_TCP_FILE_INFO_RQ)    = &CKernel::dealFileSendRq;
    NetProtocol(_DEF_TCP_FILE_INFO_RS)    = &CKernel::dealFileSendRs;
    NetProtocol(_DEF_TCP_SEND_FILEBLK_RQ) = &CKernel::dealSendFileBlockRq;
}
//�򿪷�����
bool CKernel::startServer()
{
    cout << "CKernel::startServer" << endl;
    //1���������ݿ�
    if (!m_sql.ConnectMySql("127.0.0.1","root","199891","20220919im"))
    {
        cout << "�������ݿ�ʧ��" << endl;
        return false;
    }
    //2��������
    m_pMediator = new TcpServerNetMediator;
    if (!m_pMediator->OpenNet())
    {
        cout << "������ʧ��" << endl;
        return false;
    }
    return true;
}

//�رշ�����
void CKernel::closeServer()
{
    m_sql.DisConnect();
    if (m_pMediator)
    {
        m_pMediator->CloseNet();
        delete m_pMediator;
        m_pMediator = nullptr;
    }
}



//�ַ����н��յ������ݣ����ö�Ӧ�Ĵ�����
void CKernel::dealReadyData(char* buf, int nLen, long lSend)
{
    cout << "CKernel::dealReadyData" << endl;
    //1��ȡ��Э��ͷ
    PackType type = *(PackType*)buf;
    //2������Э��ͷ���ö�Ӧ����
    //2��1���ж������±��Ƿ�����Ч��Χ��
    if (type >= _DEF_TCP_REGISTER_RQ && type <= _DEF_TCP_SEND_FILEBLK_RQ)
    {
        PFUN pf = NetProtocol(type);
        if (pf)
        {
            (this->*pf)(buf, nLen, lSend);
        }
        else
        {
            //�����ӡ�����־�����������ܣ�1��Э��ͷ�ʹ�����û�а󶨣�2��typeֵ�ڽṹ���ʼ������
            cout<< "pf Э��ͷ��" << type << endl;
        }
    }
    else
    {
        cout << "Э��ͷ��" << type << endl;
    }
    //���տռ�
    delete []buf;

}

//����ע������
void CKernel::dealRegisterRq(char* buf, int nLen, long lSend)
{
    cout << "CKernel::dealRegisterRq" << endl;
    //1�����
    _STRU_REGISTER_RQ* rq = (_STRU_REGISTER_RQ*)buf;
    //2��У���û���������ݣ����ͻ���У�����һ�£�
    //3��У��绰�����Ƿ�ע���
    //3.1�����ݵ绰�����ѯ�绰����
    list<string>resList;
    char sqlBuf[1024] = "";
    sprintf(sqlBuf, "select tel from t_user where tel = '%s';", rq->tel);
   
    if (!m_sql.SelectMySql(sqlBuf,1,resList))
    {
        cout << "��ѯ���ݿ�ʧ��,sql: " << sqlBuf << endl;
        return;
    }
    _STRU_REGISTER_RS rs;
    //3.2�жϲ�ѯ���
    if (resList.size() > 0)
    {
        //4����������Ϊ�գ�˵���绰�����Ѿ���ע�ᣬ��ע������ֵ��tel_is_used
        rs.result = tel_is_used;
    }else{
        //4��������Ϊ�գ����ж��ǳ��ܷ�ע���
        //4.1�������ǳƲ�ѯ�ǳ�
        sprintf(sqlBuf, "select name from t_user where name = '%s';", rq->name);
        if (!m_sql.SelectMySql(sqlBuf, 1, resList))
        {
            cout << "��ѯ���ݿ�ʧ��,sql: " << sqlBuf << endl;
            return;
        }
        //4.2�жϲ�ѯ���
        if (resList.size() > 0)
        {
            //5����������Ϊ�գ�˵���ǳ��Ѿ���ע�������ע������ֵ��name_is_exists
            rs.result = name_is_exists;
        }
        else
        {
            //6��������Ϊ�գ����û�����Ϣд�����ݿ�
            sprintf(sqlBuf, "insert into t_user (name,tel,password,feeling,iconid) values ('%s','%s','%s','12345',1);", rq->name,rq->tel,rq->password);
            if (!m_sql.UpdateMySql(sqlBuf))
            {
                cout << "��ѯ���ݿ�ʧ��,sql: " << sqlBuf << endl;
                return;
            }
            //7����ע������ֵ
            rs.result = register_success;
        }

    }

    //8���ظ��ͻ���
    m_pMediator->SendData((char*)&rs, sizeof(rs), lSend);
}

//�����¼����
void CKernel::dealLoginRq(char* buf, int nLen, long lSend)
{
    cout << "CKernel::dealLoginRq" << endl;
    //1�����
    _STRU_LOGIN_RQ* rq = (_STRU_LOGIN_RQ*)buf;
    //2��У��
    //3�����ݵ绰�����ѯ����
    list<string>resList;
    char sqlBuf[1024] = "";
    sprintf(sqlBuf, "select id,password from t_user where tel = '%s';", rq->tel);
    if (!m_sql.SelectMySql(sqlBuf, 2, resList))
    {
        cout << "��ѯ���ݿ�ʧ��, sql: " << sqlBuf << endl;
        return;
    }

    _STRU_LOGIN_RS rs;
    //4���жϲ�ѯ���
    if (resList.empty())
    {
        //5��������Ϊ�գ�˵���õ绰����δ��ע�ᣬ�������ֵ user_not_exists
        rs.result = user_not_exists;
    }
    else 
    {
        //ȡ���û�id
        int userId = atoi(resList.front().c_str());
        resList.pop_front();

        //6����������Ϊ�գ��Ƚ��û����������ʹ����ݿ��в�ѯ���������Ƿ���ͬ

        string password = resList.front();
        resList.pop_front();
        if (0 != strcmp(password.c_str(),rq->password))
        {
            //7������������ͬ��˵��������󣬸������ֵ password_error
            rs.result = password_error;
        }
        //if (rq->password != password)
        //{
        //    //7������������ͬ��˵��������󣬸������ֵ password_error
        //    rs.result = password_error;
        //}
        else 
        {
            //8�������ͬ���������ֵ login_success
            rs.result = login_success;
            rs.id = userId;

            //9�����ͻ��˻ظ����
            m_pMediator->SendData((char*)&rs, sizeof(rs), lSend);

            //���浱ǰ��¼�û���socket��map��key��id��value��socket
            m_mapIdToSocket[userId] = lSend;
            //��ѯ��ǰ��¼�û�����Ϣ���������ѵ���Ϣ
            getUserInfo(userId);
            _STRU_GROUP_INFO group_info;
            getGroupInfo(userId, group_info);
            return;
        }
    }
    //9�����ͻ��˻ظ����
    m_pMediator->SendData((char*)&rs, sizeof(rs), lSend);

}

//��ѯ�û���Ϣ
void CKernel::getUserInfo(int id)
{
    //1�������Լ���id��ѯ�Լ�����Ϣ
    cout << "CKernel::getUserInfo" << endl;

    _STRU_FRIEND_INFO userInfo;
    getUserInfoById(id,&userInfo);
    //2�����Լ�����Ϣ���ظ��ͻ���

    m_pMediator->SendData((char*)&userInfo, sizeof(userInfo), m_mapIdToSocket[id]);

    //3�������û���id��ѯ����id�б�
    list<string>resList;
    char sqlBuf[1024] = "";
    sprintf(sqlBuf, "select idB from t_friend where IdA = '%d';", id);
    if (!m_sql.SelectMySql(sqlBuf, 1, resList))
    {
        cout << "��ѯ���ݿ�ʧ��, sql: " << sqlBuf << endl;
        return;
    }

    int friendId = 0;
    _STRU_FRIEND_INFO friendInfo;
    //4���������ѵ�id
    while (resList.size() > 0)
    {
        //5��ȡ�����ѵ�id
        friendId =atoi(resList.front().c_str());
        resList.pop_front();

        //6�����ݺ���id��ѯ������Ϣ
        getUserInfoById(friendId,&friendInfo);

        //7���Ѻ��ѵ���Ϣ���ظ��ͻ���
        m_pMediator->SendData((char*)&friendInfo, sizeof(friendInfo), m_mapIdToSocket[id]);

        //8����ѯ��ǰ�����Ƿ�����
        if (m_mapIdToSocket.count(friendId) > 0)
        {
            //����������ߣ�֪ͨ�����Լ�������
            m_pMediator->SendData((char*)&userInfo,sizeof(userInfo) , m_mapIdToSocket[friendId]);
        }
    }
}
//����id��ѯ��Ϣ
void CKernel::getUserInfoById(int id, _STRU_FRIEND_INFO* info)
{
    cout << "CKernel::getUserInfoById" << endl;
    info->id = id;
    //�����ݿ��в�ѯ�û���ǩ����ͷ��id���ǳ�
    list<string>resList;
    char sqlBuf[1024] = "";
    sprintf(sqlBuf, "select name,iconid,feeling from t_user where id = '%d';", id);
    if (!m_sql.SelectMySql(sqlBuf, 3, resList))
    {
        cout << "��ѯ���ݿ�ʧ��, sql: " << sqlBuf << endl;
        return;
    }

    if (resList.size() > 0)
    {
        strcpy_s(info->name, resList.front().c_str());
        resList.pop_front();

        info->iconId = atoi(resList.front().c_str());
        resList.pop_front();

        strcpy_s(info->feeling, resList.front().c_str());
        resList.pop_front();
    }
    //�ж��û�״̬
    if (m_mapIdToSocket.count(id) > 0)
    {
        info->status = status_online;
    }
    else 
    {
        info->status = status_offline;
    }


}
//��ѯȺ����Ϣ
void CKernel::getGroupInfo(int id, _STRU_GROUP_INFO info)
{
    //�����ݿ��в�ѯ�û�ӵ�е�Ⱥ��id
    list<string>resList;
    list<string>resNameList;
    char sqlBuf[1024] = "";
    sprintf(sqlBuf, "select group_id from group_members where member_id = '%d';", id);
    if (!m_sql.SelectMySql(sqlBuf, 1, resList))
    {
        cout << "��ѯ���ݿ�ʧ��, sql: " << sqlBuf << endl;
        return;
    }
    int i = 0;
    for (auto ite = resList.begin(); ite != resList.end(); ite++) {
        string GroupStrId = *ite;
        int groupId = atoi(GroupStrId.c_str());
        info.GroupId = groupId;
        //����GroupId������
        sprintf(sqlBuf, "select group_name from groups where group_id = '%d';", groupId);
        if (!m_sql.SelectMySql(sqlBuf, 1, resNameList))
        {
            cout << "��ѯ���ݿ�ʧ��, sql: " << sqlBuf << endl;
            return;
        }
        strcpy(info.GroupName, resNameList.front().c_str());
        resNameList.pop_front();
        //����Ϣ�����ͻ���
        m_pMediator->SendData((char*)&info, sizeof(info), m_mapIdToSocket[id]);
    }

}


//������������
void CKernel::dealChatRq(char* buf, int nLen, long lSend)
{
    cout << "CKernel::dealChatRq" << endl;

    //���
    _STRU_CHAT_RQ* rq = (_STRU_CHAT_RQ*)buf;
    
    _STRU_CHAT_RS rs;
    //�жϺ����Ƿ�����
    if (m_mapIdToSocket.count(rq->friendId) <= 0)
    {
        //������Ѳ����ߣ�ֱ�Ӹ��ͻ��˷���send_fail
        rs.result = send_fail;
        rs.friendId = rq->friendId;
        m_pMediator->SendData((char*)&rs, sizeof(rs), lSend);
    }
    else
    {
        //����������ߣ�����������ת��������
        //m_pMediator->SendData((char*)rq, sizeof(rq), m_mapIdToSocket[rq->friendId]);
        m_pMediator->SendData(buf,nLen,m_mapIdToSocket[rq->friendId]);
    }
}

//����Ⱥ������
void CKernel::dealGroupChatRq(char* buf, int nLen, long lSend)
{
    cout << "CKernel::dealGroupChatRq" << endl;
    //���
    _STRU_GROUPCHAT_RQ* rq = (_STRU_GROUPCHAT_RQ*)buf;
    //����Ⱥid��ѯ���˷����ߵ�id
    list<string>resList;
    char sqlBuf[1024] = "";
    sprintf(sqlBuf, "select member_id from group_members where group_id = '%d';", rq->GroupId);
    if (!m_sql.SelectMySql(sqlBuf, 1, resList))
    {
        cout << "��ѯ���ݿ�ʧ��, sql: " << sqlBuf << endl;
        return;
    }
    while (resList.size() > 0)
    {
        int FriendId = atoi(resList.front().c_str());
        //�ж�Ⱥ��Ա�Ƿ�����
        if (m_mapIdToSocket.count(FriendId) > 0)
        {
            //��Ⱥ������ת��������
            m_pMediator->SendData(buf, nLen, m_mapIdToSocket[FriendId]);
        }
        else
        {
            //���ͻ��˷��ͺ��Ѳ����߻ظ�
        }
        resList.pop_front();
    }

}

//������������
void CKernel::dealOfflineRq(char* buf, int nLen, long lSend)
{
    cout << "CKernel::dealOfflineRq" << endl;
    //1�����
    _STRU_OFFLINE_RQ* rq = (_STRU_OFFLINE_RQ*)buf;
    //2����ѯ�����û��ĺ���id�б�
    list<string>resList;
    char sqlBuf[1024] = "";
    sprintf(sqlBuf, "select idB from t_friend where IdA = '%d';", rq->userId);
    if (!m_sql.SelectMySql(sqlBuf, 1, resList))
    {
        cout << "��ѯ���ݿ�ʧ��, sql: " << sqlBuf << endl;
        return;
    }
    int friendId = 0;
    //3���������ѵ�id�б�
    while (resList.size() > 0)
    {
        //4��ȡ�����ѵ�id
        friendId = atoi(resList.front().c_str());
        resList.pop_front();
        //5�������ߵ�����ת�������ߵĺ���
        if (m_mapIdToSocket.count(friendId) > 0)
        {
            m_pMediator->SendData(buf,nLen,m_mapIdToSocket[friendId]);
        }
    }
    //6�������ߵ��û���socket��map���Ƴ�
    auto ite = m_mapIdToSocket.find(rq->userId);
    if (ite != m_mapIdToSocket.end())
    {
        m_mapIdToSocket.erase(ite);
    }
       

}
//������Ӻ�������
void CKernel::dealAddFriendRq(char* buf, int nLen, long lSend)
{
    cout << "CKernel::dealAddFriendRq" << endl;

    //1�����
    _STRU_ADD_FRIEND_RQ* rq= (_STRU_ADD_FRIEND_RQ*)buf;
    //2����ѯ���ݿ�
    list<string>resList;
    char sqlBuf[1024] = "";
    sprintf(sqlBuf, "select id from t_user where name = '%s';", rq->friendName);
    if (!m_sql.SelectMySql(sqlBuf, 1, resList))
    {
        cout << "��ѯ���ݿ�ʧ��, sql: " << sqlBuf << endl;
        return;
    }
    int friendId = 0;
    //3���жϴ����B�û��Ƿ����
    if (resList.size() > 0)
    {
        //ȡ������id
        friendId = atoi(resList.front().c_str());
        resList.pop_front();
       
        //4�����B�û����ڣ��ж�B�û��Ƿ�����
        if (m_mapIdToSocket.count(friendId) >0)
        {
           //5������û�B���ߣ�ת����Ӻ��������B�û�
            m_pMediator->SendData(buf,nLen,m_mapIdToSocket[friendId]);
        }
        else
        {
            //6�����B�û������ߣ���A�û��ظ���Ӻ��ѻظ�����Ӻ���ʧ��friend_offline
            _STRU_ADD_FRIEND_RS rs;
            rs.result = friend_offline;
            rs.friendId = friendId;
            strcpy_s(rs.friendName,rq->friendName);
            rs.userId = rq->userId;
            m_pMediator->SendData((char*)&rs,sizeof(rs),lSend);
        }
    }
    else
    {
         //7�����B�û������ڣ���A�û��ظ���Ӻ��ѻظ�����Ӻ���ʧ��friend_not_exists
        _STRU_ADD_FRIEND_RS rs;
        rs.result = friend_not_exists;
        rs.friendId = friendId;
        strcpy_s(rs.friendName, rq->friendName);
        rs.userId = rq->userId;
        m_pMediator->SendData((char*)&rs, sizeof(rs), lSend);
    }

}
//������Ӻ��ѻظ�
void CKernel::dealAddFriendRs(char* buf, int nLen, long lSend)
{
    cout << "CKernel::dealAddFriendRs" << endl;

    //1�����
    _STRU_ADD_FRIEND_RS* rs = (_STRU_ADD_FRIEND_RS*)buf;
    //2���ж�B�û��Ƿ�ͬ�����Ϊ����
    if (rs->result == add_success)
    {
        //3�����ͬ����Ӻ��ѣ��Ѻ��ѹ�ϵд�����ݿ�
        char sqlBuf[1024] = "";
        sprintf(sqlBuf, "insert into t_friend(idA,idB) values ('%d','%d');", rs->userId,rs->friendId);
        if (!m_sql.UpdateMySql(sqlBuf))
        {
            cout << "�������ݿ�ʧ��, sql: " << sqlBuf << endl;
            return;
        }
        sprintf(sqlBuf, "insert into t_friend(idA,idB) values ('%d','%d');", rs->friendId, rs->userId);
        if (!m_sql.UpdateMySql(sqlBuf))
        {
            cout << "�������ݿ�ʧ��, sql: " << sqlBuf << endl;
            return;
        }

        //ˢ�º����б�
        getUserInfo(rs->userId);
    }
    //4�������Ƿ�ͬ�⣬����Ҫ����ӽ�����ظ�A�ͻ���
    m_pMediator->SendData(buf, nLen, m_mapIdToSocket[rs->userId]);
}
//������Ⱥ������
void CKernel::dealCreateGroupRq(char* buf, int nLen, long lSend)
{
    cout << "CKernel::dealCreateGroupRq" << endl;
    //1�����
    _STRU_CREATE_GROUP_RQ* rq = (_STRU_CREATE_GROUP_RQ*)buf;

    //���ݷ��͹�����Ⱥ�����ƴ���Ⱥ��
    char sqlBuf[1024] = "";
    sprintf(sqlBuf, "call InsertUniqueGroupID('%s'); ",rq->GroupName);
    if (!m_sql.UpdateMySql(sqlBuf))
    {
        cout << "�������ݿ�ʧ��, sql: " << sqlBuf << endl;
        return;
    }

    //����Ⱥ����Ⱥ��
    list<string>resList;
    sprintf(sqlBuf, "select group_id from groups where group_name = '%s';", rq->GroupName);
    if (!m_sql.SelectMySql(sqlBuf, 1, resList))
    {
        cout << "��ѯ���ݿ�ʧ��, sql: " << sqlBuf << endl;
        return;
    }
    //���Ⱥ��
    int groupId = 0;
    if (resList.size() > 0)
    {
        groupId = atoi(resList.front().c_str());

    }
    //�ѷ�����id���뵽Ⱥ�ĺ�Ⱥ��Աid���ݿ���
    sprintf(sqlBuf, "insert into group_members(group_id,member_id) values ('%d','%d');", groupId, rq->userid);
    if (!m_sql.UpdateMySql(sqlBuf))
    {
        cout << "�������ݿ�ʧ��, sql: " << sqlBuf << endl;
        return;
    }
    //�����ظ���
    //_STRU_CREATE_GROUP_RS rs;
    //rs.Groupid = groupId;
    //strcpy(rs.GroupName,rq->GroupName);

    ////������id�ظ�
    //m_pMediator->SendData((char*)&rs,sizeof(rs),m_mapIdToSocket[rq->userid]);
    //����Ⱥ���б�
    _STRU_GROUP_INFO info;
    getGroupInfo(rq->userid, info);
    //�鿴lst�еĳ�Աid�Ƿ�����

    int friendId = 0;
    for (int i = 0; i < 100;i++) {
        friendId = rq->FriendNameList[i];
        if (friendId != 0) {
            if (m_mapIdToSocket.count(friendId) > 0) {
                //�Ѻ��ѵ�id���뵽Ⱥ�ĺ�Ⱥ��Աid���ݿ���
                sprintf(sqlBuf, "insert into group_members(group_id,member_id) values ('%d','%d');", groupId, friendId);
                if (!m_sql.UpdateMySql(sqlBuf))
                {
                    cout << "�������ݿ�ʧ��, sql: " << sqlBuf << endl;
                    return;
                }
                //����Ⱥ����Ϣ
                getGroupInfo(friendId, info);
            }
        }


    }
    //while (rq->FriendNameList.size() > 0) {
    //    friendId = rq->FriendNameList.front();
    //    rq->FriendNameList.pop_front();

    //    if (m_mapIdToSocket.count(friendId) > 0) {
    //        //�Ѻ��ѵ�id���뵽Ⱥ�ĺ�Ⱥ��Աid���ݿ���
    //        sprintf(sqlBuf, "insert into group_members(group_id,member_id) values ('%d','%d');", groupId, friendId);
    //        if (!m_sql.UpdateMySql(sqlBuf))
    //        {
    //            cout << "�������ݿ�ʧ��, sql: " << sqlBuf << endl;
    //            return;
    //        }
    //        //�����ѷ��ͻظ�
    //        m_pMediator->SendData((char*)&rs, sizeof(rs), m_mapIdToSocket[friendId]);
    //    }
    //} 
}
//�������ļ�����
void CKernel::dealFileSendRq(char* buf, int nLen, long lSend)
{
    cout << "CKernel::dealFileSendRq" << endl;
    //���
    _STRU_FILE_INFO_RQ* rq = (_STRU_FILE_INFO_RQ*)buf;
    //�ظ���
    _STRU_FILE_INFO_RS rs;
    //�жϺ����Ƿ�����
    if (!(m_mapIdToSocket.count(rq->friendId) > 0)) {   
        //���Ѳ����ߣ����ͻ��˷�����Ϣ��֪�䲻����
        rs.result = file_friend_offline;
        rs.userId = rq->userId;
        rs.friendId = rq->friendId;
        m_pMediator->SendData((char*)&rs, sizeof(rs),lSend);
    }
    else
    {
        //�������ߣ����ļ��������󴫸�����
        m_pMediator->SendData(buf, nLen, m_mapIdToSocket[rq->friendId]);
    }

}
//�������ļ��ظ�
void CKernel::dealFileSendRs(char* buf, int nLen, long lSend)
{
    cout << "CKernel::dealFileSendRs" << endl;
    _STRU_FILE_INFO_RS* rs = (_STRU_FILE_INFO_RS*)buf;
    //�����û��ܾ�����ͬ�⣬�ѻظ�ת�����ͻ���
    m_pMediator->SendData(buf, nLen, m_mapIdToSocket[rs->userId]);
}

//�����ļ�������ֱ��ת�������ѣ�
void CKernel::dealSendFileBlockRq(char* buf, int nLen, long lSend)
{   
    cout << "CKernel::SendFileBlockRq" << endl;
    //���
    _STRU_FILE_BLOCK_RQ* rq = (_STRU_FILE_BLOCK_RQ*)buf;
    //���ļ���ת��������
    m_pMediator->SendData(buf, nLen, m_mapIdToSocket[rq->friendId]);
}