// Copyright 2023 Danny Nguyen (danny@keeb.io)
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H
#include <lib/lib8tion/lib8tion.h>
#include "transactions.h"
// enum SPLIT_TRANSACTION_IDS_USER { LED_META };

enum keycodes { KC_CYLR = QK_USER, KC_LRST, KC_CYLT, KC_TGNE, FL_TRAN, FL_LESB, FL_SYS, ENC_ACW1, ENC_CW1, ENC_ACW2, ENC_CW2};

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

uint8_t togg_state = 1;

void togg_led(void) {
    togg_state = !togg_state;
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
        case KC_TGNE:
            if (!record->event.pressed) {
                return false;
            }
            togg_led();
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
        case ENC_ACW1:
            if (!record->event.pressed) {
                return false;
            }
            rgb_matrix_increase_val_noeeprom();
            return false;
        case ENC_CW1:
            if (!record->event.pressed) {
                return false;
            }
            rgb_matrix_decrease_val_noeeprom();
            return false;
        case ENC_ACW2:
            if (!record->event.pressed) {
                return false;
            }
            rgb_matrix_increase_speed_noeeprom();
            return false;
        case ENC_CW2:
            if (!record->event.pressed) {
                return false;
            }
            rgb_matrix_decrease_speed_noeeprom();
            return false;
        default:
            return true;
    }
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
            flag[0]    = (HSV){140, 161, 250};
            flag[1]    = (HSV){247, 115, 245};
            flag[2]    = (HSV){0, 0, 191};
            flag[3]    = (HSV){247, 115, 245};
            num_colors = 4;
            break;
        case LESBIAN:
            flag[0]    = (HSV){230, 255, 166};
            flag[1]    = (HSV){230, 138, 212};
            flag[2]    = (HSV){0, 0, 191};
            flag[3]    = (HSV){18, 171, 255};
            flag[4]    = (HSV){8, 100, 214};
            num_colors = 5;
            break;
        case SYSTEM:
            flag[0]    = (HSV){221, 230, 51};
            flag[1]    = (HSV){198, 115, 133};
            flag[2]    = (HSV){172, 87, 199};
            flag[3]    = (HSV){106, 61, 194};
            flag[4]    = (HSV){38, 140, 242};
            num_colors = 5;
            break;
    }

    const uint8_t stripe_width = 224 / num_colors;

    uint16_t scroll_offset = scale16by8(g_rgb_timer, qadd8(rgb_matrix_config.speed / 4, 1)) * direction;
    uint16_t val = rgb_matrix_get_val();
    for (uint8_t i = led_min; i < led_max; i++) {
        uint16_t scrolled_x = (g_led_config.point[i].x + scroll_offset) % (stripe_width * num_colors);
        HSV      color      = (HSV)flag[scrolled_x / stripe_width];

        color.v = ((val * color.v * togg_state) / 255);
        RGB rgb = hsv_to_rgb(color);
        rgb_matrix_set_color(i, rgb.r, rgb.g, rgb.b);
    }

    if (host_keyboard_led_state().caps_lock) {
        rgb_matrix_set_color(28, RGB_RED);
    }
    if (host_keyboard_led_state().num_lock && get_highest_layer(layer_state) == 1) {
        rgb_matrix_set_color(15, RGB_RED);
        rgb_matrix_set_color(70, RGB_RED);
    }
    return false;
}

typedef struct _master_to_slave_t {
    uint8_t m2s_flag;
    uint8_t m2s_direction;
    uint8_t m2s_toggle_state;
} master_to_slave_t;

typedef struct _slave_to_master_t {
    uint8_t s2m_flag;
    uint8_t s2m_direction;
    uint8_t s2m_toggle_state;
} slave_to_master_t;

void led_meta_slave_handler(uint8_t in_buflen, const void* in_data, uint8_t len, void* unused) {
    const master_to_slave_t *m2s = (const master_to_slave_t*)in_data;
    direction = m2s->m2s_direction;
    curr_flag = m2s->m2s_flag;
    togg_state = m2s->m2s_toggle_state;
}

void keyboard_post_init_user(void) {
    update_flag(LESBIAN);
    keymap_config.nkro = true;
    transaction_register_rpc(LED_META, led_meta_slave_handler);
}

void housekeeping_task_user(void) {
    if (is_keyboard_master()) {
        static uint32_t last_sync = 0;
        if (timer_elapsed32(last_sync) > 100) {
            master_to_slave_t m2s = {curr_flag, direction, togg_state};
            slave_to_master_t s2m = {0};
            if (transaction_rpc_exec(LED_META, sizeof(m2s), &m2s, sizeof(s2m), &s2m)) {
                last_sync = timer_read32();
                // dprintf("Slave value: %d\n", s2m.s2m_direction);
            } else {
                dprint("Slave sync failed.\n");
            }
        }
    }
}

#if defined(ENCODER_MAP_ENABLE)
const uint16_t PROGMEM encoder_map[][NUM_ENCODERS][NUM_DIRECTIONS] = {
    [0] = { ENCODER_CCW_CW(KC_VOLU, KC_VOLD) },
    [1] = { ENCODER_CCW_CW(ENC_CW1, ENC_ACW1) },
    [2] = { ENCODER_CCW_CW(ENC_CW2, ENC_ACW2) },
};
#endif

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
  [0] = LAYOUT_80_with_macro(
    KC_CYLT,          KC_ESC,  KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,              KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,  KC_F12,  KC_DEL,  KC_INS,
    XXXXXXX, KC_F14 , KC_GRV,  KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,      KC_7,    KC_8,    KC_9,    KC_0,    KC_MINS, KC_EQL,  KC_MPRV, KC_MPLY, KC_MNXT,
    XXXXXXX, XXXXXXX, KC_TAB,  KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,               KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_LBRC, KC_RBRC, KC_BSLS, KC_PGUP,
    XXXXXXX, KC_F16 , KC_CAPS, KC_A,    KC_S,    KC_D,    KC_F,    KC_G,               KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT,      KC_ENT,      KC_PGDN,
    XXXXXXX,LSG(KC_S),    KC_LSFT,      KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,               KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH, KC_RSFT, KC_UP,   KC_END,
    XXXXXXX, KC_CYLR, MO(2),  KC_LCTL, KC_LGUI,  KC_LALT, KC_BSPC, MO(1),                       KC_SPC,  TG(1),   KC_RALT, KC_RCTL, KC_RGUI, KC_LEFT, KC_DOWN, KC_RGHT
  ),
  [1] = LAYOUT_80_with_macro(
    _______,          _______, KC_F13 , KC_F14 , KC_F15 , KC_F16 , KC_F17 , KC_F18 ,            KC_F19 , KC_F20 , KC_F21 , KC_F22 , KC_F23 , KC_F24 , _______, _______,
    _______, _______, _______, KC_NUM , KC_PSLS, KC_PAST, KC_PMNS, _______, _______,   KC_NUM , _______, _______, _______, _______, _______, _______, _______, _______,
    _______, _______, _______, KC_KP_7, KC_KP_8, KC_KP_9, KC_PPLS, _______,            KC_KP_7, KC_KP_8, KC_KP_9, KC_PPLS, _______, _______, _______, _______, _______,
    _______, _______, _______, KC_KP_4, KC_KP_5, KC_KP_6, _______, _______,            KC_KP_4, KC_KP_5, KC_KP_6, KC_PENT, _______, _______,      _______,     _______,
    _______, _______,     _______,      KC_KP_1, KC_KP_2, KC_KP_3, KC_DEL,  _______,            KC_KP_1, KC_KP_2, KC_KP_3, KC_PENT, _______, _______, _______, _______,
    _______, _______, _______, _______, KC_KP_0, KC_PDOT, KC_PENT, MO(1),                       KC_KP_0, TG(1)  , TG(2)  , _______, _______, _______, _______, _______
  ),
  [2] = LAYOUT_80_with_macro(
   _______ ,         _______, _______, _______, _______, _______, _______, _______,            _______, _______, _______, _______, _______, _______, _______, _______,
   KC_TGNE, _______, _______, _______, _______, _______, FL_TRAN, FL_LESB, FL_SYS,    FL_TRAN, FL_LESB, FL_SYS,  _______, _______, _______, _______, _______, _______,
   _______, _______, _______, _______, _______, _______, _______, _______,            _______, _______, _______, _______, _______, _______, _______, _______, _______,
   _______, _______, _______, _______, _______, _______, _______, _______,            _______, _______, _______, _______, _______, _______,      _______,     _______,
   _______, NK_ON,       _______,      _______, _______, _______, _______, _______,            _______, _______, _______, _______, _______, _______, _______, _______,
   QK_BOOT, NK_TOGG, MO(2),   _______, _______, _______, _______, _______,                     _______, KC_LRST, TG(2)  , _______, _______, _______, _______, QK_BOOT
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
