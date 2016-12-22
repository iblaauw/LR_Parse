#include "NullableTree.h"

NullableTree::NullableTree(const RuleRegistry& rules, const SymbolRegistry& symbols)
    : rules(rules), symbols(symbols), symbolData(symbols.size()), ruleData(rules.size())
{}

void NullableTree::Build()
{
    for (RuleId id : rules)
    {
        const Rule& rule = rules.GetValue(id);
        for (Symbol val : rule.body)
        {
            symbolData[val].parents.push_back(id);
        }
    }
}

void Run()
{
    Symbol s = symbols.Get(RuleProperties::NULL_TOKEN);
    symbolQueue.push(s);

    DoRun();
}

void DoRun()
{
    while (symbolQueue.size() > 0 && ruleQueue.size() > 0)
    {
        if (symbolQueue.size() > 0)
        {
            Symbol s = symbolQueue.pop();
            HandleSymbol(s);
        }

        if (ruleQueue.size() > 0)
        {
            RuleId id = ruleQueue.pop();
            HandleRule(id);
        }
    }
}

void HandleSymbol(Symbol s)
{
    if (IsNull(s))
        return;

    symbolData[s].isNull = true;

    for (RuleId id : symbolData[s].parents)
    {
        ruleQueue.push(id);
    }
}

void HandleRule(RuleId id)
{
    const Rule& rule = rules.GetValue(id);

    for (Symbol s : rule.body)
    {
        if (!IsNull(s))
            return;
    }

    // Every child is nullable! Mark this rule as nullable
    symbolQueue.push(rule.head);
}


