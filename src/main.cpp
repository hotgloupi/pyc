#include <pyc/Application.hpp>
#include <pyc/error.hpp>

#include <iostream>

int main(int ac, char* av[])
{
    try
    {
        pyc::Application(ac, av).run();
    }
    catch (...)
    {
        std::cerr << "Error: " << pyc::error::string() << std::endl;
        return 1;
    }
    return 0;
}
