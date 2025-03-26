#include "GameObject.h"
#include "Hero.h"
#include "cmath"
#include <iostream>
#include "RandomNumber.h"

IBorder::IBorder(IComposite* scene, IShapeSFML* object) :
	NonDestructibleObject(scene), ILeaf(scene), m_ObjectToProtect(object)
{
}

void IBorder::Update(const float& deltatime)
{
}

BorderShip::BorderShip(IComposite* scene, IShapeSFML* game_object, Hero* ship) :
	IBorder(scene, game_object)
	, m_ship(ship)
	, m_sprite({ "FenceTmp.png","FenceTmp2.png" })
	, IsInBorder(false)
	, m_elapsedTime(0.2)
{
	m_shape = new CircleSFML(m_ObjectToProtect->getSize().x / 2, m_ObjectToProtect->getCenter());
	m_shape->setTexture(m_scene->getRoot()->getScene()->getTexture()->getTexture(m_sprite.getCurrentPath()));
}

void BorderShip::Update(const float& deltatime)
{
	m_shape->setPosition(m_ObjectToProtect->getPosition());
	m_ship->m_background->setPosition(VerifyLimit());
	if (m_elapsedTime.AutoActionIsReady(m_scene->getRoot()->getScene()->getRefreshTime().asSeconds()))
	{
		m_sprite.ChangeToNextPath();
		m_shape->setTexture(m_scene->getRoot()->getScene()->getTexture()->getTexture(m_sprite.getCurrentPath()));
	}
}

void BorderShip::Render()
{
	m_scene->getRoot()->getScene()->getWindow()->draw(static_cast<CircleSFML*>(m_shape)->getShape());
}

sf::Vector2f BorderShip::VerifyLimit()
{

	sf::Vector2f centreBackground = m_ObjectToProtect->getPosition();
	sf::Vector2f positionVaisseau = m_ship->m_shape->getPosition();

	float dx = positionVaisseau.x - centreBackground.x;
	float dy = positionVaisseau.y - centreBackground.y;
	float distance = dx * dx + dy * dy;
	float Border = 2 * (m_shape->getSize().x / 30);
	float radius = (m_shape->getSize().x / 2 - m_ship->m_shape->getSize().x - Border) * (m_shape->getSize().x / 2 - m_ship->m_shape->getSize().x - Border);
	if (distance > radius) {
		float factor = radius / distance;
		centreBackground.x = positionVaisseau.x - dx * factor;
		centreBackground.y = positionVaisseau.y - dy * factor;
	}
	return centreBackground;
}

ExternBorder::ExternBorder(IComposite* scene, IShapeSFML* game_object, Position pos, float BorderSize) :IBorder(scene, game_object), m_diffposition(0, 0), m_BorderSize(BorderSize)
{
	m_shape = new RectangleSFML(m_ObjectToProtect->getSize().x, m_ObjectToProtect->getSize().y, game_object->getCenter());
	switch (pos)
	{
	case Position::Up:
	{
		m_shape->setSize(sf::Vector2f(m_ObjectToProtect->getSize().x, m_BorderSize));
		m_diffposition.x = 0;
		m_diffposition.y = -(m_ObjectToProtect->getSize().y / 2 - m_BorderSize / 2);
		break;
	}
	case Position::Down:
	{
		m_shape->setSize(sf::Vector2f(m_ObjectToProtect->getSize().x, m_BorderSize));
		m_diffposition.x = 0;
		m_diffposition.y = m_ObjectToProtect->getSize().y / 2 + m_BorderSize / 2;
		break;
	}
	case Position::Left:
	{
		m_shape->setSize(sf::Vector2f(m_BorderSize, m_ObjectToProtect->getSize().x));
		m_diffposition.x = -(m_ObjectToProtect->getSize().y / 2 - m_BorderSize / 2);
		m_diffposition.y = 0;
		break;
	}
	case Position::Right:
	{
		m_shape->setSize(sf::Vector2f(m_BorderSize, m_ObjectToProtect->getSize().x));
		m_diffposition.x = +(m_ObjectToProtect->getSize().y / 2 - m_BorderSize / 2);
		m_diffposition.y = 0;
		break;
	}

	}
	m_shape->setCenter(sf::Vector2f(m_shape->getSize().x / 2, m_shape->getSize().y / 2));
}

void ExternBorder::Update(const float& deltatime)
{
	m_shape->setPosition(sf::Vector2f(m_ObjectToProtect->getPosition().x + m_diffposition.x, m_ObjectToProtect->getPosition().y + m_diffposition.y));
}

void ExternBorder::Render()
{
	m_scene->getRoot()->getScene()->getWindow()->draw(static_cast<RectangleSFML*>(m_shape)->getShape());
}

WorldBorder::WorldBorder(IComposite* scene, IShapeSFML* game_object, Position pos, float BorderSize, float Securitydistance) : ExternBorder(scene, game_object, pos, BorderSize)
{

	m_shape = new RectangleSFML(m_ObjectToProtect->getSize().x, m_ObjectToProtect->getSize().y, game_object->getCenter());
	switch (pos)
	{
	case Position::Up:
	{
		m_shape->setSize(sf::Vector2f(m_ObjectToProtect->getSize().x + Securitydistance * 2, m_BorderSize));
		m_diffposition.x = 0;
		m_diffposition.y = -(m_ObjectToProtect->getSize().y / 2 - m_BorderSize / 2 + Securitydistance);
		break;
	}
	case Position::Down:
	{
		m_shape->setSize(sf::Vector2f(m_ObjectToProtect->getSize().x + Securitydistance * 2, m_BorderSize));
		m_diffposition.x = 0;
		m_diffposition.y = m_ObjectToProtect->getSize().y / 2 + m_BorderSize / 2 + Securitydistance;
		break;
	}
	case Position::Left:
	{
		m_shape->setSize(sf::Vector2f(m_BorderSize, m_ObjectToProtect->getSize().x + Securitydistance * 2));
		m_diffposition.x = -(m_ObjectToProtect->getSize().y / 2 - m_BorderSize / 2 + Securitydistance);
		m_diffposition.y = 0;
		break;
	}
	case Position::Right:
	{
		m_shape->setSize(sf::Vector2f(m_BorderSize, m_ObjectToProtect->getSize().x + Securitydistance * 2));
		m_diffposition.x = +(m_ObjectToProtect->getSize().y / 2 - m_BorderSize / 2 + Securitydistance);
		m_diffposition.y = 0;
		break;
	}

	}
	m_shape->setCenter(sf::Vector2f(m_shape->getSize().x / 2, m_shape->getSize().y / 2));
}

void WorldBorder::HandleCollision(IGameObject* object)
{
	if (getObj<WorldBorder*>(object))
		return;
	if (getObj<Cursor*>(object))
		return;
	object->destroy();

}

GameBorder::GameBorder(IComposite* scene, IShapeSFML* game_object, Position pos, float BorderSize) :ExternBorder(scene, game_object, pos, BorderSize)
{
}

void GameBorder::HandleCollision(IGameObject* object)
{
	if (object->globalGameObjectType() != GameObjectType::DestructibleObject)
		return;

	object->destroy();

}

ITurret::ITurret(IComposite* scene, IShapeSFML* game_object, sf::Vector2f& positiondiff) :
	NonDestructibleObject(scene)
	, IComposite(scene)
	, m_positionDiff(positiondiff)
	, m_gameObject(game_object)
	, m_fireRate(0.5)
	, m_coolDown(0)
	, m_masShot(100, 0)
	, m_bulletSpeed(2000)
	, m_bulletLife(1)
	, m_bulletSize(10)
{
}

void ITurret::Update(const float& deltatime)
{
	IComposite::Update(deltatime);
}

void ITurret::setBullet(float Size, float Speed, float hp)
{
	if (Size != 0)
		m_bulletSize = Size;
	if (Speed != 0)
		m_bulletSpeed = Speed;
	if (hp != 0)
		m_bulletLife = hp;
}

void ITurret::SetFireRate(const float& fireRate)
{
	m_fireRate.setNewTimer(fireRate);

}

void ITurret::SetOverloadGun(const float& overloadcoodown, float MaxShot)
{
	m_coolDown.setNewTimer(overloadcoodown);
	m_masShot.setNewCounter(MaxShot, 0);
}


PlayerSprite::PlayerSprite() : AnimateSprite({ "PlayerBullet.png","PlayerBullet2.png" })
{
}

FixTurret::FixTurret(IComposite* scene, IShapeSFML* game_object, sf::Vector2f& positiondiff, float angle) : ITurret(scene, game_object, positiondiff), m_angleDiff(angle), BaseShape(10, m_gameObject->getPosition())
{
	m_shape = new SquareSFML(10, m_gameObject->getPosition());
}

void FixTurret::Update(const float& deltatime)
{
	sf::Vector2f gamePos = m_gameObject->getPosition();

	float angleRad = convertDegToRad(m_gameObject->getangle());

	sf::Vector2f rotatedOffset(
		m_positionDiff.x * cos(angleRad) - m_positionDiff.y * sin(angleRad),
		m_positionDiff.x * sin(angleRad) + m_positionDiff.y * cos(angleRad)
	);

	BaseShape.setPosition(gamePos + rotatedOffset);
	BaseShape.setRotation(m_gameObject->getangle());

	m_shape->setPosition(BaseShape.getPosition());
	m_shape->setRotation(m_gameObject->getangle() + m_angleDiff);
	m_fireRate.NextTIck(m_scene->getRoot()->getScene()->getRefreshTime().asSeconds());

	m_coolDown.NextTIck(m_scene->getRoot()->getScene()->getRefreshTime().asSeconds());
	if (m_coolDown.ActionIsReady() && m_masShot.GetCurrentCounter() > 0 && !m_masShot.CounterMax())
	{
		m_masShot.PreviousTick();
	}
	IComposite::Update(deltatime);
}

void FixTurret::Render()
{

	m_scene->getRoot()->getScene()->getWindow()->draw(static_cast<SquareSFML*>(m_shape)->getShape());
	IComposite::Render();
}

void FixTurret::Fire()
{
	if (m_masShot.CounterMax() && m_fireRate.ActionIsReady())
	{
		if (m_coolDown.ActionIsReady())
		{
			m_masShot.resetCounter();
			m_coolDown.resetTimer();
		}


	}
	else
		m_coolDown.resetTimer();
	if (m_fireRate.ActionIsReady() && !m_masShot.CounterMax())
	{
		new ClassicBullet(PlayerSprite{}, this, this, m_shape->getangle(), m_bulletSpeed, m_bulletSize, m_bulletLife);
		m_masShot.NextTIck();
		m_fireRate.resetTimer();
	}
}


AutoTurret::AutoTurret(IComposite* scene, IShapeSFML* game_object, IShapeSFML* target, sf::Vector2f& positiondiff) : ITurret(scene, game_object, positiondiff), BaseShape(10, m_gameObject->getPosition()), m_Target(target)
{
	m_shape = new SquareSFML(10, m_gameObject->getPosition());
}

void AutoTurret::Update(const float& deltatime)
{
	sf::Vector2f gamePos = m_gameObject->getPosition();

	float angleRad = convertDegToRad(m_gameObject->getangle());

	sf::Vector2f rotatedOffset(
		m_positionDiff.x * cos(angleRad) - m_positionDiff.y * sin(angleRad),
		m_positionDiff.x * sin(angleRad) + m_positionDiff.y * cos(angleRad)
	);

	BaseShape.setPosition(gamePos + rotatedOffset);
	BaseShape.setRotation(m_gameObject->getangle());

	m_shape->setPosition(BaseShape.getPosition());

	float angletoRad2 = convertRadToDeg(std::atan2(m_Target->getPosition().y - m_shape->getPosition().y, m_Target->getPosition().x - m_shape->getPosition().x));
	m_shape->setRotation(angletoRad2);
	m_fireRate.NextTIck(m_scene->getRoot()->getScene()->getRefreshTime().asSeconds());

	m_coolDown.NextTIck(m_scene->getRoot()->getScene()->getRefreshTime().asSeconds());
	if (m_coolDown.ActionIsReady() && m_masShot.GetCurrentCounter() > 0 && !m_masShot.CounterMax())
	{
		m_masShot.PreviousTick();
	}
	IComposite::Update(deltatime);
}

void AutoTurret::Render()
{
	m_scene->getRoot()->getScene()->getWindow()->draw(static_cast<SquareSFML*>(m_shape)->getShape());
	IComposite::Render();
}

void AutoTurret::Fire()
{
	if (m_masShot.CounterMax() && m_fireRate.ActionIsReady())
	{
		if (m_coolDown.ActionIsReady())
		{
			m_masShot.resetCounter();
			m_coolDown.resetTimer();
		}
	}
	else
		m_coolDown.resetTimer();
	if (m_fireRate.ActionIsReady() && !m_masShot.CounterMax())
	{
		new ClassicBullet(PlayerSprite{}, this, this, m_shape->getangle(), m_bulletSpeed, m_bulletSize, m_bulletLife);
		m_masShot.NextTIck();
		m_fireRate.resetTimer();
	}
}


IBullet::IBullet(AnimateSprite animate, IComposite* scene, ITurret* gun, float angle, float speed, float size, float hp) : DestructibleObject(scene, hp), ILeaf(scene), m_gun(gun), m_gunPosition(0, 0), m_gunangle(angle), m_speed(speed), m_size(size), m_animate(animate)
{
	m_gunPosition = m_gun->getShape()->getPosition();
}

void IBullet::Update(const float& deltatime)
{
	float angleRad = convertDegToRad(m_gunangle);
	sf::Vector2f moov(std::cos(angleRad) * m_speed * m_scene->getRoot()->getScene()->getRefreshTime().asSeconds(),
		std::sin(angleRad) * m_speed * m_scene->getRoot()->getScene()->getRefreshTime().asSeconds());

	m_gunPosition += moov;
	m_shape->setPosition(m_gunPosition);
}

ClassicBullet::ClassicBullet(AnimateSprite animate, IComposite* scene, ITurret* gun, float angle, float speed, float size, float hp) : IBullet(animate, scene, gun, angle, speed, size, hp), m_elapsedTime(0.2)
{
	m_shape = new SquareSFML(size, m_gunPosition);
	m_shape->setTexture(m_scene->getRoot()->getScene()->getTexture()->getTexture(m_animate.getCurrentPath()));
	m_shape->setRotation(m_gunangle);
}

void ClassicBullet::Render()
{
	m_scene->getRoot()->getScene()->getWindow()->draw(static_cast<SquareSFML*>(m_shape)->getShape());
}

void ClassicBullet::Update(const float& deltatime)
{
	float angleRad = convertDegToRad(m_gunangle);
	sf::Vector2f moov(std::cos(angleRad) * m_speed * m_scene->getRoot()->getScene()->getRefreshTime().asSeconds(), std::sin(angleRad) * m_speed * m_scene->getRoot()->getScene()->getRefreshTime().asSeconds());
	m_gunPosition += moov;
	m_shape->setPosition(m_gunPosition);

	if (m_elapsedTime.AutoActionIsReady(m_scene->getRoot()->getScene()->getRefreshTime().asSeconds())) {
		m_animate.ChangeToNextPath();
		m_shape->setTexture(m_scene->getRoot()->getScene()->getTexture()->getTexture(m_animate.getCurrentPath()));
	}
}

void ClassicBullet::HandleCollision(IGameObject* object)
{
	if (object->globalGameObjectType() != GameObjectType::DestructibleObject)
		return;

	ChangeLife(-1);
}

Life::Life(IComposite* scene, DestructibleObject* game_object, Color color) :NonDestructibleObject(scene), ILeaf(scene), m_object(game_object), m_animate({ "" }), m_animateBackground({ "BlackLife.png" })
{
	m_backgroundShape = new RectangleSFML(m_object->getShape()->getSize().x, 10, sf::Vector2f(m_object->getShape()->getPosition().x, m_object->getShape()->getPosition().y - m_object->getShape()->getSize().y / 2 - 10));
	m_shape = new RectangleSFML(m_object->getShape()->getSize().x - 5, 10, sf::Vector2f(m_object->getShape()->getPosition().x - 5, m_object->getShape()->getPosition().y - m_object->getShape()->getSize().y / 2 - 10));

	m_backgroundShape->setTexture(m_scene->getRoot()->getScene()->getTexture()->getTexture(m_animateBackground.getCurrentPath()));
	m_sizeDiff = m_object->getShape()->getSize().x / m_object->getCurrentLife();
	m_animate.resetTexture();
	switch (color)
	{
	case Color::Blue:
	{
		m_animate.add("BlueLife.png");
		m_shape->setTexture(m_scene->getRoot()->getScene()->getTexture()->getTexture(m_animate.getCurrentPath()));
	}
	break;
	case Color::Orange:
	{
		m_animate.add("OrangeLife.png");
		m_shape->setTexture(m_scene->getRoot()->getScene()->getTexture()->getTexture(m_animate.getCurrentPath()));
	}
	break;
	case Color::Pink:
	{
		m_animate.add("PinkLife.png");
		m_shape->setTexture(m_scene->getRoot()->getScene()->getTexture()->getTexture(m_animate.getCurrentPath()));
	}
	break;
	}
}

Life::~Life()
{
	delete m_backgroundShape;
	m_backgroundShape = nullptr;
}

void Life::Render()
{
	m_scene->getRoot()->getScene()->getWindow()->draw(static_cast<RectangleSFML*>(m_backgroundShape)->getShape());
	m_scene->getRoot()->getScene()->getWindow()->draw(static_cast<RectangleSFML*>(m_shape)->getShape());
}

void Life::Update(const float& deltatime)
{
	auto newsize = m_sizeDiff * m_object->getCurrentLife();
	m_shape->setSize(sf::Vector2f(newsize, m_shape->getSize().y));
	m_shape->setPosition(sf::Vector2f(m_object->getShape()->getPosition().x - 5, m_object->getShape()->getPosition().y - m_object->getShape()->getSize().y / 2 - 10));
	m_backgroundShape->setPosition(sf::Vector2f(m_object->getShape()->getPosition().x - 5, m_object->getShape()->getPosition().y - m_object->getShape()->getSize().y / 2 - 10));
}



Cursor::Cursor(IComposite* scene) :
	NonDestructibleObject(scene)
	, ILeaf(scene)
	, m_animate({ "Crossair.png","Crossair2.png","Crossair3.png" })
{
	m_shape = new CircleSFML(43, scene->getRoot()->getScene());
	m_shape->setTexture(m_scene->getRoot()->getScene()->getTexture()->getTexture(m_animate.getCurrentPath()));

}

void Cursor::ProcessInput(const sf::Event& event)
{
}

void Cursor::Update(const float& deltatime)
{
	sf::Vector2i mousePos = sf::Mouse::getPosition(*m_scene->getRoot()->getScene()->getWindow());
	m_shape->setPosition(sf::Vector2f(mousePos.x, mousePos.y));
}

void Cursor::Render()
{
	m_scene->getRoot()->getScene()->getWindow()->draw(static_cast<SquareSFML*>(m_shape)->getShape());
}

Physics::Physics(const float& maxVelocity)
	: m_maxVelocity(maxVelocity), movementDirection{ 0, 0, 0, 0 } {
}

void Physics::ExecutePhysics(KT::VectorND<bool, 4>& isStrafing, float framerate)
{
	movementDirection[trust::Right] = isStrafing[trust::Left] ? m_maxVelocity : 0;
	movementDirection[trust::Left] = isStrafing[trust::Right] ? -m_maxVelocity : 0;
	movementDirection[trust::Up] = isStrafing[trust::Down] ? -m_maxVelocity : 0;
	movementDirection[trust::Down] = isStrafing[trust::Up] ? m_maxVelocity : 0;
}

sf::Vector2f Physics::calculPosition(IShapeSFML* entity, ISceneBase* scene, float framerate)
{
	float x = movementDirection[trust::Left] + movementDirection[trust::Right];
	float y = movementDirection[trust::Up] + movementDirection[trust::Down];

	sf::Vector2f newPosition = {
		entity->getPosition().x + (x * scene->getRefreshTime().asSeconds()),
		entity->getPosition().y + (y * scene->getRefreshTime().asSeconds())
	};

	return newPosition;
}




DecorativeGameObject::DecorativeGameObject(IComposite* scene, const sf::Vector2f& position, float size) :
	NonDestructibleObject(scene),
	ILeaf(scene),
	m_animate({}),
	m_animationTimer(0.2f),
	m_animationSpeed(0.2f),
	m_isAnimating(true)
{
	m_shape = new SquareSFML(size, position);
}

DecorativeGameObject::DecorativeGameObject(IComposite* scene, const sf::Vector2f& position, const sf::Vector2f& size) :
	NonDestructibleObject(scene),
	ILeaf(scene),
	m_animate({}),
	m_animationTimer(0.2f),
	m_animationSpeed(0.2f),
	m_isAnimating(true)
{
	m_shape = new RectangleSFML(size.x, size.y, position);
}

void DecorativeGameObject::Update(const float& deltatime)
{
	if (m_isAnimating && m_animationTimer.AutoActionIsReady(m_scene->getRoot()->getScene()->getRefreshTime().asSeconds()))
	{
		m_animate.ChangeToNextPath();
		m_shape->setTexture(m_scene->getRoot()->getScene()->getTexture()->getTexture(m_animate.getCurrentPath()));
	}
}

void DecorativeGameObject::Render()
{
	if (auto* squareShape = dynamic_cast<SquareSFML*>(m_shape))
	{
		m_scene->getRoot()->getScene()->getWindow()->draw(squareShape->getShape());
	}
	else if (auto* rectShape = dynamic_cast<RectangleSFML*>(m_shape))
	{
		m_scene->getRoot()->getScene()->getWindow()->draw(rectShape->getShape());
	}
}

void DecorativeGameObject::setPosition(const sf::Vector2f& position)
{
	m_shape->setPosition(position);
}

sf::Vector2f DecorativeGameObject::getPosition() const
{
	return m_shape->getPosition();
}

void DecorativeGameObject::setAnimationTextures(const std::vector<std::string>& texturePaths)
{
	m_animate.resetTexture();
	for (const auto& path : texturePaths)
	{
		m_animate.add(path);
	}

	if (!texturePaths.empty())
	{
		m_shape->setTexture(m_scene->getRoot()->getScene()->getTexture()->getTexture(m_animate.getCurrentPath()));
	}
}

void DecorativeGameObject::setAnimationSpeed(float animationSpeed)
{
	m_animationSpeed = animationSpeed;
	m_animationTimer.setNewTimer(animationSpeed);
}

void DecorativeGameObject::stopAnimation()
{
	m_isAnimating = false;
}

void DecorativeGameObject::resumeAnimation()
{
	m_isAnimating = true;
}