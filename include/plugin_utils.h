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

#ifndef UTILS_H
#define UTILS_H

// 从 ZIP 文件中读取文件内容
char *read_file_from_zip(const char *zip_filename, const char *file_to_read);

// 从 ZIP 文件中提取文件
void extract_file_from_zip(const char *zip_filename, const char *file_in_zip, const char *output_filename);

// 计算文件的 MD5 校验码
char *calculate_md5(const char *filename);

// 解析 INI 文件内容到 Lua 表
void parse_ini_to_lua_table(lua_State *L, const char *ini_content);

#endif // UTILS_H