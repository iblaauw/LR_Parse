#include "FollowTree.h"

#include <iostream>

FollowTree::FollowTree(const RuleRegistry& rules, const SymbolRegistry& symbols)
    : rules(rules), symbols(symbols), followData(symbols.size()),
      followDependencies(symbols.size()), firstDependencies(symbols.size())
{}

void FollowTree::Build(const INullableProperty& nullable)
{
    for (RuleId rid : rules)
    {
        const Rule& rule = rules.GetRule(rid);
        BuildForRule(rule, nullable);
    }
}

void FollowTree::Run(const IFirstProperty& firstProp)
{
    // Add that the PSEUDOSTART must FOLLOW with EOF
    Symbol pstart = symbols.GetPseudoStartSymbol();
    Symbol eofSymbol = symbols.Get("EOF");
    symbolQueue.emplace(pstart, eofSymbol);

    InitQueue(firstProp);
    firstDependencies.clear();
    DoRun();
    followDependencies.clear();
}

void FollowTree::DoRun()
{
    while (symbolQueue.size() > 0)
    {
        auto val = symbolQueue.front();
        symbolQueue.pop();
        HandleSymbol(val.first, val.second);
    }
}

void FollowTree::BuildForRule(const Rule& rule, const INullableProperty& nullable)
{
    size_t size = rule.body.size();

    if (size == 0)
        return;

    // Do FOLLOW dependencies:
    for (int i = size - 1; i >= 0; i--)
    {
        Symbol s = rule.body[i];

        // No point in calculating the FOLLOW of terminals, they will never propagate
        //  and they will never be used
        if (symbols.IsTerminal(s))
            break;

        // Scenario:
        // Rule Head -> ... S
        // OR   Head -> ... S NULL NULL NULL
        // Then the FOLLOW(S) depends on FOLLOW(Head)
        followDependencies[rule.head].parents.insert(s);

        if (!nullable.IsNullable(s))
            break;
    }

    // Do FIRST dependencies:
    for (unsigned int i = 0; i < size - 1; ++i)
    {
        Symbol s1 = rule.body[i];
        for (unsigned int j = i + 1; j < size; ++j)
        {
            Symbol s2 = rule.body[j];

            // Scenario:
            // Rule ... -> ... S1 S2 ...
            // OR   ... -> ... S1 NULL NULL NULL S2 ...
            // Then FOLLOW(S1) depends on FIRST(S2)
            firstDependencies[s2].parents.insert(s1);

            if (!nullable.IsNullable(s2))
                break;
        }
    }
}

void FollowTree::InitQueue(const IFirstProperty& firstProp)
{
    for (unsigned int i = 0; i < firstDependencies.size(); ++i)
    {
        const auto& node = firstDependencies[i];
        Symbol firstFor = i;

        const SymbolSet& firstVals = firstProp.GetFirst(firstFor);

        for (Symbol par : node.parents)
        {
            for (Symbol fval : firstVals)
            {
                symbolQueue.emplace(par, fval);
            }
        }
    }
}

void FollowTree::HandleSymbol(Symbol s, Symbol val)
{
    // Don't propagate if there was no change (this ends infinite loops)
    // Anything that was already in FOLLOW must have been propagated already
    if (followData[s].count(val))
        return;

    followData[s].insert(val);

    // Propagate
    for (Symbol parent : followDependencies[s].parents)
    {
        symbolQueue.emplace(parent, val);
    }
}



