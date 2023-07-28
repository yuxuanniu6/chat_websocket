#ifndef ROOMMANAGER_H
#define ROOMMANAGER_H

#include "inc.h"
#include "log.h"

//函数指针
using msgHandler = std::function<void(server *s, connection_hdl hdl, Json::Value &js)>;

//定义房间对象
struct Room
{
    std::string m_id;                               //房间号
    std::map<int, connection_hdl> m_connections;    //id和hdl映射      id=0为大房间
//    std::set<connection_hdl, std::owner_less<connection_hdl>> m_connections;  //每个房间的客户端链接
};

//房间管理类
class RoomManager
{
public:
    static RoomManager* instance();                                         //单例模式
    msgHandler getHandler(server *s, int msgid);                            //返回对应的函数操作
    std::string createRoom();                                               //创建一个房间
    void login(server *s, connection_hdl hdl, Json::Value &js);             //登录{"msgid":1,"id":11,"account": 20190583016"password":"666"}
    void joinRoom(server *s, connection_hdl hdl, Json::Value &js);          //加入房间{"msgid":1,"id":12,"roomId":"zhihuishu"}
    void leaveRoom(server *s, connection_hdl hdl, Json::Value &js);         //离开房间{"msgid":2,"id":12,"leaveRoomId":"xxxx"}
    void leaveBigRoom(connection_hdl hdl, Json::Value &js);
    void oneChat(server *s, connection_hdl hdl, Json::Value &js);           //一对一聊天{"msgid":5,"id":11,"from":11,"to":12,"message":"这是一条测试消息"}
    void RoomChat(server *s, connection_hdl hdl, Json::Value &js);          //房间内聊天{"msgid":6,"id":11,"message":"发送给某个房间的所有成员"}
    void broadcast(server *s, const std::string& roomId, const std::string& message);   //暂未实现
    const std::string findUserRoom(const int& id);                          //查找用户在那个房间
    void PrintRooms(const std::string& roomId);
    int findKeysByValue(const std::map<int, connection_hdl>& myMap, const connection_hdl& value);
private:
    RoomManager();
private:
    //哈希记录消息id和对应的业务处理方法
    std::unordered_map<int, msgHandler> m_MsgHandlerMap;
    std::mutex m_lock;  //锁机制
    int m_nextRoomId;   //暂未使用
    int m_bigRoomId;    //大房间id累加+
    std::map<std::string, Room> m_rooms;    //每个房间的客户端连接
    std::map<int, connection_hdl> m_conMap; //全部链接
};

#endif