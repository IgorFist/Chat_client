cmake_minimum_required(VERSION 3.16)

project(chat_client)

find_package(Boost COMPONENTS system thread REQUIRED)

include_directories( ./include)

add_executable(chat_client ./src/main.cpp ./src/client.cpp)

set_target_properties(chat_client PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

target_link_libraries(chat_client 
    ${Boost_LIBRARIES}
)