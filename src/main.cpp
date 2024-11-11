#include <chrono>
#include <cstdlib>
#include <iostream> // for now; obviously, we're using unreal engine
#include "risk.h"

int main(int argc, char *argv[]) {

	///////////
	// SETUP //
	///////////

	// set the seed for randomness based off of time
	std::srand(std::chrono::duration_cast<std::chrono::milliseconds>
		(std::chrono::system_clock::now().time_since_epoch()).count());
	// ask for the number of players and their names [HERE]
	// I'll just choose some fixed data for now
	std::vector<std::string> playerNames = {"Bob", "Alex", "Joe", "Fred"};
	Game game(playerNames);
	for (int i = 0; i < playerNames.size(); i++) {
		// ask the players for their colors
		// I'll make everyone choose red for sake of ease
		game.players[game.getTurn()].color = red;
		game.endTurn();
	}
	// make everyone roll a die, choose the highest die for the first person
	// if there is a tie, make the people with the tie roll again
	// start giving out territories starting with the winner of the roll

	//////////////
	// GAMEPLAY //
	//////////////
	
	return 0;
}
