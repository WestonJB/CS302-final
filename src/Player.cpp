#include <string>
#include "risk.h"

// NOTE: ------- MEANS THAT A LINE IS ASSUMING THAT WE HAVE 6 CONTINENTS

Player::Player(std::string playerName) : name{playerName} {}

std::string Player::getName() const { return name; }