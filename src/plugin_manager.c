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


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#include <pthread.h>
#include <zlib.h>
#include "plugin_manager.h"
#include "plugin_utils.h"

// 初始化插件管理器
void plugin_manager_init(PluginManager *manager)
{
    manager->plugins = NULL;
    pthread_mutex_init(&manager->lock, NULL);
}

// 释放插件管理器
void plugin_manager_destroy(PluginManager *manager)
{
    pthread_mutex_lock(&manager->lock);
    Plugin *current = manager->plugins;
    while (current != NULL)
    {
        Plugin *next = current->next;
        free(current->name);
        free(current->version);
        free(current->author);
        free(current->description);
        lua_close(current->L);
        free(current);
        current = next;
    }
    pthread_mutex_unlock(&manager->lock);
    pthread_mutex_destroy(&manager->lock);
}

// 加载插件
Plugin *plugin_manager_load(PluginManager *manager, const char *zip_filename)
{
    pthread_mutex_lock(&manager->lock);

    // 从 ZIP 文件中读取存储的 MD5 校验码
    char *stored_md5 = read_file_from_zip(zip_filename, "md5.txt");
    if (stored_md5 == NULL)
    {
        fprintf(stderr, "Failed to read MD5 checksum from ZIP\n");
        pthread_mutex_unlock(&manager->lock);
        return NULL;
    }

    // 从 ZIP 文件中提取 Lua 脚本到临时文件
    const char *lua_script_filename = "app-demo.lua";
    extract_file_from_zip(zip_filename, lua_script_filename, lua_script_filename);

    // 计算 Lua 脚本的实际 MD5 校验码
    char *actual_md5 = calculate_md5(lua_script_filename);
    if (actual_md5 == NULL)
    {
        fprintf(stderr, "Failed to calculate MD5 checksum for Lua script\n");
        free(stored_md5);
        pthread_mutex_unlock(&manager->lock);
        return NULL;
    }

    // 比较校验码
    if (strcmp(stored_md5, actual_md5) != 0)
    {
        fprintf(stderr, "MD5 checksum mismatch: Lua script may have been tampered with\n");
        free(stored_md5);
        free(actual_md5);
        pthread_mutex_unlock(&manager->lock);
        return NULL;
    }

    free(stored_md5);
    free(actual_md5);

    // 创建新的 Lua 状态
    lua_State *L = luaL_newstate();
    if (L == NULL)
    {
        fprintf(stderr, "Failed to create Lua state\n");
        pthread_mutex_unlock(&manager->lock);
        return NULL;
    }

    // 打开标准库
    luaL_openlibs(L);

    // 从 ZIP 文件中读取 INI 配置文件
    char *ini_content = read_file_from_zip(zip_filename, "env.ini");
    if (ini_content == NULL)
    {
        fprintf(stderr, "Failed to read INI file from ZIP\n");
        lua_close(L);
        pthread_mutex_unlock(&manager->lock);
        return NULL;
    }

    // 解析 INI 内容到 Lua 表
    parse_ini_to_lua_table(L, ini_content);
    free(ini_content);

    // 从 ZIP 文件中读取 Lua 脚本并加载
    if (luaL_loadfile(L, lua_script_filename) || lua_pcall(L, 0, 0, 0))
    {
        fprintf(stderr, "Error loading Lua script: %s\n", lua_tostring(L, -1));
        lua_close(L);
        pthread_mutex_unlock(&manager->lock);
        return NULL;
    }

    // 获取插件元数据
    lua_getglobal(L, "name");
    const char *name = lua_tostring(L, -1);
    lua_getglobal(L, "version");
    const char *version = lua_tostring(L, -1);
    lua_getglobal(L, "author");
    const char *author = lua_tostring(L, -1);
    lua_getglobal(L, "description");
    const char *description = lua_tostring(L, -1);

    // 创建插件结构体
    Plugin *plugin = (Plugin *)malloc(sizeof(Plugin));
    if (plugin == NULL)
    {
        fprintf(stderr, "Failed to allocate memory for plugin\n");
        lua_close(L);
        pthread_mutex_unlock(&manager->lock);
        return NULL;
    }

    plugin->name = strdup(name);
    plugin->version = strdup(version);
    plugin->author = strdup(author);
    plugin->description = strdup(description);
    plugin->L = L;
    plugin->resource_ref = LUA_NOREF;
    plugin->state = PLUGIN_LOADED;
    plugin->next = manager->plugins;
    manager->plugins = plugin;

    // 调用 on_load 函数并传递 ENV 表
    lua_getglobal(L, "on_load");
    if (lua_isfunction(L, -1))
    {
        if (lua_pcall(L, 1, 0, 0) != 0)
        {
            fprintf(stderr, "Error calling on_load: %s\n", lua_tostring(L, -1));
        }
    }

    pthread_mutex_unlock(&manager->lock);
    return plugin;
}

// 卸载插件
int plugin_manager_unload(PluginManager *manager, const char *name)
{
    pthread_mutex_lock(&manager->lock);

    Plugin *prev = NULL;
    Plugin *current = manager->plugins;
    while (current != NULL)
    {
        if (strcmp(current->name, name) == 0)
        {
            // 调用 on_unload 函数
            lua_State *L = current->L;
            lua_getglobal(L, "on_unload");
            if (lua_isfunction(L, -1))
            {
                if (lua_pcall(L, 0, 0, 0) != 0)
                {
                    fprintf(stderr, "Error calling on_unload: %s\n", lua_tostring(L, -1));
                }
            }

            // 释放资源
            if (current->resource_ref != LUA_NOREF)
            {
                luaL_unref(L, LUA_REGISTRYINDEX, current->resource_ref);
            }

            // 关闭 Lua 状态
            lua_close(L);

            // 释放插件结构体
            free(current->name);
            free(current->version);
            free(current->author);
            free(current->description);
            if (prev == NULL)
            {
                manager->plugins = current->next;
            }
            else
            {
                prev->next = current->next;
            }
            free(current);

            pthread_mutex_unlock(&manager->lock);
            return 0;
        }
        prev = current;
        current = current->next;
    }

    pthread_mutex_unlock(&manager->lock);
    return -1;
}

// 执行插件的主逻辑
int plugin_manager_execute(PluginManager *manager, const char *name)
{
    pthread_mutex_lock(&manager->lock);

    Plugin *current = manager->plugins;
    while (current != NULL)
    {
        if (strcmp(current->name, name) == 0)
        {
            lua_State *L = current->L;
            lua_getglobal(L, "main");
            if (lua_isfunction(L, -1))
            {
                if (lua_pcall(L, 0, 0, 0) != 0)
                {
                    fprintf(stderr, "Error calling main: %s\n", lua_tostring(L, -1));
                    pthread_mutex_unlock(&manager->lock);
                    return -1;
                }
            }
            pthread_mutex_unlock(&manager->lock);
            return 0;
        }
        current = current->next;
    }

    pthread_mutex_unlock(&manager->lock);
    return -1;
}

// 查询插件状态
PluginState plugin_manager_query_state(PluginManager *manager, const char *name)
{
    pthread_mutex_lock(&manager->lock);

    Plugin *current = manager->plugins;
    while (current != NULL)
    {
        if (strcmp(current->name, name) == 0)
        {
            PluginState state = current->state;
            pthread_mutex_unlock(&manager->lock);
            return state;
        }
        current = current->next;
    }

    pthread_mutex_unlock(&manager->lock);
    return PLUGIN_UNLOADED;
}