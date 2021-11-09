#pragma once

#include <memory>
#include <string_view>

#include "nlohmann/json.hpp"
#include "raylib.h"

using json = nlohmann::json;

class Object
{
public:
    Object() = default;
    Object(const std::string& object_name, const std::string& model_name, Vector3 pos,
           Vector3 rot_axis, float rot_angle, Vector3 scale, Color color);

    void render();
    [[nodiscard]] json serialize() const;

    std::weak_ptr<Model> m_model;

    std::string m_name = "default_object";
    std::string m_model_name = "default_model";
    Vector3 m_pos = {0.0f, 0.0f, 0.0f};
    Vector3 m_rotation_axis = {1.0f, 0.0f, 0.0f};
    float m_angle = 0.0f;
    Vector3 m_scale = {1.0f, 1.0f, 1.0f};
    Color m_color = WHITE;
};
