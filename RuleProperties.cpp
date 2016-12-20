#include "RuleProperties.h"

RuleProperties::RuleProperties(RuleRegistry& rules, SymbolRegistry& symbols)
    : rules(rules), symbols(symbols), nullable(rules.size(), true)
{
}

void RuleProperties::FindNullable()
{
    // Set the terminals to be non-nullable
    for (Symbol i = 0; i < symbols.size(); ++i)
    {
        if (symbols.IsTerminal(s))
        {
            nullable[s] = false;
        }
        else
            break;
    }
}

bool RuleProperties::IsNullable(Symbol symbol)
{
    return nullable[symbol];
}

