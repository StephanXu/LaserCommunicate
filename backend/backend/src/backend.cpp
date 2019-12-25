// backend.cpp: 定义应用程序的入口点。
//

#include "backend.h"
#include <cpprest/http_client.h>
#include <spdlog/sinks/rotating_file_sink.h>

#include "InterfaceController.hpp"
#include "SerialComController.hpp"
#include "DI.hpp"

#include <memory>

using namespace utility;                    // Common utilities like string conversions
using namespace web;                        // Common features like URIs.
using namespace web::http;                  // Common HTTP functionality
using namespace web::http::client;          // HTTP client features
using namespace concurrency::streams;       // Asynchronous streams

std::unique_ptr<StaticConfiguration> MakeStaticConfiguration()
{
	return std::make_unique<StaticConfiguration>();
}

std::unique_ptr<UartModbus> MakeUartModbus()
{
	return std::make_unique<UartModbus>();
}

void InitLog()
{
#if _DEBUG
	auto logger = spdlog::rotating_logger_mt("file_logger", "backend.log", 1024 * 1024 * 5, 3);
#else
	auto logger = spdlog::stderr_color_mt("console");
#endif
	spdlog::set_default_logger(logger);
	spdlog::set_pattern("[%T.%e] [%-5t] %^[%l]%$  %v");
	spdlog::set_level(spdlog::level::info);
}

int main()
{
	InitLog();

	DIConfiguration di;
	di.Add(MakeStaticConfiguration);
	di.Add(MakeUartModbus);
	di.Add(InterfaceController::MakeInterfaceController);
	di.Add(SerialComController::MakeSerialComController);

	auto injector = di.BuildInjector();
	injector.GetInstance<InterfaceController>()->open().wait();
	injector.GetInstance<SerialComController>()->open().wait();

	auto config = injector.GetInstance<StaticConfiguration>();
	//ucout << utility::string_t(L"Listening for requests at: ") << config->addr + config->port << std::endl;
	spdlog::info("Listening for requests at: {}{}",
				 conversions::to_utf8string(config->addr),
				 conversions::to_utf8string(config->port));

	std::string line;
	std::getline(std::cin, line);

	injector.GetInstance<InterfaceController>()->close().wait();
	injector.GetInstance<SerialComController>()->close().wait();
	return 0;
}
