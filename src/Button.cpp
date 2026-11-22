#include "Button.h"

Button::Button(sf::Vector2f pos, sf::Vector2f sz, sf::Color col, const sf::Font& font, std::string textStr) : text(font)
{
	shape.setPosition(pos);
	shape.setSize(sz);
	shape.setFillColor(col);

	text.setString(textStr);
	text.setCharacterSize(24);
	text.setPosition({ pos.x + 10.f, pos.y + 10.f });
}

void Button::buttonHoverHighlight(sf::RenderWindow& window)
{
	sf::Vector2i mousePos = sf::Mouse::getPosition(window);
	if ((mousePos.x >= shape.getPosition().x && mousePos.x <= shape.getPosition().x + shape.getSize().x)
		&& (mousePos.y >= shape.getPosition().y && mousePos.y <= shape.getPosition().y + shape.getSize().y))
	{
		shape.setFillColor(sf::Color::Blue);
	}
	else
	{
		shape.setFillColor(sf::Color::Red);
	}
}

bool Button::buttonClick(sf::RenderWindow & window, bool mouseClicked)
{
	sf::Vector2i mousePos = sf::Mouse::getPosition(window);
	if (mouseClicked
		&& (mousePos.x >= shape.getPosition().x && mousePos.x <= shape.getPosition().x + shape.getSize().x)
		&& (mousePos.y >= shape.getPosition().y && mousePos.y <= shape.getPosition().y + shape.getSize().y))
	{
		return true;
	}

	return false;
}

void Button::draw(sf::RenderWindow& window)
{
	window.draw(shape);
	window.draw(text);
}