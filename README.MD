                                redis                    mysql
tcp服务
    文件上传下载
udp服务
    ？？
http服务
    文件服务 get post方法
mqtt服务

json yaml


tcp 定时 超时（心跳）

udp 无连接
    除了上面的以为还有是否接收socket
    receive
    receive_from
    send_to
    可以实现广播组播单播
    音视频 游戏 DNS


HTTP请求报文的具体形式
GET /index.html  HTTP/1.1   
Host: www.example.com  
User-Agent: Mozilla/5.0 
Accept: text/html 
Accept-Language: en-US 
Connection: keep-alive 

    http::request<http::string_body> req{
        http::verb::post, 
        "/chat/completions", 
        11 
    };
or  拼请求行
    req.method() = http::verb::post;
    req.version() = 11;
    req.target() = "/chat/completions";
拼请求头
req.set(http::field::host, "api.openai.com");
....
拼body
req.body() = "{\"model\": \"gpt-3.5-turbo\", \"messages\": [{\"role\": \"user\", \"content\": \"Hello!\"}]}";
req.prepare_payload(); 


HTTP回复报文的具体形式
HTTP/1.1 200 OK 
Date: Thu, 08 May 2025 09:37:55 GMT 
Server: Apache/2.4.6 
Content-Type: text/html; charset=UTF-8 
Content-Length: 1234 
Connection: close 

拼响应行
    beast::http::response<boost::beast::http::string_body> res{beast::http::status::ok, req.version()};
or
    res.result(beast::http::status::ok); // 设置响应状态 request.version()
    res.set...
    res.body() = "Hello, World!";