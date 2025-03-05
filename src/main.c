#include "plugin_manager.h"
#include <stdio.h>

int main()
{
    PluginManager *manager = plugin_manager_init();
    if (manager == NULL)
    {
        fprintf(stderr, "Failed to initialize plugin manager\n");
        return 1;
    }

    // 加载插件
    if (plugin_load(manager, "plugin.zip") != 0)
    {
        fprintf(stderr, "Failed to load plugin\n");
        plugin_manager_free(manager);
        return 1;
    }

    // 执行插件
    if (plugin_execute(manager, 0) != 0)
    {
        fprintf(stderr, "Failed to execute plugin\n");
    }

    // 卸载插件
    if (plugin_unload(manager, 0) != 0)
    {
        fprintf(stderr, "Failed to unload plugin\n");
    }

    // 释放插件管理器
    plugin_manager_free(manager);

    return 0;
}