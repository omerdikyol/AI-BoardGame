#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>

#define BOARD_SIZE 7
#define PLAYER1 'X'
#define PLAYER2 'O'
#define MOBILITY_WEIGHT 1
#define CAPTURE_WEIGHT 2
#define BLOCK_WEIGHT 3
#define CENTER_CONTROL_WEIGHT 4
#define MINIMAX_DEPTH 5
#define SAFETY_WEIGHT 1.5
#define AGGRESSIVENESS_WEIGHT 1.25

typedef struct
{
    int from_row;
    int from_col;
    int to_row;
    int to_col;
} Move;

typedef struct
{
    char board[BOARD_SIZE][BOARD_SIZE];
    char current_player;
    int turn_limit;
} GameState;

// Function prototypes
void init_game_state(GameState *state, int num_pieces, int turn_limit);
void randomly_place_pieces(GameState *state, char player, int num_pieces);
void ask_user_settings(int *num_pieces, int *turn_limit, char *user_player);
void print_game_state(GameState *state);
int is_valid_move(GameState *state, Move move);
int has_valid_moves(GameState *state, char player);
int count_mobility(GameState *state, int row, int col);
int evaluate_move(GameState *state, Move move);
int evaluate_game_state(GameState *state);
void generate_moves(GameState *state, Move *moves, int *num_moves);
int minimax(GameState *state, int depth, int maximizingPlayer, int alpha, int beta);
Move get_computer_move(GameState *state);
void get_move_from_user(GameState *state, Move *move);
void make_move(GameState *state, Move move);
void print_move(char player, Move move);
char get_winner(GameState *state);
int is_game_over(GameState *state);
void play_game(GameState *state, char user_player);
int count_opponent_nearby(GameState *state, int row, int col);
int count_proximity_to_opponent(GameState *state, int row, int col);

void init_game_state(GameState *state, int num_pieces, int turn_limit)
{
    for (int i = 0; i < BOARD_SIZE; i++)
    {
        for (int j = 0; j < BOARD_SIZE; j++)
        {
            state->board[i][j] = '.';
        }
    }
    state->turn_limit = turn_limit;
    state->current_player = PLAYER1;
}

void randomly_place_pieces(GameState *state, char player, int num_pieces)
{
    int placed = 0;
    while (placed < num_pieces)
    {
        int row = rand() % BOARD_SIZE;
        int col = rand() % BOARD_SIZE;
        if (state->board[row][col] == '.')
        {
            state->board[row][col] = player;
            placed++;
        }
    }
}

void ask_user_settings(int *num_pieces, int *turn_limit, char *user_player)
{
    printf("Enter the number of pieces for each player: ");
    scanf("%d", num_pieces);
    printf("Enter the turn limit for the game: ");
    scanf("%d", turn_limit);
    printf("Do you want to be Player 1 or 2? (Enter 1 or 2): ");
    scanf(" %c", user_player);
    *user_player = (*user_player == '1') ? PLAYER1 : PLAYER2;
}

void print_game_state(GameState *state)
{
    printf("  ");
    for (int i = 0; i < BOARD_SIZE; i++)
    {
        printf("  %d ", i + 1);
    }
    printf("\n");

    for (int i = 0; i < BOARD_SIZE; i++)
    {
        printf("%c ", 'a' + i);

        for (int j = 0; j < BOARD_SIZE; j++)
        {
            printf("| %c ", state->board[i][j]);
        }
        printf("|\n");

        printf("  ");
        for (int j = 0; j < BOARD_SIZE; j++)
        {
            printf("+---");
        }
        printf("+\n");
    }
}

int is_valid_move(GameState *state, Move move)
{
    if (move.from_row < 0 || move.from_row >= BOARD_SIZE ||
        move.from_col < 0 || move.from_col >= BOARD_SIZE ||
        move.to_row < 0 || move.to_row >= BOARD_SIZE ||
        move.to_col < 0 || move.to_col >= BOARD_SIZE)
    {
        return 0;
    }

    if (state->board[move.from_row][move.from_col] != state->current_player)
    {
        return 0;
    }

    if (state->board[move.to_row][move.to_col] != '.')
    {
        return 0;
    }

    if (!((move.from_row == move.to_row && abs(move.from_col - move.to_col) == 1) ||
          (move.from_col == move.to_col && abs(move.from_row - move.to_row) == 1)))
    {
        return 0;
    }

    return 1;
}

int has_valid_moves(GameState *state, char player)
{
    Move move;
    for (int i = 0; i < BOARD_SIZE; i++)
    {
        for (int j = 0; j < BOARD_SIZE; j++)
        {
            if (state->board[i][j] == player)
            {
                int directions[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
                for (int d = 0; d < 4; d++)
                {
                    move.from_row = i;
                    move.from_col = j;
                    move.to_row = i + directions[d][0];
                    move.to_col = j + directions[d][1];
                    if (is_valid_move(state, move))
                    {
                        return 1;
                    }
                }
            }
        }
    }
    return 0;
}

int count_mobility(GameState *state, int row, int col)
{
    int mobility = 0;
    int directions[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
    for (int d = 0; d < 4; d++)
    {
        int new_row = row + directions[d][0];
        int new_col = col + directions[d][1];
        if (new_row >= 0 && new_row < BOARD_SIZE && new_col >= 0 && new_col < BOARD_SIZE)
        {
            if (state->board[new_row][new_col] == '.')
            {
                mobility++;
            }
        }
    }
    return mobility;
}

int evaluate_move(GameState *state, Move move)
{
    char opponent = (state->current_player == PLAYER1) ? PLAYER2 : PLAYER1;
    int value = 0;
    int row = move.to_row, col = move.to_col;

    value += MOBILITY_WEIGHT * count_mobility(state, row, col);

    if (row > 0 && state->board[row - 1][col] == opponent)
        value += CAPTURE_WEIGHT;
    if (row < BOARD_SIZE - 1 && state->board[row + 1][col] == opponent)
        value += CAPTURE_WEIGHT;
    if (col > 0 && state->board[row][col - 1] == opponent)
        value += CAPTURE_WEIGHT;
    if (col < BOARD_SIZE - 1 && state->board[row][col + 1] == opponent)
        value += CAPTURE_WEIGHT;

    if (row > 0 && state->board[row - 1][col] == state->current_player)
        value += BLOCK_WEIGHT;
    if (row < BOARD_SIZE - 1 && state->board[row + 1][col] == state->current_player)
        value += BLOCK_WEIGHT;
    if (col > 0 && state->board[row][col - 1] == state->current_player)
        value += BLOCK_WEIGHT;
    if (col < BOARD_SIZE - 1 && state->board[row][col + 1] == state->current_player)
        value += BLOCK_WEIGHT;

    if (row == BOARD_SIZE / 2 || col == BOARD_SIZE / 2)
        value += CENTER_CONTROL_WEIGHT;

    return value;
}
int evaluate_game_state(GameState *state)
{
    int score = 0;
    int center_row = BOARD_SIZE / 2;
    int center_col = BOARD_SIZE / 2;

    for (int i = 0; i < BOARD_SIZE; i++)
    {
        for (int j = 0; j < BOARD_SIZE; j++)
        {
            if (state->board[i][j] == PLAYER1 || state->board[i][j] == PLAYER2)
            {
                int pieceValue = (state->board[i][j] == state->current_player) ? 1 : -1;

                pieceValue += MOBILITY_WEIGHT * count_mobility(state, i, j);

                pieceValue += CENTER_CONTROL_WEIGHT / (abs(center_row - i) + 1) / (abs(center_col - j) + 1);

                pieceValue -= SAFETY_WEIGHT * count_opponent_nearby(state, i, j);

                pieceValue += AGGRESSIVENESS_WEIGHT * count_proximity_to_opponent(state, i, j);

                score += pieceValue;
            }
        }
    }

    return score;
}

int count_opponent_nearby(GameState *state, int row, int col)
{
    int count = 0;
    char opponent = (state->board[row][col] == PLAYER1) ? PLAYER2 : PLAYER1;

    for (int i = -1; i <= 1; i++)
    {
        for (int j = -1; j <= 1; j++)
        {
            int new_row = row + i;
            int new_col = col + j;
            if (new_row >= 0 && new_row < BOARD_SIZE && new_col >= 0 && new_col < BOARD_SIZE)
            {
                if (state->board[new_row][new_col] == opponent)
                {
                    count++;
                }
            }
        }
    }

    return count;
}

int count_proximity_to_opponent(GameState *state, int row, int col)
{
    int count = 0;
    char opponent = (state->board[row][col] == PLAYER1) ? PLAYER2 : PLAYER1;

    for (int i = -2; i <= 2; i++)
    {
        for (int j = -2; j <= 2; j++)
        {
            if (abs(i) <= 1 && abs(j) <= 1)
                continue;
            int new_row = row + i;
            int new_col = col + j;
            if (new_row >= 0 && new_row < BOARD_SIZE && new_col >= 0 && new_col < BOARD_SIZE)
            {
                if (state->board[new_row][new_col] == opponent)
                {
                    count++;
                }
            }
        }
    }

    return count;
}

void generate_moves(GameState *state, Move *moves, int *num_moves)
{
    *num_moves = 0;
    for (int i = 0; i < BOARD_SIZE; i++)
    {
        for (int j = 0; j < BOARD_SIZE; j++)
        {
            if (state->board[i][j] == state->current_player)
            {
                int directions[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
                for (int d = 0; d < 4; d++)
                {
                    Move move;
                    move.from_row = i;
                    move.from_col = j;
                    move.to_row = i + directions[d][0];
                    move.to_col = j + directions[d][1];
                    if (is_valid_move(state, move))
                    {
                        moves[*num_moves] = move;
                        (*num_moves)++;
                    }
                }
            }
        }
    }
}

int minimax(GameState *state, int depth, int maximizingPlayer, int alpha, int beta)
{
    if (depth == 0 || is_game_over(state))
    {
        return evaluate_game_state(state);
    }

    Move moves[BOARD_SIZE * BOARD_SIZE];
    int num_moves;
    generate_moves(state, moves, &num_moves);

    if (maximizingPlayer)
    {
        int maxEval = INT_MIN;
        for (int i = 0; i < num_moves; i++)
        {
            GameState newState = *state;
            make_move(&newState, moves[i]);
            newState.current_player = (newState.current_player == PLAYER1) ? PLAYER2 : PLAYER1;
            int eval = minimax(&newState, depth - 1, 0, alpha, beta);
            if (eval > maxEval)
            {
                maxEval = eval;
            }
            if (eval > alpha)
            {
                alpha = eval;
            }
            if (beta <= alpha)
            {
                break;
            }
        }
        return maxEval;
    }
    else
    {
        int minEval = INT_MAX;
        for (int i = 0; i < num_moves; i++)
        {
            GameState newState = *state;
            make_move(&newState, moves[i]);
            newState.current_player = (newState.current_player == PLAYER1) ? PLAYER2 : PLAYER1;
            int eval = minimax(&newState, depth - 1, 1, alpha, beta);
            if (eval < minEval)
            {
                minEval = eval;
            }
            if (eval < beta)
            {
                beta = eval;
            }
            if (beta <= alpha)
            {
                break;
            }
        }
        return minEval;
    }
}

Move get_computer_move(GameState *state)
{
    Move bestMove;
    int depth = 1;
    int bestValue;

    while (depth <= MINIMAX_DEPTH)
    {
        bestValue = INT_MIN;

        Move moves[BOARD_SIZE * BOARD_SIZE];
        int num_moves;
        generate_moves(state, moves, &num_moves);

        for (int i = 0; i < num_moves; i++)
        {
            GameState newState = *state;
            make_move(&newState, moves[i]);
            newState.current_player = (newState.current_player == PLAYER1) ? PLAYER2 : PLAYER1;
            int value = minimax(&newState, depth - 1, 0, INT_MIN, INT_MAX);
            if (value > bestValue)
            {
                bestValue = value;
                bestMove = moves[i];
            }
        }

        depth++;
    }

    return bestMove;
}
void get_move_from_user(GameState *state, Move *move)
{
    char from_row_char, to_row_char;
    printf("Choose piece to move (e.g., b2): ");
    scanf(" %c%d", &from_row_char, &move->from_col);
    move->from_row = from_row_char - 'a';
    move->from_col--;

    printf("Choose the new position for %c%d (e.g., c2): ", from_row_char, move->from_col + 1);
    scanf(" %c%d", &to_row_char, &move->to_col);
    move->to_row = to_row_char - 'a';
    move->to_col--;

    if (!is_valid_move(state, *move))
    {
        printf("Invalid move. Please try again.\n");
        get_move_from_user(state, move);
    }
}

void make_move(GameState *state, Move move)
{
    state->board[move.to_row][move.to_col] = state->board[move.from_row][move.from_col];
    state->board[move.from_row][move.from_col] = '.';
}

void print_move(char player, Move move)
{
    printf("Player %c moves the piece at %c%d to %c%d\n",
           player, move.from_row + 'a', move.from_col + 1, move.to_row + 'a', move.to_col + 1);
}

int count_available_moves(GameState *state, char player)
{
    int available_moves = 0;
    Move move;
    // print the player
    printf("Player %c has the following moves:\n", player);

    state->current_player = player;
    for (int i = 0; i < BOARD_SIZE; i++)
    {
        for (int j = 0; j < BOARD_SIZE; j++)
        {
            if (state->board[i][j] == player)
            {
                int directions[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
                for (int d = 0; d < 4; d++)
                {
                    move.from_row = i;
                    move.from_col = j;
                    move.to_row = i + directions[d][0];
                    move.to_col = j + directions[d][1];
                    if (is_valid_move(state, move))
                    {
                        available_moves++;
                    }
                }
            }
        }
    }

    return available_moves;
}

char get_winner(GameState *state)
{
    int player1_moves = count_available_moves(state, PLAYER1);
    printf("Player 1 has %d moves left\n", player1_moves);
    int player2_moves = count_available_moves(state, PLAYER2);

    // print the counts
    printf("Player 2 has %d moves left\n", player2_moves);

    if (player1_moves > player2_moves)
        return PLAYER1;
    else if (player2_moves > player1_moves)
        return PLAYER2;
    else
        return 'D';
}

int is_game_over(GameState *state)
{
    if (state->turn_limit <= 0)
        return 1;

    if (!has_valid_moves(state, PLAYER1) && !has_valid_moves(state, PLAYER2))
        return 1;

    return 0;
}

void play_game(GameState *state, char user_player)
{
    int total_turns = state->turn_limit * 2;
    while (!is_game_over(state) && total_turns > 0)
    {
        print_game_state(state);

        if (!has_valid_moves(state, state->current_player))
        {
            printf("Player %c has no valid moves. Skipping turn.\n", state->current_player);
        }
        else
        {
            Move move;
            if (state->current_player == user_player)
            {
                get_move_from_user(state, &move);
            }
            else
            {
                move = get_computer_move(state);
                if (move.from_row != -1)
                {
                    print_move(state->current_player, move);
                }
            }
            make_move(state, move);
        }

        state->current_player = (state->current_player == PLAYER1) ? PLAYER2 : PLAYER1;
        total_turns--;
    }

    print_game_state(state);
    char winner = get_winner(state);
    if (winner != 'D')
        printf("Winner: Player %c\n", winner);
    else
        printf("The game is a draw.\n");
}

int main()
{
    GameState state;
    int num_pieces, turn_limit;
    char user_player;

    srand(time(NULL));
    ask_user_settings(&num_pieces, &turn_limit, &user_player);
    init_game_state(&state, num_pieces, turn_limit);
    randomly_place_pieces(&state, PLAYER1, num_pieces);
    randomly_place_pieces(&state, PLAYER2, num_pieces);
    state.current_player = user_player;

    play_game(&state, user_player);

    return 0;
}