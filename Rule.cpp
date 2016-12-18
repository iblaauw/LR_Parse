#include "Rule.h"

bool Rule::operator< (const Rule& other) const
{
    if (head < other.head)
        return true;

    if (head > other.head)
        return false;

    int mysize = body.size();
    int othersize = other.body.size();

    if (mysize < othersize)
        return true;

    if (mysize > othersize)
        return false;

    for (int i = 0; i < mysize; i++)
    {
        if (body[i] < other.body[i])
            return true;

        if (body[i] > other.body[i])
            return false;
    }

    return false;
}

bool Rule::operator== (const Rule& other) const
{
    if (head != other.head)
        return false;

    if (body.size() != other.body.size())
        return false;

    for (unsigned int i = 0; i < body.size(); i++)
    {
        if (body[i] != other.body[i])
            return false;
    }

    return true;
}


RuleId RuleRegistry::Register(const Rule& rule)
{
    RuleId id = this->MyBase::Register(rule);

    Symbol head = rule.head;
    rulesByHead[head].push_back(id);

    return id;
}

RuleId RuleRegistry::GetOrRegister(const Rule& rule)
{
    auto iter = valToKey.find(rule);
    if (iter != valToKey.end())
        return iter->second;

    return this->Register(rule);
}

RuleId RuleRegistry::GetId(const Rule& rule) const
{
    return Get(rule);
}

const Rule& RuleRegistry::GetRule(RuleId id) const
{
    return GetValue(id);
}

void RuleRegistry::GetRulesForHead(Symbol head, std::vector<RuleId>& rulesOut) const
{
    rulesOut.clear();
    const auto& rules = rulesByHead.at(head);
    rulesOut.insert(rulesOut.begin(), rules.begin(), rules.end());
}


