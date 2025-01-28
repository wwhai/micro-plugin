-- Copyright (C) 2025 wwhai
--
-- This program is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Affero General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- This program is distributed in the hope that it will be useful,
-- but WITHOUT ANY WARRANTY; without even the implied warranty of
-- MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
-- GNU Affero General Public License for more details.
--
-- You should have received a copy of the GNU Affero General Public License
-- along with this program.  If not, see <https://www.gnu.org/licenses/>.


-- 插件元数据
name = "MyPlugin"
version = "1.0.0"
author = "John Doe"
description = "This is a sample Lua plugin."

-- 私有命名空间模拟（Lua 的闭包和模块模式）
local private_data = {}

-- 初始化资源
function init_resources()
    -- 示例：初始化一个表作为资源
    private_data.resource_table = {}
    if not private_data.resource_table then
        error("Failed to initialize resources")
    end
    return true
end

-- 释放资源
function release_resources()
    -- 示例：清空资源表
    private_data.resource_table = nil
    return true
end

-- 生命周期回调：插件加载时调用
function on_load(Env)
    -- 调用资源初始化函数
    local success = init_resources()
    if not success then
        error("Resource initialization failed during on_load")
    end

    -- 打印 ENV 表中的内容
    for k, v in pairs(Env) do
        print("ENV[" .. k .. "] = " .. v)
    end

    print("Plugin loaded successfully")
end

-- 生命周期回调：插件卸载时调用
function on_unload()
    -- 调用资源释放函数
    local success = release_resources()
    if not success then
        error("Resource release failed during on_unload")
    end
    print("Plugin unloaded successfully")
end

-- 主逻辑函数
function main()
    -- 示例：向资源表中添加数据
    table.insert(private_data.resource_table, "Some data")
    print("Plugin main logic executed")
end
