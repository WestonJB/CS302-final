#include <algorithm>
#include <cstdlib>
#include <string>
#include <vector>
#include "risk.h"

// NOTE: ------- MEANS THAT A LINE IS ASSUMING THAT WE HAVE 6 CONTINENTS

Game::Game(std::vector<std::string> names) : turn{-1} {
    // initialize the players
    for (int i = 0; i < names.size(); i++) players.push_back(Player(names[i]));
    // initialize the board
    continents.resize(6); // -------
    // do a bunch of work for the map here
    // initialize the drawPile here
}

std::vector<int> Game::rollDice(int numDice) const {
    std::vector<int> rolls;
    for (int i = 0; i < numDice; i++) rolls.push_back(std::rand() % 6 + 1);
    return rolls;
}

int Game::getTurn() const { return turn; }

void Game::endTurn() { turn = (turn + 1) % players.size(); }

int Game::captureTerritory(Player *player, Territory *territory) {
    /* Return Key:
     * 0: normal capture
     * 1: captured continent
     * 2: captured world (game over) */
    Player *prevOwner = territory->owner;
    // give the new player the territory
    territory->owner = player;
    player->territories.push_back(territory);
    // erase the old player's ownership entirely
    if (prevOwner != NULL) {
        prevOwner->territories.erase(std::find(prevOwner->territories.begin(),
            prevOwner->territories.end(), territory));
        // if the old player owns the continent, remove their ownership there
        if (territory->continent->owner == prevOwner) {
            territory->continent->owner = NULL;
            prevOwner->continents.erase
                (std::find(prevOwner->continents.begin(),
                prevOwner->continents.end(), territory->continent));
        }
    }
    // check if the player now owns the continent
    bool contOwner = true;
    for (auto it = territory->continent->territories.begin();
            it != territory->continent->territories.end(); it++) {
        if (it->owner != player) {
            contOwner = false;
            break;
        }
    }
    // if they do, add ownership
    if (contOwner) {
        territory->continent->owner = player;
        player->continents.push_back(territory->continent);
        // check if they have every continent
        if (player->continents.size() == 6) return 1; // -------
        return 2;
    }
    return 0;
}