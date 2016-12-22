#include "NullableTree.h"

NullableTree::NullableTree(const RuleRegistry& rules, const SymbolRegistry& symbols)
    : rules(rules), symbolData(symbols.size())
{}

#include <iostream>

void NullableTree::Build()
{
    for (RuleId id : rules)
    {
        const Rule& rule = rules.GetRule(id);
        for (Symbol val : rule.body)
        {
            symbolData[val].parents.push_back(id);
        }
    }
}

void NullableTree::Run(Symbol nullSymbol)
{
    symbolQueue.push(nullSymbol);

    DoRun();
}

void NullableTree::DoRun()
{
    while (symbolQueue.size() > 0 || ruleQueue.size() > 0)
    {
        if (symbolQueue.size() > 0)
        {
            Symbol s = symbolQueue.front();
            symbolQueue.pop();
            HandleSymbol(s);
        }

        if (ruleQueue.size() > 0)
        {
            RuleId id = ruleQueue.front();
            ruleQueue.pop();
            HandleRule(id);
        }
    }
}

void NullableTree::HandleSymbol(Symbol s)
{
    if (IsNull(s))
        return;

    symbolData[s].isNull = true;

    for (RuleId id : symbolData[s].parents)
    {
        ruleQueue.push(id);
    }
}

void NullableTree::HandleRule(RuleId id)
{
    const Rule& rule = rules.GetRule(id);

    for (Symbol s : rule.body)
    {
        if (!IsNull(s))
            return;
    }

    // Every child is nullable! Mark this rule as nullable
    symbolQueue.push(rule.head);
}


