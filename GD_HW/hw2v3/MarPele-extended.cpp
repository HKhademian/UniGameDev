#include "MarPele.hpp"

class ExtendedPhysics : public ClassicGamePhysics {
public:
	explicit ExtendedPhysics(Game *game) : ClassicGamePhysics(game) {}

private:
	bool onEvent(GameEvent event, void *caller, void *eventData) override;

	bool onEvent_KnightMoves(Knight *knight, Position toPos) override;

	bool onEvent_FightDiceRolled(void *fighter, unsigned int diceValue);

private:
	void *fighter1 = nullptr, *fighter2 = nullptr;
	unsigned dice1 = 0, dice2 = 0;
};

class ExtendedAI : public AI {
public:
	explicit ExtendedAI(Game *game) : AI(game) {}
};

int main() {
	auto game = new Game(nullptr, nullptr);
	game->ai = new ExtendedAI(game);
	game->physics = new ExtendedPhysics(game);
	bench(game);
	delete game;
	return 0;
}

class FightRollDiceCommand : public Command {
public:
	explicit FightRollDiceCommand(ExtendedPhysics &physics, void *fighter)
			: fighter(fighter) {}

private:
	bool redo(Game *game) override;

	void *fighter;
};

bool ExtendedPhysics::onEvent(GameEvent event, void *caller, void *eventData) {
	if (event == FightDiceRolled)
		return onEvent_FightDiceRolled(caller, *(unsigned *) eventData);
	return ClassicGamePhysics::onEvent(event, caller, eventData);
}

bool ExtendedPhysics::onEvent_KnightMoves(Knight *knight, Position toPos) {
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
			fighter1 = knight, fighter2 = otherKnight, dice1 = 0, dice2 = 0;
			game->commandQueue.push(new FightRollDiceCommand(*this, knight));
			game->commandQueue.push(new FightRollDiceCommand(*this, otherKnight));
			return true;
		}
	}

	return true;
}

bool ExtendedPhysics::onEvent_FightDiceRolled(void *fighter, unsigned int diceValue) {
	bool capture = false;
	if (fighter == fighter1) {
		capture = true;
		dice1 = diceValue;
#ifndef BENCH
		std::cout << *(Knight *) fighter1 << " Diced to Attack " << *(Knight *) fighter2 << " : " << dice1 << std::endl;
#endif
	}
	if (fighter == fighter2) {
		capture = true;
		dice2 = diceValue;
#ifndef BENCH
		std::cout << *(Knight *) fighter2 << " Diced to Defend " << *(Knight *) fighter1 << " : " << dice2 << std::endl;
#endif
	}

	if (dice1 != 0 && dice2 != 0) {
		unsigned d1 = dice1, d2 = dice2;
		void *f1 = fighter1, *f2 = fighter2;
		fighter1 = nullptr, fighter2 = nullptr, dice1 = 0, dice2 = 0; // clear fight data
		if (d1 >= d2) {
			game->triggerEvent(KnightKill, f1, f2);
		} else {
			game->triggerEvent(KnightKill, f2, f1);
		}
	}

	return capture;
}

bool FightRollDiceCommand::redo(Game *game) {
	auto value = Dice::roll();
	game->triggerEvent(FightDiceRolled, fighter, &value);
	return true;
}
