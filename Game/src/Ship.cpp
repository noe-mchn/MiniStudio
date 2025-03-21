#include "Ship.h"
#include "IGameObject.h"
#include <iostream>

//=========== IDLE STATE ===========//
Ship::IState* Ship::IdleState::handle(const State& state)
{
    if (state == State::MOVE)
    {
        return new MoveState;
    }

    if (state == State::HAND_ATTACK)
    {
        return new HandAttackState();
    }

    if (state == State::PISTOL_ATTACK)
    {
        return new PistolAttackState();
    }
    return nullptr;
}

void Ship::IdleState::update(Ship* ship, float deltaTime)
{
    if (ship->m_strafe[trust::Left]
        || ship->m_strafe[trust::Right]
        || ship->m_strafe[trust::Up]
        || ship->m_strafe[trust::Down])
    {
        ship->ChangeState(State::MOVE);
    }

    if (sf::Mouse::isButtonPressed(sf::Mouse::Right) && ship->m_meleeAttackTimer.ActionIsReady())
    {
        ship->ChangeState(State::HAND_ATTACK);
        ship->m_meleeAttackTimer.resetTimer();
    }

    if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
    {
        ship->ChangeState(State::PISTOL_ATTACK);
    }
}

//=========== MOVE STATE ===========//
Ship::IState* Ship::MoveState::handle(const State& state)
{
    if (state == State::IDLE)
    {
        return new IdleState();
    }

    if (state == State::HAND_ATTACK)
    {
        return new HandAttackState();
    }

    if (state == State::PISTOL_ATTACK)
    {
        return new PistolAttackState();
    }
    return nullptr;
}

void Ship::MoveState::update(Ship* ship, float deltaTime)
{
    if (!ship->m_strafe[trust::Left]
        && !ship->m_strafe[trust::Right]
        && !ship->m_strafe[trust::Up]
        && !ship->m_strafe[trust::Down])
    {
        ship->ChangeState(State::IDLE);
    }

    static_cast<MovementInSpace*>(ship->m_physics)->ExecutePhysics(ship->m_strafe, ship->m_scene->getRoot()->getScene()->getRefreshTime().asSeconds());

    if (sf::Mouse::isButtonPressed(sf::Mouse::Right) && ship->m_meleeAttackTimer.ActionIsReady())
    {
        ship->ChangeState(State::HAND_ATTACK);
        ship->m_meleeAttackTimer.resetTimer();
    }
    else if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
    {
        ship->ChangeState(State::PISTOL_ATTACK);
    }
}

//=========== HAND ATTACK STATE ===========//
Ship::IState* Ship::HandAttackState::handle(const State& state)
{
    if (state == State::IDLE)
    {
        return new IdleState();
    }
    if (state == State::MOVE)
    {
        return new MoveState();
    }
    if (state == State::PISTOL_ATTACK)
    {
        return new PistolAttackState();
    }
    return nullptr;
}

void Ship::HandAttackState::update(Ship* ship, float deltaTime)
{
    if (attackTimer == 0.0f)
    {
        ship->createMeleeHitbox(ship->m_angle, 100.0f);
        damageDealt = false;
    }

    attackTimer += deltaTime;

    if (!damageDealt && meleeHitbox != nullptr)
    {
        ship->checkMeleeCollisions(meleeHitbox);
        damageDealt = true;
    }

    if (attackTimer >= attackDuration)
    {
        if (meleeHitbox != nullptr)
        {
            delete meleeHitbox;
            meleeHitbox = nullptr;
        }

        attackTimer = 0.0f;

        if (ship->m_strafe[trust::Left] || ship->m_strafe[trust::Right] ||
            ship->m_strafe[trust::Up] || ship->m_strafe[trust::Down])
        {
            ship->ChangeState(State::MOVE);
        }
        else
        {
            ship->ChangeState(State::IDLE);
        }
    }

    if (ship->m_strafe[trust::Left] || ship->m_strafe[trust::Right] ||
        ship->m_strafe[trust::Up] || ship->m_strafe[trust::Down])
    {
        static_cast<MovementInSpace*>(ship->m_physics)->ExecutePhysics(
            ship->m_strafe,
            ship->m_scene->getRoot()->getScene()->getRefreshTime().asSeconds()
        );
    }
}

//=========== PISTOL ATTACK STATE ===========//
Ship::IState* Ship::PistolAttackState::handle(const State& state)
{
    if (state == State::IDLE)
    {
        return new IdleState();
    }

    if (state == State::MOVE)
    {
        return new MoveState();
    }

    if (state == State::HAND_ATTACK)
    {
        return new HandAttackState();
    }

    if (state == State::RELOAD)
    {
        return new ReloadState();
    }

    return nullptr;
}

void Ship::PistolAttackState::update(Ship* ship, float deltaTime)
{
    if (!ship->m_turret)
        throw std::runtime_error("ship est nullptr!");

    if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
    {
        ship->m_turret->Fire();
    }

    if (ship->m_strafe[trust::Left]
        || ship->m_strafe[trust::Right]
        || ship->m_strafe[trust::Up]
        || ship->m_strafe[trust::Down])
    {
        ship->ChangeState(State::MOVE);
    }

    if (!ship->m_strafe[trust::Left]
        && !ship->m_strafe[trust::Right]
        && !ship->m_strafe[trust::Up]
        && !ship->m_strafe[trust::Down])
    {
        ship->ChangeState(State::IDLE);
    }
}

//=========== RELOAD STATE ===========//
Ship::IState* Ship::ReloadState::handle(const State& state)
{
    if (state == State::PISTOL_ATTACK)
    {
        return new PistolAttackState();
    }

    return nullptr;
}

void Ship::ReloadState::update(Ship* ship, float deltaTime)
{
    if (reloadTimer < reloadTime)
    {
        reloadTimer += 1;
    }

    if (reloadTimer >= reloadTime)
    {
        reloadTimer = 0.f;
        ship->ChangeState(State::PISTOL_ATTACK);
        return;
    }
}

//=========== SHIP IMPLEMENTATION ===========//
Ship::Ship(IComposite* scene, IShapeSFML* background)
    : DestructibleObject(scene, 10)
    , IComposite(scene)
    , m_background(background)
    , m_angle(0)
    , m_elapsedTime(0.2)
    , m_animate({ "SpaceHero.png" })
    , m_physics(new MovementInSpace(1000, 400, 200))
    , m_invisibility(2.5)
    , m_detectionRadius(30.0f)
    , m_meleeDamage(0.01f)
    , m_meleeAttackCooldown(0.5f)
    , m_meleeAttackTimer(2.0f)
{
    m_shape = new SquareSFML(150, scene->getRoot()->getScene());
    m_shape->setTexture(m_scene->getRoot()->getScene()->getTexture()->getTexture(m_animate.getCurrentPath()));
    new Life(this, this, Color::Blue);
    m_turret = new FixTurret(this, m_shape, sf::Vector2f(35, -25), 0.75);
    m_turret->SetFireRate(0.2f);
    m_turret->SetOverloadGun(5, 30);
    m_turret->setBullet(0, 0, 0);

    m_currentState = new IdleState();
}

Ship::~Ship()
{
    delete m_physics;
    delete m_currentState;
}

bool Ship::IsDestroyed()
{
	return m_life <= 0;
}

void Ship::ProssesInput(const sf::Event& event)
{
    m_strafe = { false, false, false, false };

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z))
        m_strafe[trust::Up] = true;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
        m_strafe[trust::Down] = true;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q))
        m_strafe[trust::Left] = true;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
        m_strafe[trust::Right] = true;

    physics();
}

void Ship::physics()
{
    m_angle = anglecalcul();
}

void Ship::Update(const float& deltatime)
{
    if (!m_currentState)
        throw std::runtime_error("current state est nullptr!");

    m_currentState->update(this, deltatime);

    m_shape->setRotation(m_angle);
    m_background->setPosition(static_cast<MovementInSpace*>(m_physics)->calculPosition(m_background, m_scene->getRoot()->getScene(), m_scene->getRoot()->getScene()->getRefreshTime().asSeconds()));

    if (m_elapsedTime.AutoActionIsReady(m_scene->getRoot()->getScene()->getRefreshTime().asSeconds()))
    {
        m_animate.ChangeToNextPath();
        m_shape->setTexture(m_scene->getRoot()->getScene()->getTexture()->getTexture(m_animate.getCurrentPath()));
    }

    m_meleeAttackTimer.NextTIck(deltatime);

    IComposite::Update(deltatime);
    m_invisibility.NextTIck(m_scene->getRoot()->getScene()->getRefreshTime().asSeconds());
}

void Ship::Render()
{
    m_scene->getRoot()->getScene()->getWindow()->draw(static_cast<SquareSFML*>(m_shape)->getShape());

    HandAttackState* state = dynamic_cast<HandAttackState*>(m_currentState);
    if (state && state->meleeHitbox)
    {
        m_scene->getRoot()->getScene()->getWindow()->draw(state->meleeHitbox->getShape());
    }

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

void Ship::ChangeState(const State& newState)
{
    if (m_currentState)
    {
        IState* state = m_currentState->handle(newState);
        if (state)
        {
            delete m_currentState;
            m_currentState = state;
        }
    }
}

float Ship::distanceToBoss(MegaBoss* boss)
{
    sf::Vector2f myPos = this->getShape()->getPosition();
    sf::Vector2f bossPos = boss->getShape()->getPosition();

    return std::sqrt(std::pow(bossPos.x - myPos.x, 2) + std::pow(bossPos.y - myPos.y, 2));
}

void Ship::createMeleeHitbox(float angle, float offsetDistance)
{
    float angleRadians = angle * 3.14159f / 180.0f;

    float offsetX = std::cos(angleRadians) * offsetDistance;
    float offsetY = std::sin(angleRadians) * offsetDistance;

    sf::Vector2f shipPos = m_shape->getPosition();

    sf::Vector2f hitboxPos(shipPos.x + offsetX, shipPos.y + offsetY);

    HandAttackState* state = dynamic_cast<HandAttackState*>(m_currentState);
    if (state)
    {
        float hitboxWidth = 100.0f;
        float hitboxHeight = 60.0f;

        state->meleeHitbox = new RectangleSFML(hitboxWidth, hitboxHeight, hitboxPos);

        sf::Color redColor(255, 0, 0, 150);
        state->meleeHitbox->getShape().setFillColor(redColor);
        state->meleeHitbox->setRotation(angle);
    }
}

void Ship::checkMeleeCollisions(RectangleSFML* attackHitbox)
{
    for (auto child : m_scene->getRoot()->getFullTree())
    {
        MegaBoss* boss = dynamic_cast<MegaBoss*>(child);
        if (boss)
        {
            AABB attackBB = attackHitbox->GetBoundingBox();
            AABB bossBB = boss->getShape()->GetBoundingBox();

            bool collision = !(
                attackBB.Amin.x > bossBB.Amax.x ||
                attackBB.Amax.x < bossBB.Amin.x ||
                attackBB.Amin.y > bossBB.Amax.y ||
                attackBB.Amax.y < bossBB.Amin.y
                );

            if (collision)
            {
                boss->ChangeLife(-m_meleeDamage);

                std::cout << "Melee hit on boss! Damage: " << m_meleeDamage << std::endl;
            }
        }
    }
}