#include "ClosureTree.h"

ClosureTree::ClosureTree(ClosureRegistry& closures)
    : closures(closures)
{}

void ClosureTree::Build(const RuleRegistry& rules, const SymbolRegistry& symbols)
{
    Closure start = Closure::CreateBeginning(rules, symbols);
    State startState = closures.Register(start);
    stateQueue.push(startState);

    DoBuild();
}

void ClosureTree::DoBuild()
{
    while (stateQueue.size() > 0)
    {
        State state = stateQueue.front();
        stateQueue.pop();
        HandleState(state);
    }
}

void ClosureTree::HandleState(State state)
{
    // We've already covered this state!
    if (nodes.find(state) != nodes.end())
        return;

    const Closure& closure = closures.GetValue(state);
    StateNode& node = nodes[state]; // Insert the node

    // Get the set of symbols that generate transitions
    SymbolSet advanceable;
    closure.GetAdvanceable(advanceable);

    for (Symbol s : advanceable)
    {
        // Get next state
        State nextState = closures.GetOrRegister(closure.Advance(s));

        // Add to data
        node.children.emplace_back(nextState, s);
        stateQueue.push(nextState);
    }
}

