#include "FollowTree.h"

void FollowTree::Build(const SymbolRegistry& symbols, const INullableProperty& nullable)
{
    for (RuleId rid : rules)
    {
        const Rule& rule = rules.GetRule(rid);
        BuildForRule(rule, nullable, symbols);
    }
}

void FollowTree::Run()
{
    InitQueue();
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

void FollowTree::BuildForRule(const Rule& rule, const INullableProperty& nullable, const SymbolRegistry& symbols)
{
    size_t size = rule.body.size();

    // Do FOLLOW dependencies:
    for (unsigned int i = size - 1; i >= 0; i--)
    {
        Symbol s = rule.body[i];

        // No point in calculating the FOLLOW of terminals, they will never propagate
        //  and they will never be used
        if (symbols.IsTerminal(s))
            continue;

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

void FollowTree::InitQueue()
{
    for (unsigned int i = 0; i < firstDependencies.size(); ++i)
    {
        const auto& node = firstDependencies[i];
        Symbol firstFor = i;
        for (Symbol par : node.parents)
        {
            symbolQueue.emplace(par, firstFor);
        }
    }
}

void FollowTree::HandleSymbol(Symbol s, Symbol firstOf)
{
    std::vector<Symbol> firstVals;
    firstProp.GetFirst(firstOf, firstVals);

    size_t original = followData[s].size();

    // Insert FIRST vals into FOLLOW set
    for (Symbol sf : firstVals)
    {
        followData[s].insert(sf);
    }

    // Don't propagate if there was no change (this ends infinite loops)
    // Anything that was already in FOLLOW must have been propagated already
    if (followData[s].size() == original)
        return;

    for (Symbol parent : followDependencies[s].parents)
    {
        symbolQueue.emplace(parent, firstOf);
    }
}



