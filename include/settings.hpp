#pragma once

#include <string>

// Screen
constexpr int SCREEN_WIDTH  = 1600;
constexpr int SCREEN_HEIGHT = 900;
constexpr uint32_t FRAMES_IN_FLIGHT = 2;

constexpr double TARGET_FPS = 120.0;
constexpr double FRAME_TIME = 1.0 / TARGET_FPS;

inline std::string SCREEN_NAME = "Vulkan Engine";
inline std::string ENGINE_NAME = "Vulkan Engine";

// Camera
constexpr float CAMERA_FOV  = 76.0f;
constexpr float CAMERA_NEAR = 0.1f;
constexpr float CAMERA_FAR  = 100.0f;

constexpr float CAMERA_MOVE_SPEED  = 15.0f;
constexpr float CAMERA_SENSITIVITY = 80.0f;
