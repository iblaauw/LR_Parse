#pragma once

#include <unordered_set>

#include "SymbolRegistry.h"

using SymbolSet = std::unordered_set<Symbol>;

class INullableProperty
{
public:
    virtual bool IsNullable(Symbol symbol) const = 0;
};

class IFirstProperty
{
public:
    virtual const SymbolSet& GetFirst(Symbol symbol) const = 0;
};

class IFollowProperty
{
public:
    virtual const SymbolSet& GetFollow(Symbol symbol) const = 0;
};


