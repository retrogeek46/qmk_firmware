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
    _FUNC_MAC,
    _MAPS,
    NUMBER_OF_LAYERS
};

enum my_keycodes {
  ENC_MODE = SAFE_RANGE,
  MOU_MODE,
  TEST_RGB,
  LYR_STATE,
  KNOB,
  RESET_VARS
};

enum encoder_modes {
    ENC_MODE_0,
    ENC_MODE_1,
    _NUM_OF_ENC_MODES,
    ENC_MODE_2
};

static uint8_t encoder_mode = ENC_MODE_0;
static uint8_t current_os = 0;
static uint8_t layer_to_switch = 0;
static bool bspc_pressed = false;
static bool mouseEnabled = false;

#define MAC_HME LGUI(KC_LEFT)
#define MAC_END LGUI(KC_RGHT)
#define M_CAPA LSG(KC_3)
#define M_CAPP LSG(KC_4)
#define W_CAPA LGUI(KC_PSCR)
#define W_CAPP LGUI(KC_PSCR)
#define L_BRCK S(KC_LBRC)
#define R_BRCK S(KC_RBRC)
#define L_ANGL S(KC_COMM)
#define R_ANGL S(KC_DOT)

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [_QWERTY] = LAYOUT_all(
        KC_ESC,    KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_MINS, KC_EQL,  KC_BSPC, KC_BSPC, XXXXXXX, KNOB, XXXXXXX,
        KC_TAB,    KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_LBRC, KC_RBRC, KC_BSLS, KC_DEL,
        MO(6),     KC_A,    KC_S,    KC_D,    KC_F,    KC_G,    KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT,          KC_ENT,  KC_HOME,
        KC_LSFT,   KC_LSFT, KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH, KC_RSFT, KC_UP,   KC_END,
        KC_LCTL,   KC_LGUI, KC_LALT,                   KC_SPC,  KC_BSPC, MO(4),            KC_APP,  MO(4),            KC_LEFT, KC_DOWN, KC_RGHT
    ),
    [_COLEMAK_DH] = LAYOUT_all(
        KC_ESC,    KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_MINS, KC_EQL,  KC_BSPC, KC_BSPC, XXXXXXX, KNOB, XXXXXXX,
        KC_TAB,    KC_Q,    KC_W,    KC_F,    KC_P,    KC_B,    KC_J,    KC_L,    KC_U,    KC_Y,    KC_SCLN, KC_LBRC, KC_RBRC, KC_BSLS, KC_DEL,
        MO(6),     KC_A,    KC_R,    KC_S,    KC_T,    KC_G,    KC_M,    KC_N,    KC_E,    KC_I,    KC_O,    KC_QUOT,          KC_ENT,  KC_HOME,
        KC_LSFT,   KC_LSFT, KC_Z,    KC_X,    KC_C,    KC_D,    KC_V,    KC_K,    KC_H,    KC_COMM, KC_DOT,  KC_SLSH, KC_RSFT, KC_UP,   KC_END,
        KC_LCTL,   KC_LGUI, KC_LALT,                   KC_SPC,  KC_BSPC, MO(4),            KC_APP,  MO(4),            KC_LEFT, KC_DOWN, KC_RGHT
    ),
    [_MACOS] = LAYOUT_all(
        KC_ESC,    KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_MINS, KC_EQL,  KC_BSPC, KC_BSPC, XXXXXXX, KNOB, XXXXXXX,
        KC_TAB,    KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_LBRC, KC_RBRC, KC_BSLS, KC_DEL,
        MO(6),     KC_A,    KC_S,    KC_D,    KC_F,    KC_G,    KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT,          KC_ENT,  MAC_HME,
        KC_LSFT,   KC_LSFT, KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH, KC_RSFT, KC_UP,   MAC_END,
        KC_LCTL,   KC_LALT, KC_LGUI,                   KC_SPC,  KC_BSPC, MO(4),            KC_APP,  MO(4),            KC_LEFT, KC_DOWN, KC_RGHT
    ),
    [_MIDI] = LAYOUT_all(
        TO(0),     XXXXXXX, MI_Cs,   MI_Ds,   XXXXXXX, MI_Fs,   MI_Gs,   MI_As,   XXXXXXX, MI_Cs_1, MI_Ds_1, XXXXXXX, XXXXXXX, _______, _______, XXXXXXX, KNOB, XXXXXXX,
        MI_OCTU,   MI_C,    MI_D,    MI_E,    MI_F,    MI_G,    MI_A,    MI_B,    MI_C_1,  MI_D_1,  MI_E_1,  XXXXXXX, XXXXXXX, XXXXXXX, _______,
        MO(6),     XXXXXXX, XXXXXXX, XXXXXXX, MI_Fs_1, MI_Gs_1, MI_As_1, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,          XXXXXXX, _______,
        MI_OCTD,   XXXXXXX, XXXXXXX, XXXXXXX, MI_F_1,  MI_G_1,  MI_A_1,  MI_B_1,  MI_C_2,  XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, _______, _______,
        _______,   _______, _______,                   _______, _______, _______,          _______, _______,          _______, _______, _______
    ),

    [_FUNC] = LAYOUT_all(
        KC_ESC,    KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,  KC_F12,  W_CAPA,  KC_SLEP, XXXXXXX, KNOB, XXXXXXX,
        _______,   KC_GRV,  _______, KC_LBRC, KC_RBRC, _______, _______, KC_HOME, KC_UP,   KC_END,  _______, _______, _______, _______, QK_BOOT,
        MO(6),     KC_MINS, KC_EQL,  S(KC_9), S(KC_0), _______, _______, KC_LEFT, KC_DOWN, KC_RGHT, KC_ENT,  _______,          _______, KC_PGUP,
        KC_LSFT,   KC_LSFT, _______, L_ANGL,  R_ANGL,  _______, _______, _______, _______, _______, _______, _______, _______, _______, KC_PGDN,
        _______,   _______, _______,                   _______, KC_LSFT, _______,          _______, _______,          _______, _______, _______
    ),

    [_FUNC_MAC] = LAYOUT_all(
        _______,   KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,  KC_F12,  W_CAPA,  KC_SLEP, XXXXXXX, KNOB, XXXXXXX,
        _______,   KC_GRV,  _______, KC_LBRC, KC_RBRC, _______, _______, MAC_HME, KC_UP,   MAC_END, KC_PSCR, _______, _______, _______, QK_BOOT,
        MO(6),     KC_MINS, KC_EQL,  S(KC_9), S(KC_0), _______, _______, KC_LEFT, KC_DOWN, KC_RGHT, KC_ENT,  _______,          _______, KC_PGUP,
        KC_LSFT,   KC_LSFT, _______, L_ANGL,  R_ANGL,  _______, _______, _______, _______, _______, _______, _______, _______, _______, KC_PGDN,
        _______,   _______, _______,                   _______, KC_LSFT, _______,          _______, _______,          _______, _______, _______
    ),

    [_MAPS] = LAYOUT_all(
        ENC_MODE,  TO(0)  , TO(1)  , TO(2)  , TO(3)  , TO(4)  , TO(5),   _______, _______, _______, _______, _______, _______, _______, _______, XXXXXXX, KNOB, XXXXXXX,
        _______,   _______, _______, _______, _______, _______, _______, KC_HOME, KC_MS_U, KC_END,  _______, _______, _______, _______, _______,
        _______,   _______, _______, KC_F18,  _______, _______, _______, KC_MS_L, KC_MS_D, KC_MS_R, _______, _______,          _______, W_CAPA,
        _______,   _______, _______, KC_F17,  _______, _______, _______, _______, _______, _______, _______, _______, _______, KC_MS_U, RESET_VARS,
        _______,   _______, _______,                   _______, _______, _______,          _______, _______,          KC_MS_L, KC_MS_D, KC_MS_R
    ),
};

void keyboard_post_init_user(void) {
    // Customize these values to desired behaviour
    debug_enable=true;
    debug_keyboard=true;
}

void cycle_encoder_state(void) {
    if (encoder_mode == _NUM_OF_ENC_MODES - 1) {
        encoder_mode = 0;
    } else {
        encoder_mode = encoder_mode + 1 % _NUM_OF_ENC_MODES;
    }
}

// HID
void update_encoder_state(void) {
    // print("triggered through raw_hid");
    cycle_encoder_state();
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
                    // set_cpu_usage_rgb(da     );
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

layer_state_t layer_state_set_user(layer_state_t state) {
    switch (get_highest_layer(state)) {
        case _MACOS:
            current_os = 1;
            break;
        case _QWERTY:
            current_os = 0;
            break;
    }
  return state;
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case RESET_VARS:
            if (record->event.pressed) {
                encoder_mode = ENC_MODE_0;
                current_os = 0;
                layer_to_switch = 0;
                bspc_pressed = false;
                mouseEnabled = false;
            }
            return false;
        case LYR_STATE:
            if (record->event.pressed) {
                layer_to_switch = (layer_to_switch + 1) % (NUMBER_OF_LAYERS - 1);
                // printf("layer state is %d, layer var is %d \n", biton32(layer_state), layer_to_switch);
                layer_move(layer_to_switch);
            }
            return false;
        // toggle encoder states
        case ENC_MODE:
            // print("detected keystroke");
            if (record->event.pressed) {
                cycle_encoder_state();
            }
            return false;
        // knob press
        case KNOB:
            if (record->event.pressed) {
                if (bspc_pressed) {
                    cycle_encoder_state();
                } else {
                    tap_code(KC_MPLY);
                }
            }
            return false;
        case KC_BSPC:
            if (record->event.pressed) {
                bspc_pressed = true;
            } else {
                bspc_pressed = false;
            }
            return true;
        default:
            return true;
    }
};

#ifdef ENCODER_ENABLE
bool encoder_update_user(uint8_t index, bool clockwise) {
    // print("in encoder update user\n");
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