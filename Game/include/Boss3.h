//#pragma once
//#include "IGameObject.h"
//#include "GameObject.h"
//#include "BossBullet.h"
//
//namespace BossAI
//{
//    class RandomAttackSelector;
//    class RandomMovementSelector;
//}
//
//enum class EntityMovementPattern3
//{
//    Horizontal,
//    Vertical,
//    Diagonal,
//    Random,
//    Chase,
//    Stationary
//};
//
//enum class EntityPhase3
//{
//    One,
//    Two,
//    Three,
//    Four,
//    Five
//};
//
//struct EntityParameters3
//{
//    float speed;
//    float attackRate;
//    int attackCount;
//    float spreadAngle;
//    float projectileSize;
//    float projectileSpeed;
//    float damageMultiplier;
//
//    static EntityParameters3 getForPhase(EntityPhase3 phase);
//};
//
//class Boss3 : public DestructibleObject, public IComposite
//{
//protected:
//    enum State
//    {
//        PATROL
//        , CHASE
//        , RELOAD
//        , FIRE
//    };
//
//    struct IState
//    {
//        virtual ~IState() = default;
//        virtual IState* handle(const State& state) = 0;
//        virtual void update(Boss3* boss, float deltaTime) = 0;
//    };
//    struct PatrolState : IState
//    {
//        ~PatrolState() override = default;
//        IState* handle(const State& state) override;
//        void update(Boss3* boss, float deltaTime) override;
//    };
//    struct ChaseState : IState
//    {
//        ~ChaseState() override = default;
//        IState* handle(const State& state) override;
//        void update(Boss3* boss, float deltaTime) override;
//    };
//    struct ReloadState : IState
//    {
//        ~ReloadState() override = default;
//        IState* handle(const State& state) override;
//        void update(Boss3* boss, float deltaTime) override;
//
//    };
//    struct FireState : IState
//    {
//        ~FireState() override = default;
//        IState* handle(const State& state) override;
//        void update(Boss3* boss, float deltaTime) override;
//
//    };
//
//public:
//    Boss3(IComposite* scene, const sf::Vector2f& spawnPosition, float maxHealth = 5000.0f);
//    virtual ~Boss3();
//
//    void ProssesInput(const sf::Event& event) override {};
//    void Update(const float& deltatime);
//    void Render() override;
//    void HandleCollision(IGameObject* object) override;
//    void ChangeLife(const float& life);
//
//    float getMaxLife() const { return m_maxLife; }
//    float getCurrentLife() const { return m_life; }
//    IComposite* getScene() const { return m_scene; }
//
//    float getSpeed();
//    void setSpeed(float speed);
//    void moveTowards(const sf::Vector2f& target, float deltaTime);
//    void moveToPosition(const sf::Vector2f& position);
//
//    virtual void fireProjectiles(int count, float spreadAngle);
//    virtual void fireSpecialProjectile(ProjectileType type);
//
//    void fireGrowingProjectile();
//    void fireFastProjectile();
//
//    virtual void activateDefensiveAbility(float duration);
//    virtual void activateOffensiveBoost(float multiplier, float duration);
//    virtual void regenerateHealth(float amount);
//
//    virtual void updateEntityParameters();
//    EntityPhase3 getCurrentPhase() const { return m_currentPhase; }
//
//    void move(const sf::Vector2f& offset);
//
//    void changeState(const State& newState);
//
//    IState* m_currentState;
//
//protected:
//    virtual void findTarget();
//    virtual void createWeapons(float offset);
//    sf::Vector2f worldToScreenPosition(const sf::Vector2f& worldPos) const;
//    sf::Vector2f screenToWorldPosition(const sf::Vector2f& screenPos) const;
//
//    float m_maxLife;
//    float m_speed;
//
//    AnimateSprite m_animate;
//    Timer m_animationTimer;
//
//    EntityParameters3 m_entityParams;
//
//    bool m_offensiveBoostActive;
//    float m_damageMultiplier;
//    Timer m_offensiveBoostTimer;
//    float m_patrolTimer;
//    Timer m_invulnerabilityTimer;
//    bool m_isInvulnerable;
//
//    EntityPhase3 m_currentPhase;
//
//    IShapeSFML* m_target;
//
//    std::array<ITurret*, 4> m_weapons;
//
//    BossAI::RandomAttackSelector* m_specialAttackSelector;
//    BossAI::RandomMovementSelector* m_movementSelector;
//
//    sf::Vector2f m_worldPosition;
//    float m_movementAngle;
//
//    float m_detectionRadius;
//    bool m_isTrackingTarget;
//
//    Timer m_attackTimer;
//
//    float calculateAngleToTarget() const;
//    bool isTargetInDetectionZone() const;
//    bool shouldAttackTarget() const;
//    bool isTargetValid() const;
//
//    int m_projectileCount = 0;
//    int m_maxProjectilesBeforeReload = 5;
//
//
//    Boss3* boss;
//};