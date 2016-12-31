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

void ParseLookahead::Rollback(int val)
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
    std::cout << "DO " << testing << std::endl;
    if (testing)
    {
        if (testResult) // Don't recurse if test failed already
        {
            testResult = Is(func);
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
    std::cout << "IS " << testing << std::endl;
    // Save previous state
    bool prev = testing;
    bool prevResult = testResult;

    testing = true;
    testResult = true;

    func(this);

    assert(testing);

    bool result = testResult;

    // Restore previous state
    testing = prev;
    testResult = prevResult;

    return result;
}

void ParseContext::Do(Filter charset)
{
    std::cout << "DO2 " << testing << std::endl;
    if (testing)
    {
        if (testResult)
        {
            char c = lookahead.GetNext();
            testResult = Is(charset);
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
    std::cout << "IS2 " << testing << std::endl;
    char c = lookahead.GetNext();
    bool result = charset(c);
    lookahead.Rollback(); // restore state
    return result;
}



