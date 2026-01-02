# USB Sound Map
# Copyright (C) 2025, Bruce MacKinnon KC1FSZ
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# ( at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.
cmake_minimum_required(VERSION 3.13)
project(sound-map C CXX ASM)
set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD 23)

add_compile_options(-fstack-protector-all -Wall -Wpedantic -ggdb -O0)

add_executable(unit-test
  src/tests/unit-test.cpp
  src/sound-map.cpp
  src/sound-map-2.cpp
  src/usb-dir.c
)

target_include_directories(unit-test PRIVATE src)
target_include_directories(unit-test PRIVATE include)

target_link_libraries(unit-test -lusb-1.0)

add_executable(demo-1
  src/demos/demo-1.cpp
  src/sound-map.cpp
  src/sound-map-2.cpp
  src/usb-dir.c
)

target_include_directories(demo-1 PRIVATE src)
target_include_directories(demo-1 PRIVATE include)

target_link_libraries(demo-1 -lusb-1.0)

add_executable(demo-2
  src/demos/demo-2.cpp
  src/sound-map.cpp
)

target_include_directories(demo-2 PRIVATE src)
target_include_directories(demo-2 PRIVATE include)

target_link_libraries(demo-2 -lusb-1.0)

add_executable(demo-3
  src/demos/demo-3.cpp
  src/sound-map-2.cpp
  src/sound-map.cpp
  src/usb-dir.c
)

target_include_directories(demo-3 PRIVATE src)
target_include_directories(demo-3 PRIVATE include)

target_link_libraries(demo-3 -lusb-1.0)
