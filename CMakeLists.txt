cmake_minimum_required(VERSION 3.10)
project(OpenAIChatbot)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


find_package(PkgConfig REQUIRED)
pkg_check_modules(JSONCPP jsoncpp)


find_package(CURL REQUIRED)


add_executable(chatbot main.cpp)


target_link_libraries(chatbot ${CURL_LIBRARIES} ${JSONCPP_LIBRARIES})
target_include_directories(chatbot PRIVATE ${CURL_INCLUDE_DIRS} ${JSONCPP_INCLUDE_DIRS})
target_compile_options(chatbot PRIVATE ${JSONCPP_CFLAGS_OTHER})
