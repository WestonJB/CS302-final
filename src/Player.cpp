#include <string>
#include "../include/risk.h"

void Player::setName(const std::string &newName) { name = newName; }

std::string Player::getName() const { return name; }

void Player::setColor(Color newColor) { color = newColor; }

Color Player::getColor() const { return color; }