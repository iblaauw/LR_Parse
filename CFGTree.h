#pragma once

// This file contains the AST for the generated CFG

#include <forward_list>

#include "CFGParser.h"

class ParseContext;

class IdentifierNode : public CFGNode
{
public:
    std::string value;
protected:
    std::ostream& PrintTo(std::ostream& out) const override;
};

class LiteralNode : public CFGNode
{
public:
    std::string value;
protected:
    std::ostream& PrintTo(std::ostream& out) const override;
};

class ProgramListNode : public CFGNode
{
public:
    std::forward_list<NodePtr> children;
    static void Commit(ParseContext* context);
protected:
    std::ostream& PrintTo(std::ostream& out) const override;
};

class RuleTokenNode : public CFGNode
{
};

class LiteralRuleToken : public RuleTokenNode
{
public:
    std::unique_ptr<LiteralNode> literal;
protected:
    std::ostream& PrintTo(std::ostream& out) const override;
};

class NullRuleToken : public RuleTokenNode
{
protected:
    std::ostream& PrintTo(std::ostream& out) const override;
};

class CharRuleToken : public RuleTokenNode
{
protected:
    std::ostream& PrintTo(std::ostream& out) const override;
};

class QuoteRuleToken : public RuleTokenNode
{
protected:
    std::ostream& PrintTo(std::ostream& out) const override;
};

class IdentifierRuleToken : public RuleTokenNode
{
public:
    std::unique_ptr<IdentifierNode> identifier;
protected:
    std::ostream& PrintTo(std::ostream& out) const override;
};

class RuleHeadNode :public CFGNode
{
public:
    std::unique_ptr<IdentifierNode> identifier; // is nullable!!
protected:
    std::ostream& PrintTo(std::ostream& out) const override;
};

class RuleBodyNode : public CFGNode
{
public:
    std::forward_list<std::unique_ptr<RuleTokenNode>> children;
protected:
    std::ostream& PrintTo(std::ostream& out) const override;
};


