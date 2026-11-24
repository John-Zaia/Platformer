#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>

#include "Player.h"
#include "Portal.h"

class Level
{
private:
	std::vector<sf::RectangleShape> platforms;
	std::vector<sf::CircleShape> triangles;
	std::vector<Portal> portals;

	float finishPosition;
public:
	Level(float finishPosition);

	void addPlatform(float xSize, float ySize, float xPosition, float yPosition, sf::Texture& groundTexture);
	void addTriangle(float radius, int points, float xPosition, float yPosition, float rotation, sf::Texture& triangleTexture);
	void addPortal(float xPosition, float yPosition, float xScale, float yScale, sf::Texture& portalTexture);
	void update(Player& player, float deltaTime, sf::Sound& portalSound);
	void draw(sf::RenderWindow& window);
	bool isComplete(const Player& player) const;
};