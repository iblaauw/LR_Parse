#pragma once

#include <vector>
#include <queue>

#include "SymbolRegistry.h"
#include "Rule.h"
#include "Properties.h"

class NullableTree : public INullableProperty
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

    bool IsNullable(Symbol s) const override { return symbolData[s].isNull; };

private:
    void DoRun();
    void HandleSymbol(Symbol s);
    void HandleRule(RuleId id);
};

