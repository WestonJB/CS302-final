#include <algorithm>
#include <cstdlib>
#include <string>
#include <vector>
#include "risk.h"

// NOTE: ------- MEANS THAT A LINE IS ASSUMING THAT WE HAVE 6 CONTINENTS
//       +++++++ MEANS THAT A LINE IS ASSUMING THAT WE HAVE 42 TERRITORIES

// Note: does not work for 2 players
Game::Game(const std::vector<std::string> &names) : turn{-1}, terOcc{0} {
    // initialize the players
    players.resize(names.size());
    for (int i = 0; i < names.size(); i++) {
        players[i] = new Player;
        players[i]->setName(names[i]);
        players[i]->armies = 50 - names.size() * 5;
    }
    // initialize the board
    continents.resize(6); // -------
    // do a bunch of work for the map here
    // initialize the drawPile here
}
Game::~Game() {
    for (auto it = player.begin(); it != player.end(); it++) {
        delete (*it);
    }
}

std::vector<int> Game::rollDice(int numDice) const {
    std::vector<int> rolls;
    for (int i = 0; i < numDice; i++) rolls.push_back(std::rand() % 6 + 1);
    return rolls;
}

void Game::setTurn(int newTurn) { turn = newTurn; }

int Game::getTurn() const { return turn; }

void Game::endTurn() { turn = (turn + 1) % players.size(); }

int Game::addArmy(Player *player, Territory *territory) {
    /* Return Key:
     * 0: added a piece to the territory
     * 1: error, must add piece to unoccupied territory
     * 2: error, must add piece to one's own territory */
    Player *player = players[turn];
    if (terOcc < 42) { // +++++++
        if (territory->owner != NULL) return 1;
        territory->owner = player;
        player->territories.push_back(territory);
        territory->armies = 1;
        territory->infantry = 1;
        --player->armies;
        if (findContOwner(territory->continent) == player) {
            territory->continent->owner = player;
            player->continents.push_back(territory->continent);
        }
    } else {
        if (territory->owner != player) return 2;
        ++territory->armies;
        ++territory->infantry;
        --player->armies;
    }
    return 0;
}

void Game::giveArmies(Player *player) {
    Player *player = players[turn];
    int newArmies = player->territories.size() / 3;
    for (auto it = player->continents.begin(); it != player->continents.end();
            it++) {
        newArmies += it->newArmies;
    }
    if (3 < newArmies) player->armies += newArmies;
    else player->armies += 3;
}

void Game::fortify(Territory *start, Territory *end, int armies) {
    /* Return Key:
     * 0: moved armies from start to end
     * 1: error, non-positive number of armies
     * 2: error, do not own starting territory
     * 3: error, do not own ending territory
     * 4: error, can not move that many armies into ending territory */
    Player *player = players[turn];
    if (armies < 1) return 1;
    if (start->owner != player) return 2;
    if (end->owner != player) return 3;
    if (start->armies - armies < 1) return 4;
    // do something here
    return 0;
}

// returns NULL if there is no owner
Player *Game::findContOwner(Continent *continent) const {
    Player *owner = continent->territories.begin()->owner;
    for (auto it = continent->territories.begin() + 1;
            it != continent->territories.end(); it++) {
        if (it->owner != owner) return NULL;
    }
    return owner;
}

// NOTE: needs to implement cards and eliminating players
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
    // if the player now owns the continent, add ownership
    if (findContOwner(continent) == player) {
        territory->continent->owner = player;
        player->continents.push_back(territory->continent);
        // check if they have every continent
        if (player->continents.size() == 6) return 1; // -------
        return 2;
    }
    return 0;
}