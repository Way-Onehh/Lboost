/*
    @task  json
    解析 json 
    {  
        "project": {  
            "name": "Boost.JSON Demo",  
            "version": "1.0",  
            "dependencies": ["Boost>=1.80", "CMake>=3.20"],  
            "metadata": {  
            "author": "Your Name",  
            "license": "MIT",  
            "requires_network": false  
            }  
        },  
        "timestamps": {  
            "created": "2025-05-07T00:00:00Z",  
            "updated": "2025-05-07T15:29:00Z"  
        }  
    }  
        任务要求
        解析部分：正确处理嵌套对象、数组及混合数据类型（字符串、布尔值、数字）。
        序列化部分：确保生成的 JSON 符合格式要求，包括嵌套对象和数组的构建。
        时间处理：序列化时的时间字段需符合 ISO 8601 格式（如 2025-05-07T15:29:00Z）。
*/

#include<boost/json.hpp>
#include<vector>
#include<iostream>
#include<ctime>
const char* json_str =   
R"({  
    "project": {  
        "name": "Boost.JSON Demo",  
        "version": "1.0",  
        "dependencies": ["Boost>=1.80", "CMake>=3.20"],  
        "metadata": {  
        "author": "Your Name",  
        "license": "MIT",  
        "requires_network": false  
        }  
    },  
    "timestamps": {  
        "created": "Wed May  7 16:04:23 2025",  
        "updated": "Wed May  7 16:04:23 2025"  
    } 
})";

auto main() -> int 
{
    //声明
    using namespace boost::json;
    using namespace std;
    using string = std::string;
    using array = boost::json::array;
    
    // 0变量
        string name,version;
        vector<string> dependencies;
        object metadata;
    
    // 1解析
        try
        {
            auto json = parse(json_str);
            name=json.at("project").at("name").as_string();
            version=json.at("project").at("version").as_string();
            for(auto e : json.at("project").at("dependencies").as_array())
            {
                dependencies.push_back(string(e.as_string()));
            }
            metadata=json.at("project").at("metadata").as_object();
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
        }
    
    // 2构建
        // 获取系统当前时间点
        auto now = std::chrono::system_clock::now();
        // 转换为time_t（秒级精度）
        std::time_t now_time = std::chrono::system_clock::to_time_t(now);
        char buffer[26];
        
        //线程安全ctime
        #ifdef _WIN32 
            ctime_s(buffer, sizeof(buffer), &now_time); // Windows 
        #else 
            ctime_r(&now_time, buffer); // Linux/macOS 
        #endif 
        
        object obj;
        obj["name"]=name;
        obj["version"]=version; 
        obj["dependencies"]=array(dependencies.begin(),dependencies.end());//迭代器简化
        obj["metadata"]=metadata;
        obj["timestamps"]=object{  
            {"created", "Wed May  7 16:04:23 2025"},  
            {"updated", buffer}  
        };

    // 3序列化
        string str=serialize(obj);
        cout<<str<<endl;
    return 0;
}