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

    for (const NodePtr& child : children)
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

/*
    START at CFG

    // Whitespace + LineEnd

    OptWhitespace : null
                  : Whitespace

    Whitespace : WhitespaceChar
               : WhitespaceChar Whitespace

    charset WhitespaceChar : ' ', '\t'

    LineEnd : '\r' '\n'
            : '\n'

    EmptyLine : OptWhitespace LineEnd

    // Identifier

    Identifier : IdentifierChars

    IdentifierChars : IdentifierChar IdentifierEndChars

    IdentiferEndChars : null
                     : IdentifierEndChar IdentifierEndChars

    charset IdentifierChar : 'a'-'z', 'A'-'Z', '_'

    charset IdentifierEndChars : IdentifierChar, '0'-'9'

    // CFG

    CFG : ProgramList
    ProgramList : null
                : ProgramStatement ProgramList

    ProgramStatement : Statement
                     : EmptyLine

    // Statement
    Statement : OptWhitespace Charset
              : OptWhitespace Rule

    // Rule
    Rule : RuleHead OptWhitespace ':' OptWhitespace RuleBody

    RuleHead : Identifier
             : null

    RuleBody : RuleTokenList

    RuleTokenList : RuleToken
                  : RuleToken RuleTokenListEnd

    RuleTokenListEnd : WhiteSpace RuleTokenList

    // RuleToken
    RuleToken : LiteralToken
              : RuleTokenIdentifier

    // Literal
    Literal : QUOTE LiteralCharList QUOTE

    LiteralCharList : LiteralChar
                    : LiteralChar LiteralCharList

    charset LiteralChar : CHAR except QUOTE, '\n', '\r'

    RuleTokenIdentifier : NullKeyword
                        : CharKeyword
                        : QuoteKeyword
                        : Identifier

    // Token Keywords
    NullKeyword : 'null'
    CharKeyword : 'CHAR'
    QuoteKeyword : 'QUOTE'

    // Charset
    Charset : CharsetHead OptWhitespace ':' OptWhitespace CharsetBody

    CharsetHead : 'charset' Whitespace Identifier

    CharsetBody : CharsetBodyList

    CharsetBodyList : CharsetToken
                    : CharsetToken CharsetBodyEnd

    CharsetBodyEnd : OptWhitespace ',' OptWhitespace CharsetBodyList

    CharsetToken : CharsetRange
                 : Literal
                 : CharKeyword
                 : QuoteKeyword
                 : Identifier

    CharsetRange : Literal '-' Literal
            // Note: NO whitespace, invalid ranges checked in semantics (i.e. 'abc'-'123')

*/




// Utilities, identifiers, whitespace

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

bool LiteralChar(char c) { return c != '\'' && c != '\n' && c != '\r'; }

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

void LineEnd(ParseContext* context)
{
    context->AutoName();

    if (context->Is(DoLiteral<'\r'>))
    {
        context->Do(DoLiteral<'\r'>);
    }
    context->Do(DoLiteral<'\n'>);
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
        std::unique_ptr<CharNode> cnode = context->AcquireAs<CharNode>(0);
        if (context->Size() == 1)
        {
            auto node = utils::make_unique<IdentifierNode>();
            node->value += cnode->value;
            context->Commit(std::move(node));
        }
        else
        {
            auto node = context->AcquireAs<IdentifierNode>(1);
            node->value = cnode->value + node->value;
            context->Commit(std::move(node));
        }
    }

}

void EmptyLine(ParseContext* context)
{
    context->AutoName();
    context->Do(OptWhitespace);
    context->Do(LineEnd);
}


// Keywords
void NullKeyword(ParseContext* context)
{
    context->AutoName();
    context->Do(DoLiteral<'n'>);
    context->Do(DoLiteral<'u'>);
    context->Do(DoLiteral<'l'>);
    context->Do(DoLiteral<'l'>);
}

void CharKeyword(ParseContext* context)
{
    context->AutoName();
    context->Do(DoLiteral<'C'>);
    context->Do(DoLiteral<'H'>);
    context->Do(DoLiteral<'A'>);
    context->Do(DoLiteral<'R'>);
}

void QuoteKeyword(ParseContext* context)
{
    context->AutoName();
    context->Do(DoLiteral<'Q'>);
    context->Do(DoLiteral<'U'>);
    context->Do(DoLiteral<'O'>);
    context->Do(DoLiteral<'T'>);
    context->Do(DoLiteral<'E'>);
}

// Literal

void LiteralCharList(ParseContext* context)
{
    context->Do(LiteralChar);
    if (context->Is(LiteralCharList))
    {
        context->Do(LiteralCharList);
    }

    if (context->IsCommitting())
    {
        auto charNode = context->AcquireAs<CharNode>(0);
        std::unique_ptr<LiteralNode> node;
        if (context->Size() == 1)
        {
            node = utils::make_unique<LiteralNode>();
        }
        else
        {
            node = context->AcquireAs<LiteralNode>(1);
        }

        node->value = charNode->value + node->value;

        context->Commit(std::move(node));
    }
}

void Literal(ParseContext* context)
{
    context->Do(DoLiteral<'\''>);
    context->Do(LiteralCharList);
    context->Do(DoLiteral<'\''>);

    if (context->IsCommitting())
    {
        auto node = context->AcquireAs<LiteralNode>(1);
        context->Commit(std::move(node));
    }
}

////**** Core Grammar ****////

// Rules

void RuleTokenIdentifier(ParseContext* context)
{
    context->AutoName();
    if (context->Is(NullKeyword))
    {
        context->Do(NullKeyword);
    }
    else if (context->Is(CharKeyword))
    {
        context->Do(CharKeyword);
    }
    else if (context->Is(QuoteKeyword))
    {
        context->Do(QuoteKeyword);
    }
    else
    {
        context->Do(Identifier);
    }
}

void RuleToken(ParseContext* context)
{
    context->AutoName();

    if (context->Is(Literal))
    {
        context->Do(Literal);
    }
    else
    {
        context->Do(RuleTokenIdentifier);
    }
}

void RuleTokenList(ParseContext* context);

void RuleTokenListEnd(ParseContext* context)
{
    context->AutoName();
    context->Do(Whitespace);
    context->Do(RuleTokenList);
}

void RuleTokenList(ParseContext* context)
{
    context->AutoName();
    context->Do(RuleToken);
    if (context->Is(RuleTokenListEnd))
    {
        context->Do(RuleTokenListEnd);
    }
}

void RuleBody(ParseContext* context)
{
    context->AutoName();
    context->Do(RuleTokenList);
}

void RuleHead(ParseContext* context)
{
    context->AutoName();

    if (context->Is(Identifier))
    {
        context->Do(Identifier);
    }
}

void Rule(ParseContext* context)
{
    context->AutoName();
    context->Do(RuleHead);
    context->Do(OptWhitespace);
    context->Do(DoLiteral<':'>);
    context->Do(OptWhitespace);
    context->Do(RuleBody);
}

// Charset

void CharsetRange(ParseContext* context)
{
    context->AutoName();

    context->Do(Literal);
    context->Do(DoLiteral<'-'>);
    context->Do(Literal);
}

void CharsetToken(ParseContext* context)
{
    context->AutoName();

    if (context->Is(CharsetRange))
    {
        context->Do(CharsetRange);
    }
    else if (context->Is(Literal))
    {
        context->Do(Literal);
    }
    else if (context->Is(CharKeyword))
    {
        context->Do(CharKeyword);
    }
    else if (context->Is(QuoteKeyword))
    {
        context->Do(QuoteKeyword);
    }
    else
    {
        context->Do(Identifier);
    }
}

void CharsetBodyList(ParseContext* context);

void CharsetBodyEnd(ParseContext* context)
{
    context->AutoName();

    context->Do(OptWhitespace);
    context->Do(DoLiteral<','>);
    context->Do(OptWhitespace);
    context->Do(CharsetBodyList);
}

void CharsetBodyList(ParseContext* context)
{
    context->AutoName();

    context->Do(CharsetToken);
    if (context->Is(CharsetBodyEnd))
    {
        context->Do(CharsetBodyEnd);
    }
}

void CharsetBody(ParseContext* context)
{
    context->AutoName();

    context->Do(CharsetBodyList);
}

void CharsetHead(ParseContext* context)
{
    context->AutoName();

    context->Do(DoLiteral<'c'>);
    context->Do(DoLiteral<'h'>);
    context->Do(DoLiteral<'a'>);
    context->Do(DoLiteral<'r'>);
    context->Do(DoLiteral<'s'>);
    context->Do(DoLiteral<'e'>);
    context->Do(DoLiteral<'t'>);

    context->Do(Whitespace);
    context->Do(Identifier);
}

void Charset(ParseContext* context)
{
    context->AutoName();
    context->Do(CharsetHead);
    context->Do(OptWhitespace);
    context->Do(DoLiteral<':'>);
    context->Do(OptWhitespace);
    context->Do(CharsetBody);
}

// Program Statements

void Statement(ParseContext* context)
{
    context->AutoName();
    context->Do(OptWhitespace);
    if (context->Is(Charset))
    {
        context->Do(Charset);
    }
    else
    {
        context->Do(Rule);
    }
}

void ProgramStatement(ParseContext* context)
{
    context->AutoName();
    if (context->Is(Statement))
    {
        context->Do(Statement);
    }
    else
    {
        context->Do(EmptyLine);
    }
}

void ProgramList(ParseContext* context)
{
    //context->AutoName();

    if (context->Is(ProgramStatement))
    {
        context->Do(ProgramStatement);
        context->Do(ProgramList);
    }

    if (context->IsCommitting())
    {
        ProgramListNode::Commit(context);
    }
}

void CFG(ParseContext* context)
{
    context->AutoName();
    context->Do(ProgramList);
}

/*
    As : ASeq ATerm

    ASeq : A ASeqEnd
         : A

    ASeqEnd : B ASeq

*/

CFGParser::CFGParser(std::istream& input) : input(input)
{}

void CFGParser::Parse()
{
    ParseContext context(input);
    //auto* result = context.Start(Statement2);
    auto* result = context.Start(CFG);
    //auto* result = context.Start(A);

    std::cout << *result << std::endl;
}

