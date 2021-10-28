#include "scene_model.h"

SceneModel::SceneModel(std::string_view path, Vector3 pos, Vector3 rot_axis,
                       float rot_angle, Vector3 scale, Color color)
    : m_pos(pos)
    , m_rotation_axis(rot_axis)
    , m_angle(rot_angle)
    , m_scale(scale)
    , m_color(color)

{
    m_rl_model = std::make_unique<Model>(LoadModel(path.data()));
}

void SceneModel::render()
{
    DrawModelEx(*m_rl_model, m_pos, m_rotation_axis, m_angle, m_scale, m_color);
}
