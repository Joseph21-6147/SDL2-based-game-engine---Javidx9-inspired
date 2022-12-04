/* SGE_Utilities.cpp - part of the SDL2-based Game Engine (SGE) v.20221204
 * =======================================================================
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

#include "SGE_Utilities.h"

#include <iostream>

//                           +------------------+                            //
// --------------------------+ GLOBAL VARIABLES +--------------------------- //
//                           +------------------+                            //

std::ofstream debugFile;

SDL_PixelFormat *glbPixelFormatPtr = nullptr;
SDL_Renderer    *glbRendererPtr    = nullptr;

//                              +------------+                               //
// -----------------------------+ FUNCTIONS  +------------------------------ //
//                              +------------+                               //

// fills an SDL_Rect structure using the input parameters
void flc::InitSDL_Rect( SDL_Rect &rect, int x, int y, int w, int h ) {
    rect = { x, y, w, h };
}

// fills an SDL_Rect structure using the input parameters
void flc::InitSDL_Point( SDL_Point &point, int x, int y ) {
    point = { x, y };
}

//                                                                           //
// ------------------------------------------------------------------------- //
//                                                                           //
