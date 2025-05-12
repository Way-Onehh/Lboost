cmake_minimum_required(VERSION 3.12)
project(Backend VERSION 0.0.1)
set(CMAKE_CXX_STANDARD 17)

if(POLICY CMP0048)
  cmake_policy(SET CMP0144 NEW)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug" AND DEFINED ENV{SANITIZER_FLAGS})
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        # GCC/Clang 配置 
        add_compile_options(-fsanitize=$ENV{SANITIZER_FLAGS} -g -O1 -fno-omit-frame-pointer)
        add_link_options(-fsanitize=$ENV{SANITIZER_FLAGS})
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC" AND CMAKE_CXX_COMPILER_FRONTEND_VARIANT MATCHES "Clang")
        # MSVC 的 Clang-cl 模式（需验证支持程度）
        add_compile_options(/fsanitize=$ENV{SANITIZER_FLAGS} /Zi /Od)
        add_link_options(/fsanitize=$ENV{SANITIZER_FLAGS})
    else()
        message(WARNING "not supported for current compiler: ${CMAKE_CXX_COMPILER_ID}")
    endif()
endif()

if(WIN32 OR WIN64)
set(BOOST_ROOT "D:/My_SDK/boost")
find_package(Boost REQUIRED json charconv context) 
add_compile_definitions(_WIN32_WINNT=0x0A00)
else()
find_package(Boost REQUIRED json context)
find_package(OpenSSL REQUIRED)
endif()



add_executable(json json.cpp)
target_link_libraries(json PUBLIC Boost::json Boost::container ) 
target_include_directories(json PUBLIC ${Boost_INCLUDE_DIR})

add_executable(tcpS tcpS.cpp)
target_include_directories(tcpS PUBLIC ${Boost_INCLUDE_DIR})

add_executable(tcpC tcpC.cpp)
target_include_directories(tcpC PUBLIC ${Boost_INCLUDE_DIR})

add_executable(udpS udpS.cpp)
target_include_directories(udpS PUBLIC ${Boost_INCLUDE_DIR})

add_executable(udpC udpC.cpp)
target_include_directories(udpC PUBLIC ${Boost_INCLUDE_DIR})

add_executable(httpC httpC.cpp)
target_link_libraries(httpC PUBLIC Boost::json Boost::container ) 

if(WIN32)
target_include_directories(httpC PUBLIC ${Boost_INCLUDE_DIR} "D:/OpenSSL-Win64/include")
target_link_libraries(httpC PUBLIC "D:/OpenSSL-Win64/lib/VC/x64/MD/libssl_static.lib"  "D:/OpenSSL-Win64/lib/VC/x64/MD/libcrypto_static.lib" crypt32.lib)
else()
target_include_directories(httpC PUBLIC ${Boost_INCLUDE_DIR})
target_link_libraries(httpC PUBLIC ssl crypto)
endif()

add_executable(httpS httpS.cpp)
target_include_directories(httpS PUBLIC ${Boost_INCLUDE_DIR})

add_executable(api api.cpp)
target_include_directories(api PUBLIC ${Boost_INCLUDE_DIR})


add_executable(mysql mysql.cpp) 
if(WIN32)
target_include_directories(mysql PUBLIC ${Boost_INCLUDE_DIR} "D:/OpenSSL-Win64/include")
target_link_libraries(mysql PUBLIC Boost::charconv "D:/OpenSSL-Win64/lib/VC/x64/MD/libssl_static.lib"  "D:/OpenSSL-Win64/lib/VC/x64/MD/libcrypto_static.lib" crypt32.lib)
endif()

add_executable(coroutine coroutine.cpp)
target_include_directories(coroutine PUBLIC ${Boost_INCLUDE_DIR})
target_link_libraries(coroutine PUBLIC Boost::context)

add_executable(asio asio.cpp)
target_include_directories(asio PUBLIC ${Boost_INCLUDE_DIR})

add_executable(asio1 asio1.cpp)
target_include_directories(asio1 PUBLIC ${Boost_INCLUDE_DIR})

add_executable(asio2 asio2.cpp)
target_include_directories(asio2 PUBLIC ${Boost_INCLUDE_DIR})
