
#include "backend.h"
#include "SerialComController.hpp"
#include "Modbus.hpp"
#include "utils.hpp"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/exception/all.hpp>

SerialComController::SerialComController(utility::string_t url, UartModbus* uartModbus)
	:m_Listener(url)
	, m_Modbus(uartModbus)
{
	m_Listener.support(methods::GET, std::bind(&SerialComController::Get, this, std::placeholders::_1));
	m_Listener.support(methods::POST, std::bind(&SerialComController::Post, this, std::placeholders::_1));
}

SerialComController::~SerialComController()
{
}


void SerialComController::Get(http_request message)
{

}

void SerialComController::Post(http_request message)
{
	message.extract_string().then(
		[=](pplx::task<string_t> t) {
			try
			{
				boost::property_tree::ptree root;
				std::istringstream ss(utility::conversions::to_utf8string(t.get()));
				boost::property_tree::read_json(ss, root);
				std::string action = root.get<std::string>("action");
				if (!action.compare("activate"))
				{
					m_Modbus->Open(
						root.get<std::string>("deviceName"),
						strtol(root.get<std::string>("baudrate").c_str(), NULL, 10),
						static_cast<UartModbus::Parity>(strtol(root.get<std::string>("parity").c_str(), NULL, 10)),
						strtol(root.get<std::string>("dataBit").c_str(), NULL, 10),
						static_cast<UartModbus::StopBit>(strtol(root.get<std::string>("stopBit").c_str(), NULL, 10)),
						strtol(root.get<std::string>("slaveId").c_str(), NULL, 10));
					m_Modbus->SetFloatByteOrder(UartModbus::FloatByteOrder::BADC);
				}
				else if (!action.compare("deactivate"))
				{
					m_Modbus->Close();
				}
				message.reply(MakeSingleValueResponse(status_codes::OK, "ret", "ok"));
			}
			catch (UartModbus::ConnectError & e)
			{
				message.reply(MakeErrorResponse(status_codes::InternalError, e.what()));
			}
			catch (...)
			{
#ifdef _DEBUG
				message.reply(MakeErrorResponse(status_codes::InternalError, 
												boost::current_exception_diagnostic_information()));
#else
				message.reply(MakeErrorResponse(status_codes::InternalError,
												"Undefined error"));
#endif
			}
		}
	);
}