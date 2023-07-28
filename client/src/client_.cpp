#include <iostream>
#include <websocketpp/client.hpp>
#include <websocketpp/config/asio_no_tls_client.hpp>

typedef websocketpp::client<websocketpp::config::asio_client> client;

class WebSocketClient {
public:
    WebSocketClient() {
        client_.set_close_handler([this](websocketpp::connection_hdl hdl) {
            std::cout << "Disconnected from server" << std::endl;
            reconnect();
        });
    }

    void connect(const std::string& uri) {
        websocketpp::lib::error_code ec;
        client::connection_ptr con = client_.get_connection(uri, ec);
        if (ec) {
            std::cout << "Connect initialization error: " << ec.message() << std::endl;
            return;
        }
        client_.connect(con);
        client_.run();
    }

    void reconnect() {
        while (true) {
            std::this_thread::sleep_for(std::chrono::seconds(5)); // 5秒后尝试重新连接

            std::cout << "Attempting to reconnect..." << std::endl;
            try {
                client_.reset(); // 重置客户端
                connect("ws://123.249.1.203:8081");
                break; // 重新连接成功，退出重连循环
            } catch (websocketpp::exception const& e) {
                std::cout << "Reconnect error: " << e.what() << std::endl;
            }
        }
    }

private:
    client client_;
};

int main() {
    WebSocketClient client;
    client.connect("ws://123.249.1.203:8081");

    return 0;
}
