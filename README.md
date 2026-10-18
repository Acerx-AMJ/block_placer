# Block Placer
A game heavily inspired by Tetris featuring single player mode, co-op mode and a versus mode. Play with friends on the same machine or challenge yourself and go alone. Try the game out for free on [Itch.io](https://acerxamj.itch.io/block-placer).

## Features
- Single player mode
- Co-op mode
- Versus mode
- Rotation, clearing and other mechanics found in Tetris games

## Controls
- A/D/left/right - Move tetromino left/right
- W/up - Rotate tetromino
- S/down - Drop tetromino by one tile
- space/enter - Send tetromino down
- esc - Pause the game

## Building
This project depends on [Raylib](https://www.raylib.com/) and C++17. You can compile it using CMake:
```bash
cmake -B build
cmake --build build
```

The executable will be located in `build/block`. Or using your favorite compiler:
```bash
g++ -std=c++17 -Iinclude src/*.cpp -o block -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
```

The assets folder must be in the same folder in which the executable is ran. The game will create two extra .data files in the same folder as it is ran.

## Installing
You can install the game for free on [Itch.io](https://acerxamj.itch.io/block-placer).

## Credits
This project, including all assets, was made by Daniel Vishnevsky and is licensed under MIT License.
