cmake_minimum_required(VERSION 3.12)
project(Backend VERSION 0.0.1)
set(CMAKE_CXX_STANDARD 17)

if(POLICY CMP0048)
  cmake_policy(SET CMP0144 NEW)
endif()

if(WIN32)
set(BOOST_ROOT "D:/My_SDK/boost")
find_package(Boost REQUIRED json) 
else()
find_package(Boost REQUIRED json)
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
target_include_directories(httpC PUBLIC ${Boost_INCLUDE_DIR} "D:/OpenSSL-Win64/include")
target_link_libraries(httpC PUBLIC Boost::json Boost::container ) 
if(WIN32)
target_link_libraries(httpC PUBLIC "D:/OpenSSL-Win64/lib/VC/x64/MD/libssl_static.lib"  "D:/OpenSSL-Win64/lib/VC/x64/MD/libcrypto_static.lib" crypt32.lib)
else()
target_link_libraries(httpC PUBLIC ssl crypto)
endif()

add_executable(httpS httpS.cpp)
target_include_directories(httpS PUBLIC ${Boost_INCLUDE_DIR})

add_executable(api api.cpp)
target_include_directories(api PUBLIC ${Boost_INCLUDE_DIR})




