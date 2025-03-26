#pragma once
#include <SFML/Graphics.hpp>
#include <functional>
#include <string>

class Button
{
public:
    Button(const sf::Vector2f& position, const sf::Vector2f& size,
        const std::string& text, const sf::Font& font, unsigned int fontSize = 24);

    void setPosition(const sf::Vector2f& position);
    void setSize(const sf::Vector2f& size);
    void setText(const std::string& text);
    void setColors(const sf::Color& normal, const sf::Color& hover, const sf::Color& pressed);
    void setTextColors(const sf::Color& normal, const sf::Color& hover, const sf::Color& pressed);
    void setCallback(const std::function<void()>& callback);

    bool contains(const sf::Vector2f& point) const;
    void update(const sf::Vector2f& mousePosition);
    void handleEvent(const sf::Event& event, const sf::Vector2f& mousePosition);
    void draw(sf::RenderWindow& window);

private:
    enum class State
    {
        Normal,
        Hover,
        Pressed
    };

    sf::RectangleShape m_shape;
    sf::Text m_text;
    std::function<void()> m_callback;
    State m_state;

    sf::Color m_normalColor;
    sf::Color m_hoverColor;
    sf::Color m_pressedColor;

    sf::Color m_textNormalColor;
    sf::Color m_textHoverColor;
    sf::Color m_textPressedColor;

    void updateVisuals();
};