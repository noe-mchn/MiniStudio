#include "MenuScenes.h"
#include "SceneManager.h"
#include "Game.h"
#include <iostream>
#include <sstream>

//===== IMPLÉMENTATION DU MENU PRINCIPAL =====//

MainMenuScene::MainMenuScene(sf::RenderWindow* window, const float& framerate, TextureCache* texture, SceneManager* sceneManager)
    : ISceneBase(window, framerate, texture)
    , m_sceneManager(sceneManager)
{
    if (!m_font.loadFromFile("arial.ttf")) 
    {
        std::cerr << "Couldn't load font!" << std::endl;
    }

    m_backgroundShape.setSize(sf::Vector2f(window->getSize().x, window->getSize().y));
    m_backgroundShape.setPosition(0, 0);
    m_backgroundShape.setFillColor(sf::Color(20, 20, 50));

    getWindow()->setMouseCursorVisible(true);

    setupTitle();
    setupButtons();
}

MainMenuScene::~MainMenuScene() 
{
    for (auto button : m_buttons) 
    {
        delete button;
    }
    m_buttons.clear();
}

void MainMenuScene::Update(const float& deltatime) 
{
    sf::Vector2i mousePos = sf::Mouse::getPosition(*getWindow());
    sf::Vector2f mousePosF(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));

    for (auto button : m_buttons) 
    {
        button->update(mousePosF);
    }
    getWindow()->setMouseCursorVisible(true);
}

void MainMenuScene::ProcessInput(const sf::Event& event) 
{
    sf::Vector2i mousePos = sf::Mouse::getPosition(*getWindow());
    sf::Vector2f mousePosF(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));

    for (auto button : m_buttons) 
    {
        button->handleEvent(event, mousePosF);
    }
}

void MainMenuScene::Render() 
{
    sf::RenderWindow* window = getWindow();

    window->draw(m_backgroundShape);
    window->draw(m_titleText);

    for (auto button : m_buttons) 
    {
        button->draw(*window);
    }
}

void MainMenuScene::setupTitle() 
{
    m_titleText.setFont(m_font);
    m_titleText.setString("CYBERPUNK SHOOTER");
    m_titleText.setCharacterSize(72);
    m_titleText.setFillColor(sf::Color::White);
    m_titleText.setStyle(sf::Text::Bold);

    sf::FloatRect titleBounds = m_titleText.getLocalBounds();
    m_titleText.setOrigin(titleBounds.left + titleBounds.width / 2.0f,
        titleBounds.top + titleBounds.height / 2.0f);
    m_titleText.setPosition(getWindow()->getSize().x / 2.0f, getWindow()->getSize().y / 4.0f);
}

void MainMenuScene::setupButtons() 
{
    sf::Vector2f buttonSize(300, 60);
    float startY = getWindow()->getSize().y / 2.0f;
    float spacing = 80.0f;


    Button* playButton = new Button(
        sf::Vector2f((getWindow()->getSize().x - buttonSize.x) / 2.0f, startY),
        buttonSize,
        "JOUER",
        m_font,
        32
    );
    playButton->setCallback([this]() 
        {
        Game* gameScene = dynamic_cast<Game*>(m_sceneManager->getScene(1));
        if (gameScene) 
        {
            gameScene->ResetGame();
            getWindow()->setMouseCursorVisible(false);
        }
        m_sceneManager->SetScene(1);
        });
    m_buttons.push_back(playButton);

    // Bouton Quitter
    Button* quitButton = new Button(
        sf::Vector2f((getWindow()->getSize().x - buttonSize.x) / 2.0f, startY + spacing),
        buttonSize,
        "QUITTER",
        m_font,
        32
    );
    quitButton->setCallback([this]() 
        {
        getWindow()->close();
        });
    m_buttons.push_back(quitButton);
}

////===== IMPLÉMENTATION DU MENU PAUSE =====//

PauseMenuScene::PauseMenuScene(sf::RenderWindow* window, const float& framerate, TextureCache* texture, SceneManager* sceneManager)
    : ISceneBase(window, framerate, texture)
    , m_sceneManager(sceneManager)
{

    if (!m_font.loadFromFile("arial.ttf")) 
    {
        std::cerr << "Couldn't load font!" << std::endl;
    }

    m_backgroundShape.setSize(sf::Vector2f(window->getSize().x, window->getSize().y));
    m_backgroundShape.setPosition(0, 0);
    m_backgroundShape.setFillColor(sf::Color(0, 0, 0, 0));

    m_overlay.setSize(sf::Vector2f(window->getSize().x, window->getSize().y));
    m_overlay.setFillColor(sf::Color(0, 0, 0, 180));
    m_overlay.setPosition(0, 0);

    setupTitle();
    setupButtons();

    window->setMouseCursorVisible(true);
}

PauseMenuScene::~PauseMenuScene() 
{
    for (auto button : m_buttons) 
    {
        delete button;
    }
    m_buttons.clear();
}

void PauseMenuScene::Update(const float& deltatime) 
{
    sf::Vector2i mousePos = sf::Mouse::getPosition(*getWindow());
    sf::Vector2f mousePosF(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));

    for (auto button : m_buttons) 
    {
        button->update(mousePosF);
    }
}

void PauseMenuScene::ButtonPressed()
{
    Game* gameScene = dynamic_cast<Game*>(m_sceneManager->getScene(1));
    if (gameScene)
    {
        gameScene->SetPaused(false);
        getWindow()->setMouseCursorVisible(false);
    }
    else
    {
        std::cout << "Cast échoué dans ButtonPressed()" << std::endl;
    }

    m_sceneManager->SetScene(1);
}

void PauseMenuScene::ProcessInput(const sf::Event& event) 
{
    sf::Vector2i mousePos = sf::Mouse::getPosition(*getWindow());
    sf::Vector2f mousePosF(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));


    if (event.type == sf::Event::KeyPressed)
    {
        if (event.key.code == sf::Keyboard::P || event.key.code == sf::Keyboard::Escape) 
        {
            Game* gameScene = dynamic_cast<Game*>(m_sceneManager->getScene(1));
            if (gameScene) 
            {
                gameScene->SetPaused(false);
                getWindow()->setMouseCursorVisible(false);
            }
            m_sceneManager->SetScene(1);
        }
    }

    for (auto button : m_buttons) 
    {
        button->handleEvent(event, mousePosF);
    }
}

void PauseMenuScene::Render() 
{
    sf::RenderWindow* window = getWindow();

    window->draw(m_backgroundShape);
    window->draw(m_overlay);
    window->draw(m_titleText);

    for (auto button : m_buttons) 
    {
        button->draw(*window);
    }
}

void PauseMenuScene::setupTitle() 
{
    m_titleText.setFont(m_font);
    m_titleText.setString("PAUSE");
    m_titleText.setCharacterSize(72);
    m_titleText.setFillColor(sf::Color::White);
    m_titleText.setStyle(sf::Text::Bold);

    sf::FloatRect titleBounds = m_titleText.getLocalBounds();
    m_titleText.setOrigin(titleBounds.left + titleBounds.width / 2.0f,
        titleBounds.top + titleBounds.height / 2.0f);
    m_titleText.setPosition(getWindow()->getSize().x / 2.0f, getWindow()->getSize().y / 4.0f);
}

void PauseMenuScene::setupButtons() 
{
    sf::Vector2f buttonSize(300, 60);
    float startY = getWindow()->getSize().y / 2.0f;
    float spacing = 80.0f;

    Button* resumeButton = new Button(
        sf::Vector2f((getWindow()->getSize().x - buttonSize.x) / 2.0f, startY),
        buttonSize,
        "REPRENDRE",
        m_font,
        32 );
    
    resumeButton->setCallback(std::bind(&PauseMenuScene::ButtonPressed, this));

    m_buttons.push_back(resumeButton);

    Button* restartButton = new Button(
        sf::Vector2f((getWindow()->getSize().x - buttonSize.x) / 2.0f, startY + spacing),
        buttonSize,
        "RECOMMENCER",
        m_font,
        32
    );
    restartButton->setCallback([this]() 
        {

        Game* gameScene = dynamic_cast<Game*>(m_sceneManager->getScene(1));
        if (gameScene) {
            gameScene->ResetGame();
            getWindow()->setMouseCursorVisible(false);
        }
        m_sceneManager->SetScene(1);
        });
    m_buttons.push_back(restartButton);


    Button* mainMenuButton = new Button(
        sf::Vector2f((getWindow()->getSize().x - buttonSize.x) / 2.0f, startY + spacing * 2),
        buttonSize,
        "MENU PRINCIPAL",
        m_font,
        32
    );
    mainMenuButton->setCallback([this]() 
        {
        getWindow()->setMouseCursorVisible(true);
        m_sceneManager->SetScene(0);
        });
    m_buttons.push_back(mainMenuButton);
}

////===== IMPLÉMENTATION DE LA SCÈNE DE GAME OVER =====//
//
//GameOverScene::GameOverScene(sf::RenderWindow* window, const float& framerate, TextureCache* texture, SceneManager* sceneManager)
//    : ISceneBase(window, framerate, texture)
//    , m_sceneManager(sceneManager)
//    , m_nameInputActive(false)
//    , m_playerName("Player")
//{
//    // Charger la police
//    if (!m_font.loadFromFile("C:\\Code\\MiniStudio\\Re\\font\\BRLNSB")) {
//        // Utiliser une police par défaut de SFML si le chargement échoue
//        std::cerr << "Couldn't load font!" << std::endl;
//    }
//
//    // Créer l'arrière-plan
//    m_backgroundShape.setSize(sf::Vector2f(window->getSize().x, window->getSize().y));
//    m_backgroundShape.setPosition(0, 0);
//    m_backgroundShape.setFillColor(sf::Color(50, 0, 0, 220));
//
//    setupTitle();
//
//    // Configurer le texte du score
//    m_scoreText.setFont(m_font);
//    m_scoreText.setCharacterSize(48);
//    m_scoreText.setFillColor(sf::Color::White);
//    sf::FloatRect scoreBounds = m_scoreText.getLocalBounds();
//    m_scoreText.setOrigin(scoreBounds.left + scoreBounds.width / 2.0f,
//        scoreBounds.top + scoreBounds.height / 2.0f);
//    m_scoreText.setPosition(getWindow()->getSize().x / 2.0f, getWindow()->getSize().y / 3.0f);
//
//    // Configuration pour la saisie du nom
//    m_namePromptText.setFont(m_font);
//    m_namePromptText.setString("Nouveau record ! Entrez votre nom :");
//    m_namePromptText.setCharacterSize(32);
//    m_namePromptText.setFillColor(sf::Color::Yellow);
//    sf::FloatRect promptBounds = m_namePromptText.getLocalBounds();
//    m_namePromptText.setOrigin(promptBounds.left + promptBounds.width / 2.0f,
//        promptBounds.top + promptBounds.height / 2.0f);
//    m_namePromptText.setPosition(getWindow()->getSize().x / 2.0f, getWindow()->getSize().y / 2.0f - 30);
//
//    m_nameInputText.setFont(m_font);
//    m_nameInputText.setString(m_playerName + "_");
//    m_nameInputText.setCharacterSize(40);
//    m_nameInputText.setFillColor(sf::Color::White);
//    sf::FloatRect inputBounds = m_nameInputText.getLocalBounds();
//    m_nameInputText.setOrigin(inputBounds.left + inputBounds.width / 2.0f,
//        inputBounds.top + inputBounds.height / 2.0f);
//    m_nameInputText.setPosition(getWindow()->getSize().x / 2.0f, getWindow()->getSize().y / 2.0f + 30);
//
//    setupButtons();
//
//    // Préparer l'écran de GameOver
//    updateScoreDisplay();
//
//    // Rendre le curseur visible
//    window->setMouseCursorVisible(true);
//}
//
//GameOverScene::~GameOverScene() {
//    for (auto button : m_buttons) {
//        delete button;
//    }
//    m_buttons.clear();
//}
//
//void GameOverScene::Update(const float& deltatime) {
//    if (!m_nameInputActive) {
//        // Mise à jour des boutons avec la position de la souris
//        sf::Vector2i mousePos = sf::Mouse::getPosition(*getWindow());
//        sf::Vector2f mousePosF(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));
//
//        for (auto button : m_buttons) {
//            button->update(mousePosF);
//        }
//    }
//
//    // Faire clignoter le curseur pour la saisie du nom
//    static float blinkTimer = 0;
//    static bool showCursor = true;
//
//    if (m_nameInputActive) {
//        blinkTimer += deltatime;
//        if (blinkTimer >= 500) { // Basculer toutes les 500ms
//            showCursor = !showCursor;
//            m_nameInputText.setString(m_playerName + (showCursor ? "_" : ""));
//
//            sf::FloatRect inputBounds = m_nameInputText.getLocalBounds();
//            m_nameInputText.setOrigin(inputBounds.left + inputBounds.width / 2.0f,
//                inputBounds.top + inputBounds.height / 2.0f);
//            m_nameInputText.setPosition(getWindow()->getSize().x / 2.0f, getWindow()->getSize().y / 2.0f + 30);
//
//            blinkTimer = 0;
//        }
//    }
//}
//
//void GameOverScene::ProssesInput(const sf::Event& event) {
//    if (m_nameInputActive) {
//        if (event.type == sf::Event::TextEntered) {
//            // Gérer la saisie du nom
//            if (event.text.unicode == 8) { // Backspace
//                if (!m_playerName.empty()) {
//                    m_playerName.pop_back();
//                }
//            }
//            else if (event.text.unicode == 13) { // Enter
//                submitHighScore();
//                m_nameInputActive = false;
//            }
//            else if (event.text.unicode >= 32 && event.text.unicode < 128) {
//                if (m_playerName.length() < 15) { // Limiter la longueur du nom
//                    m_playerName += static_cast<char>(event.text.unicode);
//                }
//            }
//
//            m_nameInputText.setString(m_playerName + "_");
//            sf::FloatRect inputBounds = m_nameInputText.getLocalBounds();
//            m_nameInputText.setOrigin(inputBounds.left + inputBounds.width / 2.0f,
//                inputBounds.top + inputBounds.height / 2.0f);
//        }
//    }
//    else {
//        sf::Vector2i mousePos = sf::Mouse::getPosition(*getWindow());
//        sf::Vector2f mousePosF(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));
//
//        for (auto button : m_buttons) {
//            button->handleEvent(event, mousePosF);
//        }
//    }
//}
//
//void GameOverScene::Render() {
//    sf::RenderWindow* window = getWindow();
//
//    window->draw(m_backgroundShape);
//    window->draw(m_titleText);
//    window->draw(m_scoreText);
//
//    if (m_nameInputActive) {
//        window->draw(m_namePromptText);
//        window->draw(m_nameInputText);
//    }
//    else {
//        for (auto button : m_buttons) {
//            button->draw(*window);
//        }
//    }
//}
//
//void GameOverScene::setupTitle() {
//    m_titleText.setFont(m_font);
//    m_titleText.setString("GAME OVER");
//    m_titleText.setCharacterSize(72);
//    m_titleText.setFillColor(sf::Color::White);
//    m_titleText.setStyle(sf::Text::Bold);
//
//    sf::FloatRect titleBounds = m_titleText.getLocalBounds();
//    m_titleText.setOrigin(titleBounds.left + titleBounds.width / 2.0f,
//        titleBounds.top + titleBounds.height / 2.0f);
//    m_titleText.setPosition(getWindow()->getSize().x / 2.0f, getWindow()->getSize().y / 6.0f);
//}
//
//void GameOverScene::setupButtons() {
//    sf::Vector2f buttonSize(300, 60);
//    float startY = getWindow()->getSize().y * 0.6f;
//    float spacing = 80.0f;
//
//    // Bouton Rejouer
//    Button* playAgainButton = new Button(
//        sf::Vector2f((getWindow()->getSize().x - buttonSize.x) / 2.0f, startY),
//        buttonSize,
//        "REJOUER",
//        m_font,
//        32
//    );
//    playAgainButton->setCallback([this]() {
//        // Réinitialiser la partie
//        Game* gameScene = dynamic_cast<Game*>(m_sceneManager->getScene(1));
//        if (gameScene) {
//            gameScene->ResetGame();
//            getWindow()->setMouseCursorVisible(false);
//        }
//        m_sceneManager->SetScene(1); // Aller à la scène de jeu
//        });
//    m_buttons.push_back(playAgainButton);
//
//    // Bouton Menu Principal
//    Button* mainMenuButton = new Button(
//        sf::Vector2f((getWindow()->getSize().x - buttonSize.x) / 2.0f, startY + spacing),
//        buttonSize,
//        "MENU PRINCIPAL",
//        m_font,
//        32
//    );
//    mainMenuButton->setCallback([this]() {
//        getWindow()->setMouseCursorVisible(true);
//        m_sceneManager->SetScene(0); // Retour au menu principal
//        });
//    m_buttons.push_back(mainMenuButton);
//}
//
//void GameOverScene::updateScoreDisplay() {
//    Game* gameScene = dynamic_cast<Game*>(m_sceneManager->getScene(1));
//    if (gameScene) {
//        std::stringstream ss;
//        ss << "Score final: " << gameScene->GetScore();
//        m_scoreText.setString(ss.str());
//
//        sf::FloatRect scoreBounds = m_scoreText.getLocalBounds();
//        m_scoreText.setOrigin(scoreBounds.left + scoreBounds.width / 2.0f,
//            scoreBounds.top + scoreBounds.height / 2.0f);
//
//        checkHighScore();
//    }
//}
//
//void GameOverScene::checkHighScore() {
//    Game* gameScene = dynamic_cast<Game*>(m_sceneManager->getScene(1));
//    if (gameScene) {
//        ScoreManager* scoreManager = gameScene->GetScoreManager();
//        int currentScore = scoreManager->getCurrentScore();
//
//        if (scoreManager->isHighScore(currentScore)) {
//            m_nameInputActive = true;
//        }
//        else {
//            m_nameInputActive = false;
//        }
//    }
//}
//
//void GameOverScene::submitHighScore() {
//    if (m_playerName.empty()) {
//        m_playerName = "Player";
//    }
//
//    Game* gameScene = dynamic_cast<Game*>(m_sceneManager->getScene(1));
//    if (gameScene) {
//        ScoreManager* scoreManager = gameScene->GetScoreManager();
//        scoreManager->addHighScore(m_playerName);
//    }
//}