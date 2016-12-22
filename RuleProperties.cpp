#include "RuleProperties.h"

RuleProperties::RuleProperties(const RuleRegistry& rules, SymbolRegistry& symbols)
    : rules(rules), symbols(symbols), nullable(symbols.size(), true)
{
}

void RuleProperties::FindNullable()
{
    std::vector<bool> isset(symbols.size(), false);

    // Set the terminals to be non-nullable
    for (Symbol i = 0; i < symbols.size(); ++i)
    {
        if (symbols.IsTerminal(i))
        {
            isset[i] = true;
            nullable[i] = false;
        }
        else
            break;
    }

    for (Symbol s = symbols.GetTerminalBoundary(); s < symbols.size(); s++)
    {
        ComputeNullable(s, isset);
    }
}

bool RuleProperties::IsNullable(Symbol symbol)
{
    return nullable[symbol];
}

bool RuleProperties::ComputeNullable(Symbol s, std::vector<bool>& isset)
{
    if (isset[s])
        return nullable[s];

    std::vector<RuleId> ruleset;
    rules.GetRulesForHead(s, ruleset);

    isset[s] = true;

    for (RuleId ruleid : ruleset)
    {
        bool val = ComputeRuleNullable(ruleid, s, isset);
        if (val)
        {
            nullable[s] = true;
            return true;
        }
    }

    // If a recursive loop is formed, it needs to
    nullable[s] = false;
    return false;
}

bool RuleProperties::ComputeRuleNullable(RuleId ruleid, Symbol s, std::vector<bool>& isset)
{
    const Rule& rule = rules.GetRule(ruleid);
    for (Symbol other : rule.body)
    {
        if (other == s)
            return false;

        bool val = ComputeNullable(other, isset);
        if (!val)
            return false;
    }

    return true;
}



