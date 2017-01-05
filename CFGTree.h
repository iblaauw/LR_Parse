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

