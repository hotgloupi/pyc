#pragma once

#include <pyc/ast/Statement.hpp>

namespace pyc { namespace ast {

    class DelStatement : public Statement
    {
    private:
        Ptr<Expression> _exprlist;

    public:
        DelStatement(Ptr<Expression> exprlist)
            : _exprlist(std::move(exprlist))
        {}
    };

}}
