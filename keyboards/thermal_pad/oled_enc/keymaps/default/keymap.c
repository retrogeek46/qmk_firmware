// Copyright 2023 QMK
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H
#include <stdio.h>
#include "raw_hid.h"
#include <print.h>

// combos
#define KC_CAPA G(KC_PSCR)
#define KC_CAPS G(S(KC_S))
#define CLS_TAB C(KC_W)
#define DITTO   C(KC_Q)
#define LAUNCHR A(KC_SPC)
#define EVRTHNG A(S(KC_F))
#define ALT_TAB A(KC_TAB)
#define DSCD_MT C(KC_M)

enum custom_keycodes {
    KC_ENC = SAFE_RANGE,
    KC_OLED,
};

enum LAYERS {
    BASE,
    ALT
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
    _NUM_OF_ENC_MODES
};

uint8_t encoder_mode = ENC_MODE_0;
uint8_t oled_state   = OLED_LAYER;
int     hid_code;
int     current_title_code[21];
int     current_artist_code[21];
int     time_code[21];
int     led_index = 0;
bool    media_updated;
bool    clock_updated;
bool    reset_oled = false;

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [BASE] = LAYOUT(
        KC_CAPA,    LAUNCHR,    EVRTHNG,
        KC_CAPS,    ALT_TAB,    DITTO,
        XXXXXXX,    CLS_TAB,    MO(1),      KC_MPLY
    ),
    [ALT] = LAYOUT(
        QK_BOOT,    XXXXXXX,    XXXXXXX,
        XXXXXXX,    KC_SLEP,    XXXXXXX,
        KC_ENC,     KC_OLED,    XXXXXXX,    DSCD_MT
    )
};

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
        case KC_ENC:
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

///////////////////////////////////////
// oled stuff
const char code_to_name[60] = {
    ' ', ' ', ' ', ' ',  'A', 'B', 'C', 'D', 'E', 'F',
    'G', 'H', 'I', 'J',  'K', 'L', 'M', 'N', 'O', 'P',
    'Q', 'R', 'S', 'T',  'U', 'V', 'W', 'X', 'Y', 'Z',
    '1', '2', '3', '4',  '5', '6', '7', '8', '9', '0',
    'R', 'E', 'B', 'T',  '_', '-', '=', '[', ']', '\\',
    '#', ';', '\'', '`', ',', '.', '/', ':', ' ', ' '
};

#ifdef OLED_ENABLE
oled_rotation_t oled_init_user(oled_rotation_t rotation) {
      return OLED_ROTATION_180;
}

void oled_render_layer_state(void) {
    switch (biton32(layer_state)) {
        case BASE:
            oled_write_ln_P(PSTR("Layer: BASE"), false);
            break;
        case ALT:
            oled_write_ln_P(PSTR("Layer: ALT"), false);
            break;
    }
    switch (encoder_mode) {
        case ENC_MODE_0:
            oled_write_ln_P(PSTR("Encoder: Seek"), false);
            break;
        case ENC_MODE_1:
            oled_write_ln_P(PSTR("Encoder: Scroll"), false);
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

void oled_render_static_art(void) {
    // oled_write_raw_P(saitama_ok, sizeof(saitama_ok));
    oled_write_raw_P(master_sword, sizeof(master_sword));
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
        oled_render_static_art();
        oled_scroll_right();
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
#endif
///////////////////////////////////////

///////////////////////////////////////
// encoder stuff
#ifdef ENCODER_ENABLE
bool encoder_update_user(uint8_t index, bool clockwise) {
    if (!clockwise) {
        switch (encoder_mode) {
            case ENC_MODE_0:
                tap_code(KC_RIGHT);
                break;
            case ENC_MODE_1:
                tap_code16(KC_WH_D);
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
            default:
                tap_code(KC_LEFT);
                break;
        }
    }
    return false;
}
#endif
///////////////////////////////////////
