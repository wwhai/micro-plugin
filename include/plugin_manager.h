#ifndef PLUGIN_MANAGER_H
#define PLUGIN_MANAGER_H

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <zip.h>

// 插件状态枚举
typedef enum
{
    PLUGIN_UNLOADED,
    PLUGIN_LOADED,
    PLUGIN_RUNNING
} PluginStatus;

// 插件结构体
typedef struct
{
    char *name;
    char *zip_path;
    lua_State *L;
    PluginStatus status;
} Plugin;

// 插件管理器结构体
typedef struct
{
    Plugin *plugins;
    int plugin_count;
} PluginManager;

// 初始化插件管理器
PluginManager *plugin_manager_init();

// 加载插件
int plugin_load(PluginManager *manager, const char *zip_path);

// 执行插件
int plugin_execute(PluginManager *manager, int plugin_index);

// 卸载插件
int plugin_unload(PluginManager *manager, int plugin_index);

// 释放插件管理器
void plugin_manager_free(PluginManager *manager);

#endif