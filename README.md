# BleDebounce

A lightweight, non-blocking ESP32 button debounce library with tap-and-hold state detection for **BLE Mouse** and **BLE Keyboard** HID projects.

## Index
- [Usage](#usage)
  - [Installation (`platformio.ini`)](#installation-platformioini)
  - [Quick Start Example](#quick-start-example)
- [What It Does](#what-it-does)
- [How It Works](#how-it-works)
  - [3-State Machine](#3-state-machine)
  - [Hardware Wiring](#hardware-wiring)
- [Dependencies](#dependencies)

---

## Usage

### Installation (`platformio.ini`)
Add `ble-hid-debounce` to your project's `lib_deps`. PlatformIO will automatically resolve and install the timer and BLE HID dependencies:

```ini
[env:esp32dev]
platform = espressif32
board = esp32dev
framework = arduino
lib_deps =
    YOUR_PIO_USERNAME/ble-hid-debounce
```

Quick Start Example

```C++

#include <Arduino.h>
#include <BleKeyboard.h>
#include <BleMouse.h>
#include <BleDebounce.h>

BleKeyboard bleKeyboard("Custom MacroPad");
BleMouse bleMouse("Custom MacroMouse");

// Initialize buttons: (&hid_instance, gpio_pin, key_or_mouse_code)
debounce enterKey(&bleKeyboard, 4, KEY_RETURN);
debounce leftClick(&bleMouse, 5, MOUSE_LEFT);

void setup() {
  pinMode(4, INPUT_PULLUP);
  pinMode(5, INPUT_PULLUP);

  bleKeyboard.begin();
  bleMouse.begin();
}

void loop() {
  if (bleKeyboard.isConnected()) {
    enterKey.run_process();
  }
  if (bleMouse.isConnected()) {
    leftClick.run_process();
  }
}
```

What It Does

BleDebounce converts standard tactile switches into dual-action Bluetooth HID inputs for custom macro pads, gamepads, and wireless controllers. Instead of firing raw keystrokes on every loop cycle, it filters contact bounce and distinguishes between:

    Single Tap: Sends a single click() (mouse) or write() (keyboard) upon initial press.

    Sustained Hold: Transitions into a continuous press() if the switch stays held past the debounce/hold threshold, then automatically triggers release() when lifted.

    Unified Interface: Overloaded constructors allow the same class to manage both BleMouse and BleKeyboard instances cleanly.

How It Works
3-State Machine

Calling run_process() inside loop() evaluates two internal flags (Tracking_Status and hold_Engaged) alongside a non-blocking wait_time timer:

    Initial Press (!Tracking_Status && pin == LOW):

        Marks the button as active (Tracking_Status = true), starts the hold timer, fires a single HID .click() or .write(), and debounces the mechanical contact.

    Hold Transition (wait_time.is_Time_Enlapsed() && pin == LOW && !hold_Engaged):

        If the button remains pressed once the timer elapses, it sends a continuous .press() command and locks hold_Engaged = true to prevent repeated press packets.

    Release (pin == HIGH && Tracking_Status):

        Once the button is released, it sends .release() to clear the active HID key/button state and resets both tracking flags for the next input.

Hardware Wiring

All buttons must be wired as active-low using internal or external pull-up resistors:

    Unpressed: Pin is pulled HIGH (3.3V) via pinMode(pin, INPUT_PULLUP).

    Pressed: Switch connects the GPIO pin directly to GND (LOW).

Dependencies

    wait-time (Non-blocking millis() timer utility)

    ESP32 BLE Mouse

    ESP32 BLE Keyboard

