#include <iostream>
#include <fstream>
#include <array>
#include <vector>
#include <cstdlib>
#include <ctime>

struct Point {
    int x, y;
};

int player;
const int SIZE = 8;
std::array<std::array<int, SIZE>, SIZE> board;
std::vector<Point> next_valid_spots;

// Define State
using State = std::array<std::array<int, SIZE>, SIZE>;

// Calculate state value
int state_value(State Board){
    
    int value = 0;

    for (int i = 0; i < SIZE; i++){
        for (int j = 0; j < SIZE; j++){
            if (Board[i][j] == 1) value += 1; // black
            if (Board[i][j] == 2) value -= 1; // white
        }
    }

    int corner = 10;
    // Upper left
    if (Board[0][0] == player) value += corner;
    if (Board[0][0] == 3 - player) value -= corner;
    // Upper right
    if (Board[0][SIZE-1] == player) value += corner;
    if (Board[0][SIZE-1] == 3 - player) value -= corner;
    // Lower left
    if (Board[SIZE-1][0] == player) value += corner;
    if (Board[SIZE-1][0] == 3 - player) value -= corner;
    // Lower right
    if (Board[SIZE-1][SIZE-1] == player) value += corner;
    if (Board[SIZE-1][SIZE-1] == 3 - player) value -= corner;

    return value;
}

// Flip the board
std::vector<int> dx = {-1, -1, -1, 0, 0, 1, 1, 1};
std::vector<int> dy = {-1, 0, 1, -1, 1, -1, 0, 1};
State flip_board(const State Board, const int x, const int y){
    State ss = Board;
    int targetX, targetY;
    int currX, currY;
    for (int i = 0; i < 8; i++){
        targetX = x, targetY = y; 
        currX = x, currY = y;
        while(1){
            currX += dx[i], currY += dy[i];
            if (currX < 0 || currX >= SIZE || currY < 0 || currY >= SIZE) break;
            if (ss[currX][currY] == player){
                targetX = currX;
                targetY = currY;
            } else if (ss[currX][currY] == 0) break;
        }
        currX = x, currY = y;
        while(currX != targetX && currY != targetY){
            currX += dx[i], currY += dy[i];
            ss[currX][currY] = player;
        }
    }    
    return ss;
}

void read_board(std::ifstream& fin) {
    fin >> player;
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            fin >> board[i][j];
        }
    }
}

void read_valid_spots(std::ifstream& fin) {
    int n_valid_spots;
    fin >> n_valid_spots;
    int x, y;
    for (int i = 0; i < n_valid_spots; i++) {
        fin >> x >> y;
        next_valid_spots.push_back({x, y});
    }
}

void write_valid_spot(std::ofstream& fout) {
    int n_valid_spots = next_valid_spots.size();

    // srand(time(NULL));
    // // Choose random spot. (Not random uniform here)
    // int index = (rand() % n_valid_spots);
    // Point p = next_valid_spots[index];

    int maxVal;
    Point p;
    for (int i = 0; i < n_valid_spots; i++){
        State ss = flip_board(board, next_valid_spots[i].x, next_valid_spots[i].y);
        int value = state_value(ss);
        if (i == 0 || value < maxVal){
            maxVal = value;
            p.x = next_valid_spots[i].x, p.y = next_valid_spots[i].y;
        }
    }

    // Remember to flush the output to ensure the last action is written to file.
    fout << p.x << " " << p.y << std::endl;
    fout.flush();
}

int main(int, char** argv) {
    std::ifstream fin(argv[1]);
    std::ofstream fout(argv[2]);
    read_board(fin);
    read_valid_spots(fin);
    write_valid_spot(fout);
    fin.close();
    fout.close();
    return 0;
}
