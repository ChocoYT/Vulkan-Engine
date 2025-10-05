%VK_SDK_PATH%\Bin\dxc.exe -spirv %CD%\shaders\renderVS.hlsl -Fo %CD%\shaders\renderVS.spv -T vs_6_0 -E main -fspv-target-env=vulkan1.3
%VK_SDK_PATH%\Bin\dxc.exe -spirv %CD%\shaders\renderPS.hlsl -Fo %CD%\shaders\renderPS.spv -T ps_6_0 -E main -fspv-target-env=vulkan1.3
pause
