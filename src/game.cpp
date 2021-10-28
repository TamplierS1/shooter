#include "game.h"

Game::Game()
{
    InitWindow(m_win_width, m_win_height, "shooter");

    m_camera.Setup(m_cam_fov, std::forward<Vector3&&>(m_cam_start_pos));
    m_camera.MoveSpeed = m_cam_speed;
    SetTargetFPS(m_fps);
}

Game::~Game()
{
    CloseWindow();
}

int Game::run()
{
    while (!WindowShouldClose())
    {
        m_camera.Update();

        BeginDrawing();
        ClearBackground(BLACK);

        m_camera.BeginMode3D();
        DrawCube({0.0f, 3.0f, 0.0f}, 2.0f, 2.0f, 2.0f, RAYWHITE);
        m_camera.EndMode3D();

        EndDrawing();
    }

    return EXIT_SUCCESS;
}
