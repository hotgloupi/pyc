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
      assert(_source == other._source);
      return _cursor == other._cursor;
    }

    char const& SourceLocation::dereference() const
    {
        if (!_source->get(_cursor, _value))
        {
            //_cursor = SourceLocation::end_cursor;
            _value = '\0';
        }
        return _value;
    }

}}
