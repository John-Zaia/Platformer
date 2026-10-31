#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>

class Player
{
private:

	sf::Texture playerRunTexture;
	sf::Texture playerRunUpsideDownTexture;
	sf::Texture playerJumpTexture;
	sf::Texture playerJumpUpsideDownTexture;

	sf::RectangleShape player;

	float velocityY = 0.f;
	float gravity = 980.f;
	float speed = 400.f;
	bool grounded = false;
	bool gravityReversed = false;

	sf::Clock animationClock;
	int playerFrame = 0;

	bool dead = false;
	sf::Clock deathClock;
	int totalDeaths = 0;

	void movement(float deltaTime);
	void jump();
	void applyGravity(float deltaTime, const std::vector<sf::RectangleShape>& platforms);

	void updateAnimation();
	bool detectDeath(const std::vector<sf::CircleShape>& obstacles);

	void handleDeath();

public:
	Player();

	bool loadTextures();

	void update(
		float deltaTime,
		const std::vector<sf::RectangleShape>& platforms,
		const std::vector<sf::CircleShape>& obstacles
	);

	void draw(sf::RenderWindow& window);

	void reset();


	sf::Vector2f getPosition() const;
	sf::FloatRect getBounds() const;

	int getTotalDeaths() const;
	bool isGravityReversed() const;

	void activateGravityPortal();
};