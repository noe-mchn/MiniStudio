#pragma once
#include "IGameObject.h"
#include "GameObject.h"
#include "BossBullet.h"
#include "Animation.h"
#include <SFML/Graphics.hpp>

enum class EnemyState {
    PATROL,
    CHASE,
    ATTACK,
    STUNNED,
    DEAD
};

class Enemy : public DestructibleObject, public IComposite {
public:
    Enemy(IComposite* scene, const sf::Vector2f& spawnPosition, float maxHealth = 200.0f);
    ~Enemy();

    void Update(const float& deltaTime) override;
    void Render() override;
    void HandleCollision(IGameObject* object) override;
    void ChangeLife(const float& life) override;

private:
    class IState {
    public:
        virtual ~IState() {}
        virtual IState* handle(const EnemyState& state) = 0;
        virtual void update(Enemy* enemy, float deltaTime) = 0;
    };

    class PatrolState : public IState {
    public:
        IState* handle(const EnemyState& state) override;
        void update(Enemy* enemy, float deltaTime) override;
    };

    class ChaseState : public IState {
    public:
        IState* handle(const EnemyState& state) override;
        void update(Enemy* enemy, float deltaTime) override;
    };

    class AttackState : public IState {
    public:
        IState* handle(const EnemyState& state) override;
        void update(Enemy* enemy, float deltaTime) override;
    private:
        float attackCooldown = 0.0f;
    };

    class StunnedState : public IState {
    public:
        IState* handle(const EnemyState& state) override;
        void update(Enemy* enemy, float deltaTime) override;
    private:
        float stunDuration = 0.0f;
    };

    class DeadState : public IState {
    public:
        IState* handle(const EnemyState& state) override;
        void update(Enemy* enemy, float deltaTime) override;
    private:
        float deathTimer = 0.0f;
    };

    void setupAnimations();
    void findTarget();
    void changeState(const EnemyState& newState);
    void patrol();
    void moveTowards(const sf::Vector2f& target, float deltaTime);
    void performMeleeAttack();
    float calculateAngleToTarget() const;
    bool isTargetInDetectionZone() const;
    sf::Vector2f worldToScreenPosition(const sf::Vector2f& worldPos) const;
    sf::Vector2f screenToWorldPosition(const sf::Vector2f& screenPos) const;
    std::string getOrientationString() const;
    Orientation determineOrientation(float angle);

    float m_maxLife;
    float m_speed;
    float m_damage;
    float m_attackRange;
    float m_attackCooldown;
    bool m_isInvulnerable;
    float m_stunDuration;
    Hero* m_target;

    sf::Vector2f m_worldPosition;
    float m_movementAngle;
    float m_detectionRadius;
    float m_patrolTimer;

    IState* m_currentState;
    AnimateSprite m_animate;
    AnimationComponent* m_animationComponent;
    Timer m_attackTimer;
    Timer m_invulnerabilityTimer;

    Orientation m_currentOrientation;
};