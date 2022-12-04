#ifndef SGE_UTILITIES_H
#define SGE_UTILITIES_H

/* SGE_Utilities.h - part of the SDL2-based Game Engine (SGE) v.20221204
 * =====================================================================
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

//                          +--------------------+                           //
// -------------------------+ MODULE DESCRIPTION +-------------------------- //
//                          +--------------------+                           //

#include <fstream>

#include <SDL.h>

#include "vector_types.h"
#include   "my_utility.h"

//                               +-----------+                               //
// ------------------------------+ CONSTANTS +------------------------------ //
//                               +-----------+                               //

#define DEBUG_MODE        false
#define DEBUG_FILE_NAME   "debug_out.txt"

//                           +------------------+                            //
// --------------------------+ GLOBAL VARIABLES +--------------------------- //
//                           +------------------+                            //

extern std::ofstream debugFile;

// a hacky way to make these engine variables available
// for the Sprite and Decal classes. Is there a more elegant solution?
extern SDL_PixelFormat *glbPixelFormatPtr;
extern SDL_Renderer    *glbRendererPtr;

//                              +------------+                               //
// -----------------------------+ PROTOTYPES +------------------------------ //
//                              +------------+                               //

namespace flc {

    // miscellaneous functions
    void InitSDL_Rect(  SDL_Rect  &rec, int x, int y, int w, int h );
    void InitSDL_Point( SDL_Point &pnt, int x, int y );

    // the v2d_generic<> templated types are from vector_types.h
    // I want these typedefs to be available inside namespace flc...
    typedef v2d_generic<int      > vi2d;
    typedef v2d_generic<float    > vf2d;
    typedef v2d_generic<double   > vd2d;
    typedef v2d_generic<long long> vllong2d;

} // end namespace flc

//                                                                           //
// ------------------------------------------------------------------------- //
//                                                                           //

#endif // SGE_UTILITIES_H
