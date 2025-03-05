cmake_minimum_required(VERSION 3.10)

# 设置项目名称
project(PluginManager)

# 设置 C 标准
set(CMAKE_C_STANDARD 11)

# 查找 Lua 库
find_package(Lua REQUIRED)

# 查找 libzip 库
find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBZIP REQUIRED libzip)

# 查找 inih 库
# 如果是手动下载的 inih，可直接包含头文件和源文件路径
# 如果是通过包管理器安装的，使用 pkg_check_modules 查找
option(USE_SYSTEM_INIH "Use system installed inih library" ON)
if(USE_SYSTEM_INIH)
    pkg_check_modules(INI REQUIRED inih)
else()
    # 手动指定 inih 头文件和源文件路径
    set(INI_INCLUDE_DIRS "${CMAKE_SOURCE_DIR}/path/to/inih")
    set(INI_SOURCES "${CMAKE_SOURCE_DIR}/path/to/inih/ini.c")
endif()

# 包含头文件目录
include_directories(
    include
    ${LUA_INCLUDE_DIR}
    ${LIBZIP_INCLUDE_DIRS}
    ${INI_INCLUDE_DIRS}
)

# 添加可执行文件
add_executable(plugin_manager
    src/main.c
    src/plugin_manager.c
    ${INI_SOURCES}
)

# 链接库
target_link_libraries(plugin_manager
    ${LUA_LIBRARIES}
    ${LIBZIP_LIBRARIES}
    ${INI_LIBRARIES}
    pthread
)