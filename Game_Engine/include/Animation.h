#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <unordered_map>
#include "IGameObject.h"
#include "TextureCache.h"


enum class Orientation {
    DOWN,
    UP,
    LEFT,
    RIGHT
};

class Animation
{
public:
    Animation();
    Animation(const std::string& textureName, int frameCount, float frameTime, bool looping = true);

    void update(float deltaTime);
    void reset();

    void setTexture(const std::string& textureName);
    void setFrameCount(int frameCount);
    void setFrameTime(float frameTime);
    void setLooping(bool looping);
    void setFrameSize(const sf::Vector2i& frameSize);

    void setStartPosition(const sf::Vector2i& startPos, int rowCount = 1);
    sf::Vector2i getFrameSize() const { return m_frameSize; }
    void setOrientation(Orientation orientation);

    bool isFinished() const;
    int getCurrentFrame() const;
    sf::IntRect getCurrentFrameRect() const;

    std::string m_textureName;

private:
    int m_frameCount;
    float m_frameTime;
    float m_currentTime;
    int m_currentFrame;
    bool m_looping;
    bool m_finished;
    sf::Vector2i m_frameSize;
    sf::Vector2i m_startPosition;
    int m_rowCount;
    int m_currentRow;
};


class AnimationComponent : public ILeaf
{
public:
    AnimationComponent(IComposite* parent);
    ~AnimationComponent() override = default;

    void Update(const float& deltaTime) override;
    void ProcessInput(const sf::Event& event) override;
    void Render() override;

    void addAnimation(const std::string& name, const Animation& animation);
    void playAnimation(const std::string& name);
    void stopAnimation();

    void updatePosition(const sf::Vector2f& position);
    void setScale(const sf::Vector2f& scale);
    sf::Vector2f getScale() const;

    bool isAnimationFinished() const;
    std::string getCurrentAnimation() const;
    sf::Sprite& getSprite() { return m_sprite; }
    std::unordered_map<std::string, Animation>& getAnimations() { return m_animations; }

private:
    std::unordered_map<std::string, Animation> m_animations;
    std::string m_currentAnimation;
    sf::Sprite m_sprite;
    sf::Vector2f m_scale;
    bool m_playing;
};


class AnimatedGameObject : public DestructibleObject
{
public:
    AnimatedGameObject(IComposite* scene, const float& life = 100.0f);
    virtual ~AnimatedGameObject() = default;

    void Update(const float& deltaTime) override;
    void ProcessInput(const sf::Event& event) override;
    void Render() override;

    AnimationComponent* getAnimationComponent() { return m_animationComponent; }

protected:
    AnimationComponent* m_animationComponent;
};