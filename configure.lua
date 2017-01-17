
return function(build)
    local exe = build:fs():copy('bin/pyc', 'bin/pyc')
    exe:set_property('install', true)
    print(exe)
    local test = Rule:new():add_target(build:virtual_node("test"))
    test:add_source(exe)
    for _, v in ipairs(build:fs():glob('test/lex', '*.py')) do
        test:add_source(v)
        test:add_shell_command(ShellCommand:new(exe, v))
    end
    build:add_rule(test)
end
