#include <iostream>
#include <boost/asio.hpp> 
#include <vector>
using namespace boost::asio;
using namespace std;
int main() {
    io_context io;
    
    try{
        ip::udp::socket sock(io,ip::udp::endpoint(ip::udp::v4(),0)); 
        for(;;)
        {
            string buf;
            cin>>buf;
            sock.send_to(buffer(buf),ip::udp::endpoint(ip::make_address("127.0.0.1"),8888));
            sock.receive(buffer(buf));
            cout<<buf<<endl;    
        }
    }catch(exception e){cout<<e.what()<<endl;}

    return 0;
}