#pragma once

#include <vector>

#include "Rule.h"
#include "SymbolRegistry.h"
#include "NullableTree.h"
#include "Properties.h"

class RuleProperties : INullableProperty
{
private:
    const RuleRegistry& rules;
    SymbolRegistry& symbols;

    NullableTree nullable;
    FirstTree firstTree;
public:
    static const std::string NULL_TOKEN;

    RuleProperties(const RuleRegistry& rules, SymbolRegistry& symbols);

    void Compute();
    inline bool IsNullable(Symbol symbol) const { return nullable.IsNullable(symbol); }

};


