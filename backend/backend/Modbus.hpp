
#ifndef MODBUS_HPP
#define MODBUS_HPP

#include <modbus.h>
#include <fmt/format.h>

#include <string>
#include <functional>

/**
 * @class	UartModbus
 *
 * @brief	����ModbusЭ���װʵ�֣����̰߳�ȫ��
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
	 * @brief	У�鷽ʽ�����ڹ��캯����Open������
	 */
	enum Parity
	{
		NoPariDty = 'N', ///< ��У��
		OddParity = 'O', ///< ��У��
		EvenParity = 'E',///< żУ��
	};

	/**
	 * @enum	StopBit
	 *
	 * @brief	ֹͣλ�����ڹ��캯����Open������
	 */
	enum StopBit
	{
		OneStopBit = 0, ///< 1λֹͣλ
		TwoStopBits = 2 ///< 2λֹͣλ
	};

	/**
	 * @enum	FloatByteOrder
	 *
	 * @brief	�������ֽ�˳��
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
	 * @param	device		  	portname(������): ��Windows����"COM1""COM2"�ȣ���Linux����"/dev/ttyS1"��.
	 * @param	baudrate	  	baudrate(������): 9600��19200��38400��43000��56000��57600��115200.
	 * @param	parity		  	parity(У��λ): 0Ϊ��У�飬1Ϊ��У�飬2ΪżУ�飬3Ϊ���У��(��������windows)
	 * @param	dataBit		  	databit(����λ): 4-8(windows),5-8(linux)��ͨ��Ϊ8λ.
	 * @param	stopBit		  	stopbit(ֹͣλ): 1Ϊ1λֹͣλ��2Ϊ2λֹͣλ,3Ϊ1.5λֹͣλ.
	 * @param	slaveId		  	Identifier for the slave.
	 * @param	floatByteOrder	(Optional) �����ֽ���˳��.
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
	 * @param	floatByteOrder	(Optional) �����ֽ���˳��.
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
		if (m_IsValid)
		{
			modbus_close(m_Connection);
		}
		if (m_Connection)
		{
			modbus_free(m_Connection);
		}
	}

	/**
	 * @fn	bool UartModbus::Open(const std::string device, int baudrate, Parity parity, int dataBit, StopBit stopBit, int slaveId)
	 *
	 * @brief	��������,�ɹ�����true��ʧ�ܷ���false
	 *
	 * @author	Xu Zihan
	 * @date	2019/11/29
	 *
	 * @exception	ModbusConnectError	Raised when the Modbus Connect error condition occurs.
	 * @exception	std::runtime_error	Raised when a runtime error condition occurs.
	 *
	 * @param	device  	portname(������): ��Windows����"COM1""COM2"�ȣ���Linux����"/dev/ttyS1"��.
	 * @param	baudrate	baudrate(������): 9600��19200��38400��43000��56000��57600��115200.
	 * @param	parity  	parity(У��λ): 0Ϊ��У�飬1Ϊ��У�飬2ΪżУ�飬3Ϊ���У��(��������windows)
	 * @param	dataBit 	databit(����λ): 4-8(windows),5-8(linux)��ͨ��Ϊ8λ.
	 * @param	stopBit 	stopbit(ֹͣλ): 1Ϊ1λֹͣλ��2Ϊ2λֹͣλ,3Ϊ1.5λֹͣλ.
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
	 * @fn	void UartModbus::SetFloatByteOrder(FloatByteOrder floatByteOrder) noexcept
	 *
	 * @brief	���ø������ֽ�˳��
	 *
	 * @author	Xu Zihan
	 * @date	2019/11/29
	 *
	 * @param	floatByteOrder	�������ֽ�˳��.
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
	 * @brief	�����16λ�Ĵ�����ȡ���ڷ������ͣ�
	 *
	 * @exception	IOError	Raised when an I/O error condition occurs.
	 *
	 * @tparam	T	�������ͣ�����Ƶ����Ĵ�����������.
	 * @param	registerAddress	�Ĵ�����ַ.
	 *
	 * @returns	�Ĵ���ֵ.
	 */
	template<typename T>
	T ReadRegisters(const unsigned int registerAddress)
	{
		if (!m_IsValid || !m_Connection)
			return 0;
		constexpr size_t bufLength = sizeof(T) / sizeof(uint16_t);
		uint16_t buf[bufLength]{};
		if (-1 == modbus_read_registers(m_Connection, registerAddress, bufLength, buf))
		{
			throw IOError(IOError::IODirection::In, registerAddress, errno);
		}
		return GetNumFromBuffer<T, uint16_t>(buf, bufLength);
	}

	/**
	 * @fn	template<typename T> void UartModbus::WriteRegisters(unsigned int registerAddress, const T value)
	 *
	 * @brief	д���16λ�Ĵ�����ȡ����д�����ͣ�
	 *
	 * @exception	IOError	Raised when an I/O error condition occurs.
	 *
	 * @tparam	T	д�����ͣ�����Ƶ�д�Ĵ�����������.
	 * @param	registerAddress	�Ĵ�����ַ.
	 * @param	value		   	��д���ֵ.
	 */
	template<typename T>
	void WriteRegisters(unsigned int registerAddress, const T value)
	{
		if (!m_IsValid || !m_Connection)
			return 0;
		constexpr size_t bufLength = sizeof(T) / sizeof(uint16_t);
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
	 * @brief	�ӻ������ж�ȡ���ݣ�С�ˣ� 
	 * 			����uint8_t�Ļ�����{0x01,0x02,0x03,0x04}��ȡ��uint32_t������ӦΪ0x04030201
	 *
	 * @tparam	RetT	  	��������.
	 * @tparam	BufferType	����������.
	 * @param	buffer	The ������.
	 * @param	length	The ����������.
	 *
	 * @returns	�ӻ������ж�ȡ��ֵ.
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
	 * @brief	�ӻ������ж�ȡ���ݣ�С�ˣ��������ػ���
	 *
	 * @param	buffer	������.
	 * @param	length	��������С.
	 *
	 * @returns	�ӻ������ж�ȡ��ֵ.
	 */
	template<>
	inline float GetNumFromBuffer(const uint16_t* buffer, size_t length)
	{
		return m_GetFloatFunc(buffer);
	}

	/**
	 * @fn	template<typename ValueType, typename BufferType> inline void UartModbus::SetNumToBuffer(BufferType* buffer, size_t length, const ValueType value)
	 *
	 * @brief	�����ݴ��뻺������С�ˣ� 
	 * 			����uint32_t����0x04030201�洢��uint8_t������ӦΪ{0x01,0x02,0x03,0x04}
	 *
	 * @tparam	ValueType 	��������.
	 * @tparam	BufferType	����������.
	 * @param [out]	buffer	������.
	 * @param 	   	length	����������.
	 * @param 	   	value 	Ҫ�洢������������ֵ.
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
	 * @brief	�����ݴ��뻺������С�ˣ��������ػ���
	 *
	 * @param [out]	buffer	������.
	 * @param 	   	length	����������.
	 * @param 	   	value 	Ҫ�洢������������ֵ.
	 */
	template<>
	inline void SetNumToBuffer(uint16_t* buffer, size_t length, float value)
	{
		m_SetFloatFunc(value, buffer);
	}

};

#endif // MODBUS_HPP