#pragma once

#include <vector>

#include "Rule.h"
#include "SymbolRegistry.h"

struct RulePiece
{
    RuleId rule;
    int position;

    bool AtEnd(const RuleRegistry& rules) const;
    Symbol Next(const RuleRegistry& rules) const;
    RulePiece Advance() const;

    bool operator<(const RulePiece& other) const;
    bool operator==(const RulePiece& other) const;

private:
    inline const Rule& GetRule(const RuleRegistry& rules) const
    {
        return rules.GetRule(rule);
    }
};

class Closure
{
private:
    const RuleRegistry& rules;

    std::vector<RulePiece> rulePieces;
public:
    Closure(RulePiece rulePiece, const RuleRegistry& rules);

    void GetAdvanceable(SybmolSet& symbolsOut) const;

    Closure Advance(Symbol s) const;

    bool operator<(const Closure& other) const;
    bool operator==(const Closure& other) const;

private:
    Closure(const RuleRegistry& rules);

    void Complete();
    void AddToQueue(RulePiece piece, std::queue<RuleId>& ruleQueue, const SymbolSet& used);
    void HandleRule(RuleId rid, std::queue<RuleId>& ruleQueue, SymbolSet& used);
};

using State = int;
using ClosureRegistry = RegistryBase<Closure>;

