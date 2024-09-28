#ifndef RISK_H
#define RISK_H



#include <string>
#include <vector>

struct Player;

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
		std::vector<Territory*> territories;
		std::vector<Continent*> continents;
		std::vector<Card> cards;
		std::string getName() const;
	private:
		std::string name;
};
class Game {
	public:
		Game(std::vector<std::string> names);
		std::vector<Player> players;
		std::vector<Continent> continents;
		std::vector<Card> drawPile;
	private:
		int turn;
		bool setup;
};



#endif
