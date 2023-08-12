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

#define KC_CAPA LGUI(KC_PSCR)
#define KC_CAPS LGUI(S(KC_S))

enum layers {
    _BASE,
    _SYMB,
    _NUMP,
    _MAPS,
};

enum custom_keycodes {
    KC_OLED = SAFE_RANGE,
    KC_ENCD
};

enum oled_states {
    OLED_LAYER,
    OLED_CLOCK,
    OLED_MEDIA,
    _OLED_STATE_RANGE
};

enum encoder_modes {
    ENC_MODE_0,
    ENC_MODE_1,
    _NUM_OF_ENC_MODES,
    ENC_MODE_2
};

uint8_t encoder_mode = ENC_MODE_0;
uint8_t oled_state   = OLED_LAYER;
uint8_t ENC_RGB_0[3] = {200, 200, 200};
uint8_t ENC_RGB_1[3] = {0, 0, 110};
int     hid_code;
int     current_title_code[21];
int     current_artist_code[21];
int     time_code[21];
int     led_index = 0;
bool    media_updated;
bool    clock_updated;
bool    reset_oled = false;

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
  [0] = LAYOUT_split_3x6_3(
  //,-----------------------------------------------------.                    ,-----------------------------------------------------.
       KC_ESC,    KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,                         KC_Y,    KC_U,    KC_I,    KC_O,   KC_P,  KC_MPLY,
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
      KC_LCTL,    KC_A,    KC_S,    KC_D,    KC_F,    KC_G,                         KC_H,    KC_J,    KC_K,    KC_L, KC_SCLN, KC_QUOT,
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
      KC_LSFT,    KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,                         KC_N,    KC_M, KC_COMM,  KC_DOT, KC_SLSH, KC_BSLS,
  //|--------+--------+--------+--------+--------+--------+--------|  |--------+--------+--------+--------+--------+--------+--------|
                                           KC_SPC,   MO(2), KC_LALT,    KC_BSPC,   MO(1), KC_LGUI
                                      //`--------------------------'  `--------------------------'

  ),

  [1] = LAYOUT_split_3x6_3(
  //,-----------------------------------------------------.                    ,-----------------------------------------------------.
       KC_TAB,  KC_GRV, XXXXXXX, KC_LBRC, KC_RBRC, XXXXXXX,                      KC_OLED, KC_HOME,   KC_UP,  KC_END,  KC_INS, KC_SLEP,
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
      KC_LCTL, KC_MINS, KC_EQL,  KC_LPRN, KC_RPRN, XXXXXXX,                      KC_ENCD, KC_LEFT, KC_DOWN,KC_RIGHT,  KC_ENT,  KC_DEL,
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
      KC_LSFT, KC_UNDS, KC_PLUS,   KC_LT,   KC_GT, XXXXXXX,                      KC_CAPA, KC_CAPS, XXXXXXX, XXXXXXX, XXXXXXX, KC_LGUI,
  //|--------+--------+--------+--------+--------+--------+--------|  |--------+--------+--------+--------+--------+--------+--------|
                                           KC_SPC,   MO(3), KC_LALT,    KC_BSPC,   MO(1), KC_LGUI
                                      //`--------------------------'  `--------------------------'
  ),

  [2] = LAYOUT_split_3x6_3(
  //,-----------------------------------------------------.                    ,-----------------------------------------------------.
      QK_BOOT, XXXXXXX,    KC_7,    KC_8,    KC_9, KC_DEL ,                      XXXXXXX,   KC_F9,  KC_F10,  KC_F11,  KC_F12, KC_SLEP,
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
      KC_LCTL,    KC_0,    KC_4,    KC_5,    KC_6, KC_INS ,                      XXXXXXX,   KC_F5,   KC_F6,   KC_F7,   KC_F8, XXXXXXX,
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
      KC_LSFT,    KC_0,    KC_1,    KC_2,    KC_3, XXXXXXX,                      XXXXXXX,   KC_F1,   KC_F2,   KC_F3,   KC_F4, XXXXXXX,
  //|--------+--------+--------+--------+--------+--------+--------|  |--------+--------+--------+--------+--------+--------+--------|
                                           KC_SPC,   MO(2), KC_LALT,    KC_BSPC,   MO(3),  KC_APP
                                      //`--------------------------'  `--------------------------'
  ),

  [3] = LAYOUT_split_3x6_3(
  //,-----------------------------------------------------.                    ,-----------------------------------------------------.
      RGB_TOG, KC_ENCD, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                      XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
      KC_LCTL, RGB_HUI, RGB_SAI, RGB_VAI, XXXXXXX, XXXXXXX,                      XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
      RGB_MOD, RGB_HUD, RGB_SAD, RGB_VAD, XXXXXXX, XXXXXXX,                      XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
  //|--------+--------+--------+--------+--------+--------+--------|  |--------+--------+--------+--------+--------+--------+--------|
                                           KC_SPC,   MO(2), KC_LALT,    KC_BSPC,   MO(1),  KC_APP
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

// void enc_sync_receiver(uint8_t in_buflen, const void* in_data, uint8_t out_buflen, void* out_data) {
//     const enum encoder_modes* m2s = (const enum encoder_modes*) in_data;
//     // printf("in sync reciever method");
//     encoder_mode = *m2s;
// }

void keyboard_post_init_user(void) {
    // Customize these values to desired behaviour
    debug_enable = true;
    //debug_keyboard = true;

    // transaction_register_rpc(ENC_SYNC, enc_sync_receiver);
}

void update_encoder_state(void) {
    if (encoder_mode == _NUM_OF_ENC_MODES - 1) {
        encoder_mode = 0;
    } else {
        encoder_mode = encoder_mode + 1 % _NUM_OF_ENC_MODES;
    }
    // printf("exec-ing sync method");
    // transaction_rpc_exec(ENC_SYNC, sizeof(enum encoder_modes), &encoder_mode, 0, 0);
}

void send_keyboard_state(void) {
    uint8_t send_data[32] = {23};
    // send_data[1] = encoder_mode + 1;
    send_data[2] = biton32(layer_state);
    // send_data[3] = current_os;
    raw_hid_send(send_data, sizeof(send_data));
}

void test_rgb_value(int index) {
    led_index = index;
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
                    test_rgb_value(data[2]);
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
        case KC_ENCD:
            if (record->event.pressed) {
                if (encoder_mode == _NUM_OF_ENC_MODES - 1) {
                    encoder_mode = 0;
                } else {
                    encoder_mode = encoder_mode + 1 % _NUM_OF_ENC_MODES;
                }
                // transaction_rpc_exec(ENC_SYNC, sizeof(enum encoder_modes), &encoder_mode, 0, 0);
            }
            return false;
        default:
            return true;
    }
}

#ifdef RGB_MATRIX_ENABLE
#define NUM_LED_PER_SIDE 27
// bool rgb_matrix_indicators_kb(void) {
//     if (is_keyboard_master()) {
//         // rgb_matrix_set_color(led_index, 100, 0, 0);
//     } else {
//         if (encoder_mode == ENC_MODE_0) {
//             // printf("rgb enc 0");
//             rgb_matrix_set_color(24 + NUM_LED_PER_SIDE, ENC_RGB_0[0], ENC_RGB_0[1], ENC_RGB_0[2]);
//         } else if (encoder_mode == ENC_MODE_1) {
//             // printf("rgb enc 1");
//             rgb_matrix_set_color(24 + NUM_LED_PER_SIDE, ENC_RGB_1[0], ENC_RGB_1[1], ENC_RGB_1[2]);
//         }
//     }
//     return true;
// }
#endif

#ifdef OLED_ENABLE
oled_rotation_t oled_init_user(oled_rotation_t rotation) {
    if (!is_keyboard_master()) {
      return OLED_ROTATION_270;  // flips the display 180 degrees if offhand
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
    switch (encoder_mode) {
        case ENC_MODE_0:
            oled_write_ln_P(PSTR("Encoder: Seek"), false);
            break;
        case ENC_MODE_1:
            oled_write_ln_P(PSTR("Encoder: Scroll"), false);
            break;
        case ENC_MODE_2:
            oled_write_ln_P(PSTR("Encoder: AHK"), false);
            break;
    }
}

static const char PROGMEM saitama_ok[] = {
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,128,160,168,170,234,250,254,255,127,127,127,127,127,127,127,127,255,255,255,255,255,127,127,127,127,127,127,127,127,255,248,224,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,192,224,224,240,240,248,248,248,248,248,240,240,224,224,192,128,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,160,136,168,  8,170,170,170,255,255,255,255,243,237,221,221,221,221,213,221,237,243,255,127,255,243,237,221,221,221,221,213,221,237,243,255,255,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 64, 64,240,252,255,255,255,  7,251,251,251,  7,255,255,  3,223,175,119,251,255,255,255,252,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  2,  8, 10,138,162,170,191,255,255,255,255,255,255,255,255,255,255,255,255,255,239,224,255,255,255,255,255,255,255,255,255,255, 63,  7,  1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  7, 15, 31, 63,255,254,254,254,255,255,255,254,255,255,255,254,255,127, 31, 15,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  2, 10, 42,171,239,255,255,255,255,255,255,255,255,255,255,254,254,254,254,254,255,255,255,255,127, 31,  7,  1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  3,  3,  3,  3,  3,  1,  1,  1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
};

static const char PROGMEM master_sword[] = {
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,128,192,224,112,184, 88, 44, 22,  6,  2,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,  0,  0,  0,  0,  0,  0,  0,  0,  0,128,192,224,240,248,240,224,192,128,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,128,192,192,224,240,224,192, 64,192, 64, 64,192, 64, 64,192, 64, 64,192, 64, 64,192, 64, 64,224,240,241,227,207,254,253, 62,113,224,224,224,240,240,224,224,224, 96, 48, 24, 56,120,112,240,240,224,224, 96, 96,160,128,224,224,  0,128,224, 32,192,192,224,224,224, 96, 64,192,192,192, 64,  0,  0,  0,143,  7,  3,  1,  1,  1,129,225,241,217,109, 55, 27, 15,  7,  3,  1,  0,  0,  0,  0,  0, 16, 24, 28, 30, 31, 30, 28, 24, 16, 32, 16, 24, 28, 30, 31, 30, 28, 24, 16,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
    0,  0,  0,  0,  1,  3,  3,  7, 15,  7,  3,  2,  3,  2,  2,  3,  2,  2,  3,  2,  2,  3,  2,  2,  3,  2,  2,  7, 15,143,199,243,127,191,124,134,  7,  7,  7, 15, 15,  7,  7,  7,  6, 12, 24, 28, 30, 14, 15,  7,  1,  4,  6,  3,  3,  7,  4,  2,  1,  5,  4,  6,  7,  3,  3,  7,  7,  3,  3,  7,  7,  3,  2,  6,  7,129,193, 97,176,216,108, 63, 28,  4,  3,  0,  0,  0,128,192,  0,  4,252,252, 68,228,  4, 28,  0,  4,252,252,  4,  0,  0,  0,  0,  4,252,252,  4,  4,  8,240, 64,  0,  0,196,180,140,188,240,  0,  0,  0,  0, 
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  3,  7, 14, 29, 26, 52,104, 96, 64,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  4,  6,  7,  7,  7,  7,  7,  6,  6,  6,  6,  6,  6,  6,  6,  7,  7,  3,  0,  4,  7,  7,  4,  4,  4,  7,  0,  0,  7,  7,  4,  4,  6,  7,  0,  4,  7,  7,  4,  4,  2,  1,  0,  4,  7,  4,  0,  0,  4,  7,  7,  4,  0,  0,
};

// 'hat0', 32x128px
const char epd_bitmap_hat0 [] PROGMEM = {
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,255,255,255,255,255,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,255,255,255,255,255,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,255,255,255,255,255,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,255,255,255,255,255,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
};
// 'hat1', 32x128px
const char epd_bitmap_hat1 [] PROGMEM = {
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,128,192, 96, 96,112, 48, 48, 48,240,192,192,192,192,192,192,192,192,192,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,255,255,255,255,255,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        12, 12, 12, 12, 12, 12, 12, 12, 12,  6,  3,  1, 16,248,  0,  0,  0,  0,252,255, 48, 48, 48, 48, 48, 48, 48, 63, 63,248,248,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,255,255,255,255,255,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
        0,192,192,192,192,192,192,192,192,128,128,  0,  0,  0,  3,  6,  6,254,254,199,199,198,198,198,198,198,198,198,254,254,  7,  7,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,255,255,255,255,255,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
        0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  3,  3,  7, 14, 12, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 31, 31,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,255,255,255,255,255,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
        0,
    };
// 'hat2', 32x128px
const char epd_bitmap_hat2 [] PROGMEM = {
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,254, 10, 10, 14,  0,130,130,254,130,130,  0,158,146,242,  0,  2,  2,254,  2,  2,  0,  0,  0,  0,  0,128,192, 96, 96,112, 48, 48, 48,240,192,192,192,192,192,192,192,192,192,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,255,255,255,255,255,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12,  6,  3,  1, 16,248,  0,  0,  0,  0,252,255, 48, 48, 48, 48, 48, 48, 48, 63, 63,248,248,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,255,255,255,255,255,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
        0,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,128,128,  0,  0,  0,  3,  6,  6,254,254,199,199,198,198,198,198,198,198,198,254,254,  7,  7,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,255,255,255,255,255,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  3,  3,  7, 14, 12, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 31, 31,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,255,255,255,255,255,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
        0,
    };
// 'hat3', 32x128px
const char epd_bitmap_hat3 [] PROGMEM = {
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,254, 10, 10, 14,  0,130,130,254,130,130,  0,158,146,242,  0,  2,  2,254,  2,  2,  0,252,130,130,130,126,  0,254,128,128,128,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,128,192,224, 48, 48, 48, 48, 48, 48, 48, 48,240,240,192,192,192,192,192,192,192,192,192,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,255,255,255,255,255,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 14,  3,  3,  0,  0,  0,  0, 16,248,  0,  0,  0,  0,255,255, 48, 48, 48, 48, 48, 48, 48, 63, 63,248,248,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,255,255,255,255,255,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
        0,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,128,128,  0,  0,  0,  3,  6,  6,254,254,199,199,198,198,198,198,198,198,198,254,254,  7,  7,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,255,255,255,255,255,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  3,  3,  7, 30, 28, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 31, 31,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,255,255,255,255,255,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
        0,
    };
// 'hat4', 32x128px
const char epd_bitmap_hat4 [] PROGMEM = {
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,254, 10, 10, 14,  0,130,130,254,130,130,  0,158,146,242,  0,  2,  2,254,  2,  2,  0,252,130,130,130,126,  0,254,128,128,128,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,128,192,224, 48, 48, 48, 48, 48, 48, 48, 48,240,240,192,192,192,192,192,193,195,207,207, 63, 63, 48,112,112,240,240,120, 56, 60, 30, 15,  7,129,128,192,  0,  0,  0,  0,  0,
        12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 14,  3,  3,  0,  0,  0,  0, 16,248,  0,  0,  0,  0,255,255, 48, 48, 48, 48, 48, 48, 48, 63, 63,248,248,  0,  0,  0,  0,  0,  0,  0,  0,130,130,195, 65, 96, 32, 32,  0,  0,  0, 
        0,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,128,128,  0,  0,  0,  3,  6,  6,254,254,199,199,198,198,198,198,198,198,198,254,254,  7,  7,128,128,128,128,128,  0,  0,  0, 17, 48, 32, 32, 96, 64,192,128,  0,  0, 
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  3,  3,  7, 30, 28, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,152,223,223,192,248,126,255,  3,  3, 15, 15, 31, 30, 60,120,112, 96,  0,  0,  0,  0,  1,  0,  0, 
        0,
    };
// 'hat5', 32x128px
const char epd_bitmap_hat5 [] PROGMEM = {
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,254, 10, 10, 14,  0,130,130,254,130,130,  0,158,146,242,  0,  2,  2,254,  2,  2,  0,252,130,130,130,126,  0,254,128,128,128,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  3, 15, 15, 63, 63, 60, 60, 28, 28, 28, 12, 14,  6,  6,  7,  3,  1,  0,  0,  0,  0,  0,  0,  0,
        12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12,
        
        92,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,192,192,192,192,224,240,248, 56,120,240,224,224,224,192,192,128,  0,  0,  0,  0,  0,  0,  0,  0, 0,
    };

// Array of all bitmaps for convenience. (Total bytes used to store images in PROGMEM = 3168)
const int epd_bitmap_allArray_LEN = 6;
const char* epd_bitmap_allArray[6] = {
    epd_bitmap_hat0,
    epd_bitmap_hat1,
    epd_bitmap_hat2,
    epd_bitmap_hat3,
    epd_bitmap_hat4,
    epd_bitmap_hat5
};

void oled_render_static_art(void) {
    // oled_write_raw_P(saitama_ok, sizeof(saitama_ok));
    // oled_write_raw_P(master_sword, sizeof(master_sword));
    oled_write_raw_P(epd_bitmap_hat3, sizeof(epd_bitmap_hat3));
}

#define ANIM_FRAME_TIME 100
int current_frame = 0;
int frame_length = 5;

void oled_render_anim(void) {
    static uint32_t anim_timer = 0;

    if (timer_elapsed32(anim_timer) > ANIM_FRAME_TIME) {
        anim_timer = timer_read32();
        current_frame = (current_frame + 1) % frame_length;
        oled_clear();
        // oled_write_raw_P(epd_bitmap_allArray[current_frame], sizeof(epd_bitmap_allArray[current_frame]));
        switch(current_frame) {
            case 0:
                oled_write_raw_P(epd_bitmap_hat0, sizeof(epd_bitmap_hat0));
                break;
            case 1:
                oled_write_raw_P(epd_bitmap_hat1, sizeof(epd_bitmap_hat1));
                break;
            case 2:
                oled_write_raw_P(epd_bitmap_hat2, sizeof(epd_bitmap_hat2));
                break;
            case 3:
                oled_write_raw_P(epd_bitmap_hat3, sizeof(epd_bitmap_hat3));
                break;
            case 4:
                oled_write_raw_P(epd_bitmap_hat4, sizeof(epd_bitmap_hat4));
                break;
            case 5:
                oled_write_raw_P(epd_bitmap_hat5, sizeof(epd_bitmap_hat5));
                break;
            default:
                oled_write_raw_P(epd_bitmap_hat0, sizeof(epd_bitmap_hat0));
                break;
        }
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
    if (is_keyboard_master()) {
        oled_render_anim();
        // oled_render_static_art();
        // oled_scroll_right();
    } else {
        if (reset_oled) {
            oled_clear();
            reset_oled = false;
        }
        switch (oled_state) {
            case OLED_CLOCK:
                oled_render_clock();
                break;
            case OLED_MEDIA:
                oled_render_media();
                break;
            case OLED_LAYER:
                oled_render_layer_state();
                break;
            default:
                oled_render_layer_state();
        }
    }
    return false;
}
#endif // OLED_ENABLE

#ifdef ENCODER_ENABLE
bool encoder_update_user(uint8_t index, bool clockwise) {
    if (clockwise) {
        switch (encoder_mode) {
            case ENC_MODE_0:
                tap_code(KC_RIGHT);
                break;
            case ENC_MODE_1:
                tap_code16(KC_WH_D);
                break;
            case ENC_MODE_2:
                // tap_code(KC_F24);
                break;
            default:
                tap_code(KC_RIGHT);
                break;
        }
    } else {
        switch (encoder_mode) {
            case ENC_MODE_0:
                tap_code(KC_LEFT);
                break;
            case ENC_MODE_1:
                tap_code16(KC_WH_U);
                break;
            case ENC_MODE_2:
                // tap_code(KC_F23);
                break;
            default:
                tap_code(KC_LEFT);
                break;
        }
    }
    return false;
}
#endif // ENCODER_ENABLE
