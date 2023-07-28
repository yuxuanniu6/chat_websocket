cmake_minimum_required(VERSION 3.0)

project(server)


# include 头文件
include_directories(${PROJECT_SOURCE_DIR}../../websocketpp)
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/usr/include/jsoncpp)

# 搜索某目录下的源文件   路径 变量名
aux_source_directory(src SRC_LIST)

# 添加执行文件   可执行程序名 源文件名称
add_executable(server ${SRC_LIST})

# 链接库
target_link_libraries(server -std=c++11 pthread jsoncpp)

# set(变量名  路径)
# ouput filePath
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
