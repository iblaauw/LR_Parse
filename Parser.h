#pragma once

#include <vector>

#include "Stream.h"
#include "Token.h"
#include "ActionTable.h"

class Parser
{
private:
    struct Item
    {
        State state;
        Symbol symbol;
    };

    Stream<Token>& tokens;
    const ActionTable& table;

    Symbol current;
    std::vector<Item> items;
public:
    Parser(Stream<Token>& tokens, const ActionTable& table);
    void Parse();

private:
    ActionTable::Action GetAction();
    void DoAction(ActionTable::Action action);

    void DoShift(State state);
    void DoReduce(RuleId rid);
    void DoGoto(Symbol symbol);

    void Advance();

    void OnReduce(Symbol head, const std::vector<Symbol>& body);
};

