#include <algorithm>
#include <cstdlib>
#include <string>
#include <vector>
#include "../include/risk.h"

#include <typeinfo>

// Note: does not work for 2 players (might be okay since Risk typically has 3 to 6 players)
Game::Game(const std::vector<std::string> &names) : turn{-1}, terrOcc{0},
        terrOne{NULL}, terrTwo{NULL}, trades{0}, gotTradeBonus{false},
        attArm{0}, captured{true} {
    // initialize the players
    players.resize(names.size());
    for (int i = 0; i < names.size(); i++) {
        players[i] = new Player;
        players[i]->setName(names[i]);
        players[i]->armies = 50 - names.size() * 5;
    }
    // initialize the board
    // allocate and name the continents
    continents.resize(6);
    for (Continent *cont : continents) {
        cont = new Continent;
        cont->newArmies = 4;
    }
    continents[0]->name = "Northwest";
    continents[1]->name = "Southwest";
    continents[2]->name = "South-central";
    continents[3]->name = "Southeast";
    continents[4]->name = "North-central";
    continents[5]->name = "Northeast";
    // allocate and name the territories
    for (Continent *cont : continents) {
        cont->territories.resize(7);
        for (Territory *terr : cont->territories) {
            terr = new Territory;
            terr->continent = cont;
        }
    }
    continents[0]->territories[0]->name = "Iceland";
    continents[0]->territories[1]->name = "Ireland";
    continents[0]->territories[2]->name = "Northern Ireland";
    continents[0]->territories[3]->name = "United Kingdom";
    continents[0]->territories[4]->name = "France";
    continents[0]->territories[5]->name = "Belgium";
    continents[0]->territories[6]->name = "Netherlands";
    continents[1]->territories[0]->name = "Portugal";
    continents[1]->territories[1]->name = "Spain";
    continents[1]->territories[2]->name = "Morocco";
    continents[1]->territories[3]->name = "Algeria";
    continents[1]->territories[4]->name = "Tunisia";
    continents[1]->territories[5]->name = "Italy";
    continents[1]->territories[6]->name = "Switzerland";
    continents[2]->territories[0]->name = "Austria";
    continents[2]->territories[1]->name = "Hungary";
    continents[2]->territories[2]->name = "Slovakia";
    continents[2]->territories[3]->name = "Slovenia";
    continents[2]->territories[4]->name = "Croatia";
    continents[2]->territories[5]->name = "Bosnia and Herzegovina";
    continents[2]->territories[6]->name = "Belgrade";
    continents[3]->territories[0]->name = "Romania";
    continents[3]->territories[1]->name = "Bulgaria";
    continents[3]->territories[2]->name = "Turkey";
    continents[3]->territories[3]->name = "Greece";
    continents[3]->territories[4]->name = "Moldova";
    continents[3]->territories[5]->name = "Albnia";
    continents[3]->territories[6]->name = "Macedonia";
    continents[4]->territories[0]->name = "Norway";
    continents[4]->territories[1]->name = "Sweden";
    continents[4]->territories[2]->name = "Denmark";
    continents[4]->territories[3]->name = "Germany";
    continents[4]->territories[4]->name = "Poland";
    continents[4]->territories[5]->name = "Czech Republic";
    continents[4]->territories[6]->name = "Kaliningrad";
    continents[5]->territories[0]->name = "Russia";
    continents[5]->territories[1]->name = "Finland";
    continents[5]->territories[2]->name = "Latvia";
    continents[5]->territories[3]->name = "Lithuania";
    continents[5]->territories[4]->name = "Belarus";
    continents[5]->territories[5]->name = "Ukraine";
    continents[5]->territories[6]->name = "Estonia";
    // we need to set adjeceny lists HERE
    // initialize the drawPile
    drawPile = {new Card{"wild", "wild"}, new Card{"wild", "wild"},
        new Card{"Iceland", "Infantry"}, new Card{"Ireland", "Cavalry"}, new
        Card{"Northern Ireland", "Artillery"}, new Card{"United Kingdom",
        "Infantry"}, new Card{"France", "Cavalry"}, new Card{"Belgium",
        "Artillery"}, new Card{"Netherlands", "Infantry"}, new Card{"Portugal",
        "Cavalry"}, new Card{"Spain", "Artillery"}, new Card{"Morocco",
        "Infantry"}, new Card{"Algeria", "Cavalry"}, new Card{"Tunisia",
        "Artillery"}, new Card{"Italy", "Infantry"}, new Card{"Switzerland",
        "Artillery"}, new Card{"Austria", "Infantry"}, new Card{"Hungary",
        "Cavalry"}, new Card{"Slovakia", "Cavalry"}, new Card{"Slovenia",
        "Infantry"}, new Card{"Croatia", "Cavalry"}, new
        Card{"Bosnia and Herzegovina", "Artillery"}, new Card{"Belgrade",
        "Infantry"}, new Card{"Romania", "Cavalry"}, new Card{"Bulgaria",
        "Infantry"}, new Card{"Turkey", "Cavalry"}, new Card{"Greece",
        "Artillery"}, new Card{"Moldova", "Artillery"}, new Card{"Albania",
        "Infantry"}, new Card{"Macedonia", "Cavalry"}, new Card{"Norway",
        "Artillery"}, new Card{"Sweden", "Infantry"}, new Card{"Denmark",
        "Cavalry"}, new Card{"Germany", "Artillery"}, new Card{"Poland",
        "Infantry"}, new Card{"Czech Republic", "Cavalry"}, new
        Card{"Kaliningrad", "Artillery"}, new Card{"Russia", "Infantry"}, new
        Card{"Finland", "Cavalry"}, new Card{"Latvia", "Artillery"}, new
        Card{"Lithuania", "Infantry"}, new Card{"Belarus", "Cavalry"}, new
        Card{"Ukraine", "Artillery"}, new Card{"Estonia", "Artillery"}};
    // shuffle the drawPile
    int temp;
    Card *temp2;
    for (int i = 0; i < drawPile.size() - 1; i++) {
        temp = std::rand() % (drawPile.size() - i) + i;
        temp2 = drawPile[i];
        drawPile[i] = drawPile[temp];
        drawPile[temp] = temp2;
    }
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
    attArm = 0;
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
    if (terrOcc < 42) {
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
    Player *player = players[turn];
    if (start->owner != player) return 1;
    if (end->owner == player) return 2;
    if (!areTerritoriesConnected(start, end)) return 3;
    if (start->armies < 2) return 4;
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
     * 5: error, player one must have at least one more infantry than dice
     * 6: error, player two must roll 1-2 dice
     * 7: error, player two must have at least one infantry per die */
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
        // note: the player needs to move a piece to the captured territory
        attArm = playerOneDice;
        return captureVal + 1;
    }
    return 0;
}

int Game::occupyTerritory(const std::vector<char> &armies) {
    /* Return Key:
     * 0: occupied the territory
     * 1: error, need as many armies as dice rolled to capture (attArm) */
    int movedArmies = 0;
    for (char piece : armies) {
        switch (piece) {
            case 'i':
                movedArmies += 1;
                break;
            case 'c':
                movedArmies += 5;
                break;
            case 'a':
                movedArmies += 10;
                break;
        }
    }
    if (movedArmies < attArm) return 1;
    fortify(armies);
    attArm = 0;
    return 0;
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
    removeElement(player->cards, player->cards[cardsInd[0]]);
    removeElement(player->cards, player->cards[cardsInd[1]]);
    removeElement(player->cards, player->cards[cardsInd[2]]);
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
    removeElement(prevOwner->territories, territory);
    // if the old player owns the continent, remove their ownership there
    if (territory->continent->owner == prevOwner) {
        territory->continent->owner = NULL;
        removeElement(prevOwner->continents, territory->continent);
    }
    // if the player now owns the continent, add ownership
    if (findContOwner(territory->continent) == player) {
        territory->continent->owner = player;
        player->continents.push_back(territory->continent);
        // check if they have every continent
        if (player->continents.size() == 6) return 2;
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
        removeElement(players, prevOwner);
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

template <typename T>
bool Game::removeElement(std::vector<T> &list, T &element) {
    for (int i = 0; i < list.size(); i++) {
        if (list[i] = element) {
            for (int j = i + 1; j < list.size(); j++) {
                list[j - 1] = list[j];
            }
            list.pop_back();
            return true;
        }
    }
    return false;
}
