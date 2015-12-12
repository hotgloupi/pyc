#pragma once

namespace pyc {

    class Application
    {
    public:
        struct Options
        {
            bool interpreter = false;
        };

    private:
        Options const _options;

    public:
        Application(int ac, char* av[]);
        Application(Options const& options);
        void run();

    public:
        static Options parse_args(int ac, char* av[]);
    };
}
