
#include "ppexpr_grammar_p.h"
#include <QtCore>

namespace NMakeFile {
    class MacroTable;
}

class PPExprParser : protected ppexpr_grammar
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
