# macro-plugin

## 一、项目概述
本项目实现了插件管理器，用于加载、执行和卸载 Lua 脚本插件。插件以 ZIP 包的形式分发，其中包含 Lua 脚本和一个 INI 配置文件（作为 ENV 内容）。管理器在加载插件时会进行 MD5 校验，以确保插件文件未被篡改。

## 二、文件结构
项目主要包含以下文件：
- **`plugin_manager.h`**：定义插件管理器相关的结构体、枚举类型以及函数接口。
- **`plugin_manager.c`**：实现插件管理器的各种功能，包括插件的加载、卸载、执行和状态查询等。
- **`utils.h`**：定义一些实用工具函数的接口，如 ZIP 文件读取、MD5 计算等。
- **`utils.c`**：实现 `utils.h` 中声明的工具函数。
- **`main.c`**：项目的入口文件，演示了如何使用插件管理器加载、执行和卸载插件。
- **`app-demo.lua`**：示例 Lua 插件脚本，包含插件元数据、资源管理函数、生命周期回调函数和主逻辑函数。
- **`env.ini`**：INI 配置文件，作为插件的环境配置信息。

## 三、编译与运行
### 3.1 编译
使用 CMake 进行项目的编译，确保系统已经安装了 CMake、Lua 开发库、OpenSSL 开发库和 zlib 开发库。在项目根目录下执行以下命令：
```sh
mkdir build
cd build
cmake ..
make
```

### 3.2 运行
编译完成后，会在 `build` 目录下生成一个名为 `plugin_manager` 的可执行文件。运行该可执行文件即可测试插件管理器的功能：
```sh
./plugin_manager
```

## 四、插件管理器功能
### 4.1 插件加载
- **函数**：`Plugin* plugin_manager_load(PluginManager *manager, const char *zip_filename)`
- **功能**：从指定的 ZIP 文件中加载插件，进行 MD5 校验，读取 INI 配置文件，初始化 Lua 环境，调用插件的 `on_load` 函数。
- **返回值**：如果加载成功，返回指向插件结构体的指针，包含插件的元信息；如果加载失败，返回 `NULL`。

### 4.2 插件卸载
- **函数**：`int plugin_manager_unload(PluginManager *manager, const char *name)`
- **功能**：根据插件名称卸载指定的插件，调用插件的 `on_unload` 函数，释放相关资源。
- **返回值**：如果卸载成功，返回 0；如果卸载失败，返回 -1。

### 4.3 插件执行
- **函数**：`int plugin_manager_execute(PluginManager *manager, const char *name)`
- **功能**：根据插件名称执行指定插件的主逻辑函数 `main`。
- **返回值**：如果执行成功，返回 0；如果执行失败，返回 -1。

### 4.4 插件状态查询
- **函数**：`PluginState plugin_manager_query_state(PluginManager *manager, const char *name)`
- **功能**：根据插件名称查询指定插件的状态。
- **返回值**：返回插件的状态，可能的值为 `PLUGIN_UNLOADED`、`PLUGIN_LOADED` 或 `PLUGIN_RUNNING`。

## 五、插件脚本规范
### 5.1 元数据
插件脚本中需要定义以下元数据：
```lua
name = "MyPlugin"
version = "1.0.0"
author = "John Doe"
description = "This is a sample Lua plugin."
```

### 5.2 生命周期回调函数
- **`on_load(Env)`**：插件加载时调用，用于初始化资源。`Env` 是一个 Lua 表，包含从 INI 配置文件中解析的环境信息。
- **`on_unload()`**：插件卸载时调用，用于释放资源。
- **`main()`**：插件的主逻辑函数，在插件执行时调用。

### 5.3 资源管理函数
- **`init_resources()`**：初始化插件所需的资源。
- **`release_resources()`**：释放插件占用的资源。

## 六、MD5 校验机制
为了防止插件文件被篡改，插件 ZIP 包中会包含一个 `md5.txt` 文件，其中存储了 Lua 脚本的 MD5 校验码。在加载插件时，管理器会重新计算 Lua 脚本的 MD5 校验码，并与存储的校验码进行比较。如果校验码不匹配，将拒绝加载插件，并输出错误信息。

## 七、线程安全
插件管理器使用 `pthread_mutex_t` 实现了线程安全，确保在多线程环境下插件的加载、卸载和执行不会引发竞态条件。

## 八、错误处理
在插件加载、执行和卸载过程中，会对各种可能的错误进行检查，如文件读取失败、MD5 校验失败、Lua 脚本加载失败等。如果发生错误，会输出相应的错误信息，并进行适当的清理操作。

## 九、示例代码
以下是一个简单的示例，演示了如何使用插件管理器：
```c
#include <stdio.h>
#include "plugin_manager.h"

// 示例用法
int main() {
    PluginManager manager;
    plugin_manager_init(&manager);

    // 加载插件
    Plugin* plugin = plugin_manager_load(&manager, "plugin.zip");
    if (plugin != NULL) {
        printf("Plugin loaded successfully\n");
        printf("Plugin Name: %s\n", plugin->name);
        printf("Plugin Version: %s\n", plugin->version);
        printf("Plugin Author: %s\n", plugin->author);
        printf("Plugin Description: %s\n", plugin->description);

        // 执行插件的主逻辑
        if (plugin_manager_execute(&manager, plugin->name) == 0) {
            printf("Plugin executed successfully\n");
        }

        // 卸载插件
        if (plugin_manager_unload(&manager, plugin->name) == 0) {
            printf("Plugin unloaded successfully\n");
        }
    }

    plugin_manager_destroy(&manager);
    return 0;
}
```

## 十、注意事项
- 本项目使用了一些第三方库，如 Lua、OpenSSL 和 zlib，在编译和运行时需要确保这些库已经正确安装。
- `read_file_from_zip` 和 `extract_file_from_zip` 函数的实现使用了占位代码，实际应用中需要使用如 `minizip` 等第三方库来实现具体的 ZIP 文件读取和提取功能。
- 运行时需要确保 `plugin.zip` 文件包含 `app-demo.lua`、`env.ini` 和 `md5.txt`。