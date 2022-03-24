/* Copyright 2021 Retrogeek46

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
#include <print.h>
#include "raw_hid.h"
#include "openrgb.h"

enum layers {
    _QWERTY,
    _FUNC,
    _MAPS,
    _COLEMAK
};

enum my_keycodes {
  ENC_MODE = SAFE_RANGE,
  MOU_MODE
};

enum encoder_modes {
    ENC_MODE_0,
    ENC_MODE_1,
    _NUM_OF_ENC_MODES,
    ENC_MODE_2
};

uint8_t encoder_mode = ENC_MODE_0;
bool mouseEnabled = false;

// qk_tap_dance_action_t tap_dance_actions[] = {
//     [TD_SPCBAR] = ACTION_TAP_DANCE_DOUBLE(KC_SPC, KC_BSPC),
// };

// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

//      ESC      F1       F2       F3       F4       F5       F6       F7       F8       F9       F10      F11      F12      Prt           Rotary(Mute)
//      ~        1        2        3        4        5        6        7        8        9        0         -       (=)      BackSpc           Del
//      Tab      Q        W        E        R        T        Y        U        I        O        P        [        ]        \                 PgUp
//      Caps     A        S        D        F        G        H        J        K        L        ;        "                 Enter             PgDn
//      Sh_L              Z        X        C        V        B        N        M        ,        .        ?                 Sh_R     Up       End
//      Ct_L     Win_L    Alt_L                               SPACE                               Alt_R    FN       Ct_R     Left     Down     Right


    // The FN key by default maps to a momentary toggle to layer 1 to provide access to the RESET key (to put the board into bootloader mode). Without
    // this mapping, you have to open the case to hit the button on the bottom of the PCB (near the USB cable attachment) while plugging in the USB
    // cable to get the board into bootloader mode - definitely not fun when you're working on your QMK builds. Remove this and put it back to KC_RGUI
    // if that's your preference.
    //
    // To put the keyboard in bootloader mode, use FN+backslash. If you accidentally put it into bootloader, you can just unplug the USB cable and
    // it'll be back to normal when you plug it back in.
    //
    // This keyboard defaults to 6KRO instead of NKRO for compatibility reasons (some KVMs and BIOSes are incompatible with NKRO).
    // Since this is, among other things, a "gaming" keyboard, a key combination to enable NKRO on the fly is provided for convenience.
    // Press Fn+N to toggle between 6KRO and NKRO. This setting is persisted to the EEPROM and thus persists between restarts.
    [_QWERTY] = LAYOUT(
        KC_ESC,    KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,   KC_F7,   LWIN(KC_PSCR), MOU_MODE, KC_BTN1, KC_BTN3,   KC_BTN2, ENC_MODE,         KC_MPLY,
        KC_GRV,    KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,    KC_8,          KC_9,     KC_0,    KC_MINS,   KC_EQL,  KC_BSPC,          KC_DEL,
        KC_TAB,    KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,    KC_U,    KC_I,          KC_O,     KC_P,    KC_LBRC,   KC_RBRC, KC_BSLS,          KC_INS,
        MO(_MAPS), KC_A,    KC_S,    KC_D,    KC_F,    KC_G,    KC_H,    KC_J,    KC_K,          KC_L,     KC_SCLN, KC_QUOT,            KC_ENT,           KC_HOME,
        KC_LSFT,   KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_N,    KC_M,    KC_COMM,       KC_DOT,   KC_SLSH,            KC_RSFT,          KC_UP,   KC_END,
        KC_LCTL,   KC_LGUI, KC_LALT,                            KC_SPC,                                    KC_APP,  MO(_FUNC), KC_RCTL, KC_LEFT, KC_DOWN, KC_RGHT
    ),

    [_FUNC] = LAYOUT(
        _______, KC_MYCM, KC_WHOM, KC_CALC, KC_MSEL, KC_MPRV, KC_MNXT, KC_MPLY, KC_MSTP, KC_MUTE, KC_VOLD, KC_VOLU, _______, KC_PSCR,         KC_MPLY,
        _______, KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,  KC_F12,  _______,          _______,
        _______, _______, RGB_VAI, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, RESET,            _______,
        _______, _______, RGB_VAD, _______, _______, _______, _______, _______, _______, _______, _______, _______,          _______,          _______,
        _______, _______, RGB_HUI, _______, _______, _______, NK_TOGG, _______, _______, _______, _______,          RGB_TOG,          RGB_MOD, _______,
        _______, _______, _______,                            _______,                            _______, _______, _______, RGB_SPD, RGB_RMOD, RGB_SPI
    ),

    [_MAPS] = LAYOUT(
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,          _______,
        _______, KC_F13,   KC_F14,  KC_F15,  KC_F16, _______, _______, _______, _______, _______, _______, _______, _______, _______,          _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,          _______,
        _______, _______, _______,  KC_F18, _______, _______, _______, _______, _______, _______, _______, _______,          _______,          _______,
        _______, _______, _______,  KC_F17, _______, _______, _______, _______, _______, _______, _______,          _______,          KC_MS_U, _______,
        _______, _______, _______,                            _______,                            _______, _______, _______, KC_MS_L, KC_MS_D, KC_MS_R
    ),

    [_COLEMAK] = LAYOUT(
        KC_ESC,    KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,   KC_F7,   LWIN(KC_PSCR), MOU_MODE, KC_BTN1, KC_BTN3,   KC_BTN2, ENC_MODE,         KC_MPLY,
        KC_GRV,    KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,    KC_8,          KC_9,     KC_0,    KC_MINS,   KC_EQL,  KC_BSPC,          KC_DEL,
        KC_TAB,    KC_Q,    KC_W,    KC_F,    KC_P,    KC_G,    KC_J,    KC_L,    KC_U,          KC_Y,     KC_SCLN, KC_LBRC,   KC_RBRC, KC_BSLS,          KC_INS,
        MO(_MAPS), KC_A,    KC_R,    KC_S,    KC_T,    KC_D,    KC_H,    KC_N,    KC_E,          KC_I,     KC_O,    KC_QUOT,            KC_ENT,           KC_HOME,
        KC_LSFT,   KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_K,    KC_M,    KC_COMM,       KC_DOT,   KC_SLSH,            KC_RSFT,          KC_UP,   KC_END,
        KC_LCTL,   KC_LGUI, KC_LALT,                            KC_SPC,                                    KC_APP,  MO(_FUNC), KC_RCTL, KC_LEFT, KC_DOWN, KC_RGHT
    ),
};
// clang-format on

// HID Input
// bool is_hid_connected = false;

// void raw_hid_send_data(void) {
//     uint8_t data[32] = {0};
//     data[0] = encoder_mode + 1;
//     raw_hid_send(data, sizeof(data));
// }

// void raw_hid_receive(uint8_t *data, uint8_t length) {
//     is_hid_connected = true;

//     // Initial connections use '1' in the first byte to indicate this
//     if (length > 1 && data[0] == 1) {
//         raw_hid_send_data();
//         return;
//     } else {
//         if (encoder_mode == _NUM_OF_ENC_MODES - 1) {
//             encoder_mode = 0;
//         } else {
//             encoder_mode = encoder_mode + 1 % _NUM_OF_ENC_MODES;
//         }
//     }
// }

void update_encoder_state(void) {
    // print("trigerred through raw_hid");
    if (encoder_mode == _NUM_OF_ENC_MODES - 1) {
        encoder_mode = 0;
    } else {
        encoder_mode = encoder_mode + 1 % _NUM_OF_ENC_MODES;
    }
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    // print("some button pressed");
    switch (keycode) {
        // toggle encoder states
        case ENC_MODE:
            if (record->event.pressed) {
                // print("changing encoder state using designated button");
                if (encoder_mode == _NUM_OF_ENC_MODES - 1) {
                    encoder_mode = 0;
                } else {
                    encoder_mode = encoder_mode + 1 % _NUM_OF_ENC_MODES;
                }
            }
            return false;
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

void keyboard_post_init_user(void) {
    // Customise these values to desired behaviour
    // debug_enable=true;
    // debug_keyboard=true;
}

#ifdef RGB_MATRIX_ENABLE
void rgb_matrix_indicators_kb(void) {
    // left side
    rgb_matrix_set_color(67, 0, 0, 0);
    rgb_matrix_set_color(70, 0, 0, 0);
    rgb_matrix_set_color(73, 0, 0, 0);
    rgb_matrix_set_color(76, 0, 0, 0);
    rgb_matrix_set_color(80, 0, 0, 0);
    rgb_matrix_set_color(83, 0, 0, 0);
    rgb_matrix_set_color(87, 0, 0, 0);
    rgb_matrix_set_color(91, 0, 0, 0);
    // right side
    rgb_matrix_set_color(68, 0, 0, 0);
    rgb_matrix_set_color(71, 0, 0, 0);
    rgb_matrix_set_color(74, 0, 0, 0);
    rgb_matrix_set_color(77, 0, 0, 0);
    rgb_matrix_set_color(81, 0, 0, 0);
    rgb_matrix_set_color(84, 0, 0, 0);
    rgb_matrix_set_color(88, 0, 0, 0);
    rgb_matrix_set_color(92, 0, 0, 0);
    switch (encoder_mode) {
        case ENC_MODE_0:
            // print screen
            rgb_matrix_set_color(69, 0xff, 0xff, 0);
            // left side
            // rgb_matrix_set_color(67, 0, 0x80, 0);
            // rgb_matrix_set_color(70, 0, 0x80, 0);
            // rgb_matrix_set_color(73, 0, 0x80, 0);
            // rgb_matrix_set_color(76, 0, 0x80, 0);
            // rgb_matrix_set_color(80, 0, 0x80, 0);
            // rgb_matrix_set_color(83, 0, 0x80, 0);
            // rgb_matrix_set_color(87, 0, 0x80, 0);
            // rgb_matrix_set_color(91, 0, 0x80, 0);
            // right side
            // rgb_matrix_set_color(68, 0, 0x80, 0);
            // rgb_matrix_set_color(71, 0, 0x80, 0);
            // rgb_matrix_set_color(74, 0, 0x80, 0);
            // rgb_matrix_set_color(77, 0, 0x80, 0);
            // rgb_matrix_set_color(81, 0, 0x80, 0);
            // rgb_matrix_set_color(84, 0, 0x80, 0);
            // rgb_matrix_set_color(88, 0, 0x80, 0);
            // rgb_matrix_set_color(92, 0, 0x80, 0);
            break;
        case ENC_MODE_1:
            // print screen
            rgb_matrix_set_color(69, 0xff, 0, 0);
            // left side
            // rgb_matrix_set_color(67, 0xff, 0, 0);
            // rgb_matrix_set_color(70, 0xff, 0, 0);
            // rgb_matrix_set_color(73, 0xff, 0, 0);
            // rgb_matrix_set_color(76, 0xff, 0, 0);
            // rgb_matrix_set_color(80, 0xff, 0, 0);
            // rgb_matrix_set_color(83, 0xff, 0, 0);
            // rgb_matrix_set_color(87, 0xff, 0, 0);
            // rgb_matrix_set_color(91, 0xff, 0, 0);
            // right side
            // rgb_matrix_set_color(68, 0xff, 0, 0);
            // rgb_matrix_set_color(71, 0xff, 0, 0);
            // rgb_matrix_set_color(74, 0xff, 0, 0);
            // rgb_matrix_set_color(77, 0xff, 0, 0);
            // rgb_matrix_set_color(81, 0xff, 0, 0);
            // rgb_matrix_set_color(84, 0xff, 0, 0);
            // rgb_matrix_set_color(88, 0xff, 0, 0);
            // rgb_matrix_set_color(92, 0xff, 0, 0);
            break;
        case ENC_MODE_2:
            rgb_matrix_set_color(69, 0xff, 0x69, 0xB4);
            break;
        default:
            rgb_matrix_set_color(69, 0xff, 0xff, 0xff);
            break;
    }
    switch (biton32(layer_state)) {
        case _FUNC:
            // print("In _FUNC Layer, setting RGB");
            // Print Screen
            rgb_matrix_set_color(69, 0, 0, 0xff);
            // Num Row
            rgb_matrix_set_color(7,  0, 0, 0xff);
            rgb_matrix_set_color(13, 0, 0, 0xff);
            rgb_matrix_set_color(19, 0, 0, 0xff);
            rgb_matrix_set_color(24, 0, 0, 0xff);
            rgb_matrix_set_color(29, 0, 0, 0xff);
            rgb_matrix_set_color(35, 0, 0, 0xff);
            rgb_matrix_set_color(40, 0, 0, 0xff);
            rgb_matrix_set_color(45, 0, 0, 0xff);
            rgb_matrix_set_color(51, 0, 0, 0xff);
            rgb_matrix_set_color(57, 0, 0, 0xff);
            rgb_matrix_set_color(62, 0, 0, 0xff);
            rgb_matrix_set_color(78,                 0, 0, 0xff);
            // QMK RGB Controls
            rgb_matrix_set_color(14, 0, 0, 0xff);
            rgb_matrix_set_color(15, 0, 0, 0xff);
            rgb_matrix_set_color(16, 0, 0, 0xff);
            rgb_matrix_set_color(38, 0, 0, 0xff);
            rgb_matrix_set_color(90, 0, 0, 0xff);
            rgb_matrix_set_color(93, 0, 0, 0xff);
            // Arrows
            rgb_matrix_set_color(94, 0, 0, 0xff);
            rgb_matrix_set_color(95, 0, 0, 0xff);
            rgb_matrix_set_color(97, 0, 0, 0xff);
            rgb_matrix_set_color(79, 0, 0, 0xff);
            break;
        case _MAPS:
            // print("In _MAPS Layer, setting RGB");
            // 1-2-3-4
            rgb_matrix_set_color(7,  0, 0xff, 0);
            rgb_matrix_set_color(13, 0, 0xff, 0);
            rgb_matrix_set_color(19, 0, 0xff, 0);
            rgb_matrix_set_color(24, 0, 0xff, 0);
            // C-D
            rgb_matrix_set_color(22, 0, 0xff, 0);
            rgb_matrix_set_color(21, 0, 0xff, 0);
            // Arrows
            rgb_matrix_set_color(94, 0, 0xff, 0);
            rgb_matrix_set_color(95, 0, 0xff, 0);
            rgb_matrix_set_color(97, 0, 0xff, 0);
            rgb_matrix_set_color(79, 0, 0xff, 0);
            break;
    }
    if (mouseEnabled && biton32(layer_state) == _QWERTY) {
        // F9
        rgb_matrix_set_color(50, 0xff, 0, 0);
        // Arrows
        rgb_matrix_set_color(94, 0xf2, 0x52, 0x78);
        rgb_matrix_set_color(95, 0xf2, 0x52, 0x78);
        rgb_matrix_set_color(97, 0xf2, 0x52, 0x78);
        rgb_matrix_set_color(79, 0xf2, 0x52, 0x78);
    }
}
#endif

#ifdef ENCODER_ENABLE
bool encoder_update_user(uint8_t index, bool clockwise) {
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
