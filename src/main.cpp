#include <cstdlib>
#include <iostream>
#include <stdexcept>

#include "App.hpp"

int main()
{
    try {
        App app = App::Create();
        
        app.Run();
    }
    catch (const std::exception &exception)
    {
        std::cerr << exception.what() << std::endl;
        return 1;
    }

    return 0;
}
