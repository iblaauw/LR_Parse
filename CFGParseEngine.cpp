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

void ParseLookahead::Rollback(unsigned int val)
{
    assert(lookPos > val);
    assert(val >= 0);
    assert(val < lookahead.size());
    lookPos = val;
}

ParseContext::ParseContext(std::istream& input)
    : lookahead(input), current(nullptr), testing(false)
{}

CFGNode* ParseContext::Start(Callable func)
{
    JoinNode* node = new JoinNode();

    current = node;
    testing = false;
    testResult = true;

    func(this);

    assert(!testing);
    assert(current == node);

    return current;
}

void ParseContext::Do(Callable func)
{
    if (testing)
    {
        if (testResult) // Don't recurse if test failed already
        {
            Simulate(func);
        }
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
        func(this);
        assert(!testing);
        assert(current == newNode);

        // Restore back to previous state
        current = node;
    }
}

bool ParseContext::Is(Callable func)
{
    if (testing && !testResult)
        return false;

    // Save previous state
    bool prev = testing;
    bool prevResult = testResult;
    unsigned int pos = lookahead.GetPosition();

    testResult = true;
    testing = true;

    func(this);

    assert(testing);

    bool result = testResult;

    // Restore previous state
    testing = prev;
    testResult = prevResult;
    lookahead.Rollback(pos);

    return result;
}

void ParseContext::Do(Filter charset)
{
    if (testing)
    {
        if (testResult) // Don't recurse if test failed already
        {
            Simulate(charset);
        }
    }
    else
    {
        char c = lookahead.Consume();
        if (!charset(c))
            throw CFGException("Invalid syntax!");

        CharNode* node = new CharNode();
        node->value = c;

        current->children.push_back(node);
    }
}

bool ParseContext::Is(Filter charset)
{
    unsigned int pos = lookahead.GetPosition();
    char c = lookahead.GetNext();
    bool result = charset(c);
    lookahead.Rollback(pos); // restore state
    return result;
}

void ParseContext::Simulate(Callable func)
{
    // No need for anything complicated, just forward it
    func(this);
}

void ParseContext::Simulate(Filter charset)
{
    char c = lookahead.GetNext();
    bool result = charset(c);

    if (!result) // The simulate failed! Mark it as a failure
    {
        testResult = false;
    }
}



