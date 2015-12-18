#include "SourceLocation.hpp"
#include "Source.hpp"

#include <cassert>

namespace pyc { namespace parser {

    SourceLocation::SourceLocation()
    {}

    SourceLocation::SourceLocation(Source& source, u64 cursor)
      : _source(&source)
      , _cursor(cursor)
    {}

    void SourceLocation::increment()
    {
        assert(_cursor != SourceLocation::end_cursor);
        _cursor += 1;
    }

    bool SourceLocation::equal(SourceLocation const& other) const
    {
      assert(_source == nullptr || other._source == nullptr ||
             _source == other._source);
      return _cursor == other._cursor;
    }

    char SourceLocation::dereference() const
    {
        char value;
        if (!_source->get(_cursor, value))
        {
            _cursor = SourceLocation::end_cursor;
            return '\0';
        }
        return value;
    }

}}
