#include "CFGParser.h"

#include <cassert>

#include "message_exception.h"
#include "CFGParseEngine.h"

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

template <char C>
bool DoLiteral(char c) { return c == C; }

bool IdentifierChar(char c)
{
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c == '_');
}

bool IdentifierEndChars(char c)
{
    return IdentifierChar(c) || (c >= '0' && c <= '9');
}

bool LiteralChar(char c) { return c != '\''; }

bool WhitespaceChar(char c) { return c == ' ' || c == '\t'; }

void Whitespace(ParseContext* context)
{
    std::cout << "Whitespace" << std::endl;

    context->Do(WhitespaceChar);
    if (context->Is(Whitespace))
    {
        context->Do(Whitespace);
    }
}

void OptWhitespace(ParseContext* context)
{
    std::cout << "OptWhitespace" << std::endl;

    if (context->Is(Whitespace))
    {
        context->Do(Whitespace);
    }
}

void Identifier(ParseContext* context)
{
    std::cout << "Identifier" << std::endl;

    context->Do(IdentifierChar);
    if (context->Is(Identifier))
    {
        context->Do(Identifier);
    }
}

/*
    As : ASeq ATerm

    ASeq : A ASeqEnd
         : A

    ASeqEnd : B ASeq

*/

void StatementSequence(ParseContext* context);

void StatementSequenceEnd(ParseContext* context)
{
    std::cout << "Statement Seq End" << std::endl;

    context->Do(Whitespace);
    context->Do(StatementSequence);
}

void StatementSequence(ParseContext* context)
{
    std::cout << "Statement Seq" << std::endl;

    context->Do(Identifier);
    if (context->Is(StatementSequenceEnd))
    {
        context->Do(StatementSequenceEnd);
    }
}

void Statement(ParseContext* context)
{
    std::cout << "Statement" << std::endl;
    context->Do(OptWhitespace);
    context->Do(StatementSequence);
    context->Do(OptWhitespace);
    context->Do(DoLiteral<';'>);
}

CFGParser::CFGParser(std::istream& input) : input(input)
{}

void CFGParser::Parse()
{
    ParseContext context(input);
    context.Start(Statement);
}


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

