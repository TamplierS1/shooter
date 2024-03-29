#pragma once

#include <memory>
#include <optional>

#include "TPOrbitCamera.h"

#include "scene.h"

class SceneEditor
{
public:
    SceneEditor();
    ~SceneEditor();

    int run();

private:
    void handle_input();
    void render_scene();
    void render_gui();
    void render_file_browser(bool is_visible);
    void render_spawn_menu(bool is_visible);
    void render_object_menu();
    void render_transform_gizmo();

    void update_object_transform(Object* object);

    void serialize() const;
    void load_scene(std::string_view path);
    [[nodiscard]] std::shared_ptr<Object> create_default_object(
        const std::string& model_name) const;
    void zero_transform_axises();
    [[nodiscard]] Vector3 calc_mouse_dist_traveled() const;
    [[nodiscard]] std::optional<std::shared_ptr<Object>> get_closest_clicked_object()
        const;

    bool m_is_active = true;

    const int m_win_width = 1920;
    const int m_win_height = 1080;
    TPOrbitCamera m_camera;

    std::unique_ptr<Scene> m_scene;
    std::shared_ptr<Object> m_selected_object;

    Vector3 m_move_axis = {0, 0, 0};
    Vector3 m_scale_axis = {0, 0, 0};
    Vector2 m_prev_mouse_pos = {0, 0};
};
