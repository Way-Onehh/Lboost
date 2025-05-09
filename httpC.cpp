#include <boost/beast.hpp> 
#include <boost/asio/ssl.hpp> 
#include <iostream>
#include <fstream>
#include <boost/json.hpp>

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
namespace ssl = net::ssl;
namespace json = boost::json;
using tcp = net::ip::tcp;

auto main()->int {
    try {
        // 网络层初始化 io_context  ssl::context 初始化
        net::io_context ioc;
        ssl::context ctx(ssl::context::tlsv12_client);
        ctx.set_default_verify_paths();  // 加载系统CA证书 
 
        // 创建SSL套接字
        tcp::resolver resolver(ioc);
        net::ssl::stream<beast::tcp_stream> stream(ioc, ctx);
        auto const results = resolver.resolve("api.deepseek.com",  "443");
        beast::get_lowest_layer(stream).connect(results);
        stream.handshake(ssl::stream_base::client); 
 
        // 构造HTTP请求 
        std::string json_str;
#ifdef _WIN32
        std::ifstream file("../../config.json", std::ios::binary);
#else
        std::ifstream file("../config.json");
#endif
        if (!file.is_open())  {
            std::cerr << "无法打开文件" << std::endl;
            return 1;
        }
		std::string line;
		while (std::getline(file, line)) {  // 逐行读取 
            json_str+=line;
        }
        file.close(); 

        json::value config = json::parse(json_str);
        std::string key = "Bearer ";
        key += config.at("deepseek").as_string();
        http::request<http::string_body> req{
            http::verb::post, 
            "/chat/completions", 
            11 
        };
        req.set(http::field::host,  "api.deepseek.com"); 
        req.set(http::field::authorization,  key);
        req.set(http::field::content_type,  "application/json");
        req.body()  = R"({
            "model": "deepseek-chat",
            "messages": [{"role": "user", "content": "Hello!"}],
            "stream": false 
        })";
        req.prepare_payload(); 
        
        // 发送请求并读取响应 
        http::write(stream, req);
        beast::flat_buffer buffer;
        http::response<http::dynamic_body> res;
        http::read(stream, buffer, res);
 
        // 打印状态行 
        std::cout << "HTTP/" << res.version()  << " "
        << res.result_int()  << " " << res.reason()  << "\n";
        
        // 打印头部 
        for(const auto& header : res.base())
        std::cout << header.name_string()  << ": " << header.value()  << "\n";
        // 打印空行分隔头部和正文 
        std::cout << "\n";

        // 打印正文 
        std::cout <<beast::make_printable(res.body().data())<< std::endl;
    } catch (std::exception const& e) {
        std::cerr << "Error: " << e.what()  << std::endl;
        return EXIT_FAILURE;
    }
    return 0;
}
