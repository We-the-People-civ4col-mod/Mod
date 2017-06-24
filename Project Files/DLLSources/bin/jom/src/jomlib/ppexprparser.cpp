
#include "ppexprparser.h"
#include "macrotable.h"
#include "ppexpr-lex.inc"

PPExprParser::PPExprParser()
:   stack_size(0),
    sym_stack(0),
    state_stack(0),
    m_macroTable(0)
{
}

PPExprParser::~PPExprParser()
{
    if (stack_size) {
        free(sym_stack);
        free(state_stack);
    }
}

inline void PPExprParser::reallocateStack()
{
    if (!stack_size)
        stack_size = 128;
    else
        stack_size <<= 1;

    sym_stack = reinterpret_cast<Value*> (realloc(sym_stack, stack_size * sizeof(Value)));
    state_stack = reinterpret_cast<int*> (realloc(state_stack, stack_size * sizeof(int)));
}

bool PPExprParser::parse(const char* str)
{
    const int INITIAL_STATE = 0;
    int yytoken = -1;

    if (!state_stack)
        reallocateStack();

    tos = 0;
    m_errorMessage.clear();
    state_stack[++tos] = INITIAL_STATE;
    yyInputBuffer = yy_scan_string(str);
    if (!yyInputBuffer) {
        m_errorMessage = "Can't create lexer's input buffer.";
        return false;
    }

    while (true)
    {
        if (yytoken == -1 && - TERMINAL_COUNT != action_index [state_stack [tos]]) {
            yytoken = yylex();
#ifdef DEBUG_GRAMMAR
            printf("*** yylex %d\n", yytoken);
#endif
        }

        int act = t_action(state_stack [tos], yytoken);

        if (act == ACCEPT_STATE) {
            yy_delete_buffer((YY_BUFFER_STATE)yyInputBuffer);
            return true;
        }

        else if (act > 0) {
            if (++tos == stack_size) {
                reallocateStack();
                if (!state_stack) {
                    m_errorMessage = "stack overflow";
                    yy_delete_buffer((YY_BUFFER_STATE)yyInputBuffer);
                    return false;
                }
            }

            sym_stack [tos] = yylval;
            state_stack [tos] = act;
            yytoken = -1;
        }

        else if (act < 0)
        {
            int r = - act - 1;

#ifdef DEBUG_GRAMMAR
            int ridx = rule_index [r];
            printf ("*** reduce using rule %d %s ::=", r + 1, spell[rule_info [ridx]]);
            ++ridx;
            for (int i = ridx; i < ridx + rhs [r]; ++i)
            {
                int symbol = rule_info [i];
                if (const char *name = spell [symbol])
                    printf (" %s", name);
                else
                    printf (" #%d", symbol);
            }
            printf ("\n");
#endif

            tos -= rhs [r];
            act = state_stack [tos++];

            switch (r) {

    case 2: // term0
        sym(1).num = (sym(1).num != 0 && sym(3).num != 0) ? 1 : 0;
        break;

    case 3: // term0
        sym(1).num = (sym(1).num != 0 || sym(3).num != 0) ? 1 : 0;
        break;

    case 4: // term0
        sym(1).num &= sym(3).num;
        break;

    case 5: // term0
        sym(1).num |= sym(3).num;
        break;

    case 8: { // strterm1
        QByteArray* lhs = sym(1).str;
        QByteArray* rhs = sym(3).str;
        sym(1).num = (*lhs == *rhs) ? 1 : 0;
        delete lhs;
        delete rhs;
        break;
    }

    case 9: { // strterm1
        QByteArray* lhs = sym(1).str;
        QByteArray* rhs = sym(3).str;
        sym(1).num = (*lhs == *rhs) ? 0 : 1;
        delete lhs;
        delete rhs;
        break;
    }

    case 11: // term2
        sym(1).num = (sym(1).num == sym(3).num);
        break;

    case 12: // term2
        sym(1).num = (sym(1).num != sym(3).num);
        break;

    case 13: // term2
        sym(1).num = (sym(1).num < sym(3).num);
        break;

    case 14: // term2
        sym(1).num = (sym(1).num > sym(3).num);
        break;

    case 15: // term2
        sym(1).num = (sym(1).num <= sym(3).num);
        break;

    case 16: // term2
        sym(1).num = (sym(1).num >= sym(3).num);
        break;

    case 18: // term3
        sym(1).num <<= sym(3).num;
        break;

    case 19: // term3
        sym(1).num >>= sym(3).num;
        break;

    case 21: // term4
        sym(1).num += sym(3).num;
        break;

    case 22: // term4
        sym(1).num -= sym(3).num;
        break;

    case 24: // term5
        sym(1).num *= sym(3).num;
        break;

    case 25: { // term5
        const int rhs = sym(3).num;
        if (rhs == 0) {
            m_errorMessage = "division by zero";
            yy_delete_buffer((YY_BUFFER_STATE)yyInputBuffer);
            return false;
        }
        sym(1).num /= rhs;
        break;
    }

    case 26: // term5
        sym(1).num %= sym(3).num;
        break;

    case 28: // term6
        sym(1).num = -sym(2).num;
        break;

    case 29: // term6
        sym(1).num = ~sym(2).num;
        break;

    case 30: // term6
        sym(1).num = (sym(2).num == 0) ? 1 : 0;
        break;

    case 32: // primary
        sym(1).num = sym(2).num;
        break;

            } // switch

            state_stack [tos] = nt_action (act, lhs [r] - TERMINAL_COUNT);
        }
        else
        {
            // ### ERROR RECOVERY HERE
            break;
        }
    }

    m_errorMessage = "Syntax Error";
    yy_delete_buffer((YY_BUFFER_STATE)yyInputBuffer);
    return false;
}
