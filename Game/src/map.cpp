#include "map.h"

#include <SFML/Graphics.hpp>
#include <vector>


TilesMap::TilesMap(const MapRepresentation& repr, TextureCache* texture) : m_width(0), m_height(0), m_tileSize(0), m_texture(texture)
{
    m_width = repr[0].size();
    m_height = repr.size();
    m_tileSize = 32;

    m_tiles.resize(m_width * m_height);

    for (int row = 0; row < m_height; ++row)
    {
        for (int col = 0; col < m_width; ++col)
        {
            getTile(row, col) = repr[row][col];
        }
    }


}

void TilesMap::render(sf::RenderWindow& w)
{

    sf::Texture& texture = m_texture->getTexture("preview_774.png");

    for (int row = 0; row < m_height; ++row)
    {
        for (int col = 0; col < m_width; ++col)
        {
            sf::Sprite sprite;
            sprite.setTexture(texture);

            if (getTile(row, col) == 'H')
            {
                sprite.setTextureRect(sf::IntRect(0, 0, m_tileSize, m_tileSize));
                sprite.setPosition({ static_cast<float>(m_tileSize) * col, static_cast<float>(m_tileSize) * row });
                w.draw(sprite);
            }
            else if (getTile(row, col) == 'F')
            {
                sprite.setTextureRect(sf::IntRect(0, 3 * m_tileSize, m_tileSize, m_tileSize));
                sprite.setPosition({ static_cast<float>(m_tileSize) * col, static_cast<float>(m_tileSize) * row });
                w.draw(sprite);
            }
        }
    }
}
