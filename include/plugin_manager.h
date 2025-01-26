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

#include <stdbool.h>

typedef struct
{
    char name[100];
    char version[20];
    char author[100];
    char description[200];
    char signature[100];
    int state; // 0: Stopped, 1: Running
} PluginDetail;

// 初始化插件管理器
void plugin_manager_init();

// 加载插件
bool plugin_manager_load(const char *zip_path);

// 卸载插件
bool plugin_manager_unload(const char *plugin_name);

// 查看所有已加载的插件
void plugin_manager_list();

// 查看插件状态
PluginDetail plugin_manager_get_state(const char *plugin_name);

// 清理插件管理器
void plugin_manager_cleanup();

#endif