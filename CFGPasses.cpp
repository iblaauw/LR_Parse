#include "CFGPasses.h"

void CFGPassBase::Start(NodePtr& node)
{
    // Traverse the tree
    node->AcceptPass(*this);
}

//void CFGPassBase::Visit(CFGNode& node)
//{
//    // NOOP
//}
//
//void CFGPassBase::Visit(JoinNode& node)
//{
//    for (const NodePtr& child : node.children)
//    {
//        child.AcceptPass(*this);
//    }
//}
