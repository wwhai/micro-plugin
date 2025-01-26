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
#include "plugin_manager.h"

int main()
{
    plugin_manager_init();

    // 测试加载插件
    if (plugin_manager_load("test_plugin.zip"))
    {
        printf("Plugin loaded successfully for testing.\n");
    }
    else
    {
        printf("Failed to load plugin for testing.\n");
    }

    // 测试查看插件列表
    plugin_manager_list();

    // 测试卸载插件
    if (plugin_manager_unload("TestPlugin"))
    {
        printf("Plugin unloaded successfully for testing.\n");
    }
    else
    {
        printf("Failed to unload plugin for testing.\n");
    }

    // 清理插件管理器
    plugin_manager_cleanup();

    return 0;
}