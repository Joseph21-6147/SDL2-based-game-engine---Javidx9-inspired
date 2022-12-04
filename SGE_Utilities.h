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

#include "SGE_vector_types.h"

//                               +-----------+                               //
// ------------------------------+ CONSTANTS +------------------------------ //
//                               +-----------+                               //

#define DEBUG_MODE        false
#define DEBUG_FILE_NAME   "debug_out.txt"

#define PI  3.1415926535f

#define ALIGN_STRLEN     14  // default length for string alignment

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

// translate val into it's corresponding hexadecimal representation.
// e.g. 0 translates to '0' and 15 translates to 'F'.
// val must be in [0, 15]
char        to_hex_position( uint8_t val );
// translates val into its corresponding hexadecimal representation.
// since val is 32 bits, it translates into a string of length 8.
std::string to_hex( uint32_t val );

// functions produce an aligned string of length positions containing nValue
std::string right_align( int nValue, int positions );
std::string  left_align( int nValue, int positions );
std::string right_align( std::string &s, int positions );
std::string  left_align( std::string &s, int positions );
// fValue resp. s contains a floating point number which may (or may not) have a
// dot in it. Align the dot of this number on dotPosition, creating a string of
// totalPositions
std::string dot_align( float fValue  , int dotPosition, int totalPositions );
std::string dot_align( std::string &s, int dotPosition, int totalPositions );

// return the clamped value of a in [ a_start, a_end ]
int   Clamp( int   a, int   a_start, int   a_end );
float Clamp( float a, float a_start, float a_end );


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
