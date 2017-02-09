#include "keyboard_event.h"
#include "suinput.h"
#include "stdio.h"
#include <iostream>
#include <string>
#include "string.h"
#include <stdexcept>
#include <key_alphabet_to_event.h>
#include "unistd.h"

int uinput_fd;
struct uinput_user_dev user_dev;
uint16_t key_codes[6] = {0};

void keyboard_event(int16_t *keys) {

    if (keys[1] >= 0) {
        int key_count = 0;
        for (int i = 0; i < MAX_COMBO_KEY; i++) {
            if (keys[i] > 0) {
                key_count++;
                key_codes[i] = keys[key_count - 1];
            }
        }
        suinput_emit_combo(uinput_fd, key_codes, key_count);
        suinput_syn(uinput_fd);
    } else {
        suinput_emit_click(uinput_fd, keys[0]);
        suinput_syn(uinput_fd);
    }
}


void mouse_move_event(int mousex,int mousey)
{
    std::cout << "MOVE MOUSE" << std::endl;
    suinput_emit(uinput_fd, EV_ABS, ABS_X, mousex);
    suinput_emit(uinput_fd, EV_ABS, ABS_Y, mousey);
    suinput_syn(uinput_fd);
    usleep(10);
}

void mouse_press_event(int mousex,int mousey,int key)
{
    suinput_emit_click(uinput_fd,key);
    suinput_syn(uinput_fd);
}
void init_keyboard_event() {
    memset(&user_dev, 0, sizeof(struct uinput_user_dev));
    strcpy(user_dev.name, "libsuinput-keyboard");

    uinput_fd = suinput_open();

    if (uinput_fd == -1) {
        std::runtime_error("suinput_open");
    }

    for (int i = 0; i < BTN_TRIGGER_HAPPY40 + 1; i++) {
        suinput_enable_event(uinput_fd, EV_KEY, i);
    }

    suinput_enable_event(uinput_fd, EV_ABS, ABS_X);
    suinput_enable_event(uinput_fd, EV_ABS, ABS_Y);
    suinput_enable_event(uinput_fd, EV_REL, REL_X);
    suinput_enable_event(uinput_fd, EV_REL, REL_Y);
    suinput_enable_event(uinput_fd, EV_REL, REL_WHEEL);

    suinput_create(uinput_fd, &user_dev);
}