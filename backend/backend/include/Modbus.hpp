
#ifndef MODBUS_HPP
#define MODBUS_HPP

#include <modbus.h>
#include <fmt/format.h>

#include <string>
#include <functional>
#include <memory>
/**
 * @class	UartModbus
 *
 * @brief	串口Modbus协议封装实现（非线程安全）
 *
 * @author	Xu Zihan
 * @date	2019/11/29
 */
class UartModbus
{
public:
	/**
	 * @enum	Parity
	 *
	 * @brief	校验方式（用于构造函数或Open函数）
	 */
	enum Parity
	{
		NoPariDty = 'N', ///< 无校验
		OddParity = 'O', ///< 奇校验
		EvenParity = 'E',///< 偶校验
	};

	/**
	 * @enum	StopBit
	 *
	 * @brief	停止位（用于构造函数或Open函数）
	 */
	enum StopBit
	{
		OneStopBit = 0, ///< 1位停止位
		TwoStopBits = 2 ///< 2位停止位
	};

	/**
	 * @enum	FloatByteOrder
	 *
	 * @brief	浮点数字节顺序
	 */
	enum class FloatByteOrder
	{
		DCBA,
		ABCD,
		BADC,
		CDAB
	};

	/**
	 * @class	ConnectError
	 *
	 * @brief	A connect error.
	 *
	 * @author	Xu Zihan
	 * @date	2019/11/29
	 */
	class ConnectError : public std::runtime_error
	{
	public:
		ConnectError(const std::string& action, int errnum)
			: std::runtime_error(fmt::format("{} failed: {}", action, modbus_strerror(errnum)))
			, m_ErrNum(errnum)
		{
		}
	private:
		int m_ErrNum;
	};

	/**
	 * @class	IOError
	 *
	 * @brief	An i/o error.
	 *
	 * @author	Xu Zihan
	 * @date	2019/11/29
	 */
	class IOError :public std::runtime_error
	{
	public:
		enum class IODirection
		{
			In,
			Out
		};

		IOError(IODirection direction, unsigned int registerAddress, int errnum)
			:std::runtime_error(fmt::format("Modbus IO fail: {} [{}] {}",
											direction == IODirection::In ? "In" : "Out",
											registerAddress,
											modbus_strerror(errnum)))
			, m_RegisterAddress(registerAddress)
			, m_Direction(direction)
			, m_ErrNum(errnum)
		{
		}
		unsigned int RegisterAddress() { return m_RegisterAddress; }
		IODirection Direction() { return m_Direction; }
		int ErrNum() { return m_ErrNum; }
	private:
		unsigned int m_RegisterAddress;
		IODirection m_Direction;
		int m_ErrNum;
	};

	/**
	 * @fn	UartModbus::UartModbus(const std::string device, int baudrate, Parity parity, int dataBit, StopBit stopBit, int slaveId, FloatByteOrder floatByteOrder = FloatByteOrder::DCBA)
	 *
	 * @brief	Constructor
	 *
	 * @author	Xu Zihan
	 * @date	2019/11/29
	 *
	 * @param	device		  	portname(串口名): 在Windows下是"COM1""COM2"等，在Linux下是"/dev/ttyS1"等.
	 * @param	baudrate	  	baudrate(波特率): 9600、19200、38400、43000、56000、57600、115200.
	 * @param	parity		  	parity(校验位): 0为无校验，1为奇校验，2为偶校验，3为标记校验(仅适用于windows)
	 * @param	dataBit		  	databit(数据位): 4-8(windows),5-8(linux)，通常为8位.
	 * @param	stopBit		  	stopbit(停止位): 1为1位停止位，2为2位停止位,3为1.5位停止位.
	 * @param	slaveId		  	Identifier for the slave.
	 * @param	floatByteOrder	(Optional) 浮点字节数顺序.
	 */
	UartModbus(const std::string device,
			   int baudrate,
			   Parity parity,
			   int dataBit,
			   StopBit stopBit,
			   int slaveId,
			   FloatByteOrder floatByteOrder = FloatByteOrder::DCBA)
	{
		SetFloatByteOrder(floatByteOrder);
		Open(device, baudrate, parity, dataBit, stopBit, slaveId);
	}

	/**
	 * @fn	UartModbus::UartModbus(FloatByteOrder floatByteOrder = FloatByteOrder::DCBA)
	 *
	 * @brief	Constructor
	 *
	 * @author	Xu Zihan
	 * @date	2019/11/29
	 *
	 * @param	floatByteOrder	(Optional) 浮点字节数顺序.
	 */
	UartModbus(FloatByteOrder floatByteOrder = FloatByteOrder::DCBA) noexcept
	{
		SetFloatByteOrder(floatByteOrder);
	}

	UartModbus(const UartModbus& uartModbus) = delete;
	UartModbus& operator=(UartModbus& uartModbus) = delete;
	UartModbus(UartModbus&& uartModbus)noexcept
	{
		*this = std::move(uartModbus);
	}
	UartModbus& operator=(UartModbus&& uartModbus)noexcept
	{
		m_Connection = uartModbus.m_Connection;
		m_IsValid = uartModbus.m_IsValid;
		m_GetFloatFunc = uartModbus.m_GetFloatFunc;
		m_SetFloatFunc = uartModbus.m_SetFloatFunc;
		uartModbus.m_Connection = nullptr;
		uartModbus.m_IsValid = false;
	}

	~UartModbus()
	{
		Close();
	}

	/**
	 * @fn	bool UartModbus::Open(const std::string device, int baudrate, Parity parity, int dataBit, StopBit stopBit, int slaveId)
	 *
	 * @brief	建立连接,成功返回true，失败返回false
	 *
	 * @author	Xu Zihan
	 * @date	2019/11/29
	 *
	 * @exception	ModbusConnectError	Raised when the Modbus Connect error condition occurs.
	 * @exception	std::runtime_error	Raised when a runtime error condition occurs.
	 *
	 * @param	device  	portname(串口名): 在Windows下是"COM1""COM2"等，在Linux下是"/dev/ttyS1"等.
	 * @param	baudrate	baudrate(波特率): 9600、19200、38400、43000、56000、57600、115200.
	 * @param	parity  	parity(校验位): 0为无校验，1为奇校验，2为偶校验，3为标记校验(仅适用于windows)
	 * @param	dataBit 	databit(数据位): 4-8(windows),5-8(linux)，通常为8位.
	 * @param	stopBit 	stopbit(停止位): 1为1位停止位，2为2位停止位,3为1.5位停止位.
	 * @param	slaveId 	Identifier for the slave.
	 *
	 * @returns	True if it succeeds, false if it fails.
	 */
	bool Open(const std::string device,
			  int baudrate,
			  Parity parity,
			  int dataBit,
			  StopBit stopBit,
			  int slaveId)
	{
		m_Connection = modbus_new_rtu(device.c_str(), baudrate, parity, dataBit, stopBit);
		if (!m_Connection)
		{
			m_IsValid = false;
			throw ConnectError("Create structure", errno);
		}
		if (-1 == modbus_set_slave(m_Connection, slaveId))
		{
			modbus_free(m_Connection);
			m_Connection = nullptr;
			throw ConnectError("Set slave", errno);
		}
		if (-1 == modbus_connect(m_Connection))
		{
			modbus_free(m_Connection);
			m_Connection = nullptr;
			throw ConnectError("Establish connection", errno);
		}
		m_IsValid = true;
	}

	/**
	 * @fn	void UartModbus::Close()
	 *
	 * @brief	关闭连接（如存在）并释放资源（如存在）
	 *
	 * @author	Xu Zihan
	 * @date	2019/11/30
	 */
	void Close()
	{
		if (m_IsValid)
		{
			modbus_close(m_Connection);
			m_IsValid = false;
		}
		if (m_Connection)
		{
			modbus_free(m_Connection);
			m_Connection = nullptr;
		}
	}

	/**
	 * @fn	void UartModbus::SetFloatByteOrder(FloatByteOrder floatByteOrder) noexcept
	 *
	 * @brief	设置浮点数字节顺序
	 *
	 * @author	Xu Zihan
	 * @date	2019/11/29
	 *
	 * @param	floatByteOrder	浮点数字节顺序.
	 */
	void SetFloatByteOrder(FloatByteOrder floatByteOrder) noexcept
	{
		switch (floatByteOrder)
		{
		case FloatByteOrder::DCBA:
			m_GetFloatFunc = modbus_get_float_dcba;
			m_SetFloatFunc = modbus_set_float_dcba;
			break;
		case FloatByteOrder::ABCD:
			m_GetFloatFunc = modbus_get_float_abcd;
			m_SetFloatFunc = modbus_set_float_abcd;
			break;
		case FloatByteOrder::BADC:
			m_GetFloatFunc = modbus_get_float_badc;
			m_SetFloatFunc = modbus_set_float_badc;
			break;
		case FloatByteOrder::CDAB:
			m_GetFloatFunc = modbus_get_float_cdab;
			m_SetFloatFunc = modbus_set_float_cdab;
			break;
		}
	}

	/**
	 * @fn	template<typename T> T UartModbus::ReadRegisters(const unsigned int registerAddress)
	 *
	 * @brief	读多个16位寄存器（取决于返回类型）
	 *
	 * @exception	IOError	Raised when an I/O error condition occurs.
	 *
	 * @tparam	T	返回类型（借此推导读寄存器的数量）.
	 * @param	registerAddress	寄存器地址.
	 *
	 * @returns	寄存器值.
	 */
	template<typename T>
	T ReadRegisters(const unsigned int registerAddress)
	{
		if (!m_IsValid || !m_Connection)
			return 0;
		constexpr size_t bufLength = sizeof(T) / sizeof(uint16_t) < 1 ?
			1 : sizeof(T) / sizeof(uint16_t);
		uint16_t buf[bufLength]{};
		if (-1 == modbus_read_registers(m_Connection, registerAddress, bufLength, buf))
		{
			throw IOError(IOError::IODirection::In, registerAddress, errno);
		}
		return GetNumFromBuffer<T, uint16_t>(buf, bufLength);
	}

	/**
	 * @fn	template<typename T> void UartModbus::WriteRegisters(unsigned int registerAddress, T value)
	 *
	 * @brief	写多个16位寄存器（取决于写入类型）
	 *
	 * @exception	IOError	Raised when an I/O error condition occurs.
	 *
	 * @tparam	T	写入类型（借此推导写寄存器的数量）.
	 * @param	registerAddress	寄存器地址.
	 * @param	value		   	将写入的值.
	 */
	template<typename T>
	void WriteRegisters(unsigned int registerAddress, T value)
	{
		if (!m_IsValid || !m_Connection)
			return;
		constexpr size_t bufLength = sizeof(T) / sizeof(uint16_t) < 1 ?
			1 : sizeof(T) / sizeof(uint16_t);
		uint16_t buf[bufLength]{};
		if (-1 == modbus_write_registers(m_Connection, registerAddress, bufLength, buf))
		{
			throw IOError(IOError::IODirection::Out, registerAddress, errno);
		}
	}

private:
	bool m_IsValid = false;
	modbus_t* m_Connection = nullptr;
	std::function<float(const uint16_t*)> m_GetFloatFunc;
	std::function<void(float, uint16_t*)> m_SetFloatFunc;

	/**
	 * @fn	template<typename RetT, typename BufferType> inline RetT UartModbus::GetNumFromBuffer(const BufferType* buffer, size_t length)
	 *
	 * @brief	从缓冲区中读取数据（小端） 
	 * 			例如uint8_t的缓冲区{0x01,0x02,0x03,0x04}读取到uint32_t数据中应为0x04030201
	 *
	 * @tparam	RetT	  	返回类型.
	 * @tparam	BufferType	缓冲区类型.
	 * @param	buffer	The 缓冲区.
	 * @param	length	The 缓冲区长度.
	 *
	 * @returns	从缓冲区中读取的值.
	 */
	template<typename RetT, typename BufferType>
	inline RetT GetNumFromBuffer(const BufferType* buffer, size_t length)
	{
		RetT result = 0;
		for (int i{}; i < sizeof(RetT) && i < length; ++i)
		{
			result += (buffer[i] << (sizeof(BufferType) * 8 * i));
		}
		return result;
	}

	/**
	 * @fn	template<> inline float UartModbus::GetNumFromBuffer(const uint16_t* buffer, size_t length)
	 *
	 * @brief	从缓冲区中读取数据（小端）（浮点特化）
	 *
	 * @param	buffer	缓冲区.
	 * @param	length	缓冲区大小.
	 *
	 * @returns	从缓冲区中读取的值.
	 */
	template<>
	inline float GetNumFromBuffer(const uint16_t* buffer, size_t length)
	{
		return m_GetFloatFunc(buffer);
	}

	/**
	 * @fn	template<typename ValueType, typename BufferType> inline void UartModbus::SetNumToBuffer(BufferType* buffer, size_t length, const ValueType value)
	 *
	 * @brief	将数据存入缓冲区（小端） 
	 * 			例如uint32_t数据0x04030201存储到uint8_t缓冲区应为{0x01,0x02,0x03,0x04}
	 *
	 * @tparam	ValueType 	数据类型.
	 * @tparam	BufferType	缓冲区类型.
	 * @param [out]	buffer	缓冲区.
	 * @param 	   	length	缓冲区长度.
	 * @param 	   	value 	要存储到缓冲区的数值.
	 */
	template<typename ValueType, typename BufferType>
	inline void SetNumToBuffer(BufferType* buffer, size_t length, const ValueType value)
	{
		for (int i{}; i < sizeof(ValueType) && i < length; ++i)
		{
			buffer[i] = value >> (i * 8);
		}
	}

	/**
	 * @fn	template<> inline void UartModbus::SetNumToBuffer(uint16_t* buffer, size_t length, float value)
	 *
	 * @brief	将数据存入缓冲区（小端）（浮点特化）
	 *
	 * @param [out]	buffer	缓冲区.
	 * @param 	   	length	缓冲区长度.
	 * @param 	   	value 	要存储到缓冲区的数值.
	 */
	template<>
	inline void SetNumToBuffer(uint16_t* buffer, size_t length, float value)
	{
		m_SetFloatFunc(value, buffer);
	}

};

#endif // MODBUS_HPP