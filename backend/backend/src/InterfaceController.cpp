#include "backend.h"
#include "InterfaceController.hpp"
#include "modbus.h"
#include "utils.hpp"

#include <cpprest/http_msg.h>
#include <cpprest/uri.h>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

#include <fstream>
#include <sstream>
#include <typeindex>

InterfaceController::InterfaceController(utility::string_t url, UartModbus* uartModbus)
	:m_Listener(url)
	, m_Modbus(uartModbus)
{
	LoadConfiguration();
	m_Listener.support(methods::GET, std::bind(&InterfaceController::Get, this, std::placeholders::_1));
}

InterfaceController::~InterfaceController()
{
}

void InterfaceController::Get(http_request message)
{
	try
	{
		std::vector<string_t> paths = uri::split_path(message.relative_uri().path());
		if (paths.size() < 1)
		{
			// request all list
			boost::property_tree::ptree root;
			boost::property_tree::ptree actions;
			for (auto&& item : m_Interfaces)
			{
				boost::property_tree::ptree action;
				action.put("id", item.first);
				action.put("symbol", item.second->Symbol());
				action.put("desc", item.second->Describe());
				actions.push_back(std::make_pair("", action));
			}
			root.add_child("action", actions);
			std::ostringstream ss;
			boost::property_tree::write_json(ss, root, false);
			message.reply(status_codes::OK, ss.str(), "application/json; charset=utf-8");
			return;
		}
		std::string interfaceId = utility::conversions::to_utf8string(paths[0]);
		auto interfaceIter = m_Interfaces.find(interfaceId);
		if (m_Interfaces.end() == interfaceIter)
		{
			// interface do not exist
			ReplyError(message, status_codes::NotFound, "Interface not found");
			return;
		}
		std::string result = interfaceIter->second->Read();
		ReplySingleValue(message, status_codes::OK, "content", result);
	}
	catch (std::runtime_error & e)
	{
		ReplyError(message, status_codes::InternalError, e.what());
	}
	catch (...)
	{
		ReplyError(message, status_codes::InternalError, "Undefined error");
	}
}

void InterfaceController::LoadConfiguration()
{
	boost::property_tree::ptree config;
	std::ifstream f("Config.json");
	if (!f.is_open())
	{
		throw std::runtime_error("Can't open config file.");
	}
	boost::property_tree::read_json(f, config);
	for (auto&& item : config.get_child("action"))
	{
		std::string dataType = item.second.get<std::string>("dataType");
		unsigned int registerAddress = strtol(item.second.get<std::string>("register").c_str(), NULL, 16);
		std::string symbol = item.second.get<std::string>("symbol");
		std::string desc = item.second.get<std::string>("desc");
		bool writable = item.second.get<bool>("writable");
		unsigned int availableInMode = strtol(item.second.get<std::string>("availableInMode").c_str(), NULL, 10);
		std::string uidStr{};
		do
		{
			boost::uuids::uuid uid = boost::uuids::random_generator()();
			uidStr = boost::uuids::to_string(uid);
		} while (m_Interfaces.end() != m_Interfaces.find(uidStr));

		if (0 == dataType.compare("U32"))
		{
			m_Interfaces.insert(std::make_pair(uidStr, InterfaceWrapper::MakeInterfaceWrapper<uint32_t>(
				m_Modbus, registerAddress, symbol, desc, writable, availableInMode)));
		}
		else if (0 == dataType.compare("U16"))
		{
			m_Interfaces.insert(std::make_pair(uidStr, InterfaceWrapper::MakeInterfaceWrapper<uint16_t>(
				m_Modbus, registerAddress, symbol, desc, writable, availableInMode)));
		}
		else if (0 == dataType.compare("U8"))
		{
			m_Interfaces.insert(std::make_pair(uidStr, InterfaceWrapper::MakeInterfaceWrapper<uint32_t>(
				m_Modbus, registerAddress, symbol, desc, writable, availableInMode)));
		}
		else if (0 == dataType.compare("I32"))
		{
			m_Interfaces.insert(std::make_pair(uidStr, InterfaceWrapper::MakeInterfaceWrapper<int32_t>(
				m_Modbus, registerAddress, symbol, desc, writable, availableInMode)));
		}
		else if (0 == dataType.compare("I16"))
		{
			m_Interfaces.insert(std::make_pair(uidStr, InterfaceWrapper::MakeInterfaceWrapper<int16_t>(
				m_Modbus, registerAddress, symbol, desc, writable, availableInMode)));
		}
		else if (0 == dataType.compare("I8"))
		{
			m_Interfaces.insert(std::make_pair(uidStr, InterfaceWrapper::MakeInterfaceWrapper<int8_t>(
				m_Modbus, registerAddress, symbol, desc, writable, availableInMode)));
		}
		else if (0 == dataType.compare("F32"))
		{
			m_Interfaces.insert(std::make_pair(uidStr, InterfaceWrapper::MakeInterfaceWrapper<float>(
				m_Modbus, registerAddress, symbol, desc, writable, availableInMode)));
		}
		else
		{
			throw std::invalid_argument("Invalid type");
		}
	}
}

