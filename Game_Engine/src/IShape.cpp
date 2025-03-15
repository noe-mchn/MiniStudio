#include "IShape.h"

AnimateSprite::AnimateSprite(std::initializer_list<std::string> init) : m_curentTexture(0)
{
	for (auto& idx : init)
	{
		m_textureContainer.pushBack(idx);
	}
}

void AnimateSprite::add(std::string toBeAdded)
{
	m_textureContainer.pushBack(toBeAdded);
}

std::string AnimateSprite::getPath(const std::string& check)
{
	if (m_textureContainer.Empty())
		throw std::out_of_range("Vector is empty");
	for (auto& idx : m_textureContainer)
	{
		if (idx == check)
			return idx;
	}
	throw std::runtime_error("Impossible to find path");
}

std::string AnimateSprite::getPath(const int& idx)
{
	if (m_textureContainer.Empty())
		throw std::out_of_range("Vector is empty");
	if (idx >= m_textureContainer.Size())
		throw std::out_of_range("Out of range");
	return m_textureContainer[idx];
}

std::string AnimateSprite::getCurrentPath()
{
	return m_textureContainer[m_curentTexture];
}

void AnimateSprite::resetTexture()
{
	m_textureContainer.clear();
	m_curentTexture = 0;
}

void AnimateSprite::ChangeToNextPath()
{
	if (m_textureContainer.Empty())
		throw std::out_of_range("Vector is empty");
	if (m_curentTexture == m_textureContainer.Size() - 1)
		m_curentTexture = 0;
	else
		++m_curentTexture;
}

void AnimateSprite::ChangePath(const int& idx)
{
	if (idx < 0 || idx >= m_textureContainer.Size())
		throw std::runtime_error("Out of range");
	m_curentTexture = idx;
}

void AnimateSprite::ChangeToPreviousPath()
{
	if (m_textureContainer.Empty())
		throw std::out_of_range("Vector is empty");
	if (m_curentTexture == 0)
		m_curentTexture = m_textureContainer.Size() - 1;
	else
		--m_curentTexture;
}
//
//Animate::Animate(const std::string& texturePath, int cols, int rows)
//	: m_texture(texturePath), m_cols(cols), m_rows(rows), m_curentTexture(0)
//{
//	sf::Texture tempTexture;
//	if (!tempTexture.loadFromFile(texturePath)) 
//	{
//		throw std::runtime_error("Impossible de charger la texture: " + texturePath);
//	}
//
//	sf::Vector2u textureSize = tempTexture.getSize();
//	divideSheet(textureSize.x, textureSize.y);
//}
//
//std::string Animate::getTexture()
//{
//	return m_texture;
//}
//
//void Animate::divideSheet(int sheetWidth, int sheetHeight)
//{
//	if (m_cols == 0 || m_rows == 0) return;
//
//	m_widthSpriteWithDeadZone = static_cast<float>(sheetWidth) / m_cols;
//	m_heightSpriteWithDeadZone = static_cast<float>(sheetHeight) / m_rows;
//
//	deadZone(m_widthSpriteWithDeadZone - 2, m_heightSpriteWithDeadZone - 2);
//}
//
//void Animate::deadZone(float spriteWidth, float spriteHeight)
//{
//	m_widthDeadZone = (m_widthSpriteWithDeadZone - spriteWidth) / 2.0f;
//	m_heightDeadZone = (m_heightSpriteWithDeadZone - spriteHeight) / 2.0f;
//}
//
//void Animate::firstSprite()
//{
//	m_currentCol = 0;
//	m_curentTexture = m_currentRow * m_cols;
//	updateSpriteCoordinates();
//}
//
//void Animate::changeToNextSprite()
//{
//	m_curentTexture = (m_curentTexture + 1) % (m_cols * m_rows);
//	updateSpriteCoordinates();
//}
//
//sf::IntRect Animate::getTextureRect() const
//{
//	int col = m_curentTexture % m_cols;
//	int row = m_curentTexture / m_cols;
//
//	return sf::IntRect
//	(static_cast<int>(col * m_widthSpriteWithDeadZone + m_widthDeadZone),
//		static_cast<int>(row * m_heightSpriteWithDeadZone + m_heightDeadZone),
//		static_cast<int>(m_widthSpriteWithDeadZone - 2 * m_widthDeadZone),
//		static_cast<int>(m_heightSpriteWithDeadZone - 2 * m_heightDeadZone)
//	);
//}
//
//void Animate::update(float deltaTime)
//{
//	m_animationTimer += deltaTime;
//	if (m_animationTimer >= m_animationSpeed)
//	{
//		m_animationTimer = 0.0f;
//		changeToNextSprite();
//	}
//
//}
//
//void Animate::updateSpriteCoordinates()
//{
//	int col = m_curentTexture % m_cols;
//	int row = m_curentTexture / m_cols;
//
//	m_currentRect = sf::IntRect(
//		static_cast<int>(col * m_widthSpriteWithDeadZone + m_widthDeadZone),
//		static_cast<int>(row * m_heightSpriteWithDeadZone + m_heightDeadZone),
//		static_cast<int>(m_widthSpriteWithDeadZone - 2 * m_widthDeadZone),
//		static_cast<int>(m_heightSpriteWithDeadZone - 2 * m_heightDeadZone)
//	);
//}
//
//void Animate::setAnimationRow(int row)
//{
//	if (row >= 0 && row < m_rows) {
//		m_currentRow = row;
//		m_currentCol = 0;
//		m_curentTexture = m_currentRow * m_cols;
//	}
//}



RectangleSFML::RectangleSFML(float width, float height, sf::Vector2f position, sf::Vector2f Origin) :m_shape(sf::Vector2f(width, height))
{
	m_shape.setPosition(position);
	m_shape.setOrigin(Origin);
}

RectangleSFML::RectangleSFML(float width, float height, sf::Vector2f position) :m_shape(sf::Vector2f(width, height))
{
	m_shape.setPosition(position);
	m_shape.setOrigin(m_shape.getSize().x / 2, m_shape.getSize().y / 2);
}

RectangleSFML::RectangleSFML(float width, float height, ISceneBase* scene) :m_shape(sf::Vector2f(width, height))
{
	m_shape.setPosition(scene->getWindow()->getSize().x / 2, scene->getWindow()->getSize().y / 2);
	m_shape.setOrigin(m_shape.getSize().x / 2, m_shape.getSize().y / 2);
}

sf::RectangleShape& RectangleSFML::getShape()
{
	return m_shape;
}

AABB RectangleSFML::GetBoundingBox()
{
	AABB boundingbox{sf::Vector2f(0,0),sf::Vector2f(0,0) };
	boundingbox.Amin.x = m_shape.getPosition().x - m_shape.getSize().x / 2;
	boundingbox.Amin.y = m_shape.getPosition().y - m_shape.getSize().y / 2;

	boundingbox.Amax.x = m_shape.getPosition().x + m_shape.getSize().x / 2;
	boundingbox.Amax.y = m_shape.getPosition().y + m_shape.getSize().y / 2;

	
	return boundingbox;
}

sf::Vector2f RectangleSFML::getPosition()
{
	return m_shape.getPosition();
}

sf::Vector2f RectangleSFML::getSize()
{
	return m_shape.getSize();
}

float RectangleSFML::getangle()
{
	return m_shape.getRotation();
}

void RectangleSFML::setTexture(const sf::Texture& texture)
{
	m_shape.setTexture(&texture);
}

void RectangleSFML::setTextureRect(const sf::IntRect& texture)
{
	m_shape.setTextureRect(texture);
}

void RectangleSFML::setPosition(const sf::Vector2f& position)
{
	m_shape.setPosition(position);
}

void RectangleSFML::setSize(const sf::Vector2f& size)
{
	m_shape.setSize(size);
}

void RectangleSFML::setRotation(const float& angle)
{
	m_shape.setRotation(angle);
}

void RectangleSFML::setCenter(sf::Vector2f vec)
{
	m_shape.setOrigin(vec);
}

sf::Vector2f RectangleSFML::getCenter()
{
	return m_shape.getOrigin();
}



SquareSFML::SquareSFML(float size, sf::Vector2f position, sf::Vector2f Origin) :RectangleSFML(size, size, position, Origin)
{
	m_shape.setPosition(position);
	m_shape.setOrigin(Origin);
}

SquareSFML::SquareSFML(float size, sf::Vector2f position) :RectangleSFML(size, size, position)
{
	m_shape.setPosition(position);
	m_shape.setOrigin(m_shape.getSize().x / 2, m_shape.getSize().y / 2);
}

SquareSFML::SquareSFML(float size, ISceneBase* scene) :RectangleSFML(size, size, scene)
{
	m_shape.setPosition(scene->getWindow()->getSize().x / 2, scene->getWindow()->getSize().y / 2);
	m_shape.setOrigin(m_shape.getSize().x / 2, m_shape.getSize().y / 2);
}

CircleSFML::CircleSFML(float r, sf::Vector2f position, sf::Vector2f Origin) :m_shape(r)
{
	m_shape.setPosition(sf::Vector2f(position));
	m_shape.setOrigin(Origin);
}

CircleSFML::CircleSFML(float r, sf::Vector2f position) :m_shape(r)
{
	m_shape.setPosition(position);
	m_shape.setOrigin(m_shape.getRadius(), m_shape.getRadius());
}

CircleSFML::CircleSFML(float r, ISceneBase* scene) :m_shape(r)
{
	m_shape.setPosition(scene->getWindow()->getSize().x / 2, scene->getWindow()->getSize().y / 2);
	m_shape.setOrigin(m_shape.getRadius(), m_shape.getRadius());
}

sf::CircleShape& CircleSFML::getShape()
{
	return m_shape;
}

AABB CircleSFML::GetBoundingBox()
{
	AABB boundingbox{ sf::Vector2f(0,0),sf::Vector2f(0,0) };
	boundingbox.Amin.x = m_shape.getPosition().x - m_shape.getRadius();
	boundingbox.Amin.y = m_shape.getPosition().y - m_shape.getRadius();

	boundingbox.Amax.x = m_shape.getPosition().x + m_shape.getRadius();
	boundingbox.Amax.y = m_shape.getPosition().y + m_shape.getRadius();

	return boundingbox;
}

sf::Vector2f CircleSFML::getPosition()
{
	return m_shape.getPosition();
}

sf::Vector2f CircleSFML::getSize()
{
	return sf::Vector2f{ m_shape.getRadius() * 2, m_shape.getRadius() * 2 };
}

float CircleSFML::getangle()
{
	return m_shape.getRotation();
}

void CircleSFML::setTexture(const sf::Texture& texture)
{
	m_shape.setTexture(&texture);
}

void CircleSFML::setTextureRect(const sf::IntRect& texture)
{
	m_shape.setTextureRect(texture);
}

void CircleSFML::setPosition(const sf::Vector2f& position)
{
	m_shape.setPosition(position);
}

void CircleSFML::setSize(const sf::Vector2f& size)
{
	if (size.x != size.y)
		throw std::runtime_error("Size must be equal");
	m_shape.setRadius(size.x / 2);
}

void CircleSFML::setRotation(const float& angle)
{
	m_shape.setRotation(angle);
}

void CircleSFML::setCenter(sf::Vector2f vec)
{
	m_shape.setOrigin(vec);
}
sf::Vector2f CircleSFML::getCenter()
{
	return m_shape.getOrigin();
}