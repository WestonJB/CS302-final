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
	std::vector<Territory> territories;
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
		std::vector<Card> cards;
		
		void setName(const std::string &newName);
		std::string getName() const;
		void setColor(Color newColor);
		Color getColor() const;
	private:
		std::string name;
		Color color;
};

class Game {
	public:
		std::vector<Player*> players;
		std::vector<Continent> continents;
		std::vector<Card> drawPile;

		Game(const std::vector<std::string> &names); // unfinished
		~Game();
		std::vector<int> rollDice(int numDice) const;
		void setTurn();
		int getTurn() const;
		void endTurn();

		int addArmy(Territory *territory);
		void giveArmies(); // for the start of a turn
		// note: need to make a function to trade infantry

		int setFortify(Territory *start, Territory *end);
		void fortify(const std::vector<char> &armies);

		void giveCard();
		int tradeCards(const std::vector<int> &cardsInd);
	private:
		int turn;
		int terOcc; // territories occupied; used in the setup of the game
		Territory *fortOne; // territories used in fortify
		Territory *fortTwo;
		int infantry; // pieces given to the player at the start of the turn
		int calvary;
		int artillery;
		int trades;

		Player *findContOwner(Continent *continent) const; // can this be a const parameter?
		int captureTerritory(Player *player, Territory *territory); // mostly finished
};

#endif