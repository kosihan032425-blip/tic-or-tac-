/*
 * ==============================================================
 *              TIC-TAC-TOE  -  C++
 *     2-Player  |  vs Minimax AI  |  Score Tracker
 *     Windows-compatible (MinGW, no C++17 required)
 * ==============================================================
 */

#include <iostream>
#include <array>
#include <string>
#include <limits>
#include <algorithm>
#include <cstdlib>
#include <climits>
#include <windows.h>

using namespace std;

// ==============================================================
//  UTILITIES
// ==============================================================
namespace UI {

void clear() { system("cls"); }

void sleep_ms(int ms) { Sleep(ms); }   // Windows native

void pause() {
    cout << "\n  Press Enter to continue...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
}

void border(char c = '=', int w = 46) {
    cout << "  " << string(w, c) << "\n";
}

void title(const string& t) {
    border('=');
    int pad  = max(0, (42 - (int)t.size()) / 2);
    int rpad = max(0, 42 - pad - (int)t.size());
    cout << "  |" << string(pad,' ') << t << string(rpad,' ') << "|\n";
    border('=');
}

int getInt(const string& prompt, int lo, int hi) {
    int v;
    while (true) {
        cout << prompt;
        if (cin >> v && v >= lo && v <= hi) { cin.ignore(); return v; }
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "  [!!] Enter a number between " << lo << " and " << hi << ".\n";
    }
}

} // namespace UI


// ==============================================================
//  TIC-TAC-TOE CLASS
// ==============================================================
class TicTacToe {
private:

    array<char, 9> board;

    char   current;
    bool   vsAI;
    string p1name, p2name;
    int    p1score, p2score, draws;

    static const int WINS[8][3];

    // ==========================================================
    //  BOARD RENDERING
    // ==========================================================
    void drawBoard() const {
        UI::clear();
        UI::title("  TIC-TAC-TOE  ");

        cout << "\n"
             << "  " << p1name << " [X]: " << p1score
             << "   Draws: " << draws
             << "   " << p2name << " [O]: " << p2score
             << "\n\n";

        for (int r = 0; r < 3; ++r) {
            cout << "        ";
            for (int c = 0; c < 3; ++c) {
                char cell = board[r * 3 + c];
                if      (cell == 'X') cout << " X ";
                else if (cell == 'O') cout << " O ";
                else                  cout << " " << cell << " ";
                if (c < 2) cout << "|";
            }
            cout << "\n";
            if (r < 2) cout << "       ---+---+---\n";
        }

        cout << "\n"
             << "  Cell layout:\n"
             << "     1 | 2 | 3\n"
             << "    ---+---+---\n"
             << "     4 | 5 | 6\n"
             << "    ---+---+---\n"
             << "     7 | 8 | 9\n\n";
    }

    // ==========================================================
    //  GAME STATE CHECKS
    // ==========================================================
    char checkWinner() const {
        for (int i = 0; i < 8; ++i) {
            int a = WINS[i][0], b = WINS[i][1], c = WINS[i][2];
            if (board[a] == board[b] && board[b] == board[c])
                return board[a];
        }
        return ' ';
    }

    bool isBoardFull() const {
        for (int i = 0; i < 9; ++i)
            if (board[i] != 'X' && board[i] != 'O') return false;
        return true;
    }

    bool isCell(char c) const { return c != 'X' && c != 'O'; }

    // ==========================================================
    //  MINIMAX AI
    // ==========================================================
    int minimax(array<char, 9> b, bool maximising, int depth) {
        for (int i = 0; i < 8; ++i) {
            int a = WINS[i][0], w = WINS[i][1], c = WINS[i][2];
            if (b[a] == b[w] && b[w] == b[c])
                return b[a] == 'O' ? (10 - depth) : (depth - 10);
        }
        bool full = true;
        for (int i = 0; i < 9; ++i)
            if (b[i] != 'X' && b[i] != 'O') { full = false; break; }
        if (full) return 0;

        if (maximising) {
            int best = INT_MIN;
            for (int i = 0; i < 9; ++i) {
                if (b[i] != 'X' && b[i] != 'O') {
                    char tmp = b[i]; b[i] = 'O';
                    int val = minimax(b, false, depth + 1);
                    if (val > best) best = val;
                    b[i] = tmp;
                }
            }
            return best;
        } else {
            int best = INT_MAX;
            for (int i = 0; i < 9; ++i) {
                if (b[i] != 'X' && b[i] != 'O') {
                    char tmp = b[i]; b[i] = 'X';
                    int val = minimax(b, true, depth + 1);
                    if (val < best) best = val;
                    b[i] = tmp;
                }
            }
            return best;
        }
    }

    int bestAIMove() {
        int bestVal = INT_MIN, bestIdx = -1;
        for (int i = 0; i < 9; ++i) {
            if (board[i] != 'X' && board[i] != 'O') {
                char tmp = board[i]; board[i] = 'O';
                int val = minimax(board, false, 0);
                board[i] = tmp;
                if (val > bestVal) { bestVal = val; bestIdx = i; }
            }
        }
        return bestIdx;
    }

    // ==========================================================
    //  TURN HANDLERS
    // ==========================================================
    void humanTurn(const string& playerName) {
        while (true) {
            cout << "  " << playerName
                 << " [" << current << "] - Pick a cell (1-9): ";
            int cell;
            if (!(cin >> cell)) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "  [!!] Enter a number 1-9.\n";
                continue;
            }
            cin.ignore();
            int idx = cell - 1;
            if (idx < 0 || idx > 8) {
                cout << "  [!!] Must be between 1 and 9.\n";
                continue;
            }
            if (!isCell(board[idx])) {
                cout << "  [!!] Cell " << cell << " is already taken.\n";
                continue;
            }
            board[idx] = current;
            break;
        }
    }

    void aiTurn() {
        cout << "  CPU [O] is thinking";
        for (int i = 0; i < 3; ++i) {
            UI::sleep_ms(300);
            cout << ".";
            cout.flush();
        }
        cout << "\n";
        UI::sleep_ms(200);
        int m = bestAIMove();
        if (m != -1) board[m] = 'O';
    }

    void switchPlayer() { current = (current == 'X') ? 'O' : 'X'; }

    // ==========================================================
    //  ONE COMPLETE ROUND
    // ==========================================================
    bool playRound() {
        for (int i = 0; i < 9; ++i) board[i] = '1' + i;
        current = 'X';

        while (true) {
            drawBoard();

            bool isAI = vsAI && (current == 'O');
            if (isAI) aiTurn();
            else      humanTurn(current == 'X' ? p1name : p2name);

            char w = checkWinner();
            if (w != ' ') {
                drawBoard();
                if (w == 'X') {
                    ++p1score;
                    cout << "  *** " << p1name << " wins this round! ***\n";
                } else {
                    ++p2score;
                    cout << "  *** " << p2name << " wins this round! ***\n";
                }
                break;
            }
            if (isBoardFull()) {
                drawBoard();
                ++draws;
                cout << "  *** It's a draw! ***\n";
                break;
            }
            switchPlayer();
        }

        cout << "\n  Play another round? (y/n): ";
        char c; cin >> c; cin.ignore();
        return (c == 'y' || c == 'Y');
    }

    // ==========================================================
    //  SETUP
    // ==========================================================
    void setup() {
        UI::clear();
        UI::title("  GAME SETUP  ");
        cout << "\n"
             << "  Mode:\n"
             << "    [1] 2-Player\n"
             << "    [2] vs CPU  (Minimax AI)\n\n";
        int mode = UI::getInt("  Choice: ", 1, 2);
        vsAI = (mode == 2);

        if (cin.peek() == '\n') cin.ignore();

        cout << "\n  Player 1 name (X): ";
        getline(cin, p1name);
        if (p1name.empty()) p1name = "Player 1";

        if (vsAI) {
            p2name = "CPU";
        } else {
            cout << "  Player 2 name (O): ";
            getline(cin, p2name);
            if (p2name.empty()) p2name = "Player 2";
        }

        p1score = p2score = draws = 0;
    }

    // ==========================================================
    //  FINAL RESULTS
    // ==========================================================
    void showResults() const {
        UI::clear();
        UI::title("  FINAL RESULTS  ");
        cout << "\n";
        UI::border('-');
        cout << "  " << p1name << " [X] : " << p1score << " win(s)\n"
             << "  " << p2name << " [O] : " << p2score << " win(s)\n"
             << "  Draws         : " << draws     << "\n";
        UI::border('-');
        cout << "\n";

        if (p1score > p2score)
            cout << "  CHAMPION: " << p1name << " - well played!\n";
        else if (p2score > p1score)
            cout << "  CHAMPION: " << p2name << " - well played!\n";
        else
            cout << "  Overall draw - evenly matched!\n";

        UI::pause();
    }

public:
    TicTacToe()
        : current('X'), vsAI(false),
          p1score(0), p2score(0), draws(0) {}

    void run() {
        setup();
        while (playRound()) {}
        showResults();
    }
};

// Static win-pattern table (no constexpr, works on all MinGW versions)
const int TicTacToe::WINS[8][3] = {
    {0,1,2}, {3,4,5}, {6,7,8},   // rows
    {0,3,6}, {1,4,7}, {2,5,8},   // columns
    {0,4,8}, {2,4,6}             // diagonals
};


// ==============================================================
//  ENTRY POINT
// ==============================================================
int main() {
    SetConsoleOutputCP(CP_UTF8);

    bool playAgain = true;
    while (playAgain) {
        TicTacToe game;
        game.run();

        UI::clear();
        UI::title("  TIC-TAC-TOE  ");
        cout << "\n  Start a brand-new game? (y/n): ";
        char c; cin >> c; cin.ignore();
        playAgain = (c == 'y' || c == 'Y');
    }

    UI::clear();
    UI::title("  THANKS FOR PLAYING!  ");
    cout << "\n\n";
    return 0;
}