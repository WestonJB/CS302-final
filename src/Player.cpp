#include <string>
#include "risk.h"

void Game::setName(const std::string &newName) { name = newName; }

std::string Player::getName() const { return name; }

void Game::setColor(Color newColor) { color = newColor; }

Color Game::getColor() const { return color; }