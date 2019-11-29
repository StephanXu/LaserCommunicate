#ifndef SERIALCONTROLLER_HPP
#define SERIALCONTROLLER_HPP

#include <cpprest/http_listener.h>
#include <cpprest/http_msg.h>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

#include "Modbus.hpp"

#include <iostream>
#include <iterator>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <typeindex>
#include <functional>
#include <unordered_map>

using namespace std;
using namespace web;
using namespace http;
using namespace utility;
using namespace http::experimental::listener;

class InterfaceWrapper
{
public:
	InterfaceWrapper(unsigned int registerAddress,
					 std::string symbol,
					 std::string describe,
					 bool writable,
					 unsigned int availableInMode,
					 size_t dataTypeHash)
		: m_RegisterAddress(registerAddress)
		, m_Symbol(symbol)
		, m_Describe(describe)
		, m_Writable(writable)
		, m_AvailableInMode(availableInMode)
		, m_DataTypeHash(dataTypeHash)
	{
	}

	InterfaceWrapper(const InterfaceWrapper& wrapper) = default;

	//template<typename T>
	//void SetDataType()
	//{
	//	m_DataTypeHash(std::type_index(typeid(T).hash_code()));
	//}

	unsigned int m_RegisterAddress;
	std::string m_Symbol;
	std::string m_Describe;
	bool m_Writable;
	unsigned int m_AvailableInMode;
	size_t m_DataTypeHash;

	//class IInterface
	//{
	//public:
	//	virtual std::type_index DataType() = 0;
	//};

	//template<typename T>
	//class InterfaceImpl :public IInterface
	//{
	//public:
	//	//InterfaceImpl(UartModbus* instance,
	//	//			  unsigned int registerAddress,
	//	//			  bool writable)
	//	//	: m_ReadValueFunc(std::bind(&UartModbus::ReadRegisters<T>, instance, registerAddress))
	//	//	, m_WriteValueFunc(std::bind(&UartModbus::WriteRegisters<T>, instance, registerAddress, std::placeholders::_2))
	//	//	, m_Writable(writable)
	//	//{
	//	//}
	//	//T Read() { return m_ReadValueFunc(); }
	//	//void Write(const T value) { if (m_Writable) m_WriteValueFunc(value); }
	//	
	//	std::type_index DataType() override { return std::type_index(typeid(T)); }
	//private:
	//	/*bool m_Writable;
	//	std::function<T()> m_ReadValueFunc;
	//	std::function<void(const T)> m_WriteValueFunc;*/
	//};

	//std::unique_ptr<IInterface> m_IOPort;
};

class SerialController
{
public:
	SerialController()
	{
	}
	SerialController(utility::string_t url) :m_Listener(url)
	{
		boost::property_tree::ptree config;
		ifstream f("Config.json");
		if (!f.is_open())
		{
			throw std::runtime_error("Can't open config file.");
		}
		boost::property_tree::read_json(f, config);
		for (auto&& item : config.get_child("actions"))
		{
			std::string dataType = item.second.get<std::string>("dataType");
			size_t dataTypeHash = 0;
			if (0 == dataType.compare("U32"))
				dataTypeHash = std::type_index(typeid(uint32_t)).hash_code();
			else if (0 == dataType.compare("U16"))
				dataTypeHash = std::type_index(typeid(uint16_t)).hash_code();
			else if (0 == dataType.compare("U8"))
				dataTypeHash = std::type_index(typeid(uint8_t)).hash_code();
			else if (0 == dataType.compare("I32"))
				dataTypeHash = std::type_index(typeid(int32_t)).hash_code();
			else if (0 == dataType.compare("I16"))
				dataTypeHash = std::type_index(typeid(int8_t)).hash_code();
			else if (0 == dataType.compare("I8"))
				dataTypeHash = std::type_index(typeid(int8_t)).hash_code();
			else if (0 == dataType.compare("F32"))
				dataTypeHash = std::type_index(typeid(float)).hash_code();

			std::string uidStr{};
			do
			{
				boost::uuids::uuid uid = boost::uuids::random_generator()();
				uidStr = boost::uuids::to_string(uid);
			} while (m_Interfaces.end() != m_Interfaces.find(uidStr));
			m_Interfaces.insert(std::make_pair(
				uidStr,
				InterfaceWrapper(strtol(item.second.get<std::string>("register").c_str(), NULL, 16),
								 item.second.get<std::string>("symbol"),
								 item.second.get<std::string>("desc"),
								 item.second.get<bool>("writable"),
								 item.second.get<bool>("availableInMode"), dataTypeHash)));
		}


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
		message.extract_string().then(
			[=](pplx::task<string_t> t) {
				try
				{
					boost::property_tree::ptree root;
					std::stringstream ss;
					ss << utility::conversions::to_utf8string(t.get());
					
					boost::property_tree::read_json(ss, root);

					message.reply(status_codes::OK, root.get<std::string>("id"), "text");
				}
				catch (...)
				{
					//...
				}
			}
		);
	}

	std::unordered_map<std::string, InterfaceWrapper> m_Interfaces;

	http_listener m_Listener;
};
#endif //SERIALCONTROLLER_HPP