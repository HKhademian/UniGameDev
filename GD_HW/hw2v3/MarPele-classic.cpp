#include "MarPele.hpp"

int main() {
	auto game = new Game;
	bench(game);
	delete game;
	return 0;
}
