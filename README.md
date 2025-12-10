# TaxiDriverArduino
Taxi Driver - Matrix Project An Arduino-based endless runner game built for the Introduction to Robotics course. Features an 8x8 LED Matrix display with scrolling map logic, an LCD menu system, and a unique hands-free speed control mechanism using an ultrasonic sensor as a gas pedal.



#  Taxi Driver: The Matrix Edition

**UniBuc Robotics Project (2025-2026)** *An Arduino-based endless runner game with physical speed control.*

##  Description
**Taxi Driver** is an interactive arcade game where players navigate a taxi through heavy traffic on a busy boulevard.The project is built around an **8x8 LED Matrix**, but utilizes a logical map far larger than the display (scrolling mechanics), fulfilling the *16x16 logical functionality* requirement.

What sets this game apart is the **hands-free control system**: the car's speed is controlled by the physical distance of the player's hand from an ultrasonic sensor, acting as a virtual gas pedal.

---

##  Gameplay & Mechanics

### 1. The "Gas Pedal" (Ultrasonic Sensor)
Instead of a simple button, the game uses an **HC-SR04** sensor to control the speed of the map scrolling:
* **Hand Close (< 5cm):** Full Throttle (Map scrolls fast, ~100ms refresh).
* **Hand Mid-Range (10-20cm):** Cruising Speed.
* **Hand Removed:** Brake/Stop.

### 2. Relative Velocity Logic
The game simulates realistic physics relative to the player's speed:
* **Overtaking:** If you accelerate (hand close), you move faster than the traffic. Enemy cars appear to move *towards* you and you overtake them.
* **Being Left Behind:** If you brake (remove hand), the traffic maintains its speed, causing enemy cars to pull away (move *up* the screen).

### 3. Visuals & Audio
* **Player:** Rendered as a 2-LED vehicle on the bottom lanes.
* **Enemies:** Dynamic 2-LED vehicles that spawn on random lanes.
* **Audio:** A passive buzzer simulates engine RPM (low frequency ticking that speeds up with the map) and plays start-up melodies.

---

## Hardware Setup

The project is built on an **Arduino Uno** using the following components:

| Component | Pin Connection | Note |
| :--- | :--- | :--- |
| **LED Matrix (MAX7219)** | D12 (DIN), D11 (CLK), D10 (CS) | 8x8 Display |
| **LCD 1602** | D9 (RS), D8 (EN), D7, D4, D3, A2 | Parallel Connection |
| **Joystick (XY)** | A0 (VRx), A1 (VRy) | Steering (Left/Right) |
| **Joystick (Button)** | D2 | Selection / Menu |
| **Ultrasonic Sensor** | D6 (Trig), D5 (Echo) | Speed Control |
| **Buzzer** | D13 | Sound Effects |

---

##  Checkpoint 1 Status (Current Progress)

By **December 5th**, the following features have been fully implemented and tested:

*  **Menu System:** Functional LCD menu with navigation (Start Game, Settings, Highscore) controlled via Joystick.
*  **Game Engine:** Core loop implemented with vertical scrolling map logic.
*  **Controls:** * Steering: Joystick Left/Right works with wall collision prevention.
* Acceleration: Ultrasonic sensor successfully mapped to game speed.
*  **Audio:** Implemented "Theme Song" on start-up and dynamic engine sounds.
* **Graphics:** Matrix rendering logic updated to support 2-LED vehicle sizes (Player & Enemies).
*  **Physics:** "Relative Speed" logic implemented (Overtaking mechanics).

---

##  Future Roadmap 

The goal is to turn this engine into a complete product with progression and rewards:

1.  **The "Taxi" Element:**
    * Implement **Clients** (Blue dots on the sidewalk).
    * Logic to stop the car exactly next to a client to pick them up.
    * Delivery mechanic: Drop off client at a destination for points.
2.  **Score & Economy:**
    * Tracking money earned from fares.
    * Highscore system saved to **EEPROM**.
3.  **Progression:**
    * Traffic density increases as the score goes up.
4.  **Polish:**
    * Animations for picking up clients.
    * Settings menu implementation (LCD Brightness, Sound On/Off).

---

## Media

[Video Demo](https://youtu.be/8h4KZu4q_a4)
<br>

### Project Photos

![Photo 1 - Taxi Driver Arduino Setup](https://github.com/dbogdanm/TaxiDriverArduino/blob/main/Checkpoint%201/Poza1.jpg?raw=true)
<br>

![Photo 2 - LCD Menu](https://github.com/dbogdanm/TaxiDriverArduino/blob/main/Checkpoint%201/Poza2.jpg?raw=true)
<br>

![Photo 3 - Ultrasonic Sensor as Gas Pedal](https://github.com/dbogdanm/TaxiDriverArduino/blob/main/Checkpoint%201/Poza3.jpg?raw=true)
<br>

![Photo 4 - Full Breadboard Layout](https://github.com/dbogdanm/TaxiDriverArduino/blob/main/Checkpoint%201/Poza4.jpg?raw=true)


# Checkpoint 2: The Gameplay Update

**Status:** Completed
**Focus:** Implementing the core "Taxi" mechanics, objectives, time constraints, and scoring system.

## Description
In this second phase, the project has evolved from a simple endless runner engine into a fully playable arcade game. The "Taxi" mechanic is now fully functional: the player acts against the clock to locate passengers, pick them up, and deliver them to specific destinations to earn points and extend their time.

## New Features Implemented

### 1. Passenger System (Pick-up & Drop-off)
The core loop of the game is now implemented with a specific interaction mechanic:
* **Spawning:** Passengers appear randomly on the left or right sidewalk (columns 0 or 7 on the Matrix).
* **The "Stop" Mechanic:** To pick up or drop off a passenger, the player must align the car with the target and **come to a complete stop** (by removing their hand from the ultrasonic sensor) while adjacent to the objective.
* **Navigation:** Once a passenger is onboard, a random destination distance is generated.

### 2. Time Attack Mode
* **Countdown:** The game now starts with **60 seconds** on the clock.
* **Risk vs. Reward:** The player must drive fast to reach the destination, but driving too fast makes it harder to stop precisely at the drop-off point.
* **Bonuses:** Successfully delivering a passenger grants a **+10 second bonus** to the timer, keeping the game alive.
* **Game Over:** The game ends when the timer hits 0 or the player crashes into traffic.

### 3. Smart UI (LCD HUD)
The LCD display has been upgraded to provide a dynamic navigation system:
* **Dynamic Navigation:** Arrows (`<-` or `->`) appear on the screen to indicate which side of the road the objective is on.
* **Distance Tracker:** Real-time distance display (in virtual meters) to the next pickup or drop-off.
* **Status Indicators:** Text changes between "Get" (find passenger) and "Drop" (drive to destination).
* **Stats:** Displays current Money and Time Remaining.

### 4. Scoring & Persistence
* **Economy:** Money is earned per successful delivery.
* **High Score:** The game now saves the highest score to the Arduino's **EEPROM**. The high score persists even after the board is powered off or reset.

### 5. Audio Feedback
New sound effects have been added to enhance game feel:
* **Success Chime:** A positive melody plays when money is earned.
* **Timer Warning:** Ticking sound intensifies when time is running low (< 10 seconds).
* **Game Over:** A distinct melody for running out of time versus crashing.

---

## Logic Flow Updates

The code structure has been refined into a robust **State Machine**:

1.  **INTRO:** Splash screen ("Taxi Driver") and startup tune.
2.  **MENU:**
    * *Start Game:* Initializes variables (Time=60, Score=0).
    * *High Score:* Reads and displays value from EEPROM.
    * *Reset Score:* Clears EEPROM data.
3.  **GAME LOOP:**
    * Calculates physics (Ultrasonic speed control).
    * Updates Timers (Game time & Movement ticks).
    * Checks for "Stop" condition at objectives.
    * Renders Matrix (Car, Enemies, Objectives).
4.  **GAME OVER:** Displays reason for loss (Crash/Time) and final score.

---

## Hardware Changes
*No changes to physical wiring since Checkpoint 1.* The code utilizes the existing setup (Joystick, Ultrasonic, LCD, Matrix) but maximizes their potential through software logic.


## Media

[Video Demo](https://www.youtube.com/watch?v=SsuHdYgD8D8)
<br>

### Project Photos

![Photo 1 - Taxi Driver Arduino Setup](https://github.com/dbogdanm/TaxiDriverArduino/blob/main/Checkpoint%202/poza1.jpg?raw=true)
<br>

![Photo 2 - LCD Menu](https://github.com/dbogdanm/TaxiDriverArduino/blob/main/Checkpoint%202/poza2.jpg?raw=true)
<br>

![Photo 3 - Ultrasonic Sensor as Gas Pedal](https://github.com/dbogdanm/TaxiDriverArduino/blob/main/Checkpoint%202/poza3.jpg?raw=true)
<br>




