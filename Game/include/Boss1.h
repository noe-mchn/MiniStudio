#pragma once
#include "IGameObject.h"
#include "GameObject.h"
#include "BossBullet.h"

namespace BossAI
{
    class RandomAttackSelector;
    class RandomMovementSelector;
}

// Type de mode du boss
enum class BossMode
{
    Type1,  // Comportement du Boss1
    Type2,  // Comportement du Boss2
    Type3,  // Comportement du Boss3
    Combined // Combinaison des trois comportements
};

enum class MovementPattern
{
    Horizontal,
    Vertical,
    Diagonal,
    Random,
    Chase,
    Circular,
    Stationary
};

enum class BossPhase
{
    One,    // 100%-80% vie
    Two,    // 80%-60% vie
    Three,  // 60%-40% vie
    Four,   // 40%-20% vie
    Five    // 20%-0% vie
};

struct BossParameters
{
    float speed;
    float attackRate;
    int attackCount;
    float spreadAngle;
    float projectileSize;
    float projectileSpeed;
    float damageMultiplier;
    bool invulnerableToCollisions;

    static BossParameters getForPhase(BossPhase phase, BossMode mode);
};

class MegaBoss : public DestructibleObject, public IComposite
{
protected:
    enum State
    {
        PATROL,     // Patrouille et mouvement
        CHASE,      // Poursuite de la cible
        LOAD,       // Chargement des attaques
        FIRE,       // Attaque
        HAND,       // Attaque spéciale (main)
        PROTECT,    // Mode défensif
        DESTRUCT,   // Séquence destructive
        HELP,       // Mode d'appel à l'aide
        DEAD        // État mort
    };

    struct IState
    {
        virtual ~IState() = default;
        virtual IState* handle(const State& state) = 0;
        virtual void update(MegaBoss* boss, float deltaTime) = 0;
    };

    // États
    struct PatrolState : IState
    {
        ~PatrolState() override = default;
        IState* handle(const State& state) override;
        void update(MegaBoss* boss, float deltaTime) override;
    };

    struct ChaseState : IState
    {
        ~ChaseState() override = default;
        IState* handle(const State& state) override;
        void update(MegaBoss* boss, float deltaTime) override;
    };

    struct LoadState : IState
    {
        ~LoadState() override = default;
        IState* handle(const State& state) override;
        void update(MegaBoss* boss, float deltaTime) override;
    private:
        float reloadTime = 100.0f;
        float reloadTimer = 0.0f;
    };

    struct FireState : IState
    {
        ~FireState() override = default;
        IState* handle(const State& state) override;
        void update(MegaBoss* boss, float deltaTime) override;
    };

    struct HandState : IState
    {
        ~HandState() override = default;
        IState* handle(const State& state) override;
        void update(MegaBoss* boss, float deltaTime) override;
        IGameObject* ship = nullptr;
    };

    struct ProtectState : IState
    {
        ~ProtectState() override = default;
        IState* handle(const State& state) override;
        void update(MegaBoss* boss, float deltaTime) override;

        float maxProtectionTime = 10.0f;  // 10 secondes max de protection
        float protectionTimer = 0.0f;
    };

    struct DestructState : IState
    {
        ~DestructState() override = default;
        IState* handle(const State& state) override;
        void update(MegaBoss* boss, float deltaTime) override;
    };

    struct HelpState : IState
    {
        ~HelpState() override = default;
        IState* handle(const State& state) override;
        void update(MegaBoss* boss, float deltaTime) override;
    };

    struct DeadState : IState
    {
        ~DeadState() override = default;
        IState* handle(const State& state) override;
        void update(MegaBoss* boss, float deltaTime) override;
    };

public:
    MegaBoss(IComposite* scene, const sf::Vector2f& spawnPosition, BossMode mode = BossMode::Combined, float maxHealth = 10000.0f);
    virtual ~MegaBoss();

    void ProssesInput(const sf::Event& event) override {};
    void Update(const float& deltatime) override;
    void Render() override;
    void HandleCollision(IGameObject* object) override;
    void ChangeLife(const float& life) override;

    float getMaxLife() const { return m_maxLife; }
    float getCurrentLife() const { return m_life; }
    IComposite* getScene() const { return m_scene; }

    float getSpeed() const { return m_speed; }
    void setSpeed(float speed) { m_speed = speed; }
    sf::Vector2f getPosition() const { return m_worldPosition; }

    void moveTowards(const sf::Vector2f& target, float deltaTime);
    void moveToPosition(const sf::Vector2f& position);
    void move(const sf::Vector2f& offset);

    // Méthodes d'attaque
    void fireProjectiles(int count, float spreadAngle);
    void fireSpecialProjectile(ProjectileType type);
    void fireGrowingProjectile();
    void fireFastProjectile();
    void fireCircularPattern();
    void fireWavePattern();

    // Méthodes de capacités
    void activateDefensiveAbility(float duration);
    void activateOffensiveBoost(float multiplier, float duration);
    void regenerateHealth(float amount);
    void summonMinions(int count);

    // Changement de mode et phase
    void setMode(BossMode mode);
    BossMode getMode() const { return m_currentMode; }
    void updateParameters();
    BossPhase getCurrentPhase() const { return m_currentPhase; }

    // Changement d'état
    void changeState(const State& newState);

protected:
    void findTarget();
    void createWeapons(float offset);
    sf::Vector2f worldToScreenPosition(const sf::Vector2f& worldPos) const;
    sf::Vector2f screenToWorldPosition(const sf::Vector2f& screenPos) const;
    void patrol();

    // Informations de base
    float m_maxLife;
    float m_speed;
    BossMode m_currentMode;
    IState* m_currentState;

    // Animation
    AnimateSprite m_animate;
    Timer m_animationTimer;

    // Paramètres selon la phase
    BossParameters m_bossParams;
    BossPhase m_currentPhase;

    // Gestion des boosts
    bool m_offensiveBoostActive;
    float m_damageMultiplier;
    Timer m_offensiveBoostTimer;

    // Invulnérabilité
    Timer m_invulnerabilityTimer;
    bool m_isInvulnerable;
    bool m_isInvulnerableToCollisions;

    // Suivi de cible
    IShapeSFML* m_target;
    float m_detectionRadius;
    bool m_isTrackingTarget;

    // Armes et attaques
    std::array<ITurret*, 6> m_weapons;
    Timer m_attackTimer;
    float m_patrolTimer;
    int m_projectileCount;
    int m_maxProjectilesBeforeReload;

    // Position et mouvement
    sf::Vector2f m_worldPosition;
    float m_movementAngle;
    MovementPattern m_movementPattern;

    // Assistants IA
    BossAI::RandomAttackSelector* m_specialAttackSelector = nullptr;
    BossAI::RandomMovementSelector* m_movementSelector = nullptr;

    // Utilitaires
    float calculateAngleToTarget() const;
    bool isTargetInDetectionZone() const;
    bool shouldAttackTarget() const;
    bool isTargetValid() const;
};