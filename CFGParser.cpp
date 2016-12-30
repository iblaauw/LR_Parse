#include "CFGParser.h"

#include <cassert>

/// The CFG for parsing the input CFG:
/*  Start state = CFG
    CFG : ProgramList
    ProgramList : null
                : ProgramStatement ProgramList
    ProgramStatement : Statement
                     : OptWhitespace LineEnd
                     : Comment

    OptWhitespace : null
                  : WhitespaceChar OptWhitespace

    Whitespace : WhitespaceChar
               : WhitespaceChar Whitespace

    charset WhitespaceChar : ' ', '\t'

    LineEnd : '\r' '\n'
            : '\n'

    Statement : Rule
              : Charset

    Rule : RuleHead OptWhitespace ':' OptWhitespace RuleBodyList RuleEnd

    RuleHead : OptWhitespace Identifier

    Identifier : IdentifierChars

    IdentifierChars : IdentifierStartChar IdentifierEndChars

    IdentiferEndChars : null
                     : IdentifierEndChar IdentifierEndChars

    RuleBodyList : RuleBody
                 : RuleBody OptWhitespace LineEnd OptWhitespace ':' OptWhitespace RuleBodyList

    RuleBody : RuleToken
             : RuleToken Whitespace RuleBody

    RuleToken : RuleTokenIdentifier
              : Literal

    RuleTokenIdentifier : NullKeyword
                        : CharKeyword

    NullKeyword : 'null'

    CharKeyword : 'CHAR'

    charset IdentifierChar : 'a'-'z', 'A'-'Z', '_'

    charset IdentifierEndChars : IdentifierChar, '0'-'9'

    Literal : QUOTE LiteralChars QUOTE
            : QuoteKeyword

    QuoteKeyword : 'QUOTE'

    LiteralChars : LiteralChar
                 : LiteralChar LiteralChars

    charset LiteralChar except QUOTE : CHAR
*/

void CFGParser::Start(CFGResult (*func)())
{
    JoinNode* node = new JoinNode();

    current = node;
    testing = false;

    CFGResult result = func();
    assert(!result.isTest);
    assert(current == node);
}

CFGResult CFGParser::Do(CFGResult (*func)())
{
    if (testing)
    {
        CFGResult result = func();
        assert(result.isTest);
        return result;
    }
    else
    {
        // Create a new node to represent this action
        JoinNode* node = current;
        JoinNode* newNode = new JoinNode();
        node->children.push_back(newNode);

        // Set new state
        current = newNode;

        // Call the function
        CFGResult result = func();
        assert(!result.isTest);
        assert(current == newNode);

        // Restore back to previous state
        current = node;
        return CFGResult { false, false };
    }
}

bool CFGParser::Is(CFGResult (*func)())
{
    bool prev = testing;
    testing = true;

    CFGResult result = func();
    assert(result.isTest);

    testing = prev;

    return result.testResult;
}

CFGResult CFGParser::Do(bool (*charset)(char))
{
    if (testing)
    {
        bool result = Is(charset);
        return CFGResult { true, result };
    }

    char c = Consume();
    if (!charset(c))
        throw CFGException("Invalid syntax!");

    CharNode* node = new CharNode();
    node->value = c;

    current->children.push_back(node);

    return CFGResult { false, false };
}

bool CFGParser::Is(bool (*charset)(char))
{
    int prev = lookPos;
    char c = GetNext();
    bool result = charset(c);
    lookPos = prev; // restore state
    return result;
}

char CFGParser::GetNext()
{
    char c;
    if (lookPos == lookahead.size())
    {
        c = input.get();
        lookahead.push_back(c);
    }
    else
    {
        c = lookahead[lookPos];
    }

    lookPos++;
    return c;
}

char CFGParser::Consume()
{
    assert(lookPos == 0);
    char c = lookahead.front();
    lookahead.pop_front();
    return c;
}

//void CFGParser::Parse()
//{
//    Start(CFG);
//}
//
//void CFGParser::CFG()
//{
//    ProgramList();
//}
//
//void CFGParser::ProgramList()
//{
//    if (IsProgramStatement())
//    {
//        ProgramStatement();
//        ProgramList();
//    }
//    else
//    {
//        // null
//    }
//}
//
//void CFGParser::ProgramStatement()
//{
//    if (IsStatement())
//    {
//        Statement();
//    }
//    else if (IsOptWhitespace())
//    {
//        OptWhitespace();
//        LineEnd();
//    }
//}
//
//void CFGParser::Whitespace()
//{
//    WhitespaceChar();
//    if (IsWhitespace())
//    {
//        Whitespace();
//    }
//}
//

