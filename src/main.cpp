#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

void setupPlayer(sf::RectangleShape& player)
{
	player.setSize(sf::Vector2f(50.f, 100.f));
	player.setFillColor(sf::Color(100, 250, 50));
}

void setupPlatform(sf::RectangleShape& platform)
{
	platform.setSize({ 300.f, 30.f });
	platform.setFillColor(sf::Color::White);
	platform.setPosition({ 250.f, 550.f });
}

void playerMovement(sf::RectangleShape& player, float deltaTime, float speed, float jump)
{
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right))
	{
		player.move({ speed * deltaTime, 0.f });
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left))
	{
		player.move({ -speed * deltaTime, 0.f });
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up))
	{
		player.move({ 0.f, -jump * deltaTime });
	}

}

bool detectCollision(sf::RectangleShape& player, sf::RectangleShape& platform)
{
	if (player.getGlobalBounds().findIntersection(platform.getGlobalBounds()))
	{
		return true;
	}

	return false;
}

void playerGravity(sf::RectangleShape& player, float deltaTime, float& velocityY, float gravity, bool collided)
{
	if (collided)
	{
		player.setPosition({ player.getPosition().x, 450.f });
		velocityY = 0.f;
	}
	else if (player.getPosition().y < 500)
	{
		velocityY += gravity * deltaTime;
		player.move({ 0.f, velocityY * deltaTime });
	}
	else
	{
		velocityY = 0.f;
	}
}

int main()
{
	sf::RenderWindow window(sf::VideoMode({ 800, 600 }), "My Window");

	sf::RectangleShape player;
	setupPlayer(player);

	sf::RectangleShape platform;
	setupPlatform(platform);

	sf::Clock clock;
	float speed = 200.f;
	float jump = 400.f;
	float velocityY = 0.f;
	float gravity = 980.f;

	while (window.isOpen())
	{
		float deltaTime = clock.restart().asSeconds();

		while (const std::optional event = window.pollEvent())
		{
			if (event->is<sf::Event::Closed>())
				window.close();
		}

		bool collided = detectCollision(player, platform);
		playerGravity(player, deltaTime, velocityY, gravity, collided);
		playerMovement(player, deltaTime, speed, jump);

		window.clear(sf::Color::Black);
		window.draw(player);
		window.draw(platform);
		window.display();

	}

}
