#include "Hero.h"
#include "IGameObject.h"
#include "Animation.h"
#include <iostream>

//=========== IDLE STATE ===========//
Hero::IState* Hero::IdleState::handle(const State& state)
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

void Hero::IdleState::update(Hero* ship, float deltaTime)
{
    std::string animationName = "idle_" + ship->getOrientationString();
    if (ship->m_animationComponent->getCurrentAnimation() != animationName)
    {
        ship->m_animationComponent->playAnimation(animationName);
    }

    if (ship->m_strafe[trust::Left]
        || ship->m_strafe[trust::Right]
        || ship->m_strafe[trust::Up]
        || ship->m_strafe[trust::Down])
    {
        ship->ChangeState(State::MOVE);
        return;
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
Hero::IState* Hero::MoveState::handle(const State& state)
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

void Hero::MoveState::update(Hero* ship, float deltaTime)
{
    std::string animationName = "move_" + ship->getOrientationString();
    if (ship->m_animationComponent->getCurrentAnimation() != animationName)
    {
        ship->m_animationComponent->playAnimation(animationName);
    }

    if (!ship->m_strafe[trust::Left]
        && !ship->m_strafe[trust::Right]
        && !ship->m_strafe[trust::Up]
        && !ship->m_strafe[trust::Down])
    {
        ship->ChangeState(State::IDLE);
        return;
    }

    static_cast<Physics*>(ship->m_physics)->ExecutePhysics(
        ship->m_strafe,
        ship->m_scene->getRoot()->getScene()->getRefreshTime().asSeconds()
    );

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
Hero::IState* Hero::HandAttackState::handle(const State& state)
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

void Hero::HandAttackState::update(Hero* ship, float deltaTime)
{
    std::string attackAnimName = "attack_" + ship->getOrientationString();
    if (ship->m_animationComponent->getCurrentAnimation() != attackAnimName)
    {
        ship->m_animationComponent->playAnimation(attackAnimName);
    }

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
        static_cast<Physics*>(ship->m_physics)->ExecutePhysics(
            ship->m_strafe,
            ship->m_scene->getRoot()->getScene()->getRefreshTime().asSeconds()
        );
    }
}

//=========== PISTOL ATTACK STATE ===========//
Hero::IState* Hero::PistolAttackState::handle(const State& state)
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

void Hero::PistolAttackState::update(Hero* ship, float deltaTime)
{
    std::string attackAnimName = "attack_" + ship->getOrientationString();
    if (ship->m_animationComponent->getCurrentAnimation() != attackAnimName)
    {
        ship->m_animationComponent->playAnimation(attackAnimName);
    }

    if (!ship->m_turret)
        throw std::runtime_error("ship est nullptr!");

    if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
    {
        ship->m_turret->Fire();
    }

    if (ship->m_animationComponent->isAnimationFinished())
    {
        if (ship->m_strafe[trust::Left]
            || ship->m_strafe[trust::Right]
            || ship->m_strafe[trust::Up]
            || ship->m_strafe[trust::Down])
        {
            ship->ChangeState(State::MOVE);
        }
        else if (!sf::Mouse::isButtonPressed(sf::Mouse::Left))
        {
            ship->ChangeState(State::IDLE);
        }
        else
        {
            ship->m_animationComponent->playAnimation(attackAnimName);
        }
    }

    if (ship->m_strafe[trust::Left]
        || ship->m_strafe[trust::Right]
        || ship->m_strafe[trust::Up]
        || ship->m_strafe[trust::Down])
    {
        static_cast<Physics*>(ship->m_physics)->ExecutePhysics(
            ship->m_strafe,
            ship->m_scene->getRoot()->getScene()->getRefreshTime().asSeconds()
        );
    }
}

//=========== RELOAD STATE ===========//
Hero::IState* Hero::ReloadState::handle(const State& state)
{
    if (state == State::PISTOL_ATTACK)
    {
        return new PistolAttackState();
    }

    return nullptr;
}

void Hero::ReloadState::update(Hero* ship, float deltaTime)
{
    if (ship->m_animationComponent->getCurrentAnimation() != "reload")
        ship->m_animationComponent->playAnimation("reload");

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
Hero::Hero(IComposite* scene, IShapeSFML* background)
    : DestructibleObject(scene, 10)
    , IComposite(scene)
    , m_background(background)
    , m_angle(0)
    , m_elapsedTime(0.2)
    , m_animate({ "Hero.png" })
    , m_physics(new Physics(500))
    , m_invisibility(2.5)
    , m_detectionRadius(30.0f)
    , m_meleeDamage(0.01f)
    , m_meleeAttackCooldown(0.5f)
    , m_meleeAttackTimer(2.0f)
    , m_currentOrientation(Orientation::DOWN)
{
    m_shape = new SquareSFML(32, scene->getRoot()->getScene());
    m_shape->setTexture(m_scene->getRoot()->getScene()->getTexture()->getTexture(m_animate.getCurrentPath()));

    m_animationComponent = new AnimationComponent(this);
    setupAnimations();

    m_animationComponent->setScale(sf::Vector2f(4.0f, 4.0f));

    m_animationComponent->updatePosition(m_shape->getPosition());

    new Life(this, this, Color::Blue);
    m_turret = new FixTurret(this, m_shape, sf::Vector2f(35, -25), 0.75);
    m_turret->SetFireRate(0.2f);
    m_turret->SetOverloadGun(5, 30);
    m_turret->setBullet(0, 0, 0);

    m_currentState = new IdleState();

    m_animationComponent->playAnimation("idle_down");
}

Hero::~Hero()
{
    delete m_physics;
    delete m_currentState;
}

void Hero::setupAnimations()
{
    const int FRAME_WIDTH = 64;
    const int FRAME_HEIGHT = 64;

    const int IDLE_FRAMES = 6;
    const int MOVE_FRAMES = 5;
    const int ATTACK_FRAMES = 6;

    Animation idleAnimDown("Hero.png", IDLE_FRAMES, 100.f);
    idleAnimDown.setFrameSize(sf::Vector2i(FRAME_WIDTH, FRAME_HEIGHT));
    idleAnimDown.setStartPosition(sf::Vector2i(0, 0 * FRAME_HEIGHT), 1);

    Animation idleAnimUp("Hero.png", IDLE_FRAMES, 100.f);
    idleAnimUp.setFrameSize(sf::Vector2i(FRAME_WIDTH, FRAME_HEIGHT));
    idleAnimUp.setStartPosition(sf::Vector2i(0, 3 * FRAME_HEIGHT), 1);

    Animation idleAnimLeft("Hero.png", IDLE_FRAMES, 100.f);
    idleAnimLeft.setFrameSize(sf::Vector2i(FRAME_WIDTH, FRAME_HEIGHT));
    idleAnimLeft.setStartPosition(sf::Vector2i(0, 7 * FRAME_HEIGHT), 1);

    Animation idleAnimRight("Hero.png", IDLE_FRAMES, 100.f);
    idleAnimRight.setFrameSize(sf::Vector2i(FRAME_WIDTH, FRAME_HEIGHT));
    idleAnimRight.setStartPosition(sf::Vector2i(0, 9 * FRAME_HEIGHT), 1);

    Animation moveAnimDown("Hero.png", MOVE_FRAMES, 100.f); 
    moveAnimDown.setFrameSize(sf::Vector2i(FRAME_WIDTH, FRAME_HEIGHT));
    moveAnimDown.setStartPosition(sf::Vector2i(0, 1 * FRAME_HEIGHT), 1);

    Animation moveAnimUp("Hero.png", MOVE_FRAMES, 100.f);
    moveAnimUp.setFrameSize(sf::Vector2i(FRAME_WIDTH, FRAME_HEIGHT));
    moveAnimUp.setStartPosition(sf::Vector2i(0, 4 * FRAME_HEIGHT), 1);

    Animation moveAnimLeft("Hero.png", MOVE_FRAMES, 100.f);
    moveAnimLeft.setFrameSize(sf::Vector2i(FRAME_WIDTH, FRAME_HEIGHT));
    moveAnimLeft.setStartPosition(sf::Vector2i(0, 6 * FRAME_HEIGHT), 1);

    Animation moveAnimRight("Hero.png", MOVE_FRAMES, 100.f);
    moveAnimRight.setFrameSize(sf::Vector2i(FRAME_WIDTH, FRAME_HEIGHT));
    moveAnimRight.setStartPosition(sf::Vector2i(0, 10 * FRAME_HEIGHT), 1);

    Animation attackAnimDown("Hero.png", ATTACK_FRAMES, 50, false);
    attackAnimDown.setFrameSize(sf::Vector2i(FRAME_WIDTH, FRAME_HEIGHT));
    attackAnimDown.setStartPosition(sf::Vector2i(0, 2 * FRAME_HEIGHT), 1);

    Animation attackAnimUp("Hero.png", ATTACK_FRAMES, 50, false);
    attackAnimUp.setFrameSize(sf::Vector2i(FRAME_WIDTH, FRAME_HEIGHT));
    attackAnimUp.setStartPosition(sf::Vector2i(0, 5 * FRAME_HEIGHT), 1);

    Animation attackAnimLeft("Hero.png", ATTACK_FRAMES, 50, false);
    attackAnimLeft.setFrameSize(sf::Vector2i(FRAME_WIDTH, FRAME_HEIGHT));
    attackAnimLeft.setStartPosition(sf::Vector2i(0, 8 * FRAME_HEIGHT), 1);

    Animation attackAnimRight("Hero.png", ATTACK_FRAMES, 50, false);
    attackAnimRight.setFrameSize(sf::Vector2i(FRAME_WIDTH, FRAME_HEIGHT));
    attackAnimRight.setStartPosition(sf::Vector2i(0, 11 * FRAME_HEIGHT), 1);

    m_animationComponent->addAnimation("idle_down", idleAnimDown);
    m_animationComponent->addAnimation("idle_up", idleAnimUp);
    m_animationComponent->addAnimation("idle_left", idleAnimLeft);
    m_animationComponent->addAnimation("idle_right", idleAnimRight);

    m_animationComponent->addAnimation("move_down", moveAnimDown);
    m_animationComponent->addAnimation("move_up", moveAnimUp);
    m_animationComponent->addAnimation("move_left", moveAnimLeft);
    m_animationComponent->addAnimation("move_right", moveAnimRight);

    m_animationComponent->addAnimation("attack_down", attackAnimDown);
    m_animationComponent->addAnimation("attack_up", attackAnimUp);
    m_animationComponent->addAnimation("attack_left", attackAnimLeft);
    m_animationComponent->addAnimation("attack_right", attackAnimRight);
}

bool Hero::IsDestroyed()
{
    return m_life <= 0;
}

void Hero::ProcessInput(const sf::Event& event)
{

}

void Hero::physics()
{
    m_angle = anglecalcul();
}

void Hero::Update(const float& deltatime)
{
    if (!m_currentState)
        throw std::runtime_error("current state est nullptr!");

    m_strafe = { false, false, false, false };

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z))
        m_strafe[trust::Up] = true;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
        m_strafe[trust::Down] = true;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q))
        m_strafe[trust::Left] = true;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
        m_strafe[trust::Right] = true;

    m_angle = anglecalcul();

    Orientation newOrientation = determineOrientation(m_angle);
    if (newOrientation != m_currentOrientation)
    {
        m_currentOrientation = newOrientation;

        std::string currentAnim = m_animationComponent->getCurrentAnimation();
        if (!currentAnim.empty())
        {
            std::string baseAnim;
            if (currentAnim.find("idle_") != std::string::npos) baseAnim = "idle_";
            else if (currentAnim.find("move_") != std::string::npos) baseAnim = "move_";
            else if (currentAnim.find("attack_") != std::string::npos) baseAnim = "attack_";

            if (!baseAnim.empty())
            {
                std::string newAnimName = baseAnim + getOrientationString();
                m_animationComponent->playAnimation(newAnimName);
            }
        }
    }

    m_currentState->update(this, deltatime);

    m_shape->setRotation(0);

    m_background->setPosition(static_cast<Physics*>(m_physics)->calculPosition(
        m_background, m_scene->getRoot()->getScene(), m_scene->getRoot()->getScene()->getRefreshTime().asSeconds()));

    m_animationComponent->updatePosition(m_shape->getPosition());

    m_animationComponent->Update(deltatime);

    m_meleeAttackTimer.NextTIck(deltatime);
    IComposite::Update(deltatime);
    m_invisibility.NextTIck(m_scene->getRoot()->getScene()->getRefreshTime().asSeconds());
}

void Hero::Render()
{
    m_animationComponent->Render();

    if (m_animationComponent->getCurrentAnimation() != "")
    {
        sf::RectangleShape debugRect;
        sf::Vector2f pos = m_shape->getPosition();
        sf::Vector2i frameSize = m_animationComponent->getAnimations()[m_animationComponent->getCurrentAnimation()].getFrameSize();
        float scale = m_animationComponent->getScale().x;

        debugRect.setSize(sf::Vector2f(frameSize.x * scale, frameSize.y * scale));
        debugRect.setOrigin(frameSize.x * scale / 2.0f, frameSize.y * scale / 2.0f);
        debugRect.setPosition(pos);
        debugRect.setFillColor(sf::Color::Transparent);

        m_scene->getRoot()->getScene()->getWindow()->draw(debugRect);
    }

    HandAttackState* state = dynamic_cast<HandAttackState*>(m_currentState);
    if (state && state->meleeHitbox)
    {
        m_scene->getRoot()->getScene()->getWindow()->draw(state->meleeHitbox->getShape());
    }

    IComposite::Render();
}

float Hero::anglecalcul()
{
    sf::Vector2i mousePos = sf::Mouse::getPosition(*m_scene->getRoot()->getScene()->getWindow());
    sf::Vector2f shipPos = m_shape->getPosition();
    float deltaX = mousePos.x - shipPos.x;
    float deltaY = mousePos.y - shipPos.y;
    float angle = std::atan2(deltaY, deltaX) * 180 / 3.14159f;
    return angle;
}

void Hero::HandleCollision(IGameObject* object)
{
    if (object->globalGameObjectType() != GameObjectType::DestructibleObject)
        return;

    ChangeLife(-1);
}

void Hero::ChangeLife(const float& life)
{
    if (!m_invisibility.ActionIsReady())
        return;

    m_life += life;
    if (m_life <= 0)
        destroy();
    m_invisibility.resetTimer();
}

void Hero::ChangeState(const State& newState)
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

Orientation Hero::determineOrientation(float angle)
{
    angle = std::fmod(angle + 360.0f, 360.0f);

    if (angle >= 315.0f || angle < 45.0f)
        return Orientation::RIGHT;
    else if (angle >= 45.0f && angle < 135.0f)
        return Orientation::DOWN;
    else if (angle >= 135.0f && angle < 225.0f)
        return Orientation::LEFT;
    else
        return Orientation::UP;
}

float Hero::distanceToBoss(MegaBoss* boss)
{
    sf::Vector2f myPos = this->getShape()->getPosition();
    sf::Vector2f bossPos = boss->getShape()->getPosition();

    return std::sqrt(std::pow(bossPos.x - myPos.x, 2) + std::pow(bossPos.y - myPos.y, 2));
}

void Hero::createMeleeHitbox(float angle, float offsetDistance)
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

void Hero::checkMeleeCollisions(RectangleSFML* attackHitbox)
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
            }
        }
    }
}

std::string Hero::getOrientationString() const
{
    switch (m_currentOrientation)
    {
    case Orientation::UP:
        return "up";
    case Orientation::DOWN:
        return "down";
    case Orientation::LEFT:
        return "left";
    case Orientation::RIGHT:
        return "right";
    default:
        return "down";
    }
}
