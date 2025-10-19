#pragma once

#include <vulkan/vulkan.h>

#include <iostream>
#include <vector>

class DescriptorPool
{
    public:
        void init(
            VkDevice device,
    
            uint32_t frameCount
        );
        void cleanup(VkDevice device);

        const VkDescriptorPool getHandle() const { return m_handle; }

    private:
        VkDescriptorPool m_handle = VK_NULL_HANDLE;
};
