#ifndef INTENTARGUMENT_H
#define INTENTARGUMENT_H

#include <Arduino.h>

struct IntentArgument {

    static const IntentArgument NO_ARG;

    enum class ArgType { NONE, INT, STRING } type = ArgType::NONE;

    union {
        int intValue;
        const char* strValue;
    };

    IntentArgument() = default;
    explicit IntentArgument(int value) : type(ArgType::INT), intValue(value) {}
    explicit IntentArgument(const char* value) : type(ArgType::STRING), strValue(value) {}
};

#endif