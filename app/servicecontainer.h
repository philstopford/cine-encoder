/***********************************************************************

                          C I N E   E N C O D E R
                                JULY, 2020
                            COPYRIGHT (C) 2020

 FILE: servicecontainer.h
 COMMENT: Dependency injection container for service management
 LICENSE: GNU General Public License v3.0

***********************************************************************/

#ifndef SERVICECONTAINER_H
#define SERVICECONTAINER_H

#include <memory>
#include <functional>
#include <typeindex>
#include <unordered_map>
#include "interfaces.h"

/**
 * @brief Service container for dependency injection
 * 
 * Provides a centralized way to register and resolve service dependencies.
 * This improves testability by allowing mock implementations to be injected.
 */
class ServiceContainer
{
public:
    // Singleton access
    static ServiceContainer& instance();
    
    // Service registration
    template<typename Interface, typename Implementation>
    void registerSingleton();
    
    template<typename Interface>
    void registerSingleton(std::shared_ptr<Interface> instance);
    
    template<typename Interface>
    void registerFactory(std::function<std::shared_ptr<Interface>()> factory);
    
    // Service resolution
    template<typename Interface>
    std::shared_ptr<Interface> resolve();
    
    template<typename Interface>
    bool isRegistered() const;
    
    // Clear all registrations (useful for testing)
    void clear();
    
    // Register default services
    void registerDefaultServices();

private:
    ServiceContainer() = default;
    ~ServiceContainer() = default;
    ServiceContainer(const ServiceContainer&) = delete;
    ServiceContainer& operator=(const ServiceContainer&) = delete;
    
    struct ServiceInfo {
        std::function<std::shared_ptr<void>()> factory;
        std::shared_ptr<void> instance; // For singleton pattern
        bool isSingleton;
    };
    
    std::unordered_map<std::type_index, ServiceInfo> m_services;
};

// Template implementations
template<typename Interface, typename Implementation>
void ServiceContainer::registerSingleton()
{
    m_services[std::type_index(typeid(Interface))] = ServiceInfo{
        []() -> std::shared_ptr<void> {
            return std::make_shared<Implementation>();
        },
        nullptr,
        true
    };
}

template<typename Interface>
void ServiceContainer::registerSingleton(std::shared_ptr<Interface> instance)
{
    m_services[std::type_index(typeid(Interface))] = ServiceInfo{
        [instance]() -> std::shared_ptr<void> { return instance; },
        instance,
        true
    };
}

template<typename Interface>
void ServiceContainer::registerFactory(std::function<std::shared_ptr<Interface>()> factory)
{
    m_services[std::type_index(typeid(Interface))] = ServiceInfo{
        [factory]() -> std::shared_ptr<void> { return factory(); },
        nullptr,
        false
    };
}

template<typename Interface>
std::shared_ptr<Interface> ServiceContainer::resolve()
{
    auto it = m_services.find(std::type_index(typeid(Interface)));
    if (it == m_services.end()) {
        return nullptr;
    }
    
    ServiceInfo& info = it->second;
    
    if (info.isSingleton) {
        if (!info.instance) {
            info.instance = info.factory();
        }
        return std::static_pointer_cast<Interface>(info.instance);
    } else {
        return std::static_pointer_cast<Interface>(info.factory());
    }
}

template<typename Interface>
bool ServiceContainer::isRegistered() const
{
    return m_services.find(std::type_index(typeid(Interface))) != m_services.end();
}

// Convenience macros for service access
#define RESOLVE_SERVICE(Interface) ServiceContainer::instance().resolve<Interface>()
#define REGISTER_SERVICE(Interface, Implementation) ServiceContainer::instance().registerSingleton<Interface, Implementation>()

#endif // SERVICECONTAINER_H