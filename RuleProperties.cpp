#include "RuleProperties.h"

/*static*/ const std::string RuleProperties::NULL_TOKEN = "NULL";

RuleProperties::RuleProperties(const RuleRegistry& rules, SymbolRegistry& symbols)
    : rules(rules), symbols(symbols), nullable(rules, symbols), firstTree(rules, symbols)
{
}

#include <iostream>

void RuleProperties::Compute()
{
    nullable.Build();

    Symbol nullSymbol = symbols.Get(NULL_TOKEN);
    nullable.Run(nullSymbol);

    firstTree.Build(nullable);
    firstTree.Run();
}



