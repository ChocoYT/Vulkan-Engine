# Layout

|
├── assets/                 <-- Resources loaded at runtime  
|   ├── models/    
|   ├── textures/  
|   └── shaders/  
|       ├── glsl/           <-- Raw shader source files (.vert, .frag)  
|       ├── hlsl/  
|       └── spirv/          <-- Compiled SPIR-V binaries (.spv)  
|  
└── ├── Engine/  
|   |   ├── Application  
|   |   └── WindowSystem  
|   |  
|   ├── Renderer/  
|   |   ├── Renderer  
|   |   ├── CommandPool  
|   |   ├── DescriptorPool  
|   |   ├── FrameContext    <-- Synchronization/Per-Frame resources  
|   |   ├── GraphicsPipeline  
|   |   ├── RenderPass  
|   |   ├── SwapChain  
|   |   └── VulkanDevice    <-- Includes Instance, Debug, PhysicalDevice, Device logic  
|   |  
|   └── App/  
|       ├── App  
|       └── main            <-- Entry point  
