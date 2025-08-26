# ESP32 Analog Measurement & Digital Filtering System

This project implements a complete analog measurement and digital signal processing pipeline on the ESP32, designed for precision sensor interfacing and real-time data analysis. It demonstrates how to generate analog test signals, buffer and acquire them, apply digital filtering, and transmit results for further analysis.

## Overview

The system is tailored for use with a resistive bridge sensor circuit, but the architecture is adaptable to other analog measurement scenarios. The ESP32's onboard DAC generates a stable excitation waveform, which is buffered by an external op-amp before being applied to the sensor. The sensor's output is read by the ESP32's ADC, digitally filtered to isolate the signal of interest, and then streamed over UART for visualization or logging.

## System Architecture

1.  **Signal Generation:** The ESP32 DAC (GPIO 25) outputs a programmable waveform (e.g., sine wave) with configurable frequency and amplitude to excite the sensor circuit.
2.  **Buffering:** An external op-amp, configured as a voltage follower, buffers the DAC output to prevent loading and ensure signal integrity.
3.  **Signal Acquisition:** The sensor's analog output (typically the midpoint of a resistive bridge) is sampled by the ESP32 ADC (GPIO 34).
4.  **Digital Filtering:** Each ADC sample is processed in real-time by a cascaded biquad (second-order section) bandpass filter, implemented in software, to remove noise and unwanted frequency components.
5.  **Data Output:** Filtered data is formatted and transmitted over UART (115200 baud) for monitoring, plotting, or logging on a host computer.

## Features

- Software-defined analog signal generation using ESP32 DAC
- Real-time digital bandpass filtering with configurable parameters
- Robust hardware interface for sensor excitation and signal buffering
- Serial data streaming for easy integration with analysis tools
- Modular code structure for easy adaptation to other sensor types

## Getting Started

This is a PlatformIO project. To build and flash the firmware:

1. **Clone the repository:**
    ```bash
    git clone https://github.com/jedlamartin/esp_filt.git
    ```
2. **Open** the project in VSCode with the PlatformIO extension.
3. **Build** the project (checkmark icon in PlatformIO toolbar).
4. **Connect** your ESP32 board via USB.
5. **Upload** the firmware (arrow icon in PlatformIO toolbar).
6. **Monitor** the serial output (plug icon in PlatformIO toolbar).

## Hardware Connections

| ESP32 Pin | Connection          | Function                   |
| :-------- | :------------------ | :------------------------- |
| GPIO 25   | Op-Amp Input        | DAC Output (Excitation)    |
| GPIO 34   | Bridge Midpoint     | ADC Input (Measurement)    |
| GND       | Circuit GND         | Common Ground              |
| 3.3V      | Op-Amp VCC          | Power                      |

## Customization

- **Waveform Generation:** Modify the waveform type, frequency, or amplitude in the firmware to suit your sensor or test requirements.
- **Filter Parameters:** Adjust the biquad filter coefficients to target your desired frequency band.
- **Data Output:** Change the UART format or baud rate as needed for your host application.

## License

MIT License. See `LICENSE` for details.