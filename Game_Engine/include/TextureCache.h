#pragma once
#include <SFML/Graphics.hpp>
#include <filesystem>
#include <unordered_map>
#include <memory>
#include <vector>
#include <string>
#include <iostream>

namespace sf { class Texture; }

inline std::string findResourceFolder() {
    try {
        std::filesystem::path currentPath = std::filesystem::current_path();

        if (std::filesystem::exists(currentPath / "Re")) {
            return (currentPath / "Re").string();
        }

        std::filesystem::path buildPath = currentPath;
        for (int i = 0; i < 5; i++) {
            if (std::filesystem::exists(buildPath / "Re")) {
                return (buildPath / "Re").string();
            }

            if (std::filesystem::exists(buildPath.parent_path() / "Re")) {
                return (buildPath.parent_path() / "Re").string();
            }

            buildPath = buildPath.parent_path();
        }

        return (currentPath / "Re").string();
    }
    catch (const std::exception& e) {
        std::cerr << "Error finding resource folder: " << e.what() << std::endl;
        return "Re";
    }
}

class TextureCache
{
public:
    TextureCache(const std::string& execFilePath);
    ~TextureCache();

    sf::Texture& getTexture(const std::string& filename);
    void debugResourcePath();

private:
    struct TextureInfo
    {
        sf::Texture* texture;
        std::string path;
    };

    std::string getAbsoluteFilepath(const std::string& filename);

    std::vector<TextureInfo> m_allTextureInfos;
    std::string m_execFilePath;
    std::string m_resourceBasePath;
};

class TextureManager
{
public:
    static TextureManager& getInstance()
    {
        static TextureManager instance;
        return instance;
    }

    TextureManager(const TextureManager&) = delete;
    TextureManager& operator=(const TextureManager&) = delete;

    void initialize();
    void cleanup();

    bool loadTexture(const std::string& name, const std::string& filename);
    sf::Texture* getTexture(const std::string& name);
    void removeTexture(const std::string& name);
    bool hasTexture(const std::string& name) const;
    sf::Vector2u getTextureDimensions(const std::string& name) const;
    bool loadTexturesFromDirectory(const std::string& directory, const std::string& pattern = "*");
    std::vector<std::string> getLoadedTextureNames() const;
    void loadAllGameTextures();

private:
    TextureManager();
    ~TextureManager();

    std::unordered_map<std::string, std::unique_ptr<sf::Texture>> m_textures;
    bool m_initialized;
};