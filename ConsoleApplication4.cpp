#include <iostream>
#include <vector>
#include <random>
#include <limits>

class Board {
public:
    char board[3][3] = { {' ', ' ', ' '}, {' ', ' ', ' '}, {' ', ' ', ' '} };

    bool isFull() const {
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                if (board[i][j] == ' ') {
                    return false;
                }
            }
        }
        return true;
    }

    bool checkWin(char symbol) const {
        for (int i = 0; i < 3; i++) {
            if (board[i][0] == symbol && board[i][1] == symbol && board[i][2] == symbol) return true;
            if (board[0][i] == symbol && board[1][i] == symbol && board[2][i] == symbol) return true;
        }
        if (board[0][0] == symbol && board[1][1] == symbol && board[2][2] == symbol) return true;
        if (board[0][2] == symbol && board[1][1] == symbol && board[2][0] == symbol) return true;
        return false;
    }

    void printBoard() const {
        std::cout << "   0 1 2\n";
        for (int i = 0; i < 3; i++) {
            std::cout << i << " |";
            for (int j = 0; j < 3; j++) {
                std::cout << board[i][j] << "|";
            }
            std::cout << std::endl;
        }
    }
};

class Player {
public:
    char symbol;

    Player(char s) : symbol(s) {}

    virtual int makeMove(const Board& board) = 0;
};

class HumanPlayer : public Player {
public:
    HumanPlayer(char s) : Player(s) {}

    int makeMove(const Board& board) override {
        int row, col;
        do {
            std::cout << "Nhap nuoc di (hang cot): ";
            std::cin >> row >> col;
        } while (row < 0 || row > 2 || col < 0 || col > 2 || board.board[row][col] != ' ');
        return row * 3 + col;
    }
};

class Robot : public Player {
public:
    enum Difficulty { EASY, MEDIUM, HARD };
    Difficulty difficulty;
    std::mt19937 rng;

    Robot(char s, Difficulty d) : Player(s), difficulty(d), rng(std::random_device{}()) {}

    int makeMove(const Board& board) override {
        if (difficulty == EASY) {
            std::uniform_int_distribution<int> dist(0, 8);
            int move;
            do {
                move = dist(rng);
            } while (board.board[move / 3][move % 3] != ' ');
            return move;
        }
        else { 
            int bestScore = -std::numeric_limits<int>::max();
            int bestMove = -1;
            for (int i = 0; i < 9; i++) {
                if (board.board[i / 3][i % 3] == ' ') {
                    Board newBoard = board;
                    newBoard.board[i / 3][i % 3] = symbol;
                    int score = minimax(newBoard, 0, false);
                    if (score > bestScore) {
                        bestScore = score;
                        bestMove = i;
                    }
                }
            }
            return bestMove;
        }
    }

private:
    int minimax(Board board, int depth, bool maximizingPlayer) {
        if (board.checkWin(symbol)) return 10 - depth;
        if (board.checkWin(symbol == 'X' ? 'O' : 'X')) return depth - 10;
        if (board.isFull()) return 0;

        if (maximizingPlayer) {
            int maxEval = -std::numeric_limits<int>::max();
            for (int i = 0; i < 9; i++) {
                if (board.board[i / 3][i % 3] == ' ') {
                    board.board[i / 3][i % 3] = symbol;
                    maxEval = std::max(maxEval, minimax(board, depth + 1, false));
                    board.board[i / 3][i % 3] = ' '; 
                }
            }
            return maxEval;
        }
        else {
            int minEval = std::numeric_limits<int>::max();
            for (int i = 0; i < 9; i++) {
                if (board.board[i / 3][i % 3] == ' ') {
                    board.board[i / 3][i % 3] = symbol == 'X' ? 'O' : 'X';
                    minEval = std::min(minEval, minimax(board, depth + 1, true));
                    board.board[i / 3][i % 3] = ' '; 
                }
            }
            return minEval;
        }
    }
};

class Game {
public:
    Board board;
    Player* player1;
    Player* player2;

    Game() {
        int choice;
        std::cout << "Chon che do choi:\n";
        std::cout << "1. Nguoi vs Nguoi\n";
        std::cout << "2. Nguoi vs May\n";
        std::cin >> choice;

        if (choice == 1) {
            player1 = new HumanPlayer('X');
            player2 = new HumanPlayer('O');
        }
        else {
            player1 = new HumanPlayer('X');
            std::cout << "Chọn độ khó cho máy:\n";
            std::cout << "1. De\n";
            std::cout << "2. Trung binh\n";
            std::cout << "3. Kho\n";
            std::cin >> choice;
            Robot::Difficulty difficulty = Robot::EASY;
            if (choice == 2) difficulty = Robot::MEDIUM;
            else if (choice == 3) difficulty = Robot::HARD;
            player2 = new Robot('O', difficulty);
        }
    }

    ~Game() {
        delete player1;
        delete player2;
    }

    void play() {
        int currentPlayer = 0;
        while (!board.isFull() && !board.checkWin('X') && !board.checkWin('O')) {
            board.printBoard();
            int move = (currentPlayer == 0 ? player1 : player2)->makeMove(board);
            board.board[move / 3][move % 3] = (currentPlayer == 0 ? 'X' : 'O');
            currentPlayer = 1 - currentPlayer;
        }

        board.printBoard();
        if (board.checkWin('X')) {  
            std::cout << "Người chơi X thắng!\n";
        }
        else if (board.checkWin('O')) {
            std::cout << "Người chơi O thắng!\n";
        }
        else {
            std::cout << "Hòa!\n";
        }
    }
};

int main() {
    Game game;
    game.play();
    return 0;
}
