#include "ImportStatement.hpp"

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/join.hpp>

namespace pyc { namespace ast {

    //static std::vector<std::string> split_dotted_name(std::string const& name)
    //{
    //    std::vector<std::string> res;
    //    boost::split(res, name, boost::is_any_of("."));
    //    return res;
    //}

    ImportStatement::ImportStatement(Style style,
                                     std::vector<std::string> from,
                                     ImportAsList imports)
      : Statement(NodeKind::import_statement)
      , _style(style)
      , _from(std::move(from))
      , _imports(std::move(imports))
    {}

    void ImportStatement::dump(std::ostream& out, unsigned indent) const
    {
        out << "ImportStatement(\n";
        _indent(out, indent + 1) << "style = \"" << _style << "\",\n";
        _indent(out, indent + 1) << "from = \"" << boost::join(_from, ".") << "\",\n";
        _indent(out, indent + 1) << "imports = [\n";
        for (auto&& import: _imports)
        {
            _indent(out, indent + 2) << "(\"" << boost::join(import.import, ".")
                                     << "\", \"" << import.as << "\"),\n";
        }
        _indent(out, indent + 1 ) << "]\n";
        _indent(out, indent) << ")";
    }

    std::ostream& operator <<(std::ostream& out, ImportStatement::Style style)
    {
        switch (style)
        {
        case ImportStatement::Style::relative_to_parent:
            return out << "relative-to-parent";
        case ImportStatement::Style::relative_to_current:
            return out << "relative-to-current";
        case ImportStatement::Style::absolute:
            return out << "absolute";
        default:
            return out << "INVALID";
        }
    }
}}
