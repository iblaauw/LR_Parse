#pragma once

#include <vector>

#include "Rule.h"
#include "SymbolRegistry.h"

class RuleProperties
{
private:
    const RuleRegistry& rules;
    SymbolRegistry& symbols;

    std::vector<bool> nullable;
public:
    static const std::string NULL_TOKEN = "NULL";

    RuleProperties(const RuleRegistry& rules, SymbolRegistry& symbols);

    void FindNullable();
    bool IsNullable(Symbol symbol);

private:
    bool ComputeNullable(Symbol symbol, std::vector<bool>& isset);
    bool ComputeRuleNullable(RuleId ruleid, Symbol s, std::vector<bool>& isset);

};

