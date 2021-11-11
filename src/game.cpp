#include "game.h"
#include "scene.h"
#include "asset_manager.h"

Game::Game()
{
    InitWindow(m_win_width, m_win_height, "shooter");

    m_camera.Setup(m_cam_fov, std::forward<Vector3&&>(m_cam_start_pos));
    m_camera.MoveSpeed = m_cam_speed;

    AssetManager::get().load_models("res/models");

    m_scene = std::make_unique<Scene>("res/Field.json");

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
        m_scene->render();
        m_camera.EndMode3D();

        EndDrawing();
    }

    return EXIT_SUCCESS;
}
