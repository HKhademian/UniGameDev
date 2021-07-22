#ifndef GD_HW_COLORS_HPP
#define GD_HW_COLORS_HPP

#include "Terminal.hpp"

/// each players has unique color across multiple games
enum Color {
	Red = 0, Green = 1, Blue = 2, Yellow = 3, White = 4
};

const Color Colors[] = {Red, Green, Blue, Yellow, White};
const char *ColorNames[] = {"Red", "Green", "Blue", "Yellow", "White"};
const int ColorFores[] = {31, 32, 34, 33, 37};
const char *ColorBacks[] = {TERM_BLACK, TERM_BLACK, TERM_BLACK, TERM_BLACK, TERM_BLACK};


#endif
