#include "Source.hpp"
#include "SourceLocation.hpp"

#include <boost/iostreams/get.hpp>
#include <boost/iostreams/operations.hpp>

namespace io = boost::iostreams;

namespace pyc { namespace parser {

    Source::Source(std::istream& input)
      : _input(input)
    {}

    Source::Location Source::begin()
    { return Location(*this); }

    Source::Location Source::end() const
    { return Location(); }

    bool Source::pop(char& out)
    {
        auto res = io::get(_input);
        if (res == EOF)
            return false;
        if (res == io::WOULD_BLOCK)
            return false;
        out = res;
        return true;
    }

}}
