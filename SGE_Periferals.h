#ifndef SGE_PERIFERALS_H
#define SGE_PERIFERALS_H

/* SGE_Periferals.h - part of the SDL2-based Game Engine (SGE) v.20221204
 * ======================================================================
 *
 * The SGE was developed by Joseph21 and is heavily inspired bij the Pixel Game Engine (PGE) by Javidx9
 * (see: https://github.com/OneLoneCoder/olcPixelGameEngine). It's interface is deliberately kept very
 * close to that of the PGE, so that programs can be ported from the one to the other quite easily.
 *
 * License
 * -------
 * This code is completely free to use, change, rewrite or get inspiration from. At the same time, there's
 * no warranty that this code is free of bugs. If you use (any part of) this code, you accept each and any
 * risk or consequence thereof.
 *
 * Although there is no obligation to mention or shout out to the creator, I wouldn't mind if you did :)
 *
 * Have fun with it!
 *
 * Joseph21
 * december 4, 2022
 */

#include <SDL.h>

#include "SGE_Utilities.h"

//                          +--------------------+                           //
// -------------------------+ MODULE DESCRIPTION +-------------------------- //
//                          +--------------------+                           //

//                               +-----------+                               //
// ------------------------------+ CONSTANTS +------------------------------ //
//                               +-----------+                               //

// nr of keys that are discriminated per peripheral
#define NUM_KEYBD_KEYS     232    // other keys are not relevant
#define NUM_MOUSE_BUTTONS    3    // left, middle, right mouse button

// key states
#define KEY_UNKNOWN  0
#define KEY_IDLE     1
#define KEY_DOWN     2
#define KEY_UP       3
#define KEY_REPEAT   4

namespace flc {

    // this struct defines the state a key can have
    struct KeyState {
        bool bIdle;
        bool bPressed;
        bool bReleased;
        bool bHeld;
    };

    // enum Key enumerates all relevant keys, so that they can be used as array indices
    // the SDL keys have mostly identical names, but prefixed by SDL_SCANCODE_
    // (some key names are altered to comply with PGE interface)
    // This enum Keys is used to index into the keyState array.
    enum Key {
        NONE               = SDL_SCANCODE_UNKNOWN,
        A                  = SDL_SCANCODE_A,
        B                  = SDL_SCANCODE_B,
        C                  = SDL_SCANCODE_C,
        D                  = SDL_SCANCODE_D,
        E                  = SDL_SCANCODE_E,
        F                  = SDL_SCANCODE_F,
        G                  = SDL_SCANCODE_G,
        H                  = SDL_SCANCODE_H,
        I                  = SDL_SCANCODE_I,
        J                  = SDL_SCANCODE_J,
        K                  = SDL_SCANCODE_K,
        L                  = SDL_SCANCODE_L,
        M                  = SDL_SCANCODE_M,
        N                  = SDL_SCANCODE_N,
        O                  = SDL_SCANCODE_O,
        P                  = SDL_SCANCODE_P,
        Q                  = SDL_SCANCODE_Q,
        R                  = SDL_SCANCODE_R,
        S                  = SDL_SCANCODE_S,
        T                  = SDL_SCANCODE_T,
        U                  = SDL_SCANCODE_U,
        V                  = SDL_SCANCODE_V,
        W                  = SDL_SCANCODE_W,
        X                  = SDL_SCANCODE_X,
        Y                  = SDL_SCANCODE_Y,
        Z                  = SDL_SCANCODE_Z,
        K1                 = SDL_SCANCODE_1,
        K2                 = SDL_SCANCODE_2,
        K3                 = SDL_SCANCODE_3,
        K4                 = SDL_SCANCODE_4,
        K5                 = SDL_SCANCODE_5,
        K6                 = SDL_SCANCODE_6,
        K7                 = SDL_SCANCODE_7,
        K8                 = SDL_SCANCODE_8,
        K9                 = SDL_SCANCODE_9,
        K0                 = SDL_SCANCODE_0,
        RETURN             = SDL_SCANCODE_RETURN,
        ESCAPE             = SDL_SCANCODE_ESCAPE,
        BACK               = SDL_SCANCODE_BACKSPACE,
        TAB                = SDL_SCANCODE_TAB,
        SPACE              = SDL_SCANCODE_SPACE,
        MINUS              = SDL_SCANCODE_MINUS,
        EQUALS             = SDL_SCANCODE_EQUALS,
        LEFTBRACKET        = SDL_SCANCODE_LEFTBRACKET,
        RIGHTBRACKET       = SDL_SCANCODE_RIGHTBRACKET,
        BACKSLASH          = SDL_SCANCODE_BACKSLASH,
        NONUSHASH          = SDL_SCANCODE_NONUSHASH,
        SEMICOLON          = SDL_SCANCODE_SEMICOLON,
        APOSTROPHE         = SDL_SCANCODE_APOSTROPHE,
        GRAVE              = SDL_SCANCODE_GRAVE,
        COMMA              = SDL_SCANCODE_COMMA,
        PERIOD             = SDL_SCANCODE_PERIOD,
        SLASH              = SDL_SCANCODE_SLASH,
        CAPSLOCK           = SDL_SCANCODE_CAPSLOCK,
        F1                 = SDL_SCANCODE_F1,
        F2                 = SDL_SCANCODE_F2,
        F3                 = SDL_SCANCODE_F3,
        F4                 = SDL_SCANCODE_F4,
        F5                 = SDL_SCANCODE_F5,
        F6                 = SDL_SCANCODE_F6,
        F7                 = SDL_SCANCODE_F7,
        F8                 = SDL_SCANCODE_F8,
        F9                 = SDL_SCANCODE_F9,
        F10                = SDL_SCANCODE_F10,
        F11                = SDL_SCANCODE_F11,
        F12                = SDL_SCANCODE_F12,
        PRINTSCREEN        = SDL_SCANCODE_PRINTSCREEN,
        SCROLL             = SDL_SCANCODE_SCROLLLOCK,
        PAUSE              = SDL_SCANCODE_PAUSE,
        INS                = SDL_SCANCODE_INSERT,
        HOME               = SDL_SCANCODE_HOME,
        PGUP               = SDL_SCANCODE_PAGEUP,
        DEL                = SDL_SCANCODE_DELETE,
        END                = SDL_SCANCODE_END,
        PGDN               = SDL_SCANCODE_PAGEDOWN,
        RIGHT              = SDL_SCANCODE_RIGHT,
        LEFT               = SDL_SCANCODE_LEFT,
        DOWN               = SDL_SCANCODE_DOWN,
        UP                 = SDL_SCANCODE_UP,
        NUMLOCKCLEAR       = SDL_SCANCODE_NUMLOCKCLEAR,
        NP_DIV             = SDL_SCANCODE_KP_DIVIDE,
        NP_MUL             = SDL_SCANCODE_KP_MULTIPLY,
        NP_SUB             = SDL_SCANCODE_KP_MINUS,
        NP_ADD             = SDL_SCANCODE_KP_PLUS,
        ENTER              = SDL_SCANCODE_KP_ENTER,
        NP1                = SDL_SCANCODE_KP_1,
        NP2                = SDL_SCANCODE_KP_2,
        NP3                = SDL_SCANCODE_KP_3,
        NP4                = SDL_SCANCODE_KP_4,
        NP5                = SDL_SCANCODE_KP_5,
        NP6                = SDL_SCANCODE_KP_6,
        NP7                = SDL_SCANCODE_KP_7,
        NP8                = SDL_SCANCODE_KP_8,
        NP9                = SDL_SCANCODE_KP_9,
        NP0                = SDL_SCANCODE_KP_0,
        NP_DECIMAL         = SDL_SCANCODE_KP_PERIOD,
        NONUSBACKSLASH     = SDL_SCANCODE_NONUSBACKSLASH,
        APPLICATION        = SDL_SCANCODE_APPLICATION,
        POWER              = SDL_SCANCODE_POWER,
        KP_EQUALS          = SDL_SCANCODE_KP_EQUALS,
        F13                = SDL_SCANCODE_F13,
        F14                = SDL_SCANCODE_F14,
        F15                = SDL_SCANCODE_F15,
        F16                = SDL_SCANCODE_F16,
        F17                = SDL_SCANCODE_F17,
        F18                = SDL_SCANCODE_F18,
        F19                = SDL_SCANCODE_F19,
        F20                = SDL_SCANCODE_F20,
        F21                = SDL_SCANCODE_F21,
        F22                = SDL_SCANCODE_F22,
        F23                = SDL_SCANCODE_F23,
        F24                = SDL_SCANCODE_F24,
        EXECUTE            = SDL_SCANCODE_EXECUTE,
        HELP               = SDL_SCANCODE_HELP,
        MENU               = SDL_SCANCODE_MENU,
        SELECT             = SDL_SCANCODE_SELECT,
        STOP               = SDL_SCANCODE_STOP,
        AGAIN              = SDL_SCANCODE_AGAIN,
        UNDO               = SDL_SCANCODE_UNDO,
        CUT                = SDL_SCANCODE_CUT,
        COPY               = SDL_SCANCODE_COPY,
        PASTE              = SDL_SCANCODE_PASTE,
        FIND               = SDL_SCANCODE_FIND,
        MUTE               = SDL_SCANCODE_MUTE,
        VOLUMEUP           = SDL_SCANCODE_VOLUMEUP,
        VOLUMEDOWN         = SDL_SCANCODE_VOLUMEDOWN,
        KP_COMMA           = SDL_SCANCODE_KP_COMMA,
        KP_EQUALSAS400     = SDL_SCANCODE_KP_EQUALSAS400,
        INTERNATIONAL1     = SDL_SCANCODE_INTERNATIONAL1,
        INTERNATIONAL2     = SDL_SCANCODE_INTERNATIONAL2,
        INTERNATIONAL3     = SDL_SCANCODE_INTERNATIONAL3,
        INTERNATIONAL4     = SDL_SCANCODE_INTERNATIONAL4,
        INTERNATIONAL5     = SDL_SCANCODE_INTERNATIONAL5,
        INTERNATIONAL6     = SDL_SCANCODE_INTERNATIONAL6,
        INTERNATIONAL7     = SDL_SCANCODE_INTERNATIONAL7,
        INTERNATIONAL8     = SDL_SCANCODE_INTERNATIONAL8,
        INTERNATIONAL9     = SDL_SCANCODE_INTERNATIONAL9,
        LANG1              = SDL_SCANCODE_LANG1,
        LANG2              = SDL_SCANCODE_LANG2,
        LANG3              = SDL_SCANCODE_LANG3,
        LANG4              = SDL_SCANCODE_LANG4,
        LANG5              = SDL_SCANCODE_LANG5,
        LANG6              = SDL_SCANCODE_LANG6,
        LANG7              = SDL_SCANCODE_LANG7,
        LANG8              = SDL_SCANCODE_LANG8,
        LANG9              = SDL_SCANCODE_LANG9,
        ALTERASE           = SDL_SCANCODE_ALTERASE,
        SYSREQ             = SDL_SCANCODE_SYSREQ,
        CANCEL             = SDL_SCANCODE_CANCEL,
        CLEAR              = SDL_SCANCODE_CLEAR,
        PRIOR              = SDL_SCANCODE_PRIOR,
        RETURN2            = SDL_SCANCODE_RETURN2,
        SEPARATOR          = SDL_SCANCODE_SEPARATOR,
        OUT                = SDL_SCANCODE_OUT,
        OPER               = SDL_SCANCODE_OPER,
        CLEARAGAIN         = SDL_SCANCODE_CLEARAGAIN,
        CRSEL              = SDL_SCANCODE_CRSEL,
        EXSEL              = SDL_SCANCODE_EXSEL,
        KP_00              = SDL_SCANCODE_KP_00,
        KP_000             = SDL_SCANCODE_KP_000,
        THOUSANDSSEPARATOR = SDL_SCANCODE_THOUSANDSSEPARATOR,
        DECIMALSEPARATOR   = SDL_SCANCODE_DECIMALSEPARATOR,
        CURRENCYUNIT       = SDL_SCANCODE_CURRENCYUNIT,
        CURRENCYSUBUNIT    = SDL_SCANCODE_CURRENCYSUBUNIT,
        KP_LEFTPAREN       = SDL_SCANCODE_KP_LEFTPAREN,
        KP_RIGHTPAREN      = SDL_SCANCODE_KP_RIGHTPAREN,
        KP_LEFTBRACE       = SDL_SCANCODE_KP_LEFTBRACE,
        KP_RIGHTBRACE      = SDL_SCANCODE_KP_RIGHTBRACE,
        KP_TAB             = SDL_SCANCODE_KP_TAB,
        KP_BACKSPACE       = SDL_SCANCODE_KP_BACKSPACE,
        KP_A               = SDL_SCANCODE_KP_A,
        KP_B               = SDL_SCANCODE_KP_B,
        KP_C               = SDL_SCANCODE_KP_C,
        KP_D               = SDL_SCANCODE_KP_D,
        KP_E               = SDL_SCANCODE_KP_E,
        KP_F               = SDL_SCANCODE_KP_F,
        KP_XOR             = SDL_SCANCODE_KP_XOR,
        KP_POWER           = SDL_SCANCODE_KP_POWER,
        KP_PERCENT         = SDL_SCANCODE_KP_PERCENT,
        KP_LESS            = SDL_SCANCODE_KP_LESS,
        KP_GREATER         = SDL_SCANCODE_KP_GREATER,
        KP_AMPERSAND       = SDL_SCANCODE_KP_AMPERSAND,
        KP_DBLAMPERSAND    = SDL_SCANCODE_KP_DBLAMPERSAND,
        KP_VERTICALBAR     = SDL_SCANCODE_KP_VERTICALBAR,
        KP_COLON           = SDL_SCANCODE_KP_COLON,
        KP_HASH            = SDL_SCANCODE_KP_HASH,
        KP_SPACE           = SDL_SCANCODE_KP_SPACE,
        KP_AT              = SDL_SCANCODE_KP_AT,
        KP_EXCLAM          = SDL_SCANCODE_KP_EXCLAM,
        KP_MEMSTORE        = SDL_SCANCODE_KP_MEMSTORE,
        KP_MEMRECALL       = SDL_SCANCODE_KP_MEMRECALL,
        KP_MEMCLEAR        = SDL_SCANCODE_KP_MEMCLEAR,
        KP_MEMADD          = SDL_SCANCODE_KP_MEMADD,
        KP_MEMSUBTRACT     = SDL_SCANCODE_KP_MEMSUBTRACT,
        KP_MEMMULTIPLY     = SDL_SCANCODE_KP_MEMMULTIPLY,
        KP_MEMDIVIDE       = SDL_SCANCODE_KP_MEMDIVIDE,
        KP_PLUSMINUS       = SDL_SCANCODE_KP_PLUSMINUS,
        KP_CLEAR           = SDL_SCANCODE_KP_CLEAR,
        KP_CLEARENTRY      = SDL_SCANCODE_KP_CLEARENTRY,
        KP_BINARY          = SDL_SCANCODE_KP_BINARY,
        KP_OCTAL           = SDL_SCANCODE_KP_OCTAL,
        KP_DECIMAL         = SDL_SCANCODE_KP_DECIMAL,
        KP_HEXADECIMAL     = SDL_SCANCODE_KP_HEXADECIMAL,
        CTRL               = SDL_SCANCODE_LCTRL,            // used to be LCTRL
        SHIFT              = SDL_SCANCODE_LSHIFT,           // used to be LSHIFT
        LALT               = SDL_SCANCODE_LALT,
        LGUI               = SDL_SCANCODE_LGUI,
        RCTRL              = SDL_SCANCODE_RCTRL,
        RSHIFT             = SDL_SCANCODE_RSHIFT,
        RALT               = SDL_SCANCODE_RALT,
        RGUI               = SDL_SCANCODE_RGUI,
        MODE               = SDL_SCANCODE_MODE,
        AUDIONEXT          = SDL_SCANCODE_AUDIONEXT,
        AUDIOPREV          = SDL_SCANCODE_AUDIOPREV,
        AUDIOSTOP          = SDL_SCANCODE_AUDIOSTOP,
        AUDIOPLAY          = SDL_SCANCODE_AUDIOPLAY,
        AUDIOMUTE          = SDL_SCANCODE_AUDIOMUTE,
        MEDIASELECT        = SDL_SCANCODE_MEDIASELECT,
        WWW                = SDL_SCANCODE_WWW,
        MAIL               = SDL_SCANCODE_MAIL,
        CALCULATOR         = SDL_SCANCODE_CALCULATOR,
        COMPUTER           = SDL_SCANCODE_COMPUTER,
        AC_SEARCH          = SDL_SCANCODE_AC_SEARCH,
        AC_HOME            = SDL_SCANCODE_AC_HOME,
        AC_BACK            = SDL_SCANCODE_AC_BACK,
        AC_FORWARD         = SDL_SCANCODE_AC_FORWARD,
        AC_STOP            = SDL_SCANCODE_AC_STOP,
        AC_REFRESH         = SDL_SCANCODE_AC_REFRESH,
        AC_BOOKMARKS       = SDL_SCANCODE_AC_BOOKMARKS,
        BRIGHTNESSDOWN     = SDL_SCANCODE_BRIGHTNESSDOWN,
        BRIGHTNESSUP       = SDL_SCANCODE_BRIGHTNESSUP,
        DISPLAYSWITCH      = SDL_SCANCODE_DISPLAYSWITCH,
        KBDILLUMTOGGLE     = SDL_SCANCODE_KBDILLUMTOGGLE,
        KBDILLUMDOWN       = SDL_SCANCODE_KBDILLUMDOWN,
        KBDILLUMUP         = SDL_SCANCODE_KBDILLUMUP,
        EJECT              = SDL_SCANCODE_EJECT,
        SLEEP              = SDL_SCANCODE_SLEEP
    };

} // end namespace flc

//                                                                           //
// ------------------------------------------------------------------------- //
//                                                                           //

#endif // SGE_PERIFERALS_H
