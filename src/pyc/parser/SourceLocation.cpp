#include "SourceLocation.hpp"
#include "Source.hpp"

#include <cassert>

namespace pyc { namespace parser {

    SourceLocation::SourceLocation()
    {}

    SourceLocation::SourceLocation(Source& source, u64 cursor)
      : _source(&source)
      , _cursor(cursor)
    {
        assert(_cursor != SourceLocation::end_cursor);
        if (!_source->pop(_value))
            _cursor = SourceLocation::end_cursor;
    }

    void SourceLocation::increment()
    {
        assert(_cursor != SourceLocation::end_cursor);
        if (_source->pop(_value))
            _cursor += 1;
        else
            _cursor = SourceLocation::end_cursor;
    }

    bool SourceLocation::equal(SourceLocation const& other) const
    {
      assert(_source == nullptr || other._source == nullptr ||
             _source == other._source);
      return _cursor == other._cursor;
    }

    char const& SourceLocation::dereference() const
    {
        return _value;
    }

}}
