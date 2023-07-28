#include "WebsocketClient.h"

WebsocketClient::WebsocketClient()
{
	m_WebsocketClient.clear_access_channels(websocketpp::log::alevel::all);  // 开启全部接入日志级别
	m_WebsocketClient.clear_error_channels(websocketpp::log::elevel::all);   // 开启全部错误日志级别

	m_WebsocketClient.init_asio();       // 初始化asio
	m_WebsocketClient.start_perpetual(); // 避免请求为空时退出，实际上，也是避免asio退出

	// 独立运行client::run()的线程，主要是避免阻塞
	m_Thread = websocketpp::lib::make_shared<websocketpp::lib::thread>(&client::run, &m_WebsocketClient);
}

WebsocketClient::~WebsocketClient()
{
	m_WebsocketClient.stop_perpetual();

	if (m_ConnectionMetadataPtr != nullptr && m_ConnectionMetadataPtr->get_status() == "Open")
	{
		websocketpp::lib::error_code ec;
		m_WebsocketClient.close(m_ConnectionMetadataPtr->get_hdl(), websocketpp::close::status::going_away, "", ec); // 关闭连接

		if (ec) {
			std::cout << "> Error initiating close: " << ec.message() << std::endl;
		}
	}

	m_Thread->join();
}

bool WebsocketClient::Connect(std::string const & url)
{
	websocketpp::lib::error_code ec;

	// 创建connect的共享指针，注意，此时创建并没有实际建立
	client::connection_ptr con = m_WebsocketClient.get_connection(url, ec);  
    con->append_header("Room-ID", "zhihuishu");

	if (ec) {
		std::cout << "> Connect initialization error: " << ec.message() << std::endl;
		return false;
	}

	// 创建连接的metadata信息，并保存
	connection_metadata::ptr metadata_ptr = websocketpp::lib::make_shared<connection_metadata>(con->get_handle(), url);
	m_ConnectionMetadataPtr = metadata_ptr;

	// 注册连接打开的Handler
	con->set_open_handler(websocketpp::lib::bind(
		&connection_metadata::on_open,
		metadata_ptr,
		&m_WebsocketClient,
		websocketpp::lib::placeholders::_1
	));

	// 注册连接失败的Handler
	con->set_fail_handler(websocketpp::lib::bind(
		&connection_metadata::on_fail,
		metadata_ptr,
		&m_WebsocketClient,
		websocketpp::lib::placeholders::_1
	));

	// 注册连接关闭的Handler
	con->set_close_handler(websocketpp::lib::bind(
		&connection_metadata::on_close,       //方法
		metadata_ptr,                         //对象
		&m_WebsocketClient,                   //参数1   参数2  ...
		websocketpp::lib::placeholders::_1    //参数匹配问题，不是库里面的东西，需要自己给出
	));

	// 注册连接接收消息的Handler
	con->set_message_handler(websocketpp::lib::bind(
		&connection_metadata::on_message,
		metadata_ptr,
		websocketpp::lib::placeholders::_1,
		websocketpp::lib::placeholders::_2
	));

	// 进行连接
	m_WebsocketClient.connect(con);

	std::cout << "Websocket连接成功" << std::endl;

    // 注意，不能在Websocket连接完成之后马上就发送消息，不然会出现Invalid State的错误，
    // 导致消息发送不成功，所以在连接成功之后，主线程休眠1秒
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

	return true;
}

bool WebsocketClient::Close(std::string reason)
{
	websocketpp::lib::error_code ec;

	if (m_ConnectionMetadataPtr != nullptr)
	{
		int close_code = websocketpp::close::status::normal;
		// 关闭连接
		m_WebsocketClient.close(m_ConnectionMetadataPtr->get_hdl(), close_code, reason, ec);
		if (ec) {
			std::cout << "> Error initiating close: " << ec.message() << std::endl;
			return false;
		}
		std::cout << "关闭Websocket连接成功" << std::endl;
	}
    Reconnected();
	return true;
}

bool WebsocketClient::Send(std::string message)
{
	websocketpp::lib::error_code ec;

	if (m_ConnectionMetadataPtr != nullptr)
	{
		// 连接发送数据
		m_WebsocketClient.send(m_ConnectionMetadataPtr->get_hdl(), ansi_to_utf8(message), websocketpp::frame::opcode::text, ec);
		if (ec) {
			std::cout << "> Error sending message: " << ec.message() << std::endl;
			return false;
		}
		std::cout << "发送数据成功" << std::endl;
	}

	return true;
}

void WebsocketClient::Reconnected()
{
    std::cout << "Reconnecting..." << std::endl;
    // 等待一段时间（例如5秒）再重新连接，可以根据实际情况进行调整
    std::this_thread::sleep_for(std::chrono::seconds(5)); //防止多个客户端同时发起重连

    bool flag = Connect(m_ConnectionMetadataPtr->get_url());
    if(!flag){
        std::cout << "reconnect is error" << std::endl;
        return;
    }
    std::cout << "connect is success" << std::endl;
}

connection_metadata::ptr WebsocketClient::GetConnectionMetadataPtr()
{
	return m_ConnectionMetadataPtr;
}


