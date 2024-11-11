#include <algorithm>
#include <cstdlib>
#include <string>
#include <vector>
#include "risk.h"

// NOTE: ------- MEANS THAT A LINE IS ASSUMING THAT WE HAVE 6 CONTINENTS
//       +++++++ MEANS THAT A LINE IS ASSUMING THAT WE HAVE 42 TERRITORIES

// Note: does not work for 2 players
Game::Game(const std::vector<std::string> &names) : turn{-1}, terOcc{0},
        fortOne{NULL}, fortTwo{NULL}, infantry{0}. calvary{0}, artillery{0},
        trades{0} {
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
Game::~Game() { for (auto i : players) delete i; }

std::vector<int> Game::rollDice(int numDice) const {
    std::vector<int> rolls;
    for (int i = 0; i < numDice; i++) rolls.push_back(std::rand() % 6 + 1);
    return rolls;
}

void Game::setTurn(int newTurn) { turn = newTurn; }

int Game::getTurn() const { return turn; }

void Game::endTurn() {
    turn = (turn + 1) % players.size();
    fortOne = NULL;
    fortTwo = NULL;
    // should already be 0, but if some error stops that, this resets it
    infantry = 0;
	calvary = 0;
	artillery = 0;
}

int Game::addArmy(Territory *territory) {
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

void Game::giveArmies() {
    Player *player = players[turn];
    int newArmies = player->territories.size() / 3;
    for (auto i : player->continents) newArmies += i->newArmies;
    if (3 < newArmies) infantry += newArmies;
    else infantry += 3;
}

int Game::setFortify(Territory *start, Territory *end) {
    /* Return Key:
     * 0: set start to end territories for fortify
     * 1: error, do not own starting territory
     * 2: error, do not own ending territory */
    Player *player = players[turn];
    if (start->owner != player) return 1;
    if (end->owner != player) return 2;
    fortOne = start;
    fortTwo = end;
    return 0;
}

void Game::fortify(const std::vector<char> &pieces) {
    int value;
    for (int i = 0; i < pieces.size(); pieces++) {
        // find the value of the piece and change the armies on territories
        switch (pieces) {
            case 'i':
                value = 1;
                fortOne->infantry -= 1;
                fortTwo->infantry += 1;
                break;
            case 'c':
                value = 5;
                fortOne->calvary -= 1;
                fortTwo->calvary += 1;
                break;
            case 'a':
                value = 10;
                fortOne->artillery -= 1;
                fortTwo->artillery += 1;
                break;
            // this should never run
            default:
                value = 1;
                break;
        }
        fortOne->armies -= value;
        fortTwo->armies += value;
    }
}

void Game::giveCard() {
    Player *player = players[turn];
    player->cards.push_back(drawPile[drawPile.size() - 1]);
    drawPile.pop_back();
}

int Game::tradeCards(const std::vector<int> &cardsInd) {
    /* Return Key:
     * 0: traded in cards
     * 1: cards do not form a set that can be traded */
    // check for a wild card
    Player *player = players[turn];
    if (player->cards[cardsInd[0]] == "wild" || player->cards[cardsInd[1]]
            == "wild" || player->cards[cardsInd[2]] == "wild")
}

// returns NULL if there is no owner
Player *Game::findContOwner(Continent *continent) const {
    Player *owner = continent->territories[0].owner;
    for (auto i : continent->territories) {
        if (i.owner != owner) return NULL;
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
        if (player->continents.size() == 6) return 2; // -------
        return 1;
    }
    return 0;
}