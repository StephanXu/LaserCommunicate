#ifndef SERIALCONTROLLER_HPP
#define SERIALCONTROLLER_HPP

#include <cpprest/http_listener.h>
#include <cpprest/http_msg.h>

#include <iostream>
#include <iterator>
#include <string>

using namespace std;
using namespace web;
using namespace http;
using namespace utility;
using namespace http::experimental::listener;

class SerialController
{
public:
	SerialController()
	{
	}
	SerialController(utility::string_t url) :m_Listener(url)
	{
		m_Listener.support(methods::GET, std::bind(&SerialController::Get, this, std::placeholders::_1));
	}
	virtual ~SerialController()
	{
	}

	pplx::task<void>open() { return m_Listener.open(); }
	pplx::task<void>close() { return m_Listener.close(); }
private:

	void Get(http_request message)
	{
		message.extract_json().then(
			[=](pplx::task<web::json::value> t) {
				try
				{
					auto res = t.get();
					
					ucout << res.to_string() << std::endl;
					message.reply(status_codes::OK, "ok", "text");
				}
				catch (...)
				{
					//...
				}
			}
		);
	}

	http_listener m_Listener;
};
#endif //SERIALCONTROLLER_HPP