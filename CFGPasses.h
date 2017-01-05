#pragma once

#include "CFGTree.h"

class CFGPassBase
{
public:
    void Start(NodePtr& root);

    virtual void Visit(CFGNode                 & node) { }
    virtual void Visit(JoinNode                & node) { }
    virtual void Visit(CharNode                & node) { }
    virtual void Visit(IdentifierNode          & node) { }
    virtual void Visit(LiteralNode             & node) { }
    virtual void Visit(ProgramListNode         & node) { }
    virtual void Visit(LiteralRuleToken        & node) { }
    virtual void Visit(NullRuleToken           & node) { }
    virtual void Visit(CharRuleToken           & node) { }
    virtual void Visit(QuoteRuleToken          & node) { }
    virtual void Visit(IdentifierRuleToken     & node) { }
    virtual void Visit(RuleHeadNode            & node) { }
    virtual void Visit(RuleBodyNode            & node) { }
    virtual void Visit(RuleNode                & node) { }
    virtual void Visit(RangeCharsetToken       & node) { }
    virtual void Visit(LiteralCharsetToken     & node) { }
    virtual void Visit(IdentifierCharsetToken  & node) { }
    virtual void Visit(CharCharsetToken        & node) { }
    virtual void Visit(QuoteCharsetToken       & node) { }
    virtual void Visit(CharsetBodyNode         & node) { }
    virtual void Visit(CharsetHeadNode         & node) { }
    virtual void Visit(CharsetNode             & node) { }
};

