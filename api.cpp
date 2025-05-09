#include <iostream>
#include <boost/beast.hpp>
#include <boost/asio.hpp>
#include <boost/system.hpp>
#include <unordered_map>
#include <functional>

namespace beast = boost::beast;
namespace net = boost::asio;
using request = beast::http::request<beast::http::string_body>;
using response = beast::http::response<beast::http::string_body>;
using map = std::unordered_map<std::string,std::function<response(const request&)> >;

net::io_context ioc;
net::ip::tcp::acceptor acceptor(ioc, net::ip::tcp::endpoint(net::ip::tcp::v4(), 8080));
map routing;
void handle_accept(boost::system::error_code  ec, net::ip::tcp::socket socket) {
    if (!ec) {
        try{
            beast::flat_buffer buf;
            request req;
            beast::http::read(socket,buf,req);
            auto target = req.target();
            auto pos = target.find('?');
            response res =  routing[pos == std::string::npos ? target : target.substr(0,pos)](req); 
            beast::http::write(socket,res);  
        }catch(std::exception &e)
        {
            std::cout << e.what() << std::endl;
            response res;
            res.result(beast::http::status::bad_request);
            beast::http::write(socket,res);  
        }
    }else{
        std::cout << ec.message() << std::endl;
    }
    socket.close();
    acceptor.async_accept(handle_accept);
}

auto main() -> int {
    acceptor.listen();
    acceptor.async_accept(handle_accept);
    routing["/"] = [](const request& req) -> response {
        response res{beast::http::status::ok,req.version()};
        res.set(beast::http::field::content_type,"text/plain");
        res.body() = "Hello World!"; 
        res.content_length(res.body().size()); 
        return  res;
    };
    ioc.run();
    return 0;
}
