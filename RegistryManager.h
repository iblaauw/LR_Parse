#pragma once

#include "Token.h"
#include "SymbolRegistry.h"
#include "Rule.h"


class RegistryManager
{
public:
    static RegistryManager Instance;

    TokenRegistry tokens;
    SymbolRegistry symbols;
    RuleRegistry rules;

private:
    RegistryManager();
};
