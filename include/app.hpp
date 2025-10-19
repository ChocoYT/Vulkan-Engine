#pragma once

#include <vulkan/vulkan.h>

#include <chrono>
#include <thread>

#include "camera.hpp"
#include "mesh.hpp"
#include "settings.hpp"
#include "Core/context.hpp"
#include "Pipeline/pipeline.hpp"
#include "window.hpp"

#include "Buffers/uniform.hpp"

class App
{
    public:
        void run();

    private:
        Window   window;
        Context  context;
        Pipeline pipeline;
};
