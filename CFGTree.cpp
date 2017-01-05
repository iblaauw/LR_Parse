#include "CFGTree.h"
#include "CFGParseEngine.h"

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

