cmake_minimum_required(VERSION 3.0)

project(client)


# include 头文件
include_directories(${PROJECT_SOURCE_DIR}../../websocketpp)
include_directories(${PROJECT_SOURCE_DIR}/include)

# 搜索某目录下的源文件   路径 变量名
aux_source_directory(./src SRC_LIST)

# 添加执行文件   可执行程序名 源文件名称
add_executable(client ${SRC_LIST})

find_package(OpenSSL REQUIRED)

# 链接库
target_link_libraries(client -std=c++11 pthread OpenSSL::SSL OpenSSL::Crypto)

# set(变量名  路径)
# ouput filePath
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
