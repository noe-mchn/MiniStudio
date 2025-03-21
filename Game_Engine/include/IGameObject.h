#pragma once
#include "SFML/Graphics.hpp"
#include <vector>
#include <memory>

struct AABB {
    AABB(sf::Vector2f amin, sf::Vector2f amax) : Amin(amin), Amax(amax) {}
    sf::Vector2f Amin;
    sf::Vector2f Amax;

    bool Intersects(const AABB& other) const {
        return !(Amax.x < other.Amin.x || Amin.x > other.Amax.x ||
            Amax.y < other.Amin.y || Amin.y > other.Amax.y);
    }
};

inline float convertRadToDeg(const float& rad) { return (180.0f * rad) / 3.14159f; }
inline float convertDegToRad(const float& deg) { return (deg * 3.14159f) / 180.0f; }

class IShapeSFML;
class RootScene;
class ISceneBase;
class IComposite;

enum class Component {
    IComposite,
    ILeaf,
    IGameObject
};

class IComponent {
public:
    IComponent(IComposite* parent);
    virtual ~IComponent();

    IComponent* getParent();
    const IComponent* getParent() const;

    virtual void Update(const float& deltatime) = 0;
    virtual void ProcessInput(const sf::Event& event) = 0;
    virtual void Render() = 0;
    virtual Component GetComponentType() = 0;
    virtual const Component GetComponentType() const = 0;

    RootScene* getRoot();
    const RootScene* getRoot() const;

    void setParent(IComposite* parent);

protected:
    IComposite* m_parent;
};

class IComposite : public IComponent {
public:
    friend class IComponent;

    IComposite(IComposite* parent);
    ~IComposite();

    void Update(const float& deltatime) override;
    void ProcessInput(const sf::Event& event) override;
    void Render() override;

    std::vector<IComponent*> getChildren();
    const std::vector<IComponent*> getChildren() const;
    std::vector<IComponent*> getFullTree();

protected:
    Component GetComponentType() override {
        return Component::IComposite;
    }
    const Component GetComponentType() const override {
        return Component::IComposite;
    }

private:
    void add(IComponent* data);
    void remove(IComponent* data);
    void AddFullTree(std::vector<IComponent*>& toAdd, std::vector<IComponent*> iterate);

    std::vector<IComponent*> m_children;
};

class RootScene : public IComposite {
public:
    RootScene(ISceneBase* scene);
    ISceneBase* getScene();
    const ISceneBase* getScene() const { return m_scene; }

private:
    ISceneBase* m_scene;
};

class ILeaf : public IComponent {
public:
    ILeaf(IComposite* parent);

    virtual void Update(const float& deltatime) = 0;
    virtual void ProcessInput(const sf::Event& event) = 0;
    virtual void Render() = 0;

    Component GetComponentType() override {
        return Component::ILeaf;
    }
    const Component GetComponentType() const override {
        return Component::ILeaf;
    }
};

enum class GameObjectType {
    DestructibleObject,
    NonDestructibleObject
};

class IGameObject {
public:
    IGameObject(IComposite* scene);
    virtual ~IGameObject();

    virtual void Update(const float& deltatime) = 0;
    virtual void ProcessInput(const sf::Event& event) = 0;
    virtual void Render() = 0;

    virtual AABB GetBoundingBox();
    IShapeSFML* getShape();
    const IShapeSFML* getShape() const { return m_shape; }

    virtual GameObjectType globalGameObjectType() = 0;
    virtual void HandleCollision(IGameObject* object) {}

    bool NeedDestroy();
    void destroy();

protected:
    IComposite* m_scene;
    IShapeSFML* m_shape;

private:
    bool m_needDestroy;
};

class DestructibleObject : public IGameObject {
public:
    DestructibleObject(IComposite* scene, const float& life);

    virtual void Update(const float& deltatime) override = 0;
    virtual void ProcessInput(const sf::Event& event) override = 0;
    virtual void Render() override = 0;

    virtual void ChangeLife(const float& life) {
        m_life += life;
        if (m_life <= 0)
            destroy();
    }

    float getCurrentLife() { return m_life; }
    GameObjectType globalGameObjectType() override;

protected:
    float m_life;
};

class NonDestructibleObject : public IGameObject {
public:
    NonDestructibleObject(IComposite* scene);

    virtual void Update(const float& deltatime) override = 0;
    virtual void ProcessInput(const sf::Event& event) override = 0;
    virtual void Render() override = 0;

    GameObjectType globalGameObjectType() override;
};

template<typename type, typename type2>
type getObj(type2* obj) {
    return dynamic_cast<type>(obj);
}