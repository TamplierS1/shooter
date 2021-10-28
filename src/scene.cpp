#include <fstream>

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

    for (const auto& model : scene["Models"])
    {
        std::string obj_path = model["Path"];
        Vector3 pos{model["Position"]["x"], model["Position"]["y"],
                    model["Position"]["z"]};
        Vector3 rot_axis{model["RotationAxis"]["x"], model["RotationAxis"]["y"],
                         model["RotationAxis"]["z"]};
        float rot_angle = model["RotationAngle"];
        Vector3 scale{model["Scale"]["x"], model["Scale"]["y"], model["Scale"]["z"]};
        Color color{model["Color"]["r"], model["Color"]["g"], model["Color"]["b"],
                    model["Color"]["a"]};

        m_models.emplace_back(obj_path, pos, rot_axis, rot_angle, scale, color);
    }

    m_is_loaded = true;
}

void Scene::render()
{
    if (m_is_loaded)
    {
        for (auto& model : m_models)
        {
            model.render();
        }
    }
}

void Scene::serialize() const
{
}
