#include "Level.h"

Level::Level(float finishPosition) : finishPosition(finishPosition)
{
}

void Level::addPlatform(float xSize, float ySize, float xPosition, float yPosition, sf::Texture& groundTexture)
{
	sf::RectangleShape platform;
	platform.setSize({ xSize, ySize });
	platform.setFillColor(sf::Color::White);
	platform.setPosition({ xPosition, yPosition });

	platform.setTexture(&groundTexture);
	groundTexture.setRepeated(true);
	platform.setTextureRect(sf::IntRect({ 0, 0 }, { static_cast<int>(xSize), static_cast<int>(ySize) }));

	platforms.push_back(platform);
}

void Level::addTriangle(float radius, int points, float xPosition, float yPosition, float rotation, sf::Texture& triangleTexture)
{
	sf::CircleShape triangle;
	triangle.setRadius(radius);
	triangle.setPointCount(points);
	triangle.setPosition({ xPosition, yPosition });
	triangle.setRotation(sf::degrees(rotation));

	triangle.setTexture(&triangleTexture);
	triangleTexture.setRepeated(true);

	triangles.push_back(triangle);
}

void Level::addPortal(float xPosition, float yPosition, float xScale, float yScale, sf::Texture& portalTexture)
{
	portals.emplace_back(xPosition, yPosition, xScale, yScale, portalTexture);
}

void Level::update(Player& player, float deltaTime, sf::Sound& portalSounds)
{
	player.update(deltaTime, platforms, triangles);

	for (auto& portal : portals)
	{
		portal.update(player, portalSounds);
	}
}

void Level::draw(sf::RenderWindow & window)
{
	for (auto& portal : portals)
	{
		portal.draw(window);
	}

	for (auto& platform : platforms)
	{
		window.draw(platform);
	}

	for (auto& triangle : triangles)
	{
		window.draw(triangle);
	}
}

bool Level::isComplete(const Player& player) const
{
	return player.getPosition().x >= finishPosition;
}