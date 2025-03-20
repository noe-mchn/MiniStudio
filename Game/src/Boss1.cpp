#include "Boss1.h"
#include "RandomNumber.h"
#include "Ship.h"
#include <functional>
#include <iostream>
#include <cmath>

BossParameters BossParameters::getForPhase(BossPhase phase, BossMode mode)
{
    BossParameters params;

    // Paramètres de base
    switch (phase)
    {
    case BossPhase::One:
        params = { 300.0f, 2.0f, 1, 0.0f, 20.0f, 500.0f, 1.0f, false };
        break;
    case BossPhase::Two:
        params = { 400.0f, 1.8f, 2, 20.0f, 20.0f, 600.0f, 1.0f, false };
        break;
    case BossPhase::Three:
        params = { 500.0f, 1.5f, 3, 40.0f, 25.0f, 700.0f, 1.0f, false };
        break;
    case BossPhase::Four:
        params = { 600.0f, 1.2f, 4, 60.0f, 30.0f, 800.0f, 1.0f, false };
        break;
    case BossPhase::Five:
        params = { 700.0f, 0.9f, 5, 90.0f, 35.0f, 900.0f, 1.2f, false };
        break;
    }

    // Ajustements selon le mode
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
        // Au maximum de puissance
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

// État Patrouille
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
    sf::Vector2f direction = boss->m_target->getPosition() - boss->getShape()->getPosition();
    float distance = std::sqrt(direction.x * direction.x + direction.y * direction.y);

    boss->patrol();

    // Transitions d'état selon le mode et les conditions
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
}

// État Poursuite
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

void MegaBoss::ChaseState::update(MegaBoss* boss, float deltaTime)
{
    sf::Vector2f direction = boss->m_target->getPosition() - boss->getShape()->getPosition();
    float distance = std::sqrt(direction.x * direction.x + direction.y * direction.y);

    // Transitions d'état selon le mode et les conditions
    switch (boss->m_currentMode)
    {
    case BossMode::Type1:
        if (boss->getCurrentLife() <= boss->getMaxLife() * 0.05)
            boss->changeState(State::DESTRUCT);
        else if (distance >= 650.0f)
            boss->changeState(State::PATROL);
        else if (distance <= 300.0f)
            boss->changeState(State::LOAD);
        break;

    case BossMode::Type2:
        if (boss->getCurrentLife() <= boss->getMaxLife() * 0.3)
            boss->changeState(State::PROTECT);
        else if (distance <= 250.0f)
            boss->changeState(State::HAND);
        break;

    case BossMode::Type3:
    case BossMode::Combined:
        if (boss->getCurrentLife() <= boss->getMaxLife() * 0.15)
            boss->changeState(State::PROTECT);
        else if (distance <= 350.0f)
            boss->changeState(State::LOAD);
        else if (distance >= 800.0f)
            boss->changeState(State::PATROL);
        break;
    }

    // Mouvement de poursuite
    if (distance > 0)
    {
        direction /= distance;
        boss->move(direction * boss->getSpeed() * deltaTime);
    }
}

// État Chargement
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

// État Tir
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
        break;
    case BossPhase::Three:
        boss->fireProjectiles(3, 40.0f);
        break;
    case BossPhase::Four:
        if (UseRandomNumber().getRandomNumber<int>(0, 10) > 7)
            boss->fireGrowingProjectile();
        else
            boss->fireProjectiles(4, 60.0f);
        break;
    case BossPhase::Five:
        if (UseRandomNumber().getRandomNumber<int>(0, 10) > 5)
        {
            if (UseRandomNumber().getRandomNumber<int>(0, 1) == 0)
                boss->fireGrowingProjectile();
            else
                boss->fireFastProjectile();
        }
        else
            boss->fireProjectiles(5, 90.0f);
        break;
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

// État Main (attaque spéciale du Boss2)
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
    sf::Vector2f direction = boss->m_target->getPosition() - boss->getShape()->getPosition();
    float distance = std::sqrt(direction.x * direction.x + direction.y * direction.y);

    // Comportement spécifique de l'attaque "main"
    if (boss->m_attackTimer.ActionIsReady())
    {
        if (distance < 350.0f)
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

    if (distance > 400.0f)
    {
        boss->changeState(State::CHASE);
        return;
    }
}

// État Protection
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
    // Incrémenter le timer
    protectionTimer += deltaTime;

    // Régénération plus lente
    boss->regenerateHealth(0.1f); // 0.1 au lieu de 0.5

    // Forcer la sortie de l'état après un certain temps
    if (protectionTimer >= maxProtectionTime) {
        boss->changeState(State::CHASE);
        protectionTimer = 0.0f;
        boss->m_isInvulnerable = false; // S'assurer que l'invulnérabilité est désactivée
        return;
    }

    if (boss->m_currentMode == BossMode::Type1 &&
        boss->getCurrentLife() <= boss->getMaxLife() * 0.05)
    {
        boss->changeState(State::DESTRUCT);
        return;
    }

    if (boss->getCurrentLife() <= boss->getMaxLife() * 0.02)
    {
        boss->changeState(State::DEAD);
        return;
    }

    // Modification du seuil pour sortir plus facilement de l'état de protection
    // 40% au lieu de 35%
    if (boss->getCurrentLife() >= boss->getMaxLife() * 0.40)
    {
        boss->changeState(State::PATROL);
        return;
    }

    // Protection active
    boss->m_isInvulnerable = true;

    // Attaque défensive
    if (boss->m_attackTimer.ActionIsReady())
    {
        boss->fireCircularPattern();
        boss->m_attackTimer.resetTimer();
    }
}

// État Destruction
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
    if (boss->getCurrentLife() <= boss->getMaxLife() * 0.025)
    {
        boss->changeState(State::HELP);
        return;
    }

    // Comportement de destruction - attaques désespérées
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

// État Aide
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

    // Appel à l'aide - invocation de minions
    static Timer minionTimer(5.0f);
    if (minionTimer.AutoActionIsReady(deltaTime))
    {
        boss->summonMinions(3);
    }
}

// État Mort
MegaBoss::IState* MegaBoss::DeadState::handle(const State& state)
{
    return nullptr;
}

void MegaBoss::DeadState::update(MegaBoss* boss, float deltaTime)
{
    boss->m_isInvulnerable = true;

    // Logique d'animation de mort
    static float deathTimer = 0.0f;
    deathTimer += deltaTime;

    if (deathTimer > 3.0f)
    {
        boss->destroy();
    }
}

// Constructeur du MegaBoss
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
    , m_animate({ "BossSprite1.png", "BossSprite2.png", "BossSprite3.png" })
    , m_animationTimer(0.2f)
{
    // Initialisation des paramètres selon le mode et la phase
    m_bossParams = BossParameters::getForPhase(m_currentPhase, m_currentMode);
    m_speed = m_bossParams.speed;
    m_isInvulnerableToCollisions = m_bossParams.invulnerableToCollisions;

    // Création de la forme
    sf::Vector2f screenPosition = worldToScreenPosition(m_worldPosition);
    m_shape = new RectangleSFML(200.0f, 200.0f, screenPosition);

    // Trouver la cible
    findTarget();
    if (!m_target) {
        IShapeSFML* defaultTarget = new SquareSFML(1, m_scene->getRoot()->getScene()->GetCenterWindow());
        m_target = defaultTarget;
    }

    // Création des armes
    createWeapons(70.0f);

    // Ajout d'un indicateur de vie
    new Life(this, this, Color::Pink);

    // Texture initiale
    m_shape->setTexture(m_scene->getRoot()->getScene()->getTexture()->getTexture(m_animate.getCurrentPath()));
}

MegaBoss::~MegaBoss()
{
    delete m_currentState;
}

void MegaBoss::createWeapons(float weaponOffset)
{
    // 6 armes pour plus de puissance de feu
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
    static Timer targetSearchTimer(2.0f);

    // Recherche de cible périodique
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
            // S'assurer que le timer est réinitialisé
            m_invulnerabilityTimer.resetTimer();
        }
    }

    // Animation du sprite
    if (m_animationTimer.AutoActionIsReady(deltaTime))
    {
        m_animate.ChangeToNextPath();
        m_shape->setTexture(m_scene->getRoot()->getScene()->getTexture()->getTexture(m_animate.getCurrentPath()));
    }

    // Suivi de cible
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
        // Comportement en l'absence de cible
        m_patrolTimer += deltaTime;

        if (m_currentMode == BossMode::Combined)
        {
            // Mouvement circulaire complexe
            float radius = 300.0f;
            sf::Vector2f patrolPos;
            patrolPos.x = sin(m_patrolTimer * 0.5f) * radius;
            patrolPos.y = cos(m_patrolTimer * 0.25f) * radius;
            moveToPosition(patrolPos);
        }
    }

    // Mise à jour de l'état courant
    if (m_currentState)
    {
        m_currentState->update(this, deltaTime);
    }

    // Mise à jour des temporisateurs
    m_attackTimer.NextTIck(deltaTime);
    m_isTrackingTarget = m_target && isTargetInDetectionZone();

    // Gestion du boost offensif
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

    // Mettre à jour la position de la forme
    m_shape->setPosition(worldToScreenPosition(m_worldPosition));

    // Mettre à jour les composants enfants
    IComposite::Update(deltaTime);
}

void MegaBoss::Render()
{
    // Rendu du boss
    m_scene->getRoot()->getScene()->getWindow()->draw(static_cast<RectangleSFML*>(m_shape)->getShape());

    // Rendu des composants enfants
    IComposite::Render();
}

void MegaBoss::HandleCollision(IGameObject* object)
{
    // Toujours vérifier si c'est un destructible
    if (object->globalGameObjectType() != GameObjectType::DestructibleObject)
        return;

    // Vérifier si c'est un projectile de nos propres armes
    IBullet* bullet = getObj<IBullet*>(object);
    if (bullet) {
        // Si c'est un de nos projectiles, ignorer
        for (auto& weapon : m_weapons) {
            if (bullet->getTurret() == weapon) {
                return;
            }
        }

        // C'est un projectile du joueur - traiter normalement
    }
    else if (m_isInvulnerableToCollisions) {
        // Si ce n'est pas un projectile et qu'on est invulnérable aux collisions physiques
        return;
    }

    // Si invulnérabilité temporaire active, ignorer les dégâts
    if (m_isInvulnerable)
        return;

    // Appliquer les dégâts
    ChangeLife(-1.0f);

    // Activer l'invulnérabilité temporaire après avoir pris un coup
    m_isInvulnerable = true;
    m_invulnerabilityTimer.setNewTimer(0.5f);
    m_invulnerabilityTimer.resetTimer();
}

void MegaBoss::ChangeLife(const float& life)
{
    // Vérification problématique : elle bloque TOUS les dégâts si m_isInvulnerableToCollisions est vrai
    // if (life < 0 && (m_isInvulnerable || m_isInvulnerableToCollisions))
    //    return;

    // Nouvelle logique : ne bloque que pour l'invulnérabilité temporaire
    if (life < 0 && m_isInvulnerable)
        return;

    // Appliquer le changement de vie
    DestructibleObject::ChangeLife(life);

    // Calculer le pourcentage de vie restant
    float healthPercentage = getCurrentLife() / m_maxLife;
    BossPhase newPhase;

    // Déterminer la nouvelle phase en fonction du pourcentage de vie
    if (healthPercentage <= 0.2f) newPhase = BossPhase::Five;
    else if (healthPercentage <= 0.4f) newPhase = BossPhase::Four;
    else if (healthPercentage <= 0.6f) newPhase = BossPhase::Three;
    else if (healthPercentage <= 0.8f) newPhase = BossPhase::Two;
    else newPhase = BossPhase::One;

    // Si changement de phase, mettre à jour les paramètres
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
        // Patrouille horizontale
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
        // Patrouille en mouvement circulaire
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
        // Patrouille en zigzag
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

        // Changement de direction verticale
        if (getPosition().y >= 1000)
            movingDown = false;
        else if (getPosition().y <= 500)
            movingDown = true;
        break;

    case BossMode::Combined:
        // Combinaison de mouvements
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
            m_bossParams.projectileSize,
            m_bossParams.projectileSpeed,
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

    // Tir en cercle complet
    for (int i = 0; i < 12; i++) {
        float angle = baseAngle + (i * 30.0f);

        for (size_t j = 0; j < m_weapons.size(); j += 2) {  // Utilise une arme sur deux
            m_weapons[j]->getShape()->setRotation(angle);
            m_weapons[j]->Fire();
        }
    }
}

void MegaBoss::fireWavePattern()
{
    const int WAVE_COUNT = 3;
    const float WAVE_DELAY = 0.2f;

    // Tir en vagues
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

    // Régénération de vie
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
    // Cette méthode serait implémentée pour créer de nouveaux ennemis
    // selon l'environnement spécifique du jeu (non implémenté ici car dépendant du reste du code)
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

void MegaBoss::changeState(const State& newState)
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