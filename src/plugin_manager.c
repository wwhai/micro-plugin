#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "plugin_manager.h"
#include "plugin_loader.h"
#include "plugin_utils.h"

#define MAX_PLUGINS 10

typedef struct
{
    char name[100];
    PluginDetail detail;
    lua_State *L;
} Plugin;

Plugin plugins[MAX_PLUGINS];
int plugin_count = 0;

// 初始化插件管理器
void plugin_manager_init()
{
    plugin_count = 0;
}

// 加载插件
bool plugin_manager_load(const char *zip_path)
{
    if (plugin_count >= MAX_PLUGINS)
    {
        printf("Too many plugins loaded.\n");
        return false;
    }

    PluginDetail detail;
    lua_State *L;
    if (!load_plugin(zip_path, &detail, &L))
    {
        return false;
    }

    strncpy(plugins[plugin_count].name, detail.name, sizeof(plugins[plugin_count].name) - 1);
    plugins[plugin_count].detail = detail;
    plugins[plugin_count].detail.state = 1; // Running
    plugins[plugin_count].L = L;
    plugin_count++;

    printf("Plugin loaded successfully: %s\n", detail.name);
    return true;
}

// 卸载插件
bool plugin_manager_unload(const char *plugin_name)
{
    for (int i = 0; i < plugin_count; i++)
    {
        if (strcmp(plugins[i].name, plugin_name) == 0)
        {
            // 调用 Lua 的 on_unload 函数释放资源
            lua_getglobal(plugins[i].L, "on_unload");
            if (lua_isfunction(plugins[i].L, -1))
            {
                if (lua_pcall(plugins[i].L, 0, 0, 0) != 0)
                {
                    printf("Failed to call on_unload function: %s\n", lua_tostring(plugins[i].L, -1));
                }
            }
            lua_close(plugins[i].L);
            for (int j = i; j < plugin_count - 1; j++)
            {
                plugins[j] = plugins[j + 1];
            }
            plugin_count--;
            printf("Plugin unloaded successfully: %s\n", plugin_name);
            return true;
        }
    }

    printf("Plugin not found: %s\n", plugin_name);
    return false;
}

// 查看所有已加载的插件
void plugin_manager_list()
{
    if (plugin_count == 0)
    {
        printf("No plugins loaded.\n");
        return;
    }

    printf("Loaded plugins:\n");
    for (int i = 0; i < plugin_count; i++)
    {
        printf("%s\n", plugins[i].name);
    }
}

// 查看插件状态
PluginDetail plugin_manager_get_state(const char *plugin_name)
{
    PluginDetail empty_detail = {0};
    for (int i = 0; i < plugin_count; i++)
    {
        if (strcmp(plugins[i].name, plugin_name) == 0)
        {
            return plugins[i].detail;
        }
    }
    return empty_detail;
}

// 清理插件管理器
void plugin_manager_cleanup()
{
    for (int i = 0; i < plugin_count; i++)
    {
        lua_close(plugins[i].L);
    }
    plugin_count = 0;
}