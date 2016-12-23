#pragma once

#include "SymbolRegistry.h"

class INullableProperty
{
public:
    virtual bool IsNullable(Symbol symbol) const = 0;
};

class IFirstProperty
{
public:
    virtual void GetFirst(Symbol symbol, std::vector<Symbol>& firstOut) const = 0;
};

class IFollowProperty
{
public:
    virtual void GetFollow(Symbol symbol, std::vector<Symbol>& followOut) const = 0;
};


