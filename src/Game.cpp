#include <algorithm>
#include <cstdlib>
#include <string>
#include <vector>
#include "../include/risk.h"

#include <typeinfo>

// NOTE: ------- MEANS THAT A LINE IS ASSUMING THAT WE HAVE 6 CONTINENTS
//       +++++++ MEANS THAT A LINE IS ASSUMING THAT WE HAVE 42 TERRITORIES

// Note: does not work for 2 players
Game::Game(const std::vector<std::string> &names) : turn{-1}, terrOcc{0},
        fortOne{NULL}, fortTwo{NULL}, trades{0}, alreadyTraded{false} {
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
    for (auto i : players) {
        for (auto j : i->cards) delete j;
        delete i;
    }
    for (auto i : continents) {
        for (auto j : i->territories) delete j;
        delete i;
    }

}

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
    alreadyTraded = false;
}

int Game::addArmy(Territory *territory) {
    /* Return Key:
     * 0: added a piece to the territory
     * 1: error, must add piece to unoccupied territory (start of game)
     * 2: error, must add piece to one's own territory */
    Player *player = players[turn];
    if (terrOcc < 42) { // +++++++
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
        ++terrOcc;
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
    for (Continent *i : player->continents) newArmies += i->newArmies;
    if (3 < newArmies) player->armies += newArmies;
    else player->armies += 3;
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
    for (int i : pieces) {
        // find the value of the piece and change the armies on territories
        switch (i) {
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
    Player *player = players[turn];
    if (isValidTrade(cardsInd)) return 1;
    // if they own the territory, then 2 extra armies are placed there
    // also must consider wild + 2 different owned territories
    // first, find if they own any territories
    if (!alreadyTraded) {
        for (int i = 0; i < 3; i++) {
            ;
        }
    }
    // give armies for the trade
    switch (trades) {
        case 0:
            player->armies += 4;
            break;
        case 1:
            player->armies += 6;
            break;
        case 2:
            player->armies += 8;
            break;
        case 3:
            player->armies += 10;
            break;
        case 4:
            player->armies += 12;
            break;
        case 5:
            player->armies += 15;
            break;
        default:
            player->armies += 5 * trades - 10;
            break;
    }
    return 0;
}

// returns NULL if there is no owner
Player *Game::findContOwner(const Continent *continent) const {
    Player *owner = continent->territories[0]->owner;
    for (Territory *i : continent->territories) {
        if (i->owner != owner) return NULL;
    }
    return owner;
}

// NOTE: needs to implement cards and eliminating players
int Game::captureTerritory(Territory *territory) {
    /* Return Key:
     * 0: normal capture
     * 1: captured continent
     * 2: captured world (game over) */
    Player *player = players[turn];
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
    if (findContOwner(territory->continent) == player) {
        territory->continent->owner = player;
        player->continents.push_back(territory->continent);
        // check if they have every continent
        if (player->continents.size() == 6) return 2; // -------
        return 1;
    }
    return 0;
}

bool Game::isValidTrade(const std::vector<int> &cardsInd) const {
    // check for a wild card, matching armies, or disjoint armies
    Player *player = players[turn];
    std::string card1 = player->cards[cardsInd[0]]->army;
    std::string card2 = player->cards[cardsInd[1]]->army;
    std::string card3 = player->cards[cardsInd[2]]->army;
    if ((card1 != "wild" && card2 != "wild" && card3 != "wild") &&
            (card1 != card2 || card1 != card3 || card2 != card3) &&
            (card1 == card2 || card1 == card3 || card2 == card3)) {
        return false;
    }
    return true;
}

Territory* Game::findTerritory(const std::string &name) const {
    for (Continent *cont : continents) {
        for (Territory *terr : cont->territories) {
            if (terr->name == name) return terr;
        }
    }
    // should never happen since a card should have a valid territory on it
    return NULL;
}