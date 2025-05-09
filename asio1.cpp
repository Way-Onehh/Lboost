#include <boost/asio.hpp> 
#include <memory>
#include <thread>
#include <array>
#include <iostream>
 
namespace asio = boost::asio;
using asio::ip::tcp;
 
// 会话类：管理单个连接的生命周期
class Session : public std::enable_shared_from_this<Session> {
public:
    Session(tcp::socket socket) : socket_(std::move(socket)) {}
 
    void start() {
        do_read();
    }
 
private:
    void do_read() {
        auto self(shared_from_this());
        socket_.async_read_some(asio::buffer(data_),
            [this, self](boost::system::error_code ec, std::size_t length) {
                if (!ec) {
                    std::cout << "Received: " << std::string(data_.data(), length) << std::endl;
                    do_write(length);
                }
            });
    }
 
    void do_write(std::size_t length) {
        auto self(shared_from_this());
        asio::async_write(socket_, asio::buffer(data_, length),
            [this, self](boost::system::error_code ec, std::size_t /*length*/) {
                if (!ec) {
                    do_read();
                }
            });
    }
 
    tcp::socket socket_;
    std::array<char, 1024> data_;
};
 
// 服务器类 
class Server {
public:
    Server(asio::io_context& io_context, short port)
        : acceptor_(io_context, tcp::endpoint(tcp::v4(), port)),
          strand_(io_context.get_executor())  {
        do_accept();
    }
 
private:
    void do_accept() {
        acceptor_.async_accept(
            asio::bind_executor(strand_,
                [this](boost::system::error_code ec, tcp::socket socket) {
                    if (!ec) {
                        std::make_shared<Session>(std::move(socket))->start();
                    }
                    do_accept();
                }));
    }
 
    tcp::acceptor acceptor_;
    asio::strand<asio::io_context::executor_type> strand_;
};
 
int main() {
    try {
        asio::io_context io_context;
        
        // 设置工作守卫防止io_context空转退出 
        auto work = asio::make_work_guard(io_context);
        
        // 启动服务器
        Server server(io_context, 8080);
 
        // 信号处理（Ctrl+C优雅退出）
        asio::signal_set signals(io_context, SIGINT, SIGTERM);
        signals.async_wait([&](auto,  auto){ io_context.stop();  });
 
        // 根据CPU核心数创建线程池 
        const unsigned thread_count = std::thread::hardware_concurrency() * 2;
        std::vector<std::thread> threads;
        for (unsigned i = 0; i < thread_count; ++i) {
            threads.emplace_back([&io_context](){  io_context.run();  });
        }
 
        // 等待所有线程完成 
        for (auto& t : threads) {
            if (t.joinable())  t.join(); 
        }
    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what()  << "\n";
    }
 
    return 0;
}