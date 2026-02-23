# Tank War Game

A simplified two-player Tank War game implemented in C++ with support for Player vs Player (PVP), Player vs Environment (PVE), and Demo (AI vs AI) modes.

## Compilation and Running

### On Windows (MinGW)
```powershell
mingw32-make

# Or build debug version
mingw32-make debug

# Show help
.\tankwar.exe --help

# Run in PVP mode (default)
.\tankwar.exe

# Run in PVE mode with custom life points
.\tankwar.exe -m PVE -p 10

# Run in DEMO mode with custom log file
.\tankwar.exe --mode=DEMO --log-file=demo.log
```

### On Linux/Mac
```bash
make

# Run
./tankwar --help
./tankwar -m PVE -p 10
```

## Command-line Options

| Option | Description | Default |
|--------|-------------|---------|
| `-h` or `--help` | Print help message and exit | - |
| `--log-file <file>` | Log the game process to a file | tankwar.log |
| `-m <mode>` or `--mode=<mode>` | Game mode (PVP/PVE/DEMO) | PVP |
| `-p <point>` or `--initial-life=<point>` | Initial life points | 5 |

## Game Rules

| Rule | Value |
|------|-------|
| **Map Size** | 16x16 (shrinks every 6 turns) |
| **Movement** | Each tank moves 1 cell per turn |
| **Shooting** | Tanks shoot bullets every 1 turn |
| **Bullet Speed** | Bullets move 2 cells per turn |
| **Bullet Damage** | 2 damage points |
| **Out of Bounds** | 1 damage point per turn |

### Victory Conditions
- Tank with 0 life points loses
- If both tanks reach 0 life in same turn: draw
- Tank collision: higher life points wins

## Input Format

### Initial Tank Setup
Enter `X Y Direction` where:
- X, Y: Position coordinates (0-15)
- Direction: 0=Left, 1=Up, 2=Right, 3=Down

### Movement Commands
Enter `0`, `1`, or `2`:
- 0 = Move Forward
- 1 = Turn Left (rotate 90° counter-clockwise)
- 2 = Turn Right (rotate 90° clockwise)

## Map Display

### Tank Symbols (with Direction)
| Tank | Left | Up | Right | Down |
|------|------|-----|-------|------|
| **Tank A** | `<` | `^` | `>` | `v` |
| **Tank B** | `L` | `U` | `R` | `D` |

### Other Symbols
- `-`: Horizontal bullet
- `|`: Vertical bullet
- ` `: Empty space within map
- `-`: Outside map boundary

## Core Classes

### Tank Class
- Manages tank position, direction, and life points
- Handles movement (forward, turn left, turn right)
- Manages shooting mechanics with cooldown timer

### Bullet Class
- Manages bullet position and movement
- Handles collision detection with tanks
- Automatic cleanup when out of bounds

### GameEngine Class
- Central game coordinator
- Manages game state and turn processing
- Handles win/lose conditions
- Coordinates between all game components

### AIPlayer Class
- Implements AI decision-making algorithms
- **Enhanced with smarter logic:**
  - Bullet dodge prediction
  - Positioning for attack angles
  - Priority: Safety > Dodge > Attack > Position > Center
- Includes threat assessment and path planning

### GameMap Class
- Manages map boundaries and shrinking mechanics
- Handles out-of-bounds damage calculation
- Tracks turn count for map shrinking

## Recent Updates

### v1.1 - UI & AI Improvements
- **Direction Display**: Tanks and bullets now show their facing direction
- **Smarter AI**: Added bullet dodge logic and positioning strategies
- **Windows Compatibility**: Fixed makefile for Windows (mingw32-make)

### Game Parameters
```cpp
const int INITIAL_MAP_SIZE = 16;     // Map is 16x16
const int SHOOT_INTERVAL = 1;        // Shoot every turn
const int MAP_SHRINK_INTERVAL = 6;   // Shrink every 6 turns
const int BULLET_DAMAGE = 2;         // 2 damage per hit
const int BULLET_SPEED = 2;          // Bullets move 2 cells/turn
```