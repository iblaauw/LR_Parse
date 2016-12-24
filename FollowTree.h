#pragma once

#include <queue>
#include <vector>
#include <unordered_set>
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

    std::vector<std::unordered_set<Symbol>> followData;

    std::vector<SymbolNode> followDependencies;
    std::vector<SymbolNode> firstDependencies;

    std::queue<std::pair<Symbol, Symbol>> symbolQueue;
public:
    FollowTree(const RuleRegistry& rules, const SymbolRegistry& symbols);
    void Build(const SymbolRegistry& symbols, const INullableProperty& nullable);
    void Run(const IFirstProperty& firstProp);

    void GetFollow(Symbol symbol, std::vector<Symbol>& followOut) const override;
private:
    void DoRun(const IFirstProperty& firstProp);

    void BuildForRule(const Rule& rule, const INullableProperty& nullable,
                      const SymbolRegistry& symbols);

    void InitQueue(const IFirstProperty& firstProp);

    void HandleSymbol(Symbol s, Symbol firstOf, const IFirstProperty& firstProp);
};



