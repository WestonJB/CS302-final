#include <string>
#include <vector>

struct Player;

struct Territory {
	string name;
	Player &owner;
	int armies;
	int infantry;
	int calvary;
	int artillery;
};
struct Continents {
	string name;
	Player &owner;
	std::vector<Territory*> territories;
};
struct Card {
	string territory;
	string army;
}
struct Player {
	string name;
	std::vector<Territory*> territories;
	std::vector<Continents*> continents;
	std::vector<Card*> cards;
};
