#pragma once

#include <vector>
#include <string_view>

#include "raylib.h"

#include "scene_model.h"

class Scene
{
public:
    Scene(std::string_view path);

    void render();
    void serialize() const;

private:
    std::string m_name;
    std::vector<SceneModel> m_models;

    bool m_is_loaded = false;
};
