#ifndef GD_MAR_PELE
#define GD_MAR_PELE

#include <iostream>
#include <vector>
#include <queue>

#include "EventSystem.hpp"
#include "Colors.hpp"

#define BENCH


#pragma region Event

enum GameEvent {
	/// when a players get turn to roll dice
	PlayerTurn,

	/// fires each time dice generates a new value
	DiceRolled,

	/// fires each time dice generates a new value in battles
	FightDiceRolled,

	KnightMoves,

	KnightKill,

	PlayerWin,
};

#pragma endregion Event


#pragma region GameObjects


// every knight | snake(head,tail) | ladder(start,end) has a position
typedef unsigned int Position;

// a ladder has a start and end point
class Ladder {
public:
	friend std::ostream &operator<<(std::ostream &os, const Ladder &ladder);

	const Position start = 0, end = 0;
};

// a snake has a head , when a players knight stands in it and lose battle to snake , it must return to snakes tail
class Snake {
public:
	friend std::ostream &operator<<(std::ostream &os, const Snake &snake);

	/// power to fight in a battle against knights
	const unsigned int power = 10;

	const Position head = 0, tail = 0;
};

// every players has properties and stats across all rounds (each board he/she plays)
class Player {
public:
	friend std::ostream &operator<<(std::ostream &os, const Player &player);

	const std::string title;

	/// players knights color
	const Color color = White;

	/// number of game won
	unsigned int boardWins = 0;

	/// num of game lost
	unsigned int boardLosses = 0;

	/// num of all opponent knights killed in battle in all games
	unsigned int knightKill = 0;

	/// num of all knights killed by opponent in battle in all games
	unsigned int knightLost = 0;
};

// a knight is a colored peace with a power factor (to battle snake)
class Knight {
public:

	friend std::ostream &operator<<(std::ostream &os, const Knight &knight);

	/// power of knight in a battle against other knights / snakes
	const unsigned int power = 10;

	Player *player = nullptr;

	Position pos = 0;
};

// a board is list of elements in game , like knights , ladders , snakes , s/e positions
// it has no understanding of game logic
class Board {
public:
	~Board();

	void initSample1();

	Position size = 0, start = 0, end = 0;
	std::vector<Knight *> knights;
	std::vector<Snake *> snakes;
	std::vector<Ladder *> ladders;
};

#pragma endregion GameObjects


#pragma region GameEngine

class Game;

// ai players controller
class AI : private EventObserver<GameEvent> {
public:
	explicit AI(Game *game);

	~AI();

private:
	bool onEvent(GameEvent event, void *caller, void *eventData) override;

	bool onEvent_PlayerTurn(Player *player) const;

	bool onEvent_DiceRolled(Player *player, unsigned int diceValue);

	Game *game;
};

// controls how game dynamics and physics works (battles , wins , looses , ...)
// may have multiple variants
class GamePhysics : private EventObserver<GameEvent> {
public:
	explicit GamePhysics(Game *game, unsigned maxKnights);

	~GamePhysics();

	virtual void processCommandQueue() = 0;

	virtual bool canPlayerAddKnight(Player *player, unsigned int diceValue) = 0;

public:
	unsigned luckyValue = 6;

protected:
	unsigned maxKnights;
	Game *game;
};

// classic MarPele game physics (no attack , ...)
class ClassicGamePhysics : public GamePhysics {
public:
	explicit ClassicGamePhysics(Game *game) : GamePhysics(game, 5) {}

	void processCommandQueue() override;

	bool canPlayerAddKnight(Player *player, unsigned int diceValue) override;

protected:
	bool onEvent(GameEvent event, void *caller, void *eventData) override;

	virtual bool onEvent_KnightMoves(Knight *knight, Position toPos);

	virtual bool onEvent_KnightKill(Knight *knight, Knight *otherKnight);
};

class Command {
public:
	virtual ~Command() = default;

public:
	virtual bool redo(Game *game) = 0;

	virtual bool undo(Game *game) { return false; }
};

class RollDiceCommand : public Command {
public:
	explicit RollDiceCommand(Player *player) : player(player) {}

	bool redo(Game *game) override;

	Player *player = nullptr;
};

class MoveKnightCommand : public Command {
public:
	explicit MoveKnightCommand(Knight *knight, Position from, Position to)
			: knight(knight), from(from), to(to) {}

	explicit MoveKnightCommand(Knight *knight, Position offset)
			: MoveKnightCommand(knight, knight->pos, knight->pos + offset) {}

	bool redo(Game *game) override;

	Knight *knight = nullptr;
	Position from = 0, to = 0;
};

class EnterKnightCommand : public Command {
public:
	explicit EnterKnightCommand(Player *player)
			: player(player) {}

	bool redo(Game *game) override;

	Player *player = nullptr;
};

class ClimbLadderCommand : public Command {
public:
	explicit ClimbLadderCommand(Knight *knight, Ladder *ladder)
			: knight(knight), ladder(ladder) {}

	bool redo(Game *game) override;

	Knight *knight = nullptr;
	Ladder *ladder = nullptr;
};

class FightSnakeCommand : public Command {
public:
	explicit FightSnakeCommand(Knight *knight, Snake *snake)
			: knight(knight), snake(snake) {}

	bool redo(Game *game) override;

	Knight *knight = nullptr;
	Snake *snake = nullptr;
};

class FightKnightCommand : public Command {
public:
	explicit FightKnightCommand(Knight *knight, Knight *otherKnight)
			: knight(knight), otherKnight(otherKnight) {}

	bool redo(Game *game) override;

	Knight *knight = nullptr;
	Knight *otherKnight = nullptr;
};

#pragma endregion

#pragma region Game

void bench(Game *game);

// gameLoop , system glue
class Game : public EventSubject<GameEvent> {
public:
	Board *board = nullptr;
	std::vector<Player *> players;
	Player *winner = nullptr;

	std::queue<Command *> commandQueue;
	GamePhysics *physics = nullptr;
	AI *ai = nullptr;

	Game(GamePhysics *physics, AI *ai);

	Game();

	~Game();

	/** responsible to clear all data to new */
	void newGame();

	/** new board to play */
	void newBoard();

	/** prints game stats for each players */
	void drawStats();

	/** main application lives here */
	int gameLoop(int gameID);

	using EventSubject::triggerEvent;
};

#pragma region Game

#include "Dice.hpp"
#include "util.hpp"

#include <chrono>
#include <sstream>


#pragma region Game

void bench(Game *game) {
	auto all_start = std::chrono::high_resolution_clock::now();
	for (auto gameID = 1; gameID <= 1000; gameID++) {
		auto start = std::chrono::high_resolution_clock::now();

#ifndef BENCH
		TERM_COLOR(std::cout, 30, "\n\nGame " << gameID + 1 << std::endl);
#endif

		game->newBoard();

		auto loops = game->gameLoop(gameID);

		auto stop = std::chrono::high_resolution_clock::now();
		auto gameTime = duration_cast<std::chrono::milliseconds>(stop - start).count();

#ifndef BENCH
		std::cout << "\nGame#" << gameID << " time: " << gameTime << std::endl;
#else
		std::cout << gameID << ";" << game->winner->title << ";" << loops << ";" << gameTime << std::endl;
#endif
	}
	auto all_stop = std::chrono::high_resolution_clock::now();
	auto all_gameTime = duration_cast<std::chrono::milliseconds>(all_stop - all_start).count();

#ifndef BENCH
	game->drawStats();
	std::cout << "All Games time: " << all_gameTime << std::endl;
#endif
}

Game::Game(GamePhysics *physics, AI *ai) : physics(physics), ai(ai) {
	newGame();
}

Game::Game() : Game(nullptr, nullptr) {
	physics = new ClassicGamePhysics(this);
	ai = new AI(this);
}

Game::~Game() {
	delete board;
	clearQueue(commandQueue);
	clearVector(players);
}

void Game::newGame() {
	clearVector(players);

	for (auto color : {Red, Green, Blue, Yellow}) {
		std::ostringstream title;
		title << "AI-" << color;
		players.push_back(new Player{
				.title = title.str(),
				.color = color,
		});
	}
	newBoard();
}

void Game::newBoard() {
	clearQueue(commandQueue);

	delete board;
	board = new Board;
	board->initSample1();

	winner = nullptr;
}

void Game::drawStats() {
	TERM_COLOR(std::cout, 35, std::endl
			<< "******************" << std::endl
			<< "*** draw stats ***" << std::endl
			<< "******************" << std::endl
	);
	for (auto player:players) {
		std::cout << *player << " Wins: " << player->boardWins << std::endl;
	}
}

int Game::gameLoop(int gameID) {
	auto i = 0;
	while (winner == nullptr) {
		++i;
#ifndef BENCH
		std::cout << std::endl << "game#" << gameID << " - loop " << i << std::endl;
#endif
		for (auto player : players) {
			if (winner != nullptr) break;
			triggerEvent(PlayerTurn, this, player);
			physics->processCommandQueue();
		}
	}
	return i;
}

#pragma endregion Game


#pragma region GameObjects

void Board::initSample1() {
	clearVector(knights);
	clearVector(snakes);
	clearVector(ladders);

	size = end = 100;
	start = 1;

	ladders.push_back(new Ladder{.start = 9, .end=30});
	ladders.push_back(new Ladder{.start = 15, .end=56});
	ladders.push_back(new Ladder{.start = 30, .end=45});
	ladders.push_back(new Ladder{.start = 62, .end=76});
	ladders.push_back(new Ladder{.start = 47, .end=89});

	snakes.push_back(new Snake{.head = 98, .tail=73});
	snakes.push_back(new Snake{.head = 93, .tail=67});
	snakes.push_back(new Snake{.head = 61, .tail=19});
	snakes.push_back(new Snake{.head = 54, .tail=37});
	snakes.push_back(new Snake{.head = 34, .tail=2});
	snakes.push_back(new Snake{.head = 17, .tail=5});

}

Board::~Board() {
	clearVector(knights);
	clearVector(snakes);
	clearVector(ladders);
}

#pragma endregion GameObjects


#pragma region GameEngine

AI::AI(Game *game) : game(game) {
	game->addObserver(this);
}

AI::~AI() {
	game->removeObserver(this);
}

bool AI::onEvent(GameEvent event, void *caller, void *eventData) {
	if (event == PlayerTurn)
		return onEvent_PlayerTurn((Player *) eventData);

	if (event == DiceRolled)
		return onEvent_DiceRolled((Player *) caller, *(int *) eventData);

	return false;
}

bool AI::onEvent_PlayerTurn(Player *player) const {
#ifndef BENCH
	std::cout << *player << " TURN" << std::endl;
#endif
	game->commandQueue.push(new RollDiceCommand(player));
	return true;
}

bool AI::onEvent_DiceRolled(Player *player, unsigned diceValue) {
#ifndef BENCH
	std::cout << *player << " Diced: " << diceValue << std::endl;
#endif

	bool done = false;

	// check win condition
	for (auto knight:game->board->knights) {
		Dice::wait(25, 50);
		if (knight->player == player && (knight->pos + diceValue) == game->board->end) {
			game->commandQueue.push(new MoveKnightCommand(knight, diceValue));
			done = true;
		}
	}


	if (!done) {
		Dice::wait(50, 100);
		if (game->physics->canPlayerAddKnight(player, diceValue)) {
#ifndef BENCH
			std::cout << *player << " enters new knight" << std::endl;
#endif
			game->commandQueue.push(new EnterKnightCommand(player));
			done = true;
		}
	}

	if (!done) {
		for (auto knight : game->board->knights) {
			if (knight->player != player) continue;
			Dice::wait(25, 50);
			if (knight->pos + diceValue > game->board->end) continue;
#ifndef BENCH
			// std::cout << *player << " moves " << *knight << std::endl;
#endif
			game->commandQueue.push(new MoveKnightCommand(knight, diceValue));
			done = true;
			break;
		}
	}

	if (!done) {
#ifndef BENCH
		std::cout << *player << " STUCK " << std::endl;
#endif
	}

	if (diceValue == game->physics->luckyValue) {
		Dice::wait(100, 250);
#ifndef BENCH
		std::cout << *player << " lucky turn" << std::endl;
#endif
		game->commandQueue.push(new RollDiceCommand(player));
	}

	return true;
}

GamePhysics::GamePhysics(Game *game, unsigned maxKnights) : game(game), maxKnights(maxKnights) {
	game->addObserver(this);
}

GamePhysics::~GamePhysics() {
	game->removeObserver(this);
}

bool ClassicGamePhysics::canPlayerAddKnight(Player *player, unsigned int diceValue) {
	if (diceValue != luckyValue) return false;

	unsigned playerKnights = 0;
	for (auto knight:game->board->knights)
		if (knight->player == player)
			playerKnights++;

	return playerKnights < maxKnights;
}

void ClassicGamePhysics::processCommandQueue() {
	while (!game->commandQueue.empty() && game->winner == nullptr) {
		auto &command = *game->commandQueue.front();
		game->commandQueue.pop();
		command.redo(game);
	}
}

bool ClassicGamePhysics::onEvent(GameEvent event, void *caller, void *eventData) {
	if (event == KnightMoves)
		return onEvent_KnightMoves((Knight *) caller, *(Position *) eventData);

	if (event == KnightKill)
		return onEvent_KnightKill((Knight *) caller, (Knight *) eventData);

	if (event == PlayerWin) {
		// auto knight = (Knight *) caller;
		auto player = (Player *) eventData;
		player->boardWins += 1;
		game->winner = player;
		return true;
	}

	return false;
}

bool ClassicGamePhysics::onEvent_KnightMoves(Knight *knight, Position toPos) {
	if (toPos == game->board->end) {
		game->triggerEvent(PlayerWin, knight, knight->player);
		return true;
	}

	for (auto ladder : game->board->ladders) {
		if (ladder->start == toPos) {
			Dice::wait(200);
			game->commandQueue.push(new ClimbLadderCommand(knight, ladder));
			return true;
		}
	}

	for (auto snake : game->board->snakes) {
		if (snake->head == toPos) {
			Dice::wait(200);
			game->commandQueue.push(new FightSnakeCommand(knight, snake));
			return true;
		}
	}

	for (auto otherKnight : game->board->knights) {
		if (otherKnight->pos == toPos && otherKnight->player != knight->player) {
			Dice::wait(200);
			game->commandQueue.push(new FightKnightCommand(knight, otherKnight));
			return true;
		}
	}

	return true;
}

bool ClassicGamePhysics::onEvent_KnightKill(Knight *knight, Knight *otherKnight) {
	// std::cout << "KnightKill" << std::endl;
#ifndef BENCH
	removeElement(
			game->board->knights,
			otherKnight,
					return std::cout << *knight << " kills " << *otherKnight << std::endl, true;
	);
#else
	removeElement(
			game->board->knights,
			otherKnight,
			return true;
	);
#endif
	return true;
}

bool RollDiceCommand::redo(Game *game) {
	auto value = Dice::roll();
#ifndef BENCH
	// std::cout << *player << " Rolls Dice and get *** " << value << " ***" << std::endl;
#endif
	game->triggerEvent(DiceRolled, player, &value);
	return true;
}

bool MoveKnightCommand::redo(Game *game) {
#ifndef BENCH
	std::cout << *knight->player << " Moves  " << *knight << " to " << to << std::endl;
#endif
	knight->pos = to;
	game->triggerEvent(KnightMoves, knight, &to);
	return true;
}

bool EnterKnightCommand::redo(Game *game) {
	game->board->knights.push_back(new Knight{
			.player=player,
			.pos = game->board->start,
	});
	return true;
}

bool ClimbLadderCommand::redo(Game *game) {
#ifndef BENCH
	std::cout << *knight << " climbs ladder " << *ladder << std::endl;
#endif
	game->commandQueue.push(new MoveKnightCommand(knight, ladder->start, ladder->end));
	return true;
}

bool FightSnakeCommand::redo(Game *game) {
#ifndef BENCH
	std::cout << *knight << " eaten by " << *snake << std::endl;
#endif
	game->commandQueue.push(new MoveKnightCommand(knight, snake->head, snake->tail));
	return true;
}

bool FightKnightCommand::redo(Game *game) {
	game->triggerEvent(KnightKill, knight, otherKnight);
	return true;
}

#pragma endregion GameEngine


#pragma region Util

std::ostream &operator<<(std::ostream &os, const Ladder &ladder) {
	return os << TERM_CYAN << "Ladder[" << ladder.start << ":" << ladder.end << "]" << TERM_RESET;
}

std::ostream &operator<<(std::ostream &os, const Snake &snake) {
	return os << TERM_MAGENTA << "Snake[" << snake.head << ":" << snake.tail << "]" << TERM_RESET;
}

std::ostream &operator<<(std::ostream &os, const Player &player) {
	return TERM_COLOR(os, ColorFores[player.color], player.title);
	// return term::color(os, ColorFores[player.color]) << player.title << TERM_RESET;
}

std::ostream &operator<<(std::ostream &os, const Knight &knight) {
	return os << *knight.player << " Knight@" << knight.pos;
}

#pragma endregion Util


#endif


