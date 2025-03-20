#pragma once
#include "SceneBase.h"
#include <iostream>

class IPhysics
{
public:
	virtual ~IPhysics() = default;
};

class AnimateSprite
{
public:
	AnimateSprite(std::initializer_list<std::string> init);
	void add(std::string toBeAdded);
	std::string getPath(const std::string& check);
	std::string getPath(const int& idx);
	std::string getCurrentPath();
	void resetTexture();
	void ChangeToNextPath();
	void ChangeToPreviousPath();
	void ChangePath(const int& idx);

private:
	KT::Vector<std::string> m_textureContainer;
	int m_curentTexture;
};

class IShapeSFML
{
public:
	virtual ~IShapeSFML() = default;
	virtual sf::Vector2f getSize() = 0;
	virtual sf::Vector2f getPosition() = 0;
	virtual float getangle() = 0;
	virtual void setPosition(const sf::Vector2f&) = 0;
	virtual void setSize(const sf::Vector2f&) = 0;
	virtual void setRotation(const float& angle) = 0;
	virtual void setTexture(const sf::Texture& texture) = 0;
	virtual void setTextureRect(const sf::IntRect& rect) = 0;
	virtual void setCenter(sf::Vector2f) = 0;
	virtual sf::Vector2f getCenter() = 0;

	virtual AABB GetBoundingBox() = 0;
};



class RectangleSFML : public IShapeSFML
{
public:
	RectangleSFML(float width, float height, sf::Vector2f position, sf::Vector2f Origin);

	RectangleSFML(float width, float height, sf::Vector2f position);

	RectangleSFML(float width, float height, ISceneBase* scene);


	sf::Vector2f getPosition() override;

	sf::Vector2f getSize() override;

	float getangle() override;

	void setTexture(const sf::Texture& texture) override;


	void setTextureRect(const sf::IntRect& rect) override;

	void setPosition(const sf::Vector2f& position) override;

	void setSize(const sf::Vector2f& size)override;

	void setRotation(const float& angle) override;

	void setCenter(sf::Vector2f) override;
	sf::Vector2f getCenter() override;

	sf::RectangleShape& getShape();

	AABB GetBoundingBox()override;

protected:
	sf::RectangleShape m_shape;
};

class SquareSFML : public RectangleSFML
{
public:
	SquareSFML(float size, sf::Vector2f position, sf::Vector2f Origin);

	SquareSFML(float size, sf::Vector2f position);

	SquareSFML(float size, ISceneBase* scene);
};

class CircleSFML : public IShapeSFML
{
public:
	CircleSFML(float r, sf::Vector2f position, sf::Vector2f Origin);

	CircleSFML(float r, sf::Vector2f position);

	CircleSFML(float r, ISceneBase* scene);

	sf::Vector2f getPosition() override;

	sf::Vector2f getSize() override;

	float getangle() override;

	void setTexture(const sf::Texture& texture) override;

	void setTextureRect(const sf::IntRect& rect) override;

	void setPosition(const sf::Vector2f& position) override;

	void setSize(const sf::Vector2f& size)override;

	void setRotation(const float& angle) override;

	void setCenter(sf::Vector2f) override;
	sf::Vector2f getCenter() override;

	sf::CircleShape& getShape();
	AABB GetBoundingBox()override;

protected:
	sf::CircleShape m_shape;
};

class Timer
{
public:
	Timer(const float& timer) :m_TotalTimer(timer), m_CurrentTimer(0), m_start(true) {}
	bool AutoActionIsReady(const float& framerate)
	{
		if (m_CurrentTimer >= m_TotalTimer)
		{
			resetTimer();
			return true;
		}
		NextTIck(framerate);
		return false;
	}
	bool ActionIsReady()
	{
		if (m_CurrentTimer >= m_TotalTimer)
			return true;
		return false;
	}
	void resetTimer()
	{
		m_CurrentTimer = 0;
	}
	void setNewTimer(const float& timer)
	{
		m_TotalTimer = timer;
	}
	float getTotalTimer()
	{
		return m_TotalTimer;
	}
	float getCurrentTimer()
	{
		return m_CurrentTimer;
	}
	void NextTIck(const float& framerate, const float& idx = 1)
	{
		if (!m_start)
			return;

		m_CurrentTimer += framerate * idx;
		if (m_CurrentTimer >= m_TotalTimer)
			m_CurrentTimer = m_TotalTimer;
	}
	void PreviousTick(const float& framerate, const float& idx = 1)
	{
		if (!m_start)
			return;
		m_CurrentTimer -= framerate * idx;
		if (m_CurrentTimer <= 0)
			m_CurrentTimer = 0;
	}
	void Start()
	{
		m_start = true;
	}
	void Stop()
	{
		m_start = false;
	}
private:
	float m_TotalTimer;
	float m_CurrentTimer;
	bool m_start;
};



class Counter
{
public:
	Counter(const float& count, const float& min = std::numeric_limits<float>::min()) :m_TotalCounter(count), m_minimalCounter(min), m_CurrentCounter(0), m_start(true) {}
	Counter() :m_TotalCounter(std::numeric_limits<float>::max()), m_minimalCounter(std::numeric_limits<float>::min()), m_CurrentCounter(0), m_start(true) {}

	bool AutCounterMax()
	{
		if (m_CurrentCounter >= m_TotalCounter)
		{
			resetCounter();
			return true;
		}
		NextTIck();
		return false;
	}
	bool CounterMax()
	{
		if (m_CurrentCounter >= m_TotalCounter)
			return true;
		return false;
	}
	void resetCounter()
	{
		m_CurrentCounter = 0;
	}
	void setNewCounter(const float& max, const float& min)
	{
		m_TotalCounter = max;
		m_minimalCounter = min;
	}
	float getTotalCounter()
	{
		return m_TotalCounter;
	}
	float GetCurrentCounter()
	{
		return m_CurrentCounter;
	}
	void NextTIck(const float& idx = 1)
	{
		if (!m_start)
			return;

		m_CurrentCounter += idx;
		if (m_CurrentCounter >= m_TotalCounter)
			m_CurrentCounter = m_TotalCounter;
	}
	void PreviousTick(const float& idx = 1)
	{
		if (!m_start)
			return;
		m_CurrentCounter -= idx;
		if (m_CurrentCounter <= m_minimalCounter)
			m_CurrentCounter = m_minimalCounter;
	}
	void Start()
	{
		m_start = true;
	}
	void Stop()
	{
		m_start = false;
	}
private:
	float m_TotalCounter;
	float m_CurrentCounter;
	float m_minimalCounter;
	bool m_start;
};


class SpriteCutter
{
public:
	SpriteCutter(sf::Texture& texture, const unsigned int& col, const unsigned int& row, const unsigned int& widthI, const unsigned int& heightI, const unsigned int& deadZoneX, const unsigned int& deadZoneY)
		: m_col(col)
		, m_row(row)
		, m_widthI(widthI)
		, m_heightI(heightI)
		, m_deadZoneX(deadZoneX)
		, m_deadZoneY(deadZoneY)
		, m_texture(texture)
	{
		m_widthI = (widthI - row * deadZoneX) / row;
		m_heightI = (heightI - col * deadZoneY) / col;
	}

	sf::IntRect getFrame(const unsigned int& index)
	{
		if (index > (m_col * m_row))
			throw;

		auto realIndex = index - 1;

		auto row = realIndex / m_col;
		auto col = realIndex % m_col;

		// nbr row et col
		auto x = row * (m_deadZoneX + m_widthI);
		auto y = col * (m_deadZoneY + m_heightI);

		return sf::IntRect(x, y, m_widthI, m_heightI);
	}

	sf::Texture& getTexture()
	{
		return m_texture;
	}


private:
	int m_col;
	int m_row;
	int m_widthI;
	int m_heightI;
	int m_deadZoneX;
	int m_deadZoneY;
	sf::Texture m_texture;
};

class Animat2_0
{
public:
	Animat2_0(SpriteCutter* sprite, const unsigned int& min, const unsigned int& max, Timer time)
		: m_spritecutt(sprite)
		, m_counter(min, max)
		, m_time(time)
	{
	}

	void update(const float& deltatime, IShapeSFML* shape)
	{

		if (m_time.getCurrentTimer() >= m_time.getTotalTimer())
		{
			m_time.resetTimer();
			m_counter.NextTIck();
		}
		else
		{
			shape->setTextureRect(m_spritecutt->getFrame(m_counter.GetCurrentCounter()));
			m_time.AutoActionIsReady(deltatime);
		}
	}

	void initTexture(IShapeSFML* shape)
	{
		shape->setTexture(m_spritecutt->getTexture());
	}

	

private:
	SpriteCutter* m_spritecutt;
	Timer m_time;
	Counter m_counter;
};
