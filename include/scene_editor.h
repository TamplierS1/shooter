#pragma once

#include <memory>

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
    void render_object_menu();

    void serialize() const;
    void load_scene(std::string_view path);

    bool m_is_active = true;

    const int m_win_width = 1920;
    const int m_win_height = 1080;
    TPOrbitCamera m_camera;

    std::unique_ptr<Scene> m_scene;
    std::shared_ptr<Object> m_selected_object;
};
