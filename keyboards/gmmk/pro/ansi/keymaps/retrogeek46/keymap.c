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

int col_mod_8008[3] = {60, 71, 86};
int col_acc_8008[3] = {243, 75, 127};

int col_blue_susu[3] = {0, 255, 255};
int col_yellow_susu[3] = {200, 200, 0};
int col_red_susu[3] = {255, 0, 0};
int col_green_susu[3] = {50, 255, 0};

int cpu_rgb_68[3] = {255, 255, 255};
int cpu_rgb_71[3] = {255, 255, 255};
int cpu_rgb_74[3] = {255, 255, 255};
int cpu_rgb_77[3] = {255, 255, 255};
int cpu_rgb_81[3] = {255, 255, 255};
int cpu_rgb_84[3] = {255, 255, 255};
int cpu_rgb_88[3] = {255, 255, 255};
int cpu_rgb_92[3] = {255, 255, 255};

int col_midi_flat[3] = {255, 100, 255};
int col_midi_nat[3] = {255, 255, 255};

int col_test[3] = {255, 255, 255};
bool test_rgb = false;


// macos specific defines
#define MAC_HME LGUI(KC_LEFT)
#define MAC_END LGUI(KC_RGHT)
#define M_CAP_ALL LSG(KC_3)
#define M_CAP_PART LSG(KC_4)

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

    [_COLEMAK_DH] = LAYOUT(
        KC_ESC,    KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,   KC_F7,   LWIN(KC_PSCR), MOU_MODE, KC_BTN1, KC_BTN3,   KC_BTN2, ENC_MODE,         KC_MPLY,
        KC_GRV,    KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,    KC_8,          KC_9,     KC_0,    KC_MINS,   KC_EQL,  KC_BSPC,          KC_DEL,
        KC_TAB,    KC_Q,    KC_W,    KC_F,    KC_P,    KC_B,    KC_J,    KC_L,    KC_U,          KC_Y,     KC_SCLN, KC_LBRC,   KC_RBRC, KC_BSLS,          KC_INS,
        MO(_MAPS), KC_A,    KC_R,    KC_S,    KC_T,    KC_G,    KC_M,    KC_N,    KC_E,          KC_I,     KC_O,    KC_QUOT,            KC_ENT,           KC_HOME,
        KC_LSFT,   KC_Z,    KC_X,    KC_C,    KC_D,    KC_V,    KC_K,    KC_H,    KC_COMM,       KC_DOT,   KC_SLSH,            KC_RSFT,          KC_UP,   KC_END,
        KC_LCTL,   KC_LGUI, KC_LALT,                            KC_SPC,                                    KC_APP,  MO(_FUNC), KC_RCTL, KC_LEFT, KC_DOWN, KC_RGHT
    ),

    [_MACOS] = LAYOUT(
        KC_ESC,    KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,   M_CAP_PART, M_CAP_ALL,     MOU_MODE, KC_BTN1, KC_BTN3,   KC_BTN2, ENC_MODE,         KC_MPLY,
        KC_GRV,    KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,       KC_8,          KC_9,     KC_0,    KC_MINS,   KC_EQL,  KC_BSPC,          KC_DEL,
        KC_TAB,    KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,    KC_U,       KC_I,          KC_O,     KC_P,    KC_LBRC,   KC_RBRC, KC_BSLS,          KC_INS,
        MO(_MAPS), KC_A,    KC_S,    KC_D,    KC_F,    KC_G,    KC_H,    KC_J,       KC_K,          KC_L,     KC_SCLN, KC_QUOT,            KC_ENT,           MAC_HME,
        KC_LSFT,   KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_N,    KC_M,       KC_COMM,       KC_DOT,   KC_SLSH,            KC_RSFT,          KC_UP,   MAC_END,
        KC_LCTL,   KC_LALT, KC_LGUI,                            KC_SPC,                                       KC_APP,  MO(_FUNC), KC_RCTL, KC_LEFT, KC_DOWN, KC_RGHT
    ),

    [_MIDI] = LAYOUT(
        TO(0),     _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,          _______,
        _______,   XXXXXXX, MI_Cs,   MI_Ds,   XXXXXXX, MI_Fs,   MI_Gs,   MI_As,   XXXXXXX, MI_Cs_1, MI_Ds_1, XXXXXXX, XXXXXXX, _______,          _______,
        MI_OCTU,   MI_C,    MI_D,    MI_E,    MI_F,    MI_G,    MI_A,    MI_B,    MI_C_1,  MI_D_1,  MI_E_1,  XXXXXXX, XXXXXXX, XXXXXXX,          _______,
        MO(_MAPS), XXXXXXX, XXXXXXX, XXXXXXX, MI_Fs_1, MI_Gs_1, MI_As_1, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,          XXXXXXX,          _______,
        MI_OCTD,   XXXXXXX, XXXXXXX, MI_F_1,  MI_G_1,  MI_A_1,  MI_B_1,  MI_C_2,  XXXXXXX, XXXXXXX, XXXXXXX,          XXXXXXX,          _______, _______,
        _______,   _______, _______,                            _______,                            _______, _______, _______, _______, _______, _______
    ),

    [_FUNC] = LAYOUT(
        _______, KC_MYCM, KC_WHOM, KC_CALC, KC_MSEL, KC_MPRV, KC_MNXT,  KC_MPLY, KC_MSTP, KC_MUTE, KC_VOLD, KC_VOLU, _______, KC_PSCR,           KC_MPLY,
        _______, KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,    KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,  KC_F12,  _______,           _______,
        _______, _______, RGB_VAI, _______, _______, _______, _______,  _______, _______, _______, _______, _______, _______, RESET,             _______,
        _______, _______, RGB_VAD, _______, _______, _______, TEST_RGB, _______, _______, _______, _______, _______,          _______,           KC_PGUP,
        _______, _______, RGB_HUD, RGB_HUI, _______, _______, NK_TOGG,  _______, _______, _______, _______,          RGB_TOG,          RGB_MOD,  KC_PGDN,
        _______, _______, _______,                            _______,                             _______, _______, _______, RGB_SPD, RGB_RMOD, RGB_SPI
    ),

    [_MAPS] = LAYOUT(
        _______, TO(0)  , TO(1)  , TO(2)  , TO(3)  , TO(4)  , _______, _______, _______, _______, _______, _______, _______, _______,          _______,
        _______, KC_F13 ,  KC_F14,  KC_F15,  KC_F16, _______, _______, _______, _______, _______, _______, _______, _______, _______,          LYR_STATE,
        _______, _______, _______, _______, _______, _______, _______, _______, KC_UP,   _______, _______, _______, _______, _______,          _______,
        _______, _______, _______,  KC_F18, _______, _______, _______, KC_LEFT, KC_DOWN, KC_RGHT, _______, _______,          _______,          _______,
        _______, _______, _______,  KC_F17, _______, _______, _______, _______, _______, _______, _______,          _______,          KC_MS_U, _______,
        _______, _______, _______,                            _______,                            _______, _______, _______, KC_MS_L, KC_MS_D, KC_MS_R
    ),
};
// clang-format on

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

void send_encoder_state(void) {
    uint8_t send_data[32] = {23};
    send_data[0] = encoder_mode + 1;
    raw_hid_send(send_data, sizeof(send_data));
    // printf("sending encoder state %d %d %d", send_data[0], send_data[1], sizeof(send_data));
    // print(send_data[0]);
    // printf(encoder_mode + 1 + '0');
}

void cpu_temp_rgb_helper(int col[], int led_index) {
    switch (led_index) {
        case 0:
            // cpu_rgb_R_68 = r; cpu_rgb_G_68 = g; cpu_rgb_B_68 = b;
            // cpu_rgb_R_71 = r; cpu_rgb_G_71 = g; cpu_rgb_B_71 = b;
            // cpu_rgb_R_74 = r; cpu_rgb_G_74 = g; cpu_rgb_B_74 = b;
            // cpu_rgb_R_77 = r; cpu_rgb_G_77 = g; cpu_rgb_B_77 = b;
            // cpu_rgb_R_81 = r; cpu_rgb_G_81 = g; cpu_rgb_B_81 = b;
            // cpu_rgb_R_84 = r; cpu_rgb_G_84 = g; cpu_rgb_B_84 = b;
            // cpu_rgb_R_88 = r; cpu_rgb_G_88 = g; cpu_rgb_B_88 = b;
            // cpu_rgb_R_92 = r; cpu_rgb_G_92 = g; cpu_rgb_B_92 = b;
            cpu_rgb_68[0] = col[0]; cpu_rgb_68[1] = col[1]; cpu_rgb_68[2] = col[2];
            cpu_rgb_71[0] = col[0]; cpu_rgb_71[1] = col[1]; cpu_rgb_71[2] = col[2];
            cpu_rgb_74[0] = col[0]; cpu_rgb_74[1] = col[1]; cpu_rgb_74[2] = col[2];
            cpu_rgb_77[0] = col[0]; cpu_rgb_77[1] = col[1]; cpu_rgb_77[2] = col[2];
            cpu_rgb_81[0] = col[0]; cpu_rgb_81[1] = col[1]; cpu_rgb_81[2] = col[2];
            cpu_rgb_84[0] = col[0]; cpu_rgb_84[1] = col[1]; cpu_rgb_84[2] = col[2];
            cpu_rgb_88[0] = col[0]; cpu_rgb_88[1] = col[1]; cpu_rgb_88[2] = col[2];
            cpu_rgb_92[0] = col[0]; cpu_rgb_92[1] = col[1]; cpu_rgb_92[2] = col[2];
            break;
        case 68:
            // cpu_rgb_R_68 = r; cpu_rgb_G_68 = g; cpu_rgb_B_68 = b;
            cpu_rgb_68[0] = col[0]; cpu_rgb_68[1] = col[1]; cpu_rgb_68[2] = col[2];
            break;
        case 71:
            // cpu_rgb_R_71 = r; cpu_rgb_G_71 = g; cpu_rgb_B_71 = b;
            cpu_rgb_71[0] = col[0]; cpu_rgb_71[1] = col[1]; cpu_rgb_71[2] = col[2];
            break;
        case 74:
            // cpu_rgb_R_74 = r; cpu_rgb_G_74 = g; cpu_rgb_B_74 = b;
            cpu_rgb_74[0] = col[0]; cpu_rgb_74[1] = col[1]; cpu_rgb_74[2] = col[2];
            break;
        case 77:
            // cpu_rgb_R_77 = r; cpu_rgb_G_77 = g; cpu_rgb_B_77 = b;
            cpu_rgb_77[0] = col[0]; cpu_rgb_77[1] = col[1]; cpu_rgb_77[2] = col[2];
            break;
        case 81:
            // cpu_rgb_R_81 = r; cpu_rgb_G_81 = g; cpu_rgb_B_81 = b;
            cpu_rgb_81[0] = col[0]; cpu_rgb_81[1] = col[1]; cpu_rgb_81[2] = col[2];
            break;
        case 84:
            // cpu_rgb_R_84 = r; cpu_rgb_G_84 = g; cpu_rgb_B_84 = b;
            cpu_rgb_84[0] = col[0]; cpu_rgb_84[1] = col[1]; cpu_rgb_84[2] = col[2];
            break;
        case 88:
            // cpu_rgb_R_88 = r; cpu_rgb_G_88 = g; cpu_rgb_B_88 = b;
            cpu_rgb_88[0] = col[0]; cpu_rgb_88[1] = col[1]; cpu_rgb_88[2] = col[2];
            break;
        case 92:
            // cpu_rgb_R_92 = r; cpu_rgb_G_92 = g; cpu_rgb_B_92 = b;
            cpu_rgb_92[0] = col[0]; cpu_rgb_92[1] = col[1]; cpu_rgb_92[2] = col[2];
            break;
        default:
            break;
    }
}

void set_cpu_usage_rgb(uint8_t cpu_usage) {
    if (cpu_usage < 10) {
        cpu_temp_rgb_helper(col_yellow_susu, 68);
        cpu_temp_rgb_helper(col_yellow_susu, 71);
        cpu_temp_rgb_helper(col_yellow_susu, 74);
        cpu_temp_rgb_helper(col_yellow_susu, 77);
        cpu_temp_rgb_helper(col_yellow_susu, 81);
        cpu_temp_rgb_helper(col_yellow_susu, 84);
        cpu_temp_rgb_helper(col_yellow_susu, 88);
        cpu_temp_rgb_helper(col_yellow_susu, 92); } else
    if (cpu_usage >= 10 && cpu_usage < 30) {
        cpu_temp_rgb_helper(col_yellow_susu, 68);
        cpu_temp_rgb_helper(col_yellow_susu, 71);
        cpu_temp_rgb_helper(col_yellow_susu, 74);
        cpu_temp_rgb_helper(col_yellow_susu, 77);
        cpu_temp_rgb_helper(col_yellow_susu, 81);
        cpu_temp_rgb_helper(col_yellow_susu, 84);
        cpu_temp_rgb_helper(col_yellow_susu, 88);
        cpu_temp_rgb_helper(col_red_susu, 92); } else
    if (cpu_usage >= 30 && cpu_usage < 40) {
        cpu_temp_rgb_helper(col_yellow_susu, 68);
        cpu_temp_rgb_helper(col_yellow_susu, 71);
        cpu_temp_rgb_helper(col_yellow_susu, 74);
        cpu_temp_rgb_helper(col_yellow_susu, 77);
        cpu_temp_rgb_helper(col_yellow_susu, 81);
        cpu_temp_rgb_helper(col_yellow_susu, 84);
        cpu_temp_rgb_helper(col_red_susu, 88);
        cpu_temp_rgb_helper(col_red_susu, 92); } else
    if (cpu_usage >= 40 && cpu_usage < 50) {
        cpu_temp_rgb_helper(col_yellow_susu, 68);
        cpu_temp_rgb_helper(col_yellow_susu, 71);
        cpu_temp_rgb_helper(col_yellow_susu, 74);
        cpu_temp_rgb_helper(col_yellow_susu, 77);
        cpu_temp_rgb_helper(col_yellow_susu, 81);
        cpu_temp_rgb_helper(col_red_susu, 84);
        cpu_temp_rgb_helper(col_red_susu, 88);
        cpu_temp_rgb_helper(col_red_susu, 92); } else
    if (cpu_usage >= 50 && cpu_usage < 60) {
        cpu_temp_rgb_helper(col_yellow_susu, 68);
        cpu_temp_rgb_helper(col_yellow_susu, 71);
        cpu_temp_rgb_helper(col_yellow_susu, 74);
        cpu_temp_rgb_helper(col_yellow_susu, 77);
        cpu_temp_rgb_helper(col_red_susu, 81);
        cpu_temp_rgb_helper(col_red_susu, 84);
        cpu_temp_rgb_helper(col_red_susu, 88);
        cpu_temp_rgb_helper(col_red_susu, 92); } else
    if (cpu_usage >= 60 && cpu_usage < 70) {
        cpu_temp_rgb_helper(col_yellow_susu, 68);
        cpu_temp_rgb_helper(col_yellow_susu, 71);
        cpu_temp_rgb_helper(col_yellow_susu, 74);
        cpu_temp_rgb_helper(col_red_susu, 77);
        cpu_temp_rgb_helper(col_red_susu, 81);
        cpu_temp_rgb_helper(col_red_susu, 84);
        cpu_temp_rgb_helper(col_red_susu, 88);
        cpu_temp_rgb_helper(col_red_susu, 92); } else
    if (cpu_usage >= 70 && cpu_usage < 80) {
        cpu_temp_rgb_helper(col_yellow_susu, 68);
        cpu_temp_rgb_helper(col_yellow_susu, 71);
        cpu_temp_rgb_helper(col_red_susu, 74);
        cpu_temp_rgb_helper(col_red_susu, 77);
        cpu_temp_rgb_helper(col_red_susu, 81);
        cpu_temp_rgb_helper(col_red_susu, 84);
        cpu_temp_rgb_helper(col_red_susu, 88);
        cpu_temp_rgb_helper(col_red_susu, 92); } else
    if (cpu_usage >= 80 && cpu_usage < 90) {
        cpu_temp_rgb_helper(col_yellow_susu, 68);
        cpu_temp_rgb_helper(col_red_susu, 71);
        cpu_temp_rgb_helper(col_red_susu, 74);
        cpu_temp_rgb_helper(col_red_susu, 77);
        cpu_temp_rgb_helper(col_red_susu, 81);
        cpu_temp_rgb_helper(col_red_susu, 84);
        cpu_temp_rgb_helper(col_red_susu, 88);
        cpu_temp_rgb_helper(col_red_susu, 92); } else
    if (cpu_usage >= 90) {
        cpu_temp_rgb_helper(col_red_susu, 68);
        cpu_temp_rgb_helper(col_red_susu, 71);
        cpu_temp_rgb_helper(col_red_susu, 74);
        cpu_temp_rgb_helper(col_red_susu, 77);
        cpu_temp_rgb_helper(col_red_susu, 81);
        cpu_temp_rgb_helper(col_red_susu, 84);
        cpu_temp_rgb_helper(col_red_susu, 88);
        cpu_temp_rgb_helper(col_red_susu, 92);
    }
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

void test_rgb_value(uint8_t r, uint8_t g, uint8_t b) {
    // printf("RGB Values %d %d %d", r, g, b);
    test_rgb = true;
    col_test[0] = r;
    col_test[1] = g;
    col_test[2] = b;
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
        case TEST_RGB:
            if (record->event.pressed) {
                test_rgb = !test_rgb;
            }
            return false;
        // toggle encoder states
        case ENC_MODE:
            // print("detected keystroke");
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

#ifdef RGB_MATRIX_ENABLE
void rgb_matrix_indicators_kb(void) {
    if (test_rgb) {
        rgb_matrix_set_color(0, col_test[0], col_test[1], col_test[2]);
        rgb_matrix_set_color(67, col_test[0], col_test[1], col_test[2]);
        rgb_matrix_set_color(70, col_test[0], col_test[1], col_test[2]);
        rgb_matrix_set_color(73, col_test[0], col_test[1], col_test[2]);
        rgb_matrix_set_color(76, col_test[0], col_test[1], col_test[2]);
        rgb_matrix_set_color(80, col_test[0], col_test[1], col_test[2]);
        rgb_matrix_set_color(83, col_test[0], col_test[1], col_test[2]);
        rgb_matrix_set_color(87, col_test[0], col_test[1], col_test[2]);
        rgb_matrix_set_color(91, col_test[0], col_test[1], col_test[2]);
    }
    // sidebars
    // // left side
    // rgb_matrix_set_color(67, cpu_rgb_R, cpu_rgb_G, cpu_rgb_B);
    // rgb_matrix_set_color(70, cpu_rgb_R, cpu_rgb_G, cpu_rgb_B);
    // rgb_matrix_set_color(73, cpu_rgb_R, cpu_rgb_G, cpu_rgb_B);
    // rgb_matrix_set_color(76, cpu_rgb_R, cpu_rgb_G, cpu_rgb_B);
    // rgb_matrix_set_color(80, cpu_rgb_R, cpu_rgb_G, cpu_rgb_B);
    // rgb_matrix_set_color(83, cpu_rgb_R, cpu_rgb_G, cpu_rgb_B);
    // rgb_matrix_set_color(87, cpu_rgb_R, cpu_rgb_G, cpu_rgb_B);
    // rgb_matrix_set_color(91, cpu_rgb_R, cpu_rgb_G, cpu_rgb_B);
    // right side
    rgb_matrix_set_color(68, cpu_rgb_68[0], cpu_rgb_68[1], cpu_rgb_68[2]);
    rgb_matrix_set_color(71, cpu_rgb_71[0], cpu_rgb_71[1], cpu_rgb_71[2]);
    rgb_matrix_set_color(74, cpu_rgb_74[0], cpu_rgb_74[1], cpu_rgb_74[2]);
    rgb_matrix_set_color(77, cpu_rgb_77[0], cpu_rgb_77[1], cpu_rgb_77[2]);
    rgb_matrix_set_color(81, cpu_rgb_81[0], cpu_rgb_81[1], cpu_rgb_81[2]);
    rgb_matrix_set_color(84, cpu_rgb_84[0], cpu_rgb_84[1], cpu_rgb_84[2]);
    rgb_matrix_set_color(88, cpu_rgb_88[0], cpu_rgb_88[1], cpu_rgb_88[2]);
    rgb_matrix_set_color(92, cpu_rgb_92[0], cpu_rgb_92[1], cpu_rgb_92[2]);
    switch (encoder_mode) {
        case ENC_MODE_0:
            // print screen
            rgb_matrix_set_color(69, col_blue_susu[0], col_blue_susu[1], col_blue_susu[2]);
            break;
        case ENC_MODE_1:
            // print screen
            rgb_matrix_set_color(69, col_red_susu[0], col_red_susu[1], col_red_susu[2]);
            break;
        case ENC_MODE_2:
            rgb_matrix_set_color(69, 0xff, 0x69, 0xB4);
            break;
        default:
            rgb_matrix_set_color(69, 0xff, 0xff, 0xff);
            break;
    }
    // if (current_os == 1) {
    //     rgb_matrix_set_color(0, 255, 255, 255);
    // }
    switch (biton32(layer_state)) {
        case _QWERTY:
            // Esc
            // rgb_matrix_set_color(0,  col_green_susu[0], col_green_susu[1], col_green_susu[2]);
            break;
        case _FUNC:
            // Esc
            rgb_matrix_set_color(0,  col_red_susu[0], col_red_susu[1], col_red_susu[2]);
            // print("In _FUNC Layer, setting RGB");
            // Print Screen
            rgb_matrix_set_color(69, col_blue_susu[0], col_blue_susu[1], col_blue_susu[2]);
            // Num Row
            rgb_matrix_set_color(7,  col_blue_susu[0], col_blue_susu[1], col_blue_susu[2]);
            rgb_matrix_set_color(13, col_blue_susu[0], col_blue_susu[1], col_blue_susu[2]);
            rgb_matrix_set_color(19, col_blue_susu[0], col_blue_susu[1], col_blue_susu[2]);
            rgb_matrix_set_color(24, col_blue_susu[0], col_blue_susu[1], col_blue_susu[2]);
            rgb_matrix_set_color(29, col_blue_susu[0], col_blue_susu[1], col_blue_susu[2]);
            rgb_matrix_set_color(35, col_blue_susu[0], col_blue_susu[1], col_blue_susu[2]);
            rgb_matrix_set_color(40, col_blue_susu[0], col_blue_susu[1], col_blue_susu[2]);
            rgb_matrix_set_color(45, col_blue_susu[0], col_blue_susu[1], col_blue_susu[2]);
            rgb_matrix_set_color(51, col_blue_susu[0], col_blue_susu[1], col_blue_susu[2]);
            rgb_matrix_set_color(57, col_blue_susu[0], col_blue_susu[1], col_blue_susu[2]);
            rgb_matrix_set_color(62, col_blue_susu[0], col_blue_susu[1], col_blue_susu[2]);
            rgb_matrix_set_color(78, col_blue_susu[0], col_blue_susu[1], col_blue_susu[2]);
            // QMK RGB Controls
            rgb_matrix_set_color(14, col_blue_susu[0], col_blue_susu[1], col_blue_susu[2]);
            rgb_matrix_set_color(15, col_blue_susu[0], col_blue_susu[1], col_blue_susu[2]);
            rgb_matrix_set_color(16, col_blue_susu[0], col_blue_susu[1], col_blue_susu[2]);
            rgb_matrix_set_color(38, col_blue_susu[0], col_blue_susu[1], col_blue_susu[2]);
            rgb_matrix_set_color(90, col_blue_susu[0], col_blue_susu[1], col_blue_susu[2]);
            rgb_matrix_set_color(93, col_blue_susu[0], col_blue_susu[1], col_blue_susu[2]);
            // Arrows
            rgb_matrix_set_color(94, col_blue_susu[0], col_blue_susu[1], col_blue_susu[2]);
            rgb_matrix_set_color(95, col_blue_susu[0], col_blue_susu[1], col_blue_susu[2]);
            rgb_matrix_set_color(97, col_blue_susu[0], col_blue_susu[1], col_blue_susu[2]);
            rgb_matrix_set_color(79, col_blue_susu[0], col_blue_susu[1], col_blue_susu[2]);
            // PgUp PgDown
            rgb_matrix_set_color(86, col_blue_susu[0], col_blue_susu[1], col_blue_susu[2]);
            rgb_matrix_set_color(82, col_blue_susu[0], col_blue_susu[1], col_blue_susu[2]);
            break;
        case _MAPS:
            // Esc
            rgb_matrix_set_color(0,  0, 255, 0);
            // print("In _MAPS Layer, setting RGB");
            // 1-2-3-4
            rgb_matrix_set_color(7,  col_blue_susu[0], col_blue_susu[1], col_blue_susu[2]);
            rgb_matrix_set_color(13, col_blue_susu[0], col_blue_susu[1], col_blue_susu[2]);
            rgb_matrix_set_color(19, col_blue_susu[0], col_blue_susu[1], col_blue_susu[2]);
            rgb_matrix_set_color(24, col_blue_susu[0], col_blue_susu[1], col_blue_susu[2]);
            // C-D
            rgb_matrix_set_color(22, col_blue_susu[0], col_blue_susu[1], col_blue_susu[2]);
            rgb_matrix_set_color(21, col_blue_susu[0], col_blue_susu[1], col_blue_susu[2]);
            // Arrows
            rgb_matrix_set_color(94, col_blue_susu[0], col_blue_susu[1], col_blue_susu[2]);
            rgb_matrix_set_color(95, col_blue_susu[0], col_blue_susu[1], col_blue_susu[2]);
            rgb_matrix_set_color(97, col_blue_susu[0], col_blue_susu[1], col_blue_susu[2]);
            rgb_matrix_set_color(79, col_blue_susu[0], col_blue_susu[1], col_blue_susu[2]);
            // Del
            rgb_matrix_set_color(79, col_blue_susu[0], col_blue_susu[1], col_blue_susu[2]);
            break;
        case _COLEMAK_DH:
            rgb_matrix_set_color(0,  0, 255, 255);
            break;
        case _MACOS:
            // Esc
            rgb_matrix_set_color(0,  255, 0, 255);
            break;
        case _MIDI:
            // Esc
            rgb_matrix_set_color(0 ,  255, 255, 255);
            // first row
            rgb_matrix_set_color(13,  col_midi_flat[0], col_midi_flat[1], col_midi_flat[2]);
            rgb_matrix_set_color(19,  col_midi_flat[0], col_midi_flat[1], col_midi_flat[2]);

            rgb_matrix_set_color(29,  col_midi_flat[0], col_midi_flat[1], col_midi_flat[2]);
            rgb_matrix_set_color(35,  col_midi_flat[0], col_midi_flat[1], col_midi_flat[2]);
            rgb_matrix_set_color(40,  col_midi_flat[0], col_midi_flat[1], col_midi_flat[2]);

            rgb_matrix_set_color(51,  col_midi_flat[0], col_midi_flat[1], col_midi_flat[2]);
            rgb_matrix_set_color(57,  col_midi_flat[0], col_midi_flat[1], col_midi_flat[2]);
            // second row
            rgb_matrix_set_color(8 ,  col_midi_nat[0], col_midi_nat[1], col_midi_nat[2]);
            rgb_matrix_set_color(14,  col_midi_nat[0], col_midi_nat[1], col_midi_nat[2]);
            rgb_matrix_set_color(20,  col_midi_nat[0], col_midi_nat[1], col_midi_nat[2]);
            rgb_matrix_set_color(25,  col_midi_nat[0], col_midi_nat[1], col_midi_nat[2]);
            rgb_matrix_set_color(30,  col_midi_nat[0], col_midi_nat[1], col_midi_nat[2]);
            rgb_matrix_set_color(36,  col_midi_nat[0], col_midi_nat[1], col_midi_nat[2]);
            rgb_matrix_set_color(41,  col_midi_nat[0], col_midi_nat[1], col_midi_nat[2]);
            rgb_matrix_set_color(46,  col_midi_nat[0], col_midi_nat[1], col_midi_nat[2]);
            rgb_matrix_set_color(52,  col_midi_nat[0], col_midi_nat[1], col_midi_nat[2]);
            rgb_matrix_set_color(58,  col_midi_nat[0], col_midi_nat[1], col_midi_nat[2]);
            // third row
            rgb_matrix_set_color(26,  col_midi_flat[0], col_midi_flat[1], col_midi_flat[2]);
            rgb_matrix_set_color(31,  col_midi_flat[0], col_midi_flat[1], col_midi_flat[2]);
            rgb_matrix_set_color(37,  col_midi_flat[0], col_midi_flat[1], col_midi_flat[2]);
            // fourth row
            rgb_matrix_set_color(22,  col_midi_nat[0], col_midi_nat[1], col_midi_nat[2]);
            rgb_matrix_set_color(27,  col_midi_nat[0], col_midi_nat[1], col_midi_nat[2]);
            rgb_matrix_set_color(32,  col_midi_nat[0], col_midi_nat[1], col_midi_nat[2]);
            rgb_matrix_set_color(38,  col_midi_nat[0], col_midi_nat[1], col_midi_nat[2]);
            rgb_matrix_set_color(43,  col_midi_nat[0], col_midi_nat[1], col_midi_nat[2]);

            break;
    }
    if (mouseEnabled && biton32(layer_state) == _QWERTY) {
        // F9
        rgb_matrix_set_color(50, col_blue_susu[0], col_blue_susu[1], col_blue_susu[2]);
        // Arrows
        rgb_matrix_set_color(94, col_blue_susu[0], col_blue_susu[1], col_blue_susu[2]);
        rgb_matrix_set_color(95, col_blue_susu[0], col_blue_susu[1], col_blue_susu[2]);
        rgb_matrix_set_color(97, col_blue_susu[0], col_blue_susu[1], col_blue_susu[2]);
        rgb_matrix_set_color(79, col_blue_susu[0], col_blue_susu[1], col_blue_susu[2]);
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
