#include "SymbolRegistry.h"

void SymbolRegistry::SetStartSymbol(Symbol symbol)
{
    startSymbol = symbol;
}

Symbol SymbolRegistry::GetStartSymbol() const
{
    return startSymbol;
}

