# Word Shooter

A 2D arcade-style word puzzle game built in C++ using OpenGL and SDL2. Originally a first-semester university project, this game has been significantly refactored to include robust state management, particle physics, and dynamic progression.

## 🎮 Features
* **Dynamic Word Matching:** Shoots and matches bubbles to form valid English words using a comprehensive dictionary lookup.
* **Particle Physics System:** Custom gravity and velocity mechanics for satisfying bubble-bursting visual effects.
* **Progression & Tracking:** Persistent high scores saved to local storage, with dynamically scaling difficulty and bonus time rewards.
* **Audio Integration:** Continuous background music powered by SDL2 Mixer.
* **State Management:** Clean UI transitions between the Start Menu, active Gameplay, and Game Over screens.

## 🛠️ Prerequisites
This project is designed for Linux (Ubuntu) environments. You will need a standard GCC compiler and the following libraries installed:

```bash
sudo apt update
sudo apt install build-essential freeglut3-dev libgl1-mesa-dev libglu1-mesa-dev libsdl2-dev libsdl2-mixer-dev

```

## 🚀 Compilation & Execution

This project uses a `Makefile` for streamlined compilation.

1. **Compile the game:**
```bash
make

```


2. **Run the executable:**
```bash
./word-shooter

```


3. **Clean compiled binaries (optional):**
```bash
make clean

```



## 🕹️ Controls

* **Mouse Movement:** Aim the shooter.
* **Left-Click:** Shoot the alphabet bubble.
* **ESC:** Save your high score and exit the game.

## 👨‍💻 Author

**Nauman Rafay** 
