// 不使用ssl加密
#include <iostream>
#include <boost/beast.hpp>
#include <boost/asio.hpp>
namespace beast = boost::beast;
namespace net = boost::asio;
auto main() -> int
{
    net::io_context ioc;
    net::ip::tcp::acceptor acceptor{ioc, {boost::asio::ip::tcp::v4(), 8080}};
    net::ip::tcp::socket socket{ioc};
    acceptor.accept(socket);
    beast::http::request<boost::beast::http::dynamic_body> req;
    beast::flat_buffer buf;
    beast::http::read(socket,buf,req);
    std::cout << req.method_string() << " " << req.target() << " HTTP/" << req.version() << std::endl;
    for(auto & header : req.base())
    std::cout << header.name() << ": " << header.value() << std::endl;
    std::cout << std::endl;
    std::cout << beast::make_printable(req.body().data()) << std::endl;
    beast::http::response<boost::beast::http::string_body> res{beast::http::status::ok, req.version()};
    beast::http::write(socket,res);
    socket.close();
}