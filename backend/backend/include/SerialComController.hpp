#ifndef SERIAL_COM_CONTROLLER_HPP
#define SERIAL_COM_CONTROLLER_HPP

#include <cpprest/http_listener.h>

#include "Modbus.hpp"

#include <iostream>
#include <string>
#include <unordered_map>
#include <memory>

using namespace web;
using namespace http;
using namespace utility;
using namespace http::experimental::listener;

class SerialComController
{
public:
	SerialComController() = delete;
	SerialComController(utility::string_t url, UartModbus* uartModbus);
	virtual ~SerialComController();

	static std::unique_ptr<SerialComController> MakeSerialComController(StaticConfiguration* staticConfig,
																		UartModbus* uart)
	{
		return std::make_unique<SerialComController>(
			staticConfig->addr + staticConfig->port + L"/api/com", uart);
	}

	pplx::task<void>open() { return m_Listener.open(); }
	pplx::task<void>close() { return m_Listener.close(); }
private:

	void Get(http_request message);

	void Post(http_request message);

	UartModbus* m_Modbus = nullptr;
	http_listener m_Listener;
};


#endif // SERIAL_COM_CONTROLLER_HPP