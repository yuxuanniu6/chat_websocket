#ifndef SERVER_H
#define SERVER_H

#include "RoomManager.h"

class Server
{
public:
    //注册日志，绑定基本事件
    Server(uint16_t port);
public:
    void on_http(connection_hdl hdl);
    void on_open(connection_hdl hdl);
    void on_close(connection_hdl hdl);
    void on_fail(connection_hdl hdl);
    void on_message(connection_hdl hdl, message_ptr msg);
    void run();
private:
    typedef std::set<connection_hdl,std::owner_less<connection_hdl>> con_list;
    con_list m_cons; //总链接集合
    server m_endpoint;
    // Telemetry data
    uint64_t m_count; //链接数
    std::mutex m_lock;
};

#endif