#pragma once

#include <deque>
#include <iostream>
#include <string>

#include "CFGParser.h"

//struct CFGResult
//{
//    bool isTest;
//    bool testResult;
//};

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

    inline unsigned int GetPosition() const { return lookPos; }
    void Rollback(unsigned int val);
};

class ParseContext
{
private:
    ParseLookahead lookahead;
    JoinNode* current;
    bool testing;
    bool testResult;
public:
    using Callable = void (*)(ParseContext*);
    using Filter = bool (*)(char);

    ParseContext(std::istream& input);

    CFGNode* Start(Callable func);

    void Do(Callable func);

    bool Is(Callable func);

    void Do(Filter charset);

    bool Is(Filter charset);

private:
    void Simulate(Callable func);
    void Simulate(Filter charset);
};

