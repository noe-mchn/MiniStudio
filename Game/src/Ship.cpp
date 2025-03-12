#include "Ship.h"
#include "IGameObject.h"
#include <iostream>
#include <exception>

// state Machine du player pour passer en mode hidle ou attack à main nue ou pistoler attack...
Ship::IState* Ship::IdleState::handle(const State& state)
{
	/*if (state == State::HAND_ATTACK)
	{
		return new HandAttackState();
	}*/
	if (state == State::PISTOL_ATTACK)
	{
		return new PistolAttackState();
	}
	return nullptr;
}

void Ship::IdleState::update(Ship* ship, float deltaTime)
{
	sf::Vector2f shipPos = ship->getShape()->getPosition();
	float distance = std::sqrt(shipPos.x * shipPos.x + shipPos.y * shipPos.y);

	std::cout << distance << std::endl;

	if (distance <= 100.0f)
	{
		ship->changeState(State::PISTOL_ATTACK);
		return;
	}
}

//Ship::IState* Ship::HandAttackState::handle(const State& state)
//{
//	if (state == State::IDLE)
//	{
//		return new IdleState();
//	}
//	if (state == State::PISTOL_ATTACK)
//	{
//		return new PistolAttackState();
//	}
//	return nullptr;
//}
//
//void Ship::HandAttackState::update(Ship* ship, float deltaTime)
//{
//	sf::Vector2f direction = ship->targetPosition - ship->getShape()->getPosition();
//	float distance = std::sqrt(direction.x * direction.x + direction.y * direction.y);
//
//	if (distance > 700)
//	{
//		ship->changeState(State::IDLE);
//		return;
//	}
//
//	if (distance > 0)
//	{
//
//	}
//
//	if (distance < 200.0f)
//	{
//		ship->changeState(State::PISTOL_ATTACK);
//	}
//}



Ship::IState* Ship::PistolAttackState::handle(const State& state)
{
	/*if (state == State::HAND_ATTACK)
	{
		return new HandAttackState();
	}*/

	if (state == State::IDLE)
	{
		return new IdleState();
	}

	return nullptr;
}

void Ship::PistolAttackState::update(Ship* ship, float deltaTime)
{
	if (!ship->m_turret)
		throw std::runtime_error("ship est nullptr!");
	ship->m_turret->Fire();


	/*if (distance > 200.0f)
	{
		ship->changeState(State::HAND_ATTACK);
		return;
	}*/

}

// fin de la state machine 

Ship::Ship(IComposite* scene, IShapeSFML* background) :
	DestructibleObject(scene, 10)
	, IComposite(scene)
	, m_background(background)
	, m_angle(0)
	, m_elapsedTime(0.2)
	, m_animate({ "SpaceHero.png", "SpaceHero2.png" })
	, m_physics(new MovementInSpace(1000, 400, 200))
	, m_invisibility(2.5)

{
	m_shape = new SquareSFML(150, scene->getRoot()->getScene());
	m_shape->setTexture(m_scene->getRoot()->getScene()->getTexture()->getTexture(m_animate.getCurrentPath()));
	new Life(this, this, Color::Blue);
	m_turret = new FixTurret(this, m_shape, sf::Vector2f(35, -25), 0.75);
	m_turret->SetFireRate(0.2f);
	m_turret->SetOverloadGun(5, 30);
	m_turret->setBullet(0, 0, 0);

	currentState = new IdleState();
}

Ship::~Ship()
{
	delete m_physics;
}

void Ship::ProssesInput(const sf::Event& event)
{
	m_strafe = { false,false,false,false };
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
	{
		m_strafe[trust::Right] = true;

	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q))
	{
		m_strafe[trust::Left] = true;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z))
	{
		m_strafe[trust::Up] = true;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
	{
		m_strafe[trust::Down] = true;
	}

	physics();
}

void Ship::physics()
{
	m_angle = anglecalcul();
}

void Ship::Update(const float& deltatime)
{
	if (!currentState)
		throw std::runtime_error("current state est nullptr!");

	currentState->update(this, deltatime);

	static_cast<MovementInSpace*>(m_physics)->ExecutePhysics(m_strafe, m_scene->getRoot()->getScene()->getRefreshTime().asSeconds());
	m_shape->setRotation(m_angle);
	m_background->setPosition(static_cast<MovementInSpace*>(m_physics)->calculPosition(m_background, m_scene->getRoot()->getScene(), m_scene->getRoot()->getScene()->getRefreshTime().asSeconds()));

	if (m_elapsedTime.AutoActionIsReady(m_scene->getRoot()->getScene()->getRefreshTime().asSeconds())) 
	{
		m_animate.ChangeToNextPath();
		m_shape->setTexture(m_scene->getRoot()->getScene()->getTexture()->getTexture(m_animate.getCurrentPath()));
	}

	IComposite::Update(deltatime);
	m_invisibility.NextTIck(m_scene->getRoot()->getScene()->getRefreshTime().asSeconds());


}

void Ship::Render()
{
	m_scene->getRoot()->getScene()->getWindow()->draw(static_cast<SquareSFML*>(m_shape)->getShape());
	IComposite::Render();
}

float Ship::anglecalcul()
{
	sf::Vector2i mousePos = sf::Mouse::getPosition(*m_scene->getRoot()->getScene()->getWindow());
	sf::Vector2f shipPos = m_shape->getPosition();
	float deltaX = mousePos.x - shipPos.x;
	float deltaY = mousePos.y - shipPos.y;
	float angle = std::atan2(deltaY, deltaX) * 180 / 3.14159f;
	return angle;
}

void Ship::HandleCollision(IGameObject* object)
{
	if (object->globalGameObjectType() != GameObjectType::DestructibleObject)
		return;

	ChangeLife(-1);
}

void Ship::ChangeLife(const float& life)
{
	if (!m_invisibility.ActionIsReady())
		return;

	m_life += life;
	if (m_life <= 0)
		destroy();
	m_invisibility.resetTimer();
}

void Ship::changeState(const State& newState)
{
	if (currentState)
	{
		IState* state = currentState->handle(newState);
		if (state)
		{
			delete currentState;
			currentState = state;
		}
	}
}