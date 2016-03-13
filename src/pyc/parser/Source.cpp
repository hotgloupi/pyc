#include "Source.hpp"
#include "SourceLocation.hpp"

#include <boost/iostreams/get.hpp>
#include <boost/iostreams/operations.hpp>

#include <iostream>

namespace io = boost::iostreams;

namespace pyc { namespace parser {

    Source::Source(std::istream& input)
      : _input(input)
    {}

    Source::Location Source::begin()
    { return Location(*this); }

    Source::Location Source::end() const
    { return Location(); }

    bool Source::get(u64 pos, char& out)
    {
        while (_buffer.size() <= pos)
        {
            //std::cout << "Read at " << pos << std::endl;
            auto res = io::get(_input);
            if (res == EOF)
            {
                //std::cerr << "################# EOF!\n";
                return false;
                }
            if (res == io::WOULD_BLOCK)
                continue;// return false;
            _buffer.push_back(res);
        }
        out = _buffer.at(pos);
        //std::cout << "stdin[" << pos << "] = " << out << std::endl;

        return true;
    }

}}
