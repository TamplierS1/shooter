#include <fstream>

#include "fmt/format.h"
#include "nlohmann/json.hpp"

#include "scene.h"

using json = nlohmann::json;

Scene::Scene(std::string_view path)
{
    std::ifstream file{path.data()};
    json scene;
    file >> scene;

    //    if (!scene.contains("models"))
    //        // TODO: throw something here.
    //        return;
    //    if (!scene["models"].is_array())
    //        // TODO: throw something here.
    //        return;

    m_name = scene["Name"];

    for (const auto& object : scene["Objects"])
    {
        std::string object_name = object["Name"];
        std::string model_name = object["ModelName"];
        Vector3 pos{object["Position"]["x"], object["Position"]["y"],
                    object["Position"]["z"]};
        Vector3 rot_axis{object["RotationAxis"]["x"], object["RotationAxis"]["y"],
                         object["RotationAxis"]["z"]};
        float rot_angle = object["RotationAngle"];
        Vector3 scale{object["Scale"]["x"], object["Scale"]["y"], object["Scale"]["z"]};
        ImGuiColor color{object["Color"]["r"], object["Color"]["g"], object["Color"]["b"],
                    object["Color"]["a"]};

        m_objects.emplace_back(std::make_shared<Object>(object_name, model_name, pos, rot_axis, rot_angle, scale,
                               color));
    }

    m_is_loaded = true;
}

void Scene::render()
{
    if (m_is_loaded)
    {
        for (auto& object : m_objects)
        {
            object->render();
        }
    }
}

void Scene::serialize(std::string_view path_to_dir) const
{
    std::ofstream file{fmt::format("{}/{}.json", path_to_dir, m_name)};

    json scene;
    scene["Name"] = m_name;
    scene["Objects"] = {};

    for (const auto& object : m_objects)
    {
        scene["Objects"].push_back(object->serialize());
    }

    file << scene;
}
