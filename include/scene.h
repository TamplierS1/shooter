#pragma once

#include <vector>
#include <string_view>

#include "raylib.h"

#include "object.h"

class Scene
{
public:
    Scene() = default;
    explicit Scene(std::string_view path);

    void spawn(const std::shared_ptr<Object>& object);
    bool despawn(std::string_view object_name);
    void render();
    void serialize(std::string_view path_to_dir) const;

    std::vector<std::shared_ptr<Object>> m_objects;

private:
    std::string m_name = "default_scene";

    bool m_is_loaded = false;
};
