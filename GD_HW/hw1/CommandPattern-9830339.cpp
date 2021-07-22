#include <iostream>
#include <vector>

char getChar() {
    // TODO: use linux compatible no buffer getch
    return std::getchar();
}

namespace CommandPattern {
#define SIZE  10

#pragma region Actor

    struct Tile {
        char type;

        Tile() : Tile(' ') {}

        explicit Tile(char t) : type(t) {}

        Tile(Tile const &t) = default;

        virtual void draw() {
            std::cout << type;
        }
    };

    struct WallTile : public Tile {
        WallTile() : Tile('X') {}
    };

    struct BoxTile : public Tile {
        BoxTile() : Tile('*') {}
    };

    struct HoleTile : public Tile {
        HoleTile() : Tile('.') {}
    };

#pragma endregion

#pragma region Command

    struct Command {
        char type;

        Command() : Command(' ') {}

        explicit Command(char t) : type(t) {}

        Command(Command const &t) = default;
    };

    struct NoCommand : public Command {
        NoCommand() : Command(' ') {}
    };

    struct MoveUpCommand : public Command {
        MoveUpCommand() : Command('u') {}
    };

    struct MoveDownCommand : public Command {
        MoveDownCommand() : Command('d') {}
    };

    struct MoveLeftCommand : public Command {
        MoveLeftCommand() : Command('l') {}
    };

    struct MoveRightCommand : public Command {
        MoveRightCommand() : Command('r') {}
    };

#pragma endregion

#pragma region InputHandler

    struct InputHandler {
        Command get() {
            switch (getChar()) {
                case 'W':
                case 'w':
                    return MoveUpCommand();
                case 'S':
                case 's':
                    return MoveDownCommand();
                case 'A':
                case 'a':
                    return MoveLeftCommand();
                case 'D':
                case 'd':
                    return MoveRightCommand();

                default:
                    return NoCommand();
            }
        }
    };

#pragma endregion

#pragma region AI

    struct AI_Engine {
        bool addCommand(Command &command, int &pX, int &pY, std::vector<std::vector<Tile>> &map) {
            switch (command.type) {
                case 'u':
                    if (pY <= 0 || map[pX][pY - 1].type == '#') break;
                    if (map[pX][pY - 1].type == '*') {
                        if (pY <= 1 || map[pX][pY - 2].type == '#') break;
                        if (map[pX][pY - 2].type == '.') return false;
                        map[pX][pY - 2] = map[pX][pY - 1];
                    }
                    pY--;
                    break;
                case 'd':
                    if (pY >= SIZE - 1 || map[pX][pY + 1].type == '#') break;
                    if (map[pX][pY + 1].type == '*') {
                        if (pY >= SIZE - 2 || map[pX][pY + 2].type == '#') break;
                        if (map[pX][pY + 2].type == '.') return false;
                        map[pX][pY + 2] = map[pX][pY + 1];
                    }
                    pY++;
                    break;
                case 'l':
                    if (pX <= 0 || map[pX - 1][pY].type == '#') break;
                    if (map[pX - 1][pY].type == '*') {
                        if (pX <= 1 || map[pX - 2][pY].type == '#') break;
                        if (map[pX - 2][pY].type == '.') return false;
                        map[pX - 2][pY] = map[pX - 1][pY];
                    }
                    pX--;
                    break;
                case 'r':
                    if (pX >= SIZE - 1 || map[pX + 1][pY].type == '#') break;
                    if (map[pX + 1][pY].type == '*') {
                        if (pX >= SIZE - 2 || map[pX + 2][pY].type == '#') break;
                        if (map[pX + 2][pY].type == '.') return false;
                        map[pX + 2][pY] = map[pX + 1][pY];
                    }
                    pY++;
                    break;
            }
            return true;
        }
    };

#pragma endregion

#pragma region Game

    struct Game {
        std::vector<std::vector<Tile>> map;
        InputHandler inputHandler;
        AI_Engine ai;
        int pX = 0, pY = 0;

        void generateMap() {
            this->map = std::vector<std::vector<Tile>>(SIZE, std::vector<Tile>(SIZE, Tile()));
            for (auto x = 0; x < SIZE; x++) {
                for (auto y = 0; y < SIZE; y++) {
                    if ((x == 0) || (y == 0) || (x == 9) || (y == 9)) map[x][y] = WallTile();
                }
            }
            pX = 5, pY = 5; // players is in 5,5
            map[3][3] = BoxTile(); // 1 ball in 3,3
            map[7][7] = HoleTile(); // 1 goal in 7,7
        }

        void draw() {
            system("clear");
            for (auto x = 0; x < SIZE; x++) {
                for (auto y = 0; y < SIZE; y++) {
                    if (x == pX && y == pY) std::cout << '+';
                    else map[x][y].draw();
                }
                std::cout << std::endl;
            }
        }

        void mainLoop() {
            while (true) {
                draw();
                auto cmd = inputHandler.get();
                auto res = ai.addCommand(cmd, pX, pY, map);
                if (!res) break;
            }
        }
    };

#pragma endregion
}

int main() {
    auto game = CommandPattern::Game();
    while (true) {
        game.generateMap();

        std::cout << "You can move with W/S/A/D keys\nto start press any keys ..." << std::endl;
        getChar();

        game.mainLoop();

        std::cout << "Do you want to play (y/N)?";
        if (getChar() != 'y')break;
    }
    return 0;
}

