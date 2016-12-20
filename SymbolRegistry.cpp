#include "SymbolRegistry.h"

void SymbolRegistry::InitWithTerminals(const TokenRegistry& tokens)
{
    for (auto iter = tokens.begin_val(); iter != tokens.end_val(); ++iter)
    {
        Register(*iter);
    }

    terminalBoundary = this->current;
}

void SymbolRegistry::SetStartSymbol(Symbol symbol)
{
    startSymbol = symbol;
}

Symbol SymbolRegistry::GetStartSymbol() const
{
    return startSymbol;
}

