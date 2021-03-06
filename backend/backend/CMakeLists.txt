﻿# CMakeList.txt: backend 的 CMake 项目，在此处包括源代码并定义
# 项目特定的逻辑。
#
cmake_minimum_required (VERSION 3.8)

find_package(cpprestsdk CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(libmodbus CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
aux_source_directory(
    ./src DIR_SRC
)
# 将源代码添加到此项目的可执行文件。
add_executable (backend ${DIR_SRC})

# TODO: 如有需要，请添加测试并安装目标。
target_compile_features(backend PUBLIC cxx_std_17)
target_link_libraries(backend PRIVATE
	bcrypt
	modbus
	fmt::fmt
	spdlog::spdlog
	cpprestsdk::cpprest
	cpprestsdk::cpprestsdk_zlib_internal
	cpprestsdk::cpprestsdk_boost_internal 
	cpprestsdk::cpprestsdk_brotli_internal)