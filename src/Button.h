#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

class Button
{
private:
	sf::RectangleShape shape;
	sf::Text text;

public:
	Button(sf::Vector2f pos, sf::Vector2f sz, sf::Color col, const sf::Font& font, std::string textStr);

	void buttonHoverHighlight(sf::RenderWindow& window);
	bool buttonClick(sf::RenderWindow& window, bool mouseClicked);

	void draw(sf::RenderWindow& window);
};