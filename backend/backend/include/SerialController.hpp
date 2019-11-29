#ifndef SERIALCONTROLLER_HPP
#define SERIALCONTROLLER_HPP

#include <cpprest/http_listener.h>
#include <cpprest/http_msg.h>
#include <cpprest/uri.h>


#include "Modbus.hpp"
#include "InterfaceWrapper.hpp"

#include <iostream>
#include <string>
#include <unordered_map>

using namespace web;
using namespace http;
using namespace utility;
using namespace http::experimental::listener;

class SerialController
{
public:
	SerialController();
	SerialController(utility::string_t url);
	virtual ~SerialController();

	pplx::task<void>open() { return m_Listener.open(); }
	pplx::task<void>close() { return m_Listener.close(); }
private:

	void Get(http_request message);

	void LoadConfiguration();

	std::unordered_map<std::string, std::unique_ptr<InterfaceWrapper>> m_Interfaces;
	std::unique_ptr<UartModbus> m_Modbus;
	http_listener m_Listener;
};
#endif //SERIALCONTROLLER_HPP