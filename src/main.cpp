#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

void setupPlayer(sf::RectangleShape& player)
{
	player.setSize(sf::Vector2f(50.f, 100.f));
	player.setFillColor(sf::Color(100, 250, 50));
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

int main()
{
	sf::RenderWindow window(sf::VideoMode({ 800, 600 }), "My Window");

	sf::RectangleShape player;
	setupPlayer(player);

	sf::Clock clock;
	float speed = 200.f;

	while (window.isOpen())
	{
		float deltaTime = clock.restart().asSeconds();

		while (const std::optional event = window.pollEvent())
		{
			if (event->is<sf::Event::Closed>())
				window.close();
		}
		playerMovement(player, deltaTime, speed);

		window.clear(sf::Color::Black);
		window.draw(player);
		window.display();

	}

}
