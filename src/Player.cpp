
#include "Player.h"

namespace
{
    bool detectPlatformCollision(
        const sf::RectangleShape& player,
        const sf::RectangleShape& platform)
    {
        return player.getGlobalBounds().findIntersection(
            platform.getGlobalBounds()).has_value();
    }

    bool detectObjectCollision(
        const sf::RectangleShape& player,
        const sf::CircleShape& triangle)
    {
        return player.getGlobalBounds().findIntersection(
            triangle.getGlobalBounds()).has_value();
    }
}

Player::Player()
{
    player.setSize({ 64.f, 64.f });
    player.setPosition({ 25.f, 570.f });
}

bool Player::loadTextures()
{
    if (!playerRunTexture.loadFromFile("playerRun.png"))
        return false;

    if (!playerRunUpsideDownTexture.loadFromFile("playerRunUpsideDown.png"))
        return false;

    if (!playerJumpTexture.loadFromFile("playerJump.png"))
        return false;

    if (!playerJumpUpsideDownTexture.loadFromFile("playerJumpUpsideDown.png"))
        return false;

    player.setTexture(&playerRunTexture);

    return true;
}

sf::Vector2f Player::getPosition() const
{
    return player.getPosition();
}

sf::FloatRect Player::getBounds() const
{
    return player.getGlobalBounds();
}

int Player::getTotalDeaths() const
{
    return totalDeaths;
}

bool Player::isGravityReversed() const
{
    return gravityReversed;
}

void Player::movement(float deltaTime)
{
    bool switchMovement = false;

    // Movement for debugging
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space))
    {
        switchMovement = true;
    }

    if (!switchMovement)
    {
        player.move({ speed * deltaTime, 0.f });
    }
    else
    {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right)
            && player.getPosition().x < 750)
        {
            player.move({ speed * deltaTime, 0.f });
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left)
            && player.getPosition().x > 0)
        {
            player.move({ -speed * deltaTime, 0.f });
        }
    }
}

void Player::jump()
{
    static sf::SoundBuffer buffer;
    static bool soundLoaded = buffer.loadFromFile("jump.mp3");
    static sf::Sound jumpSound(buffer);

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up)
        && grounded)
    {
        if (soundLoaded)
            jumpSound.play();

        velocityY = gravityReversed ? 400.f : -400.f;
        grounded = false;
    }
}

void Player::applyGravity(
    float deltaTime,
    const std::vector<sf::RectangleShape>& platforms)
{
    grounded = false;

    for (const auto& platform : platforms)
    {
        if (player.getPosition().y + player.getSize().y - 5
                > platform.getPosition().y
            && player.getPosition().x < platform.getPosition().x
            && detectPlatformCollision(player, platform))
        {
            player.setPosition({
                platform.getPosition().x - player.getSize().x,
                player.getPosition().y
                });
        }
        else if (!gravityReversed
            && detectPlatformCollision(player, platform)
            && velocityY >= 0)
        {
            player.setPosition({
                player.getPosition().x,
                platform.getPosition().y - player.getSize().y
                });

            velocityY = 0.f;
            grounded = true;
            break;
        }
        else if (gravityReversed
            && detectPlatformCollision(player, platform)
            && velocityY <= 0)
        {
            player.setPosition({
                player.getPosition().x,
                platform.getPosition().y + platform.getSize().y
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

void Player::updateAnimation()
{
    if (gravityReversed && !grounded
        && sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up))
    {
        player.setTexture(&playerJumpUpsideDownTexture);
    }
    else if (gravityReversed && grounded)
    {
        player.setTexture(&playerRunUpsideDownTexture);
    }
    else if (!gravityReversed && grounded)
    {
        player.setTexture(&playerRunTexture);
    }
    else if (!gravityReversed && !grounded
        && sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up))
    {
        player.setTexture(&playerJumpTexture);
    }

    if (animationClock.getElapsedTime().asSeconds() >= 0.1f)
    {
        animationClock.restart();

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

bool Player::detectDeath(
    const std::vector<sf::CircleShape>& obstacles)
{
    static sf::SoundBuffer buffer;
    static bool soundLoaded = buffer.loadFromFile("death.mp3");
    static sf::Sound deathSound(buffer);

    for (const auto& obstacle : obstacles)
    {
        if (detectObjectCollision(player, obstacle))
        {
            if (soundLoaded)
                deathSound.play();

            return true;
        }
    }

    if (player.getPosition().y > 1000)
    {
        if (soundLoaded)
            deathSound.play();

        return true;
    }

    return false;
}

void Player::handleDeath()
{
    if (deathClock.getElapsedTime().asSeconds() >= 1.f)
    {
        gravityReversed = false;
        player.setPosition({ 0.f, 520.f });
        velocityY = 0.f;
        grounded = false;
        dead = false;
    }
}

void Player::update(
    float deltaTime,
    const std::vector<sf::RectangleShape>& platforms,
    const std::vector<sf::CircleShape>& obstacles)
{
    if (dead)
    {
        handleDeath();
        return;
    }

    applyGravity(deltaTime, platforms);
    updateAnimation();

    if (detectDeath(obstacles))
    {
        totalDeaths++;
        dead = true;
        deathClock.restart();
        return;
    }

    jump();
    movement(deltaTime);
}

void Player::reset()
{
    player.setPosition({ 25.f, 570.f });

    velocityY = 0.f;
    grounded = false;
    gravityReversed = false;
    dead = false;

    playerFrame = 0;
    animationClock.restart();
    deathClock.restart();

    player.setTexture(&playerRunTexture);
}

void Player::activateGravityPortal()
{
    if (!gravityReversed)
    {
        player.move({ 0.f, -2.f });
    }
    else
    {
        player.move({ 0.f, 2.f });
    }

    gravityReversed = !gravityReversed;
}

void Player::draw(sf::RenderWindow& window)
{
    window.draw(player);
}
