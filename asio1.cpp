//一个 io_context 对应 多个线程
#include <boost/asio.hpp> 
#include <thread>
#include <iostream>
int main() {
    boost::asio::io_context io;
    auto strand = boost::asio::make_strand(io);
    auto work = boost::asio::make_work_guard(io); // 保持事件循环 
    //线程池是消费任务队列这里是每个线程都执行    
    std::thread t([&] { io.run();  });
    std::thread t1([&] { io.run(); });
    // 提交到 strand 的任务保证顺序执行 
    boost::asio::post(strand, [] { 
        std::cout << "Strand task 1\n"; 
    });
    boost::asio::post(strand, [] { 
        std::cout << "Strand task 2\n"; 
    });
 
    work.reset();  // 允许退出 
    t.join(); 
    t1.join();
    return 0;
}