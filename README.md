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
**Project Name:** Taxi Driver - The Matrix Edition
**Status:** 100% Completed (Final Release)

##  Overview
This is the final submission for the Introduction to Robotics Matrix Project. **Taxi Driver 1.3** is now a fully featured arcade game that combines a ∞ x 16 logical game world with physical interaction. The project has evolved from a simple prototype into a polished product with a complete economy, persistent high scores, settings, and dynamic audio-visual feedback.

##  Final Features Implemented

### 1. The "Taxi" Gameplay Loop (Complete)
The core mechanic is fully functional and bug-free:
* **Mission System:** Players must locate passengers (dots on the sidewalk), come to a complete stop to pick them up, and drive them to a randomly generated destination distance.
* **Risk/Reward:** The faster you drive, the harder it is to stop exactly at the pickup point. Missing a stop results in a time and money penalty.
* **Timer & Bonuses:** The game runs on a 60-second timer. Successful drop-offs grant a **+10s time bonus** and **$50**, keeping the game loop alive.

### 2. Logical ∞ x 16 (16x16 as required) Matrix on 8x8 Hardware
As per the advanced requirements, the game world exists on a **16x14 grid** (logic) displayed on an **8x8 Matrix** (physical):
* **Camera Panning:** When the player steers to the far right (lane 8+), the viewport shifts ("pans"), rendering the second half of the road.
* **Blind Spot Warning:** To compensate for the limited view, an indicator on the LCD (`!>` or `<!`) warns the player if a car is approaching from the non-visible side of the map.

### 3. Persistent Data (EEPROM)
The game now features a fully working saving system:
* **High Scores:** The top 3 scores are saved in non-volatile memory.
* **Name Entry:** When a high score is achieved, the player enters a 3-letter name (Arcade style: A-Z selection) which is saved alongside the score.
* **Settings Memory:** Brightness preferences and Sound settings are remembered even after the Arduino is powered off.

### 4. Polished Menus & UX
* **Settings Menu:** Allows adjustment of **Matrix Brightness**, **LCD Backlight Brightness** (PWM), and **Sound ON/OFF**.
* **Custom Graphics:** The menu uses custom bitmaps (Play icon, Cup icon, Settings gear) 
* **Intro Sequence:** A startup animation and melody ("Taxi Driver v1.3").

---

##  Controls & Mechanics

| Input Component | Action | Function |
| :--- | :--- | :--- |
| **Ultrasonic Sensor** | **Gas Pedal** | **Hand < 5cm:** Turbo Speed (Overtake traffic)<br>**Hand 15cm:** Cruise Speed<br>**No Hand:** Brake/Stop (Required for pickup) |
| **Joystick (X-Axis)** | **Steering** | Move Left/Right. Moving past the edge shifts the camera. |
| **Joystick (Button)** | **Select** | Confirm menu selection / Input Name / Reset Game |
| **Joystick (Y-Axis)** | **Menu Nav** | Scroll through menu options or letters. |

**Traffic Physics:**
The game simulates **Relative Velocity**.
* *Accelerating:* You move faster than traffic -> You overtake cars (they move down).
* *Stopped:* Traffic moves at normal speed -> Cars pull away (they move up).

---

##  Final Hardware Configuration

The code is configured for **Arduino Uno** with the following pinout:

| Component | Pin (Arduino) | Description |
| :--- | :--- | :--- |
| **Ultrasonic Trig** | **D3** | Trigger pin for distance sensing |
| **Ultrasonic Echo** | **D5** | Echo pin for distance sensing |
| **LCD RS, EN** | **D9, D8** | LCD Control Pins |
| **LCD Data (D4-D7)** | **D7, D4, A3, A2** | LCD Data Lines |
| **LCD Backlight** | **D6** | PWM Control for brightness |
| **Matrix DIN** | **D12** | Data In |
| **Matrix CLK** | **D11** | Clock |
| **Matrix CS/LOAD** | **D10** | Chip Select |
| **Joystick VRx** | **A0** | Steering |
| **Joystick VRy** | **A1** | Menu Navigation |
| **Joystick SW** | **D2** | Button (Pull-up) |
| **Buzzer** | **D13** | Audio Output |

---

##  Final Deliverables

**Photos:** ![Photo 1 ](https://github.com/dbogdanm/TaxiDriverArduino/blob/main/Checkpoint3-Final-/Checkpoint3poza1.jpg?raw=true)
<br> 
![Photo 2 ](https://github.com/dbogdanm/TaxiDriverArduino/blob/main/Checkpoint3-Final-/Checkpoint3poza2.jpg?raw=true)
![Photo 3 ](https://github.com/dbogdanm/TaxiDriverArduino/blob/main/Checkpoint3-Final-/Checkpoint3poza3.jpg?raw=true)


**Video Demonstration:**  

[Video Gameplay Demo](https://www.youtube.com/watch?v=SMQctiJeqF4)
<br>  
[Video Blindspot Assist Demo](https://youtube.com/shorts/wbJxypuzjMo)

### How to Run
1.  Install the `LedControl` and `LiquidCrystal` libraries in Arduino IDE.
2.  Connect the components according to the table above.
3.  Upload `TaxiDriver_Final.ino`.

---

**Datasheets USED**

1. [MAX7219](https://www.alldatasheet.com/datasheet-pdf/view/73745/MAXIM/MAX7219.html)


