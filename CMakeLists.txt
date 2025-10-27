cmake_minimum_required(VERSION 3.28)
project(AgentDemo LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(PkgConfig REQUIRED)
pkg_check_modules(CURL REQUIRED libcurl)
find_package(nlohmann_json REQUIRED)

include_directories(include)

add_executable(ag 
    src/main.cpp
    src/agent.cpp
    src/api_client.cpp
    src/logger.cpp
)

# 链接所有依赖库
target_link_libraries(ag 
    PRIVATE 
    ${CURL_LIBRARIES}
    readline
    nlohmann_json::nlohmann_json
)

# 添加 CURL 头文件目录
target_include_directories(ag PRIVATE ${CURL_INCLUDE_DIRS})

# 可执行文件名称
set_target_properties(ag PROPERTIES OUTPUT_NAME "ag")