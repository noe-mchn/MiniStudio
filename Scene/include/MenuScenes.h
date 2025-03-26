#pragma once
#include "SceneBase.h"
#include "Button.h"
#include "ScoreManager.h"
#include <vector>
#include <SFML/Graphics.hpp>

class SceneManager;
class Game;

//===== MENU PRINCIPAL =====//
class MainMenuScene : public ISceneBase
{
public:
    MainMenuScene(sf::RenderWindow* window, const float& framerate, TextureCache* texture, SceneManager* sceneManager);
    ~MainMenuScene() override;

    void Update(const float& deltatime) override;
    void ProcessInput(const sf::Event& event) override;
    void Render() override;

private:
    /*sf::Font m_font;
    sf::Text m_titleText;*/
    std::vector<Button*> m_buttons;


private:

    sf::Texture m_titleTexture;
    sf::Sprite m_titleSprite;

    sf::Texture m_PlayTexture;
    sf::Sprite m_PlaySprite;

    sf::Texture m_HistoryTexture;
    sf::Sprite m_HistorySprite;

    sf::Texture m_QuitTexture;
    sf::Sprite m_QuitSprite;


    sf::RectangleShape m_backgroundShape;

    SceneManager* m_sceneManager;

    void setupTitle();
    void setupButtons();
};


//===== MENU PAUSE =====//
class PauseMenuScene : public ISceneBase
{
public:
    PauseMenuScene(sf::RenderWindow* window, const float& framerate, TextureCache* texture, SceneManager* sceneManager);
    ~PauseMenuScene() override;

    void Update(const float& deltatime) override;
    void ProcessInput(const sf::Event& event) override;
    void ButtonPressed();
    void Render() override;

private:
    sf::Font m_font;
    sf::Text m_titleText;
    sf::RectangleShape m_overlay;
    std::vector<Button*> m_buttons;

    sf::RectangleShape m_backgroundShape;

    SceneManager* m_sceneManager;

    void setupTitle();
    void setupButtons();
};

//===== GAME OVER =====//
class GameOverScene : public ISceneBase
{
public:
    GameOverScene(sf::RenderWindow* window, const float& framerate, TextureCache* texture, SceneManager* sceneManager);
    ~GameOverScene() override;

    void Update(const float& deltatime) override;
    void ProcessInput(const sf::Event& event) override;
    void Render() override;

private:
    sf::Font m_font;
    sf::Text m_titleText;
    sf::Text m_scoreText;
    std::vector<Button*> m_buttons;

private:
    sf::Texture m_titleTexture;
    sf::Sprite m_titleSprite;

    sf::Texture m_tryagainTexture;
    sf::Sprite m_tryagainSprite;

    sf::Texture m_flowerTexture;
    sf::Sprite m_flowerSprite;

    sf::Texture m_quitGameTexture;
    sf::Sprite m_quitGameSprite;


    sf::RectangleShape m_backgroundShape;

    SceneManager* m_sceneManager;


    bool m_nameInputActive;
    sf::Text m_namePromptText;
    sf::Text m_nameInputText;
    std::string m_playerName;

    void setupTitle();
    void setupButtons();
    //void updateScoreDisplay();
    void checkHighScore();
    void submitHighScore();
};