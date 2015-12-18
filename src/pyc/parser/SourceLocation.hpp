#pragma once

#include <pyc/inttypes.hpp>
#include <pyc/parser/fwd.hpp>

#include <boost/iterator/iterator_facade.hpp>

#include <limits>

namespace pyc { namespace parser {

    class SourceLocation
      : public boost::iterator_facade<
          SourceLocation, char const, std::forward_iterator_tag>
    {
    public:
        static u64 const end_cursor = std::numeric_limits<u64>::max();
    private:
        Source* _source = nullptr;
        u64 mutable _cursor = end_cursor;

    public:
        SourceLocation();
        explicit SourceLocation(Source& source, u64 cursor = 0);

    private:
        friend class boost::iterator_core_access;

        void increment();
        bool equal(SourceLocation const& other) const;
        char dereference() const;
    };

}}
