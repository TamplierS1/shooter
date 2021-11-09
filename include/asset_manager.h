#pragma once

#include <unordered_map>
#include <memory>
#include <string_view>

#include "raylib.h"

class AssetManager
{
public:
    AssetManager(const AssetManager&) = delete;
    AssetManager(AssetManager&&) = delete;

    [[nodiscard]] static AssetManager& get()
    {
        static AssetManager instance;
        return instance;
    }

    // The path to the directory that contains the models.
    void load_models(std::string_view path);
    // Use this method when you want to access an individual model.
    std::shared_ptr<Model> get_model(const std::string& name);

    // This is accessed only when you need access to all the models.
    std::unordered_map<std::string, std::shared_ptr<Model>> m_models;
private:
    AssetManager() = default;
};
