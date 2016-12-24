#pragma once

#include <vector>
#include <queue>
#include <unordered_set>

#include "Rule.h"
#include "SymbolRegistry.h"
#include "Properties.h"

class FirstTree : public IFirstProperty
{
private:
    struct SymbolNode {
        std::vector<RuleId> parents;
    };

    const RuleRegistry& rules;
    const SymbolRegistry& symbols;

    std::vector<SymbolNode> symbolData;
    std::vector<SymbolSet> firstData;

    std::queue<std::pair<Symbol, Symbol>> symbolQueue;
    std::queue<std::pair<RuleId, Symbol>> ruleQueue;

public:
    FirstTree(const RuleRegistry& rules, const SymbolRegistry& symbols);

    void Build(const INullableProperty& nullable);
    void Run();

    inline const SymbolSet& GetFirst(Symbol s) const override
    {
        return firstData[s];
    }

private:
    void DoRun();
    void HandleSymbol(Symbol s, Symbol terminal);
    void HandleRule(RuleId ruleid, Symbol terminal);
};
