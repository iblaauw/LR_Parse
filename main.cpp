#include <fstream>
#include <iostream>

#include "Lexer.h"
#include "Token.h"
#include "Rule.h"

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

void SetupSymbols(SymbolRegistry& symbols, TokenRegistry& tokReg)
{
    // Every token is also a symbol
    for (auto iter = tokReg.begin_val(); iter != tokReg.end_val(); ++iter)
    {
        symbols.Register(*iter);
    }

    Symbol start = symbols.Register("Program");
    symbols.SetStartSymbol(start);

    symbols.Register("StatementBlock");
    symbols.Register("Statement");
    symbols.Register("Expression");
}

void SetupRules(RuleRegistry& ruleReg, SymbolRegistry& symbols)
{
    // Program -> StatementBlock
    // StatementBlock -> StatementBlock Statement
    // Statement -> Expression ';'
    // Expression -> Expression OPERATOR Expression
    //              | NUMBER
    //              | IDENTIFIER

    Rule rule1 = { symbols.Get("Program"), { symbols.Get("StatementBlock") } };
    Rule rule2 = { symbols.Get("StatementBlock"), { symbols.Get("StatementBlock"), symbols.Get("Statement") } };
    Rule rule3 = { symbols.Get("Statement"), { symbols.Get("Expression"), symbols.Get(";") } };
    Rule rule4 = { symbols.Get("Expression"), { symbols.Get("Expression"), symbols.Get("OPERATOR"), symbols.Get("Expression") } };
    Rule rule5 = { symbols.Get("Expression"), { symbols.Get("NUMBER") } };
    Rule rule6 = { symbols.Get("Expression"), { symbols.Get("IDENTIFIER") } };

    ruleReg.Register(rule1);
    ruleReg.Register(rule2);
    ruleReg.Register(rule3);
    ruleReg.Register(rule4);
    ruleReg.Register(rule5);
    ruleReg.Register(rule6);
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
    SymbolRegistry symbolRegistry;
    RuleRegistry ruleRegistry;

    SetupTokens(tokenRegistry);

    Lexer lexer(infile, tokenRegistry);
    SetupLexer(lexer);

    SetupSymbols(symbolRegistry, tokenRegistry);
    SetupRules(ruleRegistry, symbolRegistry);

    PrintLexerAll(lexer, tokenRegistry);

    return 0;
}

