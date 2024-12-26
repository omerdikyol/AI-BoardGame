# AI-BoardGame

This project implements a two-player board game where a human player competes against an AI. The game is played on a 7x7 board, and the objective is to outmaneuver the opponent by making strategic moves. The AI uses a heuristic-based minimax algorithm to make decisions.

## Features
- **7x7 Board**: The game is played on a 7x7 grid with rows labeled `a-g` and columns labeled `1-7`.
- **Two Players**: Player 1 (`X`) and Player 2 (`O`).
- **Turn-Based Gameplay**: Players take turns moving their pieces to adjacent squares.
- **AI Opponent**: The AI uses a minimax algorithm with a heuristic function to make optimal moves.
- **Customizable Settings**: The number of pieces and the turn limit can be set by the user at the start of the game.
- **Move Validation**: Ensures that all moves are valid (e.g., no moving outside the board or to occupied squares).

## How to Use

### Prerequisites
- A C compiler (e.g., GCC).
- A terminal or command prompt.

### Steps
1. Clone the repository:
   ```bash
   git clone https://github.com/omerdikyol/AI-BoardGame.git
   cd AI-BoardGame
   ```
2. Compile the program:
   ```bash
   gcc code.c -o boardgame
   ```
3. Run the game:
   ```bash
   ./boardgame
   ```
4. Follow the on-screen instructions to set up the game (number of pieces, turn limit, and player choice).
5. Play the game by entering your moves when prompted.
 
## Example Gameplay
1. Initialization:
   ```bash
    Enter the number of pieces for each player: 5
    Enter the turn limit for the game: 10
    Do you want to be Player 1 or 2? (Enter 1 or 2): 1
   ```
2. Compile the program:
   ```bash
    1  2  3  4  5  6  7
    a | . | . | . | . | . | . | . |
    +---+---+---+---+---+---+---+
    b | . | X | . | . | . | . | . |
    +---+---+---+---+---+---+---+
    c | . | . | O | . | . | . | . |
    +---+---+---+---+---+---+---+
    d | . | . | . | . | . | . | . |
    +---+---+---+---+---+---+---+
    e | . | . | . | . | . | . | . |
    +---+---+---+---+---+---+---+
    f | . | . | . | . | . | . | . |
    +---+---+---+---+---+---+---+
    g | . | . | . | . | . | . | . |
    +---+---+---+---+---+---+---+
   ```
3. Player Move:
   ```bash
    Choose piece to move (e.g., b2): b2
    Choose the new position for b2 (e.g., c2): c2
    Player X moves the piece at b2 to c2
   ```
4. AI Move:
   ```bash
    Computer moves the piece at c3 to c4
   ```
## Heuristic Function
The AI uses a heuristic function to evaluate moves based on:
- **Mobility**: Number of available moves.
- **Capture Potential**: Proximity to opponent pieces.
- **Center Control**: Control of the center of the board.
- **Safety**: Distance from opponent pieces.
- **Aggressiveness**: Proximity to opponent pieces for potential captures.

## Game End
The game ends when:
- One player has no valid moves left.
- The turn limit is reached.
The winner is determined by the number of valid moves each player has at the end of the game.

## Project Structure
- **`code.c`**: The main C file containing the game logic, AI, and user interface.

## License
This project is open-source and available under the MIT License.
