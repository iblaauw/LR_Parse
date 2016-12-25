#include "Closure.h"

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

    if (position < other.rule)
        return true;
    return false;
}

bool RulePiece::operator==(const RulePiece& other) const
{
    return rule == other.rule && position == other.position;
}

/*static*/ Closure Closure::CreateBeginning(const RuleRegistry& rules, const SymbolRegistry& symbols)
{
    Closure result(rules, symbols);
    RuleId pseudoRule = rules.GetPseudoRule();
    result.rulePieces.push_back({pseudoRule, 0});
    result.Complete();
    return result;
}

void Closure::GetAdvanceable(SymbolSet& symbolsOut) const
{
    symbolsOut.clear();
    for (RulePiece piece : rulePieces)
    {
        if (!piece.AtEnd(rules))
        {
            symbolsOut.insert(piece.Next(rules));
        }
    }
}

Closure Closure::Advance(Symbol s) const
{
    Closure result(rules, symbols);

    for (const RulePiece p : rulePieces)
    {
        if (!p.AtEnd(rules) && p.Next(rules) == s)
        {
            result.rulePieces.push_back(p.Advance());
        }
    }

    result.Complete();

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

Closure::Closure(const RuleRegistry& rules, const SymbolRegistry& symbols)
    : rules(rules), symbols(symbols)
{}

void Closure::Complete()
{
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
        HandlePiece(piece, ruleQueue, used);
    }

    std::sort(rulePieces.begin(), rulePieces.end());
}

void Closure::HandlePiece(RulePiece piece, std::queue<RulePiece>& ruleQueue, SymbolSet& used)
{
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

    // Add the rules to the vector and queue
    for (RuleId rid : nextRules)
    {
        RulePiece piece { rid, 0 };

        rulePieces.push_back(piece);
        ruleQueue.push(piece);
    }
}


