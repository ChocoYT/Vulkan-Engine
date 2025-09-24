#include "settings.hpp"
#include "window.hpp"

#ifndef APP_HPP
#define APP_HPP

class App
{
    public:
        void run();

        Window window{SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_NAME};
};

#endif
