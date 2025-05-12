#include <boost/asio.hpp>
#include <iostream>
#include <vector>
#include <memory>
#include <thread>
#include <algorithm>

using boost::asio::ip::tcp;

class session : public std::enable_shared_from_this<session> {
public:
    session(tcp::socket socket) : socket_(std::move(socket)) {}

    void start() {
        do_read();
    }

private:
    void do_read() {
        auto self(shared_from_this());
        socket_.async_read_some(boost::asio::buffer(data_),
            [this, self](boost::system::error_code ec, std::size_t length) {
                if (!ec) {
                    std::cout << "Received: " << std::string(data_.data(), length) << std::endl;   
                    do_write(length);
                }
            });
    }

    void do_write(std::size_t length) {
        auto self(shared_from_this());
        char * p = R"(HTTP/1.1 200 OK
Content-Type: text/html; charset=UTF-8
Content-Length: 13
Connection: close
Date: Fri, 21 Jun 2024 10:00:00 GMT

Hello, World!)";

        boost::asio::async_write(socket_, boost::asio::buffer(p, strlen(p)),
            [this, self](boost::system::error_code ec, std::size_t /*length*/) {
                if (!ec) {
                    do_read();
                }
            });
    }

    tcp::socket socket_;
    std::array<char, 1024> data_;
};

void start_accept(tcp::acceptor& acceptor, std::vector<std::shared_ptr<boost::asio::io_context>>& workers) {
    auto socket = std::make_shared<tcp::socket>(acceptor.get_executor());
    acceptor.async_accept(*socket, [&, socket](boost::system::error_code ec) mutable {
        if (!ec) {
            // 获取原生句柄并释放当前socket的所有权
            auto native_sock = socket->native_handle();
            socket->release();

            // 选择下一个worker（轮询方式）
            static size_t next = 0;
            auto& worker_io = *workers[next % workers.size()];
            next++;

            // 将任务派发到worker的io_context中
            boost::asio::post(worker_io, [native_sock, &worker_io]() {
                boost::system::error_code ec;
                tcp::socket worker_socket(worker_io);
                worker_socket.assign(tcp::v4(), native_sock, ec);
                if (ec) {
                    std::cerr << "Assign failed: " << ec.message() << std::endl;
#ifdef _WIN32
                    // 在Windows上，需要显式地关闭socket
                    ::closesocket(native_sock);
#else                 
                    ::close(native_sock);
#endif
                    return;
                }

                // 创建会话并启动
                std::make_shared<session>(std::move(worker_socket))->start();
            });
        }

        // 继续接受新连接
        start_accept(acceptor, workers);
    });
}

int main() {
    try {
        const unsigned short port = 8080;
        const int num_workers = 4;

        // 主io_context用于接受连接
        boost::asio::io_context main_io;
        auto main_work = boost::asio::make_work_guard(main_io);
        tcp::acceptor acceptor(main_io, tcp::endpoint(tcp::v4(), port));

        // 创建工作io_context和线程
        std::vector<std::shared_ptr<boost::asio::io_context>> worker_ios;
        std::vector<std::thread> worker_threads;
        std::vector<boost::asio::executor_work_guard<boost::asio::io_context::executor_type>> worker_work;

        for (int i = 0; i < num_workers; ++i) {
            auto io = std::make_shared<boost::asio::io_context>();
            worker_ios.push_back(io);
            worker_work.emplace_back(boost::asio::make_work_guard(*io));
            worker_threads.emplace_back([io] {
                io->run();
                std::cout << "Worker thread stopped" << std::endl;
            });
        }

        // 开始接受连接
        start_accept(acceptor, worker_ios);

        // 运行主io_context（阻塞）
        main_io.run();

        // 停止工作线程（需先停止io_context）
        for (auto& io : worker_ios) {
            io->stop();
        }
        for (auto& t : worker_threads) {
            if (t.joinable()) t.join();
        }
    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
    return 0;
}
