#pragma once

#include <istream>
#include <set>
#include <string>
#include <vector>

#include "Stream.h"
#include "Token.h"
#include "message_exception.h"

DECL_EXCEPTION(LexerException);

//TODO: make this more dynamic / flexible
// TODO: handle strings
class Lexer : public Stream<Token>
{
private:
    std::istream& input;
    const TokenRegistry& tokenRegistry;
    std::vector<char> accumulator;

    static constexpr auto EndOfFile = std::istream::traits_type::eof();

public:
    std::set<char> valid_identifier_chars;
    std::set<std::string> operators;
    std::set<std::string> keywords;
    std::set<char> standAlones;
    bool identifierHasNumeric = true;

    Lexer(std::istream& input, const TokenRegistry& tokenRegistry);
    virtual Token GetNext() override;

private:
    static bool IsAlpha(char c);

    static bool IsNumeric(char c);

    inline bool IsIdentifierChar(char c)
    {
        return IsAlpha(c) || (identifierHasNumeric && IsNumeric(c)) || valid_identifier_chars.count(c) == 1;
    }

    bool CanBeOperator(char peeked);

    void Advance();
    Token Emit(std::string type);

    void EatWhitespace();
    Token GetIdentifier();
    Token EmitIdentifier();

    Token GetOperator();

    Token EmitStandAlone(char c);

    Token GetNumber();
};


