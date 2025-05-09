#include<boost/asio.hpp>
#include<functional>
#include<iostream>
#include<memory>
#include<vector>
namespace asio = boost::asio;
int main()
{
    asio::io_context io;
    auto work = asio::make_work_guard(io); // 保持io运行 
    asio::ip::tcp::acceptor acceptor(io, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), 8080));
    acceptor.listen();
    std::function<void(boost::system::error_code,asio::ip::tcp::socket)> handle_accept = [&](boost::system::error_code ec, asio::ip::tcp::socket socket)
    {
        std::function<void(boost::system::error_code,size_t )> do_read,do_write;
        auto socket_ptr = std::make_shared<decltype(socket)>(std::move(socket));
        auto buf_ptr = std::make_shared<std::vector<char>>(1024);
                do_write = [&,socket_ptr](boost::system::error_code ec,size_t size)
        {
            if(!ec)
            {
                socket_ptr->close();
            }
        }; 

        
        do_read = [&,socket_ptr,buf_ptr,do_write](boost::system::error_code ec,size_t size)
        {
            if(!ec)
            {
                std::cout<<"read:"<<size<<": "<<buf_ptr->data()<<std::endl;
                char *p = "hello world";
                memcpy(buf_ptr->data(),p,strlen(p));
                socket_ptr->async_write_some(asio::buffer(*buf_ptr,strlen(p)),do_write);
            }
        };
        socket_ptr->async_read_some(asio::buffer(*buf_ptr),do_read);
        acceptor.async_accept(handle_accept);
    };
    // 异步接受连接 
    asio::ip::tcp::socket socket(io);
    acceptor.async_accept(handle_accept);
    io.run();
    return 0;
}