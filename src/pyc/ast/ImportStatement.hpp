#pragma once

#include <pyc/ast/Statement.hpp>

#include <string>
#include <vector>

namespace pyc { namespace ast {

    class ImportStatement : public Statement
    {
    public:
        enum class Style
        {
            relative_to_current,
            relative_to_parent,
            absolute,
        };

        struct ImportAs
        {
            std::vector<std::string> import;
            std::string as;
        };
        typedef std::vector<ImportAs> ImportAsList;

    private:
        Style const _style;
        std::vector<std::string> const _from;
        std::vector<ImportAs> const _imports;

    public:
        ImportStatement(Style style,
                        std::vector<std::string> from,
                        ImportAsList imports);

    public:
        void dump(std::ostream& out, unsigned indent = 0) const override;
    };

    std::ostream& operator <<(std::ostream& out, ImportStatement::Style style);

}}
