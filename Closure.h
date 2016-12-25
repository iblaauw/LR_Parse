#pragma once

#include <vector>
#include <queue>

#include "Rule.h"
#include "SymbolRegistry.h"
// TODO: move SymbolSet to its own file
#include "Properties.h"

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
    const SymbolRegistry& symbols;

    std::vector<RulePiece> rulePieces;
public:
    static Closure CreateBeginning(const RuleRegistry& rules, const SymbolRegistry& symbols);

    void GetAdvanceable(SymbolSet& symbolsOut) const;

    Closure Advance(Symbol s) const;

    bool operator<(const Closure& other) const;
    bool operator==(const Closure& other) const;

private:
    Closure(const RuleRegistry& rules, const SymbolRegistry& symbols);

    void Complete();
    void HandlePiece(RulePiece piece, std::queue<RulePiece>& ruleQueue, SymbolSet& used);
};

using State = int;
using ClosureRegistry = RegistryBase<Closure>;

