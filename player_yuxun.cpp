#include <iostream>
#include <fstream>
#include <array>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <set>
// Declare
struct Point {
    int x, y;
    Point() : Point(0, 0) {}
	Point(int x, int y) : x(x), y(y) {}
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
int disc_on_board = 0;
std::vector<Point> next_valid_spots;
using State = std::array<std::array<int, SIZE>, SIZE>;
State board;

const std::array<Point, 8> directions{{
    Point(-1, -1), Point(-1, 0), Point(-1, 1),
    Point(0, -1), /*{0, 0}, */Point(0, 1),
    Point(1, -1), Point(1, 0), Point(1, 1)
}};
struct Node{
    State _s;
    int value;
    int disc_num;
    Point pos;
    Point best_choice;
    std::set<Node*> child;
    std::vector<Point> valid_spots;
};

bool is_spot_on_board(Point p);
bool is_disc_at(State s, Point p, int disc);
int state_value(Node *curr);
std::vector<Point> get_valid_spots(State s, int curr_player);
Node* make_node(State s, Point dir, int disc_num);
void build_tree(Node *curr, int curr_player);
bool is_spot_valid(State s, Point center, int curr_player);
State flip_board(State parent, Point center, int curr_player);
int alpha_beta(Node *curr, int depth, int alpha, int beta, bool maximizingPlayer);

bool is_spot_on_board(Point p){
    return 0 <= p.x && p.x < SIZE && 0 <= p.y && p.y < SIZE;
}
bool is_disc_at(State s, Point p, int disc) {
    if (!is_spot_on_board(p)) return false;
    if (s[p.x][p.y] != disc) return false;
    return true;
}
// Calculate state value

int state_value(Node *curr){
    
    State Board = curr->_s;
    int value = 0;
    curr->disc_num =0;

    // Board value only for 8x8
    int boardValue[SIZE][SIZE] = {
        {100, -3, 11, 8,  8, 11, -3, 100},
        {-3, -7, -4, 1,  1, -4, -7, -3},
        {11, -4, 2,  2,  2,  2, -4, 11},
        { 8,  1, 2, -3, -3,  2,  1,  8},
        { 8,  1, 2, -3, -3,  2,  1,  8},
        {11, -4, 2,  2,  2,  2, -4, 11},
        {-3, -7, -4, 1,  1, -4, -7, -3},
        {100, -3, 11, 8,  8, 11, -3, 100}
    };

    for (int i = 0; i < SIZE; i++){
        for (int j = 0; j < SIZE; j++){
            if (Board[i][j] == player) value += boardValue[i][j];
            if (Board[i][j] == 3 - player) value -= boardValue[i][j];
        }
    }

    // for (int i = 0; i < SIZE; i++){
    //     for (int j = 0; j < SIZE; j++){
    //         if (Board[i][j] == player) value += 1;
    //         if (Board[i][j] == 3 - player) value -= 1;
    //         if (Board[i][j] != 0) curr->disc_num++;
    //     }
    // }

    // // Corner
    // const int corner = 100000;
    // // Upper left
    // if (Board[0][0] == player) value += corner;
    // if (Board[0][0] == 3 - player) value -= corner;
    // // Upper right
    // if (Board[0][SIZE-1] == player) value += corner;
    // if (Board[0][SIZE-1] == 3 - player) value -= corner;
    // // Lower left
    // if (Board[SIZE-1][0] == player) value += corner;
    // if (Board[SIZE-1][0] == 3 - player) value -= corner;
    // // Lower right
    // if (Board[SIZE-1][SIZE-1] == player) value += corner;
    // if (Board[SIZE-1][SIZE-1] == 3 - player) value -= corner;

    // // Position around corner
    // const int aroundCorner = -30;
    // // Upper left
    // if (Board[0][1] == player) value += aroundCorner;
    // if (Board[0][1] == 3 - player) value -= aroundCorner;
    // if (Board[1][0] == player) value += aroundCorner;
    // if (Board[1][0] == 3 - player) value -= aroundCorner;
    // if (Board[1][1] == player) value += aroundCorner;
    // if (Board[1][1] == 3 - player) value -= aroundCorner;
    // // Upper right
    // if (Board[0][SIZE-2] == player) value += aroundCorner;
    // if (Board[0][SIZE-2] == 3 - player) value -= aroundCorner;
    // if (Board[1][SIZE-1] == player) value += aroundCorner;
    // if (Board[1][SIZE-1] == 3 - player) value -= aroundCorner;
    // if (Board[1][SIZE-2] == player) value += aroundCorner;
    // if (Board[1][SIZE-2] == 3 - player) value -= aroundCorner;
    // // Lower left
    // if (Board[SIZE-2][0] == player) value += aroundCorner;
    // if (Board[SIZE-2][0] == 3 - player) value -= aroundCorner;
    // if (Board[SIZE-1][1] == player) value += aroundCorner;
    // if (Board[SIZE-1][1] == 3 - player) value -= aroundCorner;
    // if (Board[SIZE-2][1] == player) value += aroundCorner;
    // if (Board[SIZE-2][1] == 3 - player) value -= aroundCorner;
    // // Lower right
    // if (Board[SIZE-1][SIZE-2] == player) value += aroundCorner;
    // if (Board[SIZE-1][SIZE-2] == 3 - player) value -= aroundCorner;
    // if (Board[SIZE-2][SIZE-1] == player) value += aroundCorner;
    // if (Board[SIZE-2][SIZE-1] == 3 - player) value -= aroundCorner;
    // if (Board[SIZE-2][SIZE-2] == player) value += aroundCorner;
    // if (Board[SIZE-2][SIZE-2] == 3 - player) value -= aroundCorner;

    // // Edge
    // const int edge = 10;
    // for (int i = 2; i < SIZE - 2; i++){
    //     if (Board[i][0] == player) value += edge;
    //     if (Board[i][0] == 3 - player) value += edge;
    //     if (Board[i][SIZE-1] == player) value += edge;
    //     if (Board[i][SIZE-1] == 3 - player) value += edge;
    //     if (Board[0][i] == player) value += edge;
    //     if (Board[0][i] == 3 - player) value += edge;
    //     if (Board[SIZE-1][i] == player) value += edge;
    //     if (Board[SIZE-1][i] == 3 - player) value += edge;
    // }

    return value;
}

std::vector<Point> get_valid_spots(State s, int curr_player) {
    std::vector<Point> valid_spots;
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            if (s[i][j] != 0) continue;
            Point p = Point(i, j);
            if (is_spot_valid(s, p, curr_player)) valid_spots.push_back(p);
        }
    }
    return valid_spots;
}


Node* make_node(State s, Point dir, int disc_num){
    Node *temp = new Node[1];
    temp->_s = s;
    temp->pos = dir;
    temp->value = state_value(temp);
    temp->disc_num = disc_num;
    return temp;
}

bool is_spot_valid(State s, Point center, int curr_player) {
    if (s[center.x][center.y] != 0) return false;

    for (Point dir: directions) {
        // Move along the direction while testing.
        Point p = center + dir;
        if (!is_disc_at(s, p, 3 - curr_player)) continue;
        p = p + dir;
        while (is_spot_on_board(p) && s[p.x][p.y] != 0) {
            if (is_disc_at(s, p, curr_player)) return true;
            p = p + dir;
        }
    }
    return false;
}

// Flip the board
State flip_board(State parent, Point center, int curr_player) {
    State ss = parent;
    
    for (Point dir: directions) {
        // Move along the direction while testing.
        Point p = center + dir;
        if (!is_disc_at(ss, p, 3 - curr_player)) continue;
        
        std::vector<Point> discs({p});
        p = p + dir;

        while (is_spot_on_board(p) && ss[p.x][p.y] != 0) {
            if (is_disc_at(ss, p, curr_player)) {
                for (Point d: discs) {
                    ss[d.x][d.y] = curr_player;
                }
            }
            discs.push_back(p);
            p = p + dir;
        }
    }

    return ss;
}

// Alpha-Beta pruning
int alpha_beta(Node *curr, int depth, int alpha, int beta, bool maximizingPlayer){
    
    if(curr->disc_num == SIZE * SIZE || depth == 0) return curr->value;

    // Find curr_childs
    if (maximizingPlayer) curr->valid_spots = get_valid_spots(curr->_s, player);
    else curr->valid_spots = get_valid_spots(curr->_s, 3 - player);


    if (maximizingPlayer){
        int value = INT32_MIN;
        for (auto i : curr->valid_spots){
            Node *child = make_node(flip_board(curr->_s, i, player), i, curr->disc_num + 1);
            value = std::max(value, alpha_beta(child, depth - 1, alpha, beta, false));

            if (value > alpha) curr->best_choice = child->pos;
            
            alpha = std::max(alpha, value);
            if (alpha >= beta) break;
        }
        return value;
    }else { // minimizingPlayer
        int value = INT32_MAX;
        for (auto i : curr->valid_spots){
            Node *child = make_node(flip_board(curr->_s, i, player), i, curr->disc_num + 1);
            value = std::min(value, alpha_beta(child, depth - 1, alpha, beta, true));
    
            if (value < beta) curr->best_choice = child->pos;
            
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
            if (board[i][j] != 0) disc_on_board++;
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

    // int n_valid_spots = next_valid_spots.size();

    // srand(time(NULL));
    // // Choose random spot. (Not random uniform here)
    // int index = (rand() % n_valid_spots);
    // Point p = next_valid_spots[index];

    Node *root = make_node(board, Point(0, 0), disc_on_board);
    alpha_beta(root, 10, INT32_MIN, INT32_MAX, true);
    
    // Remember to flush the output to ensure the last action is written to file.
    fout << root->best_choice.x << " " << root->best_choice.y << std::endl;
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
