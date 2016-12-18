#pragma once

#include <string>

#include "RegistryBase.h"

typedef int Symbol;

class SymbolRegistry : public RegistryBase<std::string>
{
private:
    Symbol startSymbol = -2;
    static const Symbol PSEUDO_START = -1;
public:
    void SetStartSymbol(Symbol symbol);
    Symbol GetStartSymbol() const;
    inline Symbol GetPseudoStartSymbol() const { return PSEUDO_START; }
};


