# Gobang AI Competition Program

A competitive Gobang (Five-in-a-Row) AI program implemented in C for tournament play. This program is designed to compete against other AI programs in a 12×12 board environment with strict time and memory constraints.

## 🎯 Project Overview

This project implements a sophisticated Gobang AI that uses advanced game-playing algorithms to make strategic decisions. The program is designed for competitive programming environments where multiple AI programs compete against each other in tournament-style matches.

## 🏆 Key Features

### Core AI Algorithms
- **Minimax Algorithm**: Implements the classic minimax decision-making algorithm for optimal play
- **Alpha-Beta Pruning**: Significantly improves search efficiency by eliminating unnecessary branches
- **Iterative Deepening**: Gradually increases search depth within time constraints
- **Move Ordering**: Prioritizes promising moves to improve pruning effectiveness

### Strategic Heuristics
- **Position Evaluation**: Advanced scoring system that considers:
  - Line formations (2, 3, 4, and 5 in a row)
  - Open vs. blocked positions
  - Central positioning advantages
  - Pattern recognition for common tactical motifs

- **Threat Detection**: 
  - Immediate win detection and execution
  - Blocking opponent's winning moves
  - Multiple threat analysis

### Performance Optimizations
- **Efficient Move Generation**: Only considers moves adjacent to existing stones
- **Time Management**: Carefully manages the 2-second per move and 90-second total time limits
- **Memory Efficiency**: Stays well within the 350MB memory constraint
- **Single-threaded Design**: Optimized for single-core execution environment

## 📋 Competition Rules Compliance

The program strictly adheres to the competition requirements:

### Board and Game Rules
- 12×12 board size
- Standard Gobang rules (5 in a row to win)
- Black moves first, alternating turns
- Handles both BLACK (1) and WHITE (2) play

### Technical Constraints
- ✅ Single C file implementation
- ✅ 2-second response time per move
- ✅ 90-second total time limit per game
- ✅ 350MB memory limit
- ✅ Standard input/output communication protocol

### Communication Protocol
Supports all required commands:
- `START [FIELD]`: Initialize game with color assignment
- `PLACE X Y`: Process opponent's move
- `TURN`: Calculate and output next move
- `END [FIELD]`: Handle game termination
- `DEBUG [MESSAGE]`: Optional debugging output

## 🔧 Technical Implementation

### Algorithm Architecture
```
Main Game Loop
├── Command Processing (START, PLACE, TURN, END)
├── Board State Management
├── Move Generation & Validation
├── Minimax Search with Alpha-Beta Pruning
│   ├── Position Evaluation Function
│   ├── Win/Loss Detection
│   ├── Time Limit Management
│   └── Move Ordering Heuristics
└── Output Formatting & Debugging
```

### Evaluation Function Components
1. **Line Scoring**: Exponential scoring for longer lines
2. **Blocking Analysis**: Penalty for blocked positions
3. **Position Control**: Center and key position weighting
4. **Threat Assessment**: Immediate tactical considerations

## 🚀 Usage Instructions

### Compilation
```bash
gcc -o gobang_ai gobang_ai.c -O2
```

### Running the Program
The program communicates via standard input/output:
```bash
./gobang_ai
```

### Sample Interaction
```
START 1
OK
TURN
5 5
PLACE 6 6
TURN
5 6
END 1
```

## 📊 Performance Characteristics

### Time Complexity
- **Best Case**: O(b^d) where b is branching factor, d is depth
- **With Alpha-Beta**: Typically O(b^(d/2)) due to pruning
- **Practical Performance**: Searches 6-8 ply within time limits

### Space Complexity
- **Board State**: O(1) - fixed 12×12 array
- **Search Stack**: O(d) - linear in search depth
- **Move Generation**: O(b) - linear in branching factor

### Strategic Strength
- Strong opening play with center-focused strategy
- Excellent tactical vision for immediate threats
- Balanced positional and tactical evaluation
- Robust endgame handling

## 🛡️ Robustness Features

### Error Handling
- Input validation for all moves
- Boundary checking for board positions
- Graceful handling of invalid commands
- Fallback strategies for time pressure

### Time Management
- Conservative time allocation (1.8s safety margin)
- Progressive search deepening
- Emergency move selection under time pressure

### Memory Management
- Static memory allocation for predictable usage
- No dynamic memory allocation to prevent leaks
- Efficient data structures for minimal footprint

## 🔬 Algorithm Analysis

### Minimax with Alpha-Beta Pruning
The core algorithm uses the minimax principle to find optimal moves by assuming both players play perfectly. Alpha-beta pruning eliminates branches that cannot affect the final decision, typically reducing the search space by approximately 50%.

### Position Evaluation Heuristics
The evaluation function combines multiple factors:
- **Material**: Count and arrangement of pieces
- **Mobility**: Available move options
- **Control**: Influence over key board areas
- **Threats**: Immediate tactical opportunities

### Move Ordering Strategy
Moves are ordered by potential strength to maximize pruning:
1. Winning moves (immediate victory)
2. Blocking moves (prevent opponent victory)
3. Threat-creating moves
4. Positionally strong moves
5. Remaining legal moves

## 📈 Expected Performance

In tournament play, this AI should demonstrate:
- **Strong Opening**: Consistent center-focused development
- **Tactical Accuracy**: Rarely misses immediate threats
- **Positional Understanding**: Builds long-term advantages
- **Endgame Precision**: Converts winning positions efficiently

## 🎓 Educational Value

This implementation serves as an excellent example of:
- Game tree search algorithms
- Heuristic evaluation design
- Time-constrained optimization
- Competitive programming techniques
- AI strategy development

## 📄 License

This project is open source and available for educational and competitive use.

## 🤝 Contributing

While this is designed for individual competition, educational improvements and optimizations are welcome for learning purposes.

---

**Note**: This AI is designed specifically for the competition environment described in the rules document. It prioritizes robustness and compliance with competition constraints over absolute playing strength.