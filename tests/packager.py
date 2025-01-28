# Copyright (C) 2025 wwhai
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License as
# published by the Free Software Foundation, either version 3 of the
# License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Affero General Public License for more details.
#
# You should have received a copy of the GNU Affero General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.

import hashlib
import zipfile
import os

def calculate_md5(filename):
    """
    计算文件的 MD5 校验码
    :param filename: 文件路径
    :return: 文件的 MD5 校验码
    """
    hash_md5 = hashlib.md5()
    with open(filename, "rb") as f:
        for chunk in iter(lambda: f.read(4096), b""):
            hash_md5.update(chunk)
    return hash_md5.hexdigest()

def pack_zip_with_md5(zip_filename, lua_script_filename, ini_filename):
    """
    打包 ZIP 文件并存储 MD5 校验码
    :param zip_filename: 生成的 ZIP 文件路径
    :param lua_script_filename: Lua 脚本文件路径
    :param ini_filename: INI 配置文件路径
    """
    # 计算 Lua 脚本的 MD5 校验码
    md5 = calculate_md5(lua_script_filename)
    with open("md5.txt", "w") as f:
        f.write(md5)

    # 创建 ZIP 文件并添加文件
    with zipfile.ZipFile(zip_filename, "w") as zipf:
        zipf.write(lua_script_filename)
        zipf.write(ini_filename)
        zipf.write("md5.txt")

    # 删除临时的 md5.txt 文件
    os.remove("md5.txt")

if __name__ == "__main__":
    # 示例用法
    lua_script = "app-demo.lua"
    ini_file = "env.ini"
    zip_file = "plugin.zip"

    # 检查 Lua 脚本和 INI 文件是否存在
    if not os.path.exists(lua_script) or not os.path.exists(ini_file):
        print("Lua script or INI file not found.")
    else:
        pack_zip_with_md5(zip_file, lua_script, ini_file)
        print(f"Plugin ZIP package {zip_file} created successfully.")