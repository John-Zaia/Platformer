#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <string>

void setupPlayer(sf::RectangleShape& player)
{
	player.setSize(sf::Vector2f(64.f, 64.f));
	player.setPosition({ 25.f, 570.f });
	
}

void playerRunningAnimation(sf::Clock& playerRunningAnimationClock, int& playerFrame, sf::RectangleShape& player, sf::Texture& playerRunTexture, sf::Texture&playerRunUpsideDown, bool& gravityReversed)
{

	if (gravityReversed)
	{
		player.setTexture(&playerRunUpsideDown);
	}
	else if(!gravityReversed)
	{
		player.setTexture(&playerRunTexture);
	}

	if (playerRunningAnimationClock.getElapsedTime().asSeconds() >= 0.1f)
	{
		playerRunningAnimationClock.restart();

		playerFrame++;

		if (playerFrame >= 8)
		{
			playerFrame = 0;
		}

		int column = playerFrame % 8;
		int row = playerFrame / 8;

		player.setTextureRect(
			sf::IntRect(
				{ column * 64, row * 64 },
				{ 64, 64 }
			)
		);
	}

}

sf::RectangleShape setupPlatform( float xSize, float ySize, float xPosition, float yPosition, sf::Texture& groundTexture)
{
	sf::RectangleShape platform;
	platform.setSize({ xSize, ySize });
	platform.setFillColor(sf::Color::White);
	platform.setPosition({ xPosition, yPosition });

	platform.setTexture(&groundTexture);
	groundTexture.setRepeated(true);
	platform.setTextureRect(sf::IntRect({ 0, 0 }, { static_cast<int>(xSize), static_cast<int>(ySize) }));

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

sf::Sprite setupPortal(float xPosition, float yPosition, float xScale, float yScale, sf::Texture& portalTexture)
{

	sf::Sprite portal(portalTexture);
	portal.setScale({ xScale, yScale });
	portal.setPosition({ xPosition, yPosition });

	return portal;
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

void playerJump(float& velocityY, bool& grounded, bool& gravityReversed)
{
	static sf::SoundBuffer buffer;
	static bool soundLoaded = buffer.loadFromFile("jump.mp3");
	static sf::Sound jumpSound(buffer);


	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up) && grounded && !gravityReversed)
	{
		if (soundLoaded) jumpSound.play();
		velocityY = -400.f;
		grounded = false;
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up) && grounded && gravityReversed)
	{
		if (soundLoaded) jumpSound.play();
		velocityY = 400.f;
		grounded = false;
	}

}

void resetPlayer(sf::RectangleShape& player, bool& gravityReversed, float& velocityY, sf::View& camera, sf::Clock& clock)
{
	player.setPosition({ 25.f, 570.f });
	gravityReversed = false;
	velocityY = 0.f;
	camera.setCenter(player.getPosition());
	clock.restart();
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

bool detectPortalCollision(sf::RectangleShape& player, sf::Sprite& portal, bool& portalAnimationPlaying)
{
	if (player.getGlobalBounds().findIntersection(portal.getGlobalBounds()) && !portalAnimationPlaying)
	{
		return true;
	}

	return false;
}

bool playerDeath(sf::RectangleShape& player, std::vector<sf::CircleShape>& triangleObstacles)
{
	static sf::SoundBuffer buffer;
	static bool soundLoaded = buffer.loadFromFile("death.mp3");
	static sf::Sound deathSound(buffer);

	for (auto i : triangleObstacles)
	{
		if (detectObjectCollision(player, i))
		{
			if (soundLoaded) deathSound.play();
			return true;
		}
	}

	if (player.getPosition().y > 1000)
	{
		if (soundLoaded) deathSound.play();
		return true;
	}

	return false;
}

sf::Text deathCounter(int deathCounter, sf::Font& font)
{
	sf::Text totalDeaths(font);
	totalDeaths.setString("Deaths: " + std::to_string(deathCounter));
	totalDeaths.setCharacterSize(30);
	totalDeaths.setPosition(sf::Vector2(0.f, 400.f));

	return totalDeaths;
}

void playerGravity(sf::RectangleShape& player, std::vector<sf::RectangleShape>& rectanglePlatforms, float deltaTime, float& velocityY, float gravity, bool& grounded, bool& gravityReversed)
{	
	grounded = false;

	for (int i = 0; i < rectanglePlatforms.size(); i++)
	{

		if (player.getPosition().y + player.getSize().y - 5 > rectanglePlatforms[i].getPosition().y && player.getPosition().x < rectanglePlatforms[i].getPosition().x && detectPlatformCollision(player, rectanglePlatforms[i]))
		{
			player.setPosition({ rectanglePlatforms[i].getPosition().x - player.getSize().x, player.getPosition().y});
		}
		else if (!gravityReversed && detectPlatformCollision(player, rectanglePlatforms[i]) && velocityY >= 0)
		{
			player.setPosition({
				player.getPosition().x,
				rectanglePlatforms[i].getPosition().y - player.getSize().y
				});


			velocityY = 0.f;
			grounded = true;
			break;
		}
		else if (gravityReversed && detectPlatformCollision(player, rectanglePlatforms[i]) && velocityY <= 0 && gravityReversed)
		{
			player.setPosition({
				player.getPosition().x,
				rectanglePlatforms[i].getPosition().y + rectanglePlatforms[i].getSize().y
				});

			velocityY = 0.f;
			grounded = true;
			break;
		}
	}

	if (!grounded)
	{
		if (gravityReversed)
		{
			velocityY -= gravity * deltaTime;
		}
		else
		{
			velocityY += gravity * deltaTime;
		}

		player.move({ 0.f, velocityY * deltaTime });
	}
}


enum class GameState
{
	MainMenu,
	LevelSelection,
	Level1,
	Level2,
	LevelComplete,
	Quit
};

class Button
{
public:
	sf::RectangleShape shape;
	sf::Text text;
	
	Button(sf::Vector2f pos, sf::Vector2f sz, sf::Color col, const sf::Font& font, std::string textStr)
		: text(font)
	{
		shape.setPosition(pos);
		shape.setSize(sz);
		shape.setFillColor(col);

		text.setFont(font);
		text.setString(textStr);
		text.setCharacterSize(24);
		text.setPosition({ pos.x + 10.f, pos.y + 10.f });
	}
};

void buttonHoverHighlight(Button& button, sf::RenderWindow& window)
{
	sf::Vector2i mousePos = sf::Mouse::getPosition(window);
	if ((mousePos.x >= button.shape.getPosition().x && mousePos.x <= button.shape.getPosition().x + button.shape.getSize().x)
		&& (mousePos.y >= button.shape.getPosition().y && mousePos.y <= button.shape.getPosition().y + button.shape.getSize().y))
	{
		button.shape.setFillColor(sf::Color::Blue);
	}
}

bool buttonClick(Button& button, sf::RenderWindow& window, bool mouseClicked)
{
	sf::Vector2i mousePos = sf::Mouse::getPosition(window);
	if (mouseClicked
		&& (mousePos.x >= button.shape.getPosition().x && mousePos.x <= button.shape.getPosition().x + button.shape.getSize().x)
		&& (mousePos.y >= button.shape.getPosition().y && mousePos.y <= button.shape.getPosition().y + button.shape.getSize().y))
	{
		return true;
	}

	return false;
}

GameState mainMenu(sf::RenderWindow& window, sf::Font& font, sf::RectangleShape& player, sf::View& camera, float& velocityY, sf::Clock& clock, bool& mouseClicked, bool& gravityReversed)
{
	static sf::SoundBuffer buffer;
	static bool soundLoaded = buffer.loadFromFile("click.mp3");
	static sf::Sound clickSound(buffer);

	Button playButton({ 350.f, 200.f }, { 100.f, 50.f }, sf::Color::Red, font, "Play");
	buttonHoverHighlight(playButton, window);
	window.draw(playButton.shape);
	window.draw(playButton.text);

	Button quitButton({ 350.f, 275.f }, { 100.f, 50.f }, sf::Color::Red, font, "Quit");
	buttonHoverHighlight(quitButton, window);
	window.draw(quitButton.shape);
	window.draw(quitButton.text);

	if(buttonClick(playButton, window, mouseClicked))
	{
		if (soundLoaded) clickSound.play();
		resetPlayer(player, gravityReversed, velocityY, camera, clock);
		return GameState::LevelSelection;
	}
	else if (buttonClick(quitButton, window, mouseClicked))
	{
		if (soundLoaded)
		{
			clickSound.play();
			while (clickSound.getStatus() == sf::Sound::Status::Playing)
			{
				sf::sleep(sf::milliseconds(1));
			}
		}
		return GameState::Quit;
	}

	return GameState::MainMenu;
}

GameState levelSelection(sf::RenderWindow& window, sf::Font& font, sf::RectangleShape& player, sf::View& camera, float& velocityY, sf::Clock& clock, bool& mouseClicked, bool &gravityReversed)
{
	static sf::SoundBuffer buffer;
	static bool soundLoaded = buffer.loadFromFile("click.mp3");
	static sf::Sound clickSound(buffer);

	Button level1Button({ 350.f, 200.f }, { 100.f, 50.f }, sf::Color::Red, font, "Level 1");
	buttonHoverHighlight(level1Button, window);
	window.draw(level1Button.shape);
	window.draw(level1Button.text);

	Button level2Button({ 350.f, 275.f }, { 100.f, 50.f }, sf::Color::Red, font, "Level 2");
	buttonHoverHighlight(level2Button, window);
	window.draw(level2Button.shape);
	window.draw(level2Button.text);

	if (buttonClick(level1Button, window, mouseClicked))
	{
		if (soundLoaded) clickSound.play();
		resetPlayer(player, gravityReversed, velocityY, camera, clock);
		return GameState::Level1;
	}
	else if(buttonClick(level2Button, window, mouseClicked))
	{
		if (soundLoaded) clickSound.play();
		resetPlayer(player, gravityReversed, velocityY, camera, clock);
		return GameState::Level2;
	}

	return GameState::LevelSelection;
}

GameState winScreen(sf::RenderWindow& window, sf::Text& winText, sf::Font& font, sf::RectangleShape& player, sf::View& camera, float& velcoityY, sf::Clock& clock, bool& mouseClicked, GameState& currentLevel, bool& gravityReversed)
{
	static sf::SoundBuffer buffer;
	static bool soundLoaded = buffer.loadFromFile("click.mp3");
	static sf::Sound clickSound(buffer);

	sf::Text text(font);

	window.clear(sf::Color::Black);
	window.setView(window.getDefaultView());

	winText.setString("You win");
	winText.setCharacterSize(30);
	winText.setPosition(sf::Vector2(350.f, 250.f));

	Button replayButton({ 350.f, 200.f }, { 100.f, 50.f }, sf::Color::Red, font, "Replay");
	buttonHoverHighlight(replayButton, window);
	window.draw(replayButton.shape);
	window.draw(replayButton.text);

	Button mainMenuButton({ 350.f, 275.f }, { 100.f, 50.f }, sf::Color::Red, font, "Main Menu");
	buttonHoverHighlight(mainMenuButton, window);
	window.draw(mainMenuButton.shape);
	window.draw(mainMenuButton.text);

	Button quitButton({ 350.f, 350.f }, { 100.f, 50.f }, sf::Color::Red, font, "Quit");
	buttonHoverHighlight(quitButton, window);
	window.draw(quitButton.shape);
	window.draw(quitButton.text);

	if (buttonClick(replayButton, window, mouseClicked))
	{
		if (soundLoaded) clickSound.play();
		resetPlayer(player, gravityReversed, velcoityY, camera, clock);
		return currentLevel;
	}
	else if (buttonClick(mainMenuButton, window, mouseClicked))
	{
		if (soundLoaded) clickSound.play();
		return GameState::MainMenu;
	}
	else if (buttonClick(quitButton, window, mouseClicked))
	{
		if (soundLoaded)
		{
			clickSound.play();
			while (clickSound.getStatus() == sf::Sound::Status::Playing)
			{
				sf::sleep(sf::milliseconds(1));
			}
		}
		return GameState::Quit;
	}

	return GameState::LevelComplete;
}

void playerState(bool& dead, sf::RectangleShape& player, std::vector<sf::RectangleShape>& platform,
	float& deltaTime, float& velocityY, float& gravity, bool& grounded, std::vector<sf::CircleShape> obstacles,
	int& totalDeath, sf::Text& deaths, sf::Clock& deathClock, float& speed, sf::View& camera, sf::RenderWindow& window,
	bool& gravityReversed, sf::Clock& playerRunningAnimationClock, int& playerFrame, sf::Texture& playerRunTexture, sf::Texture& playerRunUpsideDown)
{
	if (!dead)
	{
		playerRunningAnimation(playerRunningAnimationClock, playerFrame, player, playerRunTexture, playerRunUpsideDown, gravityReversed);
		playerGravity(player, platform, deltaTime, velocityY, gravity, grounded, gravityReversed);

		if (playerDeath(player, obstacles))
		{
			totalDeath++;
			deaths.setString("Deaths: " + std::to_string(totalDeath));
			dead = true;
			deathClock.restart();
		}
		else
		{
			playerJump(velocityY, grounded, gravityReversed);
			playerMovement(player, deltaTime, speed);
		}
	}
	else
	{
		if (deathClock.getElapsedTime().asSeconds() >= 1.f)
		{
			gravityReversed = false;
			player.setPosition({ 0.f, 520.f });
			velocityY = 0.f;
			dead = false;
		}
	}


	camera.setCenter(player.getPosition());
	window.setView(camera);

	window.clear(sf::Color::Black);
	window.draw(player);

	window.draw(deaths);
}

void portalIdleAnimation(sf::Clock& portalAnimationClock, int& portalFrame, sf::Sprite& portal)
{
	if (portalAnimationClock.getElapsedTime().asSeconds() >= 0.1f)
	{
		portalAnimationClock.restart();

		portalFrame++;

		if (portalFrame >= 8)
		{
			portalFrame = 0;
		}

		int column = portalFrame % 8;
		int row = portalFrame / 8;

		portal.setTextureRect(
			sf::IntRect(
				{ column * 64, row * 64 },
				{ 64, 64 }
			)
		);
	}
}

void portalCollisionAnimation(sf::Clock& portalAnimationClock, int& portalFrame, sf::Sprite& portal, bool& portalAnimationPlaying)
{

	if (portalAnimationClock.getElapsedTime().asSeconds() >= 0.1f)
	{
		portalAnimationClock.restart();

		portalFrame++;

		if (portalFrame >= 16)
		{
			portalFrame = 0;
			portalAnimationPlaying = false;
		}

		int column = portalFrame % 8;
		int row = portalFrame / 8;

		portal.setTextureRect(
			sf::IntRect(
				{ column * 64, row * 64 },
				{ 64, 64 }
			)
		);
	}

}

std::vector<sf::RectangleShape> level1Platforms(sf::Texture& groundTexture)
{
	std::vector<sf::RectangleShape> platforms;

	platforms.push_back(setupPlatform(800.f, 30.f, 0.f, 570.f, groundTexture));
	platforms.push_back(setupPlatform(50.f, 30.f, 1000.f, 500.f, groundTexture));
	platforms.push_back(setupPlatform(50.f, 30.f, 1200.f, 430.f, groundTexture));
	platforms.push_back(setupPlatform(50.f, 30.f, 1400.f, 360.f, groundTexture));
	platforms.push_back(setupPlatform(400.f, 30.f, 1600.f, 290.f, groundTexture));

	return platforms;
}

std::vector<sf::CircleShape> level1Triangles(sf::Texture& obstacleTexture)
{
	std::vector<sf::CircleShape> triangleObstacles;

	triangleObstacles.push_back(setupTriangle(20.f, 3, 400.f, 540.f, obstacleTexture));
	triangleObstacles.push_back(setupTriangle(20.f, 3, 430.f, 540.f, obstacleTexture));

	return triangleObstacles;
}

std::vector<sf::RectangleShape> level2Platforms(sf::Texture& groundTexture)
{
	std::vector<sf::RectangleShape> platforms;

	platforms.push_back(setupPlatform(800.f, 30.f, 0.f, 570.f, groundTexture));
	platforms.push_back(setupPlatform(800.f, 30.f, 0.f, 300.f, groundTexture));

	return platforms;
}

std::vector<sf::CircleShape> level2Triangles(sf::Texture& obstacleTexture)
{
	std::vector<sf::CircleShape> triangleObstacles;

	triangleObstacles.push_back(setupTriangle(20.f, 3, 400.f, 540.f, obstacleTexture));

	return triangleObstacles;
}

std::vector<sf::Sprite> level2Portals(sf::Texture& portalTexture)
{
	std::vector<sf::Sprite> portals;

	portals.push_back(setupPortal(200.f, 450.f, 2.f, 2.f, portalTexture));

	return portals;
}

int main()
{
	GameState gameState = GameState::MainMenu;
	GameState currentLevel;
	sf::RenderWindow window(sf::VideoMode({ 800, 600 }), "My Window");
	
	sf::Texture groundTexture;
	if (!groundTexture.loadFromFile("ground.jpg")) return 1;

	sf::Texture obstacleTexture;
	if (!obstacleTexture.loadFromFile("obstacle.jpg")) return 1;

	sf::Texture portalTexture;
	if (!portalTexture.loadFromFile("portal.png")) return 1;
	sf::Sprite portal(portalTexture);
	int portalFrame = 0;
	sf::Clock portalAnimationClock;
	bool portalAnimationPlaying = false;
	sf::SoundBuffer buffer;
	if(!buffer.loadFromFile("portalWoosh.mp3")) return 1;
	sf::Sound portalSound(buffer);

	sf::Texture playerRunTexture;
	if (!playerRunTexture.loadFromFile("playerRun.png")) return 1;
	sf::Texture playerRunUpsideDownTexture;
	if (!playerRunUpsideDownTexture.loadFromFile("playerRunUpsideDown.png")) return 1;
	sf::Clock playerRunningAnimationClock;
	int playerFrame = 0;
	sf::Texture playerJumpTexture;
	if (!playerJumpTexture.loadFromFile("playerJump.png")) return 1;
	sf::RectangleShape player;
	setupPlayer(player);
	player.setTexture(&playerRunTexture);

	sf::View camera(sf::Vector2f(0.f, 0.f), sf::Vector2f(800.f, 600.f));

	std::vector<sf::RectangleShape> level1Platform = level1Platforms(groundTexture);
	std::vector<sf::CircleShape> level1Triangle = level1Triangles(obstacleTexture);

	std::vector<sf::RectangleShape> level2Platform = level2Platforms(groundTexture);
	std::vector<sf::CircleShape> level2Triangle = level2Triangles(obstacleTexture);
	std::vector<sf::Sprite> level2Portal = level2Portals(portalTexture);

	sf::Font font;
	if (!font.openFromFile("text.ttf")) { return 1; }

	sf::Clock clock;
	sf::Clock deathClock;
	float speed = 400.f;
	float velocityY = 0.f;
	float gravity = 980.f;
	bool grounded = false;
	bool dead = false;
	int totalDeath = 0;
	sf::Text deaths = deathCounter(totalDeath, font);
	sf::Text winText(font);
	bool mouseClicked = false;
	bool gravityReversed = false;

	while (window.isOpen())
	{
		mouseClicked = false;

		while (const std::optional event = window.pollEvent())
		{
			if (event->is<sf::Event::Closed>())
				window.close();

			if (const auto* mouseButton = event->getIf<sf::Event::MouseButtonPressed>())
			{
				if (mouseButton->button == sf::Mouse::Button::Left)
				{
					mouseClicked = true;
				}
			}
		}


		if (gameState == GameState::MainMenu)
		{
			window.clear(sf::Color::Black);
			window.setView(window.getDefaultView());
			gameState = mainMenu(window, font, player, camera, velocityY, clock, mouseClicked, gravityReversed);
		}
		else if (gameState == GameState::LevelSelection)
		{
			window.clear(sf::Color::Black);
			window.setView(window.getDefaultView());
			gameState = levelSelection(window, font, player, camera, velocityY, clock, mouseClicked, gravityReversed);
		}
		else if (gameState == GameState::Level1)
		{
			currentLevel = GameState::Level1;
			float deltaTime = clock.restart().asSeconds();

			if (player.getPosition().x >= 2000)
			{
				gameState = GameState::LevelComplete;
			}

			playerState(dead, player, level1Platform, deltaTime, velocityY, gravity, grounded,
				level1Triangle, totalDeath, deaths, deathClock, speed, camera, window, gravityReversed, playerRunningAnimationClock, playerFrame, playerRunTexture, playerRunUpsideDownTexture);

			for (auto& i : level1Platform)
			{
				window.draw(i);
			}

			for (auto& i : level1Triangle)
			{
				window.draw(i);
			}
		}
		else if (gameState == GameState::Level2)
		{
			currentLevel = GameState::Level2;
			float deltaTime = clock.restart().asSeconds();

			if (player.getPosition().x >= 700)
			{
				gameState = GameState::LevelComplete;
			}

			playerState(dead, player, level2Platform, deltaTime, velocityY, gravity, grounded,
				level2Triangle, totalDeath, deaths, deathClock, speed, camera, window, gravityReversed, playerRunningAnimationClock, playerFrame, playerRunTexture, playerRunUpsideDownTexture);

			for (auto& portals : level2Portal)
			{
				if (detectPortalCollision(player, portals, portalAnimationPlaying) && !gravityReversed)
				{
					player.move({ 0.f, -2.f });
					gravityReversed = true;
					portalSound.play();
					portalAnimationPlaying = true;
					portalFrame = 8;
					portalAnimationClock.restart();
				}

				if (portalAnimationPlaying)
				{
					portalCollisionAnimation(portalAnimationClock, portalFrame, portals, portalAnimationPlaying);
				}
				else
				{
					portalIdleAnimation(portalAnimationClock, portalFrame, portals);
				}

				window.draw(portals);
			}

			for (auto& i : level2Platform)
			{
				window.draw(i);
			}

			for (auto& i : level2Triangle)
			{
				window.draw(i);
			}
		}
		else if (gameState == GameState::LevelComplete)
		{
			gameState = winScreen(window, winText, font, player, camera, velocityY, clock, mouseClicked, currentLevel, gravityReversed);
		}
		else if (gameState == GameState::Quit)
		{
			return 0;
		}

		window.display();
	}
}
