#include <fstream>
#include <iostream>

#include "Lexer.h"
#include "Token.h"
#include "Rule.h"
#include "RuleProperties.h"

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

void SetupSymbols(SymbolRegistry& symbols)
{
    Symbol start = symbols.Register("Program");
    symbols.SetStartSymbol(start);

    symbols.Register(RuleProperties::NULL_TOKEN);
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

    //Rule rule1 = { symbols.Get("Program"), { symbols.Get("StatementBlock") } };
    //Rule rule2 = { symbols.Get("StatementBlock"), { symbols.Get("StatementBlock"), symbols.Get("Statement") } };
    //Rule rule3 = { symbols.Get("Statement"), { symbols.Get("Expression"), symbols.Get(";") } };
    //Rule rule4 = { symbols.Get("Expression"), { symbols.Get("Expression"), symbols.Get("OPERATOR"), symbols.Get("Expression") } };
    //Rule rule5 = { symbols.Get("Expression"), { symbols.Get("NUMBER") } };
    //Rule rule6 = { symbols.Get("Expression"), { symbols.Get("IDENTIFIER") } };

    //ruleReg.Register(rule1);
    //ruleReg.Register(rule2);
    //ruleReg.Register(rule3);
    //ruleReg.Register(rule4);
    //ruleReg.Register(rule5);
    //ruleReg.Register(rule6);

    std::vector<Rule> rules;

    rules.push_back({ symbols.Get("Program"), { symbols.Get("StatementBlock") } });
    rules.push_back({ symbols.Get("StatementBlock"), { symbols.Get("Statement"), symbols.Get("Expression") } });
    rules.push_back({ symbols.Get("Statement"), { symbols.Get("Expression") } });
    rules.push_back({ symbols.Get("Statement"), { symbols.Get("IDENTIFIER") } });
    rules.push_back({ symbols.Get("Expression"), { symbols.Get("Statement") } });
    rules.push_back({ symbols.Get("Expression"), { symbols.Get("OPERATOR") } });
    rules.push_back({ symbols.Get("Expression"), { symbols.Get("NULL") } });

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

    PrintRules(ruleRegistry, symbolRegistry);

    RuleProperties properties(ruleRegistry, symbolRegistry);
    properties.Compute();

    PrintNullable(symbolRegistry, properties);

    return 0;
}

