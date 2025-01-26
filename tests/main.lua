-- 全局变量用于存储资源
local resources = {}

-- 初始化资源
local function init_resources(startup_params)
    -- 打印启动参数
    if startup_params then
        for k, v in pairs(startup_params) do
            print("Startup param:", k, "=", v)
        end
    end
    -- 模拟资源分配
    resources.data = {}
    for i = 1, 100 do
        table.insert(resources.data, i)
    end
    print("Resources initialized")
end

-- 释放资源
local function release_resources()
    -- 模拟资源释放
    resources.data = nil
    print("Resources released")
end

-- main 函数，插件加载时会执行此函数
function main(startup_params)
    init_resources(startup_params)
    print("MyAppPlugin is loaded and running.")
end

-- 卸载时调用的函数
function on_unload()
    release_resources()
end

-- 返回模块
return {
    main = main,
    on_unload = on_unload
}