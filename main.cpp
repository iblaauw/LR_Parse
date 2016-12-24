#include <fstream>
#include <iostream>

#include "Lexer.h"
#include "Token.h"
#include "Rule.h"
#include "RuleProperties.h"

void SetupTokens(TokenRegistry& tokenRegistry)
{
    // NULL symbol (empty string)
    tokenRegistry.Register(RuleProperties::NULL_TOKEN);

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

void SetupSymbols(SymbolRegistry& symbols)
{
    Symbol start = symbols.Register("Program");
    symbols.SetStartSymbol(start);

    symbols.Register("StatementBlock");
    symbols.Register("Statement");
    symbols.Register("Expression");
}

void SetupRules(RuleRegistry& ruleReg, const SymbolRegistry& symbols)
{
    // Program -> StatementBlock
    // StatementBlock -> StatementBlock Statement
    //              | NULL
    // Statement -> Expression ';'
    // Expression -> Expression OPERATOR Expression
    //              | NUMBER
    //              | IDENTIFIER

    std::vector<Rule> rules;

    //rules.push_back({ symbols.Get("Program"), { symbols.Get("StatementBlock") } });
    //rules.push_back({ symbols.Get("StatementBlock"), { symbols.Get("StatementBlock"), symbols.Get("Statement") } });
    //rules.push_back({ symbols.Get("StatementBlock"), { symbols.Get("NULL") } });
    //rules.push_back({ symbols.Get("Statement"), { symbols.Get("Expression"), symbols.Get(";") } });
    //rules.push_back({ symbols.Get("Expression"), { symbols.Get("Expression"), symbols.Get("OPERATOR"), symbols.Get("Expression") } });
    //rules.push_back({ symbols.Get("Expression"), { symbols.Get("NUMBER") } });
    //rules.push_back({ symbols.Get("Expression"), { symbols.Get("IDENTIFIER") } });

    rules.push_back({ symbols.Get("Program"), { symbols.Get("StatementBlock") } });
    rules.push_back({ symbols.Get("StatementBlock"), { symbols.Get("Statement"), symbols.Get("Expression") } });
    rules.push_back({ symbols.Get("Statement"), { symbols.Get("Expression") } });
    rules.push_back({ symbols.Get("Statement"), { symbols.Get("IDENTIFIER") } });
    rules.push_back({ symbols.Get("Expression"), { symbols.Get("Statement") } });
    rules.push_back({ symbols.Get("Expression"), { symbols.Get("OPERATOR") } });
    rules.push_back({ symbols.Get("Expression"), { symbols.Get("NULL") } });

    ruleReg.CreatePseudoRule(symbols);
    for (Rule& r : rules)
    {
        ruleReg.Register(r);
    }
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

void PrintSymbols(const SymbolRegistry& symbols)
{
    std::cout << std::endl;
    for (unsigned int i = 0; i < symbols.size(); i++)
    {
        std::cout << "Symbol " << i << ": " << symbols.GetValue(i) << std::endl;
    }
    std::cout << std::endl;
}

void PrintRule(const Rule& rule, const SymbolRegistry& symbols)
{
    std::cout << symbols.GetValue(rule.head) << " -> ";

    for (Symbol s : rule.body)
    {
        std::cout << symbols.GetValue(s) << " ";
    }

    std::cout << std::endl;
}

void PrintRules(const RuleRegistry& rules, const SymbolRegistry& symbols)
{
    std::cout << std::endl;
    for (RuleId rid : rules)
    {
        const Rule& rule = rules.GetRule(rid);
        PrintRule(rule, symbols);
    }
}

void PrintNullable(const SymbolRegistry& symbols, const RuleProperties& props)
{
    std::cout << std::endl << "NULLABLE:" << std::endl;
    for (Symbol s : symbols)
    {
        std::string name = symbols.GetValue(s);
        bool isnull = props.IsNullable(s);
        std::cout << name << ": " << isnull << std::endl;
    }
}

void PrintFirst(const SymbolRegistry& symbols, const RuleProperties& props)
{
    std::cout << std::endl << "FIRST:" << std::endl;
    for (Symbol s : symbols)
    {
        std::string name = symbols.GetValue(s);
        const SymbolSet& first = props.GetFirst(s);
        std::cout << name << ": ";
        for (Symbol s2 : first)
        {
            std::cout << symbols.GetValue(s2) << " ";
        }
        std::cout << std::endl;
    }
}

void PrintFollow(const SymbolRegistry& symbols, const RuleProperties& props)
{
    std::cout << std::endl << "FOLLOW:" << std::endl;
    for (Symbol s : symbols)
    {
        std::string name = symbols.GetValue(s);
        const SymbolSet& follow = props.GetFollow(s);
        std::cout << name << ": ";
        for (Symbol s2 : follow)
        {
            std::cout << symbols.GetValue(s2) << " ";
        }
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

    symbolRegistry.InitWithTerminals(tokenRegistry);
    SetupSymbols(symbolRegistry);
    SetupRules(ruleRegistry, symbolRegistry);

    PrintLexerAll(lexer, tokenRegistry);

    PrintSymbols(symbolRegistry);

    PrintRules(ruleRegistry, symbolRegistry);

    RuleProperties properties(ruleRegistry, symbolRegistry);
    properties.Compute();

    PrintNullable(symbolRegistry, properties);
    PrintFirst(symbolRegistry, properties);
    PrintFollow(symbolRegistry, properties);

    return 0;
}

