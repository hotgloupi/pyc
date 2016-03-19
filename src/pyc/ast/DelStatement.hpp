#pragma once

#include <pyc/ast/Statement.hpp>

namespace pyc { namespace ast {

    class DelStatement : public Statement
    {
    private:
        Ptr<ExpressionList> _exprlist;

    public:
        DelStatement(Ptr<ExpressionList> exprlist)
            : _exprlist(std::move(exprlist))
        {}
    };

}}
