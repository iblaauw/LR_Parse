#pragma once

#include <iostream>
#include <vector>
#include <deque>

#include "message_exception.h"

DECL_EXCEPTION(CFGException);

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

struct CFGResult
{
    bool isTest;
    bool testResult;
};

class CFGParser
{
private:
    std::istream& input;

    std::deque<char> lookahead;
    unsigned int lookPos = 0;

    // Parser state
    JoinNode* current;
    bool testing;
public:
    CFGParser(std::istream& input);

    CFGNode* Parse();

private:
    void Start(CFGResult (*func)());
    CFGResult Do(CFGResult (*func)());
    bool Is(CFGResult (*func)());;
    CFGResult Do(bool (*charset)(char));
    bool Is(bool (*charset)(char));

    char GetNext();
    char Consume();
};

