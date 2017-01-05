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
    if (context->Is(NullKeyword))
    {
        context->Do(NullKeyword);
        if (context->IsCommitting())
        {
            context->Commit(utils::make_unique<NullRuleToken>());
        }
    }
    else if (context->Is(CharKeyword))
    {
        context->Do(CharKeyword);
        if (context->IsCommitting())
        {
            context->Commit(utils::make_unique<CharRuleToken>());
        }
    }
    else if (context->Is(QuoteKeyword))
    {
        context->Do(QuoteKeyword);
        if (context->IsCommitting())
        {
            context->Commit(utils::make_unique<QuoteRuleToken>());
        }
    }
    else
    {
        context->Do(Identifier);
        if (context->IsCommitting())
        {
            auto id = context->AcquireAs<IdentifierNode>(0);
            auto node = utils::make_unique<IdentifierRuleToken>();
            node->identifier = std::move(id);
            context->Commit(std::move(node));
        }
    }
}

void RuleToken(ParseContext* context)
{
    if (context->Is(Literal))
    {
        context->Do(Literal);
        if (context->IsCommitting())
        {
            auto literal = context->AcquireAs<LiteralNode>(0);
            auto node = utils::make_unique<LiteralRuleToken>();
            node->literal = std::move(literal);
            context->Commit(std::move(node));
        }
    }
    else
    {
        context->Do(RuleTokenIdentifier);
        if (context->IsCommitting())
        {
            context->Commit(context->AcquireAs<CFGNode>(0));
        }
    }
}

void RuleTokenList(ParseContext* context);

void RuleTokenListEnd(ParseContext* context)
{
    context->Do(Whitespace, true);
    context->Do(RuleTokenList);

    if (context->IsCommitting())
    {
        // Forward
        context->Commit(context->AcquireAs<CFGNode>(0));
    }
}

void RuleTokenList(ParseContext* context)
{
    context->Do(RuleToken);
    if (context->Is(RuleTokenListEnd))
    {
        context->Do(RuleTokenListEnd);
    }

    if (context->IsCommitting())
    {
        auto token = context->AcquireAs<RuleTokenNode>(0);
        std::unique_ptr<RuleBodyNode> node;
        if (context->Size() == 1)
        {
            node = utils::make_unique<RuleBodyNode>();
        }
        else
        {
            node = context->AcquireAs<RuleBodyNode>(1);
        }
        node->children.push_front(std::move(token));
        context->Commit(std::move(node));
    }
}

void RuleBody(ParseContext* context)
{
    context->Do(RuleTokenList);

    if (context->IsCommitting())
    {
        // Forward
        context->Commit(context->AcquireAs<CFGNode>(0));
    }
}

void RuleHead(ParseContext* context)
{
    if (context->Is(Identifier))
    {
        context->Do(Identifier);
    }

    if (context->IsCommitting())
    {
        auto node = utils::make_unique<RuleHeadNode>();
        if (context->Size() > 0)
        {
            node->identifier = context->AcquireAs<IdentifierNode>(0);
        }
        context->Commit(std::move(node));
    }
}

void Rule(ParseContext* context)
{
    context->Do(RuleHead);
    context->Do(OptWhitespace, true);
    context->Do(DoLiteral<':'>, true);
    context->Do(OptWhitespace, true);
    context->Do(RuleBody);

    if (context->IsCommitting())
    {
        auto node = utils::make_unique<RuleNode>();
        node->head = context->AcquireAs<RuleHeadNode>(0);
        node->body = context->AcquireAs<RuleBodyNode>(1);
        context->Commit(std::move(node));
    }
}

// Charset

void CharsetRange(ParseContext* context)
{
    context->Do(Literal);
    context->Do(DoLiteral<'-'>);
    context->Do(Literal);

    if (context->IsCommitting())
    {
        auto node = utils::make_unique<RangeCharsetToken>();
        node->start = context->AcquireAs<LiteralNode>(0);
        node->end = context->AcquireAs<LiteralNode>(2);
        context->Commit(std::move(node));
    }
}

void CharsetToken(ParseContext* context)
{
    context->AutoName();

    if (context->Is(CharsetRange))
    {
        context->Do(CharsetRange);
        if (context->IsCommitting())
        {
            context->Commit(context->AcquireAs<CFGNode>(0)); // Forward
        }
    }
    else if (context->Is(Literal))
    {
        context->Do(Literal);
        if (context->IsCommitting())
        {
            auto node = utils::make_unique<LiteralCharsetToken>();
            node->literal = context->AcquireAs<LiteralNode>(0);
            context->Commit(std::move(node));
        }
    }
    else if (context->Is(CharKeyword))
    {
        context->Do(CharKeyword);
        if (context->IsCommitting())
        {
            context->Commit(utils::make_unique<CharCharsetToken>());
        }
    }
    else if (context->Is(QuoteKeyword))
    {
        context->Do(QuoteKeyword);
        if (context->IsCommitting())
        {
            context->Commit(utils::make_unique<QuoteCharsetToken>());
        }
    }
    else
    {
        context->Do(Identifier);
        if (context->IsCommitting())
        {
            auto node = utils::make_unique<IdentifierCharsetToken>();
            node->identifier = context->AcquireAs<IdentifierNode>(0);
            context->Commit(std::move(node));
        }
    }
}

void CharsetBodyList(ParseContext* context);

void CharsetBodyEnd(ParseContext* context)
{
    context->Do(OptWhitespace, true);
    context->Do(DoLiteral<','>, true);
    context->Do(OptWhitespace, true);
    context->Do(CharsetBodyList);

    if (context->IsCommitting())
    {
        //Forward
        context->Commit(context->AcquireAs<CFGNode>(0));
    }
}

void CharsetBodyList(ParseContext* context)
{
    context->Do(CharsetToken);
    if (context->Is(CharsetBodyEnd))
    {
        context->Do(CharsetBodyEnd);
    }

    if (context->IsCommitting())
    {
        auto token = context->AcquireAs<CharsetTokenNode>(0);
        std::unique_ptr<CharsetBodyNode> node;
        if (context->Size() == 1)
        {
            node = utils::make_unique<CharsetBodyNode>();
        }
        else
        {
            node = context->AcquireAs<CharsetBodyNode>(1);
        }

        node->children.push_front(std::move(token));
        context->Commit(std::move(node));
    }
}

void CharsetBody(ParseContext* context)
{
    context->Do(CharsetBodyList);

    if (context->IsCommitting())
    {
        context->Commit(context->AcquireAs<CFGNode>(0)); // Forward
    }
}

void CharsetHead(ParseContext* context)
{
    context->Do(DoLiteral<'c'>, true);
    context->Do(DoLiteral<'h'>, true);
    context->Do(DoLiteral<'a'>, true);
    context->Do(DoLiteral<'r'>, true);
    context->Do(DoLiteral<'s'>, true);
    context->Do(DoLiteral<'e'>, true);
    context->Do(DoLiteral<'t'>, true);

    context->Do(Whitespace, true);
    context->Do(Identifier);

    if (context->IsCommitting())
    {
        auto node = utils::make_unique<CharsetHeadNode>();
        node->identifier = context->AcquireAs<IdentifierNode>(0);
        context->Commit(std::move(node));
    }
}

void Charset(ParseContext* context)
{
    context->Do(CharsetHead);
    context->Do(OptWhitespace, true);
    context->Do(DoLiteral<':'>, true);
    context->Do(OptWhitespace, true);
    context->Do(CharsetBody);

    if (context->IsCommitting())
    {
        auto node = utils::make_unique<CharsetNode>();
        node->head = context->AcquireAs<CharsetHeadNode>(0);
        node->body = context->AcquireAs<CharsetBodyNode>(1);
        context->Commit(std::move(node));
    }
}

// Program Statements

void Statement(ParseContext* context)
{
    context->AutoName();
    context->Do(OptWhitespace, true);
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
        context->Do(EmptyLine, true);
    }

    if (context->IsCommitting())
    {
        if (context->Size() > 0)
        {
            // Forward
            context->Commit(context->AcquireAs<CFGNode>(0));
        }
    }
}

void ProgramList(ParseContext* context)
{
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

