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
    const IFirstProperty& firstProp;

    std::vector<std::unordered_set<Symbol>> followData;

    std::vector<SymbolNode> followDependencies;
    std::vector<SymbolNode> firstDependencies;

    std::queue<std::pair<Symbol, Symbol>> symbolQueue;
public:
    FollowTree(const RuleRegistry& rules, const IFirstProperty& followProp);
    void Build(const SymbolRegistry& symbols, const INullableProperty& nullable);
    void Run();

    void GetFollow(Symbol symbol, std::vector<Symbol>& followOut) const override;
private:
    void DoRun();

    void BuildForRule(const Rule& rule, const INullableProperty& nullable,
                      const SymbolRegistry& symbols);

    void InitQueue();

    void HandleSymbol(Symbol s, Symbol firstOf);
};
