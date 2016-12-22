#pragma once

#include <string>

#include "RegistryBase.h"
#include "Token.h"

typedef int Symbol;

class SymbolRegistry : public RegistryBase<std::string>
{
private:
    Symbol startSymbol = -2;
    static const Symbol PSEUDO_START = -1;
    Symbol terminalBoundary = -1;
public:
    void InitWithTerminals(const TokenRegistry& tokens);
    void SetStartSymbol(Symbol symbol);
    Symbol GetStartSymbol() const;
    inline Symbol GetPseudoStartSymbol() const { return PSEUDO_START; }
    inline bool IsTerminal(Symbol symbol) const { return symbol >= 0 && symbol < terminalBoundary; }

    inline Symbol GetTerminalBoundary() const { return terminalBoundary; }
};


