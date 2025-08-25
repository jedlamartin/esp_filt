# ESP32 DAC/ADC & Digital Filtering System

An ESP32 firmware project that demonstrates a complete measurement loop: generating an analog test signal with the onboard DAC, buffering it through an external op-amp circuit, acquiring it via the ADC, applying a real-time digital bandpass filter, and transmitting results over serial. Built with ESP-IDF in the PlatformIO environment.

## Project Description

This project was designed to interface with a custom resistive bridge measurement circuit. The core task was to create a stable, software-defined measurement system. The ESP32's DAC (Digital-to-Analog Converter) generates a precise waveform to excite the bridge circuit. The resulting analog signal from the bridge is highly susceptible to noise. This signal is acquired by the ADC (Analog-to-Digital Converter) and then processed in real-time on the ESP32 using a cascaded biquad (Second-Order Section) filter to isolate the frequency band of interest before the clean data is output via UART.

## Implementation Overview

1.  **Signal Generation:** The ESP32's DAC (GPIO 25) is programmed to output a specific waveform (e.g., sine wave) at a defined frequency and amplitude.
2.  **Hardware Interface:** The DAC output is buffered by an op-amp (configured as a voltage follower) to provide sufficient current to drive the resistive bridge without loading the DAC.
3.  **Signal Acquisition:** The voltage from the midpoint of the bridge is read back into the system using the ESP32's ADC (GPIO 34).
4.  **Digital Signal Processing (DSP):** Each sample from the ADC is passed through a real-time bandpass filter implemented as a cascade of biquad filters. This software filter removes DC offset, high-frequency noise, and any unwanted frequency components.
5.  **Data Output:** The filtered results are periodically formatted and sent over the serial port (UART) at 115200 baud for monitoring, plotting, or logging on a host computer.

## Key Features

*   Hardware-based signal generation via ESP32 DAC.
*   Real-time signal processing using a custom biquad filter library.
*   Configurable filter parameters (center frequency, bandwidth).
*   Serial data output for external analysis.

## Build & Flash

This is a PlatformIO project. Open it in VSCode with the PlatformIO extension installed.

1.  **Clone the repo:**
    ```bash
    git clone https://github.com/jedlamartin/esp_filt.git
    ```
2.  **Open** the project directory in VSCode.
3.  **Build** (PlatformIO toolbar checkmark).
4.  **Connect** ESP32 via USB.
5.  **Upload** (PlatformIO toolbar arrow).
6.  **Monitor** serial output (PlatformIO toolbar plug icon).

## Hardware Setup

| ESP32 Pin | Connection          | Function               |
| :-------- | :------------------ | :--------------------- |
| GPIO 25   | Op-Amp Input        | DAC Output (Signal Gen)|
| GPIO 34   | Bridge Midpoint     | ADC Input (Measurement)|
| GND       | Circuit GND         | Common Ground          |
| 3.3V      | Op-Amp VCC          | Power                  |

## License

MIT License. See `LICENSE` file for details.
