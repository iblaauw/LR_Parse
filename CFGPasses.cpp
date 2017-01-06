#include "CFGPasses.h"

//void CFGPassBase::Start(NodePtr& node)
//{
//    // Traverse the tree
//    node->AcceptPass(*this);
//}
//
//void CFGPassBase::Visit(JoinNode& node)
//{
//    for (const NodePtr& child : node.children)
//    {
//        child->AcceptPass(*this);
//    }
//}
//
//void CFGPassBase::Visit(ProgramListNode& node)
//{
//    for (const NodePtr& child : node.children)
//    {
//        child->AcceptPass(*this);
//    }
//}
//
//void CFGPassBase::Visit(LiteralRuleToken& node)
//{
//    node.literal->AcceptPass(*this);
//}
//
//void CFGPassBase::Visit(RuleHeadNode& node)
//{
//    node.identifier->AcceptPass(*this);
//}
//
//void CFGPassBase::Visit(RuleBodyNode& node)
//{
//    for (const auto& child : node.children)
//    {
//        child->AcceptPass(*this);
//    }
//}
//
//void CFGPassBase::Visit(RuleNode& node)
//{
//    node.head->AcceptPass(*this);
//    node.body->AcceptPass(*this);
//}
//
//void CFGPassBase::Visit(RangeCharsetToken& node)
//{
//    node.start->AcceptPass(*this);
//    node.end->AcceptPass(*this);
//}
//
//void CFGPassBase::Visit(LiteralCharsetToken& node)
//{
//    node.literal->AcceptPass(*this);
//}
//
//void CFGPassBase::Visit(IdentifierCharsetToken& node)
//{
//    node.identifier->AcceptPass(*this);
//}

#include "type_trait_utils.h"

DEF_HAS_MEMBER_VARIABLE(HasChildren, children);
DEF_HAS_MEMBER_VARIABLE(HasIdentifier, identifier);
DEF_HAS_MEMBER_VARIABLE(HasLiteral, literal);

template <class Node>
typename std::enable_if<HasChildren<Node>::value, void>::type
DoVisitImpl(CFGPassBase* pass, Node& node)
{
    
}

template <class Node>
typename std::enable_if<HasIdentifier<Node>::value, void>::type
DoVisitImpl(CFGPassBase* pass, Node& node)
{
}

template <class Node>
typename std::enable_if<HasLiteral<Node>::value, void>::type
DoVisitImpl(CFGPassBase* pass, Node& node)
{
}

template <class Node>
typename std::enable_if<ConstexprAnd<!HasChildren<Node>::value, !HasIdentifier<Node>::value, !HasLiteral<Node>::value>::value, void>::type
DoVisitImpl(CFGPassBase* pass, Node& node)
{
}

template <class Node>
void CFGPassBase::DoVisit(Node& node)
{

}


void CFGPassBase::Visit(CFGNode               & node){ DoVisit<CFGNode               >(node);}
void CFGPassBase::Visit(JoinNode              & node){ DoVisit<JoinNode              >(node);}
void CFGPassBase::Visit(CharNode              & node){ DoVisit<CharNode              >(node);}
void CFGPassBase::Visit(IdentifierNode        & node){ DoVisit<IdentifierNode        >(node);}
void CFGPassBase::Visit(LiteralNode           & node){ DoVisit<LiteralNode           >(node);}
void CFGPassBase::Visit(ProgramListNode       & node){ DoVisit<ProgramListNode       >(node);}
void CFGPassBase::Visit(LiteralRuleToken      & node){ DoVisit<LiteralRuleToken      >(node);}
void CFGPassBase::Visit(NullRuleToken         & node){ DoVisit<NullRuleToken         >(node);}
void CFGPassBase::Visit(CharRuleToken         & node){ DoVisit<CharRuleToken         >(node);}
void CFGPassBase::Visit(QuoteRuleToken        & node){ DoVisit<QuoteRuleToken        >(node);}
void CFGPassBase::Visit(IdentifierRuleToken   & node){ DoVisit<IdentifierRuleToken   >(node);}
void CFGPassBase::Visit(RuleHeadNode          & node){ DoVisit<RuleHeadNode          >(node);}
void CFGPassBase::Visit(RuleBodyNode          & node){ DoVisit<RuleBodyNode          >(node);}
void CFGPassBase::Visit(RuleNode              & node){ DoVisit<RuleNode              >(node);}
void CFGPassBase::Visit(RangeCharsetToken     & node){ DoVisit<RangeCharsetToken     >(node);}
void CFGPassBase::Visit(LiteralCharsetToken   & node){ DoVisit<LiteralCharsetToken   >(node);}
void CFGPassBase::Visit(IdentifierCharsetToken& node){ DoVisit<IdentifierCharsetToken>(node);}
void CFGPassBase::Visit(CharCharsetToken      & node){ DoVisit<CharCharsetToken      >(node);}
void CFGPassBase::Visit(QuoteCharsetToken     & node){ DoVisit<QuoteCharsetToken     >(node);}
void CFGPassBase::Visit(CharsetBodyNode       & node){ DoVisit<CharsetBodyNode       >(node);}
void CFGPassBase::Visit(CharsetHeadNode       & node){ DoVisit<CharsetHeadNode       >(node);}
void CFGPassBase::Visit(CharsetNode           & node){ DoVisit<CharsetNode           >(node);}



//template <class T>
//struct HasIdentifier
//{
//private:
//    template <class U>
//    static auto test(int) -> decltype(((U*)nullptr)->identifier);
//    template <class U>
//    static void test(...);
//public:
//    static constexpr bool value = !std::is_same<void, decltype(test<T>(0))>::value;
//};







