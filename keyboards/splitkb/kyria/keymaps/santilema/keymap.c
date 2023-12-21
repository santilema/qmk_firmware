/* Copyright 2019 Thomas Baart <thomas@splitkb.com>
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
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

enum layers {
    _QWERTY = 0,
    _DVORAK,
    _COLEMAK_DH,
    _NAV,
    _SYM,
    _FUNCTION,
    _ADJUST,
};

// Function prototypes
bool get_cell_state(uint8_t x, uint8_t y);
void update_game_of_life(void);
void render_game_of_life(void);
void randomize_grid(void);
void initialize_grid(void);

// Game of Life for OLED
#define GRID_WIDTH  128
#define GRID_HEIGHT 64

// Definition of the grid
static bool grid[GRID_HEIGHT][GRID_WIDTH];

// Definition of the game of life functions
void randomize_grid(void) {
    for (uint8_t y = 0; y < GRID_HEIGHT; y++) {
        for (uint8_t x = 0; x < GRID_WIDTH; x++) {
            grid[y][x] = (random() % 2) == 0;
        }
    }
}

void render_game_of_life(void) {
    oled_clear();

    // iterate over each cell
    for (uint8_t y = 0; y < GRID_HEIGHT; y++) {
        for (uint8_t x = 0; x < GRID_WIDTH; x++) {
            // determine the state of the cell
            bool is_alive = get_cell_state(x, y);

            if (is_alive) {
                // if the cell is alive, draw a pixel
                oled_write_pixel(x, y, true);
            }
        }
    }
    update_game_of_life();

}

bool get_cell_state(uint8_t x, uint8_t y) {
    // Check for out-of-bounds coordinates
    if (x >= GRID_WIDTH || y >= GRID_HEIGHT) {
        return false;
    }

    // Return the state of the cell
    return grid[y][x];
}

// void matrix_scan_user(void) {
//     // Update game state
//     update_game_of_life();
// }

void update_game_of_life(void) {
    static bool temp_grid[GRID_HEIGHT][GRID_WIDTH];
    // iterate over each cell
    for (uint8_t y = 0; y < GRID_HEIGHT; y++) {
        for (uint8_t x = 0; x < GRID_WIDTH; x++) {
            // determine the state of the cell
            bool is_alive = get_cell_state(x, y);

            // count the number of living neighbors
            uint8_t living_neighbors = 0;
            for (int8_t y_offset = -1; y_offset <= 1; y_offset++) {
                for (int8_t x_offset = -1; x_offset <= 1; x_offset++) {
                    // skip the current cell
                    if (x_offset == 0 && y_offset == 0) {
                        continue;
                    }

                    // Calculate wrapped coordinates
                    uint8_t neighbor_x = (x + x_offset + GRID_WIDTH) % GRID_WIDTH;
                    uint8_t neighbor_y = (y + y_offset + GRID_HEIGHT) % GRID_HEIGHT;

                    if (get_cell_state(neighbor_x, neighbor_y)) {
                        living_neighbors++;
                    }
                }
            }

            // apply the rules of the game to temp grid
            bool new_state = false;
            if (is_alive) {
                new_state = (living_neighbors == 2 || living_neighbors == 3);
            } else {
                new_state = (living_neighbors == 3);
            }
            temp_grid[y][x] = new_state;
        }
    }

    // copy temp grid to grid
    for (uint8_t y = 0; y < GRID_HEIGHT; y++) {
        for (uint8_t x = 0; x < GRID_WIDTH; x++) {
            grid[y][x] = temp_grid[y][x];
        }
    }
}

void initialize_grid(void) {
    // Clear the grid first
    for (uint8_t y = 0; y < GRID_HEIGHT; y++) {
        for (uint8_t x = 0; x < GRID_WIDTH; x++) {
            grid[y][x] = false;
        }
    }

    // Set up a specific pattern (e.g., a small glider)
    grid[30][62] = true;
    grid[30][63] = true;
    grid[30][64] = true;
    grid[30][66] = true;
    grid[31][62] = true;
    grid[32][65] = true;
    grid[32][66] = true;
    grid[33][63] = true;
    grid[33][64] = true;
    grid[33][66] = true;
    grid[34][62] = true;
    grid[34][64] = true;
    grid[34][66] = true;
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case KC_RCTL:
            if (record->event.pressed) {
                // When key is pressed
                initialize_grid();
            }
            return false;  // Skip all further processing of this key
        case KC_RGUI:
            if (record->event.pressed) {
                // When key is pressed
                randomize_grid();
            }
            return false;  // Skip all further processing of this key
        default:
            return true;   // Process all other keys normally
    }
}

// Aliases for readability
#define QWERTY   DF(_QWERTY)
#define COLEMAK  DF(_COLEMAK_DH)
#define DVORAK   DF(_DVORAK)

#define SYM      MO(_SYM)
#define NAV      MO(_NAV)
#define FKEYS    MO(_FUNCTION)
#define ADJUST   MO(_ADJUST)

#define TAB_ESC  MT(MOD_TAB, KC_ESC)
#define CTL_QUOT MT(MOD_RCTL, KC_QUOTE)
#define CTL_MINS MT(MOD_RCTL, KC_MINUS)
#define ALT_ENT  MT(MOD_LALT, KC_ENT)
#define SFT_CAPS  MT(MOD_LSFT, KC_CAPS)


// Note: LAlt/Enter (ALT_ENT) is not the same thing as the keyboard shortcut Alt+Enter.
// The notation `mod/tap` denotes a key that activates the modifier `mod` when held down, and
// produces the key `tap` when tapped (i.e. pressed and released).

// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
/*
 * Base Layer: QWERTY
 *
 * -------------------------------------------.                              ,-------------------------------------------.
 * |  Tab  |   Q  |   W  |   E  |   R  |   T  |                              |   Y  |   U  |   I  |   O  |   P  |  Bksp  |
 * |-------+------+------+------+------+------|                              |------+------+------+------+------+--------|
 * | Menu  |   A  |   S  |   D  |   F  |   G  |                              |   H  |   J  |   K  |   L  | - _  |  =  +  |
 * |-------+------+------+------+------+------+-------------.  ,-------------+------+------+------+------+------+--------|
 * | Shift |   Z  |   X  |   C  |   V  |   B  | Func |Encod.|  |Encod.| Enter|   N  |   M  |  ; : | ' "  | [ {  |   ] }  |
 * ----------------------+------+------+------+------+------|  |------+------+------+------+------+----------------------'
 *                       | Ctrl | LGUI | LAlt | Space| Nav  |  | Sym  | Space| ,  < | . >  | /  ? |
 *                       |      |      |      |      |      |  |      |      |      |      |      |
 *                       `----------------------------------'  `----------------------------------'
 */
    [_QWERTY] = LAYOUT(
     KC_TAB   , KC_Q ,  KC_W   ,  KC_E   ,   KC_R  ,   KC_T ,                                          KC_Y    ,  KC_U  ,  KC_I   ,   KC_O   ,  KC_P , KC_BSPC,
     KC_APP   , KC_A ,  KC_S   ,  KC_D   ,   KC_F  ,   KC_G ,                                          KC_H    ,  KC_J  ,  KC_K   ,   KC_L   ,KC_MINS, KC_EQL ,
     KC_LSFT  , KC_Z ,  KC_X   ,  KC_C   ,   KC_V  ,   KC_B , FKEYS  , _______,     _______ ,KC_ENT ,  KC_N    ,  KC_M  ,  KC_SCLN, KC_QUOTE ,KC_LBRC, KC_RBRC,
                                KC_LCTL  , KC_LGUI , KC_LALT, KC_SPC , NAV    ,         SYM ,KC_SPC ,  KC_COMM ,  KC_DOT,  KC_SLSH
    ),

/*
 * Nav Layer: Media, navigation
 *
 * ,-------------------------------------------.                              ,-------------------------------------------.
 * |  Esc   |      |      |      |      |      |                              | PgUp | Home |   ↑  | End  | VolUp| Delete |
 * |--------+------+------+------+------+------|                              |------+------+------+------+------+--------|
 * |        |  GUI |  Alt | Ctrl | Shift|      |                              | PgDn |  ←   |   ↓  |   →  | VolDn| Insert |
 * |--------+------+------+------+------+------+-------------.  ,-------------+------+------+------+------+------+--------|
 * |Caps Lck|      |      |      |      |      |      |      |  |      |   ↑  | Pause|M Prev|M Play|M Next|VolMut| PrtSc  |
 * `----------------------+------+------+------+------+------|  |------+------+------+------+------+----------------------'
 *                        | RCtrl|      | RAlt |      |      |  |   ←  |   ↓  |   →  |      |      |
 *                        |      |      |      |      |      |  |      |      |      |      |      |
 *                        `----------------------------------'  `----------------------------------'
 */
    [_NAV] = LAYOUT(
      KC_ESC , _______, _______, _______, _______, _______,                                     KC_PGUP, KC_HOME, KC_UP,   KC_END,  KC_VOLU, KC_DEL,
      _______, KC_LGUI, KC_LALT, KC_LCTL, KC_LSFT, _______,                                     KC_PGDN, KC_LEFT, KC_DOWN, KC_RGHT, KC_VOLD, KC_INS,
      KC_CAPS, _______, _______, _______, _______, _______, _______, _______, _______, KC_UP  ,KC_PAUSE, KC_MPRV, KC_MPLY, KC_MNXT, KC_MUTE, KC_PSCR,
                                 KC_RCTL, KC_RGUI, KC_RALT, _______, _______, KC_LEFT, KC_DOWN, KC_RGHT, _______, _______
    ),

/*
 * Sym Layer: Numbers and symbols
 *
 * ,-------------------------------------------.                              ,-------------------------------------------.
 * |    `   |  1   |  2   |  3   |  4   |  5   |                              |   6  |  7   |  8   |  9   |  0   |   =    |
 * |--------+------+------+------+------+------|                              |------+------+------+------+------+--------|
 * |    ~   |  !   |  @   |  #   |  $   |  %   |                              |   ^  |  &   |  *   |  (   |  )   |   +    |
 * |--------+------+------+------+------+------+-------------.  ,-------------+------+------+------+------+------+--------|
 * |    |   |   \  |  :   |  ;   |  -   |  [   |  {   |      |  |      |   }  |   ]  |  _   |  ,   |  .   |  /   |   ?    |
 * `----------------------+------+------+------+------+------|  |------+------+------+------+------+----------------------'
 *                        | Ctrl |      |      |      |      |  |      |      |      |      |      |
 *                        |      |      |      |      |      |  |      |      |      |      |      |
 *                        `----------------------------------'  `----------------------------------'
 */
    [_SYM] = LAYOUT(
      KC_GRV ,   KC_1 ,   KC_2 ,   KC_3 ,   KC_4 ,   KC_5 ,                                       KC_6 ,   KC_7 ,   KC_8 ,   KC_9 ,   KC_0 , KC_EQL ,
     KC_TILD , KC_EXLM,  KC_AT , KC_HASH,  KC_DLR, KC_PERC,                                     KC_CIRC, KC_AMPR, KC_ASTR, KC_LPRN, KC_RPRN, KC_PLUS,
     KC_PIPE , KC_BSLS, KC_COLN, KC_SCLN, KC_MINS, KC_LBRC, KC_LCBR, _______, _______, KC_RCBR, KC_RBRC, KC_UNDS, KC_COMM,  KC_DOT, KC_SLSH, KC_QUES,
                                 KC_LCTL, _______, _______, _______, _______, _______, _______, _______, _______, _______
    ),

/*
 * Function Layer: Function keys
 *
 * ,-------------------------------------------.                              ,-------------------------------------------.
 * |   F1   |  F2  | F3   | F4   | F5   |  F6  |                              |  F7  | F8   |  F9  | F10  | F11  |  F12   |
 * |--------+------+------+------+------+------|                              |------+------+------+------+------+--------|
 * |   F1   |  F2  | F3   | F4   | F5   |  F6  |                              |  F7  | F8   |  F9  | F10  | F11  |  F12   |
 * |--------+------+------+------+------+------+-------------.  ,-------------+------+------+------+------+------+--------|
 * |  F1    |  F2  | F3   | F4   | F5   |  F6  |      |      |  |      |      |  F7  | F8   |  F9  | F10  | F11  |  F12   |
 * `----------------------+------+------+------+------+------|  |------+------+------+------+------+----------------------'
 *                        |      |      |      |      |      |  |      |      |      |      |      |
 *                        |      |      |      |      |      |  |      |      |      |      |      |
 *                        `----------------------------------'  `----------------------------------'
 */
    [_FUNCTION] = LAYOUT(
      KC_F1,  KC_F2 ,  KC_F3,  KC_F4,  KC_F5, KC_F6,                                     KC_F7, KC_F8, KC_F9, KC_F10, KC_F11, KC_F12,
      KC_F1,  KC_F2 ,  KC_F3,  KC_F4,  KC_F5, KC_F6,                                     KC_F7, KC_F8, KC_F9, KC_F10, KC_F11, KC_F12,
      KC_F1,  KC_F2 ,  KC_F3,  KC_F4,  KC_F5, KC_F6, _______, _______, _______, _______, KC_F7, KC_F8, KC_F9, KC_F10, KC_F11, KC_F12,
                             _______, _______, _______, _______, _______, _______, _______, _______, _______, _______
    ),

/*
 * Adjust Layer: Default layer settings, RGB
 *
 * ,-------------------------------------------.                              ,-------------------------------------------.
 * |        |      |      |QWERTY|      |      |                              |      |      |      |      |      |        |
 * |--------+------+------+------+------+------|                              |------+------+------+------+------+--------|
 * |        |      |      |Dvorak|      |      |                              | TOG  | SAI  | HUI  | VAI  | MOD  |        |
 * |--------+------+------+------+------+------+-------------.  ,-------------+------+------+------+------+------+--------|
 * |        |      |      |Colmak|      |      |      |      |  |      |      |      | SAD  | HUD  | VAD  | RMOD |        |
 * `----------------------+------+------+------+------+------|  |------+------+------+------+------+----------------------'
 *                        |      |      |      |      |      |  |      |      |      |      |      |
 *                        |      |      |      |      |      |  |      |      |      |      |      |
 *                        `----------------------------------'  `----------------------------------'
 */
    [_ADJUST] = LAYOUT(
      _______, _______, _______, QWERTY , _______, _______,                                    _______, _______, _______, _______,  _______, _______,
      _______, _______, _______, DVORAK , _______, _______,                                    RGB_TOG, RGB_SAI, RGB_HUI, RGB_VAI,  RGB_MOD, _______,
      _______, _______, _______, COLEMAK, _______, _______,_______, _______, _______, _______, _______, RGB_SAD, RGB_HUD, RGB_VAD, RGB_RMOD, _______,
                                 _______, _______, _______,_______, _______, _______, _______, _______, _______, _______
    ),

// /*
//  * Layer template
//  *
//  * ,-------------------------------------------.                              ,-------------------------------------------.
//  * |        |      |      |      |      |      |                              |      |      |      |      |      |        |
//  * |--------+------+------+------+------+------|                              |------+------+------+------+------+--------|
//  * |        |      |      |      |      |      |                              |      |      |      |      |      |        |
//  * |--------+------+------+------+------+------+-------------.  ,-------------+------+------+------+------+------+--------|
//  * |        |      |      |      |      |      |      |      |  |      |      |      |      |      |      |      |        |
//  * `----------------------+------+------+------+------+------|  |------+------+------+------+------+----------------------'
//  *                        |      |      |      |      |      |  |      |      |      |      |      |
//  *                        |      |      |      |      |      |  |      |      |      |      |      |
//  *                        `----------------------------------'  `----------------------------------'
//  */
//     [_LAYERINDEX] = LAYOUT(
//       _______, _______, _______, _______, _______, _______,                                     _______, _______, _______, _______, _______, _______,
//       _______, _______, _______, _______, _______, _______,                                     _______, _______, _______, _______, _______, _______,
//       _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
//                                  _______, _______, _______, _______, _______, _______, _______, _______, _______, _______
//     ),
};

void keyboard_pre_init_user(void) {
  // Set our LED pin as output
  setPinOutput(24);
  // Turn the LED off
  // (Due to technical reasons, high is off and low is on)
  writePinHigh(24);
  initialize_grid();
}


// OLED and rotary encoder
#ifdef OLED_ENABLE
oled_rotation_t oled_init_user(oled_rotation_t rotation) { return OLED_ROTATION_180; }

bool oled_task_user(void) {
    if (is_keyboard_master()) {
        // QMK Logo and version information
        // clang-format off
        static const char PROGMEM qmk_logo[] = {
            0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8a,0x8b,0x8c,0x8d,0x8e,0x8f,0x90,0x91,0x92,0x93,0x94,
            0xa0,0xa1,0xa2,0xa3,0xa4,0xa5,0xa6,0xa7,0xa8,0xa9,0xaa,0xab,0xac,0xad,0xae,0xaf,0xb0,0xb1,0xb2,0xb3,0xb4,
            0xc0,0xc1,0xc2,0xc3,0xc4,0xc5,0xc6,0xc7,0xc8,0xc9,0xca,0xcb,0xcc,0xcd,0xce,0xcf,0xd0,0xd1,0xd2,0xd3,0xd4,0};
        // clang-format on

        oled_write_P(qmk_logo, false);
        oled_write_P(PSTR("Kyria rev1.0\n\n"), false);

        // Host Keyboard Layer Status
        oled_write_P(PSTR("Layer: "), false);
        switch (get_highest_layer(layer_state|default_layer_state)) {
            case _QWERTY:
                oled_write_P(PSTR("Base\n"), false);
                break;
            case _NAV:
                oled_write_P(PSTR("Alt\n"), false);
                break;
            case _SYM:
                oled_write_P(PSTR("Sym\n"), false);
                break;
            case _FUNCTION:
                oled_write_P(PSTR("Function\n"), false);
                initialize_grid();
                break;
            default:
                oled_write_P(PSTR("Undefined\n"), false);
        }

        // Write host Keyboard LED Status to OLEDs
        led_t led_usb_state = host_keyboard_led_state();
        oled_write_P(led_usb_state.num_lock    ? PSTR("NUMLCK ") : PSTR("       "), false);
        oled_write_P(led_usb_state.caps_lock   ? PSTR("CAPS! ") : PSTR("       "), false);
        oled_write_P(led_usb_state.scroll_lock ? PSTR("SCRLCK ") : PSTR("       "), false);
    } else {
        // Game of Life rendering for slave OLED
        render_game_of_life();
    }
    return false;
}
#endif

#ifdef ENCODER_ENABLE
bool encoder_update_user(uint8_t index, bool clockwise) {

    if (index == 0) {
        // Volume control
        if (clockwise) {
            tap_code(KC_VOLU);
        } else {
            tap_code(KC_VOLD);
        }
    } else if (index == 1) {
        // Page up/Page down
        if (clockwise) {
            tap_code(KC_RIGHT);
        } else {
            tap_code(KC_LEFT);
        }
    }
    return false;
}
#endif
