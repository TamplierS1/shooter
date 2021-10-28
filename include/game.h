#pragma once

#include "raylib.h"
#include "FPCamera.h"

class Game
{
public:
    Game();
    ~Game();

    int run();

private:
    FPCamera m_camera;
    float m_cam_fov = 90.0f;
    Vector3 m_cam_start_pos = {0.0f, 5.0f, 0.0f};
    Vector3 m_cam_speed = {20.0f, 0.0f, 20.0f};

    int m_fps = 60;

    const int m_win_width = 800;
    const int m_win_height = 600;
};
