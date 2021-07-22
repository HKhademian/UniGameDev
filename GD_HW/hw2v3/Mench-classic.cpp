#include "Mench.hpp"


int main() {
	std::cout << "main""\n";
	auto game = new Game;
	game->newGame();

	auto all_start = std::chrono::high_resolution_clock::now();
	for (auto gameID = 1; gameID <= 100; gameID++) {
		auto start = std::chrono::high_resolution_clock::now();

		TERM_COLOR(std::cout, 30, "\n\nGame " << gameID + 1 << std::endl);

		std::cout << "game" << gameID << "\n";
		game->newBoard();

		game->gameLoop(gameID);

		auto stop = std::chrono::high_resolution_clock::now();
		auto gameTime = duration_cast<std::chrono::milliseconds>(stop - start).count();

		std::cout << std::endl;
		for (auto knight:game->board->knights) {
			std::cout << *knight << std::endl;
		}

		std::cout << "\nGame#" << gameID << " time: " << gameTime << std::endl;
	}
	auto all_stop = std::chrono::high_resolution_clock::now();
	auto all_gameTime = duration_cast<std::chrono::milliseconds>(all_stop - all_start).count();

	game->drawStats();
	std::cout << "All Games time: " << all_gameTime << std::endl;
	return 0;
}
