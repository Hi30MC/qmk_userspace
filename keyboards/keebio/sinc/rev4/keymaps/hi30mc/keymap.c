// Copyright 2023 Danny Nguyen (danny@keeb.io)
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H
#include <lib/lib8tion/lib8tion.h>

enum keycodes { KC_CYLR = QK_USER, KC_LRST, KC_CYLT, FL_TRAN, FL_LESB, FL_SYS };

#define LAYER_CYCLE_START 0
#define LAYER_CYCLE_END 2
#define FLAG_CYCLE_START 0
#define FLAG_CYCLE_END 2

enum scroll_direction { LEFT = -1, RIGHT = 1, NONE = 0 };

int8_t direction = RIGHT;

enum flag_type { TRANS, LESBIAN, SYSTEM };

uint8_t curr_flag = TRANS;

void update_flag(uint8_t new_flag) {
    curr_flag = new_flag;
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case KC_CYLR:
            if (!record->event.pressed) {
                return false;
            }

            uint8_t current_layer = get_highest_layer(layer_state);
            if (current_layer > LAYER_CYCLE_END || current_layer < LAYER_CYCLE_START) {
                return false;
            }

            uint8_t next_layer = current_layer + 1;
            if (next_layer > LAYER_CYCLE_END) {
                next_layer = LAYER_CYCLE_START;
            }
            layer_move(next_layer);
            return false;
        case KC_LRST:
            if (!record->event.pressed) {
                return false;
            }
            layer_move(LAYER_CYCLE_START);
            return false;
        case KC_CYLT:
            if (!record->event.pressed) {
                return false;
            }
            curr_flag++;
            if (curr_flag > FLAG_CYCLE_END) {
                curr_flag = TRANS;
            }
            return false;
        case FL_TRAN:
            if (!record->event.pressed){
                return false;
            }
            curr_flag = TRANS;
            return false;
        case FL_LESB:
            if (!record->event.pressed){
                return false;
            }
            curr_flag = LESBIAN;
            return false;
        case FL_SYS:
            if (!record->event.pressed){
                return false;
            }
            curr_flag = SYSTEM;
            return false;
        default:
            return true;
    }
}

bool encoder_update_user(uint8_t index, bool clockwise) {
    uint8_t layer = get_highest_layer(layer_state);
    if (layer == 0) {
        if (clockwise) {
            tap_code(KC_VOLD);
        } else {
            tap_code(KC_VOLU);
        }
    } else if (layer == 1) {
        if (clockwise) {
            tap_code(QK_MOUSE_WHEEL_UP);
        } else {
            tap_code(QK_MOUSE_WHEEL_DOWN);
        }
    } else if (layer == 2) {
        if (clockwise) {
            tap_code(KC_PGUP);
        } else {
            tap_code(KC_PGDN);
        }
    }
    return false;
}

layer_state_t layer_state_set_user(layer_state_t state) {
    switch (get_highest_layer(state)) {
        case 2:
            direction = NONE;
            break;
        case 1:
            direction = LEFT;
            break;
        default:
            direction = RIGHT;
            break;
    }
    return state;
}

bool rgb_matrix_indicators_advanced_user(uint8_t led_min, uint8_t led_max) {
    HSV     flag[5]    = {0};
    uint8_t num_colors = 0;
    switch (curr_flag) {
        case TRANS:
            flag[0]    = (HSV){197 * 255 / 360, 63 * 255 / 100, 98 * 255 / 100};
            flag[1]    = (HSV){348 * 255 / 360, 45 * 255 / 100, 96 * 255 / 100};
            flag[2]    = (HSV){0 * 255 / 360, 0 * 255 / 100, 75 * 255 / 100};
            flag[3]    = (HSV){348 * 255 / 360, 45 * 255 / 100, 96 * 255 / 100};
            num_colors = 4;
            break;
        case LESBIAN:
            flag[0]    = (HSV){324 * 255 / 360, 100 * 255 / 100, 65 * 255 / 100};
            flag[1]    = (HSV){324 * 255 / 360, 54 * 255 / 100, 83 * 255 / 100};
            flag[2]    = (HSV){0 * 255 / 360, 0, 75 * 255 / 100};
            flag[3]    = (HSV){25 * 255 / 360, 67 * 255 / 100, 100 * 255 / 100};
            flag[4]    = (HSV){11 * 255 / 360, 100 * 255 / 100, 84 * 255 / 100};
            num_colors = 5;
            break;
        case SYSTEM:
            flag[0]    = (HSV){312 * 255 / 360, 90 * 255 / 100, 20 * 255 / 100};
            flag[1]    = (HSV){280 * 255 / 360, 45 * 255 / 100, 52 * 255 / 100};
            flag[2]    = (HSV){243 * 255 / 360, 34 * 255 / 100, 78 * 255 / 100};
            flag[3]    = (HSV){150 * 255 / 360, 24 * 255 / 100, 76 * 255 / 100};
            flag[4]    = (HSV){53 * 255 / 360, 55 * 255 / 100, 95 * 255 / 100};
            num_colors = 5;
            break;
    }

    const uint8_t stripe_width = 224 / num_colors;

    uint16_t scroll_offset = scale16by8(g_rgb_timer, qadd8(rgb_matrix_config.speed / 4, 1)) * direction;

    for (uint8_t i = led_min; i < led_max; i++) {
        uint16_t scrolled_x = (g_led_config.point[i].x + scroll_offset) % (stripe_width * num_colors);
        HSV      color      = (HSV)flag[scrolled_x / stripe_width];

        color.v = 50 * color.v / 255;
        RGB rgb = hsv_to_rgb(color);
        rgb_matrix_set_color(i, rgb.r, rgb.g, rgb.b);
    }

    if (host_keyboard_led_state().caps_lock) {
        rgb_matrix_set_color(28, RGB_RED);
    }
    if (host_keyboard_led_state().num_lock && get_highest_layer(layer_state) == 1) {
        rgb_matrix_set_color(15, RGB_RED);
    }
    return false;
}

void keyboard_post_init_user(void) {
    rgb_matrix_mode_noeeprom(RGB_MATRIX_DEFAULT_MODE);
}

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
  [0] = LAYOUT_80_with_macro(
    KC_CYLT,          KC_ESC,  KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,              KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,  KC_F12,  KC_DEL,  KC_INS,
    XXXXXXX, KC_F16 , KC_GRV,  KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,      KC_7,    KC_8,    KC_9,    KC_0,    KC_MINS, KC_EQL,  KC_MPRV, KC_MPLY, KC_MNXT,
    XXXXXXX, XXXXXXX, KC_TAB,  KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,               KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_LBRC, KC_RBRC, KC_BSLS, KC_END,
    XXXXXXX, XXXXXXX, KC_CAPS, KC_A,    KC_S,    KC_D,    KC_F,    KC_G,               KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT,          KC_ENT,  KC_PGUP,
    XXXXXXX,LSG(KC_S),    KC_LSFT,      KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,               KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH, KC_RSFT, KC_UP,   KC_PGDN,
    XXXXXXX, KC_CYLR, MO(2),  KC_LCTL, KC_LGUI,  KC_LALT, KC_BSPC, MO(1),                       KC_SPC,  TG(1),   KC_RALT, KC_RCTL, KC_RGUI, KC_LEFT, KC_DOWN, KC_RGHT
  ),
  [1] = LAYOUT_80_with_macro(
    _______,          _______, KC_F13 , KC_F14 , KC_F15 , KC_F16 , KC_F17 , KC_F18 ,            KC_F19 , KC_F20 , KC_F21 , KC_F22 , KC_F23 , KC_F24 , _______, _______,
    _______, _______, _______, KC_NUM , KC_PSLS, KC_PAST, KC_PMNS, _______, _______,   _______, _______, _______, _______, _______, _______, _______, _______, _______,
    _______, _______, _______, KC_KP_7, KC_KP_8, KC_KP_9, KC_PPLS, _______,            _______, _______, _______, _______, _______, _______, _______, _______, _______,
    _______, _______, _______, KC_KP_4, KC_KP_5, KC_KP_6, _______, _______,            _______, _______, _______, _______, _______, _______,          _______, _______,
    _______, _______, _______,          KC_KP_1, KC_KP_2, KC_KP_3, KC_DEL, _______,            _______, _______, _______, _______, _______, _______, _______, _______,
    _______, _______, _______, _______, KC_KP_0, KC_PDOT, KC_PENT, MO(1),                       TG(2),   TG(1)  , _______, _______, _______, _______, _______, _______
  ),
  [2] = LAYOUT_80_with_macro(
   _______ ,         _______, _______, _______, _______, _______, _______, _______,            _______, _______, _______, _______, _______, _______, _______, _______,
   RM_TOGG, _______, _______, _______, _______, _______, _______, _______, _______,   FL_TRAN, FL_LESB, FL_SYS, _______, _______, _______, _______, _______, _______,
   RM_SPDU, RM_SPDD, _______, _______, _______, _______, _______, _______,            _______, _______, _______, _______, _______, _______, _______, _______, _______,
   _______, _______, _______, _______, _______, _______, _______, _______,            _______, _______, _______, _______, _______, _______,          _______, _______,
   _______, NK_ON,   _______,          _______, _______, _______, _______, _______,            _______, _______, _______, _______, _______, _______, _______, _______,
   QK_BOOT, NK_TOGG, MO(2),   _______, _______, _______, _______, _______,                     TG(2),   KC_LRST, _______, _______, _______, _______, _______, QK_BOOT
  ),
//[N] = LAYOUT_80_with_macro( //BLANK SET
// _______,          _______, _______, _______, _______, _______, _______, _______,            _______, _______, _______, _______, _______, _______, _______, _______,
// _______, _______, _______, _______, _______, _______, _______, _______, _______,   _______, _______, _______, _______, _______, _______, _______, _______, _______,
// _______, _______, _______, _______, _______, _______, _______, _______,            _______, _______, _______, _______, _______, _______, _______, _______, _______,
// _______, _______, _______, _______, _______, _______, _______, _______,            _______, _______, _______, _______, _______, _______,          _______, _______,
// _______, _______, _______,          _______, _______, _______, _______, _______,            _______, _______, _______, _______, _______, _______, _______, _______,
// _______, _______, _______, _______, _______, _______, _______, _______,                     _______, _______, _______, _______, _______, _______, _______, _______
//),
//[-1] = META_DETAILS(
// ENCODER,          _______, _______, _______, _______, _______, _______, LEFT,               RIGHT  , _______, _______, _______, _______, _______, _______, _______,
// MACRO 1, _______, _______, _______, _______, _______, _______, _______, LEFT,      RIGHT  , _______, _______, _______, _______, _______, _______, _______, _______,
// _______, _______, _______, _______, _______, _______, _______, LEFT,               RIGHT  , _______, _______, _______, _______, _______, _______, _______, _______,
// _______, _______, _______, _______, _______, _______, _______, LEFT,               RIGHT  , _______, _______, _______, _______, _______,          _______, _______,
// _______, _______, _______,          _______, _______, _______, _______, LEFT,               RIGHT  , _______, _______, _______, _______, _______, _______, _______,
// _______, MACRO X, _______, _______, _______, _______, _______, LEFT,                        RIGHT  , _______, _______, _______, _______, _______, _______, _______
//),
};
