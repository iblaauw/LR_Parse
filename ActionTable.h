#pragma once

#include <vector>

#include "ClosureTree.h"
#include "Properties.h"


class ActionTable
{
public:
    enum class ActionType
    {
        ERROR = 0, SHIFT, REDUCE, GOTO, ACCEPT
    };

    struct Action
    {
        ActionType type;
        int id;
    };

private:
    std::vector<std::vector<Action>> table;
public:
    ActionTable();

    void Build(const ClosureTree& tree, const IFollowProperty& follow, Symbol eofSymbol);

    inline const Action& Get(State state, Symbol input) const { return table[state][input]; }

private:
    void CheckSetValue(State state, Symbol symbol, ActionType type, int value, std::string name);

    void BuildShift(State state, State newstate, Symbol symbol);
    void BuildGoto(State state, State newstate, Symbol symbol);
    void BuildReduce(State state, RuleId ruleId, Symbol head, const IFollowProperty& follow);
    void BuildAccept(State state, Symbol eofSymbol);
};

