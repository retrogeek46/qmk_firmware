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
int time_code[21];
bool media_updated;
bool clock_updated;

enum layers {
    _BASE,
    _SYMB,
    _NUMP,
    _MAPS,
};

enum custom_keycodes {
    KC_OLED = SAFE_RANGE
};

enum oled_states {
    OLED_CLOCK,
    OLED_MEDIA,
    OLED_LAYER,
    _OLED_STATE_RANGE
};

uint8_t oled_state = OLED_CLOCK;
bool reset_oled = false;

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
       KC_TAB,  KC_GRV, XXXXXXX, KC_LBRC, KC_RBRC, XXXXXXX,                      KC_OLED, KC_HOME,   KC_UP,  KC_END,  KC_INS, KC_BSLS,
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
      XXXXXXX, KC_MINS, KC_EQL,  S(KC_9), S(KC_0), XXXXXXX,                      XXXXXXX, KC_LEFT, KC_DOWN,KC_RIGHT,  KC_ENT, QK_BOOT,
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
    '#', ';', '\'', '`', ',', '.', '/', ':', ' ', ' '
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
                        current_title_code[i - 2] = data[i];
                    }
                    media_updated = true;
                    break;
                case 7:
                    for (i = 2; i < length; i++) {
                        if (data[i] == 0) {
                            current_artist_code[i - 2] = 0;
                            break;
                        }
                        current_artist_code[i - 2] = data[i];
                    }
                    media_updated = true;
                    break;
                case 8:
                    for (i = 2; i < length; i++) {
                        if (data[i] == 0) {
                            time_code[i - 2] = 0;
                            break;
                        }
                        time_code[i - 2] = data[i];
                    }
                    clock_updated = true;
                    break;
            }
            break;
    }
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case KC_OLED:
            if (record->event.pressed) {
                if (oled_state == _OLED_STATE_RANGE - 1) {
                    oled_state = 0;
                } else {
                    oled_state = oled_state + 1 % _OLED_STATE_RANGE;
                }
                reset_oled = true;
                return false;
            } else {
                return true;
            }
        default:
            return true;
    }
}

#ifdef OLED_ENABLE
static const char PROGMEM frame0[] = {
    0,  0,248, 68, 68, 68, 56,128, 64, 64,128,  0,128, 64, 64,128,  0,128, 64, 64,128,  0,  0,  0,  0,192,  0, 64,192, 64,  0,  0,  0,  0, 15,  0,  0,  0,  0, 15,  1,  1, 15,  0, 15,  2,130, 65,128, 15,  1,  1, 15,  0,  4,  8,  8,  7,  0,  8, 15,  8,  0,  0,  0,  0,128,128,128,128,128,  0,  0,  0,  0,  0,  0,  0, 13, 18,  8,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,127,  4,  4,  4,  0,124, 20,104,  0, 68,124, 68,  0,124, 84, 68,  0,124, 24, 48,124,  0,124, 68, 56,  0, 72, 84, 36, 
    0,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,  0,  0,  4,252,  0,  0,  0,  0,192,160,160,192,  0, 32,224, 32,  0,224,  0,  0,  0,192,160,160,192,  0,  0,  0,  0,  0,  0,  0,  0,224, 16, 17,137,121,  9,136,  9,  8,136,  9,120,137, 17, 17,224,  1,  1,  1,  0,  1,  0,  0,  1,128,128,128,  0,  0,  0,  0,  0,  0,  3,  4,123,128, 16,112,150,150,112, 16,  0,  3, 12, 11,  8, 16, 16, 16, 16, 16, 16, 16, 16, 17, 50, 44, 65,166, 24, 
    0,  0,  0,  0,  0,  0, 31, 96,192,  0, 16,160, 64,128,  0, 16,224, 64, 64, 32, 32, 16, 16, 16,120,128,  0,  0,  0,  0,240, 15,  0,  0,  0,  0,  0,  0, 96, 80, 79, 64, 96,159, 64, 63,  0,128,127,  0,  0,  0, 96, 80, 80, 76, 35,208,175,162,156,192, 32, 31,  0,  0,  0,  8,248, 72, 72, 72,176,  0, 64,193, 65,  1,193,  1,  0,  0,192,  0,  0,  0,192,  0,  0,192,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  3,  2,  2,  2,  1,  0,  2,  3,  2,  0,  3,226, 18, 32,195,  2,  2,  0,  1,  2,194, 33, 16, 96,128,  0,  0,  0, 
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,240, 14,  1,  0, 16,  8,  9, 17,  1,  1, 17,  9,  8, 16,  0,  7, 24, 32,192,  0,  0,128, 96, 24,134, 97, 17, 14,  0,  0,  0,131,100, 40, 24, 16, 16, 32, 32, 32, 34, 38, 34, 32, 32, 32, 16,112,136,  6,  1,  0,  0, 15, 48, 64,143,144,224, 16, 16, 12,  3,  0,  0,  0,224,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,192, 63,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  3,  4,  4,  8,  8, 16, 16, 57, 38, 32, 32, 63, 32, 32, 32, 32, 32, 63, 32, 32, 39, 56,  0,  0, 
    0,
};

static const char PROGMEM frame1[] = {
    0,  0,248, 68, 68, 68, 56,128, 64, 64,128,  0,128, 64, 64,128,  0,128, 64, 64,128,  0,  0,  0,  0,192,  0, 64,192, 64,  0,  0,  0,  0, 15,  0,  0,  0,  0, 15,  1,  1, 15,  0, 15,  2,130, 65,128, 15,  1,  1, 15,  0,  4,  8,  8,  7,  0,  8, 15,  8,  0,  0,  0,  0,128,128,128,128,128,  0,  0,  0,  0,  0,  0,  0, 13, 18,  8,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,127,  4,  4,  4,  0,124, 20,104,  0, 68,124, 68,  0,124, 84, 68,  0,124, 24, 48,124,  0,124, 68, 56,  0, 72, 84, 36, 
    0,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,  0,  0,  4,252,  0,  0,  0,  0,192,160,160,192,  0, 32,224, 32,  0,224,  0,  0,  0,192,160,160,192,  0,  0,  0,  0,  0,  0,  0,  0,224, 16, 17,137,121,  9,136,  9,  8,136,  9,120,137, 17, 17,224,  1,  1,  1,  0,  1,  0,  0,  1,  0,192, 64,128,  0,  0,  0,  0,  0,  3,  4,123,128, 16,112,150,150,112, 16,  0,  3, 12, 11,  8, 16, 16, 16, 16, 16, 16, 16, 16, 16, 49, 46, 64,167, 24, 
    0,  0,  0,  0,  0,  0, 31, 96,192,  0, 16,160, 64,128,  0, 16,224, 64, 64, 32, 32, 16, 16, 16,120,128,  0,  0,  0,  0,240, 15,  0,  0,  0,  0,  0,  0, 96, 80, 79, 64, 96,159, 64, 63,  0,128,127,  0,  0,  0, 96, 80, 80, 76, 35,208,175,162,156,192, 32, 31,  0,  0,  0,  8,248, 72, 72, 72,176,  0, 64,193, 65,  1,193,  1,  0,  0,192,  0,  0,  0,192,  0,  0,192,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  3,  2,  2,  2,  1,  0,  2,  3,  2,  0,  3,226, 18, 32,195,  2,  2,  0,  1,  2,194, 33, 16, 96,128,  0,  0,  0, 
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,240, 14,  1,  0, 16,  8,  9, 17,  1,  1, 17,  9,  8, 16,  0,  7, 24, 32,192,  0,  0,128,120,  6,129,121,  6,  0,  0,  0,  0,131,100, 40, 24, 16, 16, 32, 32, 32, 34, 38, 34, 32, 32, 32, 16,112,136,  6,  1,  0,  0, 15, 48, 64,143,144,224, 16, 16, 12,  3,  0,  0,  0,224,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,192, 63,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  3,  4,  4,  8,  8, 16, 16, 57, 38, 32, 32, 63, 32, 32, 32, 32, 32, 63, 32, 32, 39, 56,  0,  0, 
    0,
};

static const char PROGMEM frame2[] = {
    0,  0,248, 68, 68, 68, 56,128, 64, 64,128,  0,128, 64, 64,128,  0,128, 64, 64,128,  0,  0,  0,  0,192,  0, 64,192, 64,  0,  0,  0,  0, 15,  0,  0,  0,  0, 15,  1,  1, 15,  0, 15,  2,130, 65,128, 15,  1,  1, 15,  0,  4,  8,  8,  7,  0,  8, 15,  8,  0,  0,  0,  0,128,128,128,128,128,  0,  0,  0,  0,  0,  0,  0, 13, 18,  8,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,127,  4,  4,  4,  0,124, 20,104,  0, 68,124, 68,  0,124, 84, 68,  0,124, 24, 48,124,  0,124, 68, 56,  0, 72, 84, 36, 
    0,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,  0,  0,  4,252,  0,  0,  0,  0,192,160,160,192,  0, 32,224, 32,  0,224,  0,  0,  0,192,160,160,192,  0,  0,  0,  0,  0,  0,  0,  0,224, 16, 17,137,121,  9,136,  9,  8,136,  9,120,137, 17, 17,224,  1,  1,  1,  0,  1,  0,  0,  1,  0,  0,  0,192, 32,224,  0,  0,  0,  3,  4,123,128, 16,112,150,150,112, 16,  0,  3, 12, 11,  8, 16, 16, 16, 16, 16, 16, 16, 16, 16, 48, 46, 65,166, 25, 
    0,  0,  0,  0,  0,  0, 31, 96,192,  0, 16,160, 64,128,  0, 16,224, 64, 64, 32, 32, 16, 16, 16,120,128,  0,  0,  0,  0,240, 15,  0,  0,  0,  0,  0,  0, 96, 80, 79, 64, 96,159, 64, 63,  0,128,127,  0,  0,  0, 96, 80, 80, 76, 35,208,175,162,156,192, 32, 31,  0,  0,  0,  8,248, 72, 72, 72,176,  0, 64,193, 65,  1,193,  1,  0,  0,192,  0,  0,  0,192,  0,  0,192,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  3,  2,  2,  2,  1,  0,  2,  3,  2,  0,  3,226, 18, 32,195,  2,  2,  0,  1,  2,194, 33, 16, 96,128,  0,  0,  0, 
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,240, 14,  1,  0, 16,  8,  9, 17,  1,  1, 17,  9,  8, 16,  0,  7, 24, 32,192,  0,  0,128,126,  1,129,126,  0,  0,  0,  0,  0,131,100, 40, 24, 16, 16, 32, 32, 32, 34, 38, 34, 32, 32, 32, 16,112,136,  6,  1,  0,  0, 15, 48, 64,143,144,224, 16, 16, 12,  3,  0,  0,  0,224,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,192, 63,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  3,  4,  4,  8,  8, 16, 16, 57, 38, 32, 32, 63, 32, 32, 32, 32, 32, 63, 32, 32, 39, 56,  0,  0, 
    0,
};

static const char PROGMEM frame3[] = {
    0,  0,248, 68, 68, 68, 56,128, 64, 64,128,  0,128, 64, 64,128,  0,128, 64, 64,128,  0,  0,  0,  0,192,  0, 64,192, 64,  0,  0,  0,  0, 15,  0,  0,  0,  0, 15,  1,  1, 15,  0, 15,  2,130, 65,128, 15,  1,  1, 15,  0,  4,  8,  8,  7,  0,  8, 15,  8,  0,  0,  0,  0,128,128,128,128,128,  0,  0,  0,  0,  0,  0,  0, 13, 18,  8,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,127,  4,  4,  4,  0,124, 20,104,  0, 68,124, 68,  0,124, 84, 68,  0,124, 24, 48,124,  0,124, 68, 56,  0, 72, 84, 36, 
    0,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,  0,  0,  4,252,  0,  0,  0,  0,192,160,160,192,  0, 32,224, 32,  0,224,  0,  0,  0,192,160,160,192,  0,  0,  0,  0,  0,  0,  0,  0,224, 16, 17,137,121,  9,136,  9,  8,136,  9,120,137, 17, 17,224,  1,  1,  1,  0,  1,  0,  0,  1,  0,  0,  0,  0,128,128,  0,  0,  0,  3,  4,123,128, 16,112,150,150,112, 16,  0,  3, 12, 11,  8, 16, 16, 16, 16, 16, 16, 16, 16, 16, 48, 46, 65,160, 30, 
    1,  0,  0,  0,  0,  0, 31, 96,192,  0, 16,160, 64,128,  0, 16,224, 64, 64, 32, 32, 16, 16, 16,120,128,  0,  0,  0,  0,240, 15,  0,  0,  0,  0,  0,  0, 96, 80, 79, 64, 96,159, 64, 63,  0,128,127,  0,  0,  0, 96, 80, 80, 76, 35,208,175,162,156,192, 32, 31,  0,  0,  0,  8,248, 72, 72, 72,176,  0, 64,193, 65,  1,193,  1,  0,  0,192,  0,  0,  0,192,  0,  0,192,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  3,  2,  2,  2,  1,  0,  2,  3,  2,  0,  3,226, 18, 32,195,  2,  2,  0,  1,  2,194, 33, 16, 96,128,  0,  0,  0, 
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,240, 14,  1,  0, 16,  8,  9, 17,  1,  1, 17,  9,  8, 16,  0,  7, 24, 32,192,  0,  6,154, 98,  4,152, 96,  0,  0,  0,  0,  0,131,100, 40, 24, 16, 16, 32, 32, 32, 34, 38, 34, 32, 32, 32, 16,112,136,  6,  1,  0,  0, 15, 48, 64,143,144,224, 16, 16, 12,  3,  0,  0,  0,224,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,192, 63,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  3,  4,  4,  8,  8, 16, 16, 57, 38, 32, 32, 63, 32, 32, 32, 32, 32, 63, 32, 32, 39, 56,  0,  0, 
    0,
};

#define ANIM_FRAME_TIME 100
int current_frame = 0;
int frame_length = 6;

void oled_render_friends(void) {
    static uint32_t anim_timer = 0;

    if (timer_elapsed32(anim_timer) > ANIM_FRAME_TIME) {
        anim_timer = timer_read32();
        current_frame = (current_frame + 1) % frame_length;
        oled_clear();
        switch(current_frame) {
            case 0:
                oled_write_raw_P(frame0, sizeof(frame0));
                break;
            case 1:
                oled_write_raw_P(frame1, sizeof(frame1));
                break;
            case 2:
                oled_write_raw_P(frame2, sizeof(frame2));
                break;
            case 3:
                oled_write_raw_P(frame3, sizeof(frame3));
                break;
            case 4:
                oled_write_raw_P(frame2, sizeof(frame2));
                break;
            case 5:
                oled_write_raw_P(frame1, sizeof(frame1));
                break;
            default:
                oled_write_raw_P(frame0, sizeof(frame0));
                break;
        }   
    }
}

oled_rotation_t oled_init_user(oled_rotation_t rotation) {
    if (!is_keyboard_master()) {
      return OLED_ROTATION_180;  // flips the display 180 degrees if offhand
    }
    else {
       return OLED_ROTATION_270; 
    }
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

void oled_render_clock(void) {
    bool skip = false;
    for (int i = 0; i < 19; i++) {
        if (time_code[i] == 0 || skip) {
            skip = true;
            oled_write_char(code_to_name[0], false);
        } else {
            oled_write_char(code_to_name[time_code[i]], false);
        }
    }
}

void oled_render_media(void) {
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
        if (reset_oled) {
            oled_clear();
            reset_oled = false;
        }
        oled_render_friends();
        // switch (oled_state) {
        //     case OLED_CLOCK:
        //         oled_render_clock();
        //         break;
        //     case OLED_MEDIA:
        //         oled_render_media();
        //         break;
        //     case OLED_LAYER:
        //         oled_render_layer_state();
        //         break;
        //     default:
        //         oled_render_clock();
        // }
    }
    return false;
}
#endif // OLED_ENABLE
