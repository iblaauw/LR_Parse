#include "Lexer.h"

#include <cassert>

Lexer::Lexer(std::istream& input, const TokenRegistry& tokenRegistry)
    : input(input), tokenRegistry(tokenRegistry), valid_identifier_chars()
{
    //operators = { "*", "+", "=", ".", "==" };
    //keywords = { "if", "else", "int", "float", "double" };
    //standAlones = { '(', ')', '{', '}', '[', ']', ';' };
}

Token Lexer::GetNext()
{
    assert(accumulator.size() == 0);

    EatWhitespace();

    assert(accumulator.size() == 0);

    std::istream::int_type c = input.peek();
    if (c == EndOfFile)
        return Emit("EOF");

    if (IsNumeric(c))
        return GetNumber();

    if (standAlones.count(c) == 1)
    {
        input.get();
        return EmitStandAlone(c);
    }

    if (CanBeOperator(c))
        return GetOperator();

    return GetIdentifier();
}

bool Lexer::IsAlpha(char c)
{
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

bool Lexer::IsNumeric(char c)
{
    return c >= '0' && c <= '9';
}

bool Lexer::CanBeOperator(char peeked)
{
    for (std::string op : operators)
    {
        if (op.size() < accumulator.size() + 1)
            continue;

        bool correct = true;
        for (unsigned int i = 0; i < accumulator.size(); i++)
        {
            if (accumulator[i] != op[i])
            {
                correct = false;
                break;
            }
        }

        if (correct)
        {
            if (op[accumulator.size()] == peeked)
                return true;
        }
    }

    return false;
}

void Lexer::Advance()
{
    accumulator.push_back(input.get());
}

Token Lexer::Emit(std::string type)
{
    std::string data(accumulator.data(), accumulator.size());
    accumulator.clear();

    TokenType toktype = tokenRegistry.Get(type);
    return Token { toktype, data };
}

void Lexer::EatWhitespace()
{
    while (true)
    {
        char c = input.peek();
        if (c != ' ' && c != '\n' && c != '\t')
            break;

        input.get();
    }
}

Token Lexer::GetIdentifier()
{
    while (true)
    {
        char c = input.peek();
        if (!IsIdentifierChar(c))
            return EmitIdentifier();

        Advance();
    }

    DEATH();
}

Token Lexer::EmitIdentifier()
{
    std::string data(accumulator.data(), accumulator.size());
    accumulator.clear();

    TokenType type;
    if (keywords.count(data) == 1)
    {
        type = tokenRegistry.Get(data);
    }
    else
    {
        type = tokenRegistry.Get("IDENTIFIER");
    }

    return Token { type, data };
}

Token Lexer::GetOperator()
{
    while (true)
    {
        char c = input.peek();
        if (!CanBeOperator(c))
            return Emit("OPERATOR");

        Advance();
    }

    DEATH();
}

Token Lexer::EmitStandAlone(char c)
{
    std::string data(1, c);
    TokenType type = tokenRegistry.Get(data);
    return Token { type, data };
}

Token Lexer::GetNumber()
{
    bool seenDecimal = false;
    while (true)
    {
        char c = input.peek();
        if (!IsNumeric(c) && c != '.')
            return Emit("NUMBER");

        if (c == '.')
        {
            if (seenDecimal)
                throw LexerException("Invalid number!");

            seenDecimal = true;
        }

        Advance();
    }

    DEATH();
}



