#pragma once

// This file contains the AST for the generated CFG

#include <forward_list>

#include "CFGParser.h"

#define DECLARE_ACCEPT_METHOD \
    void AcceptPass(CFGPassBase& pass) override;

class ParseContext;

class CFGPassBase;

// Core

class CFGNode
{
public:
    friend std::ostream& operator<<(std::ostream& out, const CFGNode& node);
    virtual void AcceptPass(CFGPassBase& pass) = 0;
protected:
    virtual std::ostream& PrintTo(std::ostream& out) const = 0;
};

using NodePtr = std::unique_ptr<CFGNode>;

inline std::ostream& operator<<(std::ostream& out, const CFGNode& node)
{
    return node.PrintTo(out);
}


class JoinNode : public CFGNode
{
public:
    std::vector<NodePtr> children;
    std::string name;

    DECLARE_ACCEPT_METHOD
protected:
    std::ostream& PrintTo(std::ostream& out) const override;
};

class CharNode : public CFGNode
{
public:
    char value;

    DECLARE_ACCEPT_METHOD
protected:
    std::ostream& PrintTo(std::ostream& out) const override;
};

class IdentifierNode : public CFGNode
{
public:
    std::string value;
    DECLARE_ACCEPT_METHOD
protected:
    std::ostream& PrintTo(std::ostream& out) const override;
};

class LiteralNode : public CFGNode
{
public:
    std::string value;
    DECLARE_ACCEPT_METHOD
protected:
    std::ostream& PrintTo(std::ostream& out) const override;
};

class ProgramListNode : public CFGNode
{
public:
    std::forward_list<NodePtr> children;
    static void Commit(ParseContext* context);
    DECLARE_ACCEPT_METHOD
protected:
    std::ostream& PrintTo(std::ostream& out) const override;
};

// Rules

class RuleTokenNode : public CFGNode
{
};

class LiteralRuleToken : public RuleTokenNode
{
public:
    std::unique_ptr<LiteralNode> literal;
    DECLARE_ACCEPT_METHOD
protected:
    std::ostream& PrintTo(std::ostream& out) const override;
};

class NullRuleToken : public RuleTokenNode
{
public:
    DECLARE_ACCEPT_METHOD
protected:
    std::ostream& PrintTo(std::ostream& out) const override;
};

class CharRuleToken : public RuleTokenNode
{
public:
    DECLARE_ACCEPT_METHOD
protected:
    std::ostream& PrintTo(std::ostream& out) const override;
};

class QuoteRuleToken : public RuleTokenNode
{
public:
    DECLARE_ACCEPT_METHOD
protected:
    std::ostream& PrintTo(std::ostream& out) const override;
};

class IdentifierRuleToken : public RuleTokenNode
{
public:
    std::unique_ptr<IdentifierNode> identifier;
    DECLARE_ACCEPT_METHOD
protected:
    std::ostream& PrintTo(std::ostream& out) const override;
};

class RuleHeadNode :public CFGNode
{
public:
    std::unique_ptr<IdentifierNode> identifier; // is nullable!!
    DECLARE_ACCEPT_METHOD
protected:
    std::ostream& PrintTo(std::ostream& out) const override;
};

class RuleBodyNode : public CFGNode
{
public:
    std::forward_list<std::unique_ptr<RuleTokenNode>> children;
    DECLARE_ACCEPT_METHOD
protected:
    std::ostream& PrintTo(std::ostream& out) const override;
};

class RuleNode : public CFGNode
{
public:
    std::unique_ptr<RuleHeadNode> head;
    std::unique_ptr<RuleBodyNode> body;
    DECLARE_ACCEPT_METHOD
protected:
    std::ostream& PrintTo(std::ostream& out) const override;
};

// Charsets

class CharsetTokenNode : public CFGNode
{};

class RangeCharsetToken : public CharsetTokenNode
{
public:
    std::unique_ptr<LiteralNode> start, end;
    DECLARE_ACCEPT_METHOD
protected:
    std::ostream& PrintTo(std::ostream& out) const override;
};

class LiteralCharsetToken : public CharsetTokenNode
{
public:
    std::unique_ptr<LiteralNode> literal;
    DECLARE_ACCEPT_METHOD
protected:
    std::ostream& PrintTo(std::ostream& out) const override;
};

class IdentifierCharsetToken : public CharsetTokenNode
{
public:
    std::unique_ptr<IdentifierNode> identifier;
    DECLARE_ACCEPT_METHOD
protected:
    std::ostream& PrintTo(std::ostream& out) const override;
};

class CharCharsetToken : public CharsetTokenNode
{
public:
    DECLARE_ACCEPT_METHOD
protected:
    std::ostream& PrintTo(std::ostream& out) const override;
};

class QuoteCharsetToken : public CharsetTokenNode
{
public:
    DECLARE_ACCEPT_METHOD
protected:
    std::ostream& PrintTo(std::ostream& out) const override;
};

class CharsetBodyNode : public CFGNode
{
public:
    std::forward_list<std::unique_ptr<CharsetTokenNode>> children;
    DECLARE_ACCEPT_METHOD
protected:
    std::ostream& PrintTo(std::ostream& out) const override;
};

class CharsetHeadNode : public CFGNode
{
public:
    std::unique_ptr<IdentifierNode> identifier;
    DECLARE_ACCEPT_METHOD
protected:
    std::ostream& PrintTo(std::ostream& out) const override;
};

class CharsetNode : public CFGNode
{
public:
    std::unique_ptr<CharsetHeadNode> head;
    std::unique_ptr<CharsetBodyNode> body;
    DECLARE_ACCEPT_METHOD
protected:
    std::ostream& PrintTo(std::ostream& out) const override;
};

#undef DECLARE_ACCEPT_METHOD

