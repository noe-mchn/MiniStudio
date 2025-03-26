#include "MegaBoss.h"
#include "RandomNumber.h"
#include "Hero.h"
#include <functional>
#include <iostream>
#include <cmath>

BossParameters BossParameters::getForPhase(BossPhase phase, BossMode mode)
{
    BossParameters params;

    switch (phase)
    {
    case BossPhase::One:
        params = { 300.0f, 1.0f, 10, -90.0f, 20.0f, 500.0f, 1.0f, false };
        break;
    case BossPhase::Two:
        params = { 400.0f, 1.0f, 20, 0.0f, 25.0f, 750.0f, 1.5f, false };
        break;
    case BossPhase::Three:
        params = { 500.0f, 1.0f, 30, 0.0f, 30.0f, 1000.0f, 2.0f, false };
        break;
    case BossPhase::Four:
        params = { 600.0f, 1.0f, 40, 0.0f, 35.0f, 1250.0f, 2.5f, false };
        break;
    case BossPhase::Five:
        params = { 700.0f, 0.5f, 50, 0.0f, 40.0f, 1500.0f, 3.0f, false };
        break;
    }

    switch (mode)
    {
    case BossMode::Type1:
        params.speed *= 1.2f;
        params.attackRate *= 0.8f;
        break;
    case BossMode::Type2:
        params.projectileSize *= 1.5f;
        params.invulnerableToCollisions = true;
        break;
    case BossMode::Type3:
        params.spreadAngle *= 1.5f;
        params.projectileSpeed *= 1.3f;
        break;
    case BossMode::Combined:
        params.speed *= 1.1f;
        params.attackRate *= 0.9f;
        params.projectileSize *= 1.2f;
        params.projectileSpeed *= 1.2f;
        params.damageMultiplier *= 1.1f;
        params.invulnerableToCollisions = true;
        break;
    }

    return params;
}


MegaBoss::IState* MegaBoss::PatrolState::handle(const State& state)
{
    switch (state)
    {
    case State::CHASE: return new ChaseState();
    case State::LOAD: return new LoadState();
    case State::FIRE: return new FireState();
    case State::DESTRUCT: return new DestructState();
    case State::HAND: return new HandState();
    case State::PROTECT: return new ProtectState();
    default: return nullptr;
    }
}

void MegaBoss::PatrolState::update(MegaBoss* boss, float deltaTime)
{
    std::string animationName = "move_" + boss->getOrientationString();
    if (boss->getAnimationComponent()->getCurrentAnimation() != animationName)
    {
        boss->getAnimationComponent()->playAnimation(animationName);
    }

    sf::Vector2f direction = boss->m_target->getPosition() - boss->getShape()->getPosition();
    float distance = std::sqrt(direction.x * direction.x + direction.y * direction.y);

    boss->patrol();

    switch (boss->m_currentMode)
    {
    case BossMode::Type1:
        if (boss->getCurrentLife() <= boss->getMaxLife() * 0.05)
            boss->changeState(State::DESTRUCT);
        else if (distance < 600.0f)
            boss->changeState(State::CHASE);
        break;

    case BossMode::Type2:
        if (boss->getCurrentLife() <= boss->getMaxLife() * 0.3)
            boss->changeState(State::PROTECT);
        else if (distance < 500.0f)
            boss->changeState(State::HAND);
        break;

    case BossMode::Type3:
    case BossMode::Combined:
        if (boss->getCurrentLife() <= boss->getMaxLife() * 0.15)
            boss->changeState(State::PROTECT);
        else if (distance < 700.0f)
            boss->changeState(State::CHASE);
        break;
    }

    static Timer specialAttackTimer(8.0f);
    if (specialAttackTimer.AutoActionIsReady(deltaTime))
    {
        int attackChoice = UseRandomNumber().getRandomNumber<int>(0, 2);
        switch (attackChoice) {
        case 0: boss->changeState(State::HAND); break;
        case 1: boss->changeState(State::FIRE); break;
        case 2:
            if (boss->getCurrentLife() < boss->getMaxLife() * 0.8 &&
                boss->m_canEnterProtectState)
                boss->changeState(State::PROTECT);
            else
                boss->changeState(State::FIRE);
            break;
        }
    }
}



MegaBoss::IState* MegaBoss::ChaseState::handle(const State& state)
{
    switch (state)
    {
    case State::PATROL: return new PatrolState();
    case State::LOAD: return new LoadState();
    case State::FIRE: return new FireState();
    case State::DESTRUCT: return new DestructState();
    case State::HAND: return new HandState();
    case State::PROTECT: return new ProtectState();
    default: return nullptr;
    }
}

float MegaBoss::getCurrentPhaseDistance() const
{
    switch (m_currentPhase)
    {
    case BossPhase::One:   return PHASE_ONE_DISTANCE;
    case BossPhase::Two:   return PHASE_TWO_DISTANCE;
    case BossPhase::Three: return PHASE_THREE_DISTANCE;
    case BossPhase::Four:  return PHASE_FOUR_DISTANCE;
    case BossPhase::Five:  return PHASE_FIVE_DISTANCE;
    default:               return PHASE_THREE_DISTANCE;
    }
}

void MegaBoss::ChaseState::update(MegaBoss* boss, float deltaTime)
{
    std::string animationName = "move_" + boss->getOrientationString();
    if (boss->getAnimationComponent()->getCurrentAnimation() != animationName)
    {
        boss->getAnimationComponent()->playAnimation(animationName);
    }

    sf::Vector2f direction = boss->m_target->getPosition() - boss->getShape()->getPosition();
    float distance = std::sqrt(direction.x * direction.x + direction.y * direction.y);
    float targetDistance = boss->getCurrentPhaseDistance();
    float detectionDistance = targetDistance + 150.0f;
    float attackDistance = targetDistance - 50.0f;

    switch (boss->m_currentMode)
    {
    case BossMode::Type1:
        if (boss->getCurrentLife() <= boss->getMaxLife() * 0.05)
            boss->changeState(State::DESTRUCT);
        else if (distance >= detectionDistance)
            boss->changeState(State::PATROL);
        else if (distance <= attackDistance)
            boss->changeState(State::LOAD);
        break;

    case BossMode::Type2:
        if (boss->getCurrentLife() <= boss->getMaxLife() * 0.3)
            boss->changeState(State::PROTECT);
        else if (distance <= (boss->getCurrentPhase() == BossPhase::Five ? 300.0f : attackDistance))
            boss->changeState(State::HAND);
        break;

    case BossMode::Type3:
    case BossMode::Combined:
        if (boss->getCurrentLife() <= boss->getMaxLife() * 0.15)
            boss->changeState(State::PROTECT);
        else if (distance <= attackDistance)
            boss->changeState(State::LOAD);
        else if (distance >= detectionDistance)
            boss->changeState(State::PATROL);
        break;
    }

    if (distance > 0)
    {
        direction /= distance;

        if (boss->getCurrentPhase() != BossPhase::Five && distance < targetDistance - 75.0f)
        {
            boss->move(-direction * boss->getSpeed() * 0.5f * deltaTime);
        }

        else if (distance > targetDistance + 25.0f)
        {
            boss->move(direction * boss->getSpeed() * deltaTime);
        }

        else
        {
            float angle = std::atan2(direction.y, direction.x) * 180 / 3.14159f;
            boss->getShape()->setRotation(angle);
        }
    }
}



MegaBoss::IState* MegaBoss::LoadState::handle(const State& state)
{
    switch (state)
    {
    case State::FIRE: return new FireState();
    case State::PATROL: return new PatrolState();
    case State::CHASE: return new ChaseState();
    case State::DESTRUCT: return new DestructState();
    default: return nullptr;
    }
}

void MegaBoss::LoadState::update(MegaBoss* boss, float deltaTime)
{
    std::string animationName = "idle_" + boss->getOrientationString();
    if (boss->getAnimationComponent()->getCurrentAnimation() != animationName)
    {
        boss->getAnimationComponent()->playAnimation(animationName);
    }

    sf::Vector2f direction = boss->m_target->getPosition() - boss->getShape()->getPosition();
    float distance = std::sqrt(direction.x * direction.x + direction.y * direction.y);

    if (boss->getCurrentLife() <= boss->getMaxLife() * 0.05)
    {
        boss->changeState(State::DESTRUCT);
        return;
    }

    if (reloadTimer < reloadTime)
    {
        reloadTimer += 1;
    }

    if (reloadTimer >= reloadTime)
    {
        reloadTimer = 0.0f;
        boss->changeState(State::FIRE);
        return;
    }
}



MegaBoss::IState* MegaBoss::FireState::handle(const State& state)
{
    switch (state)
    {
    case State::PATROL: return new PatrolState();
    case State::CHASE: return new ChaseState();
    case State::LOAD: return new LoadState();
    case State::DESTRUCT: return new DestructState();
    default: return nullptr;
    }
}

void MegaBoss::FireState::update(MegaBoss* boss, float deltaTime)
{
    std::string animationName = "attack_" + boss->getOrientationString();
    if (boss->getAnimationComponent()->getCurrentAnimation() != animationName)
    {
        boss->getAnimationComponent()->playAnimation(animationName);
    }

    sf::Vector2f direction = boss->m_target->getPosition() - boss->getShape()->getPosition();
    float distance = std::sqrt(direction.x * direction.x + direction.y * direction.y);

    if (boss->getCurrentLife() <= boss->getMaxLife() * 0.05)
    {
        boss->changeState(State::DESTRUCT);
        return;
    }

    switch (boss->getCurrentPhase())
    {
    case BossPhase::One:
        boss->fireProjectiles(1, 0);
        break;
    case BossPhase::Two:
        boss->fireProjectiles(2, 20.0f);

        if (UseRandomNumber().getRandomNumber<int>(0, 10) > 8)
            boss->fireCircularPattern();
        break;
    case BossPhase::Three:

        if (UseRandomNumber().getRandomNumber<int>(0, 10) > 6)
            boss->fireWavePattern();
        else
            boss->fireProjectiles(3, 40.0f);
        break;
    case BossPhase::Four:

        if (UseRandomNumber().getRandomNumber<int>(0, 10) > 3)
            boss->fireGrowingProjectile();
        else
            boss->fireProjectiles(4, 60.0f);
        break;
    case BossPhase::Five:

        if (UseRandomNumber().getRandomNumber<int>(0, 10) > 2)
        {
            if (UseRandomNumber().getRandomNumber<int>(0, 2) == 0)
                boss->fireGrowingProjectile();
            else if (UseRandomNumber().getRandomNumber<int>(0, 2) == 1)
                boss->fireFastProjectile();
            else
                boss->fireCircularPattern();
        }
        else
            boss->fireProjectiles(5, 90.0f);
        break;
    }

    if (boss->getCurrentPhase() >= BossPhase::Three &&
        UseRandomNumber().getRandomNumber<int>(0, 10) > 5)
    {
        boss->fireCircularPattern();
    }

    if (boss->m_currentMode == BossMode::Combined &&
        boss->getCurrentPhase() >= BossPhase::Three &&
        UseRandomNumber().getRandomNumber<int>(0, 10) > 7)
    {
        boss->fireCircularPattern();
    }

    if (distance > 300.0f)
        boss->changeState(State::CHASE);
    else
        boss->changeState(State::LOAD);
}

MegaBoss::IState* MegaBoss::HandState::handle(const State& state)
{
    switch (state)
    {
    case State::PATROL: return new PatrolState();
    case State::CHASE: return new ChaseState();
    case State::PROTECT: return new ProtectState();
    default: return nullptr;
    }
}

void MegaBoss::HandState::update(MegaBoss* boss, float deltaTime)
{
    std::string animationName = "attack_" + boss->getOrientationString();
    if (boss->getAnimationComponent()->getCurrentAnimation() != animationName)
    {
        boss->getAnimationComponent()->playAnimation(animationName);
    }

    sf::Vector2f direction = boss->m_target->getPosition() - boss->getShape()->getPosition();
    float distance = std::sqrt(direction.x * direction.x + direction.y * direction.y);

    if (boss->m_attackTimer.ActionIsReady())
    {
        if (distance < 650.0f)
        {
            boss->fireWavePattern();
            boss->m_attackTimer.resetTimer();
        }
    }

    if (boss->getCurrentLife() <= boss->getMaxLife() * 0.3)
    {
        boss->changeState(State::PROTECT);
        return;
    }

    if (distance > 100.f)
    {
        boss->changeState(State::CHASE);
        return;
    }
}



MegaBoss::IState* MegaBoss::ProtectState::handle(const State& state)
{
    switch (state)
    {
    case State::PATROL: return new PatrolState();
    case State::CHASE: return new ChaseState();
    case State::DEAD: return new DeadState();
    case State::DESTRUCT: return new DestructState();
    default: return nullptr;
    }
}

void MegaBoss::ProtectState::update(MegaBoss* boss, float deltaTime)
{
    std::string animationName = "idle_" + boss->getOrientationString();
    if (boss->getAnimationComponent()->getCurrentAnimation() != animationName)
    {
        boss->getAnimationComponent()->playAnimation(animationName);
    }

    protectionTimer += deltaTime;

    boss->regenerateHealth(regenerationRate * deltaTime);

    if (protectionTimer >= maxProtectionTime) {
        boss->m_isInvulnerable = false;
        boss->changeState(State::CHASE);
        protectionTimer = 0.0f;

        boss->m_canEnterProtectState = false;
        boss->m_protectStateCooldown.resetTimer();
        return;
    }

    if (boss->m_currentMode == BossMode::Type1 &&
        boss->getCurrentLife() <= boss->getMaxLife() * 0.05)
    {
        boss->m_isInvulnerable = false;
        boss->changeState(State::DESTRUCT);
        return;
    }

    if (boss->getCurrentLife() <= boss->getMaxLife() * 0.02)
    {
        boss->m_isInvulnerable = false;
        boss->changeState(State::DEAD);
        return;
    }

    if (boss->getCurrentLife() >= boss->getMaxLife() * 0.40)
    {
        boss->m_isInvulnerable = false;
        boss->changeState(State::PATROL);
        return;
    }

    boss->m_isInvulnerable = true;

    if (boss->m_attackTimer.ActionIsReady())
    {
        boss->fireCircularPattern();
        boss->m_attackTimer.resetTimer();
    }
}



MegaBoss::IState* MegaBoss::DestructState::handle(const State& state)
{
    switch (state)
    {
    case State::HELP: return new HelpState();
    case State::DEAD: return new DeadState();
    default: return nullptr;
    }
}

void MegaBoss::DestructState::update(MegaBoss* boss, float deltaTime)
{
    std::string animationName = "attack_" + boss->getOrientationString();
    if (boss->getAnimationComponent()->getCurrentAnimation() != animationName)
    {
        boss->getAnimationComponent()->playAnimation(animationName);
    }

    if (boss->getCurrentLife() <= boss->getMaxLife() * 0.025)
    {
        boss->changeState(State::HELP);
        return;
    }

    if (boss->m_attackTimer.ActionIsReady())
    {
        int attackType = UseRandomNumber().getRandomNumber<int>(0, 3);
        switch (attackType)
        {
        case 0: boss->fireGrowingProjectile(); break;
        case 1: boss->fireFastProjectile(); break;
        case 2: boss->fireCircularPattern(); break;
        case 3: boss->fireWavePattern(); break;
        }
        boss->m_attackTimer.resetTimer();
    }
}


MegaBoss::IState* MegaBoss::HelpState::handle(const State& state)
{
    switch (state)
    {
    case State::DEAD: return new DeadState();
    default: return nullptr;
    }
}

void MegaBoss::HelpState::update(MegaBoss* boss, float deltaTime)
{
    if (boss->getCurrentLife() <= boss->getMaxLife() * 0.01)
    {
        boss->changeState(State::DEAD);
        return;
    }

    static Timer minionTimer(5.0f);
    if (minionTimer.AutoActionIsReady(deltaTime))
    {
        boss->summonMinions(3);
    }
}


MegaBoss::IState* MegaBoss::DeadState::handle(const State& state)
{
    return nullptr;
}

void MegaBoss::DeadState::update(MegaBoss* boss, float deltaTime)
{
    boss->m_isInvulnerable = true;

    static float deathTimer = 0.0f;
    deathTimer += deltaTime;

    if (deathTimer > 3.0f)
    {
        boss->destroy();
    }
}

MegaBoss::MegaBoss(IComposite* scene, const sf::Vector2f& spawnPosition, BossMode mode, float maxHealth)
    : DestructibleObject(scene, maxHealth)
    , IComposite(scene)
    , m_maxLife(maxHealth)
    , m_speed(100.0f)
    , m_currentMode(mode)
    , m_offensiveBoostActive(false)
    , m_damageMultiplier(1.0f)
    , m_offensiveBoostTimer(0.0f)
    , m_invulnerabilityTimer(3.0f)
    , m_isInvulnerable(false)
    , m_isInvulnerableToCollisions(false)
    , m_currentPhase(BossPhase::One)
    , m_target(nullptr)
    , m_specialAttackSelector(nullptr)
    , m_worldPosition(spawnPosition)
    , m_movementAngle(0.0f)
    , m_detectionRadius(1200.0f)
    , m_isTrackingTarget(false)
    , m_attackTimer(2.0f)
    , m_patrolTimer(0.0f)
    , m_projectileCount(0)
    , m_maxProjectilesBeforeReload(5)
    , m_movementPattern(MovementPattern::Horizontal)
    , m_currentState(new PatrolState())
    , m_animate({ "HeroF.png" })
    , m_animationTimer(0.2f)
{
    m_bossParams = BossParameters::getForPhase(m_currentPhase, m_currentMode);
    m_speed = m_bossParams.speed;
    m_isInvulnerableToCollisions = m_bossParams.invulnerableToCollisions;

    sf::Vector2f screenPosition = worldToScreenPosition(m_worldPosition);
    m_shape = new RectangleSFML(200.0f, 200.0f, screenPosition);

    findTarget();
    if (!m_target) {
        IShapeSFML* defaultTarget = new SquareSFML(1, m_scene->getRoot()->getScene()->GetCenterWindow());
        m_target = defaultTarget;
    }

    m_animationComponent = new AnimationComponent(this);
    setupAnimations();

    setScale(5.0f, 5.0f);

    m_animationComponent->updatePosition(screenPosition);

    m_animationComponent->playAnimation("patrol");

    createWeapons(70.0f);

    new Life(this, this, Color::Pink);
}


MegaBoss::~MegaBoss()
{
    delete m_currentState;
}

void MegaBoss::setupAnimations()
{
    const int FRAME_WIDTH = 64;
    const int FRAME_HEIGHT = 64;

    const int IDLE_FRAMES = 6;
    const int MOVE_FRAMES = 5;
    const int ATTACK_FRAMES = 6;

    Animation idleAnimDown("HeroF.png", IDLE_FRAMES, 100);
    idleAnimDown.setFrameSize(sf::Vector2i(FRAME_WIDTH, FRAME_HEIGHT));
    idleAnimDown.setStartPosition(sf::Vector2i(0, 0 * FRAME_HEIGHT), 1);

    Animation idleAnimUp("HeroF.png", IDLE_FRAMES, 100);
    idleAnimUp.setFrameSize(sf::Vector2i(FRAME_WIDTH, FRAME_HEIGHT));
    idleAnimUp.setStartPosition(sf::Vector2i(0, 3 * FRAME_HEIGHT), 1);

    Animation idleAnimLeft("HeroF.png", IDLE_FRAMES, 100);
    idleAnimLeft.setFrameSize(sf::Vector2i(FRAME_WIDTH, FRAME_HEIGHT));
    idleAnimLeft.setStartPosition(sf::Vector2i(0, 6 * FRAME_HEIGHT), 1);

    Animation idleAnimRight("HeroF.png", IDLE_FRAMES, 100);
    idleAnimRight.setFrameSize(sf::Vector2i(FRAME_WIDTH, FRAME_HEIGHT));
    idleAnimRight.setStartPosition(sf::Vector2i(0, 9 * FRAME_HEIGHT), 1);

    Animation moveAnimDown("HeroF.png", MOVE_FRAMES, 100);
    moveAnimDown.setFrameSize(sf::Vector2i(FRAME_WIDTH, FRAME_HEIGHT));
    moveAnimDown.setStartPosition(sf::Vector2i(0, 1 * FRAME_HEIGHT), 1);

    Animation moveAnimUp("HeroF.png", MOVE_FRAMES, 100);
    moveAnimUp.setFrameSize(sf::Vector2i(FRAME_WIDTH, FRAME_HEIGHT));
    moveAnimUp.setStartPosition(sf::Vector2i(0, 4 * FRAME_HEIGHT), 1);

    Animation moveAnimLeft("HeroF.png", MOVE_FRAMES, 100);
    moveAnimLeft.setFrameSize(sf::Vector2i(FRAME_WIDTH, FRAME_HEIGHT));
    moveAnimLeft.setStartPosition(sf::Vector2i(0, 7 * FRAME_HEIGHT), 1);

    Animation moveAnimRight("HeroF.png", MOVE_FRAMES, 100);
    moveAnimRight.setFrameSize(sf::Vector2i(FRAME_WIDTH, FRAME_HEIGHT));
    moveAnimRight.setStartPosition(sf::Vector2i(0, 10 * FRAME_HEIGHT), 1);

    Animation attackAnimDown("HeroF.png", ATTACK_FRAMES, 100, false);
    attackAnimDown.setFrameSize(sf::Vector2i(FRAME_WIDTH, FRAME_HEIGHT));
    attackAnimDown.setStartPosition(sf::Vector2i(0, 2 * FRAME_HEIGHT), 1);

    Animation attackAnimUp("HeroF.png", ATTACK_FRAMES, 100, false);
    attackAnimUp.setFrameSize(sf::Vector2i(FRAME_WIDTH, FRAME_HEIGHT));
    attackAnimUp.setStartPosition(sf::Vector2i(0, 5 * FRAME_HEIGHT), 1);

    Animation attackAnimLeft("HeroF.png", ATTACK_FRAMES, 100, false);
    attackAnimLeft.setFrameSize(sf::Vector2i(FRAME_WIDTH, FRAME_HEIGHT));
    attackAnimLeft.setStartPosition(sf::Vector2i(0, 8 * FRAME_HEIGHT), 1);

    Animation attackAnimRight("HeroF.png", ATTACK_FRAMES, 100, false);
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

void MegaBoss::createWeapons(float weaponOffset)
{
    auto weapon1 = new FixTurret(this, getShape(), sf::Vector2f(-weaponOffset, -weaponOffset / 2), 0);
    auto weapon2 = new FixTurret(this, getShape(), sf::Vector2f(weaponOffset, -weaponOffset / 2), 0);
    auto weapon3 = new FixTurret(this, getShape(), sf::Vector2f(-weaponOffset, weaponOffset / 2), 0);
    auto weapon4 = new FixTurret(this, getShape(), sf::Vector2f(weaponOffset, weaponOffset / 2), 0);
    auto weapon5 = new FixTurret(this, getShape(), sf::Vector2f(0, -weaponOffset), 0);
    auto weapon6 = new FixTurret(this, getShape(), sf::Vector2f(0, weaponOffset), 0);

    m_weapons = { weapon1, weapon2, weapon3, weapon4, weapon5, weapon6 };

    for (auto& weapon : m_weapons) {
        weapon->SetFireRate(m_bossParams.attackRate);
        weapon->SetOverloadGun(5.0f, 10);
        weapon->setBullet(m_bossParams.projectileSize, m_bossParams.projectileSpeed, m_damageMultiplier);
    }
}

sf::Vector2f MegaBoss::worldToScreenPosition(const sf::Vector2f& worldPos) const
{
    return sf::Vector2f(
        m_scene->getRoot()->getScene()->getBackgroundCenter().x + worldPos.x,
        m_scene->getRoot()->getScene()->getBackgroundCenter().y + worldPos.y
    );
}

sf::Vector2f MegaBoss::screenToWorldPosition(const sf::Vector2f& screenPos) const
{
    return sf::Vector2f(
        screenPos.x - m_scene->getRoot()->getScene()->getBackgroundCenter().x,
        screenPos.y - m_scene->getRoot()->getScene()->getBackgroundCenter().y
    );
}


void MegaBoss::Update(const float& deltaTime)
{
    static Timer targetSearchTimer(1.0f);

    if (targetSearchTimer.AutoActionIsReady(deltaTime)) {
        findTarget();
    }

    if (!m_target) {
        if (!(dynamic_cast<PatrolState*>(m_currentState))) {
            changeState(State::PATROL);
        }

        patrol();

        if (m_currentState) {
            m_currentState->update(this, deltaTime);
        }
        m_attackTimer.NextTIck(deltaTime);

        sf::Vector2f screenPos = worldToScreenPosition(m_worldPosition);
        m_shape->setPosition(screenPos);
        m_animationComponent->updatePosition(screenPos);
        m_animationComponent->Update(deltaTime);

        IComposite::Update(deltaTime);
        return;
    }

    if (m_isInvulnerable)
    {
        m_invulnerabilityTimer.NextTIck(deltaTime);
        if (m_invulnerabilityTimer.ActionIsReady())
        {
            m_isInvulnerable = false;
            m_invulnerabilityTimer.resetTimer();
        }
    }

    if (!m_canEnterProtectState)
    {
        m_protectStateCooldown.NextTIck(deltaTime);
        if (m_protectStateCooldown.ActionIsReady())
        {
            m_canEnterProtectState = true;
        }
    }

    sf::Vector2f screenPos = worldToScreenPosition(m_worldPosition);
    m_shape->setPosition(screenPos);
    m_animationComponent->updatePosition(screenPos);
    m_animationComponent->Update(deltaTime);

    if (m_isTrackingTarget && m_target)
    {
        float angleToTarget = calculateAngleToTarget();
        m_shape->setRotation(angleToTarget);

        if (m_currentState && dynamic_cast<FireState*>(m_currentState))
        {
            if (m_attackTimer.ActionIsReady())
            {
                fireProjectiles(m_bossParams.attackCount, m_bossParams.spreadAngle);
                m_attackTimer.setNewTimer(m_bossParams.attackRate);
                m_attackTimer.resetTimer();
            }
        }
    }
    else
    {
        m_patrolTimer += deltaTime;

        if (m_currentMode == BossMode::Combined)
        {
            float radius = 300.0f;
            sf::Vector2f patrolPos;
            patrolPos.x = sin(m_patrolTimer * 0.5f) * radius;
            patrolPos.y = cos(m_patrolTimer * 0.25f) * radius;
            moveToPosition(patrolPos);
        }
    }

    if (m_currentState)
    {
        m_currentState->update(this, deltaTime);
    }

    m_attackTimer.NextTIck(deltaTime);
    m_isTrackingTarget = m_target && isTargetInDetectionZone();

    if (m_offensiveBoostActive)
    {
        m_offensiveBoostTimer.NextTIck(deltaTime);
        if (m_offensiveBoostTimer.ActionIsReady())
        {
            m_offensiveBoostActive = false;
            m_damageMultiplier = 1.0f;

            for (auto& weapon : m_weapons)
            {
                weapon->setBullet(m_bossParams.projectileSize, m_bossParams.projectileSpeed, 1.0f);
            }
        }
    }

    m_shape->setPosition(worldToScreenPosition(m_worldPosition));

    IComposite::Update(deltaTime);
}

void MegaBoss::Render()
{
    m_animationComponent->Render();
    IComposite::Render();
}

void MegaBoss::HandleCollision(IGameObject* object)
{
    if (object->globalGameObjectType() != GameObjectType::DestructibleObject)
        return;

    IBullet* bullet = getObj<IBullet*>(object);
    if (bullet) {
        for (auto& weapon : m_weapons) {
            if (bullet->getTurret() == weapon) {
                return;
            }
        }
    }
    else if (m_isInvulnerableToCollisions) {
        return;
    }

    if (m_isInvulnerable)
        return;

    ChangeLife(-1.0f);

    m_isInvulnerable = true;
    m_invulnerabilityTimer.setNewTimer(0.5f);
    m_invulnerabilityTimer.resetTimer();
}

void MegaBoss::ChangeLife(const float& life)
{
    if (life < 0 && m_isInvulnerable)
        return;

    DestructibleObject::ChangeLife(life);

    float healthPercentage = getCurrentLife() / m_maxLife;
    BossPhase newPhase;

    if (healthPercentage <= 0.2f) newPhase = BossPhase::Five;
    else if (healthPercentage <= 0.4f) newPhase = BossPhase::Four;
    else if (healthPercentage <= 0.6f) newPhase = BossPhase::Three;
    else if (healthPercentage <= 0.8f) newPhase = BossPhase::Two;
    else newPhase = BossPhase::One;

    if (newPhase != m_currentPhase) {
        m_currentPhase = newPhase;
        updateParameters();
    }
}

void MegaBoss::moveTowards(const sf::Vector2f& target, float deltaTime)
{
    sf::Vector2f targetWorldPos = screenToWorldPosition(target);

    sf::Vector2f direction = targetWorldPos - m_worldPosition;
    float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);

    if (length > 0) {
        direction /= length;
    }

    sf::Vector2f movement = direction * m_speed * deltaTime;
    m_worldPosition += movement;

    if (length > 0) {
        float angle = std::atan2(direction.y, direction.x) * 180 / 3.14159f;
        m_shape->setRotation(angle);
    }
}

void MegaBoss::moveToPosition(const sf::Vector2f& position)
{
    sf::Vector2f direction = position - m_worldPosition;
    float distance = sqrt(direction.x * direction.x + direction.y * direction.y);

    if (distance > 5.0f) {
        direction /= distance;
        m_movementAngle = atan2(direction.y, direction.x) * 180.0f / 3.14159f;

        float deltaTime = 1.0f / 60.0f;
        sf::Vector2f movement = direction * m_speed * deltaTime;
        m_worldPosition += movement;

        if (m_isTrackingTarget && m_target) {
            float targetAngle = calculateAngleToTarget();
            m_shape->setRotation(targetAngle);
        }
        else {
            m_shape->setRotation(m_movementAngle);
        }
    }
    else if (m_isTrackingTarget && m_target) {
        float targetAngle = calculateAngleToTarget();
        m_shape->setRotation(targetAngle);
    }
}

void MegaBoss::move(const sf::Vector2f& offset)
{
    float maxSpeedPerFrame = 0.50f;

    sf::Vector2f clampedOffset = offset;
    float offsetLength = std::sqrt(offset.x * offset.x + offset.y * offset.y);
    if (offsetLength > maxSpeedPerFrame)
    {
        clampedOffset = (offset / offsetLength) * maxSpeedPerFrame;
    }

    m_worldPosition += clampedOffset;
    sf::Vector2f screenPos = worldToScreenPosition(m_worldPosition);
    m_shape->setPosition(screenPos);
}

void MegaBoss::patrol()
{
    static bool movingRight = true;
    static bool movingDown = true;

    switch (m_currentMode)
    {
    case BossMode::Type1:
        if (movingRight)
        {
            move(sf::Vector2f(-1059, 0));
            if (getPosition().x <= 1400)
            {
                movingRight = false;
            }
        }
        else
        {
            move(sf::Vector2f(1059, 0));
            if (getPosition().x >= 1759)
            {
                movingRight = true;
            }
        }
        break;

    case BossMode::Type2:
    {
        float radius = 200.0f;
        float angle = m_patrolTimer * 0.5f;
        sf::Vector2f movement(
            cos(angle) * radius * 0.01f,
            sin(angle) * radius * 0.01f
        );
        move(movement);
    }
    break;

    case BossMode::Type3:
        if (movingRight)
        {
            move(sf::Vector2f(0.5f, movingDown ? 0.3f : -0.3f));
            if (getPosition().x >= 1800)
            {
                movingRight = false;
            }
        }
        else
        {
            move(sf::Vector2f(-0.5f, movingDown ? 0.3f : -0.3f));
            if (getPosition().x <= 1300)
            {
                movingRight = true;
            }
        }

        if (getPosition().y >= 1000)
            movingDown = false;
        else if (getPosition().y <= 500)
            movingDown = true;
        break;

    case BossMode::Combined:
    {
        float radius = 250.0f;
        float angle = m_patrolTimer * 0.4f;
        float verticalOffset = sin(m_patrolTimer * 0.2f) * 150.0f;

        sf::Vector2f movement(
            cos(angle) * radius * 0.01f,
            sin(angle) * radius * 0.01f + verticalOffset * 0.005f
        );
        move(movement);
    }
    break;
    }
}

float MegaBoss::calculateAngleToTarget() const
{
    if (!m_target) return m_movementAngle;

    try {
        sf::Vector2f targetPos = m_target->getPosition();
        sf::Vector2f myPos = m_shape->getPosition();

        if (std::isnan(targetPos.x) || std::isnan(targetPos.y) ||
            std::isinf(targetPos.x) || std::isinf(targetPos.y) ||
            std::isnan(myPos.x) || std::isnan(myPos.y) ||
            std::isinf(myPos.x) || std::isinf(myPos.y)) {
            return m_movementAngle;
        }

        sf::Vector2f direction = targetPos - myPos;
        return std::atan2(direction.y, direction.x) * 180.0f / 3.14159f;
    }
    catch (...) {
        return m_movementAngle;
    }
}

bool MegaBoss::isTargetValid() const
{
    if (!m_target) return false;

    try {
        sf::Vector2f pos = m_target->getPosition();

        if (std::isnan(pos.x) || std::isnan(pos.y) ||
            std::isinf(pos.x) || std::isinf(pos.y)) {
            return false;
        }

        return true;
    }
    catch (...) {
        return false;
    }
}

bool MegaBoss::isTargetInDetectionZone() const
{
    if (!m_target) return false;

    try {
        sf::Vector2f targetWorldPos = screenToWorldPosition(m_target->getPosition());
        sf::Vector2f diff = targetWorldPos - m_worldPosition;
        float distanceSquared = diff.x * diff.x + diff.y * diff.y;
        return distanceSquared <= m_detectionRadius * m_detectionRadius;
    }
    catch (...) {
        return false;
    }
}

bool MegaBoss::shouldAttackTarget() const
{
    return m_target && isTargetInDetectionZone() && const_cast<Timer&>(m_attackTimer).ActionIsReady();
}

void MegaBoss::fireProjectiles(int count, float spreadAngle)
{
    if (!m_target) {
        float angle = m_shape->getangle();

        for (auto& weapon : m_weapons) {
            weapon->SetOverloadGun(0.1f, count);
            weapon->setBullet(
                m_bossParams.projectileSize,
                m_bossParams.projectileSpeed,
                m_damageMultiplier
            );

            weapon->getShape()->setRotation(angle);

            if (count == 1) {
                weapon->Fire();
            }
            else {
                float totalSpread = spreadAngle;
                float angleStep = totalSpread / (count - 1);
                float startAngle = angle - totalSpread / 2.0f;

                for (int i = 0; i < count; ++i) {
                    float bulletAngle = startAngle + i * angleStep;
                    weapon->getShape()->setRotation(bulletAngle);
                    weapon->Fire();
                }

                weapon->getShape()->setRotation(angle);
            }
        }
        return;
    }

    m_projectileCount += count;
    if (m_projectileCount >= m_maxProjectilesBeforeReload) {
        changeState(State::LOAD);
        m_projectileCount = 0;
    }
}

void MegaBoss::fireSpecialProjectile(ProjectileType type)
{
    switch (type) {
    case ProjectileType::Large:
    {
        int firstWeapon = UseRandomNumber().getRandomNumber<int>(0, m_weapons.size() - 1);
        int secondWeapon = (firstWeapon + 2) % m_weapons.size();

        float angleToTarget = m_target ? calculateAngleToTarget() : m_shape->getangle();

        new LargeProjectile(
            m_scene,
            m_weapons[firstWeapon],
            angleToTarget,
            450.0f,
            60.0f,
            3.0f
        );

        new LargeProjectile(
            m_scene,
            m_weapons[secondWeapon],
            angleToTarget,
            450.0f,
            60.0f,
            3.0f
        );
    }
    break;

    case ProjectileType::Fast:
    {
        AnimateSprite fastProjectileSprite({ "FastBullet.png" });
        for (auto& weapon : m_weapons) {
            new FastProjectile(
                fastProjectileSprite,
                m_scene,
                weapon,
                weapon->getShape()->getangle(),
                1200.0f,
                10.0f,
                1.0f
            );
        }
    }
    break;
    }
}

void MegaBoss::fireGrowingProjectile()
{
    fireSpecialProjectile(ProjectileType::Large);
}

void MegaBoss::fireFastProjectile()
{
    fireSpecialProjectile(ProjectileType::Fast);
}

void MegaBoss::fireCircularPattern()
{
    float baseAngle = UseRandomNumber().getRandomNumber<int>(0, 360);

    for (int i = 0; i < 12; i++) {
        float angle = baseAngle + (i * 30.0f);

        for (auto& weapon : m_weapons) {
            weapon->getShape()->setRotation(angle);
            weapon->Fire();
        }
    }

    if (m_currentPhase >= BossPhase::Four) {
        float secondaryBaseAngle = baseAngle + 15.0f;
        for (int i = 0; i < 12; i++) {
            float angle = secondaryBaseAngle + (i * 30.0f);
            for (size_t j = 0; j < m_weapons.size(); j += 2) {
                m_weapons[j]->getShape()->setRotation(angle);
                m_weapons[j]->Fire();
            }
        }
    }
}

void MegaBoss::fireWavePattern()
{
    const int WAVE_COUNT = 3;
    const float WAVE_DELAY = 0.2f;

    for (int i = 0; i < WAVE_COUNT; i++) {
        for (auto& weapon : m_weapons) {
            float baseAngle = calculateAngleToTarget();
            float waveSpread = 15.0f * (i + 1);

            weapon->getShape()->setRotation(baseAngle - waveSpread);
            weapon->Fire();

            weapon->getShape()->setRotation(baseAngle);
            weapon->Fire();

            weapon->getShape()->setRotation(baseAngle + waveSpread);
            weapon->Fire();
        }
    }
}

void MegaBoss::activateDefensiveAbility(float duration)
{
    m_isInvulnerable = true;
    m_invulnerabilityTimer.setNewTimer(duration);
    m_invulnerabilityTimer.resetTimer();

    regenerateHealth(m_maxLife * 0.05f);
}

void MegaBoss::activateOffensiveBoost(float multiplier, float duration)
{
    m_offensiveBoostActive = true;
    m_damageMultiplier = multiplier;
    m_offensiveBoostTimer.setNewTimer(duration);

    for (auto& weapon : m_weapons) {
        weapon->setBullet(m_bossParams.projectileSize, m_bossParams.projectileSpeed, m_damageMultiplier);
    }
}

void MegaBoss::regenerateHealth(float amount)
{
    float newHealth = m_life + amount;
    if (newHealth > m_maxLife) {
        newHealth = m_maxLife;
    }
    m_life = newHealth;
}

void MegaBoss::summonMinions(int count)
{
}

void MegaBoss::updateParameters()
{
    m_bossParams = BossParameters::getForPhase(m_currentPhase, m_currentMode);
    m_speed = m_bossParams.speed;
    m_isInvulnerableToCollisions = m_bossParams.invulnerableToCollisions;

    m_attackTimer.setNewTimer(m_bossParams.attackRate);

    for (auto& weapon : m_weapons) {
        weapon->SetFireRate(m_bossParams.attackRate);
        weapon->setBullet(m_bossParams.projectileSize, m_bossParams.projectileSpeed, m_damageMultiplier);
    }
}

void MegaBoss::setMode(BossMode mode)
{
    m_currentMode = mode;
    updateParameters();
}

void MegaBoss::findTarget()
{
    auto root = m_scene->getRoot();
    IShapeSFML* oldTarget = m_target;
    m_target = nullptr;

    std::function<bool(IComposite*)> findShipInChildren = [&](IComposite* node) -> bool {
        if (!node) return false;

        for (auto& child : node->getChildren()) {
            if (!child) continue;

            Hero* ship = getObj<Hero*>(child);
            if (ship) {
                m_target = ship->getShape();
                return true;
            }

            if (auto composite = dynamic_cast<IComposite*>(child)) {
                if (findShipInChildren(composite)) return true;
            }
        }
        return false;
        };

    for (auto& node : root->getChildren()) {
        if (!node) continue;

        Hero* ship = getObj<Hero*>(node);
        if (ship) {
            m_target = ship->getShape();
            return;
        }
    }

    findShipInChildren(root);

    if (!m_target && oldTarget) {
        changeState(State::PATROL);
    }
}

void MegaBoss::changeState(const State& newState)
{
    if (newState == State::PROTECT &&
        (!m_canEnterProtectState || m_protectionUsesRemaining <= 0))
    {
        changeState(State::CHASE);
        return;
    }

    if (newState == State::PROTECT && m_canEnterProtectState && m_protectionUsesRemaining > 0)
    {
        m_protectionUsesRemaining--;
    }

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

std::string MegaBoss::getOrientationString() const
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

Orientation MegaBoss::determineOrientation(float angle)
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

void MegaBoss::setScale(float scaleX, float scaleY)
{
    m_animationComponent->getSprite().setScale(scaleX, scaleY);
}
