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

// plugin_utils.h

// 防止头文件被重复包含
#ifndef PLUGIN_UTILS_H
#define PLUGIN_UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zip.h>
#include "plugin_manager.h"
#include <lua.h>
// 读取 app.properties 文件内容
bool read_app_properties(zip_t *z, PluginDetail *detail);
// 读取 app.properties 文件中的 [env] 部分并传递给 Lua
void read_env_params(zip_t *z, lua_State *L);
// 验证插件签名
bool verify_signature(const char *expected_signature);

#endif // PLUGIN_UTILS_H