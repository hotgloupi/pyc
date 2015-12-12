
return function(build)
    local cxx = require('configure.lang.cxx.compiler').find{
        build = build,
        standard = 'c++11',
    }

    local boost = require('configure.modules').boost.find{
        compiler = cxx,
        build = build,
        components = {
            'iostreams'
        },
    }

    local lib = cxx:link_static_library{
        name = 'pyc',
        sources = build:fs():rglob('src/pyc', '*.cpp'),
        include_directories = {'src'},
        libraries = table.extend({
        }, boost)
    }

    local exe = cxx:link_executable{
        name = 'pyc',
        sources = {'src/main.cpp'},
        include_directories = {'src'},
        libraries = {lib},
    }
end
