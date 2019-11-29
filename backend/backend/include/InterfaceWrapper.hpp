#ifndef INTERFACE_WRAPPER_HPP
#define INTERFACE_WRAPPER_HPP

#include <fmt/format.h>

#include "Modbus.hpp"

#include <typeindex>
#include <functional>

class InterfaceWrapper
{
public:
	template<typename T>
	static std::unique_ptr<InterfaceWrapper> MakeInterfaceWrapper(UartModbus* instance,
																  unsigned int registerAddress,
																  std::string symbol,
																  std::string describe,
																  bool writable,
																  unsigned int availableInMode)
	{
		std::unique_ptr<InterfaceWrapper> ret(new InterfaceWrapper(registerAddress,
																   symbol,
																   describe,
																   writable,
																   availableInMode));
		ret->m_IOPort.reset(new InterfaceImpl<T>(instance, registerAddress, writable));
		return ret;
	}

	InterfaceWrapper(const InterfaceWrapper& wrapper) = default;

	std::string Read() const
	{
		if (!m_IOPort)
			throw std::runtime_error("Empty IOPort");
		return m_IOPort->Read();
	}
	std::string Write(const std::string& value)
	{
		if (!m_IOPort)
			throw std::runtime_error("Empty IOPort");
		m_IOPort->Write(value);
	}

	unsigned int RegisterAddress() const noexcept { return m_RegisterAddress; }
	std::string Symbol() const noexcept { return m_Symbol; }
	std::string Describe() const noexcept { return m_Describe; }
	bool Writable() const noexcept { return m_Writable; }
	unsigned int AvailableInMode()const noexcept { return m_AvailableInMode; }

private:
	InterfaceWrapper(unsigned int registerAddress,
					 std::string symbol,
					 std::string describe,
					 bool writable,
					 unsigned int availableInMode) noexcept
		: m_RegisterAddress(registerAddress)
		, m_Symbol(symbol)
		, m_Describe(describe)
		, m_Writable(writable)
		, m_AvailableInMode(availableInMode)
	{
	}
private:

	unsigned int m_RegisterAddress;
	std::string m_Symbol;
	std::string m_Describe;
	bool m_Writable;
	unsigned int m_AvailableInMode;

	class IInterface
	{
	public:
		virtual std::type_index DataType() = 0;
		virtual std::string Read() = 0;
		virtual void Write(const std::string& value) = 0;
	};

	template<typename T>
	class InterfaceImpl :public IInterface
	{
	public:
		InterfaceImpl(UartModbus* instance,
					  unsigned int registerAddress,
					  bool writable)
			: m_Writable(writable)
		{
			m_ReadValueFunc = [instance, registerAddress]() {
				return instance->ReadRegisters<T>(registerAddress);
			};
			m_WriteValueFunc = [instance, registerAddress, writable](T value) {
				instance->WriteRegisters<T>(registerAddress, value);
			};
		}
		std::string Read() override
		{
			return fmt::format("{}", m_ReadValueFunc());
		}

		template<typename __Ty>
		struct StringConverter
		{
			static T StringToNum(const std::string value) { return strtol(value.c_str(), NULL, 10); }
		};

		template<>
		struct StringConverter <float>
		{
			static float StringToNum(const std::string value) { return strtof(value.c_str(), NULL); }
		};

		void Write(const std::string& value)override
		{
			if (m_Writable)
			{
				//m_WriteValueFunc(StringConverter<T>::StringToNum(value);
			}
		}

		std::type_index DataType() override { return std::type_index(typeid(T)); }
	private:
		bool m_Writable;
		std::function<T()> m_ReadValueFunc;
		std::function<void(T)> m_WriteValueFunc;
	};

	std::unique_ptr<IInterface> m_IOPort;
};

#endif // INTERFACE_WRAPPER_HPP