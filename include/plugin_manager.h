// Copyright (C) 2025 wwhai
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as
// published by the Free Software Foundation, either version 3 of the
// License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Affero General Public License for more details.
//
// You should have received a copy of the GNU Affero General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#ifndef PLUGIN_MANAGER_H
#define PLUGIN_MANAGER_H

#include <lua.h>
#include <pthread.h>
// 定义插件状态枚举
typedef enum
{
    PLUGIN_UNLOADED,
    PLUGIN_LOADED,
    PLUGIN_RUNNING
} PluginState;

// 定义插件结构体
typedef struct Plugin
{
    char *name;
    char *version;
    char *author;
    char *description;
    lua_State *L;
    int resource_ref;
    PluginState state;
    struct Plugin *next;
} Plugin;

// 定义插件管理器结构体
typedef struct
{
    Plugin *plugins;
    pthread_mutex_t lock;
} PluginManager;

// 初始化插件管理器
void plugin_manager_init(PluginManager *manager);

// 释放插件管理器
void plugin_manager_destroy(PluginManager *manager);

// 加载插件
Plugin *plugin_manager_load(PluginManager *manager, const char *zip_filename);

// 卸载插件
int plugin_manager_unload(PluginManager *manager, const char *name);

// 执行插件的主逻辑
int plugin_manager_execute(PluginManager *manager, const char *name);

// 查询插件状态
PluginState plugin_manager_query_state(PluginManager *manager, const char *name);

#endif // PLUGIN_MANAGER_H