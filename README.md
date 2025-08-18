# Zephyr RTOS Projects

This repository contains a collection of Zephyr RTOS projects demonstrating various embedded systems concepts and peripherals. The projects are designed for the **STM32L475 Discovery IoT Kit (DISCO-L475-IOT1)** and showcase a range of functionalities from basic GPIO control to complex multi-threaded applications.

## Table of Contents

- [Zephyr RTOS Projects](#zephyr-rtos-projects)
  - [Table of Contents](#table-of-contents)
  - [Projects](#projects)
  - [Prerequisites](#prerequisites)
  - [Setup and Installation](#setup-and-installation)
    - [1. Install Dependencies](#1-install-dependencies)
    - [2. Set up Python Virtual Environment](#2-set-up-python-virtual-environment)
    - [3. Install West and Get Zephyr Source](#3-install-west-and-get-zephyr-source)
    - [4. Install Zephyr SDK](#4-install-zephyr-sdk)
  - [Building and Flashing](#building-and-flashing)
    - [Build a Project](#build-a-project)
    - [Flash to Board](#flash-to-board)
    - [View Serial Output](#view-serial-output)
  - [Project Structure](#project-structure)
  - [License](#license)

## Projects

Here is a list of the projects included in this repository, each corresponding to a specific learning objective:

1.  **`01_Basic_LED`**: **Basic LED Blinking**
    - Blinks a single LED with a 500ms ON/OFF interval.
    - Demonstrates basic GPIO configuration using Zephyr's device tree bindings.

2.  **`02_DBG_Log`**: **LEDs with Debug Logging**
    - Blinks all available user LEDs with a 750ms delay.
    - Integrates Zephyr's logging system (`LOG_LEVEL_DBG`) to log LED state changes.

3.  **`03_Toggle_LED`**: **Sequential and Simultaneous LED Toggling**
    - Implements two patterns: toggling all LEDs at once, and then toggling them sequentially.
    - Uses `k_msleep()` for precise timing.

4.  **`04_Button_Polling`**: **Button-Controlled LED (Polling)**
    - Toggles an LED based on a button press, detected using a polling-based approach with `gpio_pin_get()`.

5.  **`05_Button_Interrupt`**: **Button-Controlled LED (Interrupt)**
    - Uses GPIO interrupts to toggle an LED upon a button press, providing a more efficient alternative to polling.

6.  **`06_PWM_2LED`**: **PWM LED Fading**
    - Demonstrates Pulse-Width Modulation (PWM) to control the brightness of two LEDs, making them fade in and out.

7.  **`07_UART_Polling`**: **UART Communication (Polling)**
    - Establishes basic UART communication to transmit a welcome message and echo back any received characters using a polling-based method.

8.  **`08_UART_Interrupt`**: **UART Communication (Interrupt)**
    - Enhances the UART example by using interrupts for receiving data, allowing for more efficient, non-blocking communication.

9.  **`09_UART_Interrupt_Control_LED`**: **LED Control via UART Commands**
    - Implements a simple command parser to control an LED via UART commands (`LED ON`, `LED OFF`, `TOGGLE`).

10. **`11_Mutex_Synchronization`**: **Thread Safety with Mutexes**
    - Demonstrates using a mutex to protect a shared counter that is incremented by two concurrent threads, preventing race conditions.

11. **`12_Threads_Semaphore`**: **Thread Coordination with Semaphores**
    - Implements two classic synchronization patterns:
      - **Signal and Wait**: One thread waits for a semaphore before starting, which is given by another thread.
      - **Ping-Pong**: Two threads use a pair of semaphores to alternate printing "Ping" and "Pong" messages.

12. **`13_Sensor_Data_Logging_using_LittleFS`**: **Multi-Sensor Data Logging**
    - A complex, multi-threaded application that reads data from I²C and SPI sensors (HTS221, LPS22HB, LSM6DSL).
    - Uses a dedicated logger thread to write sensor data to a file on a LittleFS filesystem every minute.
    - Employs mutexes for thread-safe data sharing and demonstrates power management by entering a low-power state between logging intervals.

## Prerequisites

-   **Hardware**: STM32L475 Discovery IoT Kit (B-L475E-IOT01A)
-   **OS**: A Linux-based operating system (Ubuntu is recommended).
-   **Tools**: Git, CMake, Python3, and other build essentials.

## Setup and Installation

Follow these steps to set up your development environment for Zephyr.

### 1. Install Dependencies

Update your package manager and install the required system dependencies.

```bash
sudo apt update
sudo apt upgrade
sudo apt install --no-install-recommends git cmake ninja-build gperf \
  ccache dfu-util device-tree-compiler wget python3-dev python3-venv python3-tk \
  xz-utils file make gcc gcc-multilib g++-multilib libsdl2-dev libmagic1
```

Verify the versions of the installed tools:
```bash
cmake --version
python3 --version
dtc --version
```

### 2. Set up Python Virtual Environment

It is recommended to use a Python virtual environment to avoid conflicts with system-wide packages.

```bash
# Create a virtual environment
python3 -m venv ~/zephyrproject/.venv

# Activate the virtual environment (do this every time you open a new terminal)
source ~/zephyrproject/.venv/bin/activate
```

### 3. Install West and Get Zephyr Source

`west` is Zephyr's primary command-line tool.

```bash
# Install west
pip install west

# Initialize the Zephyr project
west init ~/zephyrproject
cd ~/zephyrproject

# Pull the Zephyr source code and module repositories
west update

# Export a Zephyr CMake package
west zephyr-export

# Install Python dependencies for Zephyr
pip install -r ~/zephyrproject/zephyr/scripts/requirements.txt
```

### 4. Install Zephyr SDK

The SDK contains the toolchains and other necessary tools for building Zephyr applications.

```bash
cd ~/zephyrproject/zephyr
west sdk install
```

## Building and Flashing

### Build a Project

To build any of the projects in this repository, navigate to its directory and use `west build`.

```bash
# Example for 01_Basic_LED
cd 01_Basic_LED
west build -b disco_l475_iot1
```

### Flash to Board

After a successful build, flash the application to your board.

```bash
west flash
```

### View Serial Output

To view logs and other serial output from the board, you can use `minicom` or your preferred serial terminal program.

```bash
sudo minicom -D /dev/ttyACM0 -b 115200
```

## Project Structure

Each project in this repository follows a standard structure:

```tree
project_name/
├── src/            # Source code files (.c)
├── boards/         # Board-specific overlay files (optional)
├── CMakeLists.txt  # Build system configuration
├── prj.conf        # Zephyr project configuration file
```