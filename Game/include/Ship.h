#pragma once
#include "IGameObject.h"
#include "GameObject.h"
#include "Boss1.h"
#include "Boss2.h"
#include "Boss3.h"

class Boss1;

class Ship : public DestructibleObject, public IComposite
{
protected:
	enum State
	{
		 IDLE
		,MOVE
		,HAND_ATTACK
		,PISTOL_ATTACK
		,RELOAD
	};

	struct IState
	{
		virtual ~IState() = default;
		virtual IState* handle(const State& state) = 0;
		virtual void update(Ship* ship, float deltaTime) = 0;
	};

	struct IdleState : IState
	{
		~IdleState() override = default;
		IState* handle(const State& state) override;
		void update(Ship* ship, float deltaTime) override;
	};

	struct MoveState : IState
	{
		~MoveState() override = default;
		IState* handle(const State& state) override;
		void update(Ship* ship, float deltaTime) override;
	};

	struct HandAttackState : IState
	{
		~HandAttackState() override = default;
		IState* handle(const State& state) override;
		void update(Ship* ship, float deltaTime) override;

		IGameObject* m_boss2;
	};

	struct PistolAttackState : IState
	{
		~PistolAttackState() override = default;
		IState* handle(const State& state) override;
		void update(Ship* ship, float deltaTime) override;

	};

	struct ReloadState : IState
	{
		~ReloadState() override = default;
		IState* handle(const State& state) override;
		void update(Ship* ship, float deltaTime) override;

	private:
		float reloadTime = 100.0f;
		float reloadTimer = 0.0f;
	};

	IState* m_currentState;

public:
	friend BorderShip;
	Ship(IComposite* scene, IShapeSFML* background);
	~Ship();

	void ProssesInput(const sf::Event& event) override;

	void physics();

	void Update(const float& deltatime) override;
	void Render() override;

	float anglecalcul();
	void HandleCollision(IGameObject* object) override;
	void ChangeLife(const float& life) override;

	void ChangeState(const State& newState);
	float DistancedetectBoss1(Ship* ship, Boss1* boss);
	float DistancedetectBoss2(Ship* ship, Boss2* boss);

private:
	IShapeSFML* m_background;
	IShapeSFML* m_target;

	float m_angle;
	Timer m_elapsedTime;
	Timer m_invisibility;

	AnimateSprite m_animate;
	IPhysics* m_physics;
	ITurret* m_turret;
	KT::VectorND<bool, 4> m_strafe{ false,false,false,false };

	std::vector<Boss1*> m_enemiesInGame;
	float m_detectionRadius;

};