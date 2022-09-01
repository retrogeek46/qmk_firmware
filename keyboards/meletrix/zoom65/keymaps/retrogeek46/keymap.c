/* Copyright 2021 meletrix
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include QMK_KEYBOARD_H
#include "raw_hid.h"
#include <print.h>
#include "midi.h"

extern MidiDevice midi_device;

#define MIDI_CC_OFF 0
#define MIDI_CC_ON  127

enum layers {
    _QWERTY,
    _COLEMAK_DH,
    _MACOS,
    _MIDI,
    _FUNC,
    _MAPS,
    NUMBER_OF_LAYERS
};

enum my_keycodes {
  ENC_MODE = SAFE_RANGE,
  MOU_MODE,
  TEST_RGB,
  LYR_STATE
};

enum encoder_modes {
    ENC_MODE_0,
    ENC_MODE_1,
    _NUM_OF_ENC_MODES,
    ENC_MODE_2
};

uint8_t encoder_mode = ENC_MODE_0;
uint8_t current_os = 0;
uint8_t layer_to_switch = 0;
bool mouseEnabled = false;

#define MAC_HME LGUI(KC_LEFT)
#define MAC_END LGUI(KC_RGHT)
#define M_CAP_ALL LSG(KC_3)
#define M_CAP_PART LSG(KC_4)

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [_QWERTY] = LAYOUT_all(
        KC_ESC,    KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,    KC_8,    KC_9,    KC_0,      KC_MINS, KC_EQL,  KC_BSPC, KC_BSPC, XXXXXXX, KC_MUTE, XXXXXXX,
        KC_TAB,    KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,      KC_LBRC, KC_RBRC, KC_BSLS, KC_DEL,
        MO(5),     KC_A,    KC_S,    KC_D,    KC_F,    KC_G,    KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN,   KC_QUOT,          KC_ENT,  KC_HOME,
        KC_LSFT,   KC_LSFT, KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_N,    KC_M,    KC_COMM, KC_DOT,    KC_SLSH, KC_RSFT, KC_UP,   KC_END,
        KC_LCTL,   KC_LGUI, KC_LALT,                   KC_SPC,  KC_BSPC, MO(5),            KC_APP,  MO(_FUNC),          KC_LEFT, KC_DOWN, KC_RGHT
    ),
    [_COLEMAK_DH] = LAYOUT_all(
        KC_GRV,    KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,    KC_8,    KC_9,    KC_0,      KC_MINS, KC_EQL,  KC_BSPC, KC_BSPC, XXXXXXX, KC_MUTE, XXXXXXX,
        KC_TAB,    KC_Q,    KC_W,    KC_F,    KC_P,    KC_B,    KC_J,    KC_L,    KC_U,    KC_Y,    KC_SCLN,   KC_LBRC, KC_RBRC, KC_BSLS, KC_DEL,
        MO(5),     KC_A,    KC_R,    KC_S,    KC_T,    KC_G,    KC_M,    KC_N,    KC_E,    KC_I,    KC_O,      KC_QUOT,          KC_ENT,  KC_HOME,
        KC_LSFT,   KC_LSFT, KC_Z,    KC_X,    KC_C,    KC_D,    KC_V,    KC_K,    KC_H,    KC_COMM, KC_DOT,    KC_SLSH, KC_RSFT, KC_UP,   KC_END,
        KC_LCTL,   KC_LGUI, KC_LALT,                   KC_SPC,  KC_BSPC, MO(5),            KC_APP,  MO(_FUNC),          KC_LEFT, KC_DOWN, KC_RGHT
    ),
    [_MACOS] = LAYOUT_all(
        KC_GRV,    KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,    KC_8,    KC_9,    KC_0,      KC_MINS, KC_EQL,  KC_BSPC, KC_BSPC, XXXXXXX, KC_MUTE, XXXXXXX,
        KC_TAB,    KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,      KC_LBRC, KC_RBRC, KC_BSLS, KC_DEL,
        MO(5),     KC_A,    KC_S,    KC_D,    KC_F,    KC_G,    KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN,   KC_QUOT,          KC_ENT,  MAC_HME,
        KC_LSFT,   KC_LSFT, KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_N,    KC_M,    KC_COMM, KC_DOT,    KC_SLSH, KC_RSFT, KC_UP,   MAC_END,
        KC_LCTL,   KC_LALT, KC_LGUI,                   KC_SPC,  KC_BSPC, MO(5),            KC_APP,  MO(_FUNC),          KC_LEFT, KC_DOWN, KC_RGHT
    ),
    [_MIDI] = LAYOUT_all(
        TO(0),     XXXXXXX, MI_Cs,   MI_Ds,   XXXXXXX, MI_Fs,   MI_Gs,   MI_As,   XXXXXXX, MI_Cs_1, MI_Ds_1,   XXXXXXX, XXXXXXX, _______, _______, XXXXXXX, KC_MUTE, XXXXXXX,
        MI_OCTU,   MI_C,    MI_D,    MI_E,    MI_F,    MI_G,    MI_A,    MI_B,    MI_C_1,  MI_D_1,  MI_E_1,    XXXXXXX, XXXXXXX, XXXXXXX, _______,
        MO(5),     XXXXXXX, XXXXXXX, XXXXXXX, MI_Fs_1, MI_Gs_1, MI_As_1, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,   XXXXXXX,          XXXXXXX, _______,
        MI_OCTD,   XXXXXXX, XXXXXXX, XXXXXXX, MI_F_1,  MI_G_1,  MI_A_1,  MI_B_1,  MI_C_2,  XXXXXXX, XXXXXXX,   XXXXXXX, XXXXXXX, _______, _______,
        _______,   _______, _______,                   _______, _______, _______,          _______, _______,            _______, _______, _______
    ),

    [_FUNC] = LAYOUT_all(
        _______,   KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,    KC_F11,  KC_F12,  KC_PSCR, KC_SLEP, XXXXXXX, KC_MUTE, XXXXXXX,
        _______,   _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,   _______, _______, _______, _______,
        _______,   _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,   _______,          _______, KC_PGUP,
        _______,   _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,   _______, _______, QK_BOOT, KC_PGDN,
        _______,   _______, _______,                   _______, _______, _______,          _______, _______,            _______, _______, _______
    ),

    [_MAPS] = LAYOUT_all(
        ENC_MODE,  TO(0)  , TO(1)  , TO(2)  , TO(3)  , TO(4)  , _______, _______, _______, _______, _______,   _______, _______, _______, _______, XXXXXXX, KC_MUTE, XXXXXXX,
        _______,   _______, _______, _______, _______, _______, _______, _______, KC_UP,   _______, _______,   _______, _______, _______, _______,
        _______,   _______, _______, KC_F18,  _______, _______, _______, KC_LEFT, KC_DOWN, KC_RGHT, _______,   _______,          _______, _______,
        _______,   _______, _______, KC_F17,  _______, _______, _______, _______, _______, _______, _______,   _______, _______, KC_MS_U, _______,
        _______,   _______, _______,                   _______, _______, _______,          _______, _______,            KC_MS_L, KC_MS_D, KC_MS_R
    ),
};

void keyboard_post_init_user(void) {
    // Customize these values to desired behaviour
    debug_enable=true;
    debug_keyboard=true;
}

// HID
void update_encoder_state(void) {
    // print("triggered through raw_hid");
    if (encoder_mode == _NUM_OF_ENC_MODES - 1) {
        encoder_mode = 0;
    } else {
        encoder_mode = encoder_mode + 1 % _NUM_OF_ENC_MODES;
    }
}

void send_keyboard_state(void) {
    uint8_t send_data[32] = {23};
    send_data[1] = encoder_mode + 1;
    send_data[2] = biton32(layer_state);
    send_data[3] = current_os;
    raw_hid_send(send_data, sizeof(send_data));
    // printf("sending encoder state %d %d %d\n", send_data[0], send_data[1], sizeof(send_data));
}

int get_next_layer(void) {
    layer_to_switch += 1 % NUMBER_OF_LAYERS;
    // printf("layer state val is %d, _QWERTY layer val is %d\n", layer_to_switch, _QWERTY);
    return layer_to_switch;
}

void update_os_state(uint8_t current_os_param) {
    // print("change current os");
    current_os = current_os_param;
    if (current_os_param == 1) {
        // print("change current os to mac\n");
        layer_move(_MACOS);
    } else {
        // print("change current os to win\n");
        layer_move(_QWERTY);
    }
}

void raw_hid_receive(uint8_t *data, uint8_t length) {
    switch(*data) {
        case 1:
            // test_rgb_value(255, 0, 0);
            break;
        case 10:
            switch(data[1]) {
                case 1:
                    update_encoder_state();
                    break;
                case 2:
                    send_keyboard_state();
                    break;
                case 3:
                    // set_cpu_usage_rgb(data[2]);
                    break;
                case 4:
                    update_os_state(data[2]);
                    break;
                case 5:
                    // test_rgb_value(data[2], data[3], data[4]);
                    break;
                case 6:
                    break;
            }
            break;
    }
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case LYR_STATE:
            if (record->event.pressed) {
                layer_to_switch = (layer_to_switch + 1) % (NUMBER_OF_LAYERS - 1);
                // printf("layer state is %d, layer var is %d \n", biton32(layer_state), layer_to_switch);
                layer_move(layer_to_switch);
            }
            return false;
        // toggle encoder states
        case ENC_MODE:
            print("detected keystroke");
            if (record->event.pressed) {
                if (encoder_mode == _NUM_OF_ENC_MODES - 1) {
                    encoder_mode = 0;
                } else {
                    encoder_mode = encoder_mode + 1 % _NUM_OF_ENC_MODES;
                }
            }
            return false;
        // os toggle stuff
        case KC_HOME:
            if (current_os == 1) {
                if (record->event.pressed) {
                    register_code(KC_LGUI);
                    register_code(KC_LEFT);
                } else {
                    unregister_code(KC_LGUI);
                    unregister_code(KC_LEFT);
                }
                return false;
            } else {
                return true;
            }
            return true;
        case KC_END:
            if (current_os == 1) {
                if (record->event.pressed) {
                    register_code(KC_LGUI);
                    register_code(KC_RIGHT);
                } else {
                    unregister_code(KC_LGUI);
                    unregister_code(KC_RIGHT);
                }
                return false;
            } else {
                return true;
            }
            return true;
        // toggle mouse mode
        case MOU_MODE:
            if (record->event.pressed) {
                mouseEnabled = !mouseEnabled;
            }
            return false;
        // if mouse mode is active, arrow keys will control cursor
        case KC_UP:
            if (mouseEnabled) {
                if (record->event.pressed) {
                    register_code(KC_MS_U);
                } else {
                    unregister_code(KC_MS_U);
                }
                return false;
            } else {
                return true;
            }
        case KC_LEFT:
            if (mouseEnabled) {
                if (record->event.pressed) {
                    register_code(KC_MS_L);
                } else {
                    unregister_code(KC_MS_L);
                }
                return false;
            } else {
                return true;
            }
        case KC_DOWN:
            if (mouseEnabled) {
                if (record->event.pressed) {
                    register_code(KC_MS_D);
                } else {
                    unregister_code(KC_MS_D);
                }
                return false;
            } else {
                return true;
            }
        case KC_RGHT:
            if (mouseEnabled) {
                if (record->event.pressed) {
                    register_code(KC_MS_R);
                } else {
                    unregister_code(KC_MS_R);
                }
                return false;
            } else {
                return true;
            }
        default:
            return true;
    }
};

#ifdef ENCODER_ENABLE
bool encoder_update_user(uint8_t index, bool clockwise) {
    printf("in encoder update user %d", encoder_mode);
    if (clockwise) {
        switch (encoder_mode) {
            case ENC_MODE_0:
                tap_code(KC_F24);
                break;
            case ENC_MODE_1:
                tap_code(KC_WH_D);
                break;
            case ENC_MODE_2:
                tap_code16(KC_VOLU);
                break;
            default:
                tap_code(KC_VOLU);
                break;
        }
    } else {
        switch (encoder_mode) {
            case ENC_MODE_0:
                tap_code(KC_F23);
                break;
            case ENC_MODE_1:
                tap_code(KC_WH_U);
                break;
            case ENC_MODE_2:
                tap_code16(KC_VOLD);
                break;
            default:
                tap_code(KC_VOLD);
                break;
        }
    }
    return true;
}
#endif // ENCODER_ENABLE