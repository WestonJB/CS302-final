#include "risk.h"
#include <string>
#include <vector>

Player::Player(std::string playerName) : name{playerName} {}
std::string Player::getName() const { return name; }
Game::Game(std::vector<std::string> names) : setup{true} {
    // initialize the turn here
    // initialize the players
    players.push_back(Player("NULL"));
    for (int i = 0; i < names.size(); i++) {
        players.push_back(Player(names[i]));
    }
    // initialize the board
    continents.resize(6);
    // do a bunch of work for the map here
    // initialize the drawPile here
}