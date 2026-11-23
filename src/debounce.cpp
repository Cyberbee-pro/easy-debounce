#include <Arduino.h>
#include "debounce.h"

debounce::debounce(BleMouse *target_mouse, uint8_t pin, byte butt_args)
    : wait_time(80), debounce_timer(50) {
    this->pin = pin;
    this->target_mouse = target_mouse;
    this->target_keyboard = nullptr;
    this->butt_args = butt_args;
    this->hold_Engaged = false;
    this->Tracking_Status = false;
}

debounce::debounce(BleKeyboard *target_keyboard, uint8_t pin, byte butt_args)
    : wait_time(80), debounce_timer(50) {
    this->pin = pin;
    this->target_mouse = nullptr;
    this->target_keyboard = target_keyboard;
    this->butt_args = butt_args;
    this->hold_Engaged = false;
    this->Tracking_Status = false;
}

void debounce::trigger_action() {
    // State 1: Initial press detected (active-low) -> trigger click and start 50ms debounce filter
    if (!Tracking_Status && digitalRead(pin) == LOW) {
        Tracking_Status = true;
        hold_Engaged = false;
        debounce_timer.start();

        if (nullptr != target_mouse) {
            target_mouse->click(butt_args);
        } else if (nullptr != target_keyboard) {
            target_keyboard->write(butt_args);
        }
    }
    // State 2: Debounce window elapsed -> hand off to 80ms hold-threshold timer
    else if (Tracking_Status && debounce_timer.GetIsActive()) {
        if (debounce_timer.is_Time_Enlapsed()) {
            debounce_timer.stop();
            wait_time.start();
        }
    }
    // State 3: Held past threshold -> transition to continuous press state
    else if (wait_time.is_Time_Enlapsed() && digitalRead(pin) == LOW && Tracking_Status && !hold_Engaged) {
        wait_time.stop();

        if (nullptr != target_mouse) {
            target_mouse->press(butt_args);
        } else if (nullptr != target_keyboard) {
            target_keyboard->press(butt_args);
        }

        hold_Engaged = true;
    }
    // State 4: Button released -> reset state and release HID key/button
    else if (digitalRead(pin) == HIGH && Tracking_Status) {
        debounce_timer.stop();
        wait_time.stop();

        if (nullptr != target_mouse) {
            target_mouse->release(butt_args);
        } else if (nullptr != target_keyboard) {
            target_keyboard->release(butt_args);
        }

        Tracking_Status = false;
        hold_Engaged = false;
    }
}

void debounce::run_process() {
    trigger_action();
}