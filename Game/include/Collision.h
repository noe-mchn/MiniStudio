#pragma once
#include "IGameObject.h"

class IComposite;
class IComponent;
bool Collision(AABB A, AABB B);

class Colision
{
public:
	void HandleCollision(std::vector<IComponent*> objects);
private:
	bool Compose(IComponent* lhs, IComponent* rhs);
	void destroy(std::vector<IComponent*> objects);
};