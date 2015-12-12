#include "Application.hpp"

#include <pyc/parser/Source.hpp>
#include <pyc/parser/Lexer.hpp>
#include <pyc/parser/SourceRange.hpp>
#include <pyc/parser/Token.hpp>

#include <boost/iostreams/device/file_descriptor.hpp>
#include <boost/iostreams/stream.hpp>

#include <vector>
#include <string>

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
            std::cout << "COUCOUDSSDS\n";
            io::file_descriptor_source in(STDIN_FILENO, io::never_close_handle);
            std::cout << "COsUCOUDSSDS\n";
            io::stream<io::file_descriptor_source> stream(in);
            std::cout << "CsasaOsUCOUDSSDS\n";
            while (true)
            {
                std::cout << ">> ";
                std::cout.flush();
                parser::Source source(stream);
                parser::Lexer lex(source);
                parser::Token tok;
                parser::SourceRange range;
                while (lex.next_token(tok, range))
                {
                    std::cout << "Got" << tok << "\n";
                    if (tok == parser::Token::eof)
                        return;
                }
                std::cout << "PLOUF\n";
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
