#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <limits.h>

#define BOARD_SIZE 12
#define EMPTY 0
#define BLACK 1
#define WHITE 2
#define MAX_DEPTH 6
#define WIN_SCORE 1000000
#define TIMEOUT_THRESHOLD 1800  // 1.8 seconds for safety margin

// Direction vectors for checking lines
int dx[] = {0, 1, 1, 1};
int dy[] = {1, 0, 1, -1};

typedef struct {
    int board[BOARD_SIZE][BOARD_SIZE];
    int my_color;
    int opponent_color;
    clock_t start_time;
    clock_t total_start_time;
    double total_time_used;
} GameState;

GameState game;

// Function declarations
int is_valid_move(int x, int y);
int check_win(int x, int y, int color);
int count_line(int x, int y, int dx, int dy, int color);
int evaluate_position(int x, int y, int color);
int evaluate_board();
int minimax(int depth, int alpha, int beta, int is_maximizing, double time_limit);
void get_best_move(int *best_x, int *best_y);
void make_move(int x, int y, int color);
void undo_move(int x, int y);
int has_adjacent_stone(int x, int y);
void generate_moves(int moves[][2], int *move_count);
void debug_print(const char* message);

// Check if a move is valid
int is_valid_move(int x, int y) {
    return x >= 0 && x < BOARD_SIZE && y >= 0 && y < BOARD_SIZE && game.board[x][y] == EMPTY;
}

// Check if there's a win at position (x,y) for given color
int check_win(int x, int y, int color) {
    for (int dir = 0; dir < 4; dir++) {
        int count = 1; // Count the piece just placed
        
        // Check positive direction
        int nx = x + dx[dir], ny = y + dy[dir];
        while (nx >= 0 && nx < BOARD_SIZE && ny >= 0 && ny < BOARD_SIZE && 
               game.board[nx][ny] == color) {
            count++;
            nx += dx[dir];
            ny += dy[dir];
        }
        
        // Check negative direction
        nx = x - dx[dir], ny = y - dy[dir];
        while (nx >= 0 && nx < BOARD_SIZE && ny >= 0 && ny < BOARD_SIZE && 
               game.board[nx][ny] == color) {
            count++;
            nx -= dx[dir];
            ny -= dy[dir];
        }
        
        if (count >= 5) return 1;
    }
    return 0;
}

// Count consecutive stones in one direction
int count_line(int x, int y, int dx, int dy, int color) {
    int count = 0;
    int nx = x, ny = y;
    
    while (nx >= 0 && nx < BOARD_SIZE && ny >= 0 && ny < BOARD_SIZE && 
           game.board[nx][ny] == color) {
        count++;
        nx += dx;
        ny += dy;
    }
    return count;
}

// Evaluate a specific position's strategic value
int evaluate_position(int x, int y, int color) {
    int score = 0;
    
    for (int dir = 0; dir < 4; dir++) {
        int forward = count_line(x, y, dx[dir], dy[dir], color);
        int backward = count_line(x, y, -dx[dir], -dy[dir], color);
        int total = forward + backward - 1; // -1 because we counted (x,y) twice
        
        // Scoring based on line length
        if (total >= 5) score += WIN_SCORE;
        else if (total == 4) score += 10000;
        else if (total == 3) score += 1000;
        else if (total == 2) score += 100;
        else if (total == 1) score += 10;
        
        // Check for blocked lines
        int blocked = 0;
        int fx = x + forward * dx[dir], fy = y + forward * dy[dir];
        int bx = x - backward * dx[dir], by = y - backward * dy[dir];
        
        if ((fx < 0 || fx >= BOARD_SIZE || fy < 0 || fy >= BOARD_SIZE || 
             game.board[fx][fy] == (3 - color)) &&
            (bx < 0 || bx >= BOARD_SIZE || by < 0 || by >= BOARD_SIZE || 
             game.board[bx][by] == (3 - color))) {
            blocked = 2; // Both ends blocked
        } else if ((fx < 0 || fx >= BOARD_SIZE || fy < 0 || fy >= BOARD_SIZE || 
                    game.board[fx][fy] == (3 - color)) ||
                   (bx < 0 || bx >= BOARD_SIZE || by < 0 || by >= BOARD_SIZE || 
                    game.board[bx][by] == (3 - color))) {
            blocked = 1; // One end blocked
        }
        
        // Reduce score for blocked lines
        if (blocked == 2) score /= 10;
        else if (blocked == 1) score /= 3;
    }
    
    return score;
}

// Evaluate the entire board position
int evaluate_board() {
    int my_score = 0, opponent_score = 0;
    
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (game.board[i][j] == game.my_color) {
                my_score += evaluate_position(i, j, game.my_color);
            } else if (game.board[i][j] == game.opponent_color) {
                opponent_score += evaluate_position(i, j, game.opponent_color);
            }
        }
    }
    
    return my_score - opponent_score;
}

// Check if position has adjacent stones (for move ordering)
int has_adjacent_stone(int x, int y) {
    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            if (i == 0 && j == 0) continue;
            int nx = x + i, ny = y + j;
            if (nx >= 0 && nx < BOARD_SIZE && ny >= 0 && ny < BOARD_SIZE && 
                game.board[nx][ny] != EMPTY) {
                return 1;
            }
        }
    }
    return 0;
}

// Generate candidate moves (only near existing stones for efficiency)
void generate_moves(int moves[][2], int *move_count) {
    *move_count = 0;
    
    // If board is empty, start from center
    int empty_count = 0;
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (game.board[i][j] == EMPTY) empty_count++;
        }
    }
    
    if (empty_count == BOARD_SIZE * BOARD_SIZE) {
        moves[0][0] = BOARD_SIZE / 2;
        moves[0][1] = BOARD_SIZE / 2;
        *move_count = 1;
        return;
    }
    
    // Generate moves near existing stones
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (is_valid_move(i, j) && has_adjacent_stone(i, j)) {
                moves[*move_count][0] = i;
                moves[*move_count][1] = j;
                (*move_count)++;
            }
        }
    }
}

// Make a move on the board
void make_move(int x, int y, int color) {
    game.board[x][y] = color;
}

// Undo a move
void undo_move(int x, int y) {
    game.board[x][y] = EMPTY;
}

// Minimax algorithm with alpha-beta pruning
int minimax(int depth, int alpha, int beta, int is_maximizing, double time_limit) {
    // Check time limit
    clock_t current_time = clock();
    double elapsed = ((double)(current_time - game.start_time)) / CLOCKS_PER_SEC * 1000;
    if (elapsed > time_limit) {
        return evaluate_board();
    }
    
    if (depth == 0) {
        return evaluate_board();
    }
    
    int moves[144][2]; // Maximum possible moves
    int move_count;
    generate_moves(moves, &move_count);
    
    if (move_count == 0) {
        return evaluate_board();
    }
    
    if (is_maximizing) {
        int max_eval = INT_MIN;
        for (int i = 0; i < move_count; i++) {
            int x = moves[i][0], y = moves[i][1];
            
            make_move(x, y, game.my_color);
            
            // Check for immediate win
            if (check_win(x, y, game.my_color)) {
                undo_move(x, y);
                return WIN_SCORE;
            }
            
            int eval = minimax(depth - 1, alpha, beta, 0, time_limit);
            undo_move(x, y);
            
            max_eval = (eval > max_eval) ? eval : max_eval;
            alpha = (alpha > eval) ? alpha : eval;
            if (beta <= alpha) break;
        }
        return max_eval;
    } else {
        int min_eval = INT_MAX;
        for (int i = 0; i < move_count; i++) {
            int x = moves[i][0], y = moves[i][1];
            
            make_move(x, y, game.opponent_color);
            
            // Check for immediate win
            if (check_win(x, y, game.opponent_color)) {
                undo_move(x, y);
                return -WIN_SCORE;
            }
            
            int eval = minimax(depth - 1, alpha, beta, 1, time_limit);
            undo_move(x, y);
            
            min_eval = (eval < min_eval) ? eval : min_eval;
            beta = (beta < eval) ? beta : eval;
            if (beta <= alpha) break;
        }
        return min_eval;
    }
}

// Find the best move using minimax
void get_best_move(int *best_x, int *best_y) {
    game.start_time = clock();
    
    int moves[144][2];
    int move_count;
    generate_moves(moves, &move_count);
    
    if (move_count == 0) {
        *best_x = BOARD_SIZE / 2;
        *best_y = BOARD_SIZE / 2;
        return;
    }
    
    int best_score = INT_MIN;
    *best_x = moves[0][0];
    *best_y = moves[0][1];
    
    // Check for immediate winning moves first
    for (int i = 0; i < move_count; i++) {
        int x = moves[i][0], y = moves[i][1];
        make_move(x, y, game.my_color);
        if (check_win(x, y, game.my_color)) {
            undo_move(x, y);
            *best_x = x;
            *best_y = y;
            return;
        }
        undo_move(x, y);
    }
    
    // Check for blocking opponent's winning moves
    for (int i = 0; i < move_count; i++) {
        int x = moves[i][0], y = moves[i][1];
        make_move(x, y, game.opponent_color);
        if (check_win(x, y, game.opponent_color)) {
            undo_move(x, y);
            *best_x = x;
            *best_y = y;
            return;
        }
        undo_move(x, y);
    }
    
    // Use minimax for other moves
    for (int i = 0; i < move_count; i++) {
        int x = moves[i][0], y = moves[i][1];
        make_move(x, y, game.my_color);
        
        int score = minimax(MAX_DEPTH - 1, INT_MIN, INT_MAX, 0, TIMEOUT_THRESHOLD);
        
        if (score > best_score) {
            best_score = score;
            *best_x = x;
            *best_y = y;
        }
        
        undo_move(x, y);
        
        // Check time limit
        clock_t current_time = clock();
        double elapsed = ((double)(current_time - game.start_time)) / CLOCKS_PER_SEC * 1000;
        if (elapsed > TIMEOUT_THRESHOLD) break;
    }
}

// Debug output function
void debug_print(const char* message) {
    printf("DEBUG %s\n", message);
    fflush(stdout);
}

int main() {
    // Initialize game state
    memset(game.board, 0, sizeof(game.board));
    game.total_time_used = 0.0;
    
    char command[100];
    int x, y;
    
    while (scanf("%s", command) == 1) {
        if (strcmp(command, "START") == 0) {
            scanf("%d", &game.my_color);
            game.opponent_color = (game.my_color == BLACK) ? WHITE : BLACK;
            game.total_start_time = clock();
            
            printf("OK\n");
            fflush(stdout);
            
        } else if (strcmp(command, "PLACE") == 0) {
            scanf("%d %d", &x, &y);
            if (is_valid_move(x, y)) {
                game.board[x][y] = game.opponent_color;
            }
            
        } else if (strcmp(command, "TURN") == 0) {
            clock_t turn_start = clock();
            
            get_best_move(&x, &y);
            
            if (is_valid_move(x, y)) {
                game.board[x][y] = game.my_color;
                printf("%d %d\n", x, y);
                fflush(stdout);
            } else {
                // Fallback to first available move
                int found = 0;
                for (int i = 0; i < BOARD_SIZE && !found; i++) {
                    for (int j = 0; j < BOARD_SIZE && !found; j++) {
                        if (is_valid_move(i, j)) {
                            game.board[i][j] = game.my_color;
                            printf("%d %d\n", i, j);
                            fflush(stdout);
                            found = 1;
                        }
                    }
                }
            }
            
            clock_t turn_end = clock();
            game.total_time_used += ((double)(turn_end - turn_start)) / CLOCKS_PER_SEC;
            
        } else if (strcmp(command, "END") == 0) {
            scanf("%d", &x); // Read result but don't need to process
            break;
        }
    }
    
    return 0;
}