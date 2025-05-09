#include<boost/asio.hpp>
#include<iostream>
#include<vector>
auto main() -> int
{
    using namespace boost::asio;
    using namespace std;    
    // tcp 服务器 = io + pool + acceptor + socket
    io_context io;
    thread_pool pool(12);
    ip::tcp::acceptor acceptor(io, ip::tcp::endpoint(ip::tcp::v4(),8888));
    acceptor.listen();

    // tcp 分发客户端
    while (true)
    {
        auto client = acceptor.accept(); 
        post(pool, [client = std::move(client)]() mutable { // 值捕获（移动构造）
            try {// 捕获异常  
                //char buf[1024];//有溢出风险
                vector<char> buf(1024);
                while (true) {
                    auto n = client.read_some(buffer(buf));
                    auto endpoint = client.remote_endpoint();
                    buf[n] = 0;//c字符串 来输出
                    cout<< endpoint.address().to_string() << ":" << endpoint.port() << ":" << n <<":" << static_cast<char *>(buf.data())<<endl;
                    string respone;
                    cin >> respone;
                    client.write_some(buffer(respone));
                }
            }
            catch (...) {
                // 处理异常
                //线程出口
            }
        });
    }

    pool.join();
    return 0;
}