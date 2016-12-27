#include "Parser.h"

#include <cassert>
#include <iostream>

#include "RegistryManager.h"
#include "message_exception.h"

Parser::Parser(Stream<Token>& tokens, const ActionTable& table)
    : tokens(tokens), table(table)
{}

void Parser::Parse()
{
    // Set start state to 0
    items.push_back({0, -1});

    Advance();

    while (true)
    {
        ActionTable::Action action = GetAction();

        if (action.type == ActionTable::ActionType::ACCEPT)
            break;

        if (action.type == ActionTable::ActionType::ERROR)
        {
            std::cout << "Error: invalid syntax" << std::endl;
            return;
        }

        DoAction(action);
    }

    std::cout << "Success!" << std::endl;
}

ActionTable::Action Parser::GetAction()
{
    State curState = items.back().state;
    return table.Get(curState, current);
}

void Parser::DoAction(ActionTable::Action action)
{
    switch (action.type)
    {
    case ActionTable::ActionType::SHIFT:
        DoShift(action.id);
        break;
    case ActionTable::ActionType::REDUCE:
        DoReduce(action.id);
        break;
    default:
        DEATH();
        break;
    }
}

void Parser::DoShift(State state)
{
    items.push_back({state, current});
    Advance();
}

void Parser::DoReduce(RuleId rid)
{
    const Rule& rule = RegistryManager::Instance.rules.GetRule(rid);

    assert(items.size() > rule.body.size());

    // TODO: combine what we're popping
    std::vector<Symbol> prev;
    for (int i = rule.body.size(); i >= 0; --i)
    {
        Symbol prevSymbol = items.back().symbol;
        assert(prevSymbol == rule.body[i]);
        prev.push_back(prevSymbol);
        items.pop_back();
    }

    OnReduce(rule.head, prev);

    DoGoto(rule.head);
}

void Parser::DoGoto(Symbol symbol)
{
    State curState = items.back().state;
    ActionTable::Action action = table.Get(curState, symbol);

    assert(action.type == ActionTable::ActionType::GOTO);

    items.push_back({action.id, symbol});
}

void Parser::Advance()
{
    Token tok = tokens.GetNext();
    TokenType tokType = tok.type;

    std::string str = RegistryManager::Instance.tokens.GetValue(tokType);
    current = RegistryManager::Instance.symbols.Get(str);
}

void Parser::OnReduce(Symbol head, const std::vector<Symbol>& body)
{
    const auto& symbols = RegistryManager::Instance.symbols;
    std::cout << "Produce: ";
    for (Symbol s : body)
    {
        std::cout << symbols.GetValue(s) << " ";
    }
    std::cout << "-> " << symbols.GetValue(head) << std::endl;
}

