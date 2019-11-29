// backend.cpp: 定义应用程序的入口点。
//

#include "backend.h"

#include <cpprest/http_client.h>
#include <cpprest/filestream.h>
#include "SerialController.hpp"
#include <memory>

using namespace utility;                    // Common utilities like string conversions
using namespace web;                        // Common features like URIs.
using namespace web::http;                  // Common HTTP functionality
using namespace web::http::client;          // HTTP client features
using namespace concurrency::streams;       // Asynchronous streams

std::unique_ptr<SerialController> serialController{};

int main()
{
	utility::string_t port = L"8080";
	utility::string_t addr = L"http://*:";
	utility::string_t path = L"/api";
	addr.append(port).append(path);
	serialController = std::make_unique<SerialController>(addr);
	serialController->open().wait();

	ucout << utility::string_t(L"Listening for requests at: ") << addr << std::endl;

	std::string line;
	std::getline(std::cin, line);

	serialController->close().wait();
	return 0;
}
