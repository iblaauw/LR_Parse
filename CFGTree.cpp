#include "CFGTree.h"

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

