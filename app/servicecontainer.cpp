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
    // TODO: Implement proper service adapters when ready
    // This would register real implementations:
    // registerSingleton<ILogger, LoggerAdapter>();
    // registerSingleton<IConfigurationManager, ConfigurationManagerAdapter>();
    // registerSingleton<IInputValidator, InputValidatorAdapter>();
    // registerSingleton<IEncodingManager, EncodingManagerAdapter>();
    // registerSingleton<IErrorHandler, ErrorHandlerAdapter>();
    
    // For now, this method is a placeholder for future implementation
}