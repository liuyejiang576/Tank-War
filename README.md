# Tank War Game

A simplified two-player Tank War game implemented in C++ with support for Player vs Player (PVP), Player vs Environment (PVE), and Demo (AI vs AI) modes.

### Compilation and Running
```bash
make

# Or build debug version
make debug

# Or build optimized release version
make release

# Show help
./tankwar --help

# Run in PVP mode (default)
./tankwar

# Run in PVE mode with custom life points
./tankwar -m PVE -p 10

# Run in DEMO mode with custom log file
./tankwar --mode=DEMO --log-file=demo.log

# Full example
./tankwar -m PVE --initial-life=8 --log-file=mygame.log
```

### Command-line Options

- `-h` | `--help`: Print help message and exit
- `--log-file <file>`: Log the game process to a file (Default: tankwar.log)
- `-m <mode>` | `--mode=<mode>`: Specify game mode (PVP/PVE/DEMO) (Default: PVP)
- `-p <point>` | `--initial-life=<point>`: Specify initial life points (Default: 5)

### Game Rules
1. **Movement**: Each tank moves 1 meter per turn
2. **Shooting**: Tanks shoot bullets every 3 turns
3. **Bullet Speed**: Bullets move 2 meters per turn
4. **Damage**: Bullets deal 2 damage points
5. **Map Shrinking**: Map shrinks every 16 turns
6. **Out of Bounds**: Tanks outside map lose 1 life point per turn
7. **Victory Conditions**: 
   - Tank with 0 life points loses
   - If both tanks reach 0 life in same turn: draw
   - Tank collision: higher life points wins

### Input Format
- Initial tank setup: X Y Direction (0=Left, 1=Up, 2=Right, 3=Down)
- Moves: 0=Forward, 1=Turn Left, 2=Turn Right

### Map Display
- `A`/`B`: Tank positions
- `*`: Bullet positions  
- ` `: Empty space within map
- `-`: Outside map boundary

### Core Classes

**Tank Class**
- Manages tank position, direction, and life points
- Handles movement (forward, turn left, turn right)
- Manages shooting mechanics with cooldown timer

**Bullet Class**
- Manages bullet position and movement
- Handles collision detection with tanks
- Automatic cleanup when out of bounds

**GameEngine Class**
- Central game coordinator
- Manages game state and turn processing
- Handles win/lose conditions
- Coordinates between all game components

**AIPlayer Class**
- Implements AI decision-making algorithms
- Supports multiple difficulty levels
- Includes threat assessment and path planning

**GameMap Class**
- Manages map boundaries and shrinking mechanics
- Handles out-of-bounds damage calculation
- Tracks turn count for map shrinking

### Log File Format
The game generates detailed logs including:
- Turn information
- Tank movements and positions
- Bullet creation and movement
- Collision events
- Damage dealt
- Map shrinking events
- Game results