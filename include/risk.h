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
}; // more colors can be added if we want

struct Territory {
	std::string name;
	Continent *continent;
	Player *owner;
	int armies;
	int infantry;
	int calvary;
	int artillery;
};

struct Continent {
	std::string name;
	Player *owner;
	std::vector<Territory> territories;
};

struct Card {
	std::string territory;
	std::string army;
};

class Player {
	public:
		Color color;
		std::vector<Territory*> territories;
		std::vector<Continent*> continents;
		std::vector<Card> cards;
		
		Player (std::string playerName);
		std::string getName() const;
	private:
		std::string name;
};

class Game {
	public:
		std::vector<Player> players;
		std::vector<Continent> continents;
		std::vector<Card> drawPile;

		Game(std::vector<std::string> names); // unfinished
		int getTurn() const;
		int captureTerritory(Player *player, Territory *territory);
		void endTurn();
	private:
		int turn;
};

#endif