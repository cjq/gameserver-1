cmake_minimum_required(VERSION 3.5)
project(tf_httpserver)

set(CMAKE_CXX_STANDARD 11)

SET(SRC_LIST main.cpp
        tf_kernel/xxtea.cpp
        tf_kernel/tf_http.cpp
        tf_kernel/md5.cpp
        tf_kernel/base64.cpp
        )
add_executable(tf_httpserver ${SRC_LIST})

SET(LIBS jsoncpp
        evpp_static
        glog
        event
        pthread)

target_link_libraries(tf_httpserver ${LIBS})