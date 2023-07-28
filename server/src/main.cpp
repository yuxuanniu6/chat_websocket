#include "server.h"

//工作线程函数，现在替换为类部run函数
//void worker_thread(server *s)
//{
//  s->run();
//}
int main(int argc, char* argv[])
{
    uint16_t port = 8082;
    if (argc == 1) {
        std::cout << "Usage: server  [port]" << std::endl;
        return 1;
    }

    if (argc >= 2) {
        int i = atoi(argv[1]);
        if (i <= 0 || i > 65535) {
            std::cout << "invalid port" << std::endl;
            return 1;
        }
        port = uint16_t(i);
    }
    Server s(port);//创建服务器对象
//    std::cout << "port is " << port << std::endl;
//    s.run(port);
    // 开辟多个线程
    std::vector<std::thread> threads;
    int num_threads = 4;
    for(int i = 0; i < num_threads; ++i)
    {
        threads.emplace_back(&Server::run, &s);
    }
    // Start the ASIO io_service run loop
    //echo_server.run();
    for(auto& thread : threads)
    {
        thread.join();
    }

    return 0;
}