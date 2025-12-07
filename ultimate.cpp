#include <iostream>
#include <iomanip>
#include <cctype>
#include <cstdlib>
#include "ultimate.h"

using namespace std;

vector<int> index(9);
vector<char> win(9);

ultimate_Board::ultimate_Board() : Board(9, 9) {
    // Initialize all cells with blank_symbol
    for (auto& row : board)
        for (auto& cell : row)
            cell = blank_symbol;

    // Initialize index and win arrays
    for (int i = 0; i < 9; i++) {
        index[i] = 0;
        win[i] = '\0';
    }
}

void ultimate_Board::reset_board() {
    // Clear the board
    for (auto& row : board)
        for (auto& cell : row)
            cell = blank_symbol;

    // Reset move counter
    n_moves = 0;

    // Reset index and win arrays
    for (int i = 0; i < 9; i++) {
        index[i] = 0;
        win[i] = '\0';
    }
}

bool ultimate_Board::update_board(Move<char>* move) {
    int x = move->get_x();
    int y = move->get_y();
    char mark = move->get_symbol();

    // Validate move and apply if valid
    if (!(x < 0 || x >= rows || y < 0 || y >= columns) &&
        (board[x][y] == blank_symbol || mark == 0)) {

        if (mark == 0) {
            // Undo move
            n_moves--;
            board[x][y] = blank_symbol;
        }
        else {
            // Make move
            n_moves++;
            board[x][y] = toupper(mark);
            check_subboard_wins();
            mark_closed_boards();
        }
        return true;
    }
    return false;
}

void ultimate_Board::check_subboard_wins() {
    struct SubBoard { int id, row_start, row_end, col_start, col_end; };

    SubBoard subboards[9] = {
        {0,0,2,0,2},{1,0,2,3,5},{2,0,2,6,8},
        {3,3,5,0,2},{4,3,5,3,5},{5,3,5,6,8},
        {6,6,8,0,2},{7,6,8,3,5},{8,6,8,6,8}
    };

    for (auto& sb : subboards) {
        if (index[sb.id] == 1) continue;
        char winner = check_subboard_winner(sb.row_start, sb.row_end, sb.col_start, sb.col_end);
        if (winner != blank_symbol) {
            index[sb.id] = 1;
            win[sb.id] = winner;
        }
    }
}

char ultimate_Board::check_subboard_winner(int r_start, int r_end, int c_start, int c_end) {
    // Rows
    for (int i = r_start; i <= r_end; i++)
        if (board[i][c_start] != blank_symbol && board[i][c_start] != '#' &&
            board[i][c_start] == board[i][c_start + 1] &&
            board[i][c_start] == board[i][c_start + 2])
            return board[i][c_start];

    // Columns
    for (int j = c_start; j <= c_end; j++)
        if (board[r_start][j] != blank_symbol && board[r_start][j] != '#' &&
            board[r_start][j] == board[r_start + 1][j] &&
            board[r_start][j] == board[r_start + 2][j])
            return board[r_start][j];

    // Main diagonal
    if (board[r_start][c_start] != blank_symbol && board[r_start][c_start] != '#' &&
        board[r_start][c_start] == board[r_start + 1][c_start + 1] &&
        board[r_start][c_start] == board[r_start + 2][c_start + 2])
        return board[r_start][c_start];

    // Anti-diagonal
    if (board[r_start][c_start + 2] != blank_symbol && board[r_start][c_start + 2] != '#' &&
        board[r_start][c_start + 2] == board[r_start + 1][c_start + 1] &&
        board[r_start][c_start + 2] == board[r_start + 2][c_start])
        return board[r_start][c_start + 2];

    return blank_symbol;
}

void ultimate_Board::mark_closed_boards() {
    struct SubBoard { int id, row_start, row_end, col_start, col_end; };

    SubBoard subboards[9] = {
        {0,0,2,0,2},{1,0,2,3,5},{2,0,2,6,8},
        {3,3,5,0,2},{4,3,5,3,5},{5,3,5,6,8},
        {6,6,8,0,2},{7,6,8,3,5},{8,6,8,6,8}
    };

    for (auto& sb : subboards) {
        if (index[sb.id] == 1) {
            for (int i = sb.row_start; i <= sb.row_end; i++)
                for (int j = sb.col_start; j <= sb.col_end; j++)
                    if (board[i][j] == blank_symbol) board[i][j] = '#';
        }
    }
}

bool ultimate_Board::is_win(Player<char>* player) {
    char sym = player->get_symbol();
    // Rows, Columns, Diagonals
    if ((win[0]==sym && win[1]==sym && win[2]==sym) ||
        (win[3]==sym && win[4]==sym && win[5]==sym) ||
        (win[6]==sym && win[7]==sym && win[8]==sym) ||
        (win[0]==sym && win[3]==sym && win[6]==sym) ||
        (win[1]==sym && win[4]==sym && win[7]==sym) ||
        (win[2]==sym && win[5]==sym && win[8]==sym) ||
        (win[0]==sym && win[4]==sym && win[8]==sym) ||
        (win[2]==sym && win[4]==sym && win[6]==sym)) return true;

    return false;
}

bool ultimate_Board::is_draw(Player<char>* player) {
    int closed_count=0;
    for(int i=0;i<9;i++) if(index[i]==1) closed_count++;

    if(closed_count==9) {
        for(char sym : {'X','O'}) {
            if(is_win(new Player<char>("temp",sym,PlayerType::HUMAN))) return false;
        }
        return true;
    }
    if(n_moves>=81) return true;
    return false;
}

bool ultimate_Board::game_is_over(Player<char>* player) {
    return is_win(player) || is_draw(player);
}

ultimate_UI::ultimate_UI() : UI<char>("Welcome to Ultimate Tic Tac Toe", 3) {}

Player<char>* ultimate_UI::create_player(string& name, char symbol, PlayerType type) {
    cout << "Creating " << (type==PlayerType::HUMAN?"human":"computer") 
         << " player: " << name << " (" << symbol << ")\n";
    return new Player<char>(name,symbol,type);
}

Move<char>* ultimate_UI::get_move(Player<char>* player) {
    int x,y;

    if(player->get_type()==PlayerType::HUMAN) {
        cout << player->get_name() << " ,Please enter your move x and y (0-8): ";
        cin >> x >> y;
    } else {
        // ---------- Smarter AI using update_board() ----------
        ultimate_Board* board = dynamic_cast<ultimate_Board*>(player->get_board_ptr());
        int best_score=-1000, best_x=0, best_y=0;
        char sym = player->get_symbol();
        char opp = (sym=='X'?'O':'X');

        for(int i=0;i<board->get_rows();i++) {
            for(int j=0;j<board->get_columns();j++) {
                if(board->get_cell(i,j)==board->get_blank_symbol()) {
                    Move<char>* temp_move = new Move<char>(i,j,sym);
                    board->update_board(temp_move);

                    int score=0;
                    if(board->is_win(player)) score=10;
                    else {
                        Player<char> temp_opponent("temp",opp,PlayerType::HUMAN);
                        if(board->is_win(&temp_opponent)) score=5;
                    }

                    board->update_board(new Move<char>(i,j,0)); // undo
                    if(score>best_score) { best_score=score; best_x=i; best_y=j; }

                    delete temp_move;
                }
            }
        }
        x=best_x; y=best_y;
        // -----------------------------------------------------
    }

    return new Move<char>(x,y,player->get_symbol());
}

void ultimate_UI::display_meta_board(const vector<char>& win_array) {
    cout << "     0   1   2" << endl;
    cout << "   -------------" << endl;

    for(int i=0;i<3;i++){
        cout << " " << i << " |";
        for(int j=0;j<3;j++){
            int idx = i*3+j;
            char display=' ';
            if(win_array[idx]=='X') display='X';
            else if(win_array[idx]=='O') display='O';
            cout << " " << display << " |";
        }
        cout << endl << "   -------------" << endl;
    }
}

void ultimate_UI::display_board_matrix(const vector<vector<char>>& matrix) const {
    UI<char>::display_board_matrix(matrix);
    const_cast<ultimate_UI*>(this)->display_meta_board(win);
}
