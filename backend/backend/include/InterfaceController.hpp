#ifndef INTERFACE_CONTROLLER_HPP
#define INTERFACE_CONTROLLER_HPP

#include <cpprest/http_listener.h>

#include "Modbus.hpp"
#include "InterfaceWrapper.hpp"

#include <iostream>
#include <string>
#include <unordered_map>
#include <memory>

using namespace web;
using namespace http;
using namespace utility;
using namespace http::experimental::listener;

class InterfaceController
{
public:

	InterfaceController() = delete;
	InterfaceController(utility::string_t url, UartModbus* uartModbus);
	virtual ~InterfaceController();

	static std::unique_ptr<InterfaceController> MakeInterfaceController(StaticConfiguration* staticConfig,
																		UartModbus* uart)
	{
		return std::make_unique<InterfaceController>(
			staticConfig->addr + staticConfig->port + L"/api/interface", uart);
	}

	pplx::task<void>open() { return m_Listener.open(); }
	pplx::task<void>close() { return m_Listener.close(); }
private:

	void Get(http_request message);

	void Post(http_request message);

	void LoadConfiguration();

	std::unordered_map<std::string, std::unique_ptr<InterfaceWrapper>> m_Interfaces;
	UartModbus* m_Modbus = nullptr;
	http_listener m_Listener;
};



#endif //INTERFACE_CONTROLLER_HPP