#include "ActionTable.h"

#include "RegistryManager.h"
#include "message_exception.h"

DECL_EXCEPTION(TableException);

ActionTable::ActionTable()
{
    const auto& closures = RegistryManager::Instance.closures;
    const auto& symbols = RegistryManager::Instance.symbols;

    std::vector<Action> exampleRow(symbols.size());
    table.resize(closures.size(), exampleRow); // Insert a copy of exampleRow for each state
}

void ActionTable::Build(const ClosureTree& tree, const IFollowProperty& follow, Symbol eofSymbol)
{
    const auto& closures = RegistryManager::Instance.closures;
    const auto& symbols = RegistryManager::Instance.symbols;
    const auto& rules = RegistryManager::Instance.rules;

    State pseudoRule = rules.GetPseudoRule();

    for (State state : closures)
    {
        const StateNode& node = tree.GetNode(state);
        for (auto transition : node.children)
        {
            State newstate = transition.first;
            Symbol symbol = transition.second;

            if (symbols.IsTerminal(symbol))
            {
                BuildShift(state, newstate, symbol);
            }
            else
            {
                BuildGoto(state, newstate, symbol);
            }
        }

        const Closure& closure = closures.GetValue(state);
        for (RulePiece piece : closure.GetPieces())
        {
            if (piece.AtEnd(rules))
            {
                if (piece.rule == pseudoRule)
                {
                    BuildAccept(state, eofSymbol);
                }
                else
                {
                    Symbol head = rules.GetRule(piece.rule).head;
                    BuildReduce(state, piece.rule, head, follow);
                }
            }
        }
    }
}

void ActionTable::CheckSetValue(State state, Symbol symbol, ActionType type, int value, std::string name)
{
    Action& act = table[state][symbol];
    if (act.type != ActionType::ERROR)
    {
        const auto& symbols = RegistryManager::Instance.symbols;

        std::string message = "A " + name + " conflict occured while building the table:\n";
        message += "\tin table slot ";
        message += std::to_string(state) + ", " + symbols.GetValue(symbol);
        message += "\n\t";
        message += std::to_string((int)type);
        message += ", ";
        message += std::to_string(value);
        message += " -> ";
        message += std::to_string((int)act.type);
        message += ", ";
        message += std::to_string(act.id);

        throw TableException(message);
    }

    act.type = type;
    act.id = value;
}

void ActionTable::BuildShift(State state, State newstate, Symbol symbol)
{
    CheckSetValue(state, symbol, ActionType::SHIFT, newstate, "shift");
}

void ActionTable::BuildGoto(State state, State newstate, Symbol symbol)
{
    CheckSetValue(state, symbol, ActionType::GOTO, newstate, "goto");
}

void ActionTable::BuildReduce(State state, RuleId ruleId, Symbol head,
    const IFollowProperty& follow)
{
    for (Symbol s : follow.GetFollow(head))
    {
        CheckSetValue(state, s, ActionType::REDUCE, ruleId, "reduce");
    }
}

void ActionTable::BuildAccept(State state, Symbol eofSymbol)
{
    // Intentionally don't check, and just brute force override. This will never be incorrect
    Action& act = table[state][eofSymbol];
    act.type = ActionType::ACCEPT;
}
