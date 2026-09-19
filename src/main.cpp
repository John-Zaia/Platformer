#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

void setupPlayer(sf::RectangleShape& player)
{
	player.setSize(sf::Vector2f(50.f, 100.f));
	player.setFillColor(sf::Color(100, 250, 50));
	player.setPosition({ 350.f, 200.f });
}

void setupPlatform(sf::RectangleShape& platform)
{
	platform.setSize({ 800.f, 30.f });
	platform.setFillColor(sf::Color::White);
	platform.setPosition({ 0.f, 570.f });
}

void playerMovement(sf::RectangleShape& player, float deltaTime, float speed)
{
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right))
	{
		player.move({ speed * deltaTime, 0.f });
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left))
	{
		player.move({ -speed * deltaTime, 0.f });
	}


}

void playerJump(float& velocityY)
{
	velocityY = -400.f;
}

bool detectCollision(sf::RectangleShape& player, sf::RectangleShape& platform)
{
	if (player.getGlobalBounds().findIntersection(platform.getGlobalBounds()))
	{
		return true;
	}

	return false;
}

bool playerGravity(sf::RectangleShape& player, sf::RectangleShape& platform, float deltaTime, float& velocityY, float gravity)
{
	if (detectCollision(player, platform) && velocityY > 0)
	{
		player.setPosition({
			player.getPosition().x,
			platform.getPosition().y - player.getSize().y
			});

		velocityY = 0.f;
		return true;
	}
	else
	{
		velocityY += gravity * deltaTime;
		player.move({ 0.f, velocityY * deltaTime });
		return false;
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
	float velocityY = 0.f;
	float gravity = 980.f;

	while (window.isOpen())
	{
		float deltaTime = clock.restart().asSeconds();

		bool grounded = playerGravity(player, platform, deltaTime, velocityY, gravity);
		playerMovement(player, deltaTime, speed);

		while (const std::optional event = window.pollEvent())
		{
			if (event->is<sf::Event::Closed>())
				window.close();

			if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>())
			{
				if (keyPressed->code == sf::Keyboard::Key::Up && grounded)
				{
					playerJump(velocityY);
				}
			}
		}


		window.clear(sf::Color::Black);
		window.draw(player);
		window.draw(platform);
		window.display();

	}

}
