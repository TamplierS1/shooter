#include <experimental/filesystem>

#include "raylib.h"
#include "raymath.h"
#include "rlImGui.h"
#include "imgui.h"
#include "fmt/core.h"

#include "asset_manager.h"
#include "scene_editor.h"

namespace fs = std::experimental::filesystem;

// 'extension' is the file extension to look for.
static std::vector<std::string> parse_directory(std::string_view dir_path,
                                                std::string_view extension)
{
    std::vector<std::string> files;

    for (const auto& entry : fs::directory_iterator(dir_path))
    {
        if (entry.path().extension() == extension)
        {
            files.emplace_back(entry.path().stem());
        }
    }

    return files;
}

static int input_text_resize_callback(ImGuiInputTextCallbackData* data)
{
    if (data->EventFlag == ImGuiInputTextFlags_CallbackResize)
    {
        auto* str = reinterpret_cast<std::string*>(data->UserData);
        str->resize(data->BufTextLen);
        data->Buf = str->data();
    }
    return 0;
}

static bool files_getter(void* data, int idx, const char** out_text)
{
    const auto* files = reinterpret_cast<std::vector<std::string>*>(data);
    if (idx < 0 || idx >= files->size())
        return false;

    *out_text = files->at(idx).c_str();
    return true;
}

static Color vector2color(Vector3 vec)
{
    return Color{vec.x, vec.y, vec.z, 255};
}

SceneEditor::SceneEditor()
{
    InitWindow(m_win_width, m_win_height, "Scene Editor");

    InitTPOrbitCamera(&m_camera, 90, Vector3{0.0f, 0.0f, 0.0f});
    m_camera.MinimumViewY = -90.0f;
    m_camera.MaximumViewY = 90.0f;
    m_camera.MoveSpeed = {0.0f, 0.0f, 0.0f};

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
        handle_input();

        UpdateTPOrbitCamera(&m_camera);
        update_object_transform(m_selected_object.get());

        BeginDrawing();

        ClearBackground(RAYWHITE);
        render_scene();
        render_gui();

        EndDrawing();
    }

    return EXIT_SUCCESS;
}

void SceneEditor::render_transform_gizmo()
{
    DrawLine3D(m_selected_object->m_pos,
               Vector3Add(m_selected_object->m_pos, Vector3Scale(m_move_axis, 2)),
               vector2color(Vector3Scale(m_move_axis, 255)));
}

void SceneEditor::update_object_transform(Object* object)
{
    if (object != nullptr)
    {
        if (m_move_axis.x != 0 || m_move_axis.y != 0 || m_move_axis.z != 0)
        {
            object->m_pos = Vector3Add(
                object->m_pos, Vector3Multiply(m_move_axis, calc_mouse_dist_traveled()));
        }
        else if (m_scale_axis.x != 0 || m_scale_axis.y != 0 || m_scale_axis.z != 0)
        {
            object->m_scale =
                Vector3Add(object->m_scale,
                           Vector3Multiply(m_scale_axis, calc_mouse_dist_traveled()));
        }
    }
    m_prev_mouse_pos = GetMousePosition();
}

void SceneEditor::handle_input()
{
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
        auto closest_object = get_closest_clicked_object();
        if (closest_object.has_value())
        {
            m_selected_object = closest_object.value();
            m_camera.CameraPosition = closest_object.value()->m_pos;

            m_move_axis = Vector3Zero();
            m_scale_axis = Vector3Zero();
        }
    }

    if (m_selected_object != nullptr)
    {
        if (IsKeyPressed(KEY_W))
        {
            zero_transform_axises();
            if (IsKeyDown(KEY_LEFT_CONTROL))
                m_scale_axis.y = 1;
            else
                m_move_axis.y = 1;
        }
        else if (IsKeyPressed(KEY_D))
        {
            zero_transform_axises();
            if (IsKeyDown(KEY_LEFT_CONTROL))
                m_scale_axis.x = 1;
            else
                m_move_axis.x = 1;
        }
        else if (IsKeyPressed(KEY_S))
        {
            zero_transform_axises();
            if (IsKeyDown(KEY_LEFT_CONTROL))
                m_scale_axis.z = 1;
            else
                m_move_axis.z = 1;
        }
        else if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT))
        {
            zero_transform_axises();
        }
    }
}

void SceneEditor::render_scene()
{
    BeginMode3D(m_camera.ViewCamera);
    m_scene->render();

    if (m_selected_object != nullptr)
        render_transform_gizmo();
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

    bool is_file_browser_visible = ImGui::Button("Load Scene");
    render_file_browser(is_file_browser_visible);
    bool is_spawn_menu_visible = ImGui::Button("Spawn Object");
    render_spawn_menu(is_spawn_menu_visible);
    render_object_menu();

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

    if (ImGui::BeginPopup("File Browser"))
    {
        static int item_current = 0;
        std::vector<std::string> files = parse_directory("res/scenes", ".json");

        bool was_scene_loaded = false;
        if (ImGui::ListBox("##", &item_current, files_getter, &files, files.size(),
                           files.size()))
        {
            load_scene(fmt::format("{}/{}.json", "res/scenes", files[item_current]));
            was_scene_loaded = true;
        }

        ImGui::EndPopup();

        if (was_scene_loaded)
            return;
    }
}

void SceneEditor::render_spawn_menu(bool is_visible)
{
    static bool old_visibility = false;

    if (old_visibility != is_visible)
    {
        old_visibility = is_visible;

        if (is_visible)
        {
            ImGui::OpenPopup("Spawn Menu");
        }
    }

    ImGui::PushItemWidth(10.0f);
    if (ImGui::BeginPopup("Spawn Menu"))
    {
        static int item_current = 0;
        std::vector<std::string> files = parse_directory("res/models", ".glb");

        bool was_object_spawned = false;
        if (ImGui::ListBox("##", &item_current, files_getter, &files, files.size(),
                           files.size()))
        {
            m_scene->spawn(create_default_object(files[item_current]));
            was_object_spawned = true;
        }

        ImGui::EndPopup();

        if (was_object_spawned)
            return;
    }
    ImGui::PopItemWidth();
}

void SceneEditor::render_object_menu()
{
    if (m_selected_object != nullptr)
    {
        bool is_open = true;
        if (ImGui::Begin("Object Menu", &is_open))
        {
            /* NAME */
            ImGui::InputText("Name", m_selected_object->m_name.data(),
                             m_selected_object->m_name.capacity() + 1,
                             ImGuiInputTextFlags_CallbackResize,
                             input_text_resize_callback, &m_selected_object->m_name);

            /* MODEL */
            static int current_model = 0;
            std::vector<std::string> files = parse_directory("res/models", ".glb");
            if (ImGui::ListBox("Model", &current_model, files_getter, &files,
                               files.size(), files.size()))
            {
                m_selected_object->m_model_name = files[current_model];
                m_selected_object->m_model =
                    AssetManager::get().get_model(files[current_model]);
            }

            /* POSITION */
            ImGui::DragFloat3("Position",
                              reinterpret_cast<float*>(&m_selected_object->m_pos), 0.2f,
                              -1000.0f, 1000.0f, "%.2f");
            /* ROTATION AXIS */
            ImGui::DragFloat3(
                "Rotation Axis",
                reinterpret_cast<float*>(&m_selected_object->m_rotation_axis), 0.01f,
                0.0f, 1.0f, "%.2f");
            /* ROTATION ANGLE */
            ImGui::DragFloat("Rotation Angle", &m_selected_object->m_angle, 2.0f, 0.0f,
                             360.0f, "%f");
            /* SCALE */
            ImGui::DragFloat3("Scale",
                              reinterpret_cast<float*>(&m_selected_object->m_scale), 0.1f,
                              1.0f, 100.0f, "%.2f");
            /* COLOR */
            ImGui::ColorEdit4("Color",
                              reinterpret_cast<float*>(&m_selected_object->m_color));

            if (ImGui::Button("Delete"))
            {
                m_scene->despawn(m_selected_object->m_name);
                m_selected_object = nullptr;
            }
        }
        ImGui::End();
    }
}

void SceneEditor::serialize() const
{
    m_scene->serialize("res/scenes");
}

void SceneEditor::load_scene(std::string_view path)
{
    m_scene = std::make_unique<Scene>(path);
}

std::shared_ptr<Object> SceneEditor::create_default_object(
    const std::string& model_name) const
{
    static int default_object_num = 1;
    auto object = std::make_shared<Object>(
        fmt::format("object{}", default_object_num), model_name, m_camera.CameraPosition,
        Vector3{0.0f, 0.0f, 0.0f}, 0.0f, Vector3{1.0f, 1.0f, 1.0f},
        ImGuiColor{1.0f, 1.0f, 1.0f, 1.0f});
    default_object_num++;
    return object;
}

void SceneEditor::zero_transform_axises()
{
    m_move_axis = Vector3Zero();
    m_scale_axis = Vector3Zero();
}

[[nodiscard]] Vector3 SceneEditor::calc_mouse_dist_traveled() const
{
    int speed = 5.0f;
    Vector2 mouse_pos = GetMousePosition();
    Vector3 distance{
        mouse_pos.x - m_prev_mouse_pos.x,
        m_prev_mouse_pos.y - mouse_pos.y,
        mouse_pos.y - m_prev_mouse_pos.y,
    };
    return Vector3Scale(distance, GetFrameTime() * speed);
}

[[nodiscard]] std::optional<std::shared_ptr<Object>>
SceneEditor::get_closest_clicked_object() const
{
    if (!m_scene->m_objects.empty() && !ImGui::GetIO().WantCaptureMouse)
    {
        auto ray = GetMouseRay(GetMousePosition(), m_camera.ViewCamera);

        std::optional<std::shared_ptr<Object>> closest_object = std::nullopt;
        float min_distance = 1000.0f;
        for (const auto& object : m_scene->m_objects)
        {
            // Every object probably has custom transform,
            // which prevents us from just using `GetRayCollisionModel`.
            // So the only way to account for custom transform is to use
            // `GetRayCollisionMesh`.
            Mesh* meshes = object->m_model.lock()->meshes;
            for (int i = 0; i < object->m_model.lock()->meshCount; i++)
            {
                Matrix transform = MatrixIdentity();
                transform = MatrixMultiply(
                    transform,
                    MatrixScale(object->m_scale.x, object->m_scale.y, object->m_scale.z));
                transform = MatrixMultiply(
                    transform, MatrixRotate(object->m_rotation_axis, object->m_angle));
                transform = MatrixMultiply(
                    transform,
                    MatrixTranslate(object->m_pos.x, object->m_pos.y, object->m_pos.z));

                auto collision = GetRayCollisionMesh(
                    ray, meshes[i],
                    MatrixMultiply(object->m_model.lock()->transform, transform));
                if (collision.hit && collision.distance <= min_distance)
                {
                    min_distance = collision.distance;
                    closest_object = object;
                }
            }
        }
        return closest_object;
    }

    return std::nullopt;
}