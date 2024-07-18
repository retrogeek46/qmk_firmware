// Copyright 2023 QMK
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H

enum custom_keycodes {
    KB_NAME = SAFE_RANGE,
};

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
    case KB_NAME:
        if (record->event.pressed) {
            // when keycode KB_NAME is pressed
            SEND_STRING("Thermal Pad");
        } else {
            // when keycode KB_NAME is released
        }
        break;
    }
    return true;
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [0] = LAYOUT(
        KC_MPRV, KC_MPLY, KC_MNXT,
        KC_PSCR, KC_HOME, KB_NAME,
        KC_LEFT, KC_END,  KC_RGHT, KC_MUTE
    )
};

#if defined(ENCODER_MAP_ENABLE)
const uint16_t PROGMEM encoder_map[][NUM_ENCODERS][NUM_DIRECTIONS] = {
    [0] = { ENCODER_CCW_CW(KC_VOLU, KC_VOLD) }
};
#endif
