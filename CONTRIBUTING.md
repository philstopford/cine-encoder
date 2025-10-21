# Contributing to Cine Encoder

Thank you for your interest in contributing to Cine Encoder! This document provides guidelines and best practices for contributing to the project.

## Code Style

### C++ Standards

- Use modern C++17 features where appropriate
- Follow Qt coding conventions for Qt-related code
- Use RAII principles for resource management
- Prefer stack allocation over heap allocation when possible

### Naming Conventions

- **Classes**: `PascalCase` (e.g., `EncoderStream`, `MainWindow`)
- **Methods**: `camelCase` (e.g., `initEncoding`, `getEncodingState`)
- **Variables**: `camelCase` for local variables, `m_` prefix for member variables (e.g., `m_encodingState`)
- **Constants**: `SCREAMING_SNAKE_CASE` or `constexpr` variables with descriptive names
- **Private members**: Prefix with `m_` (e.g., `m_pData`, `m_theme`)

### Header Files

**DO:**
- Use include guards (`#ifndef FILENAME_H`)
- Include only necessary headers
- Forward declare classes when possible
- Document all public APIs with Doxygen comments
- Use `constexpr` for compile-time constants instead of `#define`

**DON'T:**
- Use `using namespace` in header files (causes namespace pollution)
- Include `<bits/stdc++.h>` or other non-standard headers
- Put implementation in headers unless necessary (templates, inline functions)

### Example Header Structure

```cpp
/***********************************************************************
                          C I N E   E N C O D E R
 FILE: myclass.h
 COMMENT: Brief description of what this class does
 LICENSE: GNU General Public License v3.0
***********************************************************************/

#ifndef MYCLASS_H
#define MYCLASS_H

#include <QObject>
// Other includes...

/**
 * @brief Brief description of the class
 * 
 * Detailed description of what the class does,
 * how to use it, and any important notes.
 */
class MyClass : public QObject
{
    Q_OBJECT
    
public:
    /**
     * @brief Constructor
     * @param parent Parent QObject for memory management
     */
    explicit MyClass(QObject *parent = nullptr);
    
    /**
     * @brief Brief method description
     * @param param1 Description of param1
     * @return Description of return value
     */
    int myMethod(const QString &param1);
    
private:
    int m_myPrivateMember;  ///< Brief member description
};

#endif // MYCLASS_H
```

### Source Files

**DO:**
- Add `using namespace` in .cpp files if needed (never in .h files)
- Keep functions focused and single-purpose
- Use const-correctness (`const QString &` for string parameters)
- Initialize member variables in constructor initializer lists
- Check for null pointers before dereferencing

**DON'T:**
- Use raw `new`/`delete` - prefer smart pointers or Qt parent-child ownership
- Ignore compiler warnings
- Mix Qt types and STL types unnecessarily (prefer Qt types in Qt code)

## Architecture Guidelines

### Separation of Concerns

- **UI Layer**: QWidget-based classes should only handle UI logic
- **Controller Layer**: Business logic and coordination
- **Service Layer**: Reusable services (Logger, ErrorHandler, ConfigurationManager)
- **Data Layer**: Data structures and persistence

### Dependency Injection

For testability, use dependency injection when creating new classes:

```cpp
// Good: Dependencies are injected
class MyService {
public:
    MyService(std::shared_ptr<ILogger> logger) 
        : m_logger(logger) {}
    
private:
    std::shared_ptr<ILogger> m_logger;
};

// Avoid: Hard-coded dependencies
class MyService {
    void doWork() {
        Logger::instance().info("Working...");  // Hard to test
    }
};
```

### Error Handling

Use the centralized error handling system:

```cpp
#include "errorhandler.h"

// For user-facing errors
ErrorHandler::instance().showError("File not found", "Error", parentWidget);

// For logged errors
ErrorHandler::instance().handleFileError(filePath, "read", errorDetails);

// For exceptions
try {
    riskyOperation();
} catch (const std::exception &e) {
    ErrorHandler::instance().handleException(e, "MyClass::myMethod");
}
```

### Logging

Use the logging system for debugging and diagnostics:

```cpp
#include "logger.h"

LOG_DEBUG("Detailed debug information");
LOG_INFO("General information message");
LOG_WARNING("Warning that doesn't stop execution");
LOG_ERROR("Error that needs attention");
LOG_CRITICAL("Critical error that may crash");
```

## Testing

### Writing Tests

- Write unit tests for all new business logic
- Use Qt Test framework for consistency
- Mock dependencies for isolated testing
- Test edge cases and error conditions

Example test:

```cpp
#include <QtTest>
#include "myclass.h"

class TestMyClass : public QObject
{
    Q_OBJECT
    
private slots:
    void testBasicFunctionality()
    {
        MyClass obj;
        QCOMPARE(obj.myMethod("test"), 42);
    }
    
    void testErrorHandling()
    {
        MyClass obj;
        QVERIFY_EXCEPTION_THROWN(obj.riskyMethod(), std::runtime_error);
    }
};

QTEST_MAIN(TestMyClass)
#include "test_myclass.moc"
```

### Running Tests

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
QT_QPA_PLATFORM=offscreen ./build/tests/test_myclass
```

## Git Workflow

### Commit Messages

Use clear, descriptive commit messages:

```
Add support for AV1 encoding

- Implement AV1 codec in encoder module
- Add AV1 preset configurations
- Update UI to show AV1 option
- Add tests for AV1 encoding
```

### Branch Naming

- `feature/description` - New features
- `bugfix/description` - Bug fixes
- `refactor/description` - Code refactoring
- `docs/description` - Documentation updates

## Pull Request Process

1. **Before submitting**:
   - Ensure code compiles without warnings
   - Run all tests and ensure they pass
   - Update documentation if needed
   - Follow the code style guidelines

2. **PR Description**:
   - Describe what changes you made and why
   - Reference any related issues
   - Include screenshots for UI changes
   - List any breaking changes

3. **Review Process**:
   - Address reviewer feedback promptly
   - Keep commits focused and atomic
   - Rebase if needed to keep history clean

## Questions?

If you have questions or need help:
- Check existing documentation
- Look at similar code in the codebase
- Ask in the pull request or issue tracker

Thank you for contributing to Cine Encoder!
