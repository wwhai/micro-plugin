#include <stdio.h>
#include "plugin_manager.h"

// 示例用法
int main()
{
    PluginManager manager;
    plugin_manager_init(&manager);

    // 加载插件
    Plugin *plugin = plugin_manager_load(&manager, "plugin.zip");
    if (plugin != NULL)
    {
        printf("Plugin loaded successfully\n");
        printf("Plugin Name: %s\n", plugin->name);
        printf("Plugin Version: %s\n", plugin->version);
        printf("Plugin Author: %s\n", plugin->author);
        printf("Plugin Description: %s\n", plugin->description);

        // 执行插件的主逻辑
        if (plugin_manager_execute(&manager, plugin->name) == 0)
        {
            printf("Plugin executed successfully\n");
        }

        // 卸载插件
        if (plugin_manager_unload(&manager, plugin->name) == 0)
        {
            printf("Plugin unloaded successfully\n");
        }
    }

    plugin_manager_destroy(&manager);
    return 0;
}