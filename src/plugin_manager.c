#include "plugin_manager.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <ini.h>

// 互斥锁，用于多线程安全
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

// 存储解析后的 ENV 数据
typedef struct
{
    lua_State *L;
    int env_table_index;
} EnvData;

// 自定义的 INI 解析回调函数
static int my_ini_handler(void *user, const char *section, const char *name, const char *value)
{
    EnvData *env_data = (EnvData *)user;
    lua_State *L = env_data->L;
    int env_table_index = env_data->env_table_index;

    lua_pushstring(L, name);
    lua_pushstring(L, value);
    lua_settable(L, env_table_index);

    return 1;
}

// 初始化插件管理器
PluginManager *plugin_manager_init()
{
    PluginManager *manager = (PluginManager *)malloc(sizeof(PluginManager));
    if (manager == NULL)
    {
        return NULL;
    }
    manager->plugins = NULL;
    manager->plugin_count = 0;
    return manager;
}

// 从 ZIP 包中提取文件
static int extract_file_from_zip(const char *zip_path, const char *filename, char **content)
{
    struct zip *za;
    struct zip_stat sb;
    int err;

    za = zip_open(zip_path, 0, &err);
    if (za == NULL)
    {
        return -1;
    }

    if (zip_stat(za, filename, 0, &sb) == -1)
    {
        zip_close(za);
        return -1;
    }

    *content = (char *)malloc(sb.size + 1);
    if (*content == NULL)
    {
        zip_close(za);
        return -1;
    }

    struct zip_file *zf = zip_fopen(za, filename, 0);
    if (zf == NULL)
    {
        free(*content);
        zip_close(za);
        return -1;
    }

    zip_fread(zf, *content, sb.size);
    (*content)[sb.size] = '\0';

    zip_fclose(zf);
    zip_close(za);

    return 0;
}

// 加载插件
int plugin_load(PluginManager *manager, const char *zip_path)
{
    pthread_mutex_lock(&mutex);

    manager->plugins = (Plugin *)realloc(manager->plugins, (manager->plugin_count + 1) * sizeof(Plugin));
    if (manager->plugins == NULL)
    {
        pthread_mutex_unlock(&mutex);
        return -1;
    }

    Plugin *plugin = &manager->plugins[manager->plugin_count];
    plugin->name = strdup("MyPlugin");
    plugin->zip_path = strdup(zip_path);
    plugin->L = luaL_newstate();
    luaL_openlibs(plugin->L);
    plugin->status = PLUGIN_LOADED;

    // 提取 env.ini 和 main.lua
    char *env_content = NULL;
    char *lua_content = NULL;
    if (extract_file_from_zip(zip_path, "env.ini", &env_content) != 0 ||
        extract_file_from_zip(zip_path, "main.lua", &lua_content) != 0)
    {
        free(plugin->name);
        free(plugin->zip_path);
        lua_close(plugin->L);
        manager->plugin_count--;
        pthread_mutex_unlock(&mutex);
        return -1;
    }

    // 创建一个新的 Lua 表用于存储 ENV 数据
    lua_newtable(plugin->L);
    int env_table_index = lua_gettop(plugin->L);

    // 解析 env.ini 并将键值对存储到 Lua 表中
    EnvData env_data = {plugin->L, env_table_index};
    ini_parse_string(env_content, my_ini_handler, &env_data);

    // 加载 Lua 脚本
    if (luaL_loadbuffer(plugin->L, lua_content, strlen(lua_content), "main.lua") != 0)
    {
        fprintf(stderr, "Error loading Lua script: %s\n", lua_tostring(plugin->L, -1));
        free(env_content);
        free(lua_content);
        free(plugin->name);
        free(plugin->zip_path);
        lua_close(plugin->L);
        manager->plugin_count--;
        pthread_mutex_unlock(&mutex);
        return -1;
    }

    // 执行 Lua 脚本
    if (lua_pcall(plugin->L, 0, 0, 0) != 0)
    {
        fprintf(stderr, "Error executing Lua script: %s\n", lua_tostring(plugin->L, -1));
        free(env_content);
        free(lua_content);
        free(plugin->name);
        free(plugin->zip_path);
        lua_close(plugin->L);
        manager->plugin_count--;
        pthread_mutex_unlock(&mutex);
        return -1;
    }

    // 调用 on_load 函数并传递 ENV 表
    lua_getglobal(plugin->L, "on_load");
    if (lua_isfunction(plugin->L, -1))
    {
        lua_pushvalue(plugin->L, env_table_index);
        if (lua_pcall(plugin->L, 1, 0, 0) != 0)
        {
            fprintf(stderr, "Error calling on_load: %s\n", lua_tostring(plugin->L, -1));
        }
    }

    free(env_content);
    free(lua_content);

    manager->plugin_count++;
    pthread_mutex_unlock(&mutex);
    return 0;
}

// 执行插件
int plugin_execute(PluginManager *manager, int plugin_index)
{
    pthread_mutex_lock(&mutex);

    if (plugin_index < 0 || plugin_index >= manager->plugin_count)
    {
        pthread_mutex_unlock(&mutex);
        return -1;
    }

    Plugin *plugin = &manager->plugins[plugin_index];
    if (plugin->status != PLUGIN_LOADED)
    {
        pthread_mutex_unlock(&mutex);
        return -1;
    }

    plugin->status = PLUGIN_RUNNING;

    // 调用 main 函数
    lua_getglobal(plugin->L, "main");
    if (lua_isfunction(plugin->L, -1))
    {
        if (lua_pcall(plugin->L, 0, 0, 0) != 0)
        {
            fprintf(stderr, "Error calling main: %s\n", lua_tostring(plugin->L, -1));
            plugin->status = PLUGIN_LOADED;
            pthread_mutex_unlock(&mutex);
            return -1;
        }
    }

    plugin->status = PLUGIN_LOADED;
    pthread_mutex_unlock(&mutex);
    return 0;
}

// 卸载插件
int plugin_unload(PluginManager *manager, int plugin_index)
{
    pthread_mutex_lock(&mutex);

    if (plugin_index < 0 || plugin_index >= manager->plugin_count)
    {
        pthread_mutex_unlock(&mutex);
        return -1;
    }

    Plugin *plugin = &manager->plugins[plugin_index];
    if (plugin->status != PLUGIN_LOADED)
    {
        pthread_mutex_unlock(&mutex);
        return -1;
    }

    // 调用 on_unload 函数
    lua_getglobal(plugin->L, "on_unload");
    if (lua_isfunction(plugin->L, -1))
    {
        if (lua_pcall(plugin->L, 0, 0, 0) != 0)
        {
            fprintf(stderr, "Error calling on_unload: %s\n", lua_tostring(plugin->L, -1));
        }
    }

    free(plugin->name);
    free(plugin->zip_path);
    lua_close(plugin->L);

    // 移除插件
    for (int i = plugin_index; i < manager->plugin_count - 1; i++)
    {
        manager->plugins[i] = manager->plugins[i + 1];
    }
    manager->plugin_count--;
    manager->plugins = (Plugin *)realloc(manager->plugins, manager->plugin_count * sizeof(Plugin));

    pthread_mutex_unlock(&mutex);
    return 0;
}

// 释放插件管理器
void plugin_manager_free(PluginManager *manager)
{
    for (int i = 0; i < manager->plugin_count; i++)
    {
        free(manager->plugins[i].name);
        free(manager->plugins[i].zip_path);
        lua_close(manager->plugins[i].L);
    }
    free(manager->plugins);
    free(manager);
}