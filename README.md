# Layout

VulkanEngine/  
├── Build/                      <-- Compiled Binaries, Temp Files, CMake output  
|   └── ...  
|
├── external/                   <-- Third-party source code (GLFW, GLM, etc.)  
|   ├── glfw/  
|   └── glm/  
|
├── assets/                     <-- Resources loaded at runtime  
|   ├── models/    
|   ├── textures/  
|   └── shaders/  
|       ├── glsl/               <-- Raw shader source files (.vert, .frag)  
|       ├── hlsl/  
|       └── spirv/              <-- Compiled SPIR-V binaries (.spv)  
|  
├── include/                    <-- ALL Header Files (Public Interfaces)  
|   ├── Engine/  
|   |   ├── Application.hpp  
|   |   └── WindowSystem.hpp  
|   |  
|   ├── Renderer/  
|   |   ├── Renderer.hpp  
|   |   ├── CommandPool.hpp  
|   |   ├── DescriptorPool.hpp  
|   |   ├── FrameContext.hpp    <-- Synchronization/Per-Frame resources  
|   |   ├── GraphicsPipeline.hpp  
|   |   ├── RenderPass.hpp  
|   |   ├── SwapChain.hpp  
|   |   └── VulkanDevice.hpp    <-- Includes Instance, Debug, PhysicalDevice, Device logic  
|   |
|   └── App/  
|       └── App.hpp  
|
├── src/                        <-- ALL Source Files (Implementations)  
|   ├── Engine/  
|   |   ├── Application.cpp  
|   |   └── WindowSystem.cpp  
|   |  
|   ├── Renderer/  
|   |   ├── Renderer.cpp  
|   |   ├── CommandPool.cpp  
|   |   ├── DescriptorPool.cpp  
|   |   ├── FrameContext.cpp  
|   |   ├── GraphicsPipeline.cpp  
|   |   ├── RenderPass.cpp  
|   |   ├── SwapChain.cpp  
|   |   └── VulkanDevice.cpp  
|   |  
|   └── App/  
|       ├── App.cpp  
|       └── main.cpp            <-- Entry point  
|  
├── .gitignore  
├── .gitmodules  
├── CMakeLists.txt  
└── README.md  
