#pragma once

#include <iostream>
#include <vector>

class CFGNode
{

};

class JoinNode : public CFGNode
{
public:
    std::vector<CFGNode*> children;
};

class CharNode : public CFGNode
{
public:
    char value;
};

class CFGParser
{
private:
    std::istream& input;
public:
    CFGParser(std::istream& input);

    void Parse();
};

