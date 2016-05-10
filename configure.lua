
return function(build)
    local cxx = require('configure.lang.cxx.compiler').find{
        build = build,
        standard = 'c++11',
        --        standard_library = 'libstdc++',
    }

    local boost = require('configure.modules').boost.find{
        compiler = cxx,
        build = build,
        components = {
            'iostreams',
            'filesystem',
            'system',
            'serialization',
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
        libraries = table.extend({lib}, boost),
    }


	local test = Rule:new():add_target(build:virtual_node("test"))
	test:add_source(exe)
    for _, v in ipairs(build:fs():glob('test/lex', '*.py')) do
	    test:add_source(v)
	    test:add_shell_command(ShellCommand:new(exe, v))
	end
	build:add_rule(test)

end
