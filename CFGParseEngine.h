#pragma once

#include <deque>
#include <iostream>
#include <string>

#include "CFGTree.h"
#include "compiler_patch.h"

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

    bool IsAtEnd() const;
};

class ParseContext
{
public:
    using Callable = void (*)(ParseContext*);
    using Filter = bool (*)(char);

private:
    ParseLookahead* lookahead;
    //JoinNode* current;
    bool testing;
    bool testResult;
    bool customResult;

    std::string resultName;

    std::vector<NodePtr> resultNodes;

public:
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
    inline NodePtr& Get(int i) { return resultNodes.at(i); }
    inline size_t Size() const { return resultNodes.size(); }

    template <class T>
    std::unique_ptr<T> AcquireAs(int i)
    {
        return utils::acquire_as<T, CFGNode>(std::move(resultNodes.at(i)));
    }

    void Commit(NodePtr&& node);

private:

    void Simulate(Callable func);
    void Simulate(Filter charset);
};

#define AutoName() SetName( __FUNCTION__ )




