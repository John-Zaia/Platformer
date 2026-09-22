#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

void setupPlayer(sf::RectangleShape& player)
{
	player.setSize(sf::Vector2f(50.f, 100.f));
	player.setFillColor(sf::Color(100, 250, 50));
	player.setPosition({ 350.f, 200.f });
}

void setupPlatform(sf::RectangleShape& platform, float xSize, float ySize, float xPosition, float yPosition)
{
	platform.setSize({ xSize, ySize });
	platform.setFillColor(sf::Color::White);
	platform.setPosition({ xPosition, yPosition });
}

void playerMovement(sf::RectangleShape& player, float deltaTime, float speed)
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

void playerJump(float& velocityY, bool& grounded)
{
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up) && grounded)
	{
		velocityY = -400.f;
		grounded = false;
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

void playerGravity(sf::RectangleShape& player, std::vector<sf::RectangleShape>& rectanglePlatforms, float deltaTime, float& velocityY, float gravity, bool& grounded)
{	
	grounded = false;

	for (int i = 0; i < rectanglePlatforms.size(); i++)
	{
		if (detectCollision(player, rectanglePlatforms[i]) && velocityY >= 0)
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

	std::vector<sf::RectangleShape> rectangePlatforms;

	sf::RectangleShape player;
	setupPlayer(player);

	sf::RectangleShape groundPlatform;
	setupPlatform(groundPlatform, 800.f, 30.f, 0.f, 570.f);
	rectangePlatforms.emplace_back(groundPlatform);

	sf::RectangleShape midPlatform;
	setupPlatform(midPlatform, 100.f, 30.f, 250.f, 500.f);
	rectangePlatforms.emplace_back(midPlatform);

	sf::Texture groundTexture;
	if (!groundTexture.loadFromFile("ground.jpg"))
	{
		return -1;
	}

	groundPlatform.setTexture(&groundTexture);
	groundTexture.setRepeated(true);
	groundPlatform.setTextureRect(sf::IntRect({ 0, 0 }, { static_cast<int>(800), static_cast<int>(30) }));

	sf::Clock clock;
	float speed = 200.f;
	float velocityY = 0.f;
	float gravity = 980.f;
	bool grounded = false;

	while (window.isOpen())
	{
		float deltaTime = clock.restart().asSeconds();

		playerGravity(player, rectangePlatforms, deltaTime, velocityY, gravity, grounded);

		while (const std::optional event = window.pollEvent())
		{
			if (event->is<sf::Event::Closed>())
				window.close();
		}

		playerJump(velocityY, grounded);
		playerMovement(player, deltaTime, speed);

		window.clear(sf::Color::Black);
		window.draw(player);
		window.draw(groundPlatform);
		window.draw(midPlatform);
		window.display();

	}
}
