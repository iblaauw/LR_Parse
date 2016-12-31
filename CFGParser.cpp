#include "CFGParser.h"

#include <cassert>

#include "message_exception.h"
#include "CFGParseEngine.h"
#include "CFGTree.h"

/****** Print functions for nodes ******/

std::ostream& JoinNode::PrintTo(std::ostream& out) const
{
    out << "[";
    out << name;

    for (CFGNode* child : children)
    {
        out << " ";
        out << *child;
    }

    out << " ]";

    return out;
}

std::ostream& CharNode::PrintTo(std::ostream& out) const
{
    out << '\'' << value << '\'';
    return out;
}

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
    context->AutoName();

    context->Do(WhitespaceChar);
    if (context->Is(Whitespace))
    {
        context->Do(Whitespace);
    }
}

void OptWhitespace(ParseContext* context)
{
    context->AutoName();

    if (context->Is(Whitespace))
    {
        context->Do(Whitespace);
    }
}

void Identifier(ParseContext* context)
{
    //context->AutoName();

    context->Do(IdentifierChar);
    if (context->Is(Identifier))
    {
        context->Do(Identifier);
    }

    if (context->IsCommitting())
    {
        CharNode* cnode = static_cast<CharNode*>(context->Get(0));
        if (context->Size() == 1)
        {
            IdentifierNode* node = new IdentifierNode();
            node->value += cnode->value;
            context->Commit(node);
        }
        else
        {
            IdentifierNode* node = static_cast<IdentifierNode*>(context->Get(1));
            node->value = cnode->value + node->value;
            context->Commit(node);
        }
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
    context->AutoName();

    context->Do(Whitespace, true);
    context->Do(StatementSequence);
}

void StatementSequence(ParseContext* context)
{
    context->AutoName();

    context->Do(Identifier);
    if (context->Is(StatementSequenceEnd))
    {
        context->Do(StatementSequenceEnd);
    }
}

void Statement(ParseContext* context)
{
    context->AutoName();

    context->Do(OptWhitespace, true);
    context->Do(StatementSequence);
    context->Do(OptWhitespace, true);
    context->Do(DoLiteral<';'>);
}

CFGParser::CFGParser(std::istream& input) : input(input)
{}

void CFGParser::Parse()
{
    ParseContext context(input);
    auto* result = context.Start(Statement);

    std::cout << *result << std::endl;
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

