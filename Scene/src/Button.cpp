#include "Button.h"

Button::Button(const sf::Vector2f& position, const sf::Vector2f& size,
    const std::string& text, const sf::Font& font, unsigned int fontSize)
    : m_state(State::Normal)
    , m_normalColor(sf::Color(100, 100, 100))
    , m_hoverColor(sf::Color(150, 150, 150))
    , m_pressedColor(sf::Color(50, 50, 50))
    , m_textNormalColor(sf::Color::White)
    , m_textHoverColor(sf::Color::White)
    , m_textPressedColor(sf::Color::White)
{
    m_shape.setPosition(position);
    m_shape.setSize(size);
    m_shape.setFillColor(m_normalColor);
    m_shape.setOutlineThickness(2);
    m_shape.setOutlineColor(sf::Color::Black);

    m_text.setFont(font);
    m_text.setString(text);
    m_text.setCharacterSize(fontSize);
    m_text.setFillColor(m_textNormalColor);

    sf::FloatRect textBounds = m_text.getLocalBounds();
    m_text.setOrigin(textBounds.left + textBounds.width / 2.0f,
        textBounds.top + textBounds.height / 2.0f);
    m_text.setPosition(position.x + size.x / 2.0f, position.y + size.y / 2.0f);
}

void Button::setPosition(const sf::Vector2f& position)
{
    m_shape.setPosition(position);
    sf::FloatRect textBounds = m_text.getLocalBounds();
    m_text.setPosition(position.x + m_shape.getSize().x / 2.0f,
        position.y + m_shape.getSize().y / 2.0f);
}

void Button::setSize(const sf::Vector2f& size)
{
    m_shape.setSize(size);
    sf::Vector2f position = m_shape.getPosition();
    sf::FloatRect textBounds = m_text.getLocalBounds();
    m_text.setPosition(position.x + size.x / 2.0f, position.y + size.y / 2.0f);
}

void Button::setText(const std::string& text)
{
    m_text.setString(text);
    sf::FloatRect textBounds = m_text.getLocalBounds();
    m_text.setOrigin(textBounds.left + textBounds.width / 2.0f,
        textBounds.top + textBounds.height / 2.0f);
    sf::Vector2f position = m_shape.getPosition();
    sf::Vector2f size = m_shape.getSize();
    m_text.setPosition(position.x + size.x / 2.0f, position.y + size.y / 2.0f);
}

void Button::setColors(const sf::Color& normal, const sf::Color& hover, const sf::Color& pressed)
{
    m_normalColor = normal;
    m_hoverColor = hover;
    m_pressedColor = pressed;
    updateVisuals();
}

void Button::setTextColors(const sf::Color& normal, const sf::Color& hover, const sf::Color& pressed)
{
    m_textNormalColor = normal;
    m_textHoverColor = hover;
    m_textPressedColor = pressed;
    updateVisuals();
}

void Button::setCallback(const std::function<void()>& callback)
{
    m_callback = callback;
}

bool Button::contains(const sf::Vector2f& point) const
{
    return m_shape.getGlobalBounds().contains(point);
}

void Button::update(const sf::Vector2f& mousePosition)
{
    if (contains(mousePosition))
    {
        if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
        {
            m_state = State::Pressed;
        }
        else
        {
            m_state = State::Hover;
        }
    }
    else
    {
        m_state = State::Normal;
    }

    updateVisuals();
}

void Button::handleEvent(const sf::Event& event, const sf::Vector2f& mousePosition)
{
    if (event.type == sf::Event::MouseButtonReleased &&
        event.mouseButton.button == sf::Mouse::Left)
    {
        if (contains(mousePosition) && m_state == State::Pressed && m_callback)
        {
            m_callback();
        }
    }
}

void Button::draw(sf::RenderWindow& window)
{
    window.draw(m_shape);
    window.draw(m_text);
}

void Button::updateVisuals()
{
    switch (m_state)
    {
    case State::Normal:
        m_shape.setFillColor(m_normalColor);
        m_text.setFillColor(m_textNormalColor);
        break;
    case State::Hover:
        m_shape.setFillColor(m_hoverColor);
        m_text.setFillColor(m_textHoverColor);
        break;
    case State::Pressed:
        m_shape.setFillColor(m_pressedColor);
        m_text.setFillColor(m_textPressedColor);
        break;
    }
}