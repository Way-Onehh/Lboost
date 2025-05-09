//创立一对一的io_context和线程，然后使用io_context::run() 轮询处理事件。
//使用一个io_context和一个线程，然后使用io_context来处理网络连接和数据交换。
//使用post(io_context xxx) 轮询投递事件

#include <boost/asio.hpp> 
#include <thread>
#include <vector>
#include <memory>
#include <iostream>
#include <atomic>
#include <functional>
namespace asio = boost::asio;
int main() {
    // 初始化：根据CPU核心数创建io_context和线程 
    int num_thread = std::thread::hardware_concurrency() * 2;
    std::vector<asio::io_context> io_contexts(num_thread);
    std::vector<std::thread> threads(num_thread);
    std::vector<boost::asio::executor_work_guard<boost::asio::io_context::executor_type>> works;
    std::atomic<size_t> current_io_index{0}; // 轮询计数器 

    for (int i = 0; i < num_thread; ++i) {
        works.push_back(asio::make_work_guard(io_contexts[i]));
        threads[i] = std::thread([&io_contexts, i]() {
            io_contexts[i].run();
        });
    }
    asio::ip::tcp::acceptor acceptor(io_contexts[0], asio::ip::tcp::endpoint(asio::ip::tcp::v4(), 8080));
    acceptor.listen();

    std::function<void (const boost::system::error_code&,asio::ip::tcp::socket)>handle_accept 
    = [&](const boost::system::error_code& error,asio::ip::tcp::socket socket) {
        int index = current_io_index % num_thread;
        auto socket_s = std::make_shared<asio::ip::tcp::socket>(std::move(socket)); 
        char buf[1024]={0};
        std::function<void (const boost::system::error_code&,size_t )>do_read,do_write;
        do_write = [&,socket_s](const boost::system::error_code& error,size_t length){
            socket_s->close();
        };

        do_read = [&,socket_s,buf](const boost::system::error_code& error ,size_t length) {
            std::cout << "Received: " << buf << std::endl;
            char *p = "hello world";
            memcpy((void*) buf,p,strlen(p));
            asio::async_write(
                *socket_s,
                asio::buffer(buf,strlen(p)),
                do_write
            );
        };

        // asio::post(io_contexts[index],
        //     [socket_s,buf,do_read]() {
        //         socket_s->async_read_some(
        //             asio::buffer(buf,1024),
        //             do_read
        //         );
        //     }
        // );
        socket_s->async_read_some(
            asio::buffer(buf,1024),
            do_read
        );
        current_io_index++;
        acceptor.async_accept(handle_accept);
    };
    acceptor.async_accept(handle_accept);

    for (int i = 0; i < num_thread; ++i) {
        threads[i].join();
        works[i].reset();
    }
       
    return 0;
}