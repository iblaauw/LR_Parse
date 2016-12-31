#pragma once

#include <iostream>
#include <vector>

class CFGNode
{
public:
    friend std::ostream& operator<<(std::ostream& out, const CFGNode& node);
protected:
    virtual std::ostream& PrintTo(std::ostream& out) const = 0;
};

inline std::ostream& operator<<(std::ostream& out, const CFGNode& node)
{
    return node.PrintTo(out);
}


class JoinNode : public CFGNode
{
public:
    std::vector<CFGNode*> children;
    std::string name;

protected:
    std::ostream& PrintTo(std::ostream& out) const override;
};

class CharNode : public CFGNode
{
public:
    char value;
protected:
    std::ostream& PrintTo(std::ostream& out) const override;
};

std::ostream& operator<< (std::ostream&, const JoinNode& node);
std::ostream& operator<< (std::ostream&, const CharNode& node);

class CFGParser
{
private:
    std::istream& input;
public:
    CFGParser(std::istream& input);

    void Parse();
};

