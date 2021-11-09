#include "object.h"
#include "asset_manager.h"

Object::Object(const std::string& object_name, const std::string& model_name, Vector3 pos,
               Vector3 rot_axis, float rot_angle, Vector3 scale, Color color)
    : m_name(object_name)
    , m_model_name(model_name)
    , m_pos(pos)
    , m_rotation_axis(rot_axis)
    , m_angle(rot_angle)
    , m_scale(scale)
    , m_color(color)

{
    m_model = AssetManager::get().get_model(model_name);
}

void Object::render()
{
    DrawModelEx(*m_model.lock(), m_pos, m_rotation_axis, m_angle, m_scale, m_color);
}

nlohmann::json Object::serialize() const
{
    json object;
    object["Name"] = m_name;
    object["ModelName"] = m_model_name;
    object["Position"] = {{"x", m_pos.x}, {"y", m_pos.y}, {"z", m_pos.z}};
    object["RotationAxis"] = {{"x", m_rotation_axis.x}, {"y", m_rotation_axis.y}, {"z", m_rotation_axis.z}};
    object["RotationAngle"] = m_angle;
    object["Scale"] = {{"x", m_scale.x}, {"y", m_scale.y}, {"z", m_scale.z}};
    object["Color"] = {{"r", m_color.r}, {"g", m_color.g}, {"b", m_color.b}, {"a", m_color.a}};
    return object;
}
