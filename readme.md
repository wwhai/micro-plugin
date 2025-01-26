<!--
 Copyright (C) 2025 wwhai

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU Affero General Public License as
 published by the Free Software Foundation, either version 3 of the
 License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU Affero General Public License for more details.

 You should have received a copy of the GNU Affero General Public License
 along with this program.  If not, see <https://www.gnu.org/licenses/>.
-->

# Micro-Plugin

## 项目概述
本项目是一个插件管理器，用于加载、管理和卸载各种插件。它支持从 `.zip` 文件中加载插件，查看插件列表以及获取插件的详细信息。

## 项目结构
项目的主要文件和目录结构如下：

```bash
micro-plugin/
├── CMakeLists.txt         # CMake 构建脚本
├── include/               # 头文件目录
├── src/                   # 源文件目录
│   ├── plugin_manager.c   # 插件管理器核心代码
│   ├── plugin_loader.c    # 插件加载器代码
│   ├── plugin_utils.c     # 插件工具函数代码
│   └── main.c             # 主程序入口文件
└── tests/                 # 测试目录
    ├── app.properties     # 测试插件配置文件
    ├── main.lua           # 测试插件的 Lua 脚本
    └── test_plugin_manager.c # 插件管理器的测试代码
```

## 构建项目
### 依赖项
- CMake (版本 3.10 或更高)
- Lua 库: `sudo apt-get install liblua5.4-dev`
- Zip 库: `sudo apt-get install libzip-dev`

### 构建步骤
1. 克隆项目到本地：
   ```sh
   git clone <项目仓库地址>
   cd micro-plugin
   ```
2. 创建构建目录并进入：
   ```sh
   mkdir build
   cd build
   ```
3. 使用 CMake 生成构建文件：
   ```sh
   cmake ..
   ```
4. 编译项目：
   ```sh
   make
   ```

## 使用方法
### 运行主程序
编译完成后，在 `build` 目录下会生成可执行文件 `plugin_manager` 和 `test_plugin_manager`。

运行主程序：
```sh
./plugin_manager
```

### 测试插件管理器
运行测试程序：
```sh
./test_plugin_manager
```

### 插件配置文件
插件的配置信息存储在 `app.properties` 文件中，示例如下：

```ini
[application]
name=Test-Plugin
version=1.0.0
author=WWHAI
description=This is a sample plugin.
signature=ABCDEFGHIJKLMNOPQRSTUVWXYZ

[env]
key1=value1
key2=value2
```

## 代码说明
### 主要函数
- `plugin_manager_init()`：初始化插件管理器。
- `plugin_manager_load(const char *plugin_path)`：加载指定路径的插件。
- `plugin_manager_list()`：列出所有已加载的插件。
- `plugin_manager_get_state(const char *plugin_name)`：获取指定插件的详细信息。
- `plugin_manager_unload(const char *plugin_name)`：卸载指定名称的插件。
- `plugin_manager_cleanup()`：清理插件管理器。

### Lua 脚本
在 `tests/main.lua` 中实现了一个简单的插件示例，包含初始化资源、释放资源以及主函数和卸载函数。

## 许可证
本项目使用 GNU Affero General Public License v3 或更高版本进行许可。详细信息请参阅 [GNU Affero General Public License](https://www.gnu.org/licenses/)。
