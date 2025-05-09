#include<boost/asio.hpp>
#include<iostream>
#include<vector>

auto  main() -> int
{
    using namespace boost::asio;    
    using namespace std;
    io_context io;
    try{
        ip::tcp::socket sock(io);
        sock.connect(ip::tcp::endpoint(ip::make_address("127.0.0.1"), 8888));
        std::vector<char> buf(1024);
        while (true)
        {      
            string str;
            cin>>str;
            sock.write_some(buffer(str));
            auto n = sock.read_some(buffer(buf));
            buf[n] = 0;
            cout << buf.data() << endl;
        }
    }catch(exception &e)
    {
        cerr << e.what() << endl;
    }

    return 0;
}

