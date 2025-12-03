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

* ✅ **Menu System:** Functional LCD menu with navigation (Start Game, Settings, Highscore) controlled via Joystick.
* ✅ **Game Engine:** Core loop implemented with vertical scrolling map logic.
* ✅ **Controls:** * Steering: Joystick Left/Right works with wall collision prevention.
    * Acceleration: Ultrasonic sensor successfully mapped to game speed.
* ✅ **Audio:** Implemented "Theme Song" on start-up and dynamic engine sounds.
* ✅ **Graphics:** Matrix rendering logic updated to support 2-LED vehicle sizes (Player & Enemies).
* ✅ **Physics:** "Relative Speed" logic implemented (Overtaking mechanics).

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

##  Media

Video [demo](https://youtu.be/8h4KZu4q_a4)

Photo [1](https://github.com/dbogdanm/TaxiDriverArduino/blob/main/Checkpoint%201/Poza1.jpg?raw=true)

Photo [2](https://github.com/dbogdanm/TaxiDriverArduino/blob/main/Checkpoint%201/Poza2.jpg?raw=true)

Photo [3](https://github.com/dbogdanm/TaxiDriverArduino/blob/main/Checkpoint%201/Poza3.jpg?raw=true)

Photo [4](https://github.com/dbogdanm/TaxiDriverArduino/blob/main/Checkpoint%201/Poza4.jpg?raw=true)



---

