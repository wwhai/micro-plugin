cmake_minimum_required(VERSION 3.10)
project(plugin_manager)

# 设置C语言标准
set(CMAKE_C_STANDARD 11)

# 包含头文件目录
include_directories(include)

# 添加可执行文件
add_executable(test_plugin_manager
    src/plugin_utils.c
    src/plugin_manager.c
    tests/test_plugin_manager.c
)

# 查找Lua库
find_package(Lua REQUIRED)
# 包含头文件目录
include_directories(
    ${LUA_INCLUDE_DIR}
    ${ZLIB_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}
)
# 链接库
target_link_libraries(test_plugin_manager
    ${LUA_LIBRARIES}
    ${ZLIB_LIBRARIES}
    pthread
)