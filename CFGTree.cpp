#include "CFGTree.h"

std::ostream& IdentifierNode::PrintTo(std::ostream& out) const
{
    out << "[ID \"" << value << "\"]";
    return out;
}

