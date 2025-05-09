#define BOOST_MYSQL_SEPARATE_COMPILATION


#include<boost/mysql/src.hpp>
#include<boost/asio.hpp>
#include<iostream>
namespace mysql = boost::mysql;
namespace asio = boost::asio;
auto main() -> int
{
    // The execution context, required to run I/O operations.
    asio::io_context ctx;

    // Represents a connection to the MySQL server.
    mysql::any_connection conn(ctx);
  
    // The hostname, username and password to use
    mysql::connect_params params;
    params.server_address.emplace_host_and_port("aliyun");
    params.username = "wr";
    params.password = "6014wrwr";

    // Connect to the server
    conn.connect(params);
    
    // Issue the SQL query to the server
    const char* sql = "SELECT 'Hello world!'";
    mysql::results result;
    conn.execute(sql, result);

    // Print the first field in the first row
    std::cout << result.rows().at(0).at(0) << std::endl;

    // Close the connection
    conn.close();
    return 0;
}