#pragma once
#include <SFML/Graphics.hpp>
#include <filesystem>
#include <unordered_map>
#include <memory>
#include <vector>
#include <string>
#include <iostream>

namespace sf { class Texture; }

// Fonction utilitaire pour trouver le dossier Re
inline std::string findResourceFolder() {
    try {
        // Commençons par le chemin actuel
        std::filesystem::path currentPath = std::filesystem::current_path();
        std::cout << "Current path: " << currentPath.string() << std::endl;

        // Vérifions si Re est dans le répertoire courant
        if (std::filesystem::exists(currentPath / "Re")) {
            return (currentPath / "Re").string();
        }

        // Vérifions une structure typique de projets
        std::filesystem::path buildPath = currentPath;
        for (int i = 0; i < 5; i++) { // Essayer jusqu'à 5 niveaux vers le haut
            if (std::filesystem::exists(buildPath / "Re")) {
                return (buildPath / "Re").string();
            }

            // Vérifions aussi à côté du chemin actuel
            if (std::filesystem::exists(buildPath.parent_path() / "Re")) {
                return (buildPath.parent_path() / "Re").string();
            }

            buildPath = buildPath.parent_path();
        }

        // En dernier recours, renvoyer un chemin par rapport au répertoire courant
        return (currentPath / "Re").string();
    }
    catch (const std::exception& e) {
        std::cerr << "Error finding resource folder: " << e.what() << std::endl;
        return "Re"; // Fallback
    }
}

class TextureCache
{
public:
    TextureCache(const std::string& execFilePath);
    ~TextureCache();

    sf::Texture& getTexture(const std::string& filename);
    // Nouvelle méthode pour afficher où les textures sont cherchées
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
    std::string m_resourceBasePath; // Nouveau: stocke le chemin de base vers les ressources
};

// TextureManager reste inchangé pour l'instant
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