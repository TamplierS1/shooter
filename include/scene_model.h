#pragma once

#include <memory>
#include <string_view>

#include "raylib.h"

class SceneModel
{
public:
    SceneModel(std::string_view path, Vector3 pos, Vector3 rot_axis, float rot_angle,
               Vector3 scale, Color color);

    void render();

    std::unique_ptr<Model> m_rl_model;

    Vector3 m_pos = {0.0f, 0.0f, 0.0f};
    Vector3 m_rotation_axis = {1.0f, 0.0f, 0.0f};
    float m_angle = 0.0f;
    Vector3 m_scale = {1.0f, 1.0f, 1.0f};
    Color m_color = WHITE;
};
