#include "Boss1.h"
#include "RandomNumber.h"
#include "Ship.h"
#include <functional>
#include <iostream>

EntityParameters3 EntityParameters3::getForPhase(EntityPhase3 phase)
{
    EntityParameters3 params;

    switch (phase)
    {
    case EntityPhase3::One:
        params = { 350.0f, 2.0f, 1, 0.0f, 20.0f, 500.0f, 1.0f };
        break;
    case EntityPhase3::Two:
        params = { 450.0f, 1.8f, 2, 0.0f, 20.0f, 600.0f, 1.0f };
        break;
    case EntityPhase3::Three:
        params = { 550.0f, 1.5f, 3, 0.0f, 25.0f, 700.0f, 1.0f };
        break;
    case EntityPhase3::Four:
        params = { 650.0f, 1.2f, 4, 0.0f, 30.0f, 800.0f, 1.0f };
        break;
    case EntityPhase3::Five:
        params = { 750.0f, 0.9f, 5, 0.0f, 35.0f, 900.0f, 1.2f };
        break;
    }

    return params;
}

// Patrol State
Boss3::IState* Boss3::PatrolState::handle(const State& state)
{
    if (state == State::CHASE)
    {
        return new ChaseState();
    }

    if (state == State::RELOAD)
    {
        return new ReloadState();
    }
    if (state == State::FIRE)
    {
        return new FireState();
    }

    return nullptr;
}

void Boss3::PatrolState::update(Boss3* boss, float deltaTime)
{
    sf::Vector2f direction = boss->m_target->getPosition() - boss->getShape()->getPosition();
    float distance = std::sqrt(direction.x * direction.x + direction.y * direction.y);

    if (direction.x == 0 && direction.y == 0) 
    {
        return;
    }

    if (distance < 650.0f)
    {
        boss->changeState(State::CHASE);
        return;
    }



}
///// Chase State
Boss3::IState* Boss3::ChaseState::handle(const State& state)
{
    if (state == State::PATROL)
    {
        return new PatrolState();
    }

    if (state == State::RELOAD)
    {
        return new ReloadState();
    }

    if (state == State::FIRE)
    {
        return new FireState();
    }


    return nullptr;
}

void Boss3::ChaseState::update(Boss3* boss, float deltaTime)
{
    sf::Vector2f direction = boss->m_target->getPosition() - boss->getShape()->getPosition();
    float distance = std::sqrt(direction.x * direction.x + direction.y * direction.y);

    if (distance >= 650.0f)
    {
        boss->changeState(State::PATROL);
        return;
    }


    if (distance <= 300.0f)
    {
        boss->changeState(State::FIRE);
        return;
    }

    if (distance > 0)
    {
        direction /= distance;
        boss->move(direction * boss->getSpeed() * deltaTime);
    }
    else
        return;

}

///// Reload State
Boss3::IState* Boss3::ReloadState::handle(const State& state)
{
    if (state == State::PATROL)
    {
        return new PatrolState();
    }

    if (state == State::CHASE)
    {
        return new ChaseState();
    }

    if (state == State::FIRE)
    {
        return new FireState();
    }

    return nullptr;
}

void Boss3::ReloadState::update(Boss3* boss, float deltaTime)
{
    sf::Vector2f direction = boss->m_target->getPosition() - boss->getShape()->getPosition();
    float distance = std::sqrt(direction.x * direction.x + direction.y * direction.y);

    static float reloadTime = 3.0f; 
    static float reloadTimer = 0.0f;

    reloadTimer += deltaTime;

    if (boss->m_projectileCount >= boss->m_maxProjectilesBeforeReload)
    {
        boss->m_projectileCount = 0;

    }

    if (distance >= 650.0f)
    {
        boss->changeState(State::PATROL);
        return;
    }

    if (distance < 650.0f)
    {
        boss->changeState(State::CHASE);
        return;
    }

    if (distance <= 300.0f)
    {
        boss->changeState(State::FIRE);
        return;
    }

    if (distance > 0)
    {
        direction /= distance;
        boss->move(direction * boss->getSpeed() * deltaTime);
    }
    else
        return;

}

///// Fire State
Boss3::IState* Boss3::FireState::handle(const State& state)
{
    if (state == State::PATROL)
    {
        return new PatrolState();
    }

    if (state == State::CHASE)
    {
        return new ChaseState();
    }

    if (state == State::RELOAD)
    {
        return new ReloadState();
    }



    return nullptr;
}

void Boss3::FireState::update(Boss3* boss, float deltaTime)
{
    sf::Vector2f direction = boss->m_target->getPosition() - boss->getShape()->getPosition();
    float distance = std::sqrt(direction.x * direction.x + direction.y * direction.y);

    if (boss->m_projectileCount == 0)
    {
        boss->m_projectileCount = 1;
    }

    else
    {
        boss->m_projectileCount++;
    }

    if (boss->m_projectileCount >= boss->m_maxProjectilesBeforeReload)
    {
        boss->changeState(State::RELOAD);
        return;
    }

    if (distance > 300.0f)
    {
        boss->changeState(State::CHASE);
        return;
    }


    if (distance >= 650.0f)
    {
        boss->changeState(State::PATROL);
        return;
    }

    if (distance > 0)
    {
        direction /= distance;
        boss->move(direction * boss->getSpeed() * deltaTime);
    }
    else
        return;

}

Boss3::Boss3(IComposite* scene, const sf::Vector2f& spawnPosition, float maxHealth)
    : DestructibleObject(scene, maxHealth)
    , IComposite(scene)
    , m_maxLife(maxHealth)
    , m_speed(100.0f)
    , m_animate({ "Boss1.png" })
    , m_animationTimer(0.3)
    , m_offensiveBoostActive(false)
    , m_damageMultiplier(1.0f)
    , m_offensiveBoostTimer(0.0f)
    , m_invulnerabilityTimer(3.0f)
    , m_isInvulnerable(false)
    , m_currentPhase(EntityPhase3::One)
    , m_target(nullptr)
    , m_specialAttackSelector(nullptr)
    , m_worldPosition(spawnPosition)
    , m_movementAngle(0.0f)
    , m_detectionRadius(1200.0f)
    , m_isTrackingTarget(false)
    , m_attackTimer(2.0f)
    , m_patrolTimer(0.0f)
    , m_currentState(new PatrolState())
{
    m_entityParams = EntityParameters3::getForPhase(m_currentPhase);

    sf::Vector2f screenPosition = worldToScreenPosition(m_worldPosition);
    m_shape = new RectangleSFML(200.0f, 200.0f, screenPosition);
    m_shape->setTexture(m_scene->getRoot()->getScene()->getTexture()->getTexture(m_animate.getCurrentPath()));

    findTarget();

    if (!m_target) {
        IShapeSFML* defaultTarget = new SquareSFML(1, m_scene->getRoot()->getScene()->GetCenterWindow());
        m_target = defaultTarget;
    }

    createWeapons(70.0f);

    new Life(this, this, Color::Pink);

}

Boss3::~Boss3()
{
   
}

void Boss3::createWeapons(float weaponOffset) {
    auto weapon1 = new FixTurret(this, getShape(), sf::Vector2f(-weaponOffset/3, -weaponOffset/3), 0);
    auto weapon2 = new FixTurret(this, getShape(), sf::Vector2f(weaponOffset/2, -weaponOffset/2), 0);
    auto weapon3 = new FixTurret(this, getShape(), sf::Vector2f(-weaponOffset/3, weaponOffset/3), 0);
    auto weapon4 = new FixTurret(this, getShape(), sf::Vector2f(weaponOffset/2, weaponOffset/2), 0);

    m_weapons = { weapon1, weapon2, weapon3, weapon4 };

    for (auto& weapon : m_weapons) {
        weapon->SetFireRate(m_entityParams.attackRate);
        weapon->SetOverloadGun(5.0f, 5);
        weapon->setBullet(m_entityParams.projectileSize, m_entityParams.projectileSpeed, m_damageMultiplier);
    }
}

sf::Vector2f Boss3::worldToScreenPosition(const sf::Vector2f& worldPos) const {
    return sf::Vector2f(
        m_scene->getRoot()->getScene()->getBackgroundCenter().x + worldPos.x,
        m_scene->getRoot()->getScene()->getBackgroundCenter().y + worldPos.y
    );
}

sf::Vector2f Boss3::screenToWorldPosition(const sf::Vector2f& screenPos) const {
    return sf::Vector2f(
        screenPos.x - m_scene->getRoot()->getScene()->getBackgroundCenter().x,
        screenPos.y - m_scene->getRoot()->getScene()->getBackgroundCenter().y
    );
}

float Boss3::calculateAngleToTarget() const
{
    if (!m_target) return m_movementAngle;

    try {
        sf::Vector2f targetWorldPos = screenToWorldPosition(m_target->getPosition());

        float xCorrection = 50.0f * static_cast<float>(m_currentPhase) / 3.0f;
        targetWorldPos.x += xCorrection;

        sf::Vector2f direction = targetWorldPos - m_worldPosition;
        return atan2(direction.y, direction.x) * 180.0f / 3.14159f;
    }
    catch (...) {
        return m_movementAngle;
    }
}

bool Boss3::isTargetValid() const
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

bool Boss3::isTargetInDetectionZone() const
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

bool Boss3::shouldAttackTarget() const
{
    return m_target && isTargetInDetectionZone() && const_cast<Timer&>(m_attackTimer).ActionIsReady();
}

void Boss3::moveToPosition(const sf::Vector2f& position)
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
        else if (m_target) {
            sf::Vector2f targetWorldPos = screenToWorldPosition(m_target->getPosition());
            sf::Vector2f targetDir = targetWorldPos - m_worldPosition;
            float targetAngle = atan2(targetDir.y, targetDir.x) * 180.0f / 3.14159f;

            float blendedAngle = m_movementAngle * 0.7f + targetAngle * 0.3f;
            m_shape->setRotation(blendedAngle);
        }
        else 
        {
            m_shape->setRotation(m_movementAngle);
        }
    }
    else if (m_isTrackingTarget && m_target) {
        float targetAngle = calculateAngleToTarget();
        m_shape->setRotation(targetAngle);
    }
}

void Boss3::Update(const float& deltaTime)
{

    static Timer targetSearchTimer(2.0f);

    if (!m_target || targetSearchTimer.AutoActionIsReady(deltaTime)) 
    {
        findTarget();
    }

    if (m_isInvulnerable) 
    {
        m_invulnerabilityTimer.NextTIck(deltaTime);
        if (m_invulnerabilityTimer.ActionIsReady()) 
        {
            m_isInvulnerable = false;
        }
    }

    if (m_animationTimer.AutoActionIsReady(m_scene->getRoot()->getScene()->getRefreshTime().asSeconds())) {
        m_animate.ChangeToNextPath();
        m_shape->setTexture(m_scene->getRoot()->getScene()->getTexture()->getTexture(m_animate.getCurrentPath()));
    }

    if (m_isTrackingTarget && m_target)
    {
        float angleToTarget = calculateAngleToTarget();
        m_shape->setRotation(angleToTarget);

        if (m_currentState && dynamic_cast<FireState*>(m_currentState))
        {
            if (m_attackTimer.ActionIsReady())
            {
                fireProjectiles(m_entityParams.attackCount, m_entityParams.spreadAngle);
                m_attackTimer.setNewTimer(m_entityParams.attackRate);
            }
        }
    }
    else
    {
        m_patrolTimer += deltaTime;

        float radius = 200.0f;
        sf::Vector2f patrolPos;
        patrolPos.x = sin(m_patrolTimer * 0.5f) * radius;
        patrolPos.y = cos(m_patrolTimer * 0.25f) * radius;
        moveToPosition(patrolPos);
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
                weapon->setBullet(m_entityParams.projectileSize, m_entityParams.projectileSpeed, 1.0f);
            }
        }
    }

    m_shape->setPosition(worldToScreenPosition(m_worldPosition));

    IComposite::Update(deltaTime);
}

void Boss3::Render()
{
    m_scene->getRoot()->getScene()->getWindow()->draw(static_cast<RectangleSFML*>(m_shape)->getShape());
    IComposite::Render();
}

void Boss3::HandleCollision(IGameObject* object)
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

    if (m_isInvulnerable)
        return;

    ChangeLife(-1.0f);

    m_isInvulnerable = true;
    m_invulnerabilityTimer.setNewTimer(1.0f);
    m_invulnerabilityTimer.resetTimer();

}

void Boss3::ChangeLife(const float& life)
{
    if (life < 0 && m_isInvulnerable)
        return;

    DestructibleObject::ChangeLife(life);

    float healthPercentage = getCurrentLife() / m_maxLife;
    EntityPhase3 newPhase;

    if (healthPercentage <= 0.2f) newPhase = EntityPhase3::Five;
    else if (healthPercentage <= 0.4f) newPhase = EntityPhase3::Four;
    else if (healthPercentage <= 0.6f) newPhase = EntityPhase3::Three;
    else if (healthPercentage <= 0.8f) newPhase = EntityPhase3::Two;
    else newPhase = EntityPhase3::One;

    if (newPhase != m_currentPhase) {
        m_currentPhase = newPhase;
        updateEntityParameters();
    }
}

float Boss3::getSpeed()
{
    return m_speed;
}

void Boss3::setSpeed(float speed)
{
    m_speed = speed;
}

void Boss3::moveTowards(const sf::Vector2f& target, float deltaTime)
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

void Boss3::fireProjectiles(int count, float spreadAngle)
{
    if (!m_target || !m_isTrackingTarget) return;

    float targetingAngle;
    try {
        targetingAngle = calculateAngleToTarget();
    }
    catch (...) {
        targetingAngle = m_shape->getangle();
    }

    for (auto& weapon : m_weapons) {
        weapon->SetOverloadGun(0.1f, count);
        weapon->setBullet(
            m_entityParams.projectileSize,
            m_entityParams.projectileSpeed,
            m_damageMultiplier
        );

        weapon->getShape()->setRotation(targetingAngle);

        if (count == 1) {
            weapon->Fire();
        }
        else {
            float totalSpread = spreadAngle;
            float angleStep = totalSpread / (count - 1);
            float startAngle = targetingAngle - totalSpread / 2.0f;

            for (int i = 0; i < count; ++i) {
                float bulletAngle = startAngle + i * angleStep;
                weapon->getShape()->setRotation(bulletAngle);
                weapon->Fire();
            }

            weapon->getShape()->setRotation(targetingAngle);
        }
    }
}

void Boss3::fireSpecialProjectile(ProjectileType type)
{
    switch (type) {
    case ProjectileType::Large:
    {
        int firstWeapon = UseRandomNumber().getRandomNumber<int>(0, 3);
        int secondWeapon = (firstWeapon + 2) % 4;

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

void Boss3::activateDefensiveAbility(float duration)
{
    regenerateHealth(25.0f);
}

void Boss3::activateOffensiveBoost(float multiplier, float duration)
{
    m_offensiveBoostActive = true;
    m_damageMultiplier = multiplier;
    m_offensiveBoostTimer.setNewTimer(duration);

    for (auto& weapon : m_weapons) {
        weapon->setBullet(m_entityParams.projectileSize, m_entityParams.projectileSpeed, m_damageMultiplier);
    }
}

void Boss3::regenerateHealth(float amount)
{
    float newHealth = m_life + amount;
    if (newHealth > m_maxLife) {
        newHealth = m_maxLife;
    }
    m_life = newHealth;
}

void Boss3::updateEntityParameters()
{
    m_entityParams = EntityParameters3::getForPhase(m_currentPhase);
    m_speed = m_entityParams.speed;

    m_attackTimer.setNewTimer(m_entityParams.attackRate);

    for (auto& weapon : m_weapons) {
        weapon->SetFireRate(m_entityParams.attackRate);
        weapon->setBullet(m_entityParams.projectileSize, m_entityParams.projectileSpeed, m_damageMultiplier);
    }
}

void Boss3::findTarget()
{
    auto root = m_scene->getRoot();
    m_target = nullptr;

    std::function<bool(IComposite*)> findShipInChildren = [&](IComposite* node) -> bool {
        if (!node) return false;

        for (auto& child : node->getChildren()) {
            if (!child) continue;

            Ship* ship = getObj<Ship*>(child);
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

        Ship* ship = getObj<Ship*>(node);
        if (ship) {
            m_target = ship->getShape();
            return;
        }
    }

    findShipInChildren(root);
}

void Boss3::fireGrowingProjectile()
{
    fireSpecialProjectile(ProjectileType::Large);
}

void Boss3::fireFastProjectile()
{
    fireSpecialProjectile(ProjectileType::Fast);
}

void Boss3::move(const sf::Vector2f& offset)
{
    float maxSpeedPerFrame = 0.30f;

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

void Boss3::changeState(const State& newState)
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
