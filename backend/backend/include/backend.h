// backend.h: 标准系统包含文件的包含文件
// 或项目特定的包含文件。

#pragma once

#define _TURN_OFF_PLATFORM_STRING

#include <iostream>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <cpprest/http_client.h>

struct StaticConfiguration
{
	const utility::string_t addr = L"http://*:";
	const utility::string_t port = L"34600";
};