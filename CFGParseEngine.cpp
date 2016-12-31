#include "CFGParseEngine.h"

#include <cassert>
#include <memory>

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
    : lookahead(new ParseLookahead(input)), testing(false)
{}

CFGNode* ParseContext::Start(Callable func)
{
    /** OLD
    JoinNode* node = new JoinNode();

    current = node;
    testing = false;
    testResult = true;

    func(this);

    assert(!testing);
    assert(current == node);

    return current;
    **/
    Do(func, false);
    assert(!testing);
    assert(resultNodes.size() == 1);
    return resultNodes[0].get();
}

void ParseContext::Do(Callable func, bool discard)
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
        /** OLD
        // Save prev state
        JoinNode* node = current;

        // Create a new node to represent this action
        JoinNode* newNode = new JoinNode();
        if (!discard)
        {
            // TODO: discarding leaks memory...
            node->children.push_back(newNode);
        }

        // Set new state
        current = newNode;

        // Call the function
        func(this);
        assert(!testing);
        assert(current == newNode);

        // Restore back to previous state
        current = node;
        **/

        // Clone the context
        ParseContext context(*this);

        // Call the function
        func(&context);
        assert(!context.testing);

        if (!discard)
        {
            if (context.customResult)
            {
                // Grab the custom result (stored as the only element in context's results)
                assert(context.resultNodes.size() == 1);
                resultNodes.push_back(std::move(context.resultNodes[0]));
            }
            else
            {
                // Create the default new node: a join node
                std::unique_ptr<JoinNode> newNode = utils::make_unique<JoinNode>();
                for (NodePtr& ptr : context.resultNodes)
                {
                    newNode->children.push_back(std::move(ptr));
                }

                //auto newNode = context.EmitJoin();
                resultNodes.push_back(std::move(newNode));
            }
        }

    }
}

bool ParseContext::Is(Callable func)
{
    if (testing && !testResult)
        return false;

    // Save previous state
    unsigned int pos = lookahead->GetPosition();

    // Clone context
    ParseContext context(*this);

    context.testResult = true;
    context.testing = true;

    func(&context);

    assert(context.testing);

    // Restore previous state
    lookahead->Rollback(pos);

    return context.testResult;
}

void ParseContext::Do(Filter charset, bool discard)
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
        char c = lookahead->Consume();
        if (!charset(c))
            throw CFGException("Invalid syntax!");

        if (!discard)
        {
            std::unique_ptr<CharNode> node = utils::make_unique<CharNode>();
            node->value = c;

            //current->children.push_back(node);
            resultNodes.push_back(std::move(node));
        }
    }
}

bool ParseContext::Is(Filter charset)
{
    unsigned int pos = lookahead->GetPosition();
    char c = lookahead->GetNext();
    bool result = charset(c);
    lookahead->Rollback(pos); // restore state
    return result;
}

ParseContext::ParseContext(const ParseContext& other)
    : lookahead(other.lookahead), testing(other.testing), testResult(other.testResult),
      customResult(false), resultName(), resultNodes()
{ }

void ParseContext::Commit(NodePtr&& node)
{
    assert(!testing);

    if (customResult)
        throw CFGException("Can only commit once!");

    resultNodes.clear();
    resultNodes.push_back(std::forward<NodePtr>(node));
    customResult = true;
}

void ParseContext::Simulate(Callable func)
{
    // No need for anything complicated, just forward it
    func(this);
}

void ParseContext::Simulate(Filter charset)
{
    char c = lookahead->GetNext();
    bool result = charset(c);

    if (!result) // The simulate failed! Mark it as a failure
    {
        testResult = false;
    }
}


