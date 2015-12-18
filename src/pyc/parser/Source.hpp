#pragma once

#include <pyc/parser/fwd.hpp>
#include <pyc/inttypes.hpp>

#include <iosfwd>
#include <string>

namespace pyc { namespace parser {

    class Source
    {
    public:
        typedef SourceLocation Location;
    private:
        std::istream& _input;
        std::string _buffer;

    public:
        explicit Source(std::istream& input);

    public:
        Location begin();
        Location end() const;

    public:
        bool get(u64 pos, char& out);
    };

}}

