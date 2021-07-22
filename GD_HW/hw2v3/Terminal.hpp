#ifndef GD_HW_TERMINAL
#define GD_HW_TERMINAL

// https://stackoverflow.com/a/9158263/1803735
// https://stackoverflow.com/a/51850508/1803735

#include <ostream>

#define TERM                "\033["
#define TERM_RESET          TERM "0m"
#define TERM_BLACK          TERM "30m"            /* Black */
#define TERM_RED            TERM "31m"            /* Red */
#define TERM_GREEN          TERM "32m"            /* Green */
#define TERM_YELLOW        TERM "33m"            /* Yellow */
#define TERM_BLUE          TERM "34m"            /* Blue */
#define TERM_MAGENTA        TERM "35m"            /* Magenta */
#define TERM_CYAN          TERM "36m"            /* Cyan */
#define TERM_WHITE          TERM "37m"            /* White */
#define TERM_BOLD          TERM "1m" TERM        /* Bold */
#define TERM_BOLD_BLACK    TERM_BOLD "30m"     /* Bold Black */
#define TERM_BOLD_RED      TERM_BOLD "31m"     /* Bold Red */
#define TERM_BOLD_GREEN    TERM_BOLD "32m"     /* Bold Green */
#define TERM_BOLD_YELLOW    TERM_BOLD "33m"     /* Bold Yellow */
#define TERM_BOLD_BLUE      TERM_BOLD "34m"     /* Bold Blue */
#define TERM_BOLD_MAGENTA  TERM_BOLD "35m"     /* Bold Magenta */
#define TERM_BOLD_CYAN      TERM_BOLD "36m"     /* Bold Cyan */
#define TERM_BOLD_WHITE    TERM_BOLD "37m"     /* Bold White */

#define TERM_COLOR(os, color, ...) os << TERM << (int)color << "m"<<__VA_ARGS__<<TERM_RESET

namespace term {
	template<class CharT, class Traits>
	constexpr
	std::basic_ostream<CharT, Traits> &color(std::basic_ostream<CharT, Traits> &os, int color) {
		return os << TERM << color << "m";
	}

	template<class CharT, class Traits>
	constexpr
	std::basic_ostream<CharT, Traits> &reset(std::basic_ostream<CharT, Traits> &os) {
		return os << TERM_RESET;
	}

	template<class CharT, class Traits>
	constexpr
	std::basic_ostream<CharT, Traits> &fblack(std::basic_ostream<CharT, Traits> &os) {
		return os << TERM_BLACK;
	}

	template<class CharT, class Traits>
	constexpr
	std::basic_ostream<CharT, Traits> &fred(std::basic_ostream<CharT, Traits> &os) {
		return os << TERM_RED;
	}

	template<class CharT, class Traits>
	constexpr
	std::basic_ostream<CharT, Traits> &fgreen(std::basic_ostream<CharT, Traits> &os) {
		return os << TERM_GREEN;
	}

	template<class CharT, class Traits>
	constexpr
	std::basic_ostream<CharT, Traits> &fblue(std::basic_ostream<CharT, Traits> &os) {
		return os << TERM_BLUE;
	}

	template<class CharT, class Traits>
	constexpr
	std::basic_ostream<CharT, Traits> &color(std::basic_ostream<CharT, Traits> &os) {
		return os << TERM_BLUE;
	}

}


#endif
