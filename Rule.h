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
    RuleId pseudoRule = -1;
public:
    RuleId Register(const Rule& rule);
    RuleId GetOrRegister(const Rule& rule);
    RuleId CreatePseudoRule(const SymbolRegistry& symbols);

    RuleId GetId(const Rule& rule) const;
    inline RuleId GetPseudoRule() const { return pseudoRule; }

    const Rule& GetRule(RuleId id) const;
    void GetRulesForHead(Symbol head, std::vector<RuleId>& rulesOut) const;

    inline auto begin() const -> decltype(RegistryBase<Rule>::begin())
    { return RegistryBase<Rule>::begin(); }

    inline auto end() const -> decltype(RegistryBase<Rule>::end())
    { return RegistryBase<Rule>::end(); }
};

