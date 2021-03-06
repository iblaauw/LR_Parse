#include "CFGTree.h"
#include "CFGParseEngine.h"
#include "CFGPasses.h"

// Printing statements

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

std::ostream& IdentifierNode::PrintTo(std::ostream& out) const
{
    out << "[ID \"" << value << "\"]";
    return out;
}

std::ostream& LiteralNode::PrintTo(std::ostream& out) const
{
    out << "[Literal '" << value << "']";
    return out;
}

/*static*/ void ProgramListNode::Commit(ParseContext* context)
{
    if (context->Size() == 0)
    {
        context->Commit(utils::make_unique<ProgramListNode>());
    }
    else
    {
        auto& child = context->Get(0);
        auto list = context->AcquireAs<ProgramListNode>(1);
        list->children.push_front(std::move(child));
        context->Commit(std::move(list));
    }
}

std::ostream& ProgramListNode::PrintTo(std::ostream& out) const
{
    out << "[ProgramList" << std::endl;
    for (auto& node : children)
    {
        out << "\t" << *node << std::endl;
    }

    out << "]" << std::endl;

    return out;
}

std::ostream& LiteralRuleToken::PrintTo(std::ostream& out) const
{
    out << "[Token '" << literal->value << "']";
    return out;
}

std::ostream& NullRuleToken::PrintTo(std::ostream& out) const
{
    out << "[Token <null>]";
    return out;
}

std::ostream& CharRuleToken::PrintTo(std::ostream& out) const
{
    out << "[Token <CHAR>]";
    return out;
}

std::ostream& QuoteRuleToken::PrintTo(std::ostream& out) const
{
    out << "[Token <QUOTE>]";
    return out;
}

std::ostream& IdentifierRuleToken::PrintTo(std::ostream& out) const
{
    out << "[Token \"" << identifier->value << "\"]";
    return out;
}

std::ostream& RuleHeadNode::PrintTo(std::ostream& out) const
{
    out << "[RuleHead ";
    if (identifier != nullptr)
    {
        out << '"' << identifier->value << '"';
    }
    out << "]";
    return out;
}

std::ostream& RuleBodyNode::PrintTo(std::ostream& out) const
{
    out << "[RuleBody";
    for (const auto& node : children)
    {
        out << " " << *node;
    }
    out << " ]";
    return out;
}

std::ostream& RuleNode::PrintTo(std::ostream& out) const
{
    out << "[Rule " << *head << " " << *body << " ]";
    return out;
}

std::ostream& RangeCharsetToken::PrintTo(std::ostream& out) const
{
    out << "[Token '" << start->value << "'-'" << end->value << "' ]";
    return out;
}

std::ostream& LiteralCharsetToken::PrintTo(std::ostream& out) const
{
    out << "[Token '" << literal->value << "' ]";
    return out;
}

std::ostream& IdentifierCharsetToken::PrintTo(std::ostream& out) const
{
    out << "[Token '" << identifier->value << "' ]";
    return out;
}

std::ostream& CharCharsetToken::PrintTo(std::ostream& out) const
{
    out << "[Token <CHAR> ]";
    return out;
}

std::ostream& QuoteCharsetToken::PrintTo(std::ostream& out) const
{
    out << "[Token <QUOTE> ]";
    return out;
}

std::ostream& CharsetBodyNode::PrintTo(std::ostream& out) const
{
    out << "[CharsetBody ";
    for (auto& node : children)
    {
        out << *node << " ";
    }
    out << "]";
    return out;
}

std::ostream& CharsetHeadNode::PrintTo(std::ostream& out) const
{
    out << "[CharsetHead \"" << identifier->value << "\" ]";
    return out;
}

std::ostream& CharsetNode::PrintTo(std::ostream& out) const
{
    out << "[Charset " << *head << " " << *body << " ]";
    return out;
}

// Declare Visitor Accept function

#define DEFINE_ACCEPT_METHOD(TYPE) \
    void TYPE::AcceptPass(CFGPassBase& pass) \
    { pass.Visit(*this); }

DEFINE_ACCEPT_METHOD(CFGNode                  )
DEFINE_ACCEPT_METHOD(JoinNode                 )
DEFINE_ACCEPT_METHOD(CharNode                 )
DEFINE_ACCEPT_METHOD(IdentifierNode           )
DEFINE_ACCEPT_METHOD(LiteralNode              )
DEFINE_ACCEPT_METHOD(ProgramListNode          )
DEFINE_ACCEPT_METHOD(LiteralRuleToken         )
DEFINE_ACCEPT_METHOD(NullRuleToken            )
DEFINE_ACCEPT_METHOD(CharRuleToken            )
DEFINE_ACCEPT_METHOD(QuoteRuleToken           )
DEFINE_ACCEPT_METHOD(IdentifierRuleToken      )
DEFINE_ACCEPT_METHOD(RuleHeadNode             )
DEFINE_ACCEPT_METHOD(RuleBodyNode             )
DEFINE_ACCEPT_METHOD(RuleNode                 )
DEFINE_ACCEPT_METHOD(RangeCharsetToken        )
DEFINE_ACCEPT_METHOD(LiteralCharsetToken      )
DEFINE_ACCEPT_METHOD(IdentifierCharsetToken   )
DEFINE_ACCEPT_METHOD(CharCharsetToken         )
DEFINE_ACCEPT_METHOD(QuoteCharsetToken        )
DEFINE_ACCEPT_METHOD(CharsetBodyNode          )
DEFINE_ACCEPT_METHOD(CharsetHeadNode          )
DEFINE_ACCEPT_METHOD(CharsetNode              )


