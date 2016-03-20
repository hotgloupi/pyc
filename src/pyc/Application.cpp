#include "Application.hpp"

#include <pyc/parser/Source.hpp>
#include <pyc/parser/Lexer.hpp>
#include <pyc/parser/SourceRange.hpp>
#include <pyc/parser/Token.hpp>
#include <pyc/parser/Parser.hpp>
#include <pyc/ast/serialize.hpp>

#include <boost/iostreams/device/file_descriptor.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/algorithm/string/predicate.hpp>

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>

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
                bool block = false;
                std::string str;
                do {
                    if (block)
                    {
                        std::cout << ".. ";
                        std::cout.flush();
                    }
                    std::string line;
                    if (!std::getline(std::cin, line)) return;
                    if (line.empty())
                    {
                        if (!str.empty()) str.append("\n");
                        break;
                    }
                    if (boost::ends_with(line, ":"))
                        block = true;
                    str += line;
                    str.append("\n");
                } while (block);

                if (str.empty())
                    continue;

                std::stringstream ss(str);
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
                    std::cout << "WAT?\n";
                std::cout << "************************************************\n";
            }
        }
        else
        {
            std::ifstream ifs(_options.file.c_str());
            parser::Source source(ifs);
            parser::Lexer lex(source, parser::Lexer::Mode::file);
            if (_options.syntax)
            {
                parser::Lexer::Stack stack;
                if (!lex.parse(stack))
                    throw std::runtime_error("Invalid syntax");

            }
            else
            {
                parser::Parser parser;
                auto ptr = parser.parse(lex);
                if (ptr == nullptr)
                    throw std::runtime_error("Couldn't parse the whole thing");
                ast::serialize::to_xml(std::cout, *ptr);
            }
        }
    }

    Application::Options Application::parse_args(int ac, char* av[])
    {
        std::vector<std::string> args;
        for (int i = 1; i < ac; ++i)
            args.push_back(av[i]);
        Options options;
        for (auto&& arg: args)
        {
            if (arg == "--syntax")
                options.syntax = true;
            else
            {
                if (!options.file.empty())
                    throw std::runtime_error("Cannot specify multiple files");
                options.file = arg;
            }
        }

        if (options.file.empty())
            options.interpreter = true;

        return options;
    }

}
