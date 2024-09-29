#ifndef RISK_H
#define RISK_H



#include <string>
#include <vector>

struct Player;

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
		Player (std::string playerName);
		Color color;
		std::vector<Territory*> territories;
		std::vector<Continent*> continents;
		std::vector<Card> cards;
		std::string getName() const;
		int captureTerritory(Territory *territory); // unfinished
	private:
		std::string name;
};
class Game {
	public:
		Game(std::vector<std::string> names); // unfinished
		std::vector<Player> players;
		std::vector<Continent> continents;
		std::vector<Card> drawPile;
		void endTurn();
	private:
		int turn;
};



#endif