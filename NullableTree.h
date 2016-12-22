#pragma once

#include <vector>
#include <queue>

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

    std::vector<SymbolNode> symbolData;

    std::queue<Symbol> symbolQueue;
    std::queue<RuleId> ruleQueue;

public:
    NullableTree(const RuleRegistry& rules, const SymbolRegistry& symbols);
    void Build();
    void Run(Symbol nullSymbol);

    inline bool IsNull(Symbol s) const { return symbolData[s].isNull; };

private:
    void DoRun();
    void HandleSymbol(Symbol s);
    void HandleRule(RuleId id);
};

