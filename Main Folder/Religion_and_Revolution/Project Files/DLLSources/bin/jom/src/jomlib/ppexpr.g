----------------------------------------------------------------------------
--
-- Copyright (C) 2008-2014 Digia Plc and/or its subsidiary(-ies).
-- Contact: http://www.qt-project.org/legal
--
-- This file is part of the jom project on Trolltech Labs.
--
-- This file may be used under the terms of the GNU General Public
-- License version 2.0 or 3.0 as published by the Free Software Foundation
-- and appearing in the file LICENSE.GPL included in the packaging of
-- this file.  Please review the following information to ensure GNU
-- General Public Licensing requirements will be met:
-- http://www.fsf.org/licensing/licenses/info/GPLv2.html and
-- http://www.gnu.org/copyleft/gpl.html.
--
-- This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
-- WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
--
----------------------------------------------------------------------------
%parser ppexpr_grammar
%decl ppexprparser.h
%impl ppexprparser.cpp

%token_prefix T_

%token NUMBER
%token STRING
%token BOOL_AND
%token BOOL_OR
%token BIT_AND
%token BIT_OR
%token EQUAL
%token NOT_EQUAL 
%token LESS_THAN
%token GREATER_THAN
%token EQUAL_OR_LESS_THAN
%token EQUAL_OR_GREATER_THAN
%token SHIFT_LEFT
%token SHIFT_RIGHT
%token PLUS
%token MINUS
%token MULT
%token DIV
%token MOD
%token BIT_NOT
%token BOOL_NOT
%token LEFT_PAREN
%token RIGHT_PAREN
%token LINEFEED

%start expression

/:
#include "ppexpr_grammar_p.h"
#include <QtCore>

namespace NMakeFile {
    class MacroTable;
}

class PPExprParser : protected $table
{
public:
    PPExprParser();
    ~PPExprParser();

    bool parse(const char* str);

    int expressionValue()
    {
        return sym(1).num;
    }

    QByteArray errorMessage() const
    {
        return m_errorMessage;
    }

    void setMacroTable(NMakeFile::MacroTable* macroTable)
    {
        m_macroTable = macroTable;
    }

protected:
    struct Value
    {
#ifdef _DEBUG
        Value()
        : num(0)
        {}
#endif

        union {
            int num;
            QByteArray* str;
        };
    };

protected:
    int yylex();
    inline void reallocateStack();

    inline Value &sym(int index)
    { return sym_stack [tos + index - 1]; }

protected:
    void* yyInputBuffer;
    Value yylval;
    int tos;
    int stack_size;
    Value *sym_stack;
    int *state_stack;
    NMakeFile::MacroTable* m_macroTable;
    QByteArray m_errorMessage;
};
:/

/.
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
./

expression  ::=  term0;
term0       ::=  term1;
term0       ::=  term0 BOOL_AND term1;
/.
    case $rule_number: // $rule
        sym(1).num = (sym(1).num != 0 && sym(3).num != 0) ? 1 : 0;
        break;
./

term0       ::=  term0 BOOL_OR term1;
/.
    case $rule_number: // $rule
        sym(1).num = (sym(1).num != 0 || sym(3).num != 0) ? 1 : 0;
        break;
./

term0       ::=  term0 BIT_AND term1;
/.
    case $rule_number: // $rule
        sym(1).num &= sym(3).num;
        break;
./

term0       ::=  term0 BIT_OR term1;
/.
    case $rule_number: // $rule
        sym(1).num |= sym(3).num;
        break;
./

term1     ::= term2;
term1     ::= strterm1;

strterm1    ::= STRING EQUAL STRING;
/.
    case $rule_number: { // $rule
        QByteArray* lhs = sym(1).str;
        QByteArray* rhs = sym(3).str;
        sym(1).num = (*lhs == *rhs) ? 1 : 0;
        delete lhs;
        delete rhs;
        break;
    }
./

strterm1    ::= STRING NOT_EQUAL STRING;
/.
    case $rule_number: { // $rule
        QByteArray* lhs = sym(1).str;
        QByteArray* rhs = sym(3).str;
        sym(1).num = (*lhs == *rhs) ? 0 : 1;
        delete lhs;
        delete rhs;
        break;
    }
./

term2       ::= term3;
term2       ::= term2 EQUAL term3;
/.
    case $rule_number: // $rule
        sym(1).num = (sym(1).num == sym(3).num);
        break;
./

term2       ::= term2 NOT_EQUAL term3;
/.
    case $rule_number: // $rule
        sym(1).num = (sym(1).num != sym(3).num);
        break;
./

term2       ::= term2 LESS_THAN term3;
/.
    case $rule_number: // $rule
        sym(1).num = (sym(1).num < sym(3).num);
        break;
./

term2       ::= term2 GREATER_THAN term3;
/.
    case $rule_number: // $rule
        sym(1).num = (sym(1).num > sym(3).num);
        break;
./

term2       ::= term2 EQUAL_OR_LESS_THAN term3;
/.
    case $rule_number: // $rule
        sym(1).num = (sym(1).num <= sym(3).num);
        break;
./

term2       ::= term2 EQUAL_OR_GREATER_THAN term3;
/.
    case $rule_number: // $rule
        sym(1).num = (sym(1).num >= sym(3).num);
        break;
./

term3       ::= term4;
term3       ::= term3 SHIFT_LEFT term4;
/.
    case $rule_number: // $rule
        sym(1).num <<= sym(3).num;
        break;
./

term3       ::= term3 SHIFT_RIGHT term4;
/.
    case $rule_number: // $rule
        sym(1).num >>= sym(3).num;
        break;
./

term4       ::= term5;
term4       ::= term4 PLUS term5;
/.
    case $rule_number: // $rule
        sym(1).num += sym(3).num;
        break;
./

term4       ::= term4 MINUS term5;
/.
    case $rule_number: // $rule
        sym(1).num -= sym(3).num;
        break;
./

term5       ::= term6;
term5       ::= term5 MULT term6;
/.
    case $rule_number: // $rule
        sym(1).num *= sym(3).num;
        break;
./

term5       ::= term5 DIV term6;
/.
    case $rule_number: { // $rule
        const int rhs = sym(3).num;
        if (rhs == 0) {
            m_errorMessage = "division by zero";
            yy_delete_buffer((YY_BUFFER_STATE)yyInputBuffer);
            return false;
        }
        sym(1).num /= rhs;
        break;
    }
./

term5       ::= term5 MOD term6;
/.
    case $rule_number: // $rule
        sym(1).num %= sym(3).num;
        break;
./

term6 ::= primary;
term6 ::= MINUS primary;
/.
    case $rule_number: // $rule
        sym(1).num = -sym(2).num;
        break;
./

term6 ::= BIT_NOT primary;
/.
    case $rule_number: // $rule
        sym(1).num = ~sym(2).num;
        break;
./

term6 ::= BOOL_NOT primary;
/.
    case $rule_number: // $rule
        sym(1).num = (sym(2).num == 0) ? 1 : 0;
        break;
./

primary ::= NUMBER;
primary ::= LEFT_PAREN term0 RIGHT_PAREN;
/.
    case $rule_number: // $rule
        sym(1).num = sym(2).num;
        break;
./

/.
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
./

