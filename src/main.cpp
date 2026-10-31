#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <string>
#include "Player.h"

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

sf::CircleShape setupTriangle(float radius, int points, float xPosition, float yPosition, float rotation, sf::Texture& triangleTexture)
{
	sf::CircleShape triangle;
	triangle.setRadius(radius);
	triangle.setPointCount(points);
	triangle.setPosition({ xPosition, yPosition });
	triangle.setRotation(sf::degrees(rotation));

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



sf::Text deathCounter(int deathCounter, sf::Font& font)
{
	sf::Text totalDeaths(font);
	totalDeaths.setString("Deaths: " + std::to_string(deathCounter));
	totalDeaths.setCharacterSize(30);
	totalDeaths.setPosition(sf::Vector2(0.f, 400.f));

	return totalDeaths;
}


enum class GameState
{
	MainMenu,
	PauseMenu,
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

void updateCamera(Player& player, sf::View& camera, sf::RenderWindow& window)
{
	camera.setCenter(player.getPosition());
	window.setView(camera);
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

struct PortalAnimation
{
	sf::Clock clock;
	int frame = 0;
	bool playing = false;
};


bool detectPortalCollision(
	Player& player,
	sf::Sprite& portal,
	bool& portalAnimationPlaying)
{
	if (player.getBounds().findIntersection(portal.getGlobalBounds())
		&& !portalAnimationPlaying)
	{
		return true;
	}

	return false;
}

void updatePortals(
	Player& player,
	std::vector<sf::Sprite>& portals,
	std::vector<PortalAnimation>& portalAnimations,
	sf::Sound& portalSound)
{
	for (std::size_t i = 0; i < portals.size(); i++)
	{
		auto& portal = portals[i];
		auto& animation = portalAnimations[i];

		if (detectPortalCollision(player, portal, animation.playing))
		{
			player.activateGravityPortal();

			portalSound.play();

			animation.playing = true;
			animation.frame = 8;
			animation.clock.restart();
		}

		if (animation.playing)
		{
			portalCollisionAnimation(animation.clock, animation.frame, portal, animation.playing);
		}
		else
		{
			portalIdleAnimation(animation.clock, animation.frame, portal);
		}

	}
}

void drawGame(sf::RenderWindow& window,
	std::vector<sf::Sprite>& portals,
	std::vector<sf::RectangleShape>& platforms,
	std::vector<sf::CircleShape>& triangles,
	sf::Text& deaths)
{

	for (auto& portal : portals)
		window.draw(portal);

	for (auto& platform : platforms)
		window.draw(platform);

	for (auto& triangle : triangles)
		window.draw(triangle);

	window.draw(deaths);
}

GameState mainMenu(sf::RenderWindow& window, sf::Font& font, Player& player, sf::View& camera, sf::Clock& clock, bool& mouseClicked)
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
		player.reset();
		camera.setCenter(player.getPosition());
		clock.restart();
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

GameState pauseMenu(sf::RenderWindow& window, sf::Font& font, Player& player, sf::View& camera, sf::Clock& clock, bool& mouseClicked)
{
	static sf::SoundBuffer buffer;
	static bool soundLoaded = buffer.loadFromFile("click.mp3");
	static sf::Sound clickSound(buffer);


	Button mainMenuButton({ 350.f, 275.f }, { 100.f, 50.f }, sf::Color::Red, font, "Main Menu");
	buttonHoverHighlight(mainMenuButton, window);
	window.draw(mainMenuButton.shape);
	window.draw(mainMenuButton.text);

	if (buttonClick(mainMenuButton, window, mouseClicked))
	{
		if (soundLoaded) clickSound.play();
		player.reset();
		camera.setCenter(player.getPosition());
		clock.restart();

		return GameState::MainMenu;
	}

	return GameState::PauseMenu;
}

GameState levelSelection(sf::RenderWindow& window, sf::Font& font, Player& player, sf::View& camera, sf::Clock& clock, bool& mouseClicked)
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
		player.reset();
		camera.setCenter(player.getPosition());
		clock.restart();
		return GameState::Level1;
	}
	else if(buttonClick(level2Button, window, mouseClicked))
	{
		if (soundLoaded) clickSound.play();
		player.reset();
		camera.setCenter(player.getPosition());
		clock.restart();
		return GameState::Level2;
	}

	return GameState::LevelSelection;
}

GameState winScreen(sf::RenderWindow& window, sf::Text& winText, sf::Font& font, Player& player, sf::View& camera, sf::Clock& clock, bool& mouseClicked, GameState& currentLevel)
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
		player.reset();
		camera.setCenter(player.getPosition());
		clock.restart();
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

std::vector<sf::RectangleShape> level1Platforms(sf::Texture& groundTexture)
{
	std::vector<sf::RectangleShape> platforms;

	// Starting platform
	platforms.push_back(setupPlatform(800.f, 30.f, 0.f, 570.f, groundTexture));

	// Staircase 
	platforms.push_back(setupPlatform(50.f, 30.f, 950.f, 500.f, groundTexture));
	platforms.push_back(setupPlatform(50.f, 30.f, 1150.f, 430.f, groundTexture));
	platforms.push_back(setupPlatform(50.f, 30.f, 1350.f, 360.f, groundTexture));

	// Landing platform before the first portal
	platforms.push_back(setupPlatform(200.f, 30.f, 1550.f, 290.f, groundTexture));

	// Ceiling for the gravity-reversed section
	platforms.push_back(setupPlatform(1050.f, 30.f, 1550.f, 0.f, groundTexture));

	// Floor beneath the gravity-reversed section
	platforms.push_back(setupPlatform(800.f, 30.f, 1800.f, 570.f, groundTexture));

	// Final stretch
	platforms.push_back(setupPlatform(1400.f, 30.f, 2600.f, 570.f, groundTexture));

	return platforms;
}

std::vector<sf::CircleShape> level1Triangles(sf::Texture& obstacleTexture)
{
	std::vector<sf::CircleShape> triangleObstacles;

	// Introductory obstacles
	triangleObstacles.push_back(setupTriangle(20.f, 3, 400.f, 540.f, 0.f, obstacleTexture));
	triangleObstacles.push_back(setupTriangle(20.f, 3, 430.f, 540.f, 0.f, obstacleTexture));

	// Ceiling obstacles
	triangleObstacles.push_back(setupTriangle(20.f, 3, 2100.f, 60.f, 180.f, obstacleTexture));
	triangleObstacles.push_back(setupTriangle(20.f, 3, 2150.f, 60.f, 180.f, obstacleTexture));
	triangleObstacles.push_back(setupTriangle(20.f, 3, 2200.f, 60.f, 180.f, obstacleTexture));

	// Final obstacles
	triangleObstacles.push_back(setupTriangle(20.f, 3, 3100.f, 540.f, 0.f, obstacleTexture));
	triangleObstacles.push_back(setupTriangle(20.f, 3, 3130.f, 540.f, 0.f, obstacleTexture));
	triangleObstacles.push_back(setupTriangle(20.f, 3, 3160.f, 540.f, 0.f, obstacleTexture));

	triangleObstacles.push_back(setupTriangle(20.f, 3, 3550.f, 540.f, 0.f, obstacleTexture));
	triangleObstacles.push_back(setupTriangle(20.f, 3, 3580.f, 540.f, 0.f, obstacleTexture));
	triangleObstacles.push_back(setupTriangle(20.f, 3, 3610.f, 540.f, 0.f, obstacleTexture));

	return triangleObstacles;
}

std::vector<sf::Sprite> level1Portals(sf::Texture& portalTexture)
{
	std::vector<sf::Sprite> portals;

	// Reverse gravity
	portals.push_back(setupPortal(1650.f, 175.f, 2.f, 2.f, portalTexture));

	// Restore normal gravity
	portals.push_back(setupPortal(2450.f, 30.f, 2.f, 2.f, portalTexture));

	return portals;
}

std::vector<sf::RectangleShape> level2Platforms(sf::Texture& groundTexture)
{
	std::vector<sf::RectangleShape> platforms;

	// Ceiling for the whole level 
	platforms.push_back(setupPlatform(4800.f, 30.f, 0.f, 300.f, groundTexture));

	// Zone 1 floor
	platforms.push_back(setupPlatform(1650.f, 30.f, 0.f, 570.f, groundTexture));

	// Zone 2 floor, starts after the 200px pit and holds the first portal
	platforms.push_back(setupPlatform(550.f, 30.f, 1850.f, 570.f, groundTexture));

	// Zone 3 floor, starts well before the second portal's landing spot
	platforms.push_back(setupPlatform(1500.f, 30.f, 3300.f, 570.f, groundTexture));


	return platforms;
}

std::vector<sf::CircleShape> level2Triangles(sf::Texture& obstacleTexture)
{
	std::vector<sf::CircleShape> triangleObstacles;

	// Zone 1: floor spikes, growing from 1 to 3
	triangleObstacles.push_back(setupTriangle(20.f, 3, 400.f, 540.f, 0.f, obstacleTexture));

	triangleObstacles.push_back(setupTriangle(20.f, 3, 800.f, 540.f, 0.f, obstacleTexture));
	triangleObstacles.push_back(setupTriangle(20.f, 3, 830.f, 540.f, 0.f, obstacleTexture));

	triangleObstacles.push_back(setupTriangle(20.f, 3, 1200.f, 540.f, 0.f, obstacleTexture));
	triangleObstacles.push_back(setupTriangle(20.f, 3, 1230.f, 540.f, 0.f, obstacleTexture));
	triangleObstacles.push_back(setupTriangle(20.f, 3, 1260.f, 540.f, 0.f, obstacleTexture));

	// Zone 2: ceiling spikes 
	triangleObstacles.push_back(setupTriangle(20.f, 3, 2700.f, 360.f, 180.f, obstacleTexture));
	triangleObstacles.push_back(setupTriangle(20.f, 3, 2730.f, 360.f, 180.f, obstacleTexture));
	triangleObstacles.push_back(setupTriangle(20.f, 3, 2760.f, 360.f, 180.f, obstacleTexture));

	triangleObstacles.push_back(setupTriangle(20.f, 3, 3100.f, 360.f, 180.f, obstacleTexture));
	triangleObstacles.push_back(setupTriangle(20.f, 3, 3130.f, 360.f, 180.f, obstacleTexture));

	// Zone 3: floor spikes after gravity is restored
	triangleObstacles.push_back(setupTriangle(20.f, 3, 3950.f, 540.f, 0.f, obstacleTexture));
	triangleObstacles.push_back(setupTriangle(20.f, 3, 3980.f, 540.f, 0.f, obstacleTexture));
	triangleObstacles.push_back(setupTriangle(20.f, 3, 4010.f, 540.f, 0.f, obstacleTexture));

	triangleObstacles.push_back(setupTriangle(20.f, 3, 4400.f, 540.f, 0.f, obstacleTexture));


	return triangleObstacles;
}

std::vector<sf::Sprite> level2Portals(sf::Texture& portalTexture)
{
	std::vector<sf::Sprite> portals;

	// Reverse gravity
	portals.push_back(setupPortal(2250.f, 442.f, 2.f, 2.f, portalTexture));

	// Restore gravity
	portals.push_back(setupPortal(3500.f, 330.f, 2.f, 2.f, portalTexture));


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

	sf::SoundBuffer buffer;
	if(!buffer.loadFromFile("portalWoosh.mp3")) return 1;
	sf::Sound portalSound(buffer);

	Player player;
	if (!player.loadTextures())
		return 1;

	sf::View camera(sf::Vector2f(0.f, 0.f), sf::Vector2f(800.f, 600.f));

	std::vector<PortalAnimation> portalAnimations;
	std::vector<sf::RectangleShape> level1Platform = level1Platforms(groundTexture);
	std::vector<sf::CircleShape> level1Triangle = level1Triangles(obstacleTexture);
	std::vector<sf::Sprite> level1Portal = level1Portals(portalTexture);
	portalAnimations.resize(level1Portal.size());

	std::vector<sf::RectangleShape> level2Platform = level2Platforms(groundTexture);
	std::vector<sf::CircleShape> level2Triangle = level2Triangles(obstacleTexture);
	std::vector<sf::Sprite> level2Portal = level2Portals(portalTexture);
	portalAnimations.resize(level2Portal.size());

	sf::Font font;
	if (!font.openFromFile("text.ttf")) { return 1; }

	sf::Clock clock;

	sf::Text deaths = deathCounter(player.getTotalDeaths(), font);

	sf::Text winText(font);

	bool mouseClicked = false;


	while (window.isOpen())
	{
		mouseClicked = false;

		while (const std::optional event = window.pollEvent())
		{
			if (event->is<sf::Event::Closed>())
				window.close();

			if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>())
			{
				if (keyPressed->code == sf::Keyboard::Key::Escape)
				{
					gameState = GameState::PauseMenu;
				}
			}

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
			gameState = mainMenu(window, font, player, camera, clock, mouseClicked);
		}
		else if (gameState == GameState::PauseMenu)
		{
			window.clear(sf::Color::Black);
			window.setView(window.getDefaultView());
			gameState = pauseMenu(window, font, player, camera, clock, mouseClicked);
		}
		else if (gameState == GameState::LevelSelection)
		{
			window.clear(sf::Color::Black);
			window.setView(window.getDefaultView());
			gameState = levelSelection(window, font, player, camera, clock, mouseClicked);
		}
		else if (gameState == GameState::Level1)
		{
			currentLevel = GameState::Level1;

			float deltaTime = clock.restart().asSeconds();

			if (player.getPosition().x >= 4000)
			{
				gameState = GameState::LevelComplete;
			}

			player.update(deltaTime, level1Platform, level1Triangle);

			updatePortals(player, level1Portal, portalAnimations, portalSound);

			updateCamera(player, camera, window);

			deaths.setString(
				"Deaths: " + std::to_string(player.getTotalDeaths())
			);

			window.clear();

			drawGame(window, level1Portal, level1Platform, level1Triangle, deaths);

			player.draw(window);

		}
		else if (gameState == GameState::Level2)
		{
			currentLevel = GameState::Level2;
			float deltaTime = clock.restart().asSeconds();

			if (player.getPosition().x >= 4800)
			{
				gameState = GameState::LevelComplete;
			}

			player.update(deltaTime, level2Platform, level2Triangle);

			updatePortals(player, level2Portal, portalAnimations, portalSound);

			updateCamera(player, camera, window);

			deaths.setString(
				"Deaths: " + std::to_string(player.getTotalDeaths())
			);

			window.clear();

			drawGame(window, level2Portal, level2Platform, level2Triangle, deaths);

			player.draw(window);
		}
		else if (gameState == GameState::LevelComplete)
		{
			gameState = winScreen(window, winText, font, player, camera, clock, mouseClicked, currentLevel);
		}
		else if (gameState == GameState::Quit)
		{
			return 0;
		}

		window.display();
	}
}
