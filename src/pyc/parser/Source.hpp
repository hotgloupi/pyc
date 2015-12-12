#pragma once

#include <pyc/parser/fwd.hpp>

#include <iosfwd>

namespace pyc { namespace parser {

    class Source
    {
    public:
        typedef SourceLocation Location;
    private:
        std::istream& _input;

    public:
        Source(std::istream& input);

    public:
        Location begin();
        Location end() const;

    public:
        bool pop(char& out);
    };

}}

