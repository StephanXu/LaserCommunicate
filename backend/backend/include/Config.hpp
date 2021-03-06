﻿/**
 * @file Config.hpp
 * @author Xu Zihan (stephanxu@foxmail.com)
 * @brief 配置器的相关实现
 * @version 0.1
 * @date 2019-10-25
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include <string>

#include "DI.hpp"
#include "Dispatcher.hpp"

/**
 * @fn	template<typename T> std::unique_ptr<T> CreateGeneralObject(Utils::Configuration* config)
 *
 * @brief	用于仅接收一个配置依赖项的类型的通用工厂函数
 *
 * @tparam	T	Generic type parameter.
 * @param [in]	config	配置依赖项.
 *
 * @returns	创建的对象
 */
template<typename T>
std::unique_ptr<T> CreateGeneralObject(Utils::Configuration* config)
{
	return std::make_unique<T>(config);
}


/**
 * @brief 配置器实现
 * 通过此类进行配置并生成 Dispatcher 对象
 */
class ApplicationBuilder
{
public:
    ApplicationBuilder() = default;
    ApplicationBuilder(const ApplicationBuilder &dispatcher) = delete;
    ApplicationBuilder(const ApplicationBuilder &&dispatcher) = delete;

    /**
     * @brief 注册一个状态对象
     * @tparam StatusType 状态类型
     */
    template <typename StatusType>
    void RegisterStatusType()
    {
        m_DIConfig.Add(CreateStatus<StatusType>);
    }

    /**
     * @brief 注册一项输入服务
     * @tparam ServiceType 服务类型
     */
    template <typename InputAdapterType>
    void RegisterInputAdapter()
    {
        m_DIConfig.Add(CreateGeneralService<InputAdapterType>);
        m_InputAdapterRealizer.emplace_back([](DI::Injector &injector) { return injector.GetInstance<InputAdapterType>(); });
    }

	/**
	 * @brief 注册一项服务
	 * @tparam ServiceType 服务类型
	 */
	template <typename ServiceType>
	void RegisterService()
	{
		m_DIConfig.Add(CreateGeneralService<ServiceType>);
	}

    /**
     * @brief 注册一条管道
     * 注意：目前仅支持注册一条管道
     * @tparam StatusType 状态类型
     * @tparam InputType 输入类型
     * @tparam ActionTypes 动作类型
     */
    template <typename StatusType, typename InputType, typename... ActionTypes>
    void RegisterPipeline()
    {
        m_DIConfig.Add(CreatePipeline<StatusType, ActionTypes...>);
        m_PipelineRealizer.emplace_back([](DI::Injector &injector) {
            return std::make_tuple(std::type_index(typeid(InputType)),
                                   injector.GetInstance<Pipeline>());
        });
    }

	/**
	 * @fn	void ApplicationBuilder::RegisterConfiguration()
	 *
	 * @brief	Registers the configuration
	 *
	 * @author	Xu Zihan
	 * @date	2019/11/21
	 */
	void RegisterConfiguration()
	{
		m_DIConfig.Add(Utils::CreateConfiguration);
	}

	/**
	 * @fn	template<typename T> void ApplicationBuilder::RegisterGeneral()
	 *
	 * @brief	注册一个带
	 *
	 * @tparam	T	Generic type parameter.
	 */
	template<typename T>
	void RegisterGeneral()
	{
		m_DIConfig.Add(CreateGeneralObject<T>);
	}

    template <class InstanceType, class Deleter, class... Deps>
    using InstanceFactoryFunction = std::unique_ptr<InstanceType, Deleter> (*)(Deps *...);

    /**
     * @brief 手动注册
     * 手动注册依赖
     * @tparam InstanceType 
     * @tparam Deleter 
     * @tparam Deps 
     * @param instanceFactory 
     */
    template <class InstanceType, class Deleter, class... Deps>
    void Register(InstanceFactoryFunction<InstanceType, Deleter, Deps...> instanceFactory)
    {
        m_DIConfig.Add(instanceFactory);
    }

    /**
     * @brief 实例化所有依赖并创建分发器
     * 在实例化后 ApplicationBuilder 对象则可以被析构
     * @return Dispatcher 
     */
    Dispatcher Realization()
    {
		spdlog::info("Initialize configuration");
        return Dispatcher(m_DIConfig.BuildInjector(), m_PipelineRealizer, m_InputAdapterRealizer);
    }

	/**
	 * @fn	void ApplicationBuilder::InitLog()
	 *
	 * @brief	初始化日志
	 *
	 * @author	Xu Zihan
	 * @date	2019/11/21
	 */
	void InitLog()
	{
		auto console = spdlog::stderr_color_mt("console");
		spdlog::set_default_logger(console);
		spdlog::set_pattern("[%T.%e] [%-5t] %^[%l]%$  %v");
		spdlog::set_level(spdlog::level::info);
	}

private:
    DI::DIConfiguration m_DIConfig;

    // 用于获得实例化后的Pipeline，但此处框架仅支持一个Pipeline，使用vector是为了之后支持多Pipeline做准备
    std::vector<Realizer<std::tuple<std::type_index, Pipeline *>>> m_PipelineRealizer;

    // 用于获得实例化后的InputAdapter
    std::vector<Realizer<BaseInputAdapter *>> m_InputAdapterRealizer;
};

#endif // CONFIG_HPP