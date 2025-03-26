#include "Enemy.h"
#include "RandomNumber.h"
#include "Hero.h"
#include <cmath>
#include <functional>

Enemy::Enemy(IComposite * scene, const sf::Vector2f & spawnPosition, float maxHealth)
    : DestructibleObject(scene, maxHealth)
    , IComposite(scene)
    , m_maxLife(maxHealth)
    , m_speed(70.0f)
    , m_damage(25.0f)
    , m_attackRange(100.0f)
    , m_attackCooldown(2.0f)
    , m_isInvulnerable(false)
    , m_stunDuration(0.0f)
    , m_target(nullptr)
    , m_worldPosition(spawnPosition)
    , m_movementAngle(0.0f)
    , m_detectionRadius(500.0f)
    , m_patrolTimer(0.0f)
    , m_currentState(new PatrolState())
    , m_animate({ "HeroF.png" })
    , m_attackTimer(m_attackCooldown)
    , m_invulnerabilityTimer(0.5f)
    , m_currentOrientation(Orientation::DOWN)
{
    sf::Vector2f screenPosition = worldToScreenPosition(m_worldPosition);
    m_shape = new RectangleSFML(150.0f, 150.0f, screenPosition);

    findTarget();

    m_animationComponent = new AnimationComponent(this);
    setupAnimations();

    m_animationComponent->updatePosition(screenPosition);
    m_animationComponent->playAnimation("idle_down");

    new Life(this, this, Color::Pink);
}

Enemy::~Enemy()
{
    delete m_currentState;
}

void Enemy::setupAnimations()
{
    const int FRAME_WIDTH = 64;
    const int FRAME_HEIGHT = 64;

    const int IDLE_FRAMES = 6;
    const int MOVE_FRAMES = 5;
    const int ATTACK_FRAMES = 6;

    Animation idleAnimDown("ToujoursPasDeSpriteSheet.png", IDLE_FRAMES, 100);
    idleAnimDown.setFrameSize(sf::Vector2i(FRAME_WIDTH, FRAME_HEIGHT));
    idleAnimDown.setStartPosition(sf::Vector2i(0, 0 * FRAME_HEIGHT), 1);

    Animation idleAnimUp("ToujoursPasDeSpriteSheet.png", IDLE_FRAMES, 100);
    idleAnimUp.setFrameSize(sf::Vector2i(FRAME_WIDTH, FRAME_HEIGHT));
    idleAnimUp.setStartPosition(sf::Vector2i(0, 3 * FRAME_HEIGHT), 1);

    Animation idleAnimLeft("ToujoursPasDeSpriteSheet.png", IDLE_FRAMES, 100);
    idleAnimLeft.setFrameSize(sf::Vector2i(FRAME_WIDTH, FRAME_HEIGHT));
    idleAnimLeft.setStartPosition(sf::Vector2i(0, 6 * FRAME_HEIGHT), 1);

    Animation idleAnimRight("ToujoursPasDeSpriteSheet.png", IDLE_FRAMES, 100);
    idleAnimRight.setFrameSize(sf::Vector2i(FRAME_WIDTH, FRAME_HEIGHT));
    idleAnimRight.setStartPosition(sf::Vector2i(0, 9 * FRAME_HEIGHT), 1);

    Animation moveAnimDown("ToujoursPasDeSpriteSheet.png", MOVE_FRAMES, 100);
    moveAnimDown.setFrameSize(sf::Vector2i(FRAME_WIDTH, FRAME_HEIGHT));
    moveAnimDown.setStartPosition(sf::Vector2i(0, 1 * FRAME_HEIGHT), 1);

    Animation moveAnimUp("ToujoursPasDeSpriteSheet.png", MOVE_FRAMES, 100);
    moveAnimUp.setFrameSize(sf::Vector2i(FRAME_WIDTH, FRAME_HEIGHT));
    moveAnimUp.setStartPosition(sf::Vector2i(0, 4 * FRAME_HEIGHT), 1);

    Animation moveAnimLeft("ToujoursPasDeSpriteSheet.png", MOVE_FRAMES, 100);
    moveAnimLeft.setFrameSize(sf::Vector2i(FRAME_WIDTH, FRAME_HEIGHT));
    moveAnimLeft.setStartPosition(sf::Vector2i(0, 7 * FRAME_HEIGHT), 1);

    Animation moveAnimRight("ToujoursPasDeSpriteSheet.png", MOVE_FRAMES, 100);
    moveAnimRight.setFrameSize(sf::Vector2i(FRAME_WIDTH, FRAME_HEIGHT));
    moveAnimRight.setStartPosition(sf::Vector2i(0, 10 * FRAME_HEIGHT), 1);

    Animation attackAnimDown("ToujoursPasDeSpriteSheet.png", ATTACK_FRAMES, 100, false);
    attackAnimDown.setFrameSize(sf::Vector2i(FRAME_WIDTH, FRAME_HEIGHT));
    attackAnimDown.setStartPosition(sf::Vector2i(0, 2 * FRAME_HEIGHT), 1);

    Animation attackAnimUp("ToujoursPasDeSpriteSheet.png", ATTACK_FRAMES, 100, false);
    attackAnimUp.setFrameSize(sf::Vector2i(FRAME_WIDTH, FRAME_HEIGHT));
    attackAnimUp.setStartPosition(sf::Vector2i(0, 5 * FRAME_HEIGHT), 1);

    Animation attackAnimLeft("ToujoursPasDeSpriteSheet.png", ATTACK_FRAMES, 100, false);
    attackAnimLeft.setFrameSize(sf::Vector2i(FRAME_WIDTH, FRAME_HEIGHT));
    attackAnimLeft.setStartPosition(sf::Vector2i(0, 8 * FRAME_HEIGHT), 1);

    Animation attackAnimRight("ToujoursPasDeSpriteSheet.png", ATTACK_FRAMES, 100, false);
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

    m_animationComponent->getSprite().setScale(4.0f, 4.0f);
}

void Enemy::Update(const float& deltaTime)
{
    static Timer targetSearchTimer(1.0f);

    if (targetSearchTimer.AutoActionIsReady(deltaTime)) {
        findTarget();
    }

    if (m_isInvulnerable) {
        m_invulnerabilityTimer.NextTIck(deltaTime);
        if (m_invulnerabilityTimer.ActionIsReady()) {
            m_isInvulnerable = false;
            m_invulnerabilityTimer.resetTimer();
        }
    }

    m_patrolTimer += deltaTime;
    m_attackTimer.NextTIck(deltaTime);

    if (m_currentState) {
        m_currentState->update(this, deltaTime);
    }

    sf::Vector2f screenPos = worldToScreenPosition(m_worldPosition);
    m_shape->setPosition(screenPos);
    m_animationComponent->updatePosition(screenPos);
    m_animationComponent->Update(deltaTime);

    IComposite::Update(deltaTime);
}

void Enemy::Render()
{
    m_animationComponent->Render();
    IComposite::Render();
}

void Enemy::HandleCollision(IGameObject* object)
{
    if (m_isInvulnerable)
        return;

    if (object->globalGameObjectType() != GameObjectType::DestructibleObject)
        return;

    Hero* playerShip = getObj<Hero*>(object);
    if (playerShip) {
        return;
    }

    ChangeLife(-1.0f);

    m_isInvulnerable = true;
    m_invulnerabilityTimer.resetTimer();
}

void Enemy::ChangeLife(const float& life)
{
    if (life < 0 && m_isInvulnerable)
        return;

    DestructibleObject::ChangeLife(life);

    if (getCurrentLife() <= 0) {
        changeState(EnemyState::DEAD);
    }
    else if (life < -5.0f) {
        if (UseRandomNumber().getRandomNumber<int>(0, 10) > 7) {
            changeState(EnemyState::STUNNED);
        }
    }
}

void Enemy::findTarget() {
    auto root = m_scene->getRoot();
    m_target = nullptr;

    std::function<bool(IComposite*)> findShipInChildren = [&](IComposite* node) -> bool {
        if (!node) return false;

        for (auto& child : node->getChildren()) {
            if (!child) continue;

            Hero* ship = getObj<Hero*>(child);
            if (ship) {
                m_target = ship;
                return true;
            }

            if (auto composite = dynamic_cast<IComposite*>(child)) {
                if (findShipInChildren(composite)) return true;
            }
        }
        return false;
        };

    findShipInChildren(root);
}

void Enemy::changeState(const EnemyState& newState)
{
    IState* state = nullptr;

    switch (newState) {
    case EnemyState::PATROL:
        state = new PatrolState();
        break;
    case EnemyState::CHASE:
        state = new ChaseState();
        break;
    case EnemyState::ATTACK:
        state = new AttackState();
        break;
    case EnemyState::STUNNED:
        state = new StunnedState();
        break;
    case EnemyState::DEAD:
        state = new DeadState();
        break;
    }

    if (state) {
        delete m_currentState;
        m_currentState = state;
    }
}

void Enemy::patrol()
{
    static bool movingRight = true;
    static float patrolWidth = 400.0f;

    if (movingRight) {
        m_worldPosition.x += m_speed * 0.01f;
        if (m_worldPosition.x >= patrolWidth) {
            movingRight = false;
        }
    }
    else {
        m_worldPosition.x -= m_speed * 0.01f;
        if (m_worldPosition.x <= -patrolWidth) {
            movingRight = true;
        }
    }

    m_currentOrientation = movingRight ? Orientation::RIGHT : Orientation::LEFT;
}

void Enemy::moveTowards(const sf::Vector2f& target, float deltaTime)
{
    sf::Vector2f targetWorldPos = screenToWorldPosition(target);
    sf::Vector2f direction = targetWorldPos - m_worldPosition;
    float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);

    if (length > 0) {
        direction /= length;

        float angle = std::atan2(direction.y, direction.x);
        m_movementAngle = angle * 180.0f / 3.14159f;
        m_currentOrientation = determineOrientation(m_movementAngle);

        m_worldPosition += direction * m_speed * deltaTime;
    }
}

void Enemy::performMeleeAttack() {
    if (!m_target || !m_attackTimer.ActionIsReady())
        return;

    sf::Vector2f targetPos = m_target->getShape()->getPosition();
    sf::Vector2f myPos = worldToScreenPosition(m_worldPosition);

    sf::Vector2f diff = targetPos - myPos;
    float distance = std::sqrt(diff.x * diff.x + diff.y * diff.y);

    if (distance <= m_attackRange) {
        m_target->ChangeLife(-m_damage);
        m_attackTimer.resetTimer();
    }
}

float Enemy::calculateAngleToTarget() const
{
    if (!m_target) return m_movementAngle;

    try {
        sf::Vector2f targetPos = m_target->getShape()->getPosition();
        sf::Vector2f myPos = m_shape->getPosition();

        sf::Vector2f direction = targetPos - myPos;
        return std::atan2(direction.y, direction.x) * 180.0f / 3.14159f;
    }
    catch (...) {
        return m_movementAngle;
    }
}

bool Enemy::isTargetInDetectionZone() const
{
    if (!m_target) return false;

    try {
        sf::Vector2f targetWorldPos = screenToWorldPosition(m_target->getShape()->getPosition());
        sf::Vector2f diff = targetWorldPos - m_worldPosition;
        float distanceSquared = diff.x * diff.x + diff.y * diff.y;
        return distanceSquared <= m_detectionRadius * m_detectionRadius;
    }
    catch (...) {
        return false;
    }
}

sf::Vector2f Enemy::worldToScreenPosition(const sf::Vector2f& worldPos) const
{
    return sf::Vector2f(
        m_scene->getRoot()->getScene()->getBackgroundCenter().x + worldPos.x,
        m_scene->getRoot()->getScene()->getBackgroundCenter().y + worldPos.y
    );
}

sf::Vector2f Enemy::screenToWorldPosition(const sf::Vector2f& screenPos) const
{
    return sf::Vector2f(
        screenPos.x - m_scene->getRoot()->getScene()->getBackgroundCenter().x,
        screenPos.y - m_scene->getRoot()->getScene()->getBackgroundCenter().y
    );
}

std::string Enemy::getOrientationString() const
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

Orientation Enemy::determineOrientation(float angle)
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

// Implémentation des états
Enemy::IState* Enemy::PatrolState::handle(const EnemyState& state)
{
    switch (state)
    {
    case EnemyState::CHASE: return new ChaseState();
    case EnemyState::ATTACK: return new AttackState();
    case EnemyState::STUNNED: return new StunnedState();
    case EnemyState::DEAD: return new DeadState();
    default: return nullptr;
    }
}

void Enemy::PatrolState::update(Enemy* enemy, float deltaTime)
{
    std::string animationName = "move_" + enemy->getOrientationString();
    if (enemy->m_animationComponent->getCurrentAnimation() != animationName) {
        enemy->m_animationComponent->playAnimation(animationName);
    }

    enemy->patrol();

    if (enemy->m_target && enemy->isTargetInDetectionZone()) {
        sf::Vector2f targetPos = enemy->m_target->getShape()->getPosition();
        sf::Vector2f myPos = enemy->worldToScreenPosition(enemy->m_worldPosition);
        float distance = std::hypot(targetPos.x - myPos.x, targetPos.y - myPos.y);

        if (distance <= enemy->m_attackRange * 1.5f) {
            enemy->changeState(EnemyState::ATTACK);
        }
        else {
            enemy->changeState(EnemyState::CHASE);
        }
    }
}

Enemy::IState* Enemy::ChaseState::handle(const EnemyState& state)
{
    switch (state)
    {
    case EnemyState::PATROL: return new PatrolState();
    case EnemyState::ATTACK: return new AttackState();
    case EnemyState::STUNNED: return new StunnedState();
    case EnemyState::DEAD: return new DeadState();
    default: return nullptr;
    }
}

void Enemy::ChaseState::update(Enemy* enemy, float deltaTime)
{
    if (!enemy->m_target) {
        enemy->changeState(EnemyState::PATROL);
        return;
    }

    std::string animationName = "move_" + enemy->getOrientationString();
    if (enemy->m_animationComponent->getCurrentAnimation() != animationName) {
        enemy->m_animationComponent->playAnimation(animationName);
    }

    sf::Vector2f targetPos = enemy->m_target->getShape()->getPosition();;
    enemy->moveTowards(targetPos, deltaTime);

    sf::Vector2f myPos = enemy->worldToScreenPosition(enemy->m_worldPosition);
    float distance = std::hypot(targetPos.x - myPos.x, targetPos.y - myPos.y);

    if (distance <= enemy->m_attackRange) {
        enemy->changeState(EnemyState::ATTACK);
    }
    else if (!enemy->isTargetInDetectionZone()) {
        enemy->changeState(EnemyState::PATROL);
    }
}

Enemy::IState* Enemy::AttackState::handle(const EnemyState& state)
{
    switch (state)
    {
    case EnemyState::PATROL: return new PatrolState();
    case EnemyState::CHASE: return new ChaseState();
    case EnemyState::STUNNED: return new StunnedState();
    case EnemyState::DEAD: return new DeadState();
    default: return nullptr;
    }
}

void Enemy::AttackState::update(Enemy* enemy, float deltaTime)
{
    if (!enemy->m_target) {
        enemy->changeState(EnemyState::PATROL);
        return;
    }

    sf::Vector2f targetPos = enemy->m_target->getShape()->getPosition();;
    sf::Vector2f myPos = enemy->worldToScreenPosition(enemy->m_worldPosition);
    sf::Vector2f direction = targetPos - myPos;

    float angle = std::atan2(direction.y, direction.x) * 180.0f / 3.14159f;
    enemy->m_currentOrientation = enemy->determineOrientation(angle);

    float distance = std::hypot(direction.x, direction.y);

    std::string animationName = "attack_" + enemy->getOrientationString();
    if (enemy->m_animationComponent->getCurrentAnimation() != animationName ||
        enemy->m_animationComponent->isAnimationFinished()) {
        enemy->m_animationComponent->playAnimation(animationName);
    }

    if (enemy->m_attackTimer.ActionIsReady() && distance <= enemy->m_attackRange) {
        enemy->performMeleeAttack();
        attackCooldown = enemy->m_attackCooldown;
    }
    else {
        attackCooldown -= deltaTime;
    }

    if (distance > enemy->m_attackRange * 1.2f) {
        enemy->changeState(EnemyState::CHASE);
    }
    else if (!enemy->isTargetInDetectionZone()) {
        enemy->changeState(EnemyState::PATROL);
    }
}

Enemy::IState* Enemy::StunnedState::handle(const EnemyState& state)
{
    switch (state)
    {
    case EnemyState::PATROL: return new PatrolState();
    case EnemyState::CHASE: return new ChaseState();
    case EnemyState::ATTACK: return new AttackState();
    case EnemyState::DEAD: return new DeadState();
    default: return nullptr;
    }
}

void Enemy::StunnedState::update(Enemy* enemy, float deltaTime)
{
    if (stunDuration <= 0.0f) {
        stunDuration = 3.0f;
    }

    std::string animationName = "idle_" + enemy->getOrientationString();
    if (enemy->m_animationComponent->getCurrentAnimation() != animationName) {
        enemy->m_animationComponent->playAnimation(animationName);
    }

    stunDuration -= deltaTime;

    if (static_cast<int>(stunDuration * 5) % 2 == 0) {
        enemy->m_animationComponent->getSprite().setColor(sf::Color(255, 255, 255, 180));
    }
    else {
        enemy->m_animationComponent->getSprite().setColor(sf::Color::White);
    }

    if (stunDuration <= 0.0f) {
        enemy->m_animationComponent->getSprite().setColor(sf::Color::White);

        if (enemy->m_target && enemy->isTargetInDetectionZone()) {
            enemy->changeState(EnemyState::CHASE);
        }
        else {
            enemy->changeState(EnemyState::PATROL);
        }
    }
}

Enemy::IState* Enemy::DeadState::handle(const EnemyState& state)
{
    return nullptr;
}

void Enemy::DeadState::update(Enemy* enemy, float deltaTime)
{
    if (deathTimer <= 0.0f) {
        deathTimer = 2.0f;

        std::string animationName = "idle_" + enemy->getOrientationString();
        enemy->m_animationComponent->playAnimation(animationName);

        enemy->m_animationComponent->getSprite().setColor(sf::Color(255, 100, 100, 200));
    }

    deathTimer -= deltaTime;

    sf::Color spriteColor = enemy->m_animationComponent->getSprite().getColor();
    spriteColor.a = static_cast<sf::Uint8>(std::max(0.0f, (deathTimer / 2.0f) * 255));
    enemy->m_animationComponent->getSprite().setColor(spriteColor);

}