/* Copyright 2023 @ Keychron (https://www.keychron.com)
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

#include "ch.h"
#include "hal_usb.h"
#include "keycodes.h"
#include "quantum.h"
#include QMK_KEYBOARD_H
#include "keychron_common.h"

static bool f24_tracker =  FALSE;

// clang-format off

enum layers {
    BASE,
    FUNC,
    L2,
    L3
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [BASE] = LAYOUT_numpad_6x5(
        KC_MUTE,   MO(FUNC), KC_ESC,  KC_BSPC,  KC_TAB,
        MC_1,   KC_NO,   KC_PSLS, KC_PAST,  KC_PMNS,
        MC_2,   KC_P7,    KC_P8,   KC_P9,    KC_PPLS,
        MC_3,   KC_P4,    KC_P5,   KC_P6,
        MC_4,   KC_P1,    KC_P2,   KC_P3,    KC_PENT,
        MC_5,   KC_P0,             KC_PDOT),

    [FUNC] = LAYOUT_numpad_6x5(
        RGB_TOG,   _______,  KC_MUTE, KC_VOLD,  KC_VOLU,
        MC_1,   RGB_MOD,  RGB_VAI, RGB_HUI,  KC_DEL,
        MC_2,   RGB_RMOD, RGB_VAD, RGB_HUD,  _______,
        MC_3,   RGB_SAI,  RGB_SPI, KC_MPRV,
        MC_4,   RGB_SAD,  RGB_SPD, KC_MPLY,  _______,
        MC_5,   RGB_TOG,           KC_MNXT),

    [L2] = LAYOUT_numpad_6x5(
        _______,   _______,  _______, _______,  _______,
        _______,   _______,  _______, _______,  _______,
        _______,   _______,  _______, _______,  _______,
        _______,   _______,  _______, _______,
        _______,   _______,  _______, _______,  _______,
        _______,   _______,           _______),

    [L3] = LAYOUT_numpad_6x5(
        _______,   _______,  _______, _______,   _______,
        _______,   _______,  _______, _______,   _______,
        _______,   _______,  _______, _______,   _______,
        _______,   _______,  _______, _______,
        _______,   _______,  _______, _______,   _______,
        _______,   _______,           _______)
};

#if defined(ENCODER_MAP_ENABLE)
const uint16_t PROGMEM encoder_map[][NUM_ENCODERS][2] = {
    [BASE] = { ENCODER_CCW_CW(KC_VOLD, KC_VOLU) },
    [FUNC] = { ENCODER_CCW_CW(RGB_VAD, RGB_VAI) },
    [L2] = { ENCODER_CCW_CW(_______, _______) },
    [L3] = { ENCODER_CCW_CW(_______, _______) }
};
#endif // ENCODER_MAP_ENABLE

// clang-format on

void housekeeping_task_user(void) {
    housekeeping_task_keychron();
}

enum my_keycodes {
  SOFT_RESET = QK_KB_0,
  RESET_F24
};

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
	switch (keycode) {
        case KC_A ... KC_UP: // skips numpad
        case KC_APPLICATION ... KC_F23: //notice how it skips over F24
        case KC_EXECUTE ... KC_EXSEL: //exsel is the last one before the modifier keys
			if (record->event.pressed) {
                // for some reason this registers up and down at the same time sometimes and that breaks everything. see if something changes
                // swapping to bool MIGHT have fixed
				register_code(KC_F24); //this means to send F24 down
				f24_tracker = TRUE;
				register_code(keycode);
				return false;
			}
			break;
        case SOFT_RESET:
            if (record->event.pressed) {
                // Do something when pressed
                soft_reset_keyboard(); // not sure if this actually does anything
            } else {
                // Do something else when release
            }
            return false; // Skip all further processing of this key
        case RESET_F24:
            if (record->event.pressed) {
                f24_tracker = FALSE;
            }
        // default:
        //     return  true;
	}
	return true;
}

void post_process_record_user(uint16_t keycode, keyrecord_t *record) {
	switch (keycode) {
        case KC_A ... KC_UP: // skips numpad
        case KC_APPLICATION ... KC_F23: //notice how it skips over F24
        case KC_EXECUTE ... KC_EXSEL: //exsel is the last one before the modifier keys
			if (!record->event.pressed) {
                // unregister_code(keycode); // just added this - not sure if it helps
				f24_tracker = FALSE;
				if (!f24_tracker) {
					unregister_code(KC_F24); //this means to send F24 up
				}
			}
			break;
	}
}

bool rgb_matrix_indicators_user(void) {
    uint8_t current_layer = get_highest_layer(layer_state);
    switch (current_layer) {
        case FUNC:
            rgb_matrix_set_color_all(0xFF, 0x00, 0x00);  // RGB red
            break;
        case L2:
            rgb_matrix_set_color_all(0x00, 0xFF, 0x00);  // RGB green
            break;
        case L3:
            rgb_matrix_set_color_all(0x00, 0x00, 0xFF);  // RGB blue
            break;
        default:
            break;
    }
    return false;
}
