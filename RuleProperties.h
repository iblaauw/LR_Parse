#pragma once

#include <vector>

#include "Rule.h"
#include "SymbolRegistry.h"
#include "NullableTree.h"
#include "FirstTree.h"
#include "FollowTree.h"
#include "Properties.h"

class RuleProperties : public INullableProperty, public IFirstProperty, public IFollowProperty
{
private:
    const RuleRegistry& rules;
    SymbolRegistry& symbols;

    NullableTree nullable;
    FirstTree firstTree;
    FollowTree followTree;
public:
    static const std::string NULL_TOKEN;

    RuleProperties(const RuleRegistry& rules, SymbolRegistry& symbols);

    void Compute();
    inline bool IsNullable(Symbol symbol) const override { return nullable.IsNullable(symbol); }

    inline void GetFirst(Symbol symbol, std::vector<Symbol>& firstOut) const override
    {
        firstTree.GetFirst(symbol, firstOut);
    }

    inline void GetFollow(Symbol symbol, std::vector<Symbol>& followOut) const override
    {
        followTree.GetFollow(symbol, followOut);
    }
};


