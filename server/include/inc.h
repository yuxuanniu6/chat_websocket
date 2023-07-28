#ifndef INC_H
#define INC_H

#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include <websocketpp/logger/syslog.hpp>
#include <iostream>
#include <iomanip> // 包含输入/输出流格式化库
#include <cstdio> // 包含格式化字符串函数
#include <vector>
#include <pthread.h>
#include <string>
#include <set>
#include <map>
#include <unordered_map>
#include <mutex>
#include <jsoncpp/json/json.h>
#include "json.h"

using websocketpp::connection_hdl;
using websocketpp::lib::placeholders::_1; //占位符
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::placeholders::_3;
using websocketpp::lib::bind;
typedef websocketpp::server<websocketpp::config::asio> server;
typedef server::message_ptr message_ptr;
typedef websocketpp::connection_hdl connection_hdl;


enum msgType
{
    LOGIN_MSG = 1,
    REG_MSG,
    JOIN_ROOM_MSG,      //加入房间
    LEAVE_ROOM_MSG,     //离开房间
    ONE_CHAT_MSG,            //私聊
    ROOM_CHAT_MSG,           //房间内部chat
};


#endif