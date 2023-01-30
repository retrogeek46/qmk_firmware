/*
Copyright 2019 @foostan
Copyright 2020 Drashna Jaelre <@drashna>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include QMK_KEYBOARD_H
#include <stdio.h>
#include "raw_hid.h"
#include <print.h>

#define L_ANGL S(KC_COMM)
#define R_ANGL S(KC_DOT)

int hid_code;
int current_title_code[21];
int current_artist_code[21];
bool updated;

enum layers {
    _BASE,
    _SYMB,
    _NUMP,
    _MAPS,
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
  [0] = LAYOUT_split_3x6_3(
  //,-----------------------------------------------------.                    ,-----------------------------------------------------.
       KC_ESC,    KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,                         KC_Y,    KC_U,    KC_I,    KC_O,   KC_P,   KC_DEL,
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
        MO(2),    KC_A,    KC_S,    KC_D,    KC_F,    KC_G,                         KC_H,    KC_J,    KC_K,    KC_L, KC_SCLN, KC_QUOT,
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
      KC_LSFT,    KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,                         KC_N,    KC_M, KC_COMM,  KC_DOT, KC_SLSH, KC_LGUI,
  //|--------+--------+--------+--------+--------+--------+--------|  |--------+--------+--------+--------+--------+--------+--------|
                                           KC_SPC, KC_LCTL, KC_LALT,    KC_BSPC,   MO(1),  KC_APP
                                      //`--------------------------'  `--------------------------'

  ),

  [1] = LAYOUT_split_3x6_3(
  //,-----------------------------------------------------.                    ,-----------------------------------------------------.
       KC_TAB,  KC_GRV, XXXXXXX, KC_LBRC, KC_RBRC, XXXXXXX,                      XXXXXXX, KC_HOME,   KC_UP,  KC_END,  KC_INS, KC_BSLS,
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
      QK_BOOT, KC_MINS, KC_EQL,  S(KC_9), S(KC_0), XXXXXXX,                      XXXXXXX, KC_LEFT, KC_DOWN,KC_RIGHT,  KC_ENT, XXXXXXX,
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
      KC_LSFT, XXXXXXX, XXXXXXX,  L_ANGL,  R_ANGL, XXXXXXX,                      XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, KC_LGUI,
  //|--------+--------+--------+--------+--------+--------+--------|  |--------+--------+--------+--------+--------+--------+--------|
                                           KC_SPC, KC_LCTL, KC_LALT,    KC_BSPC,   MO(1),  KC_APP
                                      //`--------------------------'  `--------------------------'
  ),

  [2] = LAYOUT_split_3x6_3(
  //,-----------------------------------------------------.                    ,-----------------------------------------------------.
       KC_GRV, XXXXXXX,    KC_7,    KC_8,    KC_9, XXXXXXX,                      KC_CIRC, KC_AMPR, KC_ASTR, KC_LPRN, KC_RPRN, KC_BSPC,
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
      KC_LCTL, XXXXXXX,    KC_4,    KC_5,    KC_6, XXXXXXX,                      KC_MINS,  KC_EQL, KC_LBRC, KC_RBRC, KC_BSLS,  KC_GRV,
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
      KC_LSFT,    KC_0,    KC_1,    KC_2,    KC_3, XXXXXXX,                      KC_UNDS, KC_PLUS, KC_LCBR, KC_RCBR, KC_PIPE, KC_TILD,
  //|--------+--------+--------+--------+--------+--------+--------|  |--------+--------+--------+--------+--------+--------+--------|
                                           KC_SPC, KC_LCTL, KC_LALT,    KC_BSPC,   MO(1),  KC_APP
                                      //`--------------------------'  `--------------------------'
  ),

  [3] = LAYOUT_split_3x6_3(
  //,-----------------------------------------------------.                    ,-----------------------------------------------------.
      QK_BOOT, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                      XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
      RGB_TOG, RGB_HUI, RGB_SAI, RGB_VAI, XXXXXXX, XXXXXXX,                      XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
      RGB_MOD, RGB_HUD, RGB_SAD, RGB_VAD, XXXXXXX, XXXXXXX,                      XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
  //|--------+--------+--------+--------+--------+--------+--------|  |--------+--------+--------+--------+--------+--------+--------|
                                           KC_SPC, KC_LCTL, KC_LALT,    KC_BSPC,   MO(1),  KC_APP
                                      //`--------------------------'  `--------------------------'
  )
};

const char code_to_name[60] = {
    ' ', ' ', ' ', ' ',  'A', 'B', 'C', 'D', 'E', 'F',
    'G', 'H', 'I', 'J',  'K', 'L', 'M', 'N', 'O', 'P',
    'Q', 'R', 'S', 'T',  'U', 'V', 'W', 'X', 'Y', 'Z',
    '1', '2', '3', '4',  '5', '6', '7', '8', '9', '0',
    'R', 'E', 'B', 'T',  '_', '-', '=', '[', ']', '\\',
    '#', ';', '\'', '`', ',', '.', '/', ' ', ' ', ' '
};

void print_int_array(uint8_t *arr, int start_index) {
    print("priting arr\n");
    if (start_index < 1) {
        start_index = 0;
    }
    int arrLen = sizeof arr / sizeof arr[0];
    for (int i = start_index; i < arrLen; i++) {
        uprintf("%d ", arr[i]);
    }
    print("\n");
}

void keyboard_post_init_user(void) {
    // Customize these values to desired behaviour
    debug_enable = true;
    //debug_keyboard = true;
}

void send_keyboard_state(void) {
    uint8_t send_data[32] = {23};
    // send_data[1] = encoder_mode + 1;
    send_data[2] = biton32(layer_state);
    // send_data[3] = current_os;
    raw_hid_send(send_data, sizeof(send_data));
}

void raw_hid_receive(uint8_t *data, uint8_t length) {
    // printf("getting raw hid data %d %d %d\n", data[0], data[1], data[2]);
    // print("raw hid\n");
    int i = 0;
    switch(*data) {
        case 1:
            // test_rgb_value(255, 0, 0);
            break;
        case 10:
            switch(data[1]) {
                case 1:
                    // update_encoder_state();
                    break;
                case 2:
                    send_keyboard_state();
                    break;
                case 3:
                    // set_cpu_usage_rgb(da     );
                    break;
                case 4:
                    // update_os_state(data[2]);
                    break;
                case 5:
                    // test_rgb_value(data[2], data[3], data[4]);
                    break;
                case 6:
                    for (i = 2; i < length; i++) {
                        if (data[i] == 0) {
                            current_title_code[i - 2] = 0;
                            break;
                        }
                        current_title_code[i - 2] = data[i] - 93;
                    }
                    updated = true;
                    break;
                case 7:
                    for (i = 2; i < length; i++) {
                        if (data[i] == 0) {
                            current_artist_code[i - 2] = 0;
                            break;
                        }
                        current_artist_code[i - 2] = data[i] - 93;
                    }
                    updated = true;
                    break;
            }
            break;
    }
}

#ifdef OLED_ENABLE
oled_rotation_t oled_init_user(oled_rotation_t rotation) {
    if (!is_keyboard_master()) {
      return OLED_ROTATION_180;  // flips the display 180 degrees if offhand
    }
    // else {
    //    return OLED_ROTATION_270; 
    // }
    return rotation;
}

void oled_render_layer_state(void) {
    switch (biton32(layer_state)) {
        case _BASE:
            oled_write_ln_P(PSTR("Layer: BASE"), false);
            break;
        case _SYMB:
            oled_write_ln_P(PSTR("Layer: SYMB"), false);
            break;
        case _NUMP:
            oled_write_ln_P(PSTR("Layer: NUMP"), false);
            break;
    }
}

void oled_render_media(void) {
    // if (updated) {
    //     updated = false;
    //     int arrLen = sizeof current_title_code / sizeof current_title_code[0];
    //     for (int i = 0; i < arrLen; i++) {
    //         // uprintf("%d ", current_title_code[i]);
    //     }
    //     // print("\n");
    // }

    bool skip = false;
    for (int i = 0; i < 21; i++) {
        if (current_title_code[i] == 0 || skip) {
            skip = true;
            oled_write_char(code_to_name[0], false);
        } else {
            oled_write_char(code_to_name[current_title_code[i]], false);
        }
    }
    
    oled_advance_page(false);
    // oled_write_ln_P(PSTR("@"), false);
    
    skip = false;
    for (int i = 0; i < 21; i++) {
        if (current_artist_code[i] == 0 || skip) {
            skip = true;
            oled_write_char(code_to_name[0], false);
        } else {
            oled_write_char(code_to_name[current_artist_code[i]], false);
        }
    }
}

// Used to draw on to the oled screen
bool oled_task_user(void) {
    if (!is_keyboard_master()) {
        //
    } else {
        // oled_render_layer_state();
        oled_render_media();
    }
    return false;
}
#endif // OLED_ENABLE
