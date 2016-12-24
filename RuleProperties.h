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

    inline const SymbolSet& GetFirst(Symbol symbol) const override
    {
        return firstTree.GetFirst(symbol);
    }

    inline const SymbolSet& GetFollow(Symbol symbol) const override
    {
        return followTree.GetFollow(symbol);
    }
};


