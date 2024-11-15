#ifndef RISK_H
#define RISK_H

#include <string>
#include <vector>

class Player;
class Continent;

enum Color {
	red,
	orange,
	yellow,
	green,
	blue,
	purple,
	pink,
	white,
	black,
	grey
}; // more or less colors can be added if we want

struct Territory {
	std::string name;
	Continent *continent;
	Player *owner;
	int armies;
	int infantry;
	int calvary;
	int artillery;
	std::vector<Territory*> nearTerritories;
};

struct Continent {
	std::string name;
	Player *owner;
	std::vector<Territory*> territories;
	std::vector<Continent*> nearContinents;
	int newArmies;
};

// note: wild cards have "wild" for both members
struct Card {
	std::string territory;
	std::string army;
};

class Player {
	public:
		std::vector<Territory*> territories;
		std::vector<Continent*> continents;
		std::vector<Card*> cards;
		int armies;
		
		void setName(const std::string &newName);
		std::string getName() const;
		void setColor(Color newColor);
		Color getColor() const;
	private:
		std::string name;
		Color color;
};

// note: we need to determine when they NEED to trade cards
class Game {
	public:
		std::vector<Player*> players;
		std::vector<Continent*> continents;

		Game(const std::vector<std::string> &names); // unfinished
		~Game();
		std::vector<int> rollDice(int numDice) const;
		void setTurn(int newTurn);
		int getTurn() const;
		void endTurn();

		int addArmy(Territory *territory);
		void giveArmies(); // for the start of a turn
		int tradeArmies(Territory *territory, char startType, char endType);

		int setFortify(Territory *start, Territory *end);
		void fortify(const std::vector<char> &armies);

		void giveCard();
		int tradeCards(const std::vector<int> &cardsInd);
	private:
		int turn;
		int terrOcc;         // territories occupied; used in setup of the game
		std::vector<Card*> drawPile;
		std::vector<Card*> discardPile;
		Territory *fortOne;  // territories used in fortify
		Territory *fortTwo;
		int trades;
		bool alreadyTraded;  // for trading cards
		bool captured;       // for getting cards

		Player *findContOwner(const Continent *continent) const;
		int captureTerritory(Territory *territory);
		bool isValidTrade(const std::vector<int> &cardsInd) const;
		Territory *findTerritory(const std::string &name) const;
};

#endif