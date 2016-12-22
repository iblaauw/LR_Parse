#pragma once

#include <vector>

#include "SymbolRegistry.h"
#include "Rule.h"

class NullableTree
{
private:
    struct SymbolNode {
        bool isNull;
        std::vector<RuleId> parents;
    };

    const RuleRegistry& rules;
    const SymbolRegistry& symbols;

    std::vector<SymbolNode> symbolData;
    std::vector<bool> ruleData;

    std::queue<Symbol> symbolQueue;
    std::queue<RuleId> ruleQueue;

public:
    NullableTree(const RuleRegistry& rules, const SymbolRegistry& symbols);
    void Build();
    void Run();

    inline bool IsNull(Symbol s) const { return symbolData[s].isNull; };

private:
    void DoRun();
    void HandleSymbol(Symbol s);
    void HandleRule(RuleId id);
};

