#ifndef KEYBOARD_MOUSE_EVENT
#define KEYBOARD_MOUSE_EVENT

#include "stdio.h"
#include "stdint.h"

#define MAX_COMBO_KEY 5
enum EVENT_TYPE{
    KEYBOARD_EVENT = 0,
    MOUSE_MOVE_EVENT = 1,
    MOUSE_PRESS_EVENT = 2
};

struct __attribute__((__packed__)) key_mouse_event{
    uint8_t event_type;
    int16_t data[5];
};


//Key1: char
//Key2 ALT SHIFT...
//Key3 ALT SHIFT...
void keyboard_event(int16_t *keys);
void mouse_move_event(int mousex,int mousey);
void mouse_press_event(int mousex,int mousey,int key);
void init_keyboard_event();

#endif
