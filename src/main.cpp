#include <cstdlib>
#include <iostream>
#include <stdexcept>

#include "app.hpp"

int main()
{
    App app;

    try
    {
        app.run();
    }
    catch (const std::exception &exception)
    {
        std::cerr << exception.what() << std::endl;
        return EXIT_FAILURE;
    }

    return 0;
}
