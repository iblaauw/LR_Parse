#pragma once

// This file contains the AST for the generated CFG

#include "CFGParser.h"

class IdentifierNode : public CFGNode
{
public:
    std::string value;
protected:
    std::ostream& PrintTo(std::ostream& out) const override;
};

