#include "CKernel.h"
#include"TcpServerMediator.h"

CKernel* CKernel::pKernel = nullptr;
//定义协议头换算成数组下标的宏
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
//初始化函数指针数组
void CKernel::setNetprotocol()
{
    cout << "CKernel::setNetprotocol" << endl;
    //初始化数组为0
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
//打开服务器
bool CKernel::startServer()
{
    cout << "CKernel::startServer" << endl;
    //1、连接数据库
    if (!m_sql.ConnectMySql("127.0.0.1","root","199891","20220919im"))
    {
        cout << "连接数据库失败" << endl;
        return false;
    }
    //2、打开网络
    m_pMediator = new TcpServerNetMediator;
    if (!m_pMediator->OpenNet())
    {
        cout << "打开网络失败" << endl;
        return false;
    }
    return true;
}

//关闭服务器
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



//分发所有接收到的数据，调用对应的处理函数
void CKernel::dealReadyData(char* buf, int nLen, long lSend)
{
    cout << "CKernel::dealReadyData" << endl;
    //1、取出协议头
    PackType type = *(PackType*)buf;
    //2、根据协议头调用对应函数
    //2、1先判断数组下标是否在有效范围内
    if (type >= _DEF_TCP_REGISTER_RQ && type <= _DEF_TCP_SEND_FILEBLK_RQ)
    {
        PFUN pf = NetProtocol(type);
        if (pf)
        {
            (this->*pf)(buf, nLen, lSend);
        }
        else
        {
            //如果打印这个日志，有两个可能，1、协议头和处理函数没有绑定；2、type值在结构体初始化错了
            cout<< "pf 协议头：" << type << endl;
        }
    }
    else
    {
        cout << "协议头：" << type << endl;
    }
    //回收空间
    delete []buf;

}

//处理注册请求
void CKernel::dealRegisterRq(char* buf, int nLen, long lSend)
{
    cout << "CKernel::dealRegisterRq" << endl;
    //1、拆包
    _STRU_REGISTER_RQ* rq = (_STRU_REGISTER_RQ*)buf;
    //2、校验用户输入的数据（跟客户端校验规则一致）
    //3、校验电话号码是否被注册过
    //3.1、根据电话号码查询电话号码
    list<string>resList;
    char sqlBuf[1024] = "";
    sprintf(sqlBuf, "select tel from t_user where tel = '%s';", rq->tel);
   
    if (!m_sql.SelectMySql(sqlBuf,1,resList))
    {
        cout << "查询数据库失败,sql: " << sqlBuf << endl;
        return;
    }
    _STRU_REGISTER_RS rs;
    //3.2判断查询结果
    if (resList.size() > 0)
    {
        //4、如果结果不为空，说明电话号码已经被注册，给注册结果赋值，tel_is_used
        rs.result = tel_is_used;
    }else{
        //4、如果结果为空，从判断昵称受否被注册过
        //4.1、根据昵称查询昵称
        sprintf(sqlBuf, "select name from t_user where name = '%s';", rq->name);
        if (!m_sql.SelectMySql(sqlBuf, 1, resList))
        {
            cout << "查询数据库失败,sql: " << sqlBuf << endl;
            return;
        }
        //4.2判断查询结果
        if (resList.size() > 0)
        {
            //5、如果结果不为空，说明昵称已经被注册过，给注册结果赋值，name_is_exists
            rs.result = name_is_exists;
        }
        else
        {
            //6、如果结果为空，把用户的信息写入数据库
            sprintf(sqlBuf, "insert into t_user (name,tel,password,feeling,iconid) values ('%s','%s','%s','12345',1);", rq->name,rq->tel,rq->password);
            if (!m_sql.UpdateMySql(sqlBuf))
            {
                cout << "查询数据库失败,sql: " << sqlBuf << endl;
                return;
            }
            //7、给注册结果赋值
            rs.result = register_success;
        }

    }

    //8、回复客户端
    m_pMediator->SendData((char*)&rs, sizeof(rs), lSend);
}

//处理登录请求
void CKernel::dealLoginRq(char* buf, int nLen, long lSend)
{
    cout << "CKernel::dealLoginRq" << endl;
    //1、拆包
    _STRU_LOGIN_RQ* rq = (_STRU_LOGIN_RQ*)buf;
    //2、校验
    //3、根据电话号码查询密码
    list<string>resList;
    char sqlBuf[1024] = "";
    sprintf(sqlBuf, "select id,password from t_user where tel = '%s';", rq->tel);
    if (!m_sql.SelectMySql(sqlBuf, 2, resList))
    {
        cout << "查询数据库失败, sql: " << sqlBuf << endl;
        return;
    }

    _STRU_LOGIN_RS rs;
    //4、判断查询结果
    if (resList.empty())
    {
        //5、如果结果为空，说明该电话号码未被注册，给结果赋值 user_not_exists
        rs.result = user_not_exists;
    }
    else 
    {
        //取出用户id
        int userId = atoi(resList.front().c_str());
        resList.pop_front();

        //6、如果结果不为空，比较用户输入的密码和从数据库中查询到的密码是否相同

        string password = resList.front();
        resList.pop_front();
        if (0 != strcmp(password.c_str(),rq->password))
        {
            //7、如果结果不相同，说明密码错误，给结果赋值 password_error
            rs.result = password_error;
        }
        //if (rq->password != password)
        //{
        //    //7、如果结果不相同，说明密码错误，给结果赋值 password_error
        //    rs.result = password_error;
        //}
        else 
        {
            //8、如果相同，给结果赋值 login_success
            rs.result = login_success;
            rs.id = userId;

            //9、给客户端回复结果
            m_pMediator->SendData((char*)&rs, sizeof(rs), lSend);

            //保存当前登录用户的socket到map，key是id，value是socket
            m_mapIdToSocket[userId] = lSend;
            //查询当前登录用户的信息及其他好友的信息
            getUserInfo(userId);
            _STRU_GROUP_INFO group_info;
            getGroupInfo(userId, group_info);
            return;
        }
    }
    //9、给客户端回复结果
    m_pMediator->SendData((char*)&rs, sizeof(rs), lSend);

}

//查询用户信息
void CKernel::getUserInfo(int id)
{
    //1、根据自己的id查询自己的信息
    cout << "CKernel::getUserInfo" << endl;

    _STRU_FRIEND_INFO userInfo;
    getUserInfoById(id,&userInfo);
    //2、把自己的信息发回给客户端

    m_pMediator->SendData((char*)&userInfo, sizeof(userInfo), m_mapIdToSocket[id]);

    //3、根据用户的id查询好友id列表
    list<string>resList;
    char sqlBuf[1024] = "";
    sprintf(sqlBuf, "select idB from t_friend where IdA = '%d';", id);
    if (!m_sql.SelectMySql(sqlBuf, 1, resList))
    {
        cout << "查询数据库失败, sql: " << sqlBuf << endl;
        return;
    }

    int friendId = 0;
    _STRU_FRIEND_INFO friendInfo;
    //4、遍历好友的id
    while (resList.size() > 0)
    {
        //5、取出好友的id
        friendId =atoi(resList.front().c_str());
        resList.pop_front();

        //6、根据好友id查询好友信息
        getUserInfoById(friendId,&friendInfo);

        //7、把好友的信息发回给客户端
        m_pMediator->SendData((char*)&friendInfo, sizeof(friendInfo), m_mapIdToSocket[id]);

        //8、查询当前好友是否在线
        if (m_mapIdToSocket.count(friendId) > 0)
        {
            //如果好友在线，通知好友自己上线了
            m_pMediator->SendData((char*)&userInfo,sizeof(userInfo) , m_mapIdToSocket[friendId]);
        }
    }
}
//根据id查询信息
void CKernel::getUserInfoById(int id, _STRU_FRIEND_INFO* info)
{
    cout << "CKernel::getUserInfoById" << endl;
    info->id = id;
    //从数据库中查询用户的签名，头像id，昵称
    list<string>resList;
    char sqlBuf[1024] = "";
    sprintf(sqlBuf, "select name,iconid,feeling from t_user where id = '%d';", id);
    if (!m_sql.SelectMySql(sqlBuf, 3, resList))
    {
        cout << "查询数据库失败, sql: " << sqlBuf << endl;
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
    //判断用户状态
    if (m_mapIdToSocket.count(id) > 0)
    {
        info->status = status_online;
    }
    else 
    {
        info->status = status_offline;
    }


}
//查询群组信息
void CKernel::getGroupInfo(int id, _STRU_GROUP_INFO info)
{
    //从数据库中查询用户拥有的群组id
    list<string>resList;
    list<string>resNameList;
    char sqlBuf[1024] = "";
    sprintf(sqlBuf, "select group_id from group_members where member_id = '%d';", id);
    if (!m_sql.SelectMySql(sqlBuf, 1, resList))
    {
        cout << "查询数据库失败, sql: " << sqlBuf << endl;
        return;
    }
    int i = 0;
    for (auto ite = resList.begin(); ite != resList.end(); ite++) {
        string GroupStrId = *ite;
        int groupId = atoi(GroupStrId.c_str());
        info.GroupId = groupId;
        //根据GroupId查名字
        sprintf(sqlBuf, "select group_name from groups where group_id = '%d';", groupId);
        if (!m_sql.SelectMySql(sqlBuf, 1, resNameList))
        {
            cout << "查询数据库失败, sql: " << sqlBuf << endl;
            return;
        }
        strcpy(info.GroupName, resNameList.front().c_str());
        resNameList.pop_front();
        //把信息发给客户端
        m_pMediator->SendData((char*)&info, sizeof(info), m_mapIdToSocket[id]);
    }

}


//处理聊天请求
void CKernel::dealChatRq(char* buf, int nLen, long lSend)
{
    cout << "CKernel::dealChatRq" << endl;

    //拆包
    _STRU_CHAT_RQ* rq = (_STRU_CHAT_RQ*)buf;
    
    _STRU_CHAT_RS rs;
    //判断好友是否在线
    if (m_mapIdToSocket.count(rq->friendId) <= 0)
    {
        //如果好友不在线，直接给客户端返回send_fail
        rs.result = send_fail;
        rs.friendId = rq->friendId;
        m_pMediator->SendData((char*)&rs, sizeof(rs), lSend);
    }
    else
    {
        //如果好友在线，把聊天请求转发给好友
        //m_pMediator->SendData((char*)rq, sizeof(rq), m_mapIdToSocket[rq->friendId]);
        m_pMediator->SendData(buf,nLen,m_mapIdToSocket[rq->friendId]);
    }
}

//处理群聊请求
void CKernel::dealGroupChatRq(char* buf, int nLen, long lSend)
{
    cout << "CKernel::dealGroupChatRq" << endl;
    //拆包
    _STRU_GROUPCHAT_RQ* rq = (_STRU_GROUPCHAT_RQ*)buf;
    //根据群id查询除了发送者的id
    list<string>resList;
    char sqlBuf[1024] = "";
    sprintf(sqlBuf, "select member_id from group_members where group_id = '%d';", rq->GroupId);
    if (!m_sql.SelectMySql(sqlBuf, 1, resList))
    {
        cout << "查询数据库失败, sql: " << sqlBuf << endl;
        return;
    }
    while (resList.size() > 0)
    {
        int FriendId = atoi(resList.front().c_str());
        //判断群成员是否在线
        if (m_mapIdToSocket.count(FriendId) > 0)
        {
            //把群聊请求转发给好友
            m_pMediator->SendData(buf, nLen, m_mapIdToSocket[FriendId]);
        }
        else
        {
            //给客户端发送好友不在线回复
        }
        resList.pop_front();
    }

}

//处理下线请求
void CKernel::dealOfflineRq(char* buf, int nLen, long lSend)
{
    cout << "CKernel::dealOfflineRq" << endl;
    //1、拆包
    _STRU_OFFLINE_RQ* rq = (_STRU_OFFLINE_RQ*)buf;
    //2、查询下线用户的好友id列表
    list<string>resList;
    char sqlBuf[1024] = "";
    sprintf(sqlBuf, "select idB from t_friend where IdA = '%d';", rq->userId);
    if (!m_sql.SelectMySql(sqlBuf, 1, resList))
    {
        cout << "查询数据库失败, sql: " << sqlBuf << endl;
        return;
    }
    int friendId = 0;
    //3、遍历好友的id列表
    while (resList.size() > 0)
    {
        //4、取出好友的id
        friendId = atoi(resList.front().c_str());
        resList.pop_front();
        //5、把下线的请求转发给在线的好友
        if (m_mapIdToSocket.count(friendId) > 0)
        {
            m_pMediator->SendData(buf,nLen,m_mapIdToSocket[friendId]);
        }
    }
    //6、把下线的用户的socket从map中移除
    auto ite = m_mapIdToSocket.find(rq->userId);
    if (ite != m_mapIdToSocket.end())
    {
        m_mapIdToSocket.erase(ite);
    }
       

}
//处理添加好友请求
void CKernel::dealAddFriendRq(char* buf, int nLen, long lSend)
{
    cout << "CKernel::dealAddFriendRq" << endl;

    //1、拆包
    _STRU_ADD_FRIEND_RQ* rq= (_STRU_ADD_FRIEND_RQ*)buf;
    //2、查询数据库
    list<string>resList;
    char sqlBuf[1024] = "";
    sprintf(sqlBuf, "select id from t_user where name = '%s';", rq->friendName);
    if (!m_sql.SelectMySql(sqlBuf, 1, resList))
    {
        cout << "查询数据库失败, sql: " << sqlBuf << endl;
        return;
    }
    int friendId = 0;
    //3、判断待添加B用户是否存在
    if (resList.size() > 0)
    {
        //取出好友id
        friendId = atoi(resList.front().c_str());
        resList.pop_front();
       
        //4、如果B用户存在，判断B用户是否在线
        if (m_mapIdToSocket.count(friendId) >0)
        {
           //5、如果用户B在线，转发添加好友请求给B用户
            m_pMediator->SendData(buf,nLen,m_mapIdToSocket[friendId]);
        }
        else
        {
            //6、如果B用户不在线，给A用户回复添加好友回复，添加好友失败friend_offline
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
         //7、如果B用户不存在，给A用户回复添加好友回复，添加好友失败friend_not_exists
        _STRU_ADD_FRIEND_RS rs;
        rs.result = friend_not_exists;
        rs.friendId = friendId;
        strcpy_s(rs.friendName, rq->friendName);
        rs.userId = rq->userId;
        m_pMediator->SendData((char*)&rs, sizeof(rs), lSend);
    }

}
//处理添加好友回复
void CKernel::dealAddFriendRs(char* buf, int nLen, long lSend)
{
    cout << "CKernel::dealAddFriendRs" << endl;

    //1、拆包
    _STRU_ADD_FRIEND_RS* rs = (_STRU_ADD_FRIEND_RS*)buf;
    //2、判断B用户是否同意添加为好友
    if (rs->result == add_success)
    {
        //3、如果同意添加好友，把好友关系写入数据库
        char sqlBuf[1024] = "";
        sprintf(sqlBuf, "insert into t_friend(idA,idB) values ('%d','%d');", rs->userId,rs->friendId);
        if (!m_sql.UpdateMySql(sqlBuf))
        {
            cout << "插入数据库失败, sql: " << sqlBuf << endl;
            return;
        }
        sprintf(sqlBuf, "insert into t_friend(idA,idB) values ('%d','%d');", rs->friendId, rs->userId);
        if (!m_sql.UpdateMySql(sqlBuf))
        {
            cout << "插入数据库失败, sql: " << sqlBuf << endl;
            return;
        }

        //刷新好友列表
        getUserInfo(rs->userId);
    }
    //4、不管是否同意，都需要把添加结果返回给A客户端
    m_pMediator->SendData(buf, nLen, m_mapIdToSocket[rs->userId]);
}
//处理创建群聊请求
void CKernel::dealCreateGroupRq(char* buf, int nLen, long lSend)
{
    cout << "CKernel::dealCreateGroupRq" << endl;
    //1、拆包
    _STRU_CREATE_GROUP_RQ* rq = (_STRU_CREATE_GROUP_RQ*)buf;

    //根据发送过来的群聊名称创建群聊
    char sqlBuf[1024] = "";
    sprintf(sqlBuf, "call InsertUniqueGroupID('%s'); ",rq->GroupName);
    if (!m_sql.UpdateMySql(sqlBuf))
    {
        cout << "插入数据库失败, sql: " << sqlBuf << endl;
        return;
    }

    //根据群名查群号
    list<string>resList;
    sprintf(sqlBuf, "select group_id from groups where group_name = '%s';", rq->GroupName);
    if (!m_sql.SelectMySql(sqlBuf, 1, resList))
    {
        cout << "查询数据库失败, sql: " << sqlBuf << endl;
        return;
    }
    //获得群号
    int groupId = 0;
    if (resList.size() > 0)
    {
        groupId = atoi(resList.front().c_str());

    }
    //把发起者id插入到群聊和群成员id数据库中
    sprintf(sqlBuf, "insert into group_members(group_id,member_id) values ('%d','%d');", groupId, rq->userid);
    if (!m_sql.UpdateMySql(sqlBuf))
    {
        cout << "插入数据库失败, sql: " << sqlBuf << endl;
        return;
    }
    //构建回复包
    //_STRU_CREATE_GROUP_RS rs;
    //rs.Groupid = groupId;
    //strcpy(rs.GroupName,rq->GroupName);

    ////给发送id回复
    //m_pMediator->SendData((char*)&rs,sizeof(rs),m_mapIdToSocket[rq->userid]);
    //更新群聊列表
    _STRU_GROUP_INFO info;
    getGroupInfo(rq->userid, info);
    //查看lst中的成员id是否在线

    int friendId = 0;
    for (int i = 0; i < 100;i++) {
        friendId = rq->FriendNameList[i];
        if (friendId != 0) {
            if (m_mapIdToSocket.count(friendId) > 0) {
                //把好友的id插入到群聊和群成员id数据库中
                sprintf(sqlBuf, "insert into group_members(group_id,member_id) values ('%d','%d');", groupId, friendId);
                if (!m_sql.UpdateMySql(sqlBuf))
                {
                    cout << "插入数据库失败, sql: " << sqlBuf << endl;
                    return;
                }
                //更新群聊信息
                getGroupInfo(friendId, info);
            }
        }


    }
    //while (rq->FriendNameList.size() > 0) {
    //    friendId = rq->FriendNameList.front();
    //    rq->FriendNameList.pop_front();

    //    if (m_mapIdToSocket.count(friendId) > 0) {
    //        //把好友的id插入到群聊和群成员id数据库中
    //        sprintf(sqlBuf, "insert into group_members(group_id,member_id) values ('%d','%d');", groupId, friendId);
    //        if (!m_sql.UpdateMySql(sqlBuf))
    //        {
    //            cout << "插入数据库失败, sql: " << sqlBuf << endl;
    //            return;
    //        }
    //        //给好友发送回复
    //        m_pMediator->SendData((char*)&rs, sizeof(rs), m_mapIdToSocket[friendId]);
    //    }
    //} 
}
//处理传输文件请求
void CKernel::dealFileSendRq(char* buf, int nLen, long lSend)
{
    cout << "CKernel::dealFileSendRq" << endl;
    //拆包
    _STRU_FILE_INFO_RQ* rq = (_STRU_FILE_INFO_RQ*)buf;
    //回复包
    _STRU_FILE_INFO_RS rs;
    //判断好友是否在线
    if (!(m_mapIdToSocket.count(rq->friendId) > 0)) {   
        //好友不在线，给客户端返回消息告知其不在线
        rs.result = file_friend_offline;
        rs.userId = rq->userId;
        rs.friendId = rq->friendId;
        m_pMediator->SendData((char*)&rs, sizeof(rs),lSend);
    }
    else
    {
        //好友在线，把文件传输请求传给好友
        m_pMediator->SendData(buf, nLen, m_mapIdToSocket[rq->friendId]);
    }

}
//处理传输文件回复
void CKernel::dealFileSendRs(char* buf, int nLen, long lSend)
{
    cout << "CKernel::dealFileSendRs" << endl;
    _STRU_FILE_INFO_RS* rs = (_STRU_FILE_INFO_RS*)buf;
    //不管用户拒绝还是同意，把回复转发给客户端
    m_pMediator->SendData(buf, nLen, m_mapIdToSocket[rs->userId]);
}

//处理文件块请求（直接转发给好友）
void CKernel::dealSendFileBlockRq(char* buf, int nLen, long lSend)
{   
    cout << "CKernel::SendFileBlockRq" << endl;
    //拆包
    _STRU_FILE_BLOCK_RQ* rq = (_STRU_FILE_BLOCK_RQ*)buf;
    //将文件块转发给好友
    m_pMediator->SendData(buf, nLen, m_mapIdToSocket[rq->friendId]);
}