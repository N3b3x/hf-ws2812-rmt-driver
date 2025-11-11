---
layout: default
title: "🤝 Contributing"
description: "Guidelines and information for contributing to the HardFOC WS2812 RMT Driver"
nav_order: 5
parent: "🔧 HardFOC WS2812 RMT Driver"
permalink: /CONTRIBUTING/
---

# 🤝 Contributing to HardFOC WS2812 RMT Driver

Thank you for your interest in contributing to the HardFOC WS2812 RMT Driver!
This document provides guidelines and information for contributors.

## 📋 **Code Standards**

### 🎯 **Coding Style and Best Practices**

**All contributors must follow the official HardFOC Coding Standards:**

📚 **[HardFOC Embedded Systems Coding Standards](https://github.com/hardfoc/org-discussions)**

The coding standards document provides comprehensive guidelines for:
- **Naming Conventions**: Functions, variables, constants, classes, and more
- **Code Organization**: File structure, include order, class organization
- **Error Handling**: Patterns for embedded systems
- **Memory Management**: Best practices for resource-constrained systems
- **Thread Safety**: Guidelines for multi-threaded code
- **Documentation Standards**: Doxygen and inline comments
- **Embedded Systems Best Practices**: Fixed-width types, volatile usage, and more

### 🎯 **Quick Reference for WS2812 RMT Driver**

- **C++20 Standard Compliance** - All code must be compatible with C++20
- **C Compatibility** - Maintain C API compatibility for legacy code
- **Consistent Naming** - Follow the established naming conventions:
  - Classes: `PascalCase` (e.g., `WS2812Strip`, `WS2812Animator`)
  - Public Functions: `PascalCase` (e.g., `Update`, `SetBrightness`)
  - Private Functions: `camelCase` (e.g., `checkInitialized`, `updateStrip`)
  - Member Variables: `snake_case` with trailing underscore (e.g., `rmt_channel_`, `led_count_`)
  - Constants: `UPPER_SNAKE_CASE` (e.g., `WS2812_DEFAULT_FREQ`, `MAX_LED_COUNT`)
  - Local Variables: `snake_case` (e.g., `led_index`, `color_value`)
  - Parameters: `snake_case` (e.g., `gpio_pin`, `led_count`)
  - C Functions: `snake_case` (e.g., `ws2812_init`, `ws2812_update`)

### 🏗️ **Architecture Guidelines**

- **Hardware Abstraction** - Use ESP32 RMT peripheral for timing
- **Error Handling** - All functions use appropriate error handling patterns
- **Safety** - Use `noexcept` where appropriate for safety-critical code
- **Dependencies** - Keep dependencies minimal (ESP-IDF RMT driver)
- **Dual API** - Support both C and C++ APIs

## 🧪 **Testing**

### 🔧 **Unit Tests and Hardware Validation Requirements**

- **Unit Tests** - Write comprehensive unit tests for all new functionality
- **Hardware Testing** - Test on actual WS2812 LED strips with ESP32
- **Integration Tests** - Verify compatibility with existing HardFOC systems
- **Performance Tests** - Ensure real-time performance requirements are met
- **Timing Tests** - Validate RMT timing accuracy
- **Multi-Strip Tests** - Test multiple strips and animations

## 📖 **Documentation**

### 📚 **Documentation Standards and Updates**

- **API Documentation** - Update documentation for all public interfaces (C and C++)
- **User Guides** - Create or update guides for new features
- **Example Code** - Provide working examples for LED control applications
- **Architecture Documentation** - Document design decisions and patterns
- **Doxygen Comments** - All public APIs must have Doxygen documentation

## 🐛 **Bug Reports**

### 🔍 **How to Report Bugs Effectively**

When reporting bugs, please include:

1. **Hardware Information**: WS2812 LED strip type, ESP32 version, LED count
2. **Environment Details**: ESP-IDF version, compiler version, operating system
3. **Reproduction Steps**: Minimal code example, configuration settings
4. **Hardware Configuration**: GPIO pin, RMT channel, LED strip configuration
5. **Debugging Information**: Error messages, log output, stack traces
6. **Timing Information**: RMT timing settings and LED strip timing requirements

## ✨ **Feature Requests**

### 🚀 **Proposing New Features and Enhancements**

When proposing new features:

1. **Use Case** - Describe the specific LED control use case
2. **Technical Specification** - Provide detailed technical requirements
3. **API Design** - Propose the interface design following established patterns
4. **Implementation Plan** - Outline the implementation approach
5. **Testing Strategy** - Describe how the feature will be tested

## 🔄 **Development Workflow**

### 📋 **Step-by-Step Development Process**

1. **Fork the Repository**
2. **Create a Feature Branch**
3. **Implement Your Changes** following the [Coding Standards](https://github.com/hardfoc/org-discussions)
4. **Write Tests** for your changes
5. **Document Your Changes** with examples
6. **Submit a Pull Request**

## 📋 **Code Quality Standards**

- **C++20 Compliance** - Code compiles without warnings
- **C Compatibility** - C API remains compatible
- **HardFOC Compatibility** - Tested on HardFOC boards
- **Error Handling** - All error conditions handled appropriately
- **Documentation** - All public APIs documented with Doxygen
- **Tests** - Adequate test coverage provided
- **Performance** - Real-time requirements met
- **Coding Standards** - Follows [HardFOC Coding Standards](https://github.com/hardfoc/org-discussions)

## 🔗 **Resources**

- **[HardFOC Coding Standards](https://github.com/hardfoc/org-discussions)** - Complete coding standards document
- **[Documentation](../docs/)** - Driver documentation
- **[Examples](../examples/)** - Example code and usage patterns

---

## 🚀 Thank You for Contributing to HardFOC

Your contributions help make HardFOC motor controller boards more accessible and powerful for everyone.

