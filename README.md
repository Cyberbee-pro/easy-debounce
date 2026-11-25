# Easy Debounce

A lightweight, non-blocking ESP32 button debounce library with tap-and-hold state detection for **BLE Mouse** and **BLE Keyboard** HID projects.

## Index
- [Usage](#usage)
  - [Installation (`platformio.ini`)](#installation-platformioini)
  - [Quick Start Example](#quick-start-example)
- [What It Does](#what-it-does)
- [How It Works](#how-it-works)
  - [4-State Machine](#4-state-machine)
  - [Hardware Wiring](#hardware-wiring)
- [Dependencies](#dependencies)

---

## Usage

### Installation (`platformio.ini`)
Add `cyberbee-pro/easy_debounce` to your project's `lib_deps`. PlatformIO will automatically resolve and install `better_wait` and the BLE HID dependencies:

```ini
[env:esp32dev]
platform = espressif32
board = esp32dev
framework = arduino
lib_deps =
    cyberbee-pro/easy_debounce
```

Quick Start Example

```C++

#include <Arduino.h>
#include <BleKeyboard.h>
#include <BleMouse.h>
#include <debounce.h>

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

easy_debounce converts standard tactile switches into dual-action Bluetooth HID inputs for custom macro pads, gamepads, and wireless controllers. Instead of firing raw keystrokes on every loop cycle, it filters mechanical contact bounce without blocking loop() and distinguishes between:

    Single Tap: Sends a single click() (mouse) or write() (keyboard) upon initial press.

    Sustained Hold: Transitions into a continuous press() if the switch stays held past the debounce and hold thresholds, then automatically triggers release() when lifted.

    Unified Interface: Overloaded constructors allow the same debounce class to manage both BleMouse and BleKeyboard instances cleanly.

How It Works
4-State Machine

Calling run_process() inside loop() evaluates two internal flags (Tracking_Status and hold_Engaged) alongside two non-blocking b_wait timers (debounce_timer at 50ms and wait_time at 80ms):

    Initial Press (!Tracking_Status && pin == LOW):
    Marks the button as active (Tracking_Status = true), starts the 50ms debounce_timer, and immediately fires a single HID .click() or .write().

    Debounce Filter (Tracking_Status && debounce_timer.GetIsActive()):
    Filters mechanical contact bounce for 50ms without blocking other buttons, then stops debounce_timer and starts the 80ms wait_time hold timer.

    Hold Transition (wait_time.is_Time_Enlapsed() && pin == LOW && Tracking_Status && !hold_Engaged):
    If the button remains pressed once the 80ms threshold elapses, it sends a continuous .press() command and sets hold_Engaged = true to prevent repeated press packets.

    Release (pin == HIGH && Tracking_Status):
    Once the switch is lifted, it stops both timers, sends .release() to clear the active HID key/button state, and resets both tracking flags for the next input.

Hardware Wiring

All buttons must be wired as active-low using internal or external pull-up resistors:

    Unpressed: Pin is pulled HIGH (3.3V) via pinMode(pin, INPUT_PULLUP).

    Pressed: Switch connects the GPIO pin directly to GND (LOW).

Dependencies

    cyberbee-pro/better_wait

    ESP32 BLE Mouse

    ESP32 BLE Keyboard