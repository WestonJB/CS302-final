#include <cstdlib>
#include <string>
#include <vector>
#include "risk.h"

Player::Player(std::string playerName) : name{playerName} {}
std::string Player::getName() const { return name; }
int Player::captureTerritory(Territory *territory) {
    /* Return Key:
     * 0: normal capture
     * 1: captured continent
     * 2: captured world (game over)
     */
    // remove territory from current owner's list
    // first check if they own the continent
    // then remove their ownership from all objects
    // put new player's ownership on everything
    return 0;
}
Game::Game(std::vector<std::string> names)
        : turn{static_cast<int>(std::rand() % names.size())} {
    // initialize the players
    for (int i = 0; i < names.size(); i++) players.push_back(Player(names[i]));
    // initialize the board
    continents.resize(6);
    // do a bunch of work for the map here
    // initialize the drawPile here
}
void Game::endTurn() { turn = (turn + 1) % players.size(); }