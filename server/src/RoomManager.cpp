#include "RoomManager.h"

RoomManager::RoomManager()
        :m_nextRoomId(-1)
        ,m_bigRoomId(0)
{
    m_MsgHandlerMap.insert({LOGIN_MSG, std::bind(&RoomManager::login, this, _1, _2, _3)}); //绑定对应函数
    m_MsgHandlerMap.insert({JOIN_ROOM_MSG, std::bind(&RoomManager::joinRoom, this, _1, _2, _3)}); //绑定对应函数
    m_MsgHandlerMap.insert({LEAVE_ROOM_MSG, std::bind(&RoomManager::leaveRoom, this, _1, _2, _3)}); //绑定对应函数
    m_MsgHandlerMap.insert({ONE_CHAT_MSG, std::bind(&RoomManager::oneChat, this, _1, _2, _3)});
    m_MsgHandlerMap.insert({ROOM_CHAT_MSG, std::bind(&RoomManager::RoomChat, this, _1, _2, _3)});
}
RoomManager* RoomManager::instance()
{
    static RoomManager roomManager;
    return &roomManager;
}
msgHandler RoomManager::getHandler(server *s, int msgid)
{
    auto it = m_MsgHandlerMap.find(msgid);
    if(it != m_MsgHandlerMap.end())
    {
        return m_MsgHandlerMap[msgid];
    }
    else
    {
        //返回一个空的
        return [=](server *s, connection_hdl hdl, Json::Value &js){
//            std::cout << "msgid: " << msgid << "can't find handler" << std::endl;
            LOG(LogLevel::INFO, "msgid: %d can't find handler!", msgid);
        };
    }
}
std::string RoomManager::createRoom()
{
    std::unique_lock<std::mutex> lock(m_lock);
    std::string roomId = "room" + std::to_string(m_nextRoomId++);
    m_rooms.emplace(roomId, Room{roomId});
    return roomId;
}
void RoomManager::leaveBigRoom(connection_hdl hdl, Json::Value &js)
{
    std::string roomId = js["roomId"].asString();//拿到具体房间号
    auto it = m_rooms.find(roomId);
    if(it != m_rooms.end())
    {
        int id = findKeysByValue(it->second.m_connections, hdl);
        it->second.m_connections.erase(id);//释放之前的链接
    }
    PrintRooms(roomId);
}
int RoomManager::findKeysByValue(const std::map<int, connection_hdl>& myMap, const connection_hdl& value)
{
    int id;
    for (const auto& pair : myMap) {
        //需要获得实际指针       *(ptr.lock())解引用
        if (auto ptr1 = pair.second.lock()) {
            if (auto ptr2 = value.lock()) {
                if (ptr1 == ptr2) {
                    id = pair.first;
                    break;
                }
            }
        }
    }
    return id; //id唯一
}
//暂时未校验登录、注册
void RoomManager::login(server *s, connection_hdl hdl, Json::Value &js)
{
    int id = js["id"].asInt();
    std::unique_lock<std::mutex> lock(m_lock);
    m_conMap.insert({id, hdl});
}
void RoomManager::joinRoom(server *s, connection_hdl hdl, Json::Value &js)
{
    int id = js["id"].asInt();
    std::string roomId = js["roomId"].asString();//拿到具体房间号
    //给系统提示，开发人员自测
    if(roomId.empty()){
        LOG(LogLevel::INFO, "roomId is %s", roomId.c_str());
        return;
    }
    auto it = m_rooms.find(roomId);
    //没有房间时   zhihuishu
    if (it == m_rooms.end()) {
        //大房间特殊处理
        if(roomId == "zhihuishu") {
            std::unique_lock <std::mutex> lock(m_lock);
            ++m_bigRoomId;
            //map<string, Room>   Room{string, con_map}   con_map<id, hdl>
            m_rooms.emplace(roomId, Room{roomId, {{m_bigRoomId,hdl}}});
        }
        else {
            std::unique_lock <std::mutex> lock(m_lock);
            m_rooms.emplace(roomId, Room{roomId,{{{id,hdl}}}});
        }
//        Room room;
//        room.m_id = roomId;
//        room.m_connections[id] = hdl;
//        m_rooms[roomId] = room;
    } else {
        if(roomId == "zhihuishu") {
            std::unique_lock <std::mutex> lock(m_lock);
            ++m_bigRoomId;
            it->second.m_connections[m_bigRoomId] = hdl;
        }
        else{
            std::cout << "已有房间" << std::endl;
            LOG(LogLevel::INFO, "already have this room !");
            std::unique_lock <std::mutex> lock(m_lock);
            it->second.m_connections[id] = hdl;
        }
    }
    PrintRooms(roomId);
}
void RoomManager::leaveRoom(server *s, connection_hdl hdl, Json::Value &js)
{
    int id = js["id"].asInt();
    std::string roomId = js["leaveRoomId"].asString();//拿到具体房间号
    auto it = m_rooms.find(roomId);
    if(it != m_rooms.end())
    {
        it->second.m_connections.erase(id);//释放之前的链接
        //如果房间没人
        if(it->second.m_connections.empty())
        {
            std::unique_lock <std::mutex> lock(m_lock);
            m_rooms.erase(it);
        }
    }
    PrintRooms(roomId);
}
void RoomManager::oneChat(server *s, connection_hdl hdl, Json::Value &js)
{
    //解析msg
    int toId = js["to"].asInt();
    std::string msg = js["message"].asString();
    {
        //在查找操作前加锁，可以确保同一时间只有一个线程在访问 m_conMap，避免竞争条件和数据不一致性。
        std::unique_lock<std::mutex> lock(m_lock);
        auto it = m_conMap.find(toId);
        //业务上，一般用户不存在的情况不会出现          一般是对方离线，所以后期加离线处理
        if(it != m_conMap.end()){
            //根据toid找到hdl，server转发到对端
            server::connection_ptr con = s->get_con_from_hdl(it->second);
            con->send(msg, websocketpp::frame::opcode::text);
            return;
        }
        else{
            //存储于离线消息中
        }
    }
}
void RoomManager::RoomChat(server *s, connection_hdl hdl, Json::Value &js)
{
    int id = js["id"].asInt();
    std::string msg = js["message"].asString();
    const std::string roomId = findUserRoom(id);  //根据发送者id找到对应房间号
    //群发
    auto it = m_rooms.find(roomId);
    if(it != m_rooms.end())
    {
        for(auto room : it->second.m_connections)
        {
            //不发给自己
            if(room.first == id)
                continue;
            try
            {
                s->send(room.second, msg, websocketpp::frame::opcode::text);
            } catch (websocketpp::exception const & e)
            {
                std::string errorMsg(e.what());
                LOG(LogLevel::WARNING, "Echo failed because: (%s)",errorMsg.c_str());
//                std::cout << "Echo failed because: " << "(" << e.what() << ")" << std::endl;
            }
        }
    }
}

//暂未实现     广播
void RoomManager::broadcast(server *s, const std::string& roomId, const std::string& message)
{
//        //谁发送：hdl, msg:xxxx
//        auto it = m_rooms.find(ddd);
//        //谁加入了某个房间，当前应该有标识，或者某个房间保存了该用户
    auto it = m_rooms.find(roomId);
    if(it != m_rooms.end())
    {
        for(auto room : it->second.m_connections)
        {
            try
            {
                s->send(room.second, message, websocketpp::frame::opcode::text);
            } catch (websocketpp::exception const & e)
            {
                std::string errorMsg(e.what());
                LOG(LogLevel::WARNING, "Echo failed because: (%s)",errorMsg.c_str());
//                std::cout << "Echo failed because: " << "(" << e.what() << ")" << std::endl;
            }
        }
    }
}
//根据发送者id找到对应房间号
const std::string RoomManager::findUserRoom(const int& id)
{
    //for  m_rooms;
    auto it = m_rooms.begin();
    while(it != m_rooms.end())
    {
        auto itt = it->second.m_connections.find(id);
        if(itt != it->second.m_connections.end())
        {
            return it->second.m_id;
        }
    }
}

//打印某个房间的成员链接
void RoomManager::PrintRooms(const std::string& roomId)
{
    std::cout << "roomid: " << roomId << std::endl;
//    LOG(LogLevel::INFO, "roomid:%d", roomId);
    auto it = m_rooms.find(roomId);
    if(it != m_rooms.end())
    {
        for(auto room : it->second.m_connections){
//            LOG(LogLevel::INFO, "%s", room.second.lock().get().c_str());
            std::cout << room.second.lock().get() << std::endl;
        }
        std::cout << std::endl;
    }

}