# Snake Game

A classic terminal-based Snake game written in C++ that runs in both Windows consoles and Linux terminals. The game renders entirely in the terminal using ANSI escape sequences (Linux) or the Windows Console API, with support for both manual play and an optional auto-play AI.

## Features

- **Retro terminal rendering** — the game draws directly to the console using cursor positioning and colors, no external graphics libraries required.
- **Cross-platform** — automatically selects the correct platform backend at compile time via `#ifdef _WIN32`:
  - `win/game.h` — Windows, uses `<conio.h>` and the Windows Console API.
  - `linux/game.h` — Linux, uses `<termios.h>`, `select()`, and ANSI escape codes.
- **Manual play** — control the snake with the keyboard (`WASD` or the arrow keys).
- **Auto-play AI** — an optional built-in AI (`SimpleAI`) that plays the game on its own, avoiding walls and its own body while chasing the fruit.
- **Score & record tracking** — each fruit is worth `10` points, and the best score is tracked across rounds.
- **Endless / looping mode** — the game restarts automatically after a game over, keeping your running record.
- **Dynamic fruit placement** — apples are always placed on a cell not currently occupied by the snake.

## Controls

| Key | Action |
| --- | ------ |
| `W` / `↑` | Move up |
| `A` / `←` | Move left |
| `S` / `↓` | Move down |
| `D` / `→` | Move right |
| `X` | Stop / pause the snake |

> When the `AI` macro is defined (see below), keyboard input is ignored and the AI plays automatically.

## Gameplay

- Guide the snake to eat the apple (`Q`) to grow longer and earn points.
- Each apple adds **10 points** to your score.
- The snake cannot reverse direction into itself — turning 180° is ignored.
- Game over occurs when the snake hits its own body. In looping mode the game then restarts and the higher of score/record is kept.
- You win if the snake fills the entire board (there is no valid cell left for a fruit).

## Configuration

The game's behavior is controlled by a few `#define` macros at the top of `main.cpp`:

| Macro | Effect when defined |
| ----- | ------------------- |
| `AI` | Enable automatic play (the `SimpleAI` logic in `Logic()`). When undefined, keyboard input is used. |
| `LOOP` | Restart the game automatically after death and keep a running record. When undefined, the program exits on game over. |
| `DEBUG` | Print extra debug info (fruit/head coordinates) on the score line instead of just the score. |

Board dimensions are set by the constants at the top of `main.cpp`:

```cpp
constexpr int width = 40, height = 20;
```

## Project Structure

```
snakegame/
├── main.cpp          # Game logic, rendering, and AI
├── win/game.h        # Windows backend (conio.h + Windows Console API)
├── linux/game.h      # Linux backend (termios + ANSI escapes)
├── README.md
└── .gitignore
```

## Building

### Windows (MSVC / cl.exe)

The VS Code configuration in `.vscode/c_cpp_properties.json` targets the MSVC toolchain with the C++23 standard. From a Developer Command Prompt:

```
cl /std:c++20 /EHsc main.cpp
```

This produces `main.exe` in the current directory.

### Linux (g++)

```
g++ -std=c++20 -o snake main.cpp
./snake
```

The Linux backend needs `<termios.h>`, which is part of POSIX and available on any standard Linux distribution. Note that terminal-based rendering requires a real (non-piped) terminal for correct display.

## How It Works

- `main()` initializes the terminal, calls `Setup()` to draw the border and place the first fruit, then runs the main loop, calling `Logic()` every **100 ms** (`sleep_ms(100)`).
- `Logic()` either reads keyboard input (`HandleInput()`) or runs the AI (`SimpleAI()`), then moves the snake and checks for collisions.
- Rendering is done by overwriting cells directly with `setCursorPos()` and color helpers, so the whole board does not need to be redrawn each frame.
- `PlaceFruit()` collects all unoccupied cells into a list and chooses one at random, guaranteeing a valid spawn location.
- The snake is stored as a `std::vector<std::pair<int,int>>` of segment coordinates; each move shifts the body and pushes the new head.

## Platforms & Tooling

- Windows: MSVC (`cl.exe`) / any compiler supporting `<conio.h>` and `<windows.h>`.
- Linux: g++ or clang with POSIX headers.
- Language standard: C++17 or newer recommended (the code uses `constexpr`, `std::pair`, `std::vector`).
