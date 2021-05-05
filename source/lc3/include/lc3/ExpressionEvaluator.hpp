#ifndef EXPRESSION_EVALUATOR_HPP
#define EXPRESSION_EVALUATOR_HPP

#include <stack>
#include <string>
#include <unordered_map>

namespace ExpressionEvaluator
{

    union eval_value
    {
        int (*func)(bool, int);
        int value;
    };

    struct eval_type
    {
        int type;
        eval_value eval;

    };

    extern std::unordered_map<std::string, eval_type> symbol_table;
    extern int (*OnUndefinedSymbol)(const std::string& symbol, bool hasref, int ref, int& error);

    enum
    {
        eval_ok = 0,
        eval_unbalanced,
        eval_invalidoperator,
        eval_invalidoperand,
        eval_malformedreference,
        eval_undefinedsymbol,
        eval_evalerr
    };

    enum
    {
        EVAL_VALUE = 0,
        EVAL_FUNCTION = 1
    };

    int Calculate(std::string_view expr, int &r);

}

#endif
