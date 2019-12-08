#include "backend.h"
#include "InterfaceController.hpp"
#include "modbus.h"
#include "utils.hpp"
#include "ConfigurationFileContent.hpp"

#include <cpprest/http_msg.h>
#include <cpprest/uri.h>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/exception/all.hpp>

#include <fstream>
#include <sstream>
#include <typeindex>

InterfaceController::InterfaceController(utility::string_t url, UartModbus* uartModbus)
	:m_Listener(url)
	, m_Modbus(uartModbus)
{
	LoadConfiguration();
	m_Listener.support(methods::GET, std::bind(&InterfaceController::Get, this, std::placeholders::_1));
	m_Listener.support(methods::POST, std::bind(&InterfaceController::Post, this, std::placeholders::_1));
}

InterfaceController::~InterfaceController()
{
}

void InterfaceController::Get(http_request message)
{
	try
	{	
		std::map<string_t, string_t> queryParams = uri::split_query(message.relative_uri().query());
		unsigned int modeFilter = 0;
		auto modeFilterQueryIter = queryParams.find(L"mode");
		if (queryParams.end() != modeFilterQueryIter)
		{
			modeFilter = strtol(conversions::to_utf8string(modeFilterQueryIter->second).c_str(), NULL, 10);
		}
		std::vector<string_t> paths = uri::split_path(message.relative_uri().path());
		if (paths.size() < 1)
		{
			// request all list
			boost::property_tree::ptree root;
			boost::property_tree::ptree actions;
			for (auto&& item : m_Interfaces)
			{
				if ((0 != item.second->AvailableInMode()) &&
					(modeFilter != item.second->AvailableInMode()) &&
					(0 != modeFilter))
				{
					continue;
				}
				boost::property_tree::ptree action;
				action.put("id", item.first);
				action.put("symbol", item.second->Symbol());
				action.put("desc", item.second->Describe());
				action.put("writable", item.second->Writable());
				actions.push_back(std::make_pair("", action));
			}
			root.add_child("action", actions);
			std::ostringstream ss;
			boost::property_tree::write_json(ss, root, false);
			message.reply(MakeResponseWithCORSHeader(status_codes::OK, ss.str()));
			return;
		}
		if (!paths[0].compare(L"all"))
		{
			boost::property_tree::ptree root;
			boost::property_tree::ptree content;
			m_Modbus->RefreshCache();
			for (auto&& item : m_Interfaces)
			{
				if ((0 != item.second->AvailableInMode()) &&
					(modeFilter != item.second->AvailableInMode()) &&
					(0 != modeFilter))
				{
					continue;
				}
				boost::property_tree::ptree value;
				value.put("id", item.first);
				value.put("symbol", item.second->Symbol());
				value.put("desc", item.second->Describe());
				value.put("writable", item.second->Writable());
				value.put("data", item.second->Read(true));
				content.push_back(std::make_pair("", value));
			}
			root.add_child("content", content);
			std::ostringstream ss;
			boost::property_tree::write_json(ss, root, false);
			message.reply(MakeResponseWithCORSHeader(status_codes::OK, ss.str()));
			return;
		}
		std::string interfaceId = utility::conversions::to_utf8string(paths[0]);
		auto interfaceIter = m_Interfaces.find(interfaceId);
		if (m_Interfaces.end() == interfaceIter)
		{
			// interface do not exist
			message.reply(MakeErrorResponse(status_codes::NotFound, "Interface not found"));
			return;
		}
		std::string result = interfaceIter->second->Read();
		message.reply(MakeSingleValueResponse(status_codes::OK, "content", result));
	}
	catch (std::runtime_error & e)
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

void InterfaceController::Post(http_request message)
{
	std::vector<string_t> paths = uri::split_path(message.relative_uri().path());
	if (paths.size() < 1)
	{
		// request all list
		message.reply(MakeErrorResponse(status_codes::BadRequest, "Can't modify interface root"));
		return;
	}
	std::string interfaceToModify = conversions::to_utf8string(paths[0]);
	message.extract_utf8string().then(
		[=](pplx::task<std::string> t) {
			try
			{
				boost::property_tree::ptree root;
				std::istringstream ss(t.get());
				boost::property_tree::read_json(ss, root);
				std::string valueToModify = root.get<std::string>("value");
				auto interfaceIter = m_Interfaces.find(interfaceToModify);
				if (m_Interfaces.end() == interfaceIter)
				{
					message.reply(MakeErrorResponse(status_codes::NotFound, "Interface not found"));
					return;
				}
				if (!interfaceIter->second->Writable())
				{
					message.reply(MakeErrorResponse(status_codes::BadRequest, "Interface is not writable"));
					return;
				}
				interfaceIter->second->Write(valueToModify);
				message.reply(MakeSingleValueResponse(status_codes::OK, "ret", "ok"));
			}
			catch (boost::property_tree::ptree_bad_path & e)
			{
				message.reply(MakeErrorResponse(status_codes::BadRequest, "Invalid argument"));
			}
			catch (...)
			{
				message.reply(MakeErrorResponse(status_codes::InternalError, 
												boost::current_exception_diagnostic_information()));
			}
		}
	);
}

void InterfaceController::LoadConfiguration()
{
	m_Modbus->CreateCachePage(0x0100, 0x023f); //user config
	m_Modbus->CreateCachePage(0x1000, 0x008f); //vender config
	m_Modbus->CreateCachePage(0x3000, 0x0025); //status config
	m_Modbus->CreateCachePage(0x3200, 0x01ff); //temperature records
	m_Modbus->CreateCachePage(0x3800, 0x01ff); //voltage records

	boost::property_tree::ptree config;
	std::string configContent(reinterpret_cast<char*>(configJson), configJsonLen);
	std::istringstream configContentStream(configContent);
	/*std::ifstream f("Config.json");
	if (!f.is_open())
	{
		throw std::runtime_error("Can't open config file.");
	}*/
	boost::property_tree::read_json(configContentStream, config);
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

