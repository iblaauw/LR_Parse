#include "CFGPasses.h"

void CFGPassBase::Start(NodePtr& node)
{
    // Traverse the tree
    node->AcceptPass(*this);
}

void CFGPassBase::Visit(JoinNode& node)
{
    for (const NodePtr& child : node.children)
    {
        child->AcceptPass(*this);
    }
}

void CFGPassBase::Visit(ProgramListNode& node)
{
    for (const NodePtr& child : node.children)
    {
        child->AcceptPass(*this);
    }
}

void CFGPassBase::Visit(LiteralRuleToken& node)
{
    node.literal->AcceptPass(*this);
}

void CFGPassBase::Visit(RuleHeadNode& node)
{
    node.identifier->AcceptPass(*this);
}

void CFGPassBase::Visit(RuleBodyNode& node)
{
    for (const auto& child : node.children)
    {
        child->AcceptPass(*this);
    }
}

void CFGPassBase::Visit(RuleNode& node)
{
    node.head->AcceptPass(*this);
    node.body->AcceptPass(*this);
}

void CFGPassBase::Visit(RangeCharsetToken& node)
{
    node.start->AcceptPass(*this);
    node.end->AcceptPass(*this);
}

void CFGPassBase::Visit(LiteralCharsetToken& node)
{
    node.literal->AcceptPass(*this);
}

void CFGPassBase::Visit(IdentifierCharsetToken& node)
{
    node.identifier->AcceptPass(*this);
}












