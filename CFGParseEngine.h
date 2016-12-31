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

class ParseAST { };

class ParseContext
{
private:
    ParseLookahead lookahead;
    JoinNode* current;
    bool testing;
    bool testResult;
    //bool customResult;

    //vector<CFGNode*> resultNodes;

public:
    using Callable = void (*)(ParseContext*);
    using Filter = bool (*)(char);

    ParseContext(std::istream& input);

    CFGNode* Start(Callable func);

    void Do(Callable func, bool discard = false);

    bool Is(Callable func);

    void Do(Filter charset, bool discard = false);

    bool Is(Filter charset);

    void SetName(std::string name);

    inline bool IsCommitting() { return !testing; }

private:

    void Simulate(Callable func);
    void Simulate(Filter charset);
};

#define AutoName() SetName( __FUNCTION__ )




