#ifndef FONTDATASTRINGS_H
#define FONTDATASTRINGS_H

/* SGE_FontData.h - part of the SDL2-based Game Engine (SGE) v.20221204
 * ====================================================================
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

#include <iostream>

//                          +--------------------+                           //
// -------------------------+ MODULE DESCRIPTION +-------------------------- //
//                          +--------------------+                           //


//                               +-----------+                               //
// ------------------------------+ CONSTANTS +------------------------------ //
//                               +-----------+                               //

// nr of fonts implemented in data strings
#define NR_OF_SPRITE_FONTS  6

//                              +------------+                               //
// -----------------------------+ PROTOTYPES +------------------------------ //
//                              +------------+                               //

// This function loads all the info for the specified (nFontIndex) sprite font. It calls one of the
// InitDataStringN() functions to populate the data string.
void InitFontSprite(
                int nFontIndex,                   // Input:  the index number for each font
                std::string &sFontFileName,       // Output: the name of the file it was loaded from (for testing)
                int &nCharsHor, int &nCharsVer,   //         characters in the sprite - horizontally and vertically
                int &nTileSizeX, int &nTileSizeY, //         character tile sizes - horizontally and vertically
                int &nAsciiOffset,                //         not all fonts have the 'A' nicely at tile address 65
                std::string &sDataString          //         string to hold the data that normally a sprite file would
            );

// these functions load the data from a code string. The original data (sprite file) was transformed into such a
// code string for this purpose. After completion, the data is put in sDataString.
void InitDataString0( std::string &sDataString );    // font index: 0, file name:               ascii 128x128.tpbg.png
void InitDataString1( std::string &sDataString );    // font index: 1, file name:                   nesfont 128x48.png
void InitDataString2( std::string &sDataString );    // font index: 2, file name: font - unknown 512x512.tpbg.mono.png
void InitDataString3( std::string &sDataString );    // font index: 3, file name:     font cells 512x512.tpbg.mono.png
void InitDataString4( std::string &sDataString );    // font index: 4, file name:      bitmapfont openGL.tpbg.mono.png
void InitDataString5( std::string &sDataString );    // font index: 5, file name:         simple font 610x650.tpbg.png

//                                                                           //
// ------------------------------------------------------------------------- //
//                                                                           //

#endif  // FONTDATASTRINGS_H

