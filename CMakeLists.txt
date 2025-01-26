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

cmake_minimum_required(VERSION 3.10)
project(plugin_manager)

set(CMAKE_C_STANDARD 11)

include_directories(include)


add_executable(test_plugin_manager
    src/plugin_manager.c
    src/plugin_loader.c
    src/plugin_utils.c
    tests/test_plugin_manager.c
)

find_package(Lua REQUIRED)
include_directories(${LUA_INCLUDE_DIR})
target_link_libraries(test_plugin_manager ${LUA_LIBRARIES} zip)