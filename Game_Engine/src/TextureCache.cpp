#include "TextureCache.h"
#include <SFML/Graphics.hpp>
#include <iostream>

TextureCache::TextureCache(const std::string& execFilePath)
    : m_execFilePath(execFilePath)
{
    m_resourceBasePath = findResourceFolder();
    std::cout << "Resource base path initialized to: " << m_resourceBasePath << std::endl;
}

TextureCache::~TextureCache()
{
    for (auto& textureInfo : m_allTextureInfos)
        delete textureInfo.texture;
    m_allTextureInfos.clear();
}

void TextureCache::debugResourcePath()
{
    std::cout << "Current resource path: " << m_resourceBasePath << std::endl;
    std::cout << "Executable path: " << m_execFilePath << std::endl;
}

std::string TextureCache::getAbsoluteFilepath(const std::string& filename)
{
    return (std::filesystem::path(m_resourceBasePath) / filename).string();
}

sf::Texture& TextureCache::getTexture(const std::string& filename)
{
    std::string path = getAbsoluteFilepath(filename);
    for (auto& texture : m_allTextureInfos)
    {
        if (texture.path == path)
            return *texture.texture;
    }

    TextureInfo ti;
    ti.path = path;
    ti.texture = new sf::Texture;
	
    if (!ti.texture->loadFromFile(path))
    {
        std::cerr << "Failed to load image \"" << path << "\". Reason: Unable to open file" << std::endl;
    }

    m_allTextureInfos.push_back(ti);
    return *ti.texture;
}

// Implémentation de TextureManager
TextureManager::TextureManager()
    : m_initialized(false)
{
}

TextureManager::~TextureManager()
{
    cleanup();
}

void TextureManager::initialize()
{
    if (!m_initialized)
    {
        m_initialized = true;
        std::cout << "TextureManager initialized" << std::endl;
    }
}

void TextureManager::cleanup()
{
    m_textures.clear();
    m_initialized = false;
    std::cout << "TextureManager cleaned up" << std::endl;
}

bool TextureManager::loadTexture(const std::string& name, const std::string& filename)
{
    if (hasTexture(name))
        return true;

    auto texture = std::make_unique<sf::Texture>();

    std::string fullPath = (std::filesystem::path(findResourceFolder()) / filename).string();

    if (!texture->loadFromFile(fullPath))
    {
        std::cerr << "Failed to load texture: " << fullPath << std::endl;
        return false;
    }

    texture->setSmooth(true);
    m_textures[name] = std::move(texture);
    std::cout << "Loaded texture: " << name << " from " << fullPath << std::endl;

    return true;
}

sf::Texture* TextureManager::getTexture(const std::string& name)
{
    auto it = m_textures.find(name);
    if (it != m_textures.end())
    {
        return it->second.get();
    }

    std::cerr << "Texture not found: " << name << std::endl;
    return nullptr;
}

void TextureManager::removeTexture(const std::string& name)
{
    auto it = m_textures.find(name);
    if (it != m_textures.end())
    {
        m_textures.erase(it);
        std::cout << "Removed texture: " << name << std::endl;
    }
}

bool TextureManager::hasTexture(const std::string& name) const
{
    return m_textures.find(name) != m_textures.end();
}

sf::Vector2u TextureManager::getTextureDimensions(const std::string& name) const
{
    auto it = m_textures.find(name);
    if (it != m_textures.end())
    {
        return it->second->getSize();
    }

    return sf::Vector2u(0, 0);
}

bool TextureManager::loadTexturesFromDirectory(const std::string& directory, const std::string& pattern)
{
    std::string fullDirectoryPath = (std::filesystem::path(findResourceFolder()) / directory).string();
    bool loadedAny = false;

    try
    {
        for (const auto& entry : std::filesystem::directory_iterator(fullDirectoryPath))
        {
            if (entry.is_regular_file())
            {
                std::string filename = entry.path().filename().string();
                std::string extension = entry.path().extension().string();

                if ((pattern == "*" || filename.find(pattern) != std::string::npos) &&
                    (extension == ".png" || extension == ".jpg" || extension == ".jpeg" ||
                        extension == ".bmp" || extension == ".tga"))
                {
                    std::string name = entry.path().stem().string();
                    std::string relativePath = directory + "/" + filename;

                    if (loadTexture(name, relativePath))
                    {
                        loadedAny = true;
                    }
                }
            }
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error loading textures from directory: " << e.what() << std::endl;
        return false;
    }

    return loadedAny;
}

std::vector<std::string> TextureManager::getLoadedTextureNames() const
{
    std::vector<std::string> names;
    names.reserve(m_textures.size());

    for (const auto& pair : m_textures)
    {
        names.push_back(pair.first);
    }

    return names;
}

void TextureManager::loadAllGameTextures()
{
    if (!m_initialized)
        initialize();

    loadTexture("Hero", "Hero.png");
    loadTexture("Boss1", "Boss1.png");

    std::cout << "All game textures loaded successfully\n";
}