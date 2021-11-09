#include <experimental/filesystem>

#include "raylib.h"
#include "rlImGui.h"
#include "imgui.h"
#include "fmt/core.h"

#include "asset_manager.h"
#include "scene_editor.h"

namespace fs = std::experimental::filesystem;

bool files_getter(void* data, int idx, const char** out_text)
{
    const auto* files = reinterpret_cast<std::vector<std::string>*>(data);
    if (idx < 0 || idx >= files->size())
        return false;

    *out_text = files->at(idx).c_str();
    return true;
}

SceneEditor::SceneEditor()
{
    InitWindow(m_win_width, m_win_height, "Scene Editor");

    InitTPOrbitCamera(&m_camera, 90, Vector3{0.0f, 0.0f, 0.0f});
    m_camera.MinimumViewY = -90.0f;
    m_camera.MaximumViewY = 90.0f;

    SetTargetFPS(60);
    rlImGuiSetup(true);

    AssetManager::get().load_models("res/models");

    m_scene = std::make_unique<Scene>();
}

SceneEditor::~SceneEditor()
{
    rlImGuiShutdown();
    CloseWindow();
}

int SceneEditor::run()
{
    while (!WindowShouldClose())
    {
        UpdateTPOrbitCamera(&m_camera);

        BeginDrawing();

        ClearBackground(RAYWHITE);
        render_scene();
        render_gui();

        EndDrawing();
    }

    return EXIT_SUCCESS;
}

void SceneEditor::render_scene()
{
    BeginMode3D(m_camera.ViewCamera);
    m_scene->render();
    EndMode3D();
}

void SceneEditor::render_gui()
{
    rlImGuiBegin();

    ImGui::Begin("Scene Editor", &m_is_active, ImGuiWindowFlags_NoSavedSettings);
    if (ImGui::Button("Save Scene"))
    {
        serialize();
    }

    bool is_visible = ImGui::Button("Load Scene");
    render_file_browser(is_visible);

    ImGui::End();
    rlImGuiEnd();
}

void SceneEditor::render_file_browser(bool is_visible)
{
    static bool old_visibility = false;

    if (old_visibility != is_visible)
    {
        old_visibility = is_visible;

        if (is_visible)
        {
            ImGui::OpenPopup("File Browser");
        }
    }

    bool is_open = true;
    if (ImGui::BeginPopupModal("File Browser", &is_open))
    {
        static int item_current = 0;
        std::vector<std::string> files;

        for (const auto& entry : fs::directory_iterator("res"))
        {
            if (entry.path().extension() == ".json")
            {
                files.emplace_back(entry.path().stem());
            }
        }

        bool was_scene_loaded = false;
        if (ImGui::ListBox("##", &item_current, files_getter, &files, files.size(), files.size()))
        {
            load_scene(fmt::format("{}/{}.json", "res", files[item_current]));
            was_scene_loaded = true;
        }

        ImGui::EndPopup();

        if (was_scene_loaded)
            return;
    }
}

void SceneEditor::serialize() const
{
    m_scene->serialize("res");
}

void SceneEditor::load_scene(std::string_view path)
{
    m_scene = std::make_unique<Scene>(path);
}
