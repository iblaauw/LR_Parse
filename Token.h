#pragma once

#include <string>

#include "RegistryBase.h"

typedef int TokenType;

struct Token
{
    TokenType type;
    std::string value;
};

using TokenRegistry = RegistryBase<std::string>;

