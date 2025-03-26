#pragma once
#include<string>
#include <SFML/Graphics.hpp>
#include "TextureCache.h"

class ISceneBase;
using sfStyle = int;


class SceneManager
{
public:

    SceneManager(const std::string& execFilePath, const size_t& width, const size_t& height, const std::string& title, const sfStyle& style = sf::Style::Default);

    ~SceneManager();
    void Exe();

    void AddScene(ISceneBase* scene);

    void SetScene(const int& idx);

    ISceneBase* getScene(const int& idx) { return m_scene[idx]; }



    void SetSelectedCharacter(const std::string& name) { m_selectedCharacter = name; }
    std::string GetSelectedCharacter() const { return m_selectedCharacter; }

    sf::RenderWindow* getWindow();

    TextureCache* getTextureCache();

private:
    sf::RenderWindow* m_window;
    std::vector<ISceneBase*> m_scene;
    ISceneBase* m_currentScene;
    sf::Event m_event;
    TextureCache* m_texture;
    std::string m_selectedCharacter;
};