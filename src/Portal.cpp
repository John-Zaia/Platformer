
#include "Portal.h"

Portal::Portal(float xPosition, float yPosition,
    float xScale, float yScale,
    sf::Texture& portalTexture)
    : portal(portalTexture)
{
    portal.setScale({ xScale, yScale });
    portal.setPosition({ xPosition, yPosition });

    portal.setTextureRect(
        sf::IntRect({ 0, 0 }, { 64, 64 })
    );
}

void Portal::portalIdleAnimation()
{
    if (clock.getElapsedTime().asSeconds() >= 0.1f)
    {
        clock.restart();

        frame++;

        if (frame >= 8)
        {
            frame = 0;
        }

        int column = frame % 8;
        int row = frame / 8;

        portal.setTextureRect(
            sf::IntRect(
                { column * 64, row * 64 },
                { 64, 64 }
            )
        );
    }
}

void Portal::portalCollisionAnimation()
{
    if (clock.getElapsedTime().asSeconds() >= 0.1f)
    {
        clock.restart();

        frame++;

        if (frame >= 16)
        {
            frame = 0;
            playing = false;
        }

        int column = frame % 8;
        int row = frame / 8;

        portal.setTextureRect(
            sf::IntRect(
                { column * 64, row * 64 },
                { 64, 64 }
            )
        );
    }
}

bool Portal::detectPortalCollision(const Player& player)
{
    return player.getBounds()
        .findIntersection(portal.getGlobalBounds()).has_value()
        && !playing;
}

void Portal::update(Player& player, sf::Sound& portalSound)
{
    if (detectPortalCollision(player))
    {
        player.activateGravityPortal();

        portalSound.play();

        playing = true;
        frame = 8;
        clock.restart();

        portal.setTextureRect(
            sf::IntRect({ 0, 64 }, { 64, 64 })
        );
    }

    if (playing)
    {
        portalCollisionAnimation();
    }
    else
    {
        portalIdleAnimation();
    }
}

void Portal::draw(sf::RenderWindow& window)
{
    window.draw(portal);
}
