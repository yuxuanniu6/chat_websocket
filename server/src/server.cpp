#include "server.h"

Server::Server(uint16_t port)
:m_count(0)
{
    m_endpoint.set_access_channels(websocketpp::log::alevel::all);
    m_endpoint.clear_access_channels(websocketpp::log::alevel::frame_payload);

    // Initialize Asio
    m_endpoint.init_asio();

    // Register our message handler
    m_endpoint.set_open_handler(bind(&Server::on_open, this, ::_1));
    m_endpoint.set_http_handler(bind(&Server::on_http, this, ::_1));
    m_endpoint.set_message_handler(bind(&Server::on_message, this, ::_1, ::_2));
    m_endpoint.set_close_handler(bind(&Server::on_close, this, ::_1));

    // Listen on port 8081
    m_endpoint.listen(port);

    // Start the server accept loop
    m_endpoint.start_accept();

}
void Server::run()
{
    // Start the ASIO io_service run loop
    try {
        m_endpoint.run();
    } catch (websocketpp::exception const & e) {
        std::cout << e.what() << std::endl;
    }
}
void Server::on_http(connection_hdl hdl)
{
    server::connection_ptr con = m_endpoint.get_con_from_hdl(hdl);

    std::string res = con->get_request_body();

    std::stringstream ss;
    ss << "got HTTP request with " << res.size() << " bytes of body data.";

    con->set_body(ss.str());
    con->set_status(websocketpp::http::status_code::ok);
}
void Server::on_open(connection_hdl hdl)
{
    std::cout << "new connection_hdl is arrive" << std::endl;
    server::connection_ptr conn = m_endpoint.get_con_from_hdl(hdl);
    std::string roomId = conn->get_request_header("Room-ID");
    Json::Value js;
    js["roomId"] = roomId;
    RoomManager::instance()->joinRoom(&m_endpoint, hdl, js); //创建链接加入大房间
//    //从客户端请求中获取要加入的房间ID
//    m_roomManager.joinRoom(roomId, hdl);
//    m_roomManager.PrintRooms(&m_endpoint, roomId);
    //链接数+1
    {
        std::unique_lock<std::mutex> lock(m_lock);
        m_cons.insert(hdl);
    }
}
void Server::on_close(connection_hdl hdl)
{
    server::connection_ptr conn = m_endpoint.get_con_from_hdl(hdl);
    std::string roomId = conn->get_request_header("Room-ID");
    Json::Value js;
    js["roomId"] = roomId;
    RoomManager::instance()->leaveBigRoom(hdl, js); //创建链接离开大房间
    std::cout << "on_close called with hdl: " << hdl.lock().get() << std::endl;
    //链接数+1
    {
        std::unique_lock<std::mutex> lock(m_lock);
        m_cons.erase(hdl);
    }
}
void Server::on_fail(connection_hdl hdl)
{
    server::connection_ptr con = m_endpoint.get_con_from_hdl(hdl);
    std::cout << "Fail handler: " << con->get_ec() << " " << con->get_ec().message()  << std::endl;
}
//业务消息处理
void Server::on_message(connection_hdl hdl, message_ptr msg)
{
    //get_payload()获取消息
    std::cout << "on_message called with hdl: " << hdl.lock().get()
              << " and message: \n" << msg->get_payload()
              << std::endl;
    std::string message = msg->get_payload();
//    std::cout << "message: " << message << std::endl;
    //解析json数据
    Json::Value js;
    Json::CharReaderBuilder reader;
    std::istringstream iss(message);
    std::string errs;
    try{
        if(!Json::parseFromStream(reader, iss, &js, &errs)){
            throw std::runtime_error("Failed to parse JSON: " + errs);
        }
        //解析    拿到对应操作标志位
        int msgid = js["msgid"].asInt();
        auto msgHandler = RoomManager::instance()->getHandler(&m_endpoint, msgid); //传给操控回调的函数
        msgHandler(&m_endpoint, hdl, js);//根据msgid调用对应的事件处理函数
    }
    catch(const std::exception& e){
        std::cerr << "Error: " << e.what() << std::endl;
        return;
    }


//    m_roomManager.broadcast(&m_endpoint, "zhihuishu", message);
    //s->send(hdl, msg->get_payload(), msg->get_opcode());
    //发送消息给客户端
    // std::string response = "hello, client";
    // //获取hdl，然后发送
    // server::connection_ptr con = s->get_con_from_hdl(hdl);
    // con->send(response, websocketpp::frame::opcode::text);

}


