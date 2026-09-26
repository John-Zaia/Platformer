#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

void setupPlayer(sf::RectangleShape& player)
{
	player.setSize(sf::Vector2f(50.f, 100.f));
	player.setFillColor(sf::Color(100, 250, 50));
	player.setPosition({ 0.f, 570.f });
}

sf::RectangleShape setupPlatform( float xSize, float ySize, float xPosition, float yPosition, sf::Texture& groundTexture)
{
	sf::RectangleShape platform;
	platform.setSize({ xSize, ySize });
	platform.setFillColor(sf::Color::White);
	platform.setPosition({ xPosition, yPosition });

	platform.setTexture(&groundTexture);
	groundTexture.setRepeated(true);
	platform.setTextureRect(sf::IntRect({ 0, 0 }, { static_cast<int>(800), static_cast<int>(30) }));

	return platform;
}

sf::CircleShape setupTriangle(float radius, int points, float xPosition, float yPosition, sf::Texture& triangleTexture)
{
	sf::CircleShape triangle;
	triangle.setRadius(radius);
	triangle.setPointCount(points);
	triangle.setPosition({ xPosition, yPosition });

	triangle.setTexture(&triangleTexture);
	triangleTexture.setRepeated(true);
	triangle.setTexture(&triangleTexture);

	return triangle;
}

void playerMovement(sf::RectangleShape& player, float deltaTime, float speed)
{
	bool switchMovement = false;

	//movement for debugging
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space))
	{
		switchMovement = true;
	}

	if (switchMovement == false)
	{
		player.move({ speed * deltaTime, 0.f });
	}
	else
	{
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right) && player.getPosition().x < 750)
		{
			player.move({ speed * deltaTime, 0.f });
		}
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left) && player.getPosition().x > 0)
		{
			player.move({ -speed * deltaTime, 0.f });
		}
	}
	
}

void playerJump(float& velocityY, bool& grounded)
{
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up) && grounded)
	{
		velocityY = -400.f;
		grounded = false;
	}

}

bool detectPlatformCollision(sf::RectangleShape& player, sf::RectangleShape& platform)
{
	if (player.getGlobalBounds().findIntersection(platform.getGlobalBounds()))
	{
		return true;
	}

	return false;
}

bool detectObjectCollision(sf::RectangleShape& player, sf::CircleShape triangle)
{
	if (player.getGlobalBounds().findIntersection(triangle.getGlobalBounds()))
	{
		return true;
	}

	return false;
}

void playerDeath(sf::RectangleShape& player, std::vector<sf::CircleShape>& triangleObstacles)
{
	for (auto i : triangleObstacles)
	{
		if (detectObjectCollision(player, i))
		{
			player.setPosition({ 0.f, 570.f });
		}
	}

	if (player.getPosition().y > 1000)
	{
		player.setPosition({ 0.f, 570.f });
	}
}

void playerGravity(sf::RectangleShape& player, std::vector<sf::RectangleShape>& rectanglePlatforms, float deltaTime, float& velocityY, float gravity, bool& grounded)
{	
	grounded = false;

	for (int i = 0; i < rectanglePlatforms.size(); i++)
	{
		if (detectPlatformCollision(player, rectanglePlatforms[i]) && velocityY >= 0)
		{
			player.setPosition({
				player.getPosition().x,
				rectanglePlatforms[i].getPosition().y - player.getSize().y
				});

			velocityY = 0.f;
			grounded = true;
			break;
		}
	}

	if (!grounded)
	{
		velocityY += gravity * deltaTime;
		player.move({ 0.f, velocityY * deltaTime });
	}
}

int main()
{
	sf::RenderWindow window(sf::VideoMode({ 800, 600 }), "My Window");
	
	sf::Texture groundTexture;
	if (!groundTexture.loadFromFile("ground.jpg")) return 1;

	sf::Texture obstacleTexture;
	if (!obstacleTexture.loadFromFile("obstacle.jpg")) return 1;

	sf::RectangleShape player;
	setupPlayer(player);

	sf::View camera(sf::Vector2f(0.f, 0.f), sf::Vector2f(800.f, 600.f));

	std::vector<sf::RectangleShape> rectangePlatforms;
	std::vector<sf::CircleShape> triangleObstacles;

	rectangePlatforms.push_back(setupPlatform(800.f, 30.f, 0.f, 570.f, groundTexture));
	triangleObstacles.push_back(setupTriangle(20.f, 3, 400.f, 540.f, obstacleTexture));

	sf::Clock clock;
	float speed = 200.f;
	float velocityY = 0.f;
	float gravity = 980.f;
	bool grounded = false;

	while (window.isOpen())
	{
		float deltaTime = clock.restart().asSeconds();

		playerGravity(player, rectangePlatforms, deltaTime, velocityY, gravity, grounded);
		playerDeath(player, triangleObstacles);

		while (const std::optional event = window.pollEvent())
		{
			if (event->is<sf::Event::Closed>())
				window.close();
		}

		playerJump(velocityY, grounded);
		playerMovement(player, deltaTime, speed);

		camera.setCenter(player.getPosition());
		window.setView(camera);

		window.clear(sf::Color::Black);
		window.draw(player);

		for (auto& i : rectangePlatforms)
		{
			window.draw(i);
		}

		for (auto& i : triangleObstacles)
		{
			window.draw(i);
		}

		window.display();
	}
}
