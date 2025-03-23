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

	void ResetGame();
	void SetPaused(bool paused);
	void ButtonIsClicked();

private:
	sf::RectangleShape m_Ship;
	Cursor cursor;
	Colision collision;
	AsteroidSpawner* m_spawner;
	BossSpawner* m_bossSpawner;
	Timer m_bossSpawnTimer;

	Ship* m_ship;
	bool m_paused = false;
};