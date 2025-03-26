//#pragma once
//#include "Collision.h"
//#include "SceneBase.h"
//#include "GameObject.h"
//#include "SceneManager.h"
//#include "ScoreManager.h"
//#include "ISpawner.h"
//
//bool Collision(AABB a, AABB b);
//
//class Game : public ISceneBase
//{
//public:
//	Game(sf::RenderWindow* window, const float& framerate, TextureCache* texture, SceneManager* manager);
//	void Init();
//	virtual ~Game()override = default;
//	virtual void Update(const float& deltatime) override;
//	virtual void ProcessInput(const sf::Event& event)override;
//	virtual void Render() override;
//	void ResetGame();
//	void SetPaused(bool paused);
//
//	void SetSceneManager(SceneManager* manager);
//
//private:
//	void Game::addChild(IGameObject* obj)
//	{
//		if (obj == &cursor)
//		{
//			std::cout << "tentative de delete Cursor interdite." << std::endl;
//			return;
//		}
//
//		m_children.push_back(obj);
//	}
//
//private:
//	sf::RectangleShape m_Ship;
//	Cursor cursor;
//	Colision collision;
//	Timer m_bossSpawnTimer;
//
//	ScoreManager* m_score;
//	SceneManager* m_sceneManager;
//	Hero* m_hero;
//	bool m_paused = false;
//	bool m_gameovered = false;
//
//	std::vector<IGameObject*> m_children;
//
//};

#pragma once
#include "Collision.h"
#include "SceneBase.h"
#include "GameObject.h"
#include "ISpawner.h"

bool Collision(AABB a, AABB b);

class Game : public ISceneBase
{
public:
	Game(sf::RenderWindow* window, const float& framerate, TextureCache* texture);
	virtual ~Game()override = default;
	virtual void Update(const float& deltatime) override;
	virtual void ProcessInput(const sf::Event& event)override;
	virtual void Render() override;

private:
	sf::RectangleShape m_Ship;
	Cursor cursor;
	Colision collision;
	//AsteroidSpawner* m_spawner;
	//BossSpawner* m_bossSpawner;
	Timer m_bossSpawnTimer;

	Hero* m_hero;
};