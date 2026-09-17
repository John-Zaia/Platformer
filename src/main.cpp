#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

void setupPlayer(sf::RectangleShape& player)
{
	player.setSize(sf::Vector2f(50.f, 100.f));
	player.setFillColor(sf::Color(100, 250, 50));
	player.setPosition({ 350.f, 400.f });
}

void setupPlatform(sf::RectangleShape& platform)
{
	platform.setSize({ 300.f, 30.f });
	platform.setFillColor(sf::Color::White);
	platform.setPosition({ 250.f, 550.f });
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
	float velocityY = 0.f;
	float gravity = 980.f;
	bool isGrounded = true;

	while (window.isOpen())
	{
		float deltaTime = clock.restart().asSeconds();
		bool collided = detectCollision(player, platform);

		if (detectCollision(player, platform) && velocityY >= 0.f)
		{
			player.setPosition({ player.getPosition().x, 450.f });
			velocityY = 0.f;
			isGrounded = true;
		}

		while (const std::optional event = window.pollEvent())
		{
			if (event->is<sf::Event::Closed>())
				window.close();

			if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>())
			{
				if (keyPressed->code == sf::Keyboard::Key::Up && isGrounded)
				{
					playerJump(velocityY);
					isGrounded = false;
				}
			}
		}

		playerGravity(player, deltaTime, velocityY, gravity, collided);
		playerMovement(player, deltaTime, speed);

		window.clear(sf::Color::Black);
		window.draw(player);
		window.draw(platform);
		window.display();

	}

}
