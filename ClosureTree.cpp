#include "ClosureTree.h"
#include "RegistryManager.h"

ClosureTree::ClosureTree()
    : closures(RegistryManager::Instance.closures)
{}

void ClosureTree::Build()
{
    Closure start = Closure::CreateBeginning();
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

