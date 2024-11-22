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
        terrOne{NULL}, terrTwo{NULL}, trades{0}, gotTradeBonus{false},
        captured{true} {
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
    for (auto i : drawPile) delete i;
    for (auto i : discardPile) delete i;
}

std::vector<int> Game::rollDice(int numDice) const {
    std::vector<int> rolls(numDice);
    for (int i = 0; i < numDice; i++) rolls[i] = std::rand() % 6 + 1;
    return rolls;
}

void Game::setTurn(int newTurn) { turn = newTurn; }

int Game::getTurn() const { return turn; }

void Game::endTurn() {
    terrOne = NULL;
    terrTwo = NULL;
    gotTradeBonus = false;
    if (captured) {
        giveCard();
        captured = false;
    }
    turn = (turn + 1) % players.size();
}

bool Game::setupFinished() const {
    for (Player *player : players) {
        if (player->armies > 0) return false;
    }
    return true;
}

// note: this is used for the setup of the game and the mid-game
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
        --player->armies; // unnecessary after the setup of the game
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

int Game::tradeArmies(Territory *territory, char startType, char endType) {
    /* Return Key:
     * 0: traded armies
     * 1: error, do not own territory
     * 2: error, trading pieces for the same pieces (startType == endType)
     * 3: error, do not own enough pieces to trade */
    Player *player = players[turn];
    if (territory->owner != player) return 1;
    if (startType == endType) return 2;
    switch (startType) {
        case 'i':
            switch (endType) {
                case 'c':
                    if (territory->infantry < 5) return 3;
                    territory->infantry -= 5;
                    ++territory->calvary;
                    break;
                case 'a':
                    if (territory->infantry < 10) return 3;
                    territory->infantry -= 10;
                    ++territory->artillery;
                    break;
            }
            break;
        case 'c':
            switch (endType) {
                case 'i':
                    --territory->calvary;
                    territory->infantry += 5;
                    break;
                case 'a':
                    if (territory->calvary < 2) return 3;
                    territory->calvary -= 2;
                    ++territory->artillery;
                    break;
            }
            break;
        case 'a':
            switch (endType) {
                --territory->artillery;
                case 'i':
                    territory->infantry += 10;
                    break;
                case 'c':
                    territory->calvary += 2;
                    break;
            }
            break;
    }
    return 0;
}

int Game::setAttack(Territory *start, Territory *end) {
    /* Return Key:
     * 0: set start and end territories for attacking
     * 1: error, do not own starting territory
     * 2: error, player owns ending territory
     * 3: error, territories are not connected
     * 4: error, do not have enough armies on starting territory */
    int returnVal = canAttack(start, end);
    if (returnVal > 0) return returnVal;
    terrOne = start;
    terrTwo = end;
    return 0;
}

int Game::attack(int playerOneDice, int playerTwoDice) {
    /* Return Key:
     * 0: battle happened
     * 1: capture
     * 2: captured continent
     * 3: captured world (game over) 
     * 11: capture and eliminated a player
     * 12: captured continent and eliminated a player
     * 4: error, player one must roll 1-3 dice
     * 5: error, player one must have one more infantry than dice
     * 6: error, player two must roll 1-2 dice
     * 7: error, player two must have one infantry per die */
    // some error checking
    if (playerOneDice < 1 || playerOneDice > 3) return 4;
    if (terrOne->infantry < playerOneDice + 1) return 5;
    if (playerOneDice < 1 || playerTwoDice > 2) return 6;
    if (terrTwo->infantry < playerTwoDice) return 7;
    // attack
    std::vector<int> playerOneRoll = rollDice(playerOneDice);
    std::vector<int> playerTwoRoll = rollDice(playerTwoDice);
    // sort the rolls
    selectionSort(playerOneRoll);
    selectionSort(playerTwoRoll);
    // compare the two highest rolls
    if (playerOneRoll[playerOneDice - 1] > playerTwoRoll[playerTwoDice - 1]) {
        // player one wins the battle
        --terrTwo->infantry;
        --terrTwo->armies;
    } else {
        // player two wins the battle
        --terrOne->infantry;
        --terrOne->armies;
    }
    // see if there is another battle to compare and compare the second highest
    if (playerOneDice > 1 && playerTwoDice > 1) {
        if (playerOneRoll[playerOneDice - 2]
                > playerTwoRoll[playerTwoDice - 2]) {
            // player one wins the battle
            --terrTwo->infantry;
            --terrTwo->armies;
        } else {
            // player two wins the battle
            --terrOne->infantry;
            --terrOne->armies;
        }
    }
    // if the opposing player has no more armies, then capture his territory
    if (terrTwo->armies == 0) {
        int captureVal = captureTerritory(terrTwo);
        if (captureVal == 2) return 3;
        // if the game is not over, then the player needs to move a piece there
        return captureVal + 1;
    }
    return 0;
}

void Game::movePiece(char army) {
    switch (army) {
        case 'i':
            --terrOne->armies;
            --terrOne->infantry;
            ++terrTwo->armies;
            ++terrTwo->infantry;
            break;
        case 'c':
            --terrOne->calvary;
            --terrOne->infantry;
            ++terrTwo->calvary;
            ++terrTwo->infantry;
            break;
        case 'a':
            --terrOne->artillery;
            --terrOne->infantry;
            ++terrTwo->artillery;
            ++terrTwo->infantry;
            break;
    }
}

int Game::setFortify(Territory *start, Territory *end) {
    /* Return Key:
     * 0: set start and end territories for fortify
     * 1: error, do not own starting territory
     * 2: error, do not own ending territory
     * 3: error, territories are not connected */
    Player *player = players[turn];
    if (start->owner != player) return 1;
    if (end->owner != player) return 2;
    if (!areTerritoriesConnected(start, end)) return 3;
    terrOne = start;
    terrTwo = end;
    return 0;
}

void Game::fortify(const std::vector<char> &pieces) {
    int value;
    for (int i : pieces) {
        // find the value of the piece and change the armies on territories
        switch (i) {
            case 'i':
                value = 1;
                terrOne->infantry -= 1;
                terrTwo->infantry += 1;
                break;
            case 'c':
                value = 5;
                terrOne->calvary -= 1;
                terrTwo->calvary += 1;
                break;
            case 'a':
                value = 10;
                terrOne->artillery -= 1;
                terrTwo->artillery += 1;
                break;
            // this should never run
            default:
                value = 1;
                break;
        }
        terrOne->armies -= value;
        terrTwo->armies += value;
    }
}

void Game::giveCard() {
    // give the player a card and remove it from the drawPile
    players[turn]->cards.push_back(drawPile[drawPile.size() - 1]);
    drawPile.pop_back();
    // if the deck is empty, make the discardPile become the drawPile
    if (drawPile.size() == 0) {
        drawPile = discardPile;
        discardPile.clear();
        // shuffle
        int temp;
        Card *temp2;
        for (int i = 0; i < drawPile.size() - 1; i++) {
            temp = std::rand() % (drawPile.size() - i) + i;
            temp2 = drawPile[i];
            drawPile[i] = drawPile[temp];
            drawPile[temp] = temp2;
        }
    }
}

int Game::tradeCards(const std::vector<int> &cardsInd) {
    /* Return Key:
     * 0: traded in cards
     * 1: error, cards do not form a set that can be traded */
    Player *player = players[turn];
    if (!isValidTrade(cardsInd)) return 1;
    // if they own the territory, then 2 extra armies are placed there
    // however, this happens once per turn
    if (!gotTradeBonus) {
        for (int i = 0; i < 3; i++) {
            Territory *cardTerr = findTerritory(player->cards[cardsInd[i]]
                ->territory);
            if (cardTerr == NULL) continue; // wild card; has no territory
            if (cardTerr->owner == player) {
                gotTradeBonus = true;
                cardTerr->armies += 2;
                cardTerr->infantry += 2;
                break;
            }
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
    // put the cards back in discardPile and remove them from the player's hand
    discardPile.push_back(player->cards[cardsInd[0]]);
    discardPile.push_back(player->cards[cardsInd[1]]);
    discardPile.push_back(player->cards[cardsInd[2]]);
    player->cards.erase(std::find(player->cards.begin(), player->cards.end(),
        player->cards[cardsInd[0]]));
    player->cards.erase(std::find(player->cards.begin(), player->cards.end(),
        player->cards[cardsInd[1]]));
    player->cards.erase(std::find(player->cards.begin(), player->cards.end(),
        player->cards[cardsInd[2]]));
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

bool Game::areTerritoriesConnected(const Territory *start, const Territory *end) const {
    for (Territory *terr : start->nearTerritories) {
        if (terr == end) return true;
    }
    return false;
}

int Game::canAttack(const Territory *start, const Territory *end) const {
    /* Return Key:
     * 0: valid for start to attack end
     * 1: error, do not own starting territory
     * 2: error, player owns ending territory
     * 3: error, territories are not connected
     * 4: error, do not have enough armies on starting territory */
    Player *player = players[turn];
    if (start->owner != player) return 1;
    if (end->owner == player) return 2;
    if (!areTerritoriesConnected(start, end)) return 3;
    if (start->armies < 2) return 4;
    return 0;
}

void Game::selectionSort(std::vector<int> &list) const {
    int max, temp;
    for (int i = 0; i < list.size() - 1; i++) {
        max = i;
        for (int j = i + 1; j < list.size(); j++) {
            if (list[j] < list[max]) max = j;
        }
        if (max != i) {
            temp = list[max];
            list[max] = list[i];
            list[i] = temp;
        }
    }
}

int Game::captureTerritory(Territory *territory) {
    /* Return Key:
     * 0: normal capture
     * 1: captured continent
     * 2: captured world (game over) 
     * 10: normal capture and eliminated a player
     * 11: captured continent and eliminated a player */
    captured = true;
    int returnType = 0;
    Player *player = players[turn];
    Player *prevOwner = territory->owner;
    // give the new player the territory and erase the old player's ownership
    territory->owner = player;
    player->territories.push_back(territory);
    prevOwner->territories.erase(std::find(prevOwner->territories.begin(),
        prevOwner->territories.end(), territory));
    // if the old player owns the continent, remove their ownership there
    if (territory->continent->owner == prevOwner) {
        territory->continent->owner = NULL;
        prevOwner->continents.erase
            (std::find(prevOwner->continents.begin(),
            prevOwner->continents.end(), territory->continent));
    }
    // if the player now owns the continent, add ownership
    if (findContOwner(territory->continent) == player) {
        territory->continent->owner = player;
        player->continents.push_back(territory->continent);
        // check if they have every continent
        if (player->continents.size() == 6) return 2; // -------
        returnType = 1;
    } else {
        returnType = 0;
    }
    // if the old player is dead, erase him from the game and give a card
    if (prevOwner->territories.size() > 0) {
        return returnType;
    } else {
        // remove the player from the player list
        delete prevOwner;
        players.erase(std::find(players.begin(), players.end(), prevOwner));
        if (players[turn] != player) --turn;
        return returnType + 10;
    }
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
    // should only happen for wild cards
    return NULL;
}