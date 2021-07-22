#pragma once

#include <random>
#include <functional>
#include <thread>

#ifndef SPEED
#define SPEED 10
#endif


class Dice;

Dice *dice_instance = nullptr;

class Dice {
public:
	static Dice &instance() {
		if (dice_instance == nullptr) {
			dice_instance = new Dice;
		}
		return *dice_instance;
	}

	static void wait(unsigned int milis) {
#if SPEED > 0
		std::this_thread::sleep_for(std::chrono::milliseconds(milis / SPEED));
#endif
	}

	static void wait(unsigned int minTime, unsigned int maxTime) {
		wait(range(minTime, maxTime));
	}

	static unsigned int range(unsigned int from, unsigned int to) {
		return instance().rand(from, to);
	}

	static unsigned int roll() {
		return instance().roll_dice();
	}

private:
	Dice() {
		generator = new std::default_random_engine(time(nullptr));
		distribution = new std::uniform_int_distribution<unsigned int>(1, 6);
	}

	unsigned int roll_dice() {
		return (*distribution)(*generator);
	}

	unsigned int rand(unsigned int from, unsigned int to) {
		std::uniform_int_distribution<unsigned int> dist(from, to);
		return dist(*generator);
	}

	std::default_random_engine *generator;
	std::uniform_int_distribution<unsigned int> *distribution;
};
