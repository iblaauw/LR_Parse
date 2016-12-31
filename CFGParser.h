#pragma once

#include <iostream>
#include <vector>
#include <memory>

namespace utils
{
    // For some reason, the compiler I'm on doesn't have c++14 yet, so here is this
    template<typename T, typename... Args>
    std::unique_ptr<T> make_unique(Args&&... args)
    {
        return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
    }

    template <class T, class U>
    std::unique_ptr<T> acquire_as(std::unique_ptr<U>&& ptr)
    {
        return std::unique_ptr<T>(static_cast<T*>(ptr.release()));
    }
}

class CFGNode
{
public:
    friend std::ostream& operator<<(std::ostream& out, const CFGNode& node);
protected:
    virtual std::ostream& PrintTo(std::ostream& out) const = 0;
};

using NodePtr = std::unique_ptr<CFGNode>;

inline std::ostream& operator<<(std::ostream& out, const CFGNode& node)
{
    return node.PrintTo(out);
}


class JoinNode : public CFGNode
{
public:
    std::vector<NodePtr> children;
    std::string name;

protected:
    std::ostream& PrintTo(std::ostream& out) const override;
};

class CharNode : public CFGNode
{
public:
    char value;
protected:
    std::ostream& PrintTo(std::ostream& out) const override;
};

std::ostream& operator<< (std::ostream&, const JoinNode& node);
std::ostream& operator<< (std::ostream&, const CharNode& node);

class CFGParser
{
private:
    std::istream& input;
public:
    CFGParser(std::istream& input);

    void Parse();
};

