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
    unsigned int position;

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
    std::vector<RulePiece> rulePieces;
public:
    static Closure CreateBeginning(const INullableProperty& nullable);

    void GetAdvanceable(SymbolSet& symbolsOut) const;

    Closure Advance(Symbol s, const INullableProperty& nullable) const;

    inline const std::vector<RulePiece>& GetPieces() const { return rulePieces; }

    bool operator<(const Closure& other) const;
    bool operator==(const Closure& other) const;

private:
    Closure();

    void Complete(const INullableProperty& nullable);
    void HandlePiece(RulePiece piece, std::queue<RulePiece>& ruleQueue, SymbolSet& used,
            const INullableProperty& nullable);

    void AddRule(RuleId rid, std::queue<RulePiece>& ruleQueue, const INullableProperty& nullable);

    void GetPiecePortions(RulePiece piece, const INullableProperty& nullable, std::vector<RulePiece>& piecesOut);

    void AddAll(const std::vector<RulePiece>& pieces, const INullableProperty& nullable);
    void DoAddPiece(RulePiece piece);

};

using State = int;
using ClosureRegistry = RegistryBase<Closure>;

