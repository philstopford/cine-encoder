/***********************************************************************

                          C I N E   E N C O D E R
                                JULY, 2020
                            COPYRIGHT (C) 2020

 FILE: servicecontainer.cpp
 COMMENT: Dependency injection container implementation
 LICENSE: GNU General Public License v3.0

***********************************************************************/

#include "servicecontainer.h"
#include "logger.h"
#include "configurationmanager.h"
#include "inputvalidator.h"
#include "encodingmanager.h"
#include "errorhandler.h"

// Forward declare adapter classes
class LoggerAdapter;
class ConfigurationManagerAdapter;
class InputValidatorAdapter;
class EncodingManagerAdapter;
class ErrorHandlerAdapter;

ServiceContainer& ServiceContainer::instance()
{
    static ServiceContainer instance;
    return instance;
}

void ServiceContainer::clear()
{
    m_services.clear();
}

void ServiceContainer::registerDefaultServices()
{
    // For now, register simple factory functions
    // We'll implement full adapters once the base classes are confirmed to work
    
    registerFactory<ILogger>([]() -> std::shared_ptr<ILogger> {
        // Return a mock logger for now
        return nullptr; // TODO: Implement proper adapter
    });
    
    // TODO: Add other service registrations once interfaces are tested
}