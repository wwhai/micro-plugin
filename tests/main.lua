-- 插件元数据
name = "MyPlugin"
version = "1.0.0"
author = "John Doe"
description = "This is a sample Lua plugin."

-- 私有命名空间模拟（Lua 的闭包和模块模式）
local private_data = {}

-- 初始化资源
function init_resources()
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
    for k, v in pairs(Env) do
        print(k .. " = " .. v)
    end
    print("Plugin loaded successfully")
end

-- 生命周期回调：插件卸载时调用
function on_unload()
    -- 调用资源释放函数
    print("Plugin unloaded successfully")
end

-- 主逻辑函数
function main()

end
