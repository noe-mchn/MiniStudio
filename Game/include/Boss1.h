#pragma once
#include "IGameObject.h"
#include "GameObject.h"
#include "BossBullet.h"

namespace BossAI 
{
    class RandomAttackSelector;
    class RandomMovementSelector;
}

enum class EntityMovementPattern
{
    Horizontal,
    Vertical,
    Diagonal,
    Random,
    Chase,
    Stationary
};

enum class EntityPhase
{
    One,
    Two,
    Three,
    Four,
    Five
};

struct EntityParameters
{
    float speed;
    float attackRate;
    int attackCount;
    float spreadAngle;
    float projectileSize;
    float projectileSpeed;
    float damageMultiplier;

    static EntityParameters getForPhase(EntityPhase phase);
};



class Boss1 : public DestructibleObject, public IComposite
{
protected:
    enum State
    {
        PATROL
        , CHASE
        , LOAD
        , FIRE
        , DESTRUCT
        , HELP
        , DEAD
    };

    struct IState
    {
        virtual ~IState() = default;
        virtual IState* handle(const State& state) = 0;
        virtual void update(Boss1* boss, float deltaTime) = 0;
    };
    struct PatrolState : IState
    {
        ~PatrolState() override = default;
        IState* handle(const State& state) override;
        void update(Boss1* boss, float deltaTime) override;
    };
    struct ChaseState : IState
    {
        ~ChaseState() override = default;
        IState* handle(const State& state) override;
        void update(Boss1* boss, float deltaTime) override;
    };
    struct LoadState : IState
    {
        ~LoadState() override = default;
        IState* handle(const State& state) override;
        void update(Boss1* boss, float deltaTime) override;

    private:
        float reloadTime = 100.0f;
        float reloadTimer = 0.0f;

    };
    struct FireState : IState
    {
        ~FireState() override = default;
        IState* handle(const State& state) override;
        void update(Boss1* boss, float deltaTime) override;

    };

    struct DestructState : IState
    {
        ~DestructState() override = default;
        IState* handle(const State& state) override;
        void update(Boss1* boss, float deltaTime) override;

    };

    struct HelpState : IState
    {
        ~HelpState() override = default;
        IState* handle(const State& state) override;
        void update(Boss1* boss, float deltaTime) override;

    };

    struct DeadState : IState
    {
        ~DeadState() override = default;
        IState* handle(const State& state) override;
        void update(Boss1* boss, float deltaTime) override;

    };

public:
    Boss1(IComposite* scene, const sf::Vector2f& spawnPosition, float maxHealth = 5000.0f);
    virtual ~Boss1();

    void ProssesInput(const sf::Event& event) override {};
    void Update(const float& deltatime) override;
    void Render() override;
    void HandleCollision(IGameObject* object) override;
    void ChangeLife(const float& life) override;

    float getMaxLife() const { return m_maxLife; }
    float getCurrentLife() const { return m_life; }
    IComposite* getScene() const { return m_scene; }

    float getSpeed();
    void setSpeed(float speed);
    sf::Vector2f getPosition() { return m_worldPosition; }
    void moveTowards(const sf::Vector2f& target, float deltaTime);
    void moveToPosition(const sf::Vector2f& position);

    virtual void fireProjectiles(int count, float spreadAngle);
    virtual void fireSpecialProjectile(ProjectileType type);

    void fireGrowingProjectile();
    void fireFastProjectile();

    virtual void activateDefensiveAbility(float duration);
    virtual void activateOffensiveBoost(float multiplier, float duration);
    virtual void regenerateHealth(float amount);

    virtual void updateEntityParameters();
    EntityPhase getCurrentPhase() const { return m_currentPhase; }

    void move(const sf::Vector2f& offset);

    void patrol();

    void changeState(const State& newState);

    IState* m_currentState;

protected:
    virtual void findTarget();
    virtual void createWeapons(float offset);
    sf::Vector2f worldToScreenPosition(const sf::Vector2f& worldPos) const;
    sf::Vector2f screenToWorldPosition(const sf::Vector2f& screenPos) const;

    float m_maxLife;
    float m_speed;


    EntityParameters m_entityParams;

    bool m_offensiveBoostActive;
    float m_damageMultiplier;
    Timer m_offensiveBoostTimer;
    float m_patrolTimer;
    Timer m_invulnerabilityTimer;
    bool m_isInvulnerable;

    EntityPhase m_currentPhase;

    IShapeSFML* m_target;

    KT::Array<ITurret*, 4> m_weapons;

    BossAI::RandomAttackSelector* m_specialAttackSelector;
    BossAI::RandomMovementSelector* m_movementSelector;

    sf::Vector2f m_worldPosition;
    float m_movementAngle;

    float m_detectionRadius;
    bool m_isTrackingTarget;

    Timer m_attackTimer;

    float calculateAngleToTarget() const;
    bool isTargetInDetectionZone() const;
    bool shouldAttackTarget() const;
    bool isTargetValid() const;


};