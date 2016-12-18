#pragma once

#include <vector>

#include "SymbolRegistry.h"
#include "RegistryBase.h"


typedef int RuleId;

class Rule
{
public:
    Symbol head;
    std::vector<Symbol> body;

    bool operator< (const Rule& other) const;
    bool operator== (const Rule& other) const;
};

class RuleRegistry : private RegistryBase<Rule>
{
private:
    typedef RegistryBase<Rule> MyBase;
    std::map<Symbol, std::vector<RuleId>> rulesByHead;
public:
    RuleId Register(const Rule& rule);
    RuleId GetOrRegister(const Rule& rule);

    RuleId GetId(const Rule& rule) const;
    const Rule& GetRule(RuleId id) const;
    void GetRulesForHead(Symbol head, std::vector<RuleId>& rulesOut) const;
};

