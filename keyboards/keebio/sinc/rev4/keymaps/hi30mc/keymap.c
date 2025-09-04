//Copyright 2023 Danny Nguyen (danny@keeb.io)
//SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H
// Define the keycode, `QK_USER` avoids collisions with existing keycodes
enum keycodes {
  KC_CYLR = SAFE_RANGE,
  KC_LRST,
};

// 1st layer on the cycle
#define LAYER_CYCLE_START 0
// Last layer on the cycle
#define LAYER_CYCLE_END   2

// Add the behaviour of this new keycode
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
    // Process other keycodes normally
    default:
      return true;
  }
}


const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
  [0] = LAYOUT_80_with_macro(
    KC_CYLR,          KC_ESC,  KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,  KC_F12,  KC_DEL,  KC_INS,
    KC_F1,   KC_F2,   KC_GRV,  KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_MINS, KC_EQL,  KC_DEL,  KC_BSPC, KC_HOME,
    KC_F3,   KC_F4,   KC_TAB,  KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_LBRC, KC_RBRC, KC_BSLS, KC_END,
    KC_F5,   KC_F6,   KC_CAPS, KC_A,    KC_S,    KC_D,    KC_F,    KC_G,    KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT,          KC_ENT,  KC_PGUP,
    KC_F7,   KC_LRST,   KC_LSFT,          KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH, KC_RSFT, KC_UP,   KC_PGDN,
    KC_F9,   KC_CYLR,  KC_LCTL, KC_LALT, KC_LGUI, MO(2),   MO(1),  KC_BSPC,           TG(1),   KC_SPC,  KC_RALT, KC_RCTL, KC_RGUI, KC_LEFT, KC_DOWN, KC_RGHT
  ),
  [1] = LAYOUT_80_with_macro(
    KC_LRST,          _______, KC_F13 , KC_F14 , KC_F15 , KC_F16 , KC_F17 , KC_F18 , KC_F19 , KC_F20 , KC_F21 , KC_F22 , KC_F23 , KC_F24 , _______, _______,
    _______, _______, _______, KC_NUM , KC_PSLS, KC_PAST, KC_PMNS, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
    _______, _______, _______, KC_KP_7, KC_KP_8, KC_KP_9, KC_PPLS, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
    _______, _______, _______, KC_KP_4, KC_KP_5, KC_KP_6, _______, _______, _______, _______, _______, _______, _______, _______,          _______, _______,
    _______, _______, _______,          KC_KP_1, KC_KP_2, KC_KP_3, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
    _______, _______, _______, _______, KC_KP_0, KC_PDOT, _______, KC_PENT,          TG(1)  , _______, _______, _______, _______, _______, _______, _______
  ),
  [2] = LAYOUT_80_with_macro(
   _______ ,          _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
   RM_TOGG, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
   RM_SPDU, RM_SPDD, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
   _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,          _______, _______,
   _______, _______, _______,          _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
   _______, _______, _______, _______, _______, MO(2)  , _______, _______,          _______, _______, _______, _______, _______, _______, _______, _______
  ),
//[N] = LAYOUT_80_with_macro( //BLANK SET
// _______,          _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
// _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
// _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
// _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,          _______, _______,
// _______, _______, _______,          _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
// _______, _______, _______, _______, _______, _______, _______, _______,          _______, _______, _______, _______, _______, _______, _______, _______
//),
//[-1] = META_DETAILS(
// _______,          _______, _______, _______, _______, _______, _______, LEFT   , RIGHT  , _______, _______, _______, _______, _______, _______, _______,
// _______, _______, _______, _______, _______, _______, _______, _______, LEFT   , RIGHT  , _______, _______, _______, _______, _______, _______, _______, _______,
// _______, _______, _______, _______, _______, _______, _______, LEFT   , RIGHT  , _______, _______, _______, _______, _______, _______, _______, _______,
// _______, _______, _______, _______, _______, _______, _______, LEFT   , RIGHT  , _______, _______, _______, _______, _______,          _______, _______,
// _______, _______, _______,          _______, _______, _______, _______, LEFT   , RIGHT  , _______, _______, _______, _______, _______, _______, _______,
// _______, _______, _______, _______, _______, _______, _______, LEFT   ,          RIGHT  , _______, _______, _______, _______, _______, _______, _______
//),
};

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
            tap_code(KC_LRST);
        } else {
            tap_code(KC_PGDN);
        }
    }

    return false;
}

layer_state_t layer_state_set_user(layer_state_t state) {
    switch (get_highest_layer(state)) {
    case 2:
        rgb_matrix_mode_noeeprom(RGB_MATRIX_CUSTOM_scrolling_trans_flag);
        break;
    case 1:
        rgb_matrix_mode_noeeprom(RGB_MATRIX_CUSTOM_scrolling_lesbian_flag);
        break;
    default:
        rgb_matrix_mode_noeeprom(RGB_MATRIX_CUSTOM_scrolling_system_flag);
        break;
    }
  return state;
}

bool rgb_matrix_indicators_advanced_user(uint8_t led_min, uint8_t led_max) {
    if (host_keyboard_led_state().caps_lock) {
        rgb_matrix_set_color(17, RGB_RED);
    }
    if (host_keyboard_led_state().num_lock && get_highest_layer(layer_state) == 1) {
        rgb_matrix_set_color(15, RGB_RED);
    }
    return false;
}

void keyboard_post_init_user(void){
    rgb_matrix_mode_noeeprom(RGB_MATRIX_DEFAULT_MODE);
}
