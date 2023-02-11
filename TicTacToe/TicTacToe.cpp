#include <iostream>
#include <vector>
#include <random>
#include <string>
#include <chrono>
#include <thread>
#include <map>

using namespace std;

const int BOARD_DIMENSION = 3;
const int CLS_DELAY_MS = 1500;
enum States {LOST = -1, TIE, WON};

class TicTacToe {
public:
	TicTacToe() : gameBoard(BOARD_DIMENSION, vector<string>(BOARD_DIMENSION, " ")), gameResult(TIE) {}

	void playGame() {
#ifdef _WIN32
		system("cls");
#else
		system("clear");
#endif
		printBoard();

		do {
			string position;

			if (isHumanTurn) {
				cout << endl << endl << "Place an " << humanSymbol << " to: ";
				cin >> position;

				if (!isValidPosition(position)) {
					cout << "Invalid position!" << endl;
					this_thread::sleep_for(std::chrono::milliseconds(CLS_DELAY_MS));
					continue;
				}
			}
			else if (isFirstMove) {
				position = getRandomAIPosition();
				isFirstMove = false;
			}
			else {
				position = getAIMove();
			}

			makeMove(position, (isHumanTurn ? humanSymbol : aiSymbol));
			isHumanTurn = !isHumanTurn;

#ifdef _WIN32
			system("cls");
#else
			system("clear");
#endif
			printBoard();
		} while (!isGameOver());

		switch (gameResult) {
		case LOST:
			cout << endl << "You lost!" << endl;
			break;
		case TIE:
			cout << endl << "Tie!" << endl;
			break;
		case WON:
			cout << endl << "You won!" << endl;
			break;
		default:
			break;
		}
	}

	bool isHumanTurn;
	bool isFirstMove;
	string humanSymbol = "O";
	string aiSymbol = "X";

private:
	vector<vector<string>> gameBoard;
	map<string, int> memo;
	States gameResult;

	void printBoard() const {
		cout << "   1   2   3" << endl;
		for (int i = 0; i < BOARD_DIMENSION; i++) {
			cout << " " << i + 1;
			for (int j = 0; j < BOARD_DIMENSION; j++)
				cout << " " << gameBoard[i][j] << (j != BOARD_DIMENSION - 1 ? " |" : "");
			cout << endl << (i != BOARD_DIMENSION - 1 ? "   -----------" : "") << endl;
		}
	}
	bool isGameOver() {
		if (gameBoard[1][1] != " " && (
			(gameBoard[0][0] == gameBoard[1][1] && gameBoard[1][1] == gameBoard[2][2]) ||
			(gameBoard[0][2] == gameBoard[1][1] && gameBoard[1][1] == gameBoard[2][0])
			)) {
			gameResult = (gameBoard[1][1] == humanSymbol ? WON : (gameBoard[1][1] == aiSymbol ? LOST : TIE));
			return true;
		}

		for (int i = 0; i < BOARD_DIMENSION; i++) {
			if (gameBoard[i][0] != " " && gameBoard[i][0] == gameBoard[i][1] && gameBoard[i][0] == gameBoard[i][2]) {
				gameResult = (gameBoard[i][0] == humanSymbol ? WON : (gameBoard[i][0] == aiSymbol ? LOST : TIE));
				return true;
			}

			if (gameBoard[0][i] != " " && gameBoard[0][i] == gameBoard[1][i] && gameBoard[0][i] == gameBoard[2][i]) {
				gameResult = (gameBoard[0][i] == humanSymbol ? WON : (gameBoard[0][i] == aiSymbol ? LOST : TIE));
				return true;
			}
		}

		for (int i = 0; i < BOARD_DIMENSION; i++)
			for (int j = 0; j < BOARD_DIMENSION; j++)
				if (gameBoard[i][j] == " ")
					return false;

		gameResult = TIE;
		return true;
	}
	bool isValidPosition(const string& pos) {
		if (pos.length() != 2)
			return false;
		if (pos[0] < '1' || pos[0] > '3' || pos[1] < '1' || pos[1] > '3')
			return false;
		if (gameBoard[pos[0] - '1'][pos[1] - '1'] != " ")
			return false;
		return true;
	}
	void makeMove(const string& pos, const string& player) {
		gameBoard[pos[0] - '1'][pos[1] - '1'] = player;
	}
	string intPos2String(int i, int j) {
		return to_string(i + 1) + to_string(j + 1);
	}
	string getBoardSignature() {
		string sign;
		for (int i = 0; i < BOARD_DIMENSION; i++)
			for (int j = 0; j < BOARD_DIMENSION; j++)
				sign += gameBoard[i][j];

		return sign;
	}
	string getRandomAIPosition() {
		random_device rd;
		mt19937 mt(rd());
		uniform_int_distribution<int> dist(0, 2);
		int i, j;
		do {
			i = dist(mt);
			j = dist(mt);
		} while (gameBoard[i][j] != " ");
		return intPos2String(i, j);
	}
	string getAIMove() {
		int bestVal = INT_MIN;
		string bestMove = "";

		for (int i = 0; i < BOARD_DIMENSION; i++) {
			for (int j = 0; j < BOARD_DIMENSION; j++) {
				if (gameBoard[i][j] == " ") {
					gameBoard[i][j] = aiSymbol;
					int moveVal = minimax(0, false);
					gameBoard[i][j] = " ";

					if (moveVal > bestVal) {
						bestMove = intPos2String(i, j);
						bestVal = moveVal;
					}
				}
			}
		}

		return bestMove;
	}
	int minimax(int depth, bool isMax) {
		string key = getBoardSignature();
		if (memo.count(key))
			return memo[key];

		if (isGameOver())
			return (gameResult == WON ? -10 + depth : (gameResult == LOST ? 10 - depth : 0));

		int best;
		if (isMax) {
			best = INT_MIN;
			for (int i = 0; i < BOARD_DIMENSION; i++) {
				for (int j = 0; j < BOARD_DIMENSION; j++) {
					if (gameBoard[i][j] == " ") {
						gameBoard[i][j] = aiSymbol;
						best = max(best, minimax(depth + 1, !isMax));
						gameBoard[i][j] = " ";
					}
				}
			}
		}
		else {
			best = INT_MAX;
			for (int i = 0; i < BOARD_DIMENSION; i++) {
				for (int j = 0; j < BOARD_DIMENSION; j++) {
					if (gameBoard[i][j] == " ") {
						gameBoard[i][j] = humanSymbol;
						best = min(best, minimax(depth + 1, !isMax));
						gameBoard[i][j] = " ";
					}
				}
			}
		}

		memo[key] = best;
		return best;
	}
};

int main() {
#ifdef _WIN32
	system("cls");
#else
	system("clear");
#endif
	TicTacToe game;

	string start;
	cout << "Who starts: ";
	cin >> start;
	game.isHumanTurn = start != game.aiSymbol;
	game.isFirstMove = !game.isHumanTurn;

	game.playGame();

	string res;
	cout << endl << "Play again (y/n): ";
	cin >> res;
	if (res == "y")
		main();

	return 0;
}
