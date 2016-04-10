#pragma once

#include <pyc/inttypes.hpp>
#include <pyc/parser/fwd.hpp>

#include <boost/iterator/iterator_facade.hpp>

#include <limits>

namespace pyc { namespace parser {

    class SourceLocation
      : public boost::iterator_facade<
          SourceLocation, char const, std::random_access_iterator_tag, char const&, i64>
    {
    public:
        static u64 const end_cursor = std::numeric_limits<u64>::max();
    private:
        char mutable _value;
        Source* _source = nullptr;
        u64 mutable _cursor = end_cursor;

    public:
        SourceLocation();
        explicit SourceLocation(Source& source, u64 cursor = 0);

    private:
        friend class boost::iterator_core_access;

        void increment();
        void advance(size_t);
        i64 distance_to(SourceLocation const& other) const
        {
             return other._cursor - _cursor;
        }
        bool equal(SourceLocation const& other) const;
        char const& dereference() const;
    };

}}
