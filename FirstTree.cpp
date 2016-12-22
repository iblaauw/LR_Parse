#include "FirstTree.h"

FirstTree::FirstTree(const RuleRegistry& rules, const SymbolRegistry& symbols)
    : rules(rules), symbols(symbols), symbolData(symbols.size()), firstData(symbols.size())
{}

void FirstTree::Build(const INullableProperty& nullable)
{
    for (RuleId id : rules)
    {
        const Rule& rule = rules.GetRule(id);
        for (Symbol val : rule.body)
        {
            symbolData[val].parents.push_back(id);
            if (!nullable.IsNullable(val))
                break;
        }
    }
}

void FirstTree::Run()
{
    Symbol nullSymbol = symbols.GetValue(RuleProperties::NULL_TOKEN);

    // Put all terminals into the queue (except NULL)
    for (int i = 0; i < symbols.GetTerminalBoundary(); i++)
    {
        if (i == nullSymbol)
            continue;

        symbolQueue.emplace(i, i);
    }

    DoRun();
}

void FirstTree:GetFirst(Symbol s, std::vector<Symbol>& firstOut) const
{
    firstOut.clear();
    const auto& firsts = firstData[s];
    firstOut.insert(firstOut.begin(), firsts.begin(), firsts.end());
}

void FirstTree::DoRun()
{
    while (symbolQueue.size() > 0 || ruleQueue.size() > 0)
    {
        if (symbolQueue.size() > 0)
        {
            auto s = symbolQueue.front();
            symbolQueue.pop();
            HandleSymbol(s->first, s->second);
        }

        if (ruleQueue.size() > 0)
        {
            auto r = ruleQueue.front();
            ruleQueue.pop();
            HandleRule(r->first, r->second);
        }
    }
}

void FirstTree::HandleSymbol(Symbol s, Symbol terminal)
{
    auto& set = firstData[s];

    if (set.count(terminal))
        return;

    set.insert(terminal);

    for (RuleId rule : symbolData[s].parents)
    {
        ruleQueue.emplace(rule, terminal);
    }
}

void FirstTree::HandleRule(RuleId ruleid, Symbol terminal)
{
    const Rule& rule = rules.GetRule(ruleid);
    symbolQueue.emplace(rule.head, terminal);
}

