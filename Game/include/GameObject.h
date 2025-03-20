#pragma once
#include "IGameObject.h"
#include "VectorND.h"
#include "SceneBase.h"
#include "IShape.h"
#include <array>

class Ship;
enum trust
{
	Right = 0
	, Left = 1
	, Up = 2
	, Down = 3
	, Default = 4
};

class Cursor : public NonDestructibleObject, public ILeaf
{
public:
	Cursor(IComposite* scene);
	void ProssesInput(const sf::Event& event) override;

	void Update(const float& deltatime);

	void Render() override;

private:
	AnimateSprite m_animate;
};

class MovementInSpace : public IPhysics
{
public:
	MovementInSpace(const float& maxVelority, const float& acceleratrion, const float& decceleration);
	void ExecutePhysics(KT::VectorND<bool, 4>& isStrafing, float framerate);

	sf::Vector2f calculPosition(IShapeSFML* entity, ISceneBase* scene, float framerate);
	float getMaxVelocity()
	{
		return m_maxVelocity;
	}
private:
	float m_maxVelocity;
	float m_acceleration;
	float m_decceleration;
	std::array<float, 4>velocity;
};

class IBorder : public NonDestructibleObject, public ILeaf
{
public:

	IBorder(IComposite* scene, IShapeSFML* object);
	void ProssesInput(const sf::Event& event) override = 0;
	void Update(const float& deltatime);
	void Render() override = 0;
protected:
	IShapeSFML* m_ObjectToProtect;
};

class BorderShip : public IBorder
{
public:
	BorderShip(IComposite* scene, IShapeSFML* game_object, Ship* ship);
	void ProssesInput(const sf::Event& event) override {}
	void Update(const float& deltatime);
	void Render() override;

private:
	sf::Vector2f VerifyLimit();
	Ship* m_ship;
	AnimateSprite m_sprite;
	Timer m_elapsedTime;
	bool IsInBorder;
};

enum class Position
{
	Left
	, Right
	, Up
	, Down
};

class ExternBorder : public IBorder
{
public:
	ExternBorder(IComposite* scene, IShapeSFML* game_object, Position pos, float BorderSize);
	void ProssesInput(const sf::Event& event) override {}
	void Update(const float& deltatime);
	void Render() override;
protected:
	sf::Vector2f m_diffposition;
	float m_BorderSize;
};

class WorldBorder : public ExternBorder
{
public:
	WorldBorder(IComposite* scene, IShapeSFML* game_object, Position pos, float BorderSize, float Securitydistance);
	void HandleCollision(IGameObject* object) override;
};

class GameBorder : public ExternBorder
{
public:
	GameBorder(IComposite* scene, IShapeSFML* game_object, Position pos, float BorderSize);
	void HandleCollision(IGameObject* object) override;
};

class ITurret : public NonDestructibleObject, public IComposite
{
public:
	ITurret(IComposite* scene, IShapeSFML* game_object, sf::Vector2f& positiondiff);
	virtual ~ITurret() = default;
	void ProssesInput(const sf::Event& event) override = 0;
	void setBullet(float Size, float Speed, float hp);
	void Update(const float& deltatime);
	void Render() override = 0;
	virtual void Fire() = 0;
	void SetFireRate(const float& fireRate);
	void SetOverloadGun(const float& overloadcoodown, float MaxShot);
	IShapeSFML* getGameObject() const { return m_gameObject; }

protected:
	sf::Vector2f m_positionDiff;
	IShapeSFML* m_gameObject;
	Timer m_fireRate;
	Timer m_coolDown;
	Counter m_masShot;

	float m_bulletSpeed;
	float m_bulletLife;
	float m_bulletSize;
};

class PlayerSprite : public AnimateSprite
{
public:
	PlayerSprite();
};

class FixTurret : public  ITurret
{
public:
	FixTurret(IComposite* scene, IShapeSFML* game_object, sf::Vector2f& positiondiff, float angle);
	void ProssesInput(const sf::Event& event) override {}
	void Update(const float& deltatime);
	void Render() override;
	void Fire() override;
private:
	float m_angleDiff;
	SquareSFML BaseShape;
};

class AutoTurret : public ITurret
{
public:
	AutoTurret(IComposite* scene, IShapeSFML* game_object, IShapeSFML* Target, sf::Vector2f& positiondiff);
	void ProssesInput(const sf::Event& event) override {}
	void Update(const float& deltatime);
	void Render() override;
	void Fire() override;
private:
	SquareSFML BaseShape;
	IShapeSFML* m_Target;

};

class IBullet : public DestructibleObject, public ILeaf
{
public:
	IBullet(AnimateSprite animate, IComposite* scene, ITurret* gun, float angle, float speed, float size, float hp);
	void Render() override = 0;
	void ProssesInput(const sf::Event& event) = 0;
	void Update(const float& deltatime);

	ITurret* getTurret() const { return m_gun; }

protected:
	ITurret* m_gun;
	sf::Vector2f m_gunPosition;
	float m_gunangle;
	float m_speed;
	float m_size;
	AnimateSprite m_animate;
};

class ClassicBullet : public IBullet
{
public:
	ClassicBullet(AnimateSprite animate, IComposite* scene, ITurret* gun, float angle, float speed, float size, float hp);
	void Render() override;
	void ProssesInput(const sf::Event& event) {};
	void Update(const float& deltatime);
	void HandleCollision(IGameObject* object) override;

private:
	Timer m_elapsedTime;
};

enum class Color
{
	Blue
	, Pink
	, Orange
};

class Life : public NonDestructibleObject, public ILeaf
{
public:
	Life(IComposite* scene, DestructibleObject* game_object, Color color);
	~Life();
private:
	void Render() override;
	void ProssesInput(const sf::Event& event) {};
	void Update(const float& deltatime);
protected:
	DestructibleObject* m_object;
	IShapeSFML* m_backgroundShape;
	AnimateSprite m_animate;
	AnimateSprite m_animateBackground;
	float m_sizeDiff;
};

class Asteroid : public  DestructibleObject, public  IComposite
{
public:
	Asteroid(IComposite* scene, const sf::Vector2f& Spawnposition, const sf::Vector2f& Size, const float& angle, const float& speed, const float& life);
	void Render() override;
	void ProssesInput(const sf::Event& event) {};
	void Update(const float& deltatime);
	void HandleCollision(IGameObject* object) override;
	void ChangeLife(const float& life)
	{
		if (!m_invisibility.ActionIsReady())
			return;

		m_life += life;
		if (m_life <= 0)
			destroy();
		m_invisibility.resetTimer();
	}
private:
	Timer m_elapsedTime;
	AnimateSprite m_animate;
	sf::Vector2f m_psotition;
	float m_angle;
	float m_speed;
	float m_rotation;
	Timer m_invisibility;
};

class Comete : public  DestructibleObject, public  IComposite
{
public:
	Comete(IComposite* scene, const sf::Vector2f& Spawnposition, const sf::Vector2f& Size, const float& angle, const float& speed, const float& life);
	void Render() override;
	void ProssesInput(const sf::Event& event) {};
	void Update(const float& deltatime);
	void HandleCollision(IGameObject* object) override;
	void ChangeLife(const float& life)
	{
		if (!m_invisibility.ActionIsReady())
			return;

		m_life += life;
		if (m_life <= 0)
			destroy();
		m_invisibility.resetTimer();
	}
private:
	Timer m_elapsedTime;
	AnimateSprite m_animate;
	sf::Vector2f m_position;
	float m_angle;
	float m_speed;
	float m_rotation;
	Timer m_invisibility;
};