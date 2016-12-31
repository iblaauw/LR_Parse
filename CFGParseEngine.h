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
    ParseLookahead* lookahead;
    //JoinNode* current;
    bool testing;
    bool testResult;
    bool customResult;

    std::string resultName;

    std::vector<CFGNode*> resultNodes;

public:
    using Callable = void (*)(ParseContext*);
    using Filter = bool (*)(char);

    ParseContext(std::istream& input);
    ParseContext(const ParseContext& other);

    CFGNode* Start(Callable func);

    void Do(Callable func, bool discard = false);

    bool Is(Callable func);

    void Do(Filter charset, bool discard = false);

    bool Is(Filter charset);

    inline void SetName(std::string name) { resultName = name; }
    inline std::string GetName() const { return resultName; }

    inline bool IsCommitting() { return !testing; }
    inline CFGNode* Get(int i) const { return resultNodes.at(i); }
    inline size_t Size() const { return resultNodes.size(); }

    void Commit(CFGNode* node);

private:

    void Simulate(Callable func);
    void Simulate(Filter charset);

    JoinNode* EmitJoin();
};

#define AutoName() SetName( __FUNCTION__ )




