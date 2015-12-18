#include "Application.hpp"

#include <pyc/parser/Source.hpp>
#include <pyc/parser/Lexer.hpp>
#include <pyc/parser/SourceRange.hpp>
#include <pyc/parser/Token.hpp>

#include <boost/iostreams/device/file_descriptor.hpp>
#include <boost/iostreams/stream.hpp>

#include <vector>
#include <string>
#include <sstream>

namespace io = boost::iostreams;

namespace pyc {

    Application::Application(int ac, char* av[])
      : _options(Application::parse_args(ac, av))
    {

    }

    Application::Application(Options const& options)
      : _options(options)
    {}

    void Application::run()
    {
        if (_options.interpreter)
        {
            //io::file_descriptor_source in(STDIN_FILENO, io::never_close_handle);
            //io::stream<io::file_descriptor_source> stream(in);
            while (true)
            {

                std::cout << ">> ";
                std::cout.flush();
                std::string line;
                if (!std::getline(std::cin, line)) return;

                line.append("\n");
                std::stringstream ss(line);
                parser::Lexer::Stack stack;
                parser::Source source(ss);
                parser::Lexer lex(source, parser::Lexer::Mode::single);

                if (lex.parse(stack))
                {
                    for (auto& e: stack)
                    {
                        std::cout << ">>>>>>>>>>>>>>>>>>> Got " << e.first << "\n";
                        if (e.first == parser::Token::eof)
                            return;
                    }
                    //std::cout << ".. ";
                    //std::cout.flush();
                }
                else
                    std::cerr << "WAT?\n";
                std::cerr << "************************************************\n";
            }
        }

    }

    Application::Options Application::parse_args(int ac, char* av[])
    {
        std::vector<std::string> args;
        for (int i = 1; i < ac; ++i)
            args.push_back(av[i]);
        Options options;
        options.interpreter = true;

        return options;
    }

}
