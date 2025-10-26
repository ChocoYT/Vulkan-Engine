#include <cstdlib>
#include <iostream>
#include <stdexcept>

#include "app.hpp"

int main()
{
    try {
        App app;
        
        app.init();
        app.run();
    }
    catch (const std::exception &exception)
    {
        std::cerr << exception.what() << std::endl;
        return 1;
    }

    return 0;
}
