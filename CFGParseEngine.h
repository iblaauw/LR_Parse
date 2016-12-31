#pragma once

#include <deque>
#include <iostream>

#include "CFGParser.h"

struct CFGResult
{
    bool isTest;
    bool testResult;
};

class ParseLookahead
{
private:
    std::istream& input;
    std::deque<char> lookahead;
    unsigned int lookPos = 0;

public:
    ParseLookahead(std::istream& input);

    char GetNext();
    char Consume();

    void Rollback();
};

class ParseContext
{
private:
    ParseLookahead lookahead;
    JoinNode* current;
    bool testing;
public:
    using Callable = CFGResult (*)(ParseContext*);
    using Filter = bool (*)(char);

    ParseContext(std::istream& input);

    inline CFGNode* GetCurrent() const { return current; }

    void Start(Callable func);

    CFGResult Do(Callable func);

    bool Is(Callable func);

    CFGResult Do(Filter charset);

    bool Is(Filter charset);
};

