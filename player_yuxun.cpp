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
    std::set<Node*> child;
    std::vector<Point> valid_spots;
};

bool is_spot_on_board(Point p);
bool is_disc_at(State s, Point p, int disc);
int state_value(State Board);
std::vector<Point> get_valid_spots(State s, int curr_player);
Node* make_node(State s);
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
int state_value(State Board){
    
    int value = 0;

    for (int i = 0; i < SIZE; i++){
        for (int j = 0; j < SIZE; j++){
            if (Board[i][j] == player) value += 1;
            if (Board[i][j] == 3 - player) value -= 1;
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

std::vector<Point> get_valid_spots(State s, int curr_player) {
    std::vector<Point> valid_spots;
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            Point p = Point(i, j);
            if (s[i][j] != 0) continue;
            if (is_spot_valid(s, p, curr_player)) valid_spots.push_back(p);
        }
    }
    return valid_spots;
}


Node* make_node(State s){
    Node *temp = new Node();
    temp->_s = s;
    temp->value = state_value(s);
    return temp;
}
void build_tree(Node *curr, int curr_player){
    
    curr->valid_spots = get_valid_spots(curr->_s, curr_player);
    
    for (auto i : curr->valid_spots){
        curr->child.insert(make_node(flip_board(curr->_s, i, curr_player)));

    }

    for(auto i : curr->child){
        build_tree(i, 3 - curr_player);
    }
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

    if (depth == 0 || curr->child.empty()) return curr->value;

    if (maximizingPlayer){
        int value = INT32_MIN;
        for (auto child : curr->child){
            value = std::max(value, alpha_beta(child, depth - 1, alpha, beta, false));
            alpha = std::max(alpha, value);
            if (alpha >= beta) break;
        }
        return value;
    }else { // minimizingPlayer
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

    // int n_valid_spots = next_valid_spots.size();

    // srand(time(NULL));
    // // Choose random spot. (Not random uniform here)
    // int index = (rand() % n_valid_spots);
    // Point p = next_valid_spots[index];

    Node *root = make_node(board);
    build_tree(root, player);
    
    int maxChild, count = 0, id;
    Point p;
    for (auto it = root->child.begin(); it != root->child.end(); it++){
        int value = alpha_beta(*it, 64, INT32_MIN, INT32_MAX, 3 - player);
        if (it == root->child.begin() || value > maxChild){
            maxChild = value;
            id = count;
        }
        count++;
    }
    auto it = root->valid_spots.begin();
    while(id--) it++;
    p = *it;

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
