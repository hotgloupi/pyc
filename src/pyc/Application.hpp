#pragma once

#include <boost/filesystem/path.hpp>

namespace pyc {

    class Application
    {
    public:
        struct Options
        {
            bool interpreter = false;
            boost::filesystem::path file;
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
