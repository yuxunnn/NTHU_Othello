#include <iostream>
#include <fstream>
#include <array>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <set>
struct Point {
    int x, y;
    Point() : Point(0, 0) {}
	Point(float x, float y) : x(x), y(y) {}
	bool operator==(const Point& rhs) const {
		return x == rhs.x && y == rhs.y;
	}
	bool operator!=(const Point& rhs) const {
		return !operator==(rhs);
	}
	Point operator+(const Point& rhs) const {
		return Point(x + rhs.x, y + rhs.y);
	}
	Point operator-(const Point& rhs) const {
		return Point(x - rhs.x, y - rhs.y);
	}
};

int player;
const int SIZE = 8;
std::array<std::array<int, SIZE>, SIZE> board;
std::vector<Point> next_valid_spots;

using State = std::array<std::array<int, SIZE>, SIZE>;

const std::array<Point, 8> directions{{
    Point(-1, -1), Point(-1, 0), Point(-1, 1),
    Point(0, -1), /*{0, 0}, */Point(0, 1),
    Point(1, -1), Point(1, 0), Point(1, 1)
}};
bool is_spot_on_board(Point p){
    return 0 <= p.x && p.x < SIZE && 0 <= p.y && p.y < SIZE;
}
bool is_disc_at(State s, Point p, int disc) {
    if (!is_spot_on_board(p)) return false;
    if (s[p.x][p.y] != disc) return false;
    return true;
}
struct Node{
    State _s;
    int value;
    std::set<Node*> child;
};

Node* make_node(State s){
    Node *temp = new Node();
    temp->_s = s;
    temp->value = state_value(s);
    return temp;
}

Node* build_tree(Node *curr){

}

// Calculate state value
int state_value(State Board){
    
    int value = 0;

    for (int i = 0; i < SIZE; i++){
        for (int j = 0; j < SIZE; j++){
            if (Board[i][j] == 1) value += 1; // black
            if (Board[i][j] == 2) value -= 1; // white
        }
    }

    const int corner = 10;
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
State flip_board(State parent, Point center, int curr_player) {
    State s = parent;
    for (Point dir: directions) {
        // Move along the direction while testing.
        Point p = center + dir;
        if (!is_disc_at(s, p, 3 - curr_player)) continue;
        std::vector<Point> discs({p});
        p = p + dir;
        while (is_spot_on_board(p) && s[p.x][p.y] != 0) {
            if (is_disc_at(s, p, curr_player)) {
                for (Point d: discs) {
                    s[d.x][d.y] = curr_player;
                }
            }
            discs.push_back(p);
            p = p + dir;
        }
    }
    return s;
}

// Alpha-Beta pruning
int alpha_beta(Node *curr, int depth, int alpha, int beta, bool maximizingPlayer){

    if (depth == 0 || curr->child.empty()) return curr->value;

    if (maximizingPlayer){
        int value = INT32_MIN;
        for (auto child : curr->child){
            value = std::max(value, alpha_beta(child, depth - 1, alpha, beta, false));
            alpha = std::max(alpha, value);
            if (alpha >= beta) break;
        }
        return value;
    }else {
        int value = INT32_MAX;
        for (auto child : curr->child){
            value = std::min(value, alpha_beta(child, depth - 1, alpha, beta, true));
            beta = std::min(beta, value);
            if (beta <= alpha) break;
        }
        return value;
    }

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
    Node *root = make_node(board);


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
