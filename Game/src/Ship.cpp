#include "Ship.h"
#include "IGameObject.h"
#include "Animation.h"
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
    if (ship->m_animationComponent->getCurrentAnimation() != "idle")
    {
        ship->m_animationComponent->playAnimation("idle");
        ship->m_animationComponent->getAnimations()["idle"].setOrientation(ship->m_currentOrientation);
    }

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
    if (ship->m_animationComponent->getCurrentAnimation() != "move")
    {
        ship->m_animationComponent->playAnimation("move");
        ship->m_animationComponent->getAnimations()["move"].setOrientation(ship->m_currentOrientation);
    }

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
    if (ship->m_animationComponent->getCurrentAnimation() != "attack")
    {
        ship->m_animationComponent->playAnimation("attack");
        ship->m_animationComponent->getAnimations()["attack"].setOrientation(ship->m_currentOrientation);
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
    // Jouer l'animation d'attaque au pistolet
    if (ship->m_animationComponent->getCurrentAnimation() != "pistol")
        ship->m_animationComponent->playAnimation("pistol");

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
    // Jouer l'animation de rechargement
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
Ship::Ship(IComposite* scene, IShapeSFML* background)
    : DestructibleObject(scene, 10)
    , IComposite(scene)
    , m_background(background)
    , m_angle(0)
    , m_elapsedTime(0.2)
    , m_animate({ "Hero.png" })
    , m_physics(new MovementInSpace(1000, 400, 200))
    , m_invisibility(2.5)
    , m_detectionRadius(30.0f)
    , m_meleeDamage(0.01f)
    , m_meleeAttackCooldown(0.5f)
    , m_meleeAttackTimer(2.0f)
    , m_currentOrientation(Orientation::DOWN)
{
    m_shape = new SquareSFML(150, scene->getRoot()->getScene());
    m_shape->setTexture(m_scene->getRoot()->getScene()->getTexture()->getTexture(m_animate.getCurrentPath()));

    // Créer et configurer le composant d'animation
    m_animationComponent = new AnimationComponent(this);
    setupAnimations();

    // Mettre à jour la position initiale du composant d'animation
    m_animationComponent->updatePosition(m_shape->getPosition());

    // Configurer le reste du vaisseau
    new Life(this, this, Color::Blue);
    m_turret = new FixTurret(this, m_shape, sf::Vector2f(35, -25), 0.75);
    m_turret->SetFireRate(0.2f);
    m_turret->SetOverloadGun(5, 30);
    m_turret->setBullet(0, 0, 0);

    m_currentState = new IdleState();

    // Démarrer avec l'animation d'idle
    m_animationComponent->playAnimation("idle");
}

Ship::~Ship()
{
    delete m_physics;
    delete m_currentState;
    // Le composant d'animation sera détruit avec le ship grâce à la hiérarchie de composants
}

void Ship::setupAnimations()
{
    // S'assurer que le TextureManager a été initialisé et a chargé les textures
    TextureManager::getInstance().initialize();

    // Vérifier si les textures sont chargées, sinon les charger
    if (!TextureManager::getInstance().hasTexture("Hero"))
        TextureManager::getInstance().loadTexture("Hero", "Hero.png");

    // Créer les animations pour chaque état
    // Animation d'idle - 6 frames
    Animation idleAnim("Hero.png", 6, 0.2f); // Utiliser le nom de fichier complet
    idleAnim.setFrameSize(sf::Vector2i(64, 64));
    idleAnim.setStartPosition(sf::Vector2i(0, 0), 4);

    // Animation de mouvement - 5 frames
    Animation moveAnim("Hero.png", 5, 0.1f); // Utiliser le nom de fichier complet
    moveAnim.setFrameSize(sf::Vector2i(64, 64));
    moveAnim.setStartPosition(sf::Vector2i(6 * 64, 0), 4);

    // Animation d'attaque - 6 frames
    Animation attackAnim("Hero.png", 6, 0.05f, false); // Utiliser le nom de fichier complet
    attackAnim.setFrameSize(sf::Vector2i(64, 64));
    attackAnim.setStartPosition(sf::Vector2i((6 + 5) * 64, 0), 4);

    // Ajouter les animations au composant
    m_animationComponent->addAnimation("idle", idleAnim);
    m_animationComponent->addAnimation("move", moveAnim);
    m_animationComponent->addAnimation("attack", attackAnim);
}


bool Ship::IsDestroyed()
{
    return m_life <= 0;
}

void Ship::ProcessInput(const sf::Event& event)
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

    // Appeler la méthode update de l'état courant - CETTE LIGNE MANQUE!
    m_currentState->update(this, deltatime);

    // Appliquer la rotation et la position - CES LIGNES MANQUENT!
    m_shape->setRotation(m_angle);
    m_background->setPosition(static_cast<MovementInSpace*>(m_physics)->calculPosition(m_background, m_scene->getRoot()->getScene(), m_scene->getRoot()->getScene()->getRefreshTime().asSeconds()));

    // Mettre à jour la position du sprite d'animation - CETTE LIGNE MANQUE!
    m_animationComponent->updatePosition(m_shape->getPosition());

    // Mettre à jour l'orientation basée sur l'angle
    Orientation newOrientation = determineOrientation(m_angle);
    if (newOrientation != m_currentOrientation)
    {
        m_currentOrientation = newOrientation;

        // Mettre à jour l'orientation des animations actives
        if (m_animationComponent->getCurrentAnimation() != "")
        {
            auto& animations = m_animationComponent->getAnimations();
            auto& currentAnim = animations[m_animationComponent->getCurrentAnimation()];
            currentAnim.setOrientation(m_currentOrientation);
        }
    }

    // Mettre à jour l'animation
    m_animationComponent->Update(deltatime);

    m_meleeAttackTimer.NextTIck(deltatime);

    IComposite::Update(deltatime);
    m_invisibility.NextTIck(m_scene->getRoot()->getScene()->getRefreshTime().asSeconds());
}

void Ship::Render()
{
    // Rendre le sprite d'animation au lieu du sprite de forme
    m_animationComponent->Render();

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

Orientation Ship::determineOrientation(float angle)
{
    // Convertir l'angle en une orientation principale
    // angle est en degrés où 0 = droite, 90 = bas, 180 = gauche, 270 = haut

    // Normaliser l'angle entre 0 et 360
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