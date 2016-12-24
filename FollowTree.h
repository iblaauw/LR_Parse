#pragma once

#include <queue>
#include <vector>
#include <set>

#include "Properties.h"
#include "SymbolRegistry.h"
#include "Rule.h"

class FollowTree : public IFollowProperty
{
private:
    struct SymbolNode {
        std::set<Symbol> parents;
    };

    const RuleRegistry& rules;
    const SymbolRegistry& symbols;

    std::vector<SymbolSet> followData;

    std::vector<SymbolNode> followDependencies;
    std::vector<SymbolNode> firstDependencies;

    std::queue<std::pair<Symbol, Symbol>> symbolQueue;
public:
    FollowTree(const RuleRegistry& rules, const SymbolRegistry& symbols);
    void Build(const INullableProperty& nullable);
    void Run(const IFirstProperty& firstProp);

    inline const SymbolSet& GetFollow(Symbol symbol) const override
    {
        return followData[symbol];
    }
private:
    void DoRun();

    void BuildForRule(const Rule& rule, const INullableProperty& nullable);

    void InitQueue(const IFirstProperty& firstProp);

    void HandleSymbol(Symbol s, Symbol val);
};



