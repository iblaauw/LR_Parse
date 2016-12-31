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

CFGResult CFG(ParseContext* context)
{
    return { false, false };
}


CFGParser::CFGParser(std::istream& input) : input(input)
{}

void CFGParser::Parse()
{
    ParseContext context(input);
    context.Start(CFG);
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

