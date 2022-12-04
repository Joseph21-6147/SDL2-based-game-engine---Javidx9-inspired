/* SGE_Periferals.cpp - part of the SDL2-based Game Engine (SGE) v.20221204
 * ========================================================================
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

#include "SGE_Core.h"

// this idle state is needed in case the window doesn't have keyboard or mouse focus
flc::KeyState IdleState = { true, false, false, false };

//                               +----------+                                //
// ------------------------------+ METHODS  +------------------------------- //
//                               +----------+                                //

// set internal keyboard state array to initial
void flc::SDL_GameEngine::InitKeyboardState() {
    for (int i = 0; i < NUM_KEYBD_KEYS; i++) {
        sKeybdStates[i] = IdleState;
    }
}

// set internal mouse state array to initial
void flc::SDL_GameEngine::InitMouseState() {
    for (int i = 0; i < NUM_MOUSE_BUTTONS; i++) {
        sMouseStates[i] = IdleState;
    }
}

// set the button state kState to a particular pattern depending on the value of kVal
void flc::SDL_GameEngine::SetKeyState( KeyState &kState, int kVal ) {
    switch (kVal) {
        case KEY_IDLE:   kState = { true , false, false, false }; break;
        case KEY_DOWN:   kState = { false, true , false, false }; break;
        case KEY_UP:     kState = { false, false, true , false }; break;
        case KEY_REPEAT: kState = { false, false, false, true  }; break;
    }
}

// return if current window has mouse or keyboard focus
bool flc::SDL_GameEngine::IsFocused()      const { return                            IsKeybdFocused(); }
bool flc::SDL_GameEngine::IsKeybdFocused() const { return vWindows[nActiveWindowIx]->IsKeybdFocused(); }
bool flc::SDL_GameEngine::IsMouseFocused() const { return vWindows[nActiveWindowIx]->IsMouseFocused(); }

// keystate getters for the keyboard and mouse
flc::KeyState flc::SDL_GameEngine::GetKey(   Key eKeyIndex ) { return (IsKeybdFocused() ? sKeybdStates[ eKeyIndex ] : IdleState); }
flc::KeyState flc::SDL_GameEngine::MouseKey( int nKeyIndex ) { return (IsMouseFocused() ? sMouseStates[ nKeyIndex ] : IdleState); }

// Getter methods for mouse logical coordinates
int flc::SDL_GameEngine::MouseX() { return vWindows[nActiveWindowIx]->GetMouseX(); }
int flc::SDL_GameEngine::MouseY() { return vWindows[nActiveWindowIx]->GetMouseY(); }

const vi2d& flc::SDL_GameEngine::GetMousePos() {
    vMouse_logical.x = vWindows[nActiveWindowIx]->GetMouseX();
    vMouse_logical.y = vWindows[nActiveWindowIx]->GetMouseY();
    return vMouse_logical;
}

// Getter method for mouse physical coordinates
const vi2d& flc::SDL_GameEngine::GetWindowMouse() {
    vMouse_physical.x = vWindows[nActiveWindowIx]->GetMouseX_physical();
    vMouse_physical.y = vWindows[nActiveWindowIx]->GetMouseY_physical();
    return vMouse_physical;
}

// Returns a positive or negative integer upon mouse wheel rotation, where the value is a measure for the
// rotation speed of the mouse wheel. Returns 0 if no rotation.
int flc::SDL_GameEngine::GetMouseWheel() { return (IsMouseFocused() ? nMouseWheel : 0); }

// cursor on/off setter and query
void flc::SDL_GameEngine::SetCursorOn()  { if (!IsCursorOn()) SDL_ShowCursor( SDL_ENABLE  ); }
void flc::SDL_GameEngine::SetCursorOff() { if ( IsCursorOn()) SDL_ShowCursor( SDL_DISABLE ); }

bool flc::SDL_GameEngine::IsCursorOn() { return (SDL_ShowCursor( SDL_QUERY ) == SDL_ENABLE); }

// used each frame to update the keyboard state array with new value
void flc::SDL_GameEngine::GetUpdateKeyboardState() {
    // get state of all keyboard keys in 1 call
    const uint8_t *keys = SDL_GetKeyboardState( nullptr );
    // populate the keyboard state array
    for(int i = 0; i < NUM_KEYBD_KEYS; ++i) {
        if(keys[i] == 1) {    // this means key is pressed
            SetKeyState( sKeybdStates[i], (sKeybdStates[i].bIdle || sKeybdStates[i].bReleased) ? KEY_DOWN : KEY_REPEAT );
        } else {
            SetKeyState( sKeybdStates[i], (sKeybdStates[i].bHeld || sKeybdStates[i].bPressed ) ? KEY_UP   : KEY_IDLE   );
        }
    }
}

// used each frame to update the mouse position and mouse key state array with new value
void flc::SDL_GameEngine::GetUpdateMouseState() {
    // get mouse position
    uint32_t mse_buttons = SDL_GetMouseState( &nMouseX_physical, &nMouseY_physical );
    nMouseX_logical = nMouseX_physical / vWindows[nActiveWindowIx]->GetPixelWidth();
    nMouseY_logical = nMouseY_physical / vWindows[nActiveWindowIx]->GetPixelHeight();
    vMouse_physical = vi2d( nMouseX_physical, nMouseY_physical );
    vMouse_logical  = vi2d( nMouseX_logical,  nMouseY_logical  );
    // populate the mouse button state array
    uint32_t nMouseMasks[3] = { SDL_BUTTON_LMASK, SDL_BUTTON_RMASK, SDL_BUTTON_MMASK };
    for (int i = 0; i < 3; i++) {
        if (mse_buttons & nMouseMasks[i]) {
            SetKeyState( sMouseStates[i], (sMouseStates[i].bIdle || sMouseStates[i].bReleased) ? KEY_DOWN : KEY_REPEAT );
        } else {
            SetKeyState( sMouseStates[i], (sMouseStates[i].bHeld || sMouseStates[i].bPressed ) ? KEY_UP   : KEY_IDLE   );
        }
    }
}

//                                                                           //
// ------------------------------------------------------------------------- //
//                                                                           //
