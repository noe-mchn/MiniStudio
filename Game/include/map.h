#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include "TextureCache.h"

using MapRepresentation = std::vector<std::string>;

class TilesMap
{
public:
	TilesMap(const MapRepresentation& repr, TextureCache* texture);
	void render(sf::RenderWindow& w);

	using TileType = char;

private:
	TileType& getTile(int row, int col) { return m_tiles[row * m_width + col]; }
	const TileType& getTile(int row, int col) const { return m_tiles[row * m_width + col]; }

	std::vector<TileType> m_tiles;


	int m_width;
	int m_height;
	int m_tileSize;

	TextureCache* m_texture;
};
