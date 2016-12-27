#include "Closure.h"

#include "RegistryManager.h"

#include <cassert>
#include <algorithm>

bool RulePiece::AtEnd(const RuleRegistry& rules) const
{
    auto& rule = GetRule(rules);
    return position >= rule.body.size();
}

Symbol RulePiece::Next(const RuleRegistry& rules) const
{
    const Rule& rule = GetRule(rules);
    assert(!AtEnd(rules));
    return rule.body[position];
}

RulePiece RulePiece::Advance() const
{
    return { rule, position + 1 };
}

bool RulePiece::operator<(const RulePiece& other) const
{
    if (rule < other.rule)
        return true;
    if (rule > other.rule)
        return false;

    if (position < other.position)
        return true;
    return false;
}

bool RulePiece::operator==(const RulePiece& other) const
{
    return rule == other.rule && position == other.position;
}

/*static*/ Closure Closure::CreateBeginning(const INullableProperty& nullable)
{
    Closure result;

    const auto& rules = RegistryManager::Instance.rules;

    RuleId pseudoRule = rules.GetPseudoRule();
    std::vector<RulePiece> pieces = { { pseudoRule, 0 } };
    result.AddAll(pieces, nullable);
    result.Complete(nullable);
    return result;
}

void Closure::GetAdvanceable(SymbolSet& symbolsOut) const
{
    symbolsOut.clear();

    const auto& rules = RegistryManager::Instance.rules;

    for (RulePiece piece : rulePieces)
    {
        if (!piece.AtEnd(rules))
        {
            symbolsOut.insert(piece.Next(rules));
        }
    }
}

Closure Closure::Advance(Symbol s, const INullableProperty& nullable) const
{
    Closure result;

    const auto& rules = RegistryManager::Instance.rules;

    std::vector<RulePiece> advancePieces;

    for (const RulePiece p : rulePieces)
    {
        if (!p.AtEnd(rules) && p.Next(rules) == s)
        {
            //result.rulePieces.push_back(p.Advance());
            advancePieces.push_back(p.Advance());
        }
    }

    result.AddAll(advancePieces, nullable);

    result.Complete(nullable);

    return result;
}

bool Closure::operator<(const Closure& other) const
{
    size_t s1 = rulePieces.size();
    size_t s2 = other.rulePieces.size();
    if (s1 < s2)
        return true;
    if (s1 > s2)
        return false;

    for (unsigned int i = 0; i < s1; ++i)
    {
        const RulePiece& r1 = rulePieces[i];
        const RulePiece& r2 = other.rulePieces[i];
        if (r1 < r2)
            return true;
        if (r2 < r1)
            return false;
    }

    // They are equal
    return false;
}

bool Closure::operator==(const Closure& other) const
{
    if (rulePieces.size() != other.rulePieces.size())
        return false;

    for (unsigned int i = 0; i < rulePieces.size(); i++)
    {
        bool equal = rulePieces[i] == other.rulePieces[i];
        if (!equal)
            return false;
    }

    return true;
}

Closure::Closure()
{}

void Closure::Complete(const INullableProperty& nullable)
{
    std::sort(rulePieces.begin(), rulePieces.end());

    std::queue<RulePiece> ruleQueue;
    SymbolSet used;

    for (const RulePiece& piece : rulePieces)
    {
        ruleQueue.push(piece);
    }

    while (ruleQueue.size() > 0)
    {
        RulePiece piece = ruleQueue.front();
        ruleQueue.pop();
        HandlePiece(piece, ruleQueue, used, nullable);
    }

    //std::sort(rulePieces.begin(), rulePieces.end());
}

void Closure::HandlePiece(RulePiece piece, std::queue<RulePiece>& ruleQueue, SymbolSet& used,
            const INullableProperty& nullable)
{
    const auto& rules = RegistryManager::Instance.rules;
    const auto& symbols = RegistryManager::Instance.symbols;

    // Ignore any finished rules. These can't advance farther
    if (piece.AtEnd(rules))
        return;

    Symbol symbol = piece.Next(rules);

    // There are no productions of terminals by definition
    if (symbols.IsTerminal(symbol))
        return;

    // If already put all for this symbol in, ignore
    if (used.count(symbol))
        return;

    used.insert(symbol); // Mark as used

    // Extract the rules that produce this symbol
    std::vector<RuleId> nextRules;
    rules.GetRulesForHead(symbol, nextRules);

    // Add this rule
    for (RuleId rid : nextRules)
    {
        AddRule(rid, ruleQueue, nullable);
    }
}

// Adds the rule: takes the rule, gets all portions
void Closure::AddRule(RuleId rid, std::queue<RulePiece>& ruleQueue, const INullableProperty& nullable)
{
    std::vector<RulePiece> allPieces;
    RulePiece startPiece { rid, 0 };

    GetPiecePortions(startPiece, nullable, allPieces);

    for (RulePiece piece : allPieces)
    {
        DoAddPiece(piece);
        ruleQueue.push(piece);
    }
}

void Closure::GetPiecePortions(RulePiece piece, const INullableProperty& nullable, std::vector<RulePiece>& piecesOut)
{
    piecesOut.clear();

    const auto& rules = RegistryManager::Instance.rules;
    const Rule& rule = rules.GetRule(piece.rule);
    size_t size = rule.body.size();

    // Iterate through every possible rule piece
    for (unsigned int i = piece.position; i <= size; ++i)
    {
        RulePiece newpiece { piece.rule, i };

        // Only add the piece if we don't already have it
        if (!std::binary_search(rulePieces.begin(), rulePieces.end(), newpiece))
        {
            piecesOut.push_back(newpiece);
        }

        if (i != size)
        {
            if (!nullable.IsNullable(rule.body[i]))
            {
                break;
            }
        }
    }
}

void Closure::AddAll(const std::vector<RulePiece>& pieces, const INullableProperty& nullable)
{
    std::vector<RulePiece> portions;
    for (RulePiece p : pieces)
    {
        GetPiecePortions(p, nullable, portions);
        for (RulePiece p2 : portions)
        {
            DoAddPiece(p2);
        }
        portions.clear();
    }
}

void Closure::DoAddPiece(RulePiece piece)
{
    rulePieces.push_back(piece);
    if (rulePieces.size() > 1)
    {
        std::inplace_merge(rulePieces.begin(), rulePieces.end()-1, rulePieces.end());
    }
}


