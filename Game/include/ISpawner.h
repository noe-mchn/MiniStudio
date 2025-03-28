#pragma once
#include <iostream>
#include "IGameObject.h"
#include "RandomNumber.h"
#include "SceneBase.h"


class ISpawner : public IComposite
{
public:
	ISpawner(IComposite* scene, const size_t& maxEntity);
	void ProcessInput(const sf::Event& event) override {}
	void Render() override;

	void Update(const float& deltatime);
	virtual void Spawn() = 0;


protected:
	IComposite* m_Scene;
	size_t m_maxEntity;
	Vec2 m_SpawnZone;
	Vec2 m_RestrictedArea;

};

//class BossSpawner : public ISpawner
//{
//public:
//	BossSpawner(IComposite* scene, const size_t& maxEntity = 1);
//	void Spawn() override;
//
//	void SetSpawnPosition(const sf::Vector2f& position) { m_fixedSpawnPosition = position; m_useFixedPosition = true; }
//
//private:
//	bool m_useFixedPosition;
//	sf::Vector2f m_fixedSpawnPosition;
//	float m_bossHealth;
//};