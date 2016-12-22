#pragma once

#include "SymbolRegistry.h"

class INullableProperty
{
public:
    virtual bool IsNullable(Symbol symbol) const = 0;
};

