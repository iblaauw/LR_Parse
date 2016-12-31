#include "CFGParseEngine.h"

#include <cassert>

#include "message_exception.h"

DECL_EXCEPTION(CFGException);

ParseLookahead::ParseLookahead(std::istream& input) : input(input) {}

char ParseLookahead::GetNext()
{
    char c;
    if (lookPos == lookahead.size())
    {
        c = input.get();
        lookahead.push_back(c);
    }
    else
    {
        c = lookahead[lookPos];
    }

    lookPos++;
    return c;
}

char ParseLookahead::Consume()
{
    assert(lookPos == 0);
    char c = lookahead.front();
    lookahead.pop_front();
    return c;
}

void ParseLookahead::Rollback()
{
    assert(lookPos > 0);
    lookPos--;
}

ParseContext::ParseContext(std::istream& input)
    : lookahead(input), current(nullptr), testing(false)
{}

void ParseContext::Start(Callable func)
{
    JoinNode* node = new JoinNode();

    current = node;
    testing = false;

    CFGResult result = func(this);

    assert(!result.isTest);
    assert(current == node);
}

CFGResult ParseContext::Do(Callable func)
{
    if (testing)
    {
        bool result = Is(func);
        return CFGResult { true, result };
    }
    else
    {
        // Create a new node to represent this action
        JoinNode* node = current;
        JoinNode* newNode = new JoinNode();
        node->children.push_back(newNode);

        // Set new state
        current = newNode;

        // Call the function
        CFGResult result = func(this);
        assert(!result.isTest);
        assert(current == newNode);

        // Restore back to previous state
        current = node;
        return CFGResult { false, false };
    }
}

bool ParseContext::Is(Callable func)
{
    bool prev = testing;
    testing = true;

    CFGResult result = func(this);
    assert(result.isTest);

    testing = prev;

    return result.testResult;
}

CFGResult ParseContext::Do(Filter charset)
{
    if (testing)
    {
        bool result = Is(charset);
        return CFGResult { true, result };
    }

    char c = lookahead.Consume();
    if (!charset(c))
        throw CFGException("Invalid syntax!");

    CharNode* node = new CharNode();
    node->value = c;

    current->children.push_back(node);

    return CFGResult { false, false };
}

bool ParseContext::Is(Filter charset)
{
    char c = lookahead.GetNext();
    bool result = charset(c);
    lookahead.Rollback(); // restore state
    return result;
}

