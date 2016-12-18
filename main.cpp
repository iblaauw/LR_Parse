#include <fstream>
#include <iostream>

#include "Lexer.h"
#include "Token.h"

void SetupTokens(TokenRegistry& tokenRegistry)
{
    // Categories
    tokenRegistry.Register("EOF");
    tokenRegistry.Register("IDENTIFIER");
    tokenRegistry.Register("OPERATOR");
    tokenRegistry.Register("NUMBER");

    // Keywords
    tokenRegistry.Register("if");
    tokenRegistry.Register("else");
    tokenRegistry.Register("int");
    tokenRegistry.Register("float");
    tokenRegistry.Register("double");

    // Standalones
    tokenRegistry.Register("(");
    tokenRegistry.Register(")");
    tokenRegistry.Register("{");
    tokenRegistry.Register("}");
    tokenRegistry.Register("[");
    tokenRegistry.Register("]");
    tokenRegistry.Register(";");
}

void SetupLexer(Lexer& lex)
{
    lex.operators = { "*", "+", "=", ".", "==" };
    lex.keywords = { "if", "else", "int", "float", "double" };
    lex.standAlones = { '(', ')', '{', '}', '[', ']', ';' };
}

void PrintToken(Token& tok, const TokenRegistry& tokenRegistry)
{
    std::string name = tokenRegistry.GetValue(tok.type);
    std::cout << name << "('" << tok.value << "') ";
}

void PrintLexerAll(Lexer& lex, const TokenRegistry& reg)
{
    TokenType eof = reg.Get("EOF");

    while (true)
    {
        Token tok = lex.GetNext();
        if (tok.type == eof)
            break;

        PrintToken(tok, reg);
        std::cout << std::endl;
    }
}

int main()
{
    std::ifstream infile("input.test");

    TokenRegistry tokenRegistry;
    SetupTokens(tokenRegistry);

    Lexer lexer(infile, tokenRegistry);
    SetupLexer(lexer);

    PrintLexerAll(lexer, tokenRegistry);

    return 0;
}

