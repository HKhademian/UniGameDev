#pragma once

#include <queue>
#include "util.hpp"
#include "Terminal.hpp"
#include "Colors.hpp"
#include "Dice.hpp"
#include "EventSystem.hpp"

typedef int Position;

class Player;

class Region;

class Board;

class Knight;

class Game;

class GameCommand;

class GamePhysics;

/// each game player has an identity around all boards
struct Player {
public:
	unsigned id;
	std::string title;
	Color color;
	unsigned ai = 0; // -1=real-user  0=default-ai   1=easy-ai   2=medium-ai    3=hard-ai

	friend std::ostream &operator<<(std::ostream &os, const Player &knight);

	friend bool operator==(const Player &lhs, const Player &rhs);

	friend bool operator!=(const Player &lhs, const Player &rhs);
};

struct Board {
public:
	Game &game;
	unsigned int knightCount;
	unsigned int regionSize;
	std::vector<Region *> regions;
	std::vector<Knight *> knights;
	bool finished = false;

	Board(Game &game, unsigned knightCount, unsigned regionSize, std::vector<Player> &players);

	~Board();

	friend std::ostream &operator<<(std::ostream &os, const Board &knight);

private:
	friend class Game;
};

struct Region {
public:
	Player &player;
	Position pos;

	friend std::ostream &operator<<(std::ostream &os, const Region &knight);
};


struct Knight {
public:
	Region &region;
	Position pos;

	friend std::ostream &operator<<(std::ostream &os, const Knight &knight);
};


enum GameEvent {
	/// when a players get turn to roll dice
	PlayerTurn,

	/// fires each time dice generates a new value
	DiceRolled,

	/// fires when a knight enters to home (from parking)
	KnightEntered,

	/// fires every time a knight moves
	KnightMoved,

	/// fires when a knight moves [to] stack
	KnightStacked,

	/// fires when a knight attacks another knight
	KnightAttacked,

	/// fires when all player's knight move to stack
	PlayerWin,
};


#pragma region GameCommands

struct GameCommand {
public:
	virtual ~GameCommand() = default;

	virtual bool redo(Game &game) = 0;

	bool undo(Game &game) { return false; };
};

struct RollDiceCommand : public GameCommand {
public:
	explicit RollDiceCommand(Player &player) : player(player) {}

	bool redo(Game &game) override;

	Player &player;
};

class EnterKnightCommand : public GameCommand {
public:
	explicit EnterKnightCommand(Knight &knight) : knight(knight) {}

	bool redo(Game &game) override;

	Knight &knight;
};

class MoveKnightCommand : public GameCommand {
public:
	explicit MoveKnightCommand(Knight &knight, Position from, Position to)
			: knight(knight), from(from), to(to) {}

	explicit MoveKnightCommand(Knight &knight, Position offset)
			: MoveKnightCommand(knight, knight.pos, knight.pos + offset) {}

	bool redo(Game &game) override;

	Knight &knight;
	Position from = 0, to = 0;
};

#pragma endregion GameCommands

class GameObserver : private EventObserver<GameEvent> {
protected:
	explicit GameObserver(Game &game);

	virtual ~GameObserver();

	Game &game;
};

class GamePhysics : protected GameObserver {
public:
	virtual bool queue(GameCommand *command, void *caller) = 0;

	~GamePhysics() override = default;

	virtual Knight *canPlayerAddKnight(Player &player, unsigned diceValue) = 0;

	/// determine whether knight can move or not
	virtual bool canKnightMove(Knight &knight, unsigned diceValue) = 0;

	/// determine whether knight can move to a stack cell or not
	virtual bool canKnightStackMove(Knight &knight, unsigned diceValue) = 0;

	/// determine whether knight can move to a stack cell or not
	virtual bool canKnightAttackMove(Knight &knight, Knight &otherKnight, unsigned diceValue) = 0;

	/// is pos a stack
	virtual bool isStack(Position pos) = 0;

	unsigned luckyValue = 6;


protected:
	friend class Game;

	virtual void dropAll() = 0;

	explicit GamePhysics(Game &game);

	virtual bool process() = 0;
};

/// GameAI plays behalf of ai-players
class GameAI : protected GameObserver {
public:
	~GameAI() override = default;

protected:
	explicit GameAI(Game &game);
};

class ClassicGamePhysics : public GamePhysics {
public:
	explicit ClassicGamePhysics(Game &game);

	~ClassicGamePhysics() override;

	bool queue(GameCommand *command, void *caller) override;

	Knight *canPlayerAddKnight(Player &player, unsigned diceValue) override;

	bool canKnightMove(Knight &knight, unsigned diceValue) override;

	bool canKnightStackMove(Knight &knight, unsigned diceValue) override;

	bool canKnightAttackMove(Knight &knight, Knight &otherKnight, unsigned diceValue) override;

	bool isStack(Position pos) override;

protected:
	void dropAll() override;

	bool process() override;

private:
	std::queue<GameCommand *> commandQueue;

	bool onEvent(GameEvent event, void *caller, void *eventData) override;
};

class ClassicGameAI : public GameAI {
public:
	explicit ClassicGameAI(Game &game);

private:
	bool onEvent(GameEvent event, void *caller, void *eventData) override;
};

class Game : private EventSubject<GameEvent> {
public:
	explicit Game(
			GameAI *ai, GamePhysics *physics,
			unsigned regionCount, unsigned regionSize, unsigned knightCount
	);

	explicit Game();

	~Game() {
		if (deconst) {
			delete &ai;
			delete &physics;
		}
	}

	void newGame();

	void newBoard();

	void gameLoop(unsigned gameID);

	void drawStats();

public:
	std::vector<Player> players;
	GamePhysics *physics;
	unsigned knightCount;
	unsigned regionSize;
	Board *board = nullptr;

	using EventSubject::triggerEvent;

private:
	GameAI *ai;
	bool deconst;

	friend class GameObserver;
};


#include <iostream>
#include <sstream>
#include "Colors.hpp"

const Position homePos = 0, parkPos = (Position) -1;

void Game::gameLoop(unsigned gameID) {
	std::cout << "Game::gameLoop" << gameID << "\n";
	auto i = 0;
	while (!board->finished) {
		std::cout << "\nGame #" << gameID << " - Loop " << ++i << std::endl;
		for (auto &player:players) {
			triggerEvent(PlayerTurn, this, &player);
			if (!physics->process())break;
		}
	}
}

Game::Game(GameAI *ai, GamePhysics *physics, unsigned regionCount, unsigned regionSize, unsigned knightCount)
		: ai(ai), physics(physics), regionSize(regionSize), knightCount(knightCount), deconst(false) {
	std::cout << "Game::Game" "\n";
	for (auto i = 0; i < regionCount; i++) {
		Color color = Colors[i];

		std::ostringstream title;
		title << "AI-" << color;

		players.push_back(Player{
				.id = (unsigned) i + 1,
				.title = title.str(),
				.color = color,
				.ai=0//default ai
		});
	}
}

Game::Game() : Game(nullptr, nullptr, 4, 10, 4) {
	std::cout << "Game::Game""\n";
}

void Game::newGame() {
	std::cout << "Game::newGame""\n";
	if (ai == nullptr)
		ai = new ClassicGameAI(*this);
	if (physics == nullptr)
		physics = new ClassicGamePhysics(*this);
	newBoard();
}

void Game::newBoard() {
	std::cout << "Game::newBoard" << "\n";
	physics->dropAll();
	delete board;
	board = new Board(*this, knightCount, regionSize, players);
	board->finished = false;
}

void Game::drawStats() {
	std::cout << "Game::drawStats""\n";
}

Board::Board(Game &game, unsigned knightCount, unsigned regionSize, std::vector<Player> &players)
		: game(game), knightCount(knightCount), regionSize(regionSize) {
	std::cout << "Board::Board" "\n";
	for (Position i = 0; i < players.size(); i++) {
		auto region = new Region{
				.player = players[i],
				.pos=i,
		};
		regions.push_back(region);
		for (auto j = 0; j < knightCount; j++) {
			knights.push_back(new Knight{
					.region=*region,
					.pos=parkPos, // in parking
			});
		}
	}
}

Board::~Board() {
	clearVector(regions);
	clearVector(knights);
}

GameObserver::GameObserver(Game &game) : game(game) {
	std::cout << "GameObserver::GameObserver" "\n";
	game.addObserver(this);
}

GameObserver::~GameObserver() {
	std::cout << "GameObserver::~GameObserver" "\n";
	game.removeObserver(this);
}


#pragma region Physics

GamePhysics::GamePhysics(Game &game) : GameObserver(game) {
	std::cout << "GamePhysics::GamePhysics" "\n";
}

ClassicGamePhysics::ClassicGamePhysics(Game &game) : GamePhysics(game) {
	std::cout << "ClassicGamePhysics::ClassicGamePhysics" "\n";
}

ClassicGamePhysics::~ClassicGamePhysics() {
	std::cout << "~ClassicGamePhysics" "\n";
	clearQueue(commandQueue);
}

void ClassicGamePhysics::dropAll() {
	std::cout << "dropAll" "\n";
	clearQueue(commandQueue);
}

bool ClassicGamePhysics::queue(GameCommand *command, void *caller) {
//	std::cout << "queue" "\n";
	// TODO: check game logic / dynamics / physics
	commandQueue.push(command);
	return true;
}

bool ClassicGamePhysics::process() {
//	std::cout << "process" "\n";
	while (!game.board->finished && !commandQueue.empty()) {
		auto &command = *commandQueue.front();
		commandQueue.pop();
		// TODO: check command before execute
		command.redo(game);
	}
	return !game.board->finished;
}

Knight *ClassicGamePhysics::canPlayerAddKnight(Player &player, unsigned diceValue) {
	if (diceValue != luckyValue)return nullptr;
	Knight *selected = nullptr;
	for (auto knight:game.board->knights) {
		if (knight->region.player == player) {
			if (knight->pos == homePos)
				return nullptr;
			if (knight->pos == parkPos) {
				selected = knight;
			}
		}
	}
	return selected;
}

bool ClassicGamePhysics::canKnightMove(Knight &knight, unsigned diceValue) {
	if (knight.pos == parkPos) return false;//not in game

	const auto boardSize = game.board->regionSize * game.players.size();
	if (knight.pos + diceValue > boardSize + game.board->knightCount) return false;

	for (auto otherKnight:game.board->knights) {
		if (knight.pos + diceValue == otherKnight->pos && &knight.region.player == &otherKnight->region.player)
			return false; // no same player knight overlap allowed
	}

	return true;
}

bool ClassicGamePhysics::canKnightStackMove(Knight &knight, unsigned diceValue) {
	if (!canKnightMove(knight, diceValue))return false;
	const auto boardSize = game.board->regionSize * game.players.size();
	if (knight.pos + diceValue <= boardSize) return false;
	return true;
}

bool ClassicGamePhysics::canKnightAttackMove(Knight &knight, Knight &otherKnight, unsigned diceValue) {
	if (!canKnightMove(knight, diceValue))return false;
	if (knight.pos + diceValue == otherKnight.pos && &knight.region.player != &otherKnight.region.player)
		return true;
	return false;
}

bool ClassicGamePhysics::isStack(Position pos) {
	const auto boardSize = game.board->regionSize * game.players.size();
	return pos > boardSize && pos <= (boardSize + game.board->knightCount);
}


bool onPhysicsEvent_KnightEntered(Game &game, Knight &knight);

bool onPhysicsEvent_KnightMoved(Game &game, Knight &knight, Position toPos);

bool onPhysicsEvent_KnightStacked(Game &game, Knight &knight, Position toPos);

bool onPhysicsEvent_KnightAttacked(Game &game, Knight &knight, Knight &otherKnight);

bool onPhysicsEvent_PlayerWin(Game &game, Player &player);

bool ClassicGamePhysics::onEvent(GameEvent event, void *caller, void *eventData) {
	if (event == KnightEntered)
		return onPhysicsEvent_KnightEntered(game, *(Knight *) caller);
	if (event == KnightMoved)
		return onPhysicsEvent_KnightMoved(game, *(Knight *) caller, *(Position *) eventData);
	if (event == KnightStacked)
		return onPhysicsEvent_KnightStacked(game, *(Knight *) caller, *(Position *) eventData);
	if (event == KnightAttacked)
		return onPhysicsEvent_KnightAttacked(game, *(Knight *) caller, *(Knight *) eventData);
	if (event == PlayerWin)
		return onPhysicsEvent_PlayerWin(game, *(Player *) eventData);
	return false;
}

bool onPhysicsEvent_KnightEntered(Game &game, Knight &knight) {
	std::cout << knight << " ENTERed" << std::endl;
	return true;
}

bool onPhysicsEvent_KnightMoved(Game &game, Knight &knight, Position toPos) {
	std::cout << knight << " Moved to " << toPos << std::endl;
	return true;
}

bool onPhysicsEvent_KnightStacked(Game &game, Knight &knight, Position toPos) {
	std::cout << knight << " Stacked at " << toPos << std::endl;
	bool all_stacked = true;
	for (auto &otherKnight:game.board->knights) {
		if (otherKnight->region.player == knight.region.player) {
			if (!game.physics->isStack(otherKnight->pos)) {
				all_stacked = false;
				break;
			}
		}
	}
	if (all_stacked) {
		game.triggerEvent(PlayerWin, nullptr, &knight.region.player);
	}
	return true;
}

bool onPhysicsEvent_KnightAttacked(Game &game, Knight &knight, Knight &otherKnight) {
	std::cout << knight << " Killed " << otherKnight << std::endl;
	otherKnight.pos = parkPos;
	return true;
}

bool onPhysicsEvent_PlayerWin(Game &game, Player &player) {
	std::cout << player << " Wins " << std::endl;
	game.board->finished = true;
	return true;
}

#pragma endregion Physics


#pragma region AI

GameAI::GameAI(Game &game) : GameObserver(game) {
	std::cout << "GameAI::GameAI" "\n";
}

ClassicGameAI::ClassicGameAI(Game &game) : GameAI(game) {
	std::cout << "ClassicGameAI::ClassicGameAI" "\n";
}

bool onAiEvent_PlayerTurn(Game &game, Player &player);

bool onAiEvent_DiceRolled(Game &game, Player &player, unsigned diceValue);

bool ClassicGameAI::onEvent(GameEvent event, void *caller, void *eventData) {
	if (event == PlayerTurn) {
		Player &player = *(Player *) eventData;
		if (player.ai < 0)return false; // not ai player
		return onAiEvent_PlayerTurn(game, player);
	}

	if (event == DiceRolled) {
		Player &player = *(Player *) caller;
		if (player.ai < 0)return false; // not ai player
		return onAiEvent_DiceRolled(game, player, *(unsigned *) eventData);
	}

	return false;
}

bool onAiEvent_PlayerTurn(Game &game, Player &player) {
	std::cout << player << " TURN" << std::endl;
	game.physics->queue(new RollDiceCommand(player), nullptr);
	return true;
}

bool onAiEvent_DiceRolled(Game &game, Player &player, unsigned diceValue) {
	std::cout << player << " Diced: " << diceValue << std::endl;
	bool done = false;

	Knight *enteredKnight = game.physics->canPlayerAddKnight(player, diceValue);
	if (enteredKnight != nullptr) {
		std::cout << player << " enters new knight" << std::endl;
		game.physics->queue(new EnterKnightCommand(*enteredKnight), nullptr);
		done = true;
	}

	// try to stack
	if (!done) {
		// check possible movements
		for (auto knight:game.board->knights) {
			if (knight->region.player != player) continue; // not player's knight
			if (knight->pos == parkPos) continue; // knight is not in play
			Dice::wait(10, 25);
			if (game.physics->canKnightStackMove(*knight, knight->pos + diceValue)) {
				Dice::wait(25, 50);
				game.physics->queue(new MoveKnightCommand(*knight, diceValue), nullptr);
				done = true;
				break;
			}
		}
	}

	if (!done) {
		// check possible attacks
		for (auto knight:game.board->knights) {
			if (knight->region.player != player) continue; // not player's knight
			if (knight->pos == parkPos) continue; // knight is not in play
			for (auto otherKnight:game.board->knights) {
				if (otherKnight->region.player == player) continue; // not player's otherKnight
				if (otherKnight->pos == parkPos) continue; // otherKnight is not in play
				Dice::wait(10, 25);
				if (game.physics->canKnightAttackMove(*knight, *otherKnight, diceValue)) {
					Dice::wait(25, 50);
					game.physics->queue(new MoveKnightCommand(*knight, diceValue), nullptr);
					done = true;
				}
			}
		}
	}

	if (!done) {
		// check possible movements
		for (auto knight:game.board->knights) {
			if (knight->region.player != player) continue; // not player's knight
			if (knight->pos == parkPos) continue; // knight is not in play
			Dice::wait(10, 25);
			if (game.physics->canKnightMove(*knight, diceValue)) {
				Dice::wait(25, 50);
				game.physics->queue(new MoveKnightCommand(*knight, diceValue), nullptr);
				done = true;
				break;
			}
		}
	}

	if (!done) {
		std::cout << player << " STUCK " << std::endl;
	}

	if (diceValue == game.physics->luckyValue) {
		Dice::wait(100, 250);
		std::cout << player << " lucky turn" << std::endl;
		game.physics->queue(new RollDiceCommand(player), nullptr);
	}

	return true;
}

#pragma endregion AI


#pragma region GameCommands

bool RollDiceCommand::redo(Game &game) {
	auto value = Dice::roll();
	game.triggerEvent(DiceRolled, &player, &value);
	return true;
}

bool EnterKnightCommand::redo(Game &game) {
	knight.pos = homePos; // go to home
	game.triggerEvent(KnightEntered, &knight, nullptr);
	return true;
}

bool MoveKnightCommand::redo(Game &game) {
	knight.pos = to;
	game.triggerEvent(KnightMoved, &knight, &to);

	const auto boardSize = game.board->regionSize * game.players.size();
	if (knight.pos > boardSize) {
		game.triggerEvent(KnightStacked, &knight, &to);
		return true;
	}

	for (auto otherKnight:game.board->knights) {
		if (knight.region.player != otherKnight->region.player && otherKnight->pos == to) {
			game.triggerEvent(KnightAttacked, &knight, otherKnight);
			return true;
		}
	}

	return true;
}

#pragma endregion GameCommands


#pragma region Util

bool operator==(const Player &lhs, const Player &rhs) {
	return lhs.id == rhs.id;
}

bool operator!=(const Player &lhs, const Player &rhs) {
	return !(lhs == rhs);
}

std::ostream &operator<<(std::ostream &os, const Region &region) {
	return os; // << TERM_CYAN << "Ladder[" << ladder.start << ":" << ladder.end << "]" << TERM_RESET;
}

std::ostream &operator<<(std::ostream &os, const Board &board) {
	return os; // << TERM_MAGENTA << "Snake[" << snake.head << ":" << snake.tail << "]" << TERM_RESET;
}

std::ostream &operator<<(std::ostream &os, const Player &player) {
	return TERM_COLOR(os, ColorFores[player.color], player.title);
	// return term::color(os, ColorFores[player.color]) << player.title << TERM_RESET;
}

std::ostream &operator<<(std::ostream &os, const Knight &knight) {
	return os << knight.region.player << " Knight@" << knight.pos;
}

#pragma endregion Util
