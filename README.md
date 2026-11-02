# Arduino LED Controller Firmware

An embedded C firmware project for controlling LED patterns and brightness using an Arduino Uno (ATmega328P). The project uses Timer1 interrupts, software PWM, and serial communication to control the LED through simple text commands.

## Project Overview

This project demonstrates fundamental embedded systems concepts, including hardware timer interrupts, UART communication, LED pattern generation, and software-based brightness control. It can be simulated using Wokwi or tested on compatible physical hardware.

## Features

- **Multiple LED Modes:** OFF, SOLID, SLOW, FAST, and HEART.
- **Timer Interrupts:** Timer1 CTC mode generates a 1 ms timer tick.
- **Brightness Control:** Software PWM supports brightness settings from 0% to 100%.
- **Serial Communication:** Control the LED through the Serial Monitor.
- **Command Validation:** Invalid commands and brightness values are rejected.
- **Status Monitoring:** View the current mode, brightness, and uptime.
- **Embedded C:** Demonstrates low-level microcontroller programming concepts.

## Hardware Requirements

- Arduino Uno (ATmega328P)
- Red LED
- 330 Ω resistor
- Breadboard and jumper wires
- USB cable for programming

Alternatively, you can run the project in the Wokwi online simulator without physical hardware.

## Circuit Connections

| Arduino Component | Connection |
|---|---|
| Digital pin 13 | 330 Ω resistor |
| Resistor | LED anode (+) |
| LED cathode (-) | GND |

**Note:** You can also use the Arduino Uno's built-in LED connected to digital pin 13.

## Software Requirements

- Arduino IDE (for physical hardware)
- Web browser and Wokwi (for simulation)
- Git and GitHub (for version control)

## Project Structure

```text
arduino-led-controller/
├── sketch.ino
├── diagram.json
├── README.md
└── LICENSE
```

- `sketch.ino` — Arduino firmware.
- `diagram.json` — Wokwi circuit configuration.
- `README.md` — Project documentation.
- `LICENSE` — Project licensing information.

## Getting Started

### Option 1: Run in Wokwi

1. Open your Arduino Uno project in Wokwi.
2. Ensure `sketch.ino` and `diagram.json` are present.
3. Start the simulation by clicking the green Play button.
4. Open the Serial Monitor.
5. Enter a command and press Enter to control the LED.

### Option 2: Run on Physical Hardware

1. Connect the LED and resistor according to the circuit connections.
2. Connect the Arduino Uno to your computer using a USB cable.
3. Open `sketch.ino` in the Arduino IDE.
4. Select Arduino Uno as the board and choose the appropriate port.
5. Upload the firmware.
6. Open the Serial Monitor and set the baud rate to **9600**.

## Serial Commands

Enter the following commands in the Serial Monitor.

| Command | Description |
|---|---|
| `HELP` | Displays the available commands |
| `STATUS` | Displays the current controller status |
| `MODE OFF` | Turns the LED off |
| `MODE SOLID` | Keeps the LED continuously on |
| `MODE SLOW` | Generates a slow blinking pattern |
| `MODE FAST` | Generates a fast blinking pattern |
| `MODE HEART` | Generates a double-pulse pattern |
| `BRIGHT 50` | Sets brightness to 50% |
| `BRIGHT 100` | Sets brightness to 100% |
| `DUTY` | Reports the measured LED on-time percentage |

Brightness values can range from 0 to 100.

## Technical Details

| Parameter | Value |
|---|---|
| Microcontroller | ATmega328P |
| Development board | Arduino Uno |
| Clock frequency | 16 MHz |
| Operating voltage | 5 V |
| Timer | Timer1 |
| Timer mode | CTC |
| Timer prescaler | 64 |
| Timer interrupt interval | 1 ms |
| Serial baud rate | 9600 |
| LED output | Digital pin 13 |
| Current-limiting resistor | 330 Ω |

## Working Principle

1. The Arduino initializes the timer, LED output, and serial communication.
2. Timer1 generates periodic interrupts at 1 ms intervals.
3. The firmware updates the selected LED pattern and software PWM state.
4. The Serial Monitor receives user commands.
5. The command parser validates the input and updates the LED mode or brightness.
6. The LED responds according to the selected settings.

## Testing

The project is designed to demonstrate the following behaviours:

- LED OFF and continuously ON modes.
- Slow and fast blinking patterns.
- Double-pulse LED pattern.
- Brightness adjustment.
- Serial command processing.
- Rejection of invalid commands.

**Note:** Simulation results should not be presented as physical hardware measurements. Add your own screenshots and measured results after testing the project.

## Applications

- Learning embedded C programming.
- Understanding timer interrupts and UART communication.
- Demonstrating software PWM.
- Practising Arduino development and debugging.
- Building a foundation for more advanced embedded systems projects.

## Future Improvements

- Add push-button control.
- Introduce multiple LEDs and configurable patterns.
- Display controller status on an LCD.
- Add sensor-based LED control.
- Develop a graphical interface for controlling brightness and patterns.

## License

This project is licensed under the MIT License. See the `LICENSE` file for details.

## Author
harshacodes0



---

If you find this project useful for learning embedded systems, feel free to explore the code and experiment with additional LED patterns.
