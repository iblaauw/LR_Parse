#pragma once

#include <vector>

#include "Rule.h"
#include "SymbolRegistry.h"

class RuleProperties
{
private:
    RuleRegistry& rules;
    SymbolRegistry& symbols;

    std::vector<bool> nullable;
public:
    RuleProperties(RuleRegistry& rules, SymbolRegistry& symbols);

    void FindNullable();
    bool IsNullable(Symbol symbol);
};

