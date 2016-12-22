#pragma once

#include <vector>

#include "Rule.h"
#include "SymbolRegistry.h"
#include "NullableTree.h"

class RuleProperties
{
private:
    const RuleRegistry& rules;
    SymbolRegistry& symbols;

    NullableTree nullable;
public:
    static const std::string NULL_TOKEN;

    RuleProperties(const RuleRegistry& rules, SymbolRegistry& symbols);

    void Compute();
    inline bool IsNullable(Symbol symbol) const { return nullable.IsNull(symbol); }

};


