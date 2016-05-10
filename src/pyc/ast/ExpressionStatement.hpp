
#include <pyc/ast/Expression.hpp>
#include <pyc/ast/Statement.hpp>

namespace pyc { namespace ast {

    class ExpressionStatement : public Statement
    {
    private:
        Ptr<Expression> _value;

    public:
        explicit ExpressionStatement(Ptr<Expression> value)
            : Statement(NodeKind::expression_statement)
            , _value(std::move(value))
        {}

    public:
        void dump(std::ostream& out, unsigned indent = 0) const override
        {
            _value->dump(out, indent);
        }
    };

}}
