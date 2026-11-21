#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "Player.h"

class Portal
{
private:
    sf::Sprite portal;

    sf::Clock clock;
    int frame = 0;
    bool playing = false;

    void portalIdleAnimation();
    void portalCollisionAnimation();
    bool detectPortalCollision(const Player& player);

public:
    Portal(float xPosition, float yPosition, float xScale, float yScale, sf::Texture& portalTexture);

    void update(Player& player, sf::Sound& portalSound);
    void draw(sf::RenderWindow& window);
};