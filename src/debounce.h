#ifndef DEBOUNCE_H
#define DEBOUNCE_H

#include <Arduino.h>
#include <BleMouse.h>
#include <BleKeyboard.h>
#include <bwait.h>

class debounce {
private:
    uint8_t pin;
    byte butt_args;
    b_wait wait_time;
    b_wait debounce_timer;
    bool Tracking_Status;
    bool hold_Engaged;
    BleMouse *target_mouse;
    BleKeyboard *target_keyboard;

protected:
    void trigger_action();

public:
    debounce(BleMouse *target_mouse, uint8_t pin, byte butt_args);
    debounce(BleKeyboard *target_keyboard, uint8_t pin, byte butt_args);

    void run_process();
};

#endif