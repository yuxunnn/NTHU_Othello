#include <iostream>
#include <fstream>
#include <array>
#include <queue>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <set>
#include <utility>

struct Point {
    int x, y;
    Point() : Point(0, 0) {};
    Point(int x, int y) : x(x), y(y) {};
	bool operator==(Point& rhs) {
		return x == rhs.x && y == rhs.y;
	};
	bool operator!=(Point& rhs) {
		return !operator==(rhs);
	};
	Point operator+(Point& rhs) {
		return Point(x + rhs.x, y + rhs.y);
	};
	Point operator-(Point& rhs) {
		return Point(x - rhs.x, y - rhs.y);
	};
};

const int SIZE = 8;
const int MaxDepth = 6;
using State = std::array<std::array<int, SIZE>, SIZE>;
int player;
std::vector<Point> next_valid_spots;
State board;

const std::array<Point, 8> directions{
    Point(-1, -1), Point(-1, 0), Point(-1, 1),
    Point(0, -1), /*{0, 0}, */Point(0, 1),
    Point(1, -1), Point(1, 0), Point(1, 1)
};

std::array <Point, 4> near{
    Point(-1, 0), Point(1, 0),
    Point(0, -1), Point(0, 1)
};

struct Node{
    State _s;
    int value;
    int player_disc_cnt;
    int opponent_disc_cnt;
    Point pos;
    Point best_choice;
    std::set<Node*> child;
    std::vector<Point> valid_spots;
};

// Fuction declaration
// ========================================================================================

// Check if spot on board
bool is_spot_on_board(Point p);

// Check if the position has the right disc
bool is_disc_at(State s, Point p, int disc);

// Check if we can put the next disc
bool is_spot_valid(State s, Point center, int curr_player);

// Get valid spots
std::vector<Point> get_valid_spots(State s, int curr_player);

// Make new node
Node* make_node(State s, Point dir, int cnt);

// Count connected same disc 
int count_parity(Node *curr, int type);

// Calculate the board value
int state_value(Node *curr);

// Flip the board to the next state
State flip_board(State parent, Point center, int curr_player);

// Alpha-Beta pruning
int alpha_beta(Node *curr, int depth, int alpha, int beta, bool maximizingPlayer);

// ========================================================================================

bool is_spot_on_board(Point p){
    return 0 <= p.x && p.x < SIZE && 0 <= p.y && p.y < SIZE;
}

bool is_disc_at(State s, Point p, int disc) {
    if (!is_spot_on_board(p)) return false;
    if (s[p.x][p.y] != disc) return false;
    return true;
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

Node* make_node(State s, Point dir){
    Node *temp = new Node[1];
    temp->_s = s;
    temp->pos = dir;
    temp->player_disc_cnt = 0;
    temp->opponent_disc_cnt = 0;
    temp->value = 0;
    temp->valid_spots.clear();
    temp->value = state_value(temp);
    return temp;
}

int count_parity(Node *curr, int type){

    std::queue <std::pair<int, int>> Q;
    std::set <std::pair<int, int>> explored;
    Q.push(std::pair<int, int> {curr->pos.x, curr->pos.y});
    explored.insert(std::pair<int, int> {curr->pos.x, curr->pos.y});

    // BFS search
    int total = 1;
    while(!Q.empty()){
        auto qq = Q.front();
        Q.pop();

        for (int i = 0; i < SIZE; i++){
            if (qq.first + near[i].x >= 0 && qq.first + near[i].x < SIZE && qq.second + near[i].y >=0 && qq.second + near[i].y < SIZE){
                // Check the spot is empty
                if (curr->_s[qq.first + near[i].x][qq.second + near[i].y] == type){
                    std::pair<int, int> temp {qq.first + near[i].x, qq.second + near[i].y};
                    if (explored.find(temp) == explored.end()){
                        total++;
                        Q.push(temp);
                        explored.insert(temp);
                    }
                }
            }
        }
    }

    return total;
}

int state_value(Node *curr){
    
    State Board = curr->_s;
    int value = 0;

    int boardValue[SIZE][SIZE] = {
        {50,  -7, 20, 12,  12, 20,  -7, 50},
        {-7, -15, -4,  1,   1, -4, -15, -7},
        {20,  -4,  2,  2,   2,  2,  -4, 20},
        {12,   1,  2,  0,   0,  2,   1, 12},
        {12,   1,  2,  0,   0,  2,   1, 12},
        {20,  -4,  2,  2,   2,  2,  -4, 20},
        {-7, -15, -4,  1,   1, -4, -15, -7},
        {50,  -7, 20, 12,  12, 20,  -7, 50}
    };
    
    for (int i = 0; i < SIZE; i++) for (int j = 0; j < SIZE; j++){
        if (Board[i][j] == player){
            value += boardValue[i][j];
            curr->player_disc_cnt++;
        }else if (Board[i][j] == 3 - player){
            value -= boardValue[i][j];
            curr->opponent_disc_cnt++;
        }
    }

    return value;
}

State flip_board(State parent, Point center, int curr_player) {
    State ss = parent;
    
    ss[center.x][center.y] = curr_player;

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
                break;
            }
            discs.push_back(p);
            p = p + dir;
        }
    }


    return ss;
}

int alpha_beta(Node *curr, int depth, int alpha, int beta, bool maximizingPlayer){

    // Last state of full board
    if (curr->player_disc_cnt + curr->opponent_disc_cnt == SIZE * SIZE) return (curr->player_disc_cnt - curr->opponent_disc_cnt);
    // Already maxDepth
    if (depth == 0) return curr->value;

    // Find curr_childs
    if (maximizingPlayer) curr->valid_spots = get_valid_spots(curr->_s, player);
    else curr->valid_spots = get_valid_spots(curr->_s, 3 - player);

    // Alpha-Beat pruning
    if (maximizingPlayer){

        // Mobility become zero
        if (curr->valid_spots.empty()) return INT32_MAX;

        int value = INT32_MIN;
        
        for (auto i : curr->valid_spots){
            Node *child = make_node(flip_board(curr->_s, i, player), i);
                
            value = std::max(value, alpha_beta(child, depth - 1, alpha, beta, false));

            if (value > alpha) curr->best_choice = child->pos;
                    
            alpha = std::max(alpha, value);
            
            // Prune
            if (alpha >= beta) break;
        }
        return value;
    }else { // minimizingPlayer

        // Mobility become zero
        if (curr->valid_spots.empty()) return INT32_MAX;

        int value = INT32_MAX;

        for (auto i : curr->valid_spots){
            Node *child = make_node(flip_board(curr->_s, i, 3 - player), i);

            value = std::min(value, alpha_beta(child, depth - 1, alpha, beta, true));
            
            if (value < beta) curr->best_choice = child->pos;
            
            beta = std::min(beta, value);

            // Prune
            if (beta <= alpha) break;
        }
        return value;
    }

}

// Read the init board
void read_board(std::ifstream& fin) {
    fin >> player;
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            fin >> board[i][j];
        }
    }
}

// Read valid spots of the init board
void read_valid_spots(std::ifstream& fin) {
    int n_valid_spots;
    fin >> n_valid_spots;
    int x, y;
    for (int i = 0; i < n_valid_spots; i++) {
        fin >> x >> y;
        next_valid_spots.push_back({x, y});
    }
}

// Output the best valid spot
void write_valid_spot(std::ofstream& fout) {

    Node *root = make_node(board, Point(-1, -1));

    alpha_beta(root, MaxDepth, INT32_MIN, INT32_MAX, true);
    
    // Remember to flush the output to ensure the last action is written to file.
    fout << root->best_choice.x << " " << root->best_choice.y << std::endl;
    fout.flush();
}

// Main
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
