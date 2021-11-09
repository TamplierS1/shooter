#include <experimental/filesystem>
#include <cassert>

#include "asset_manager.h"

namespace fs = std::experimental::filesystem;

void AssetManager::load_models(std::string_view path)
{
    if (fs::exists(path) && fs::is_directory(path))
    {
        for (const auto& entry : fs::directory_iterator(path))
        {
            if (entry.path().extension() == ".obj")
            {
                std::string model_name = entry.path().stem();
                if (m_models.contains(model_name))
                    continue;

                m_models.emplace(
                    model_name, std::make_unique<Model>(LoadModel(entry.path().c_str())));
            }
        }
    }
}

std::shared_ptr<Model> AssetManager::get_model(const std::string& name)
{
    assert(m_models.contains(name));

    return m_models[name];
}
