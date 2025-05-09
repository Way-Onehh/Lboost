#include <iostream>
#include <boost/asio.hpp> 
#include <vector>
using namespace boost::asio;
using namespace std;
int main() {
    io_context io;
    try{
        ip::udp::socket sock(io,ip::udp::endpoint(ip::udp::v4(), 8888)); 
        for(;;)
        {
            vector<char> buf(1024);
            ip::udp::endpoint client_ep; // 关键：存储客户端地址 
            auto n = sock.receive_from(buffer(buf),client_ep);
            cout << buf.data() << endl;
            sock.send_to(buffer(buf,n),client_ep);
        }
    }catch(exception e){cout<<e.what()<<endl;}

    return 0;
}