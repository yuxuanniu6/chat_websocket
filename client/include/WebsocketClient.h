#ifndef WEBSOCKET_CLIENT_H
#define WEBSOCKET_CLIENT_H

// 不包含TLS Client
#include <websocketpp/config/asio_no_tls_client.hpp>
#include <websocketpp/client.hpp>

// 包含TLS Client
// #include <websocketpp/config/asio_client.hpp>
// #include <websocketpp/client.hpp>

#include <websocketpp/common/thread.hpp>
#include <websocketpp/common/memory.hpp>


#include <string>
#include <locale>
#include <codecvt>

typedef websocketpp::client<websocketpp::config::asio_client> client;

static std::wstring string_to_wstring(const std::string &s)
{
  std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
  return converter.from_bytes(s);
	//using default_convert = std::codecvt<wchar_t, char, std::mbstate_t>;
	//static std::wstring_convert<default_convert>conv(new default_convert("CHS"));
	//return conv.from_bytes(s);
}
static std::string wstring_to_string(const std::wstring &s)
{
  std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
  return converter.to_bytes(s);
	//using default_convert = std::codecvt<wchar_t, char, std::mbstate_t>;
	//static std::wstring_convert<default_convert>conv(new default_convert("CHS"));
	//return conv.to_bytes(s);
}
static std::string ansi_to_utf8(const std::string &s)
{
	static std::wstring_convert<std::codecvt_utf8<wchar_t> > conv;
	return conv.to_bytes(string_to_wstring(s));
}
static std::string utf8_to_ansi(const std::string& s)
{
	static std::wstring_convert<std::codecvt_utf8<wchar_t> > conv;
	return wstring_to_string(conv.from_bytes(s));
}


// 保存一个连接的metadata
class connection_metadata {
public:
	typedef websocketpp::lib::shared_ptr<connection_metadata> ptr;

	connection_metadata(websocketpp::connection_hdl hdl, std::string url)
		: m_Hdl(hdl)
		, m_Status("Connecting")
		, m_Url(url)
		, m_Server("N/A")
	{}

	void on_open(client * c, websocketpp::connection_hdl hdl) 
	{
		m_Status = "Open";

		client::connection_ptr con = c->get_con_from_hdl(hdl);
		m_Server = con->get_response_header("Server");

	}

	void on_fail(client * c, websocketpp::connection_hdl hdl)
	{
		m_Status = "Failed";

		client::connection_ptr con = c->get_con_from_hdl(hdl);
		m_Server = con->get_response_header("Server");
		m_Error_reason = con->get_ec().message();
	}

	void on_close(client * c, websocketpp::connection_hdl hdl)
	{
		m_Status = "Closed";
		client::connection_ptr con = c->get_con_from_hdl(hdl);
		std::stringstream s;
		s << "close code: " << con->get_remote_close_code() << " ("
			<< websocketpp::close::status::get_string(con->get_remote_close_code())
			<< "), close reason: " << con->get_remote_close_reason();
		m_Error_reason = s.str();

	}

	void on_message(websocketpp::connection_hdl, client::message_ptr msg)
	{
		if (msg->get_opcode() == websocketpp::frame::opcode::text) 
		{
			std::string message = utf8_to_ansi(msg->get_payload());
			std::cout << "收到来自服务器的消息：" << message << std::endl;
		}
		else 
		{
			std::string message = websocketpp::utility::to_hex(msg->get_payload());
		}
	}


	websocketpp::connection_hdl get_hdl() const
	{
		return m_Hdl;
	}

	std::string get_status() const
	{
		return m_Status;
	}

    std::string get_url() const
    {
        return m_Url;
    }
private:
	websocketpp::connection_hdl m_Hdl;  // websocketpp表示连接的编号
	std::string m_Status;               // 连接自动状态
	std::string m_Url;                  // 连接的URI
	std::string m_Server;               // 服务器信息
	std::string m_Error_reason;         // 错误原因
};


class WebsocketClient
{
public:
	WebsocketClient();
	virtual~WebsocketClient();

public:
	bool Connect(std::string const & url);
	bool Close(std::string reason = "");
	bool Send(std::string message);
    void Reconnected();

	connection_metadata::ptr GetConnectionMetadataPtr();

private:
	connection_metadata::ptr m_ConnectionMetadataPtr;  //链接信息
	client m_WebsocketClient;
	websocketpp::lib::shared_ptr<websocketpp::lib::thread> m_Thread; // 线程
};


#endif // !WEBSOCKET_ENDPOINT_H


